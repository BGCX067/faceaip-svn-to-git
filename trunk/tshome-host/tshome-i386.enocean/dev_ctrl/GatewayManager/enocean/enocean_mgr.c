/*
	2012-6-28 17:07:46
	  改为采用消息驱动模式
*/

#include "enocean_mgr.h"
#include "esp.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#ifdef DEV_ENOCEAN_SUPPORT

/* thread proc */
void * thread_proc_func(void *);

/* send a radio telegram */
static RETURN_TYPE enocean_radio_send(ENOCEAN_SEND_PARAM_T *param_t);
/* send a RPS radio telegram */
static RETURN_TYPE enocean_radio_rps_send(ENOCEAN_SEND_PARAM_T* param_t);
/* telegram data callback */
void enocean_radio_recv(int nId, unsigned char *pBuf, int nLen);
/* parse a radio telegram */
static RETURN_TYPE enocean_radio_parse(PACKET_ENOCEAN_SERIAL_T *packet);
/* parse a 1BS radio telegram */
static void enocean_radio_1BS_parse(ENOCEAN_DEVICE_T* device, uint8* pData_1BS);
/* parse a 4BS radio telegram */
static void enocean_radio_4BS_parse(ENOCEAN_DEVICE_T* device, uint8* pData_4BS);


/* uart interface */
static UART_INTERFACE_T g_uart_iface = {0};

/* uart buffer array */
volatile UART_BUFFER_T uart_buffer_array[MAX_BUFFER_COUNT];

sem_t __full_sem;   // 同步信号量， 当没产品时阻止消费者消费
//pthread_mutex_t __mutex; // 互斥信号量， 一次只有一个线程访问缓冲
int in = 0;   // 生产者放置产品的位置
int out = 0; // 消费者取产品的位置

/* uart thread id */
static pthread_t thread_id;
/* thread running flag */
static boolean thread_run = 0;

/* enocean devices */
#define MAX_DEVICES 100
static ENOCEAN_DEVICE_T devices[MAX_DEVICES];
static int devices_num = 0;

/*
Under teach-in mode (teach_in_mode=1), all teach-in telegrams are processed and 
those who send teach-in telegrams are recognized with device id.
*/
static boolean teach_in_mode = 0;

/*------------------ intialize handlers ------------------*/
/* init uart parameters */
int enocean_uart_init(const char *enocean_interface)
{
    const char *pDev = DEFAULT_ENOCEAN_DEVICE_INTERFACE;

    if (enocean_interface!=NULL
        && *enocean_interface!='\0')
    {
        pDev = enocean_interface;
    }

    // todo: read from database
    g_uart_iface.uart_info.id = 0x100; //enocean id
    strncpy(g_uart_iface.uart_info.com_name, pDev, COM_NAME_LENGTH);
    g_uart_iface.uart_info.baud = 57600;
    g_uart_iface.uart_info.data_bits = 8;
    g_uart_iface.uart_info.parity = 0;
    g_uart_iface.uart_info.stop_bits = 1;

    /* init buffer array */
    memset((void*)uart_buffer_array, 0x00, sizeof(UART_BUFFER_T) * MAX_BUFFER_COUNT);

    /* init device list */
    memset((void*)devices, 0x00, sizeof(ENOCEAN_DEVICE_T) * MAX_DEVICES);
    devices[0].device_id = 0x010189E5; // ....id 10189e5
    devices[0].u8Choice = RORG_RPS;
    devices[0].u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    devices[0].u8AppType = TYPE_RPS_02_01;

    devices[1].device_id = 0x0080919F;
    devices[1].u8Choice = RORG_RPS;
    devices[1].u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    devices[1].u8AppType = TYPE_RPS_02_01;

    devices[2].device_id = 0x00036A28; // 人感
    devices[2].u8Choice = RORG_4BS;
    devices[2].u8Func = FUNC_4BS_LIGHT_TEMP_OCCUPANCY;
    devices[2].u8AppType = TYPE_4BS_08_01;
    devices[2].trans_or_actuator = 1;

    devices[3].device_id = 0x01019F9D; // new actuator
    devices[3].u8Choice = RORG_RPS;
    devices[3].u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    devices[3].u8AppType = TYPE_RPS_02_01;

    devices[4].device_id = 0x0082dd70; // new window sensor
    devices[4].u8Choice = RORG_1BS;
    devices[4].u8Func = FUNC_1BS_CONTACTS_SWITCHES;
    devices[4].u8AppType = TYPE_1BS_00_01;
    devices[4].trans_or_actuator = 1;

    devices[5].device_id = 0x0010FA72; // door sensor
    devices[5].u8Choice = RORG_RPS;
    devices[5].u8Func = 0x10;
    devices[5].u8AppType = 0x00;
    devices[5].trans_or_actuator = 1;

	// 2012-7-2 10:45:29 窗磁 add
    devices[6].device_id = 0x0082a74c; // new window sensor
    devices[6].u8Choice = RORG_1BS;
    devices[6].u8Func = FUNC_1BS_CONTACTS_SWITCHES;
    devices[6].u8AppType = TYPE_1BS_00_01;
    devices[6].trans_or_actuator = 1;


    devices[7].device_id = 0x0101889D;
    devices[7].u8Choice = RORG_RPS;
    devices[7].u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    devices[7].u8AppType = TYPE_RPS_02_01;

    devices_num = 8;

    /* normal mode */
    teach_in_mode = 0;

    // 初始化同步信号量
    //int ini1 = sem_init(&__empty_sem, 0, M);
    int ini2 = sem_init(&__full_sem, 0, 0);
    if(ini2 != 0)
    {
        printf("sem init failed \n");
        exit(1);
    }
    //初始化互斥信号量
    //int ini3 = pthread_mutex_init(&__mutex, NULL);
    //if(ini3 != 0)
    //{
    //    printf("mutex init failed \n");
    //    exit(1);
    //}
    return 0;
}

