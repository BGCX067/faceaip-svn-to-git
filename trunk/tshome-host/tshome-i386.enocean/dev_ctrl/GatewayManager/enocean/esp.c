#include "esp.h"
#include "crc8.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/time.h> 
#include <stdio.h>
#include <arpa/inet.h>

#ifdef DEV_ENOCEAN_SUPPORT

#define ESP_SYNC_CODE (0x55)
#define ESP_INTERBYTE_TIME_OUT (100)
#define ESP_HEADER_NR_BYTES (4)


/* reads a char in blocking mode */
extern int ezserial_getchar(ezserial_t *serial);



/*
    ESP3 packet structure through the serial port.
    Protocol bytes are generated and sent by the application.
    
    Sync = 0x55
    CRC8H
    CRC8D
    
        1            2                 1            1           1       u16DataLen + u8OptionLen       1
    +------+------------------+---------------+-----------+-----------+-------------/------------+-----------+
    | 0x55 |    u16DataLen    |  u8OptionLen  |   u8Type  |    CRC8H  |           DATAS          |   CRC8D   |
    +------+------------------+---------------+-----------+-----------+-------------/------------+-----------+

    
    DATAS structure:
    
                    u16DataLen                           u8OptionLen
    +--------------------------------------------+----------------------+
    |                  Data                      |         Optional     |
    +--------------------------------------------+----------------------+
*/


