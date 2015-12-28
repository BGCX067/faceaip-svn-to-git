#ifndef ESP_H
#define ESP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "enocean.h"

#ifdef DEV_ENOCEAN_SUPPORT
#include "ezserial.h"


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
} PACKET_ENOCEAN_SERIAL_T;

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




/* receive a ESP telegram */
RETURN_TYPE uart_getPacket(uint8 *buffer,                             /* buffer pointing to received bytes */
                           int bufLen,                                /* byte counts */
                           PACKET_ENOCEAN_SERIAL_T *pPacket,          /* output packet structure */
                           uint16 u16BufferLength);                   /* packet data buffer length */

/* send a ESP telegram */
RETURN_TYPE uart_sendPacket(ezserial_t *uart,                         /* uart interface */
                            PACKET_ENOCEAN_SERIAL_T *pPacket);        /* packet to send */



#endif //DEV_ENOCEAN_SUPPORT

#ifdef __cplusplus
}
#endif

#endif //ESP_H