/* register a device, to listen on its status */
void enocean_register_device(ENOCEAN_DEVICE_T* device)
{
    /* init device list */
    if(devices_num == MAX_DEVICES)
    {
        fprintf(stderr, "the device list is full, so cannot register more.\n");
        return;
    }
    /* check duplicates */
    int i;
    for(i = 0; i < devices_num; i++)
    {
        if(devices[i].device_id == device->device_id)
        {
            /* already in list, return */
            return;
        }
    }

    /* copy device */
    devices[devices_num].device_id = device->device_id;
    devices[devices_num].u8Choice = device->u8Choice;
    devices[devices_num].u8Func = device->u8Func;
    devices[devices_num].u8AppType = device->u8AppType;
    devices[devices_num].on_or_off = device->on_or_off;

    devices_num ++;
}


/* Get total number of the known enocean devices */
int enocean_get_device_num()
{
    return devices_num;
}


/* Get the specified known device with index */
void enocean_get_device(int index, ENOCEAN_DEVICE_T* device)
{
    if(index >= MAX_DEVICES || index < 0)
    {
        return;
    }

    memcpy((void *)device, (void *)&devices[index], sizeof(ENOCEAN_DEVICE_T));
}



/* Switch into teach-in mode or normal mode */
void enocean_start_teach_in()
{
    teach_in_mode = 1;
}
void enocean_stop_teach_in()
{
    teach_in_mode = 0;
}


/* set enocean callbacks */
void enocean_uart_callback(enocean_callback_status_t callback)
{
    g_uart_iface.callback = callback;
}

/* open uart interface */
RETURN_TYPE enocean_uart_open()
{
    /* open enocean interface */
    g_uart_iface.uart_io = ezserial_open(
                               g_uart_iface.uart_info.com_name,
                               enocean_radio_recv,
                               g_uart_iface.uart_info.id,
                               g_uart_iface.uart_info.baud,
                               g_uart_iface.uart_info.data_bits,
                               g_uart_iface.uart_info.parity,
                               g_uart_iface.uart_info.stop_bits);
    if(NULL == g_uart_iface.uart_io)
    {
        fprintf(stderr, "open enocean uart interface '%s' failed. %s\n",
                g_uart_iface.uart_info.com_name, ezserial_geterrormsg());

        return UART_NOT_OPENED;
    }
    g_uart_iface.uart_io->id = g_uart_iface.uart_info.id;
    //__fline printf("open enocean uart interface succeed.\n");

    /* start data receive thread */
    thread_run = 1;
    pthread_create(&thread_id, NULL, thread_proc_func, NULL);

    return 0;
}

