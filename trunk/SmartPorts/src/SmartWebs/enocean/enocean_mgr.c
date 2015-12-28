/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * enocean_mgr.c - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: enocean_mgr.c 5884 2012-11-08 03:01:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-5-08 03:01:00  Genh Create
 *     2012-6-28 17:07:46
 *     				改为采用消息驱动模式
 *     2012-11-8 15:01:44 改为处理线程可选
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "CommonInclude.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "str_opr.h"
/* send a ESP telegram */

#include "enocean_mgr.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*sleep*/


#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;


#ifdef USE_PROCESS_THREAD
#include <pthread.h>
#include <semaphore.h>
#endif

#ifdef DEV_ENOCEAN_SUPPORT

/* thread proc */
#ifdef USE_PROCESS_THREAD
void * thread_proc_func(void *);
#endif

#ifdef CHECK_STATUS_THREAD
void * thread_proc_check_status(void * arg);
#endif

int uart_sendPacket(ezserial_t *uart,                         /* uart interface */
                    PACKET_ENOCEAN_SERIAL_T *pPacket);        /* packet to send */

size_t esp_onData(unsigned char *pEspBuffer, size_t *EspBufferLen, struct timeval *EspOnDataTimeval, unsigned char *pData, size_t Len, PACKET_ENOCEAN_SERIAL_T *pPacket);

/* send a radio telegram */
static RETURN_TYPE enocean_radio_send(ENOCEAN_SEND_PARAM_T *param_t);
/* send a RPS radio telegram */
static RETURN_TYPE enocean_radio_rps_send(ENOCEAN_SEND_PARAM_T* param_t);

/* telegram data callback */
void on_enocean_data(int nId, unsigned char *pBuf, int nLen);
/* parse a 1BS radio telegram */
static void on_enocean_radio_1BS(PACKET_ENOCEAN_SERIAL_T *packet, ENOCEAN_DEVICE_T* device, uint8* pData_1BS);
/* parse a 4BS radio telegram */
static void on_enocean_radio_4BS(PACKET_ENOCEAN_SERIAL_T *packet, ENOCEAN_DEVICE_T* device, uint8* pData_4BS);

/* uart interface */
static UART_INTERFACE_T g_uart_iface = {0};

/* uart buffer array */
//UART_BUFFER_T uart_buffer_array[MAX_BUFFER_COUNT];
PACKET_ENOCEAN_SERIAL_T g_message_array[MAX_BUFFER_COUNT];

#ifdef USE_PROCESS_THREAD
sem_t __full_sem;   // 同步信号量， 当没产品时阻止消费者消费
//pthread_mutex_t __mutex; // 互斥信号量， 一次只有一个线程访问缓冲
int in = 0;   // 生产者放置产品的位置
int out = 0; // 消费者取产品的位置
/* uart thread id */
static pthread_t thread_id;
/* thread running flag */
static boolean thread_run = 0;
#endif //USE_PROCESS_THREAD

#ifdef CHECK_STATUS_THREAD
static pthread_t thread_id_check_status_thread;
/* thread running flag */
static boolean thread_run_check_status_thread = 0;

// 加了维护线程，避免冲突
pthread_mutex_t __mutex_onoff;

#endif //CHECK_STATUS_THREAD

/* enocean devices */
#define MAX_DEVICES 100
static ENOCEAN_DEVICE_T g_devices_attr[MAX_DEVICES]={{0}};
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

