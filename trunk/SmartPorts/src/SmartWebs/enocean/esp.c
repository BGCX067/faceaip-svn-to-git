/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * esp.c - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: esp.c 5884 2012-11-08 02:28:42Z WuJunjie $
 *
 *  Explain:
 *     -enocean串口协议， 参考文档： 《EnOceanSerialProtocol3_V1.17_08》-
 *
 *  Update:
 *     2012-11-08 02:28:42  改写解析、发送接口 
 *                      1. 避免丢掉反馈包； 
 *						2. 提高解析效率
 *						3. 避免内存动态分配
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/time.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "CommonInclude.h"
#include "ez_system_api.h"
#include "enocean.h"
#include "crc8.h"
#include "enocean_mgr.h"
#include "str_opr.h"
#ifdef USE_PROCESS_THREAD
#include <pthread.h>
#include <semaphore.h>
#endif
#ifdef DEV_ENOCEAN_SUPPORT
    int uart_sendPacket(ezserial_t *uart,                         /* uart interface */
                        PACKET_ENOCEAN_SERIAL_T *pPacket);        /* packet to send */

    size_t esp_onData(unsigned char *pEspBuffer, size_t *EspBufferLen, struct timeval *EspOnDataTimeval, unsigned char *pData, size_t Len, PACKET_ENOCEAN_SERIAL_T *pPacket);

int __ez_timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y);
#define ESP_SYNC_CODE (0x55)
#define ESP_INTERBYTE_TIME_OUT (100)
#define ESP_HEADER_NR_BYTES (4)

#ifdef USE_PROCESS_THREAD
extern int in;
extern sem_t __full_sem;
extern PACKET_ENOCEAN_SERIAL_T g_message_array[MAX_BUFFER_COUNT];
#endif // USE_PROCESS_THREAD

/* 定义全局缓存， 可同时支持多个enocean设备 */
unsigned char g_bufEspBufferOnBoard[MAX_ESP_BUFF_LEN]={0};
size_t g_bufLen=0;
struct timeval g_bufLenTimeval=
    {
        0
    };

void on_enocean_data(int nId, unsigned char *pBuf, int nLen)
{
    /* copy to buffer array */
    if(nLen > MAX_PACKET_DATA_LEN)
    {
        __fline printf("data length exceeds max buffer length, new data skipped.\n");
        return;
    }
    else if(nLen < 1 || pBuf==NULL)
    {
        __fline printf("data not valid, nLen:%d.\n", nLen);
        return;
    }

    esp_onData(g_bufEspBufferOnBoard, &g_bufLen, &g_bufLenTimeval, pBuf, nLen, NULL);

    return;
}

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