/* close uart interface */
void enocean_uart_close()
{
    /* close thread */
    if(thread_run == 1)
    {
        thread_run = 0;
        //pthread_join(thread_id, NULL);
    }
	
    sem_post(&__full_sem);

    /* close enocean interface */
    if(g_uart_iface.uart_io)
    {
        ezserial_close(g_uart_iface.uart_io);
        g_uart_iface.uart_io = NULL;
    }
}

    /* 
    	0 - ok, live, others not live
		-1 - serial not open 
     */
int enocean_uart_status()
{
	if (NULL == g_uart_iface.uart_io)
	{
		return -1;
	}

	return 0;		
}


/* thread proc */
void * thread_proc_func(void * arg)
{
    //__fline printf("uart thread starts.\n");

    while(thread_run)
    {
        /* check buffer array */
        //while(uart_buffer_array[pop].ready == 0)
        //{
        if(thread_run == 0)
        {
            /* exit thread */
            goto EXIT_THREAD;
        }
        //sleep(1);
        //}

        sem_wait(&__full_sem);
        //pthread_mutex_lock(&__mutex);
        //__fline printf("get a sem.\n");

        out = out % MAX_BUFFER_COUNT;
        //printf("out %d. like: \n", out);
        //pop = out;


        /* allocate message structure */
        static PACKET_ENOCEAN_SERIAL_T packet = {0};
        if(NULL == packet.u8DataBuffer)
        {
            packet.u8DataBuffer = calloc(1, MAX_PACKET_DATA_LEN);
        }
        if(NULL == packet.u8DataBuffer)
        {
            fprintf(stderr, "allocate momory failed for u8DataBuffer.");
            return NULL;
        }

        /* parse buffer */
        if(OK == uart_getPacket((uint8*)uart_buffer_array[out].data, uart_buffer_array[out].len, &packet, MAX_PACKET_DATA_LEN))
        {
            /* when a whole packet received, let upper layer know */
            //__fline printf("A whole packet received.\n");
            enocean_radio_parse(&packet);


            //g_uart_iface.callback(param);


            /* free buffer */
            free(packet.u8DataBuffer);
            packet.u8DataBuffer = NULL;
        }

        /* clear data */
        memset((void*)&uart_buffer_array[out], 0x00, sizeof(UART_BUFFER_T));
        //pop = (pop + 1) % MAX_BUFFER_COUNT;
        ++out;

        //pthread_mutex_unlock(&__mutex);
        //sem_post(&__empty_sem);
    }

EXIT_THREAD:
    __fline printf("uart thread ends.\n");

    return NULL;
}



/*------------------ telegram handlers ------------------*/
RETURN_TYPE enocean_device_onoff(unsigned int u32DeviceID, int rocker, boolean on)
{
    /* init params */
    RETURN_TYPE ret;
    ENOCEAN_SEND_PARAM_T param_t = {0};
    param_t.u32DeviceID = u32DeviceID;
    param_t.u32SenderID = 0x00000000; // use gateway id
    param_t.u8Choice = RORG_RPS;
    //param_t.u8Choice = RORG_SIGNAL;
    param_t.u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    param_t.u8AppType = TYPE_RPS_02_01;
    param_t.u8Rocker = rocker==1 ? 1 : 2;
    param_t.u8State = on>0 ? 1 : 0;
    param_t.bPressed = 1;
    //__fline;
    //printf("u32DeviceID:%#x, rocker:%d, on:%d\n", u32DeviceID, rocker, on);

    /* send an enocean telegram */
    ret = enocean_radio_send(&param_t);
    //printf("enocean_radio_send return:%d", ret);

    return ret;
}
/*------------------ telegram handlers ------------------*/
RETURN_TYPE enocean_device_find(uint32 u32DeviceID, int rocker, boolean on)
{
    /* init params */
    RETURN_TYPE ret;
    ENOCEAN_SEND_PARAM_T param_t = {0};
    param_t.u32DeviceID = 0xffffffff;
    param_t.u32SenderID = 0x00000000; // use gateway id
    param_t.u8Choice = RORG_SIGNAL;
    param_t.u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    param_t.u8AppType = TYPE_RPS_02_01;
    param_t.u8Rocker = rocker==1 ? 1 : 2;
    param_t.u8State = on>0 ? 1 : 0;
    param_t.bPressed = 1;

    /* send an enocean telegram */
    ret = enocean_radio_send(&param_t);
    //printf("enocean_radio_send return:%d", ret);

    return ret;
}

