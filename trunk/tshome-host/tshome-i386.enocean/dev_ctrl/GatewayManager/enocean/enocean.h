
#ifndef ENOCEAN_H
#define ENOCEAN_H

#ifdef __cplusplus
extern "C"
{
#endif


/* define enocean support */
#define DEV_ENOCEAN_SUPPORT

#ifdef DEV_ENOCEAN_SUPPORT

/* integers */
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef uint8 boolean;

/* log header */
#ifndef __fline
    #define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define DEFAULT_ENOCEAN_DEVICE_INTERFACE "/dev/ttyUSB0"
//#define DEFAULT_ENOCEAN_DEVICE_INTERFACE "/dev/ttySAC3"

/** Three elements in EEP profile: RORG, FUNC, TYPE **/
/* RORG(Choice) */
typedef uint8 RORG_TYPE_T;
#define RORG_RPS                                  ( 0xF6 )
#define RORG_1BS                                  ( 0xD5 )
#define RORG_4BS                                  ( 0xA5 )
#define RORG_ADT                                  ( 0xA6 )
#define RORG_SIGNAL                               ( 0xD0 )

/* FUNC */
typedef uint8 FUNC_TYPE_T;
/* Function for RPS */
#define FUNC_RPS_ROCKER_SWITCH_2                  ( 0x02 )
#define FUNC_RPS_ROCKER_SWITCH_4                  ( 0x03 )
#define FUNC_RPS_POSITION_SWITCH_4                ( 0x04 )
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
} ENOCEAN_SEND_PARAM_T;

/* Enocean recv parameters */
typedef struct __enocean_callback_param_t
{
    // Enocean device id
    uint32 u32DeviceID;

    // on or off
    boolean bOn;
} ENOCEAN_CALLBACK_PARAM_T;


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


#endif


#ifdef __cplusplus
}
#endif

#endif