#if __UNIT_TEST

    ENOCEAN_DEVICE_T my_devices;

    //开关驱动器
    my_devices.device_id = 0x010189E5;
    my_devices.u8Choice = RORG_RPS;
    my_devices.u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    my_devices.u8AppType = TYPE_RPS_02_01;
    enocean_register_device(&my_devices);
    //插座
    my_devices.device_id = 0x0101889f;
    my_devices.u8Choice = RORG_RPS;
    my_devices.u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    my_devices.u8AppType = TYPE_RPS_02_01;
    enocean_register_device(&my_devices);

    my_devices.device_id = 0x0080919F;
    my_devices.u8Choice = RORG_RPS;
    my_devices.u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    my_devices.u8AppType = TYPE_RPS_02_01;
    enocean_register_device(&my_devices);

    my_devices.device_id = 0x00036A28; // 外购人感
    my_devices.u8Choice = RORG_4BS;
    my_devices.u8Func = FUNC_4BS_LIGHT_TEMP_OCCUPANCY;
    my_devices.u8AppType = TYPE_4BS_08_01;
    my_devices.trans_or_actuator = 1;
    enocean_register_device(&my_devices);
    my_devices.device_id = 0x01034e98; // 自研人感
    my_devices.u8Choice = RORG_4BS;
    my_devices.u8Func = FUNC_4BS_LIGHT_TEMP_OCCUPANCY;
    my_devices.u8AppType = TYPE_4BS_08_01;
    my_devices.trans_or_actuator = 1;
    enocean_register_device(&my_devices);

    my_devices.device_id = 0x01019F9D; // new actuator
    my_devices.u8Choice = RORG_RPS;
    my_devices.u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    my_devices.u8AppType = TYPE_RPS_02_01;
    enocean_register_device(&my_devices);

    my_devices.device_id = 0x0082dd70; // new window sensor
    my_devices.u8Choice = RORG_1BS;
    my_devices.u8Func = FUNC_1BS_CONTACTS_SWITCHES;
    my_devices.u8AppType = TYPE_1BS_00_01;
    my_devices.trans_or_actuator = 1;
    enocean_register_device(&my_devices);

    my_devices.device_id = 0x0010FA72; // door sensor
    my_devices.u8Choice = RORG_RPS;
    my_devices.u8Func = 0x10;
    my_devices.u8AppType = 0x00;
    my_devices.trans_or_actuator = 1;
    enocean_register_device(&my_devices);

    // 2012-7-2 10:45:29 窗磁 add
    my_devices.device_id = 0x0082a74c; // new window sensor
    my_devices.u8Choice = RORG_1BS;
    my_devices.u8Func = FUNC_1BS_CONTACTS_SWITCHES;
    my_devices.u8AppType = TYPE_1BS_00_01;
    my_devices.trans_or_actuator = 1;
    enocean_register_device(&my_devices);

    my_devices.device_id = 0x0101889D;
    my_devices.u8Choice = RORG_RPS;
    my_devices.u8Func = FUNC_RPS_ROCKER_SWITCH_2;
    my_devices.u8AppType = TYPE_RPS_02_01;
    enocean_register_device(&my_devices);

    my_devices.device_id = 0x010282AB; // smoke sensor
    my_devices.u8Choice = RORG_RPS;
    my_devices.u8Func = FUNC_RPS_SENSOR_1PORT_1BIT;
    my_devices.u8AppType = 0x00;
    my_devices.trans_or_actuator = 1;
    enocean_register_device(&my_devices);


#endif //__UNIT_TEST
    /* normal mode */
    teach_in_mode = 0;

#ifdef USE_PROCESS_THREAD
    // 初始化同步信号量
    int ini2 = sem_init(&__full_sem, 0, 0);
    if(ini2 != 0)
    {
        printf("sem init failed \n");
        exit(1);
    }
#endif

    // 互斥锁，保证线程访问的互斥，步骤二，初始化，默认属性初始化互斥量——NULL参数的功能
#ifdef CHECK_STATUS_THREAD
    if ( pthread_mutex_init(&__mutex_onoff, NULL) != 0 )
    {
        __trip;
        perror(strerror(errno));
        //exit(EXIT_FAILURE);
    }
#endif //CHECK_STATUS_THREAD

    return 0;
}