RETURN_TYPE enocean_device_dim_begin(uint32 u32DeviceID, int rocker, boolean up)
{
    /* init params */
    RETURN_TYPE ret;
    ENOCEAN_SEND_PARAM_T param_t = {0};
    param_t.u32DeviceID = 0; // broadcast
    param_t.u32SenderID = u32DeviceID;
    param_t.u8Choice = RORG_RPS;
    param_t.u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    param_t.u8AppType = TYPE_RPS_02_01;
    param_t.u8Rocker = rocker==1 ? 1 : 2;
    param_t.u8State = up>0 ? 1 : 0;
    param_t.bPressed = 1;

    /* send an enocean telegram */
    ret = enocean_radio_send(&param_t);

    return ret;
}

RETURN_TYPE enocean_device_dim_end(uint32 u32DeviceID, int rocker, boolean up)
{
    /* init params */
    RETURN_TYPE ret;
    ENOCEAN_SEND_PARAM_T param_t = {0};
    param_t.u32DeviceID = 0; // broadcast
    param_t.u32SenderID = u32DeviceID;
    param_t.u8Choice = RORG_RPS;
    param_t.u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    param_t.u8AppType = TYPE_RPS_02_02;
    param_t.u8Rocker = rocker==1 ? 1 : 2;
    param_t.u8State = up>0 ? 1 : 0;
    param_t.bPressed = 0;

    /* send an enocean telegram */
    ret = enocean_radio_send(&param_t);

    return ret;
}

/*
 
RADIO telegram DATA structure:
 
             1             1                 4              1
        +---------+------------------+---------------+-----------+
        | choice  |    user data     |   sender ID   |   status  |  
        +---------+------------------+---------------+-----------+
*/

static RETURN_TYPE enocean_radio_send(ENOCEAN_SEND_PARAM_T *param_t)
{
    RETURN_TYPE ret = OK;
    //printf("Enter enocean_radio_send.\n");

    /* check interface */
    if(g_uart_iface.uart_io == NULL)
    {
        //fprintf(stderr, "uart interface hasn't been opened yet.\n");
        fprintf(stderr, "uart interface(%s) hasn't been opened yet.\n", g_uart_iface.uart_info.com_name);

        return UART_NOT_OPENED;
    }

    /* RPS for 2-rocker switcher */
    if( (param_t->u8Choice == RORG_RPS && param_t->u8Func == FUNC_RPS_ROCKER_SWITCH_2) ||
        (param_t->u8Choice == RORG_ADT && param_t->u8Func == FUNC_RPS_ROCKER_SWITCH_2) ||
        (param_t->u8Choice == RORG_SIGNAL && param_t->u8Func == FUNC_RPS_ROCKER_SWITCH_2) )
    {
        /* send the radio telegram */
        if(OK != (ret = enocean_radio_rps_send(param_t)))
        {
            return ret;
        }
        //printf("send radio OK.\n");

        return ret;

    }


    return TYPE_NOT_SUPPORTED;

}

