/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * enocean_mgr.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: enocean_mgr.h 5884 2012-11-08 10:44:10Z WuJunjie $
 *
 *  Explain:
 *     -enocean设备协议， 参考文档： 《EnOcean_Equipment_Profiles_EEP2.1》-
 *
 *  Update:
 *     2012-11-08 10:44:10  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __ENOCEAN_MGR_H
#define __ENOCEAN_MGR_H

#include "enocean.h"
#include "ezserial.h"

#ifdef DEV_ENOCEAN_SUPPORT

#ifdef __cplusplus
extern "C"
{
#endif
    /* UART interface structure */
    typedef struct __uart_interface_t
    {
        ezserial_t *uart_io;
        com_info_t uart_info;
        enocean_callback_status_t callback;
    }
    UART_INTERFACE_T;

    /* ENOCEAN device structure */
    typedef struct __enocean_device_t
    {
        unsigned int device_id;                                             /* device id */
        RORG_TYPE_T u8Choice;                                         /* RORG (choice)  */
        FUNC_TYPE_T u8Func;                                           /* Func type  */
        APP_TYPE_T u8AppType;                                         /* Application type  */

        boolean on_or_off;                                            /* initial on/off status byte */
        boolean trans_or_actuator;                                    /* transmitter (1) or actuator (0) */

        // 以下用于维护设备onoff的可靠性
        int last_opr_onoff;// 最后一次操作
        time_t tt_last_opr;
        int feedback_status;
        time_t tt_last_feedback;
    }
    ENOCEAN_DEVICE_T;


    /* Initialize enocean uart interface */
    /*enocean_interface 设备接口， 如果为NULL， 则默认为：/dev/ttyUSB0*/
    int enocean_uart_init(const char *enocean_interface);

    /* Set enocean callbacks */
    void enocean_uart_callback(enocean_callback_status_t callback);

    /* Open enocean uart interface */
    RETURN_TYPE enocean_uart_open();

    /* Switch into teach-in mode */
    void enocean_start_teach_in();

    /* Stop teach-in mode, and into normal mode */
    void enocean_stop_teach_in();

    /* Register an enocean device */
    void enocean_register_device(const ENOCEAN_DEVICE_T* device);

    /* Get total number of the known/registered enocean devices */
    int enocean_get_device_num();

    /* Get the specified enocean device with index */
    void enocean_get_device(int index, ENOCEAN_DEVICE_T* device);
    int getDeviceAttr(unsigned int enoceanDevID, ENOCEAN_DEVICE_T *devAttr);

    void enocean_uart_close();

    /* get enocean uart status */
    /*
    	0 - ok, live, others not live
    -1 - serial not open 
     */
    int enocean_uart_status();

    /* Switch ON/OFF the target enocean actuator */
    RETURN_TYPE enocean_device_onoff(unsigned int u32DeviceID,             /* target device id */
                                     int rocker,                     /* rocker (1/2) */
                                     boolean on);                    /* switch on or off */

    RETURN_TYPE enocean_device_find(uint32 u32DeviceID,             /* target device id */
                                    int rocker,                     /* rocker (1/2) */
                                    boolean on);                    /* switch on or off */

    /* Begin dimming up/down the target enocean actuator */
    RETURN_TYPE enocean_device_dim_begin(uint32 u32DeviceID,         /* target device id */
                                         int rocker,                 /* rocker (1/2) */
                                         boolean up);                /* dimming up or down */

    /* End dimming up/down the target enocean actuator */
    RETURN_TYPE enocean_device_dim_end(uint32 u32DeviceID,   /* target device id */
                                       int rocker,                /* rocker (1/2) */
                                       boolean up);               /* dimming up or down  */

    RETURN_TYPE on_enocean_packet(PACKET_ENOCEAN_SERIAL_T *packet);

        void on_enocean_radio_RPS(PACKET_ENOCEAN_SERIAL_T *packet, ENOCEAN_DEVICE_T* enoction_device_attr_of_this_packet);

    

#ifdef __cplusplus
}
#endif

#endif //DEV_ENOCEAN_SUPPORT

#endif //__ENOCEAN_MGR_H

