/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * main.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: main.cpp 5884 2012-11-12 11:06:13Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-11-12 11:06:13  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "CommonInclude.h"
#include <stdio.h>
#include <stdlib.h>

#include "../enocean_mgr.h"

int test_enocean_find();
int test_enocean_onoff();

#ifdef DEV_ENOCEAN_SUPPORT
void enocean_callback(ENOCEAN_CALLBACK_PARAM_T *param);
#endif
void test_get_callback();

//
static int quit = 0;

int main(int argc, char* argv[])
{
    int iRet = 0;

    printf("\n\n$ %s: main.c,v 1.0 %s %s Exp $\n\n", argv[0], __DATE__, __TIME__);
    printf("start ...\n");

#ifdef DEV_ENOCEAN_SUPPORT

    ezserial_init();
    enocean_uart_init(DEFAULT_ENOCEAN_DEVICE_INTERFACE);
    //enocean_uart_init("/dev/ttySAC3");
    enocean_uart_open();
    enocean_uart_callback(enocean_callback);
    
    //test_enocean_find();
    test_enocean_onoff();
    //test_get_callback();

    enocean_uart_close();
    ezserial_cleanup();
#else

    printf("DEV_ENOCEAN_SUPPORT not defined, finished ...\n");
#endif // DEV_ENOCEAN_SUPPORT

    return iRet;
}

#ifdef DEV_ENOCEAN_SUPPORT

void enocean_callback(ENOCEAN_CALLBACK_PARAM_T *param)
{
    ENOCEAN_CALLBACK_PARAM_T *__pa = (ENOCEAN_CALLBACK_PARAM_T *)param;

    //__fline;
    printf("\tenocean_callback u32DeviceID:0x%0x, u8Type:0x%0x, ucFunction:0x%0x, bOn:%d, value:%f\n"
           , (unsigned int)__pa->u32DeviceID
           , __pa->u8Type
           , __pa->ucFunction
           , __pa->bOn
           , __pa->fValue);
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

void test_get_callback()
{
    while (!quit)
    {
        sleep(1);
    }
}

int test_enocean_onoff()
{
    int iRet = 0;

    // unsigned int __device_id = 0xFFED9B80; // 窗帘
    // unsigned int __device_id = 0x10189e5;// 开关
    // unsigned int __device_id = 0x101889D;
    unsigned int __device_id = 0x10189e5;
    int __rocker = 1;
    boolean __up = 1; // 0 调暗， 1 调亮

    time_t ttLastDim = 0;
    static unsigned int round_run = 0;

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
            //enocean_device_onoff(__device_id, __rocker, __up);
            //enocean_device_onoff(__device_id, __rocker, __up);

            //__fline;
            printf("\n%8d -- enocean_device_onoff(0x%x, %d, %d)\n", round_run++, __device_id, __rocker, __up);

            __up = !__up;

            ttLastDim = time(NULL);
            //break;
        }
        else
        {
            sleep(1);
        }
    } //while (!quit)

    return iRet;
}
#endif // DEV_ENOCEAN_SUPPORT