static RETURN_TYPE enocean_radio_rps_send(ENOCEAN_SEND_PARAM_T* param_t)
{
    RETURN_TYPE ret = OK;

    /* make up a RPS radio telegram */
    /* header */
    PACKET_ENOCEAN_SERIAL_T packet;
    packet.u16DataLength = htons(1+1+4+1);
    packet.u8OptionLength = 7;
    packet.u8Type = PACKET_TYPE_RADIO;
    packet.u8DataBuffer = calloc(1, (size_t)(ntohs(packet.u16DataLength) + packet.u8OptionLength));
    //printf("\npacket.u16DataLength:%d, packet.u8OptionLength:%d\n", ntohs(packet.u16DataLength), packet.u8OptionLength);

    /* check buffer */
    if(NULL == packet.u8DataBuffer)
    {
        return OUT_OF_MEMORY;
    }

    /* choice/RORG */
    packet.u8DataBuffer[0] = param_t->u8Choice; //RORG_RPS

    /* user data */
    if( (param_t->u8Rocker != 1 && param_t->u8Rocker != 2) ||
        (param_t->u8State != 0 && param_t->u8State != 1) )
    {
        free(packet.u8DataBuffer);
        return OUT_OF_RANGE;
    }
    if(param_t->u8AppType == TYPE_RPS_02_01)
    {
        if(param_t->u8Rocker == 1 && param_t->u8State == 1)
        {
            packet.u8DataBuffer[1] = 0x0 | (param_t->bPressed ? (0x1<<4) : 0);
        }
        else if(param_t->u8Rocker == 1 && param_t->u8State == 0)
        {
            packet.u8DataBuffer[1] = (0x1<<5) | (param_t->bPressed ? (0x1<<4) : 0);
        }
        else if(param_t->u8Rocker == 2 && param_t->u8State == 1)
        {
            packet.u8DataBuffer[1] = (0x2<<5) | (param_t->bPressed ? (0x1<<4) : 0);
        }
        else if(param_t->u8Rocker == 2 && param_t->u8State == 0)
        {
            packet.u8DataBuffer[1] = (0x3<<5) | (param_t->bPressed ? (0x1<<4) : 0);
        }
    }
    else
    {
        packet.u8DataBuffer[1] = 0;
    }

    /* sender ID, 4 bytes */
    *(uint32*)(&packet.u8DataBuffer[2]) = htonl(param_t->u32SenderID);
    /* status */
    if(param_t->u8AppType == TYPE_RPS_02_01)
    {
        packet.u8DataBuffer[6] = 0x30;
    }
    else
    {
        packet.u8DataBuffer[6] = 0x20;
    }

    /* sub-telgram number */
    packet.u8DataBuffer[7] = 0x05;
    /* destination ID */
    if(param_t->u32DeviceID == 0)
    {
        /* broadcast */
        *(uint32*)(&packet.u8DataBuffer[8]) = 0xFFFFFFFF;
    }
    else
    {
        /* ADT */
        *(uint32*)(&packet.u8DataBuffer[8]) = htonl(param_t->u32DeviceID);
    }
    /* dBm */
    packet.u8DataBuffer[12]= 0xFF;
    /* security */
    packet.u8DataBuffer[13]= 0x00;


    /* send the telegram */
    ret = uart_sendPacket(g_uart_iface.uart_io, &packet);

    /* free buffer */
    free(packet.u8DataBuffer);

    return ret;

}

/*static RETURN_TYPE enocean_radio_teach_send(ENOCEAN_SEND_PARAM_T* param_t)
{
    return enocean_radio_rps_send(param_t);
}*/