/* register a device, to listen on its status */
void enocean_register_device(const ENOCEAN_DEVICE_T* device)
{
    if (device==NULL
        || device->device_id<1)
    {
        __trip;
        return;
    }

    /* init device list */
    if(devices_num >= MAX_DEVICES)
    {
        fprintf(stderr, "the device list is full, so cannot register more.\n");
        return;
    }
    DBG(
        __fline;
        printf("device->device_id:0x%0x, device->u8Choice:0x%0x, device->u8Func:0x%0x\n", device->device_id, device->u8Choice, device->u8Func);
    );
    /* check duplicates */
    int i;
    for(i = 0; i < devices_num; i++)
    {
        if(g_devices_attr[i].device_id == device->device_id)
        {
            /* already in list, renew */
            g_devices_attr[i].device_id = device->device_id;
            g_devices_attr[i].u8Choice = device->u8Choice;
            g_devices_attr[i].u8Func = device->u8Func;
            g_devices_attr[i].u8AppType = device->u8AppType;
            g_devices_attr[i].on_or_off = device->on_or_off;

            return;
        }
    }

    /* copy device */
    g_devices_attr[devices_num].device_id = device->device_id;
    g_devices_attr[devices_num].u8Choice = device->u8Choice;
    g_devices_attr[devices_num].u8Func = device->u8Func;
    g_devices_attr[devices_num].u8AppType = device->u8AppType;
    g_devices_attr[devices_num].on_or_off = device->on_or_off;

    // 新加的设备才赋状态值
    g_devices_attr[devices_num].last_opr_onoff = -1; // -1 无效
    g_devices_attr[devices_num].tt_last_opr = 0;
    g_devices_attr[devices_num].feedback_status = -1;
    g_devices_attr[devices_num].tt_last_feedback = 0;

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

    memcpy((void *)device, (void *)&g_devices_attr[index], sizeof(ENOCEAN_DEVICE_T));
}

int __enocean_get_device_index(unsigned int device_id)
{
    int i;
    for(i = 0; i < devices_num; i++)
    {
        if(g_devices_attr[i].device_id == device_id)
        {
            return i;
        }
    }

    return -1;
}

// ret >=0 ok
int getDeviceAttr(unsigned int enoceanDevID, ENOCEAN_DEVICE_T *devAttr)
{
    size_t i;
    /* data telegram */
    /* is it a known device id? */
    for(i = 0; i < devices_num; i++)
    {
        if(g_devices_attr[i].device_id == enoceanDevID)
        {
            //printf("%d -- find:0x%0x , 0x%0x\n", i, enoceanDevID, g_devices_attr[i].device_id);
            memcpy(devAttr, (char *)&g_devices_attr[i], sizeof(ENOCEAN_DEVICE_T));
            return i;
            //break;
        }

        if(g_devices_attr[i].device_id < 1)
        {
            return -1;
        }
    }
    if(i == devices_num)
    {
        // discard 4BS telegrams from unknown devices */
        return -2;
        //break;
    }

    return -3;
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
                               on_enocean_data,
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
#ifdef USE_PROCESS_THREAD

    thread_run = 1;
    pthread_create(&thread_id, NULL, thread_proc_func, NULL);
#endif

#ifdef CHECK_STATUS_THREAD

    thread_run_check_status_thread = 1;
    pthread_create(&thread_id_check_status_thread, NULL, thread_proc_check_status, NULL);
#endif

    return 0;
}