RETURN_TYPE uart_getPacket(uint8 *buffer, int bufLen, PACKET_ENOCEAN_SERIAL_T *pPacket, uint16 u16BufferLength)
{
    //! uart_getPacket state machine states.
    typedef enum
    {
        //! Waiting for the synchronisation byte 0x55
        GET_SYNC_STATE=0,
        //! Copying the 4 after sync byte: raw data length (2 bytes), optional data length (1), type (1).
        GET_HEADER_STATE,
        //! Checking the header CRC8 checksum. Resynchronisation test is also done here
        CHECK_CRC8H_STATE,
        //! Copying the data and optional data bytes to the paquet buffer
        GET_DATA_STATE,
        //! Checking the info CRC8 checksum.
        CHECK_CRC8D_STATE,
    } STATES_GET_PACKET;

    
    //! UART received byte code
    uint8 u8RxByte;
    //! Checksum calculation
    static uint8 u8CRC = 0;
    //! Nr. of bytes received
    static uint16 u16Count = 0;
    //! State machine counter
    static STATES_GET_PACKET u8State = GET_SYNC_STATE;
    //! Timeout measurement
    static struct timeval t_system_time;
    static uint32 u32TickCount = 0;
    // Byte buffer pointing at the paquet address
    uint8 *u8Raw = (uint8*)pPacket;
    // Temporal variable
    uint8 i;
    // Count of input buffer
    uint8 u8BufCount = 0;
    // Check for timeout between two bytes
    gettimeofday(&t_system_time, NULL);
    if (t_system_time.tv_usec/1000 - u32TickCount > ESP_INTERBYTE_TIME_OUT)
    {
        // Reset state machine to init state
        u8State = GET_SYNC_STATE;
    }

    // State machine goes on when a new byte is received
    //printf("before ezserial_getchar\n");
    while (u8BufCount < bufLen)
    {
        u8RxByte = buffer[u8BufCount++];
        //printf("ezserial_getchar reads:%x\n", u8RxByte);

        // Tick count of last received byte
        u32TickCount = t_system_time.tv_usec/1000;
        
        // State machine to load incoming packet bytes
        switch(u8State)
        {
            // Waiting for packet sync byte 0x55
            case GET_SYNC_STATE:
                
                if (u8RxByte == ESP_SYNC_CODE)
                {
                    u8State = GET_HEADER_STATE;
                    u16Count = 0;
                    u8CRC = 0;
                }
                
                break;
                
            // Read the header bytes
            case GET_HEADER_STATE:
                
                // Copy received data to buffer
                u8Raw[u16Count++] = u8RxByte;
                u8CRC = proc_crc8(u8CRC, u8RxByte);
                // All header bytes received?
                if(u16Count == ESP_HEADER_NR_BYTES)
                {
                    // Convert data length
                    pPacket->u16DataLength = ntohs(pPacket->u16DataLength);
                    u8State = CHECK_CRC8H_STATE;
                }
                
                break;
            
            // Check header checksum & try to resynchonise if error happened
            case CHECK_CRC8H_STATE:
                
                // Header CRC correct?
                if (u8CRC != u8RxByte)
                {
                    // No. Check if there is a sync byte (0x55) in the header
                    int a = -1;
                    for (i = 0 ; i < ESP_HEADER_NR_BYTES ; i++)
                        if (u8Raw[i] == ESP_SYNC_CODE)
                        {
                            // indicates the next position to the sync byte found
                            a = i+1;
                            break;
                        };
                    
                    if ((a == -1) && (u8RxByte != ESP_SYNC_CODE))
                    {
                        // Header and CRC8H does not contain the sync code
                        u8State = GET_SYNC_STATE;
                        break;
                    }
                    else if((a == -1) && (u8RxByte == ESP_SYNC_CODE))
                    {
                        // Header does not have sync code but CRC8H does.
                        // The sync code could be the beginning of a packet
                        u8State = GET_HEADER_STATE;
                        u16Count = 0;
                        u8CRC = 0;
                        break;
                    }
                    
                    // Header has a sync byte. It could be a new telegram.
                    // Shift all bytes from the 0x55 code in the buffer.
                    // Recalculate CRC8 for those bytes
                    u8CRC = 0;
                    for (i = 0 ; i < (ESP_HEADER_NR_BYTES - a) ; i++)
                    {
                        u8Raw[i] = u8Raw[a+i];
                        u8CRC = proc_crc8(u8CRC, u8Raw[i]);
                    }
                    u16Count = ESP_HEADER_NR_BYTES - a;
                    // u16Count = i; // Seems also valid and more intuitive than u16Count -= a;
                    
                    // Copy the just received byte to buffer
                    u8Raw[u16Count++] = u8RxByte;
                    u8CRC = proc_crc8(u8CRC, u8RxByte);
                    if(u16Count < ESP_HEADER_NR_BYTES)
                    {
                        u8State = GET_HEADER_STATE;
                        break;
                    }
                    break;
                }
                
                // CRC8H correct. Length fields values valid?
                if((pPacket->u16DataLength + pPacket->u8OptionLength) == 0)
                {
                    //No. Sync byte received?
                    if((u8RxByte == ESP_SYNC_CODE))
                    {
                        //yes
                        u8State = GET_HEADER_STATE;
                        u16Count = 0;
                        u8CRC = 0;
                        break;
                    }

                    // Packet with correct CRC8H but wrong length fields.
                    u8State = GET_SYNC_STATE;
                    return OUT_OF_RANGE;
                }
                
                // Correct header CRC8. Go to the reception of data.
                u8State = GET_DATA_STATE;
                u16Count = 0;
                u8CRC = 0;
                
                break;
                
            // Copy the information bytes
            case GET_DATA_STATE:
                
                // Copy byte in the packet buffer only if the received bytes have enough room
                if(u16Count < u16BufferLength)
                {
                    pPacket->u8DataBuffer[u16Count] = u8RxByte;
                    u8CRC = proc_crc8(u8CRC, u8RxByte);
                }
                /*else
                {
                    return OUT_OF_RANGE;
                }*/
                
                // When all expected bytes received, go to calculate data checksum
                if( ++u16Count == (pPacket->u16DataLength + pPacket->u8OptionLength) )
                {
                    u8State = CHECK_CRC8D_STATE;
                }
                
                break;
                
            // Check the data CRC8
            case CHECK_CRC8D_STATE:
                
                // In all cases the state returns to the first state: waiting for next sync byte
                u8State = GET_SYNC_STATE;

                // Received packet bigger than space to allocate bytes?
                if (u16Count > u16BufferLength) return OUT_OF_RANGE;

                // Enough space to allocate packet. Equals last byte the calculated CRC8?
                if (u8CRC == u8RxByte) return OK; // Correct packet received

                // False CRC8.
                // If the received byte equals sync code, then it could be sync byte for next paquet.
                if((u8RxByte == ESP_SYNC_CODE))
                {
                    u8State = GET_HEADER_STATE;
                    u16Count = 0;
                    u8CRC = 0;
                }
                
                return NOT_VALID_CHKSUM;
                
            default:
            	{
                
                // Yes. Go to the reception of info.
                u8State = GET_SYNC_STATE;
                
                break;
            }
        }
    }
    
    return WAIT_MORE_DATA;//OK;
}