/* parse a radio telegram */
static RETURN_TYPE enocean_radio_parse(PACKET_ENOCEAN_SERIAL_T *packet)
{
    uint8 i = 0;
    uint32 sender_id = 0;
    boolean teach_in = 0;

    /* check type */
    switch(packet->u8Type)
    {
        /* RADIO type */
        case PACKET_TYPE_RADIO:
        {

            //__fline printf("received radio.\n");

            /* the 1st byte of RADIO data section is Choice. */
            switch(packet->u8DataBuffer[0])
            {
                /* RPS */
                case RORG_RPS:
                {

                    /* 1-byte data, 4-bytes sender device ID, 1-byte status */
                    sender_id = ntohl(*(uint32*)(&packet->u8DataBuffer[2]));

                    if(teach_in_mode)
                    {
                        /* in teach-in mode, all RPS devices will be recoginzed */
                        ENOCEAN_DEVICE_T dev;
                        dev.device_id = sender_id;
                        dev.u8Choice = RORG_RPS;
                        dev.u8Func = FUNC_RPS_ROCKER_SWITCH_2;
                        dev.u8AppType = TYPE_RPS_02_01;
                        dev.on_or_off = 0;
                        dev.trans_or_actuator = 1;
                        enocean_register_device(&dev);
                        __fline printf("RPS switcher '0x%08X' teached.\n", (unsigned int)sender_id);
                    }

                    /* is it a known device id? */
                    for(i = 0; i < MAX_DEVICES; i ++)
                    {
                        if(devices[i].device_id == sender_id)
                        {
                            break;
                        }
                    }
                    
//                    if(i == MAX_DEVICES)
//                    {
//                        // discard RPS telegrams from unknown devices */
//                        __fline printf("RPS telegram from unknown devices(0x%08X) is skipped.\n", (unsigned int)sender_id);
//                        break;
//                    }

                    if(devices[i].u8Choice == RORG_RPS && devices[i].u8Func == 0x10 && devices[i].u8AppType == 0)
                    {
                        /* actuator status */
                        boolean bOn = (packet->u8DataBuffer[1] == 0xE0) ? 1 : 0;

                        ENOCEAN_CALLBACK_PARAM_T param;
                        param.u32DeviceID = sender_id;
                        param.bOn = bOn;
                        devices[i].on_or_off = bOn;
                        if(g_uart_iface.callback)
                        {
                            g_uart_iface.callback(&param);
                        }
                        __fline printf("door %x turns %s.\n", (unsigned int) param.u32DeviceID, bOn?"ON":"OFF");

                    }
                    else
                    {
                        __fline printf("incorrect RPS telegram, skipped.\n");
                    }

                    break;
                }

                /* 1BS */
                case RORG_1BS:
                {

                    /* 1-byte data, 4-bytes sender device ID, 1-byte status */
                    sender_id = ntohl(*(uint32*)(&packet->u8DataBuffer[2]));

                    /* is it a teach-in telegram or data telegram? */
                    teach_in = ((packet->u8DataBuffer[1] & 0x08) == 0) ? 1 : 0;

                    if(teach_in)
                    {
                        /* teach-in telegram */
                        if(teach_in_mode)
                        {
                            /* only receive teach-in telegrams in teach-in mode */
                            ENOCEAN_DEVICE_T dev;
                            dev.device_id = sender_id;
                            dev.u8Choice = RORG_1BS;
                            dev.u8Func = 0x00;
                            dev.u8AppType = 0x01;
                            dev.on_or_off = 0;
                            dev.trans_or_actuator = 1;
                            enocean_register_device(&dev);
                        }

                        break;
                    }

                    /* data telegram */

                    /* is it a known device id? */
                    for(i = 0; i < MAX_DEVICES; i ++)
                    {
                        if(devices[i].device_id == sender_id)
                        {
                            break;
                        }
                    }
                    if(i == MAX_DEVICES)
                    {
                        // discard 1BS telegrams from unknown devices */
                        break;
                    }

                    /* parse the 1BS data */
                    enocean_radio_1BS_parse(&devices[i], &packet->u8DataBuffer[1]);

                    break;
                }

                /* 4BS */
                case RORG_4BS:
                {

                    /* 4-bytes data, 4-bytes sender device ID, 1-byte status */
                    sender_id = ntohl(*(uint32*)(&packet->u8DataBuffer[5]));

                    /* is it a teach-in telegram or data telegram? */
                    teach_in = ((packet->u8DataBuffer[4] & 0x08) == 0) ? 1 : 0;

                    if(teach_in)
                    {
                        /* teach-in telegram */
                        if(teach_in_mode)
                        {
                            /* only receive teach-in telegrams in teach-in mode */
                            ENOCEAN_DEVICE_T dev;
                            dev.device_id = sender_id;
                            dev.u8Choice = RORG_4BS;
                            dev.u8Func = (packet->u8DataBuffer[1]&0xFC) >> 2;
                            dev.u8AppType = ((packet->u8DataBuffer[1]&0x03) << 6) | ((packet->u8DataBuffer[2]&0xF8) >> 3);
                            dev.on_or_off = 0;
                            dev.trans_or_actuator = 1;
                            enocean_register_device(&dev);
                        }

                        break;
                    }

                    /* data telegram */
                    /* is it a known device id? */
                    for(i = 0; i < MAX_DEVICES; i ++)
                    {
                        if(devices[i].device_id == sender_id)
                        {
                            break;
                        }
                    }
                    if(i == MAX_DEVICES)
                    {
                        // discard 4BS telegrams from unknown devices */
                        break;
                    }

                    /* parse the 4BS data */
                    enocean_radio_4BS_parse(&devices[i], &packet->u8DataBuffer[1]);

                    break;
                }
                /* others */
                default:
                {

                    break;
                }
            }

            break;
        }

        /* RESPONSE type */
        case PACKET_TYPE_RESPONSE:
        {

            if(packet->u16DataLength == 1)
            {
                if(packet->u8DataBuffer[0] == PACKET_RET_OK)
                {
                    /* RET_OK means the previous radio sent succeed */
                    //__fline printf("received RET_OK, means last request had been successfully sent.\n");
                }
            }

            break;
        }
        /* Other types */
        default:
        {

            __fline printf("Unsupported telegram received. Type:%d.\n", packet->u8Type);
            return TYPE_NOT_SUPPORTED;
        }
    }
    return OK;
}