/* close uart interface */
void enocean_uart_close()
{
    /* close thread */
#ifdef USE_PROCESS_THREAD
    if(thread_run == 1)
    {
        thread_run = 0;
        //pthread_join(thread_id, NULL);
    }

    sem_post(&__full_sem);
#endif //USE_PROCESS_THREAD
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

#ifdef USE_PROCESS_THREAD
/* 收到完整packet */
void * thread_proc_func(void * arg)
{
    __fline printf("enocean process thread start.\n");

    while(thread_run)
    {
        sem_wait(&__full_sem);

        out = out % MAX_BUFFER_COUNT;
        // 线程退出的时候发送一次信号， 执行标记为false，下段就不执行了，防止出错。
        if(thread_run)
        {
            on_enocean_packet(&g_message_array[out]);

            ++out;
        }
    }

    __fline printf("enocean process thread ends.\n");

    return NULL;
}
#endif //USE_PROCESS_THREAD

#ifdef CHECK_STATUS_THREAD
/* 收到完整packet */
void * thread_proc_check_status(void * arg)
{
    DBG(
        __fline;
        printf("thread_proc_check_status start.\n");
    );
    while(thread_run_check_status_thread)
    {
        //////////////
        // 检查开关驱动器状态
        int i;
        for(i = 0; i < devices_num; i++)
        {
            // 自研开关
            if(g_devices_attr[i].u8Func == FUNC_RPS_ROCKER_SWITCH_2 && g_devices_attr[i].u8AppType == TYPE_RPS_02_01)
            {
                // 新加的设备才赋状态值
                g_devices_attr[devices_num].last_opr_onoff = -1; // -1 无效
                g_devices_attr[devices_num].tt_last_opr = 0;
                g_devices_attr[devices_num].feedback_status = -1;
                g_devices_attr[devices_num].tt_last_feedback = 0;

                // 有效
                if (g_devices_attr[i].last_opr_onoff >= 0
                    && g_devices_attr[i].feedback_status != g_devices_attr[i].last_opr_onoff)
                {
                    enocean_device_onoff(g_devices_attr[i].device_id, 1, g_devices_attr[i].last_opr_onoff);
                    //                    __fline;
                    //                    printf("Found g_devices_attr[%d].feedback_status(%d)!=g_devices_attr[%d].last_opr_onoff(%d)\n"
                    //	                    , i
                    //	                    , g_devices_attr[i].feedback_status
                    //	                    , i
                    //	                    , g_devices_attr[i].last_opr_onoff);

                }
            }
        }
        //////////////
        usleep(1000*50);
        //__fline printf("thread_proc_check_status process thread ends.\n");

    }

    __fline printf("thread_proc_check_status process thread ends.\n");

    return NULL;
}
#endif //CHECK_STATUS_THREAD

/*------------------ telegram handlers ------------------*/
RETURN_TYPE enocean_device_onoff(unsigned int u32DeviceID, int rocker, boolean on)
{
#ifdef CHECK_STATUS_THREAD
    pthread_mutex_lock(&__mutex_onoff);
#endif //CHECK_STATUS_THREAD

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

    //记录操作
    int no = __enocean_get_device_index(u32DeviceID);
    if (no >= 0)
    {

        g_devices_attr[no].last_opr_onoff = on; //
        g_devices_attr[no].tt_last_opr = time(NULL);

        //g_devices_attr[no].feedback_status = bOn;
        //g_devices_attr[no].tt_last_feedback = time(NULL);
    }

#ifdef CHECK_STATUS_THREAD
    pthread_mutex_unlock(&__mutex_onoff);
#endif //CHECK_STATUS_THREAD

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
RETURN_TYPE on_enocean_packet(PACKET_ENOCEAN_SERIAL_T *packet)
{
    unsigned int sender_id = 0;
    boolean teach_in = 0;
    ENOCEAN_DEVICE_T enoction_device_attr_of_this_packet;
    if (packet==NULL)
    {
        return OK;
    }

    /* check type */
    switch(packet->u8Type)
    {
        /* RADIO type */
        case PACKET_TYPE_RADIO:
        {

            DBG_CODE(
                __fline;
                printf("u8AllData(%d):\n", packet->AllDataLen);
                dumpBuffer(stdout
                           , (unsigned char *)packet->u8AllData, packet->AllDataLen,
                           SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
            );
            if(packet->u16DataLength < 1) /*RPS/1BS -- 7 , 4BS -- */
            {
                __trip;
                printf("u16DataLength(%d) not valid.\n", packet->u16DataLength);
                return TYPE_NOT_SUPPORTED;
            }
            /* the 1st byte of RADIO data section is Choice. */
            switch(packet->u8DataBuffer[0])
            {
                /* RPS */
                case RORG_RPS:
                {

                    if(packet->u16DataLength < 7) /*RPS/1BS -- 7 , 4BS -- */
                    {
                        __trip;
                        printf("u16DataLength(%d) not valid for RPS.\n", packet->u16DataLength);
                    }
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

                    int iFind = getDeviceAttr(sender_id, &enoction_device_attr_of_this_packet);
                    //__fline printf("getDeviceAttr '0x%08x' ret:%d.\n", sender_id, iFind);

                    if (iFind<0)
                    {
                        DBG_CODE(
                        __fline;
                        printf("RPS telegram from unknown devices(0x%08x) is skipped.\n", (unsigned int)sender_id);
                        );
                        DBG_CODE(
                            __fline;
                            printf("u8AllData(%d):\n", packet->AllDataLen);
                            dumpBuffer(stdout
                                       , (unsigned char *)packet->u8AllData, packet->AllDataLen,
                                       SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
                        );
                        break;
                    }
                    else
                    {
                        on_enocean_radio_RPS(packet, &enoction_device_attr_of_this_packet);
                    }
                    break;
                }

                /* 1BS */
                case RORG_1BS:
                {
                    if(packet->u16DataLength != 7) /*RPS/1BS -- 7 , 4BS -- */
                    {
                        __trip;
                        printf("u16DataLength(%d) not valid for 1BS.\n", packet->u16DataLength);
                    }
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
                    int iFind = getDeviceAttr(sender_id, &enoction_device_attr_of_this_packet);

                    if (iFind<0)
                    {
                        DBG(
                            __fline;
                            printf("1BS telegram from unknown devices(0x%08x) is skipped.\n", (unsigned int)sender_id);
                        );
                        break;
                    }

                    /* parse the 1BS data */
                    on_enocean_radio_1BS(packet, &enoction_device_attr_of_this_packet, &packet->u8DataBuffer[1]);

                    break;
                }

                /* 4BS */
                case RORG_4BS:
                {
                    if(packet->u16DataLength != 10) /*RPS/1BS -- 7 , 4BS -- */
                    {
                        __trip;
                        printf("u16DataLength(%d) not valid for RPS.\n", packet->u16DataLength);
                    }

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

                    /* is it a known device id? */
                    int iFind = getDeviceAttr(sender_id, &enoction_device_attr_of_this_packet);

                    if (iFind<0)
                    {
                        __fline;
                        printf("4BS telegram from unknown devices(0x%08x) is skipped.\n", (unsigned int)sender_id);
                        __fline;
                        printf("u8DataBuffer(%d):\n", packet->AllDataLen);
                        dumpBuffer(stdout
                                   , (unsigned char *)packet->u8AllData, packet->AllDataLen,
                                   SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
                        break;
                    }

                    /* parse the 4BS data */
                    on_enocean_radio_4BS(packet, &enoction_device_attr_of_this_packet, &packet->u8DataBuffer[1]);

                    break;
                }
                /* others */
                default:
                {
                    __fline;
                    printf("unknow RADIO telegram is skipped.\n");

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

void on_enocean_radio_RPS(PACKET_ENOCEAN_SERIAL_T *packet, ENOCEAN_DEVICE_T* enoction_device_attr_of_this_packet)
{
    if(enoction_device_attr_of_this_packet->u8Choice != RORG_RPS
       || packet->u8DataBuffer[0] != RORG_RPS)
    {
        /* not a rps device, i do not know how to use it */
        __trip;
        return;
    }
    //    __fline;
    //    printf("on_enocean_radio_RPS\n");

    ENOCEAN_CALLBACK_PARAM_T param = {0};
    param.u8Type = packet->u8DataBuffer[0];
    param.ucFunction = enoction_device_attr_of_this_packet->u8Func;

    if(enoction_device_attr_of_this_packet->u8Func == FUNC_RPS_SENSOR_1PORT)
    {
        /* actuator status */
        boolean bOn = (packet->u8DataBuffer[1] == 0xE0) ? 1 : 0;

        param.u32DeviceID = enoction_device_attr_of_this_packet->device_id;//sender_id;
        param.bOn = bOn;
        param.fValue = 0;

        __fline printf("SENSOR 0x%08x turns %s.\n", (unsigned int) param.u32DeviceID, bOn ? "ON" : "OFF");

    }
    // frw rs烟感
    else if(enoction_device_attr_of_this_packet->u8Func == FUNC_RPS_SENSOR_1PORT_1BIT)
    {
        /* actuator status */
        boolean bOn = 1;

        param.u32DeviceID = enoction_device_attr_of_this_packet->device_id;
        param.bOn = bOn;
        param.fValue = 0;

        //__fline printf("SENSOR 0x%08x turns %s.\n", sender_id, bOn ? "ON" : "OFF");

    }
    else
    {
        __fline;
        printf("RPS telegram from unknow device:0x%08x, skippe it.\n", enoction_device_attr_of_this_packet->device_id);
    }

    if(g_uart_iface.callback
       && param.u32DeviceID>0)
    {
        g_uart_iface.callback(&param);
    }
}

/* parse the data (1-byte) of a 1BS radio telegram */
static void on_enocean_radio_1BS(PACKET_ENOCEAN_SERIAL_T *packet, ENOCEAN_DEVICE_T* device, uint8* data_1bs)
{
    if(device->u8Choice != RORG_1BS && device->u8Choice != RORG_RPS)
    {
        /* not a 1BS/RPS device */
        return;
    }
    //    __fline;
    //    printf("on_enocean_radio_1BS 0x%0x\n", packet->u8Type);

    ENOCEAN_CALLBACK_PARAM_T param = {0};
    param.u8Type = packet->u8DataBuffer[0];
    param.ucFunction = device->u8Func;

    /* for our own actuators, the 4rd (offset 3) byte indicates on/off status.
            0001 1000 = ON
            0000 1000 = OFF  */
    if(device->u8Func == FUNC_RPS_ROCKER_SWITCH_2 && device->u8AppType == TYPE_RPS_02_01)
    {
        // 自研开关驱动器
        boolean bOn = ((*data_1bs & 0x10) == 0x10) ? 1 : 0;

        param.u32DeviceID = device->device_id;
        param.bOn = bOn;
        device->on_or_off = bOn;


        int no = __enocean_get_device_index(device->device_id);
        if (no >= 0)
        {
            //g_devices_attr[no].last_opr_onoff = -1; // -1 无效
            //g_devices_attr[no].tt_last_opr = 0;
            g_devices_attr[no].feedback_status = bOn;
            g_devices_attr[no].tt_last_feedback = time(NULL);
        }

        //__fline;
        //printf("actuator %#x turns %s, No:%d.\n", (unsigned int) param.u32DeviceID, bOn?"ON":"OFF", no);
    }
    else if(device->u8Func == FUNC_1BS_CONTACTS_SWITCHES && device->u8AppType == TYPE_1BS_00_01)
    {
#if 0
        /* actuator status */
        boolean bOn = ((*data_1bs & 0x01) == 0) ? 1 : 0;

        param.u32DeviceID = device->device_id;
        param.bOn = bOn;
        device->on_or_off = bOn;
        __fline printf("actuator 0x%x turns %s.\n", (unsigned int) param.u32DeviceID, bOn?"ON":"OFF");
#endif

        /* actuator status */
        boolean bOn = ((*data_1bs & 0x10) == 0x10) ? 1 : 0;

        param.u32DeviceID = device->device_id;
        param.bOn = bOn;
        device->on_or_off = bOn;
        __fline;
        printf("DBG -- FUNC_1BS_CONTACTS_SWITCHES %#x turns %s.\n", (unsigned int) param.u32DeviceID, bOn?"ON":"OFF");
    }

    if(g_uart_iface.callback
       && param.u32DeviceID>0)
    {
        g_uart_iface.callback(&param);
    }

}

/* parse the data (4-bytes) of a 4BS radio telegram */
static void on_enocean_radio_4BS(PACKET_ENOCEAN_SERIAL_T *packet, ENOCEAN_DEVICE_T* device, uint8* data_4bs)
{
    if(device->u8Choice != RORG_4BS)
    {
        /* not a 4BS device */
        return;
    }
    //    __fline;
    //    printf("on_enocean_radio_4BS\n");

    ENOCEAN_CALLBACK_PARAM_T param = {0};
    param.u8Type = packet->u8DataBuffer[0];
    param.ucFunction = device->u8Func;

    if(device->u8Func == FUNC_4BS_LIGHT_TEMP_OCCUPANCY && device->u8AppType == TYPE_4BS_08_01)
    {
        /* human occupancy sensor */
        boolean bOn = ((data_4bs[3] & 0x02) == 0) ? 1 : 0;

        param.u32DeviceID = device->device_id;
        param.bOn = bOn;
        device->on_or_off = bOn;
        if(g_uart_iface.callback)
        {
            g_uart_iface.callback(&param);
        }
        __fline printf("sensor %x signals %s.\n", (unsigned int) device->device_id, bOn?"ON":"OFF");
    }
    if(g_uart_iface.callback
       && param.u32DeviceID>0)
    {
        g_uart_iface.callback(&param);
    }

}

#endif
