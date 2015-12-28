/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * enocean.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: enocean.h 5884 2012-11-08 03:12:11Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-11-08 03:12:11  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ENOCEAN_H
#define __ENOCEAN_H

/* define enocean support */
#ifndef DEV_ENOCEAN_SUPPORT
//#define DEV_ENOCEAN_SUPPORT
#endif

// 处理消息是否单独启动线程，默认不启动
// 当处理内容耗时时, ， 请再启动
#ifndef USE_PROCESS_THREAD
//#define USE_PROCESS_THREAD
#endif

// 检查开关驱动器的状态 如果不同，自动刷
// 会造成开关驱动器只由主机管理
//CHECK_STATUS_THREAD
#ifndef CHECK_STATUS_THREAD
//#define CHECK_STATUS_THREAD
#endif//CHECK_STATUS_THREAD


#ifdef __cplusplus
extern "C"
{
#endif

    /* integers */
    typedef unsigned char uint8;
    typedef unsigned short uint16;
    typedef unsigned long uint32;
    typedef uint8 boolean;

#ifdef USE_USB_INTERFACE
#define DEFAULT_ENOCEAN_DEVICE_INTERFACE "/dev/ttyUSB0"
#else
#define DEFAULT_ENOCEAN_DEVICE_INTERFACE "/dev/ttySAC3"
#endif

    /** Three elements in EEP profile: RORG, FUNC, TYPE **/
    /* RORG(Choice) */
    typedef uint8 RORG_TYPE_T;
#define RORG_RPS                                  ( 0xF6 )
#define RORG_1BS                                  ( 0xD5 )
#define RORG_4BS                                  ( 0xA5 )
#define RORG_ADT                                  ( 0xA6 )
#define RORG_SIGNAL                               ( 0xD0 )
#define RORG_VLD                                  ( 0xD2 )

    /* FUNC */
    typedef uint8 FUNC_TYPE_T;
    /* Function for RPS */
#define FUNC_RPS_ROCKER_SWITCH_2                  ( 0x02 )
#define FUNC_RPS_ROCKER_SWITCH_4                  ( 0x03 )
#define FUNC_RPS_POSITION_SWITCH_4                ( 0x04 )
#define FUNC_RPS_SENSOR_1PORT                     ( 0x10 )
    // 传感器 一个状态 无结束标记
#define FUNC_RPS_SENSOR_1PORT_1BIT                ( 0x11 )
    // 天溯温度传感器
#define FUNC_RPS_SENSOR_TEMP_TIANSU_00001         ( 0x12 )

    /* Function for 1BS */
#define FUNC_1BS_CONTACTS_SWITCHES                ( 0x00 )
    /* Function for 4BS */
#define FUNC_4BS_TEMPREATURE                      ( 0x02 )
#define FUNC_4BS_TEMPREATURE_HUMIDITY             ( 0x04 )
#define FUNC_4BS_LIGHT                            ( 0x06 )
#define FUNC_4BS_OCCUPANCY                        ( 0x07 )
#define FUNC_4BS_LIGHT_TEMP_OCCUPANCY             ( 0x08 )
#define FUNC_4BS_GAS                              ( 0x09 )
#define FUNC_4BS_ROOM_PANEL                       ( 0x10 )
#define FUNC_4BS_CONTROLLER_STATUS                ( 0x11 )
#define FUNC_4BS_AMR                              ( 0x12 )
#define FUNC_4BS_ENVIRON_APP                      ( 0x13 )
#define FUNC_4BS_HVAC                             ( 0x20 )
#define FUNC_4BS_DIGITAL_INPUT                    ( 0x30 )
#define FUNC_4BS_ENERGY_MANAGEMENT                ( 0x37 )
#define FUNC_4BS_CENTRAL_COMMAND                  ( 0x38 )
#define FUNC_4BS_UNIVERSAL                        ( 0x3F )

    /* TYPE (App type) */
    typedef uint8 APP_TYPE_T;
#define TYPE_RPS_02_01                            ( 0x01 )
#define TYPE_RPS_02_02                            ( 0x02 )
#define TYPE_1BS_00_01                            ( 0x01 )
#define TYPE_4BS_08_01                            ( 0x01 )
#define TYPE_4BS_08_02                            ( 0x02 )
#define TYPE_4BS_08_03                            ( 0x03 )

    /* Manufacturer ID (11 bit) */
    typedef uint16 MANUFACTURER_ID;

    /* Enocean send parameters */
    typedef struct __enocean_send_param_t
    {
        // target enocean device id
        uint32 u32DeviceID;
        // source device id (gateway id or transmitter id)
        uint32 u32SenderID;
        // RORG (choice)
        RORG_TYPE_T u8Choice;
        // Func type
        FUNC_TYPE_T u8Func;
        // Application type
        APP_TYPE_T u8AppType;

        // Rocker 1 or rocker 2
        uint8 u8Rocker;
        // State 0 or 1
        uint8 u8State;
        // Pressed or released
        boolean bPressed;
    }
    ENOCEAN_SEND_PARAM_T;

    /* Enocean recv parameters */
    typedef struct __enocean_callback_param_t
    {
        // Enocean device id
        uint32 u32DeviceID;
        // on or off
        boolean bOn;
        // rps/1bs ...
        unsigned char u8Type; // rad
        // 功能， 见 FUNC_.. 的定义
        unsigned char ucFunction;
        //返回值
        float fValue;
    }
    ENOCEAN_CALLBACK_PARAM_T;


    /* callback when enocean device status changes */
    typedef void (*enocean_callback_status_t)(ENOCEAN_CALLBACK_PARAM_T *param);

    /* Packet return codes */
    typedef enum
    {
        PACKET_RET_OK = 0x00,
        PACKET_RET_ERROR,
        PACKET_RET_NOT_SUPPORTED,
        PACKET_RET_WRONG_PARAM,
        PACKET_RET_OPERATION_DENIED
    } PACKET_RETURN_CODE;

    /* Return types */
    typedef enum
    {
        OK = 0,                                 /* OK */
        UART_NOT_OPENED,                        /* UART interface not opened */
        WAIT_MORE_DATA,                         /* Need read more bytes */
        OUT_OF_RANGE,                           /* Parameter invalid, or no enough room to hold data */
        OUT_OF_MEMORY,                          /* No enough heap memory */
        TYPE_NOT_SUPPORTED,                     /* Type unsupported */
        NOT_VALID_CHKSUM                        /* CRC8 check error */
    } RETURN_TYPE;

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  ESP 3 -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    /* maximum data bytes in enocean packet (including data and optional data) */
#define MAX_PACKET_DATA_LEN 64
#define MAX_ESP_BUFF_LEN MAX_PACKET_DATA_LEN+1+4+1+1

    /* 队列长度 */
    // 异常繁忙的时候加大，还要根据cpu性能， 基本上不必考虑加大了
#define MAX_BUFFER_COUNT 32

    /* Packet structure (ESP3) */
    typedef struct __head_esp3_t
    {
        unsigned short DataLen;
        unsigned char OptionLength;
        unsigned char PacketType;
    }
    __attribute__ ((packed)) HEAD_ESP3_T;

    /* Packet structure (ESP3) */
    typedef struct __packet_esp3_t
    {
        unsigned char Sync; // 0x55
        HEAD_ESP3_T Header;
        unsigned char crcHeader;
        unsigned char *pData;
        unsigned char *pOption;
        unsigned char crcDataOption;
    }
    __attribute__ ((packed)) PACKET_ESP3_T;

    /* Packet structure (ESP3) */
    typedef struct __packet_enocean_serial_t
    {
        // Amount of raw data bytes to be received. The most significant byte is sent/received first
        uint16 u16DataLength;
        // Amount of optional data bytes to be received
        uint8 u8OptionLength;
        // Packe type code
        uint8 u8Type;
        // Data buffer: raw data + optional bytes
        uint8 *u8DataBuffer;
        // 本包的所有数据， 包括55
        uint8 u8AllData[MAX_ESP_BUFF_LEN];
        size_t AllDataLen;
    }
    PACKET_ENOCEAN_SERIAL_T;

    /* Packet type */
    typedef enum
    {
        PACKET_TYPE_RADIO = 0x01,
        PACKET_TYPE_RESPONSE,
        PACKET_TYPE_RADIO_SUB_TEL,
        PACKET_TYPE_EVENT,
        PACKET_TYPE_COMMON_COMMAND,
        PACKET_TYPE_SMART_ACK_COMMAND,
        PACKET_TYPE_REMOTE_MAN_COMMAND,

        PACKET_TYPE_RESERVED = 0x08,
        PACKET_TYPE_USER = 0x80
    } PACKET_TYPE_T;

    void on_enocean_data(int nId, unsigned char *pBuf, int nLen);


    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  ESP 3 -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__ENOCEAN_H