/* parse the data (1-byte) of a 1BS radio telegram */
static void enocean_radio_1BS_parse(ENOCEAN_DEVICE_T* device, uint8* data_1bs)
{
    if(device->u8Choice != RORG_1BS && device->u8Choice != RORG_RPS)
    {
        /* not a 1BS/PRS device */
        return;
    }

    /* for our own actuators, the 4rd (offset 3) byte indicates on/off status.
            0001 1000 = ON
            0000 1000 = OFF  */
    if(device->u8Func == FUNC_RPS_ROCKER_SWITCH_2 && device->u8AppType == TYPE_RPS_02_01)
    {
        /* actuator status */
        boolean bOn = ((*data_1bs & 0x10) == 0x10) ? 1 : 0;

        ENOCEAN_CALLBACK_PARAM_T param;
        param.u32DeviceID = device->device_id;
        param.bOn = bOn;
        device->on_or_off = bOn;
        if(g_uart_iface.callback)
        {
            g_uart_iface.callback(&param);
        }
        //__fline printf("actuator %x turns %s.\n", (unsigned int) param.u32DeviceID, bOn?"ON":"OFF");
    }
    else if(device->u8Func == FUNC_1BS_CONTACTS_SWITCHES && device->u8AppType == TYPE_1BS_00_01)
    {
        /* actuator status */
        boolean bOn = ((*data_1bs & 0x01) == 0) ? 1 : 0;

        ENOCEAN_CALLBACK_PARAM_T param;
        param.u32DeviceID = device->device_id;
        param.bOn = bOn;
        device->on_or_off = bOn;
        if(g_uart_iface.callback)
        {
            g_uart_iface.callback(&param);
        }
        __fline printf("actuator %x turns %s.\n", (unsigned int) param.u32DeviceID, bOn?"ON":"OFF");
    }
}

/* parse the data (4-bytes) of a 4BS radio telegram */
static void enocean_radio_4BS_parse(ENOCEAN_DEVICE_T* device, uint8* data_4bs)
{
    if(device->u8Choice != RORG_4BS)
    {
        /* not a 4BS device */
        return;
    }

    if(device->u8Func == FUNC_4BS_LIGHT_TEMP_OCCUPANCY && device->u8AppType == TYPE_4BS_08_01)
    {
        /* human occupancy sensor */
        boolean bOn = ((data_4bs[3] & 0x02) == 0) ? 1 : 0;

        ENOCEAN_CALLBACK_PARAM_T param;
        param.u32DeviceID = device->device_id;
        param.bOn = bOn;
        device->on_or_off = bOn;
        if(g_uart_iface.callback)
        {
            g_uart_iface.callback(&param);
        }
        __fline printf("sensor %x signals %s.\n", (unsigned int) device->device_id, bOn?"ON":"OFF");
    }

}

/* UART data callback */
void enocean_radio_recv(int nId, unsigned char *pBuf, int nLen)
{
    /* print bytes */
    //__fline printf("get bytes:%d\n", nLen);
    //return ;
    /*int i;
    for (i = 0 ; i < nLen ; i++)
    {
        __fline printf("get byte[%d]:%x\n", i, pBuf[i]);
    }*/
    //sem_wait(&empty_sem);

    /* copy to buffer array */
    if(nLen > MAX_BUFFER_LEN)
    {
        __fline printf("data length exceeds max buffer length, new data skipped.\n");
        return;
    }

    //pthread_mutex_lock(&__mutex);

    in = in % MAX_BUFFER_COUNT;
    //printf("product%d in %d. like: \t", id, in);
    //push = in;
    int temp_push = in;
    //    if(uart_buffer_array[temp_push].ready == 1)
    //    {
    //        __fline printf("buffer array already full, new data skipped.\n");
    //        return;
    //    }
    //push = (push + 1) % MAX_BUFFER_COUNT;
    //__fline printf("temp_push:%d\n", temp_push);

    uart_buffer_array[temp_push].len = nLen;
    memcpy((void*)uart_buffer_array[temp_push].data, pBuf, nLen);
    //uart_buffer_array[temp_push].ready = 1;
    ++in;

    //pthread_mutex_unlock(&__mutex);
    sem_post(&__full_sem);

    return;
}

#endif

