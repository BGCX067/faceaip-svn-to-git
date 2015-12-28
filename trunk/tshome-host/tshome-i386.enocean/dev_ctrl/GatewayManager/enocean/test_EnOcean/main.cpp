
#include "CommonInclude.h"
#include <stdio.h>
#include <stdlib.h>

#include "../enocean_mgr.h"

int test_enocean_find();
int test_enocean_onoff();

void enocean_callback(ENOCEAN_CALLBACK_PARAM_T *param);

//
static int quit = 0;

int main(int argc, char* argv[])
{
    int iRet = 0;

    printf("\n\n$ %s: main.c,v 1.0 %s %s Exp $\n\n", argv[0], __DATE__, __TIME__);
    printf("start ...\n");

    ezserial_init();
    enocean_uart_init("/dev/ttyUSB0");
    //enocean_uart_init("/dev/ttySAC3");
    enocean_uart_open();
    enocean_uart_callback(enocean_callback);

    //test_enocean_find();
    test_enocean_onoff();

    enocean_uart_close();
    ezserial_cleanup();

    return iRet;
}

void enocean_callback(ENOCEAN_CALLBACK_PARAM_T *param)
{
    ENOCEAN_CALLBACK_PARAM_T *__pa = (ENOCEAN_CALLBACK_PARAM_T *)param;

    //__fline;
    printf(" u32DeviceID:%#x, bOn:%d", (unsigned int)__pa->u32DeviceID, __pa->bOn);
}

int test_enocean_find()
{
    int iRet = 0;

    // unsigned int __device_id = 0xFFED9B80; // 窗帘
    unsigned int __device_id = 0x10189e5;// 开关
    //unsigned int __device_id = 0x01018A09;
    __device_id = 0x00223580;
    int __rocker = 1;
    boolean __up = 1; // 0 调暗， 1 调亮

    time_t ttLastDim = 0;

    __fline;
    printf("test_enocean_find ...\n");
    while (!quit)
    {
        if ((time(NULL)-ttLastDim)>=1)
       { 
            enocean_device_find(__device_id, __rocker, __up);

            printf("\nenocean_device_find - ");
            ttLastDim = time(NULL);


        }
        else
        {
            sleep(1);
        }
    } 

    return iRet;
}

int test_enocean_onoff()
{
    int iRet = 0;

    // unsigned int __device_id = 0xFFED9B80; // 窗帘
    //unsigned int __device_id = 0x10189e5;// 开关
    unsigned int __device_id = 0x101889D;
    int __rocker = 1;
    boolean __up = 1; // 0 调暗， 1 调亮

    time_t ttLastDim = 0;

    ttLastDim = time(NULL);
    __fline;
    printf("start test ...\n");
    while (!quit)
    {
        if ((time(NULL)-ttLastDim)>=1)
        {
            //enocean_device_dim_end(__device_id, __rocker, __up);
            //enocean_device_dim_end(__device_id, __rocker, __up);
            //enocean_device_dim_end(__device_id, __rocker, __up);

            enocean_device_onoff(__device_id, __rocker, __up);
            enocean_device_onoff(__device_id, __rocker, __up);
            //enocean_device_onoff(__device_id, __rocker, __up);

            __up = !__up;

            ttLastDim = time(NULL);

            //__fline;
            printf("\n%ld enocean_device_onoff(0x%x, %d, %d) %s -- ", time(NULL), __device_id, __rocker, __up, enocean_uart_status()==0?"ON":"OFF");

            //break;
        }
        else
        {
            sleep(1);
        }
    } //while (!quit)

    return iRet;
}