// ret - packet num
size_t esp_onData(unsigned char *pEspBuffer, size_t *EspBufferLen, struct timeval *pEspOnDataTimeval, unsigned char *pData, size_t Len, PACKET_ENOCEAN_SERIAL_T *pPacket)
{
    size_t curPacketPosition;
    size_t curPacketLen;
    struct timeval t_now_time;
    struct timeval t_sub_time;
    double sub_ms;
    HEAD_ESP3_T tmpHeader;
    size_t numPacket=0;
    int iRet=0;

    //printf("OnData:%d, BufLen:%d\n", Len, *EspBufferLen);

    if (Len>MAX_ESP_BUFF_LEN
        || Len<1
        || pData==NULL)
    {
        __trip;
        printf("data pointer(0x%0x) or len:%d not valid.\n", (unsigned int)pEspBuffer, Len);

        return 0;
    }
    if (*EspBufferLen+Len>MAX_ESP_BUFF_LEN)
    {
        __trip;
        printf("data too len:%d\n", *EspBufferLen+Len);

        *EspBufferLen = 0;

        return 0;
    }

    // Check for timeout between two bytes
    gettimeofday(&t_now_time, NULL);
	
    iRet = __ez_timeval_subtract(&t_sub_time, &t_now_time, pEspOnDataTimeval);
    if (iRet==0)
    {
        sub_ms = t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0;
    }
    else
    {
        __trip;
        sub_ms = 0;
    }
	
    //printf("timeval_subtract is: %ld, %ld = %lf(ms)\n", t_sub_time.tv_sec, t_sub_time.tv_usec, sub_ms);

    if (sub_ms/*t_system_time.tv_usec/1000 - u32TickCount*/ > ESP_INTERBYTE_TIME_OUT)
    {
        //__fline;
        //printf("timeout\n");
        *EspBufferLen = 0;
    }
    gettimeofday(pEspOnDataTimeval, NULL);
    ///

	// Store data
    memcpy(pEspBuffer+*EspBufferLen, pData, Len);
    *EspBufferLen = (*EspBufferLen)+Len;

    curPacketPosition =0;
    while(*EspBufferLen > (1+sizeof(HEAD_ESP3_T)+1)/*sysc+head*/)
    {
        // for sync byte 0x55
        while(*(pEspBuffer+curPacketPosition) != ESP_SYNC_CODE)
        {
            curPacketPosition++;

            if (curPacketPosition >= (*EspBufferLen))
            {
                //__trip;
                *EspBufferLen = 0;
                return numPacket;
            }
        }

        // 剩余长度不足head
        if ((*EspBufferLen-curPacketPosition) < (1+sizeof(HEAD_ESP3_T)+1))
        {
            break;
        }

        memcpy((char *)&tmpHeader, (char *)(pEspBuffer+curPacketPosition+1), sizeof(HEAD_ESP3_T));

        tmpHeader.DataLen = ntohs(tmpHeader.DataLen);
        //printf("tmpHeader.DataLen:%d, tmpHeader.OptionLength:%d, tmpHeader.PacketType:%d\n", tmpHeader.DataLen, tmpHeader.OptionLength, tmpHeader.PacketType );

        curPacketLen = 1+sizeof(HEAD_ESP3_T)+1+tmpHeader.DataLen + tmpHeader.OptionLength + 1;

        // 数据校验
        if(tmpHeader.DataLen>MAX_PACKET_DATA_LEN
           || tmpHeader.OptionLength>MAX_PACKET_DATA_LEN)
        {
            // 跳过首字符，继续下一轮
            //__trip;
            //printf("data not valid: tmpHeader.DataLen:%d, tmpHeader.OptionLength:%d, tmpHeader.PacketType:%d\n", tmpHeader.DataLen, tmpHeader.OptionLength, tmpHeader.PacketType );
            // 将余下数据打印
            DEB_CODE(
                dumpBuffer(stdout
                           , (unsigned char *)pEspBuffer+curPacketPosition, (*EspBufferLen)-curPacketPosition,
                           SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
            );

            //curPacketLen = 1;
            curPacketPosition++;
            continue;
        }

        if ((*EspBufferLen-curPacketPosition) < curPacketLen)
        {
            break;
        }

        /////////
        // find a packet
        DBG_CODE(
            __fline;
            printf("Have a packet:%d, offset:%d add:0x%0x\n", curPacketLen, curPacketPosition, (unsigned int)(pEspBuffer+curPacketPosition));
        );
        //////
        // todo crc

        //////

        //////////////
        // 发送消息
#ifdef USE_PROCESS_THREAD

        in = in % MAX_BUFFER_COUNT;
        int temp_push = in;

        g_message_array[temp_push].u16DataLength = tmpHeader.DataLen;
        g_message_array[temp_push].u8OptionLength = tmpHeader.OptionLength;
        g_message_array[temp_push].u8Type = tmpHeader.PacketType;
        g_message_array[temp_push].u8DataBuffer = g_message_array[temp_push].u8AllData+(1+sizeof(HEAD_ESP3_T)+1);
        memcpy(&g_message_array[temp_push].u8AllData, (pEspBuffer+curPacketPosition), curPacketLen);
        g_message_array[temp_push].AllDataLen = curPacketLen;

        ++in;

        sem_post(&__full_sem);
#else
        //不另外启动线程
        PACKET_ENOCEAN_SERIAL_T __packet;
        __packet.u16DataLength = tmpHeader.DataLen;
        __packet.u8OptionLength = tmpHeader.OptionLength;
        __packet.u8Type = tmpHeader.PacketType;
        __packet.u8DataBuffer = __packet.u8AllData+(1+sizeof(HEAD_ESP3_T)+1);
        memcpy(&__packet.u8AllData, (pEspBuffer+curPacketPosition), curPacketLen);
        __packet.AllDataLen = curPacketLen;

        on_enocean_packet(&__packet);
#endif// 发送消息
        //////////////

        numPacket++;
        /////////
        curPacketPosition += curPacketLen;
    }

    // 剩余数据置于首部
    *EspBufferLen = (*EspBufferLen)-curPacketPosition;
    memcpy(pEspBuffer, pEspBuffer+curPacketPosition, (*EspBufferLen) );

    return numPacket;
}

/* Send an ESP packet */
// 如果数据错， 则模块不会发送， 找这个问题的时候不能靠网关的指示灯 wujj
int uart_sendPacket(ezserial_t *uart, PACKET_ENOCEAN_SERIAL_T *pPacket)
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

int __ez_timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
#ifdef LINUX
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec)
    {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000)
    {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
    //return 0;
#else
	return -1;
#endif
}
#endif