/* Send an ESP packet */
// 如果数据错， 则模块不会发送， 找这个问题的时候不能靠网关的指示灯 wujj
RETURN_TYPE uart_sendPacket(ezserial_t *uart, PACKET_ENOCEAN_SERIAL_T *pPacket)
{
    uint16 i;
    uint8 u8CRC;
    // When both length fields are 0, this telegram is not allowed.
    if(pPacket->u16DataLength == 0 && pPacket->u8OptionLength == 0)
    {
        return OUT_OF_RANGE;
    }

    /* 0x55+head+CRC8H+data+optional + CRC8D*/
    int send_buf_len = 1+4+1+ntohs(pPacket->u16DataLength) + pPacket->u8OptionLength+1;
    uint8_t *pSendBuf = (uint8_t *)malloc(send_buf_len);
    int cur_data_len = 0;
    assert(pSendBuf);
    
    // Sync
    //ezserial_putchar(uart, ESP_SYNC_CODE);
    *(pSendBuf+cur_data_len) = ESP_SYNC_CODE;
    cur_data_len++;
    
    //printf("put byte:%x\n", ESP_SYNC_CODE);

    // Header
    //ezserial_putdata(uart, (uint8*) pPacket, 4);
    memcpy(pSendBuf+cur_data_len, (uint8*) pPacket, 4);
    cur_data_len += 4;

    // Header CRC
    u8CRC = 0;
    u8CRC = proc_crc8(u8CRC, ((uint8*)pPacket)[0]);
    u8CRC = proc_crc8(u8CRC, ((uint8*)pPacket)[1]);
    u8CRC = proc_crc8(u8CRC, ((uint8*)pPacket)[2]);
    u8CRC = proc_crc8(u8CRC, ((uint8*)pPacket)[3]);
    //ezserial_putchar(uart, u8CRC);
    //printf("put byte:%x\n", u8CRC);
    *(pSendBuf+cur_data_len) = u8CRC;
    cur_data_len++;
 
    // Data
    u8CRC = 0;
    for (i = 0 ; i < (ntohs(pPacket->u16DataLength) + pPacket->u8OptionLength) ; i++)
    {
        u8CRC = proc_crc8(u8CRC, pPacket->u8DataBuffer[i]);
        //ezserial_putchar(uart, pPacket->u8DataBuffer[i]);
        *(pSendBuf+cur_data_len) = pPacket->u8DataBuffer[i];
        cur_data_len++;
        //printf("put byte:%x\n", pPacket->u8DataBuffer[i]);
    }

    // Data CRC
    //ezserial_putchar(uart, u8CRC);
    *(pSendBuf+cur_data_len) = u8CRC;
    cur_data_len++;
    
    //printf("send_buf_len:%d\n", send_buf_len);
    ezserial_putdata(uart, pSendBuf, send_buf_len);
    
    if (EZSERIAL_OK!=ezserial_geterror())
    {
        printf("%s\n", ezserial_geterrormsg());
        return -1;
    }
    else
    {
         //printf("\t%4d bytes sent successfully.\n", send_buf_len); 
    }
    
    free(pSendBuf);
    //printf("cur_data_len:%d\n", cur_data_len);

    return OK;
}

#endif

