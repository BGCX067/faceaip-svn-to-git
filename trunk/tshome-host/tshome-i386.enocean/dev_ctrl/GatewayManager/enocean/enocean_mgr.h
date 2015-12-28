#ifndef ENOCEAN_MGR_H
#define ENOCEAN_MGR_H

#include "enocean.h"

#ifdef DEV_ENOCEAN_SUPPORT
#include "ezserial.h"


/* maximum data bytes in enocean packet (including data and optional data) */
// 异常繁忙的时候加大，还要根据cpu性能， 基本上不必考虑加大了
#define MAX_PACKET_DATA_LEN 64

/* maximum buffer count for uart */
#define MAX_BUFFER_COUNT 32
/* maximum data buffer length for uart */
#define MAX_BUFFER_LEN 64

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


    /* UART buffer structure */
    typedef struct __uart_buffer_t
    {
        uint8 data[MAX_BUFFER_LEN];                                   /* pointer to buffer */
        uint8 len;                                                    /* actual buffer length */
        boolean ready;                                                /* whether data is ready or not */
    }
    UART_BUFFER_T;


    /* ENOCEAN device structure */
    typedef struct __enocean_device_t
    {
        uint32 device_id;                                             /* device id */
        RORG_TYPE_T u8Choice;                                         /* RORG (choice)  */
        FUNC_TYPE_T u8Func;                                           /* Func type  */
        APP_TYPE_T u8AppType;                                         /* Application type  */

        boolean on_or_off;                                            /* initial on/off status byte */
        boolean trans_or_actuator;                                    /* transmitter (1) or actuator (0) */
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
    void enocean_register_device(ENOCEAN_DEVICE_T* device);

    /* Get total number of the known/registered enocean devices */
    int enocean_get_device_num();

    /* Get the specified enocean device with index */
    void enocean_get_device(int index, ENOCEAN_DEVICE_T* device);

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

#ifdef __cplusplus
}
#endif


#endif


#endif //ENOCEAN_MGR_H

