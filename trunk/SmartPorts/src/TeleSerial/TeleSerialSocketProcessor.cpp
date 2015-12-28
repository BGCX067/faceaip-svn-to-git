/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralSocketProcessor.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralSocketProcessor.cpp 5884 2012-07-18 02:51:38Z WuJunjie $
 *
 *  Explain:
 *     -单独的消息处理对象-
 *
 *  Update:
 *     2012-07-18 02:51:38  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <Utility.h>

#include "TeleSerialSocketProcessor.h"

PATTERN_SINGLETON_IMPLEMENT(CTeleSerialSocketProcessor);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
//使用自己的队列, 原始队列不用
CTeleSerialSocketProcessor::CTeleSerialSocketProcessor() :CEZThread("CTeleSerialSocketProcessor", TP_DEFAULT/*, 100*/)
{
    printf("CTeleSerialSocketProcessor Enter--------\n");

    m_pSocketMsgQue = new CSocketMsgQue(0);// 0 - do not limit size
}

CTeleSerialSocketProcessor::~CTeleSerialSocketProcessor()
{
    printf("CTeleSerialSocketProcessor Leave--------\n");

    if(m_pSocketMsgQue)
    {
        m_pSocketMsgQue->ClearMessage();
        delete m_pSocketMsgQue;
    }
}

BOOL CTeleSerialSocketProcessor::Start()
{
    if (m_bLoop)
    {
        return TRUE;
    }
    int ret = CreateThread();

    return ret;
}

BOOL CTeleSerialSocketProcessor::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = FALSE;
        DestroyThread();
    }
    return TRUE;
}
#define MSG_TYPE_PRODUCER 100
#define MSG_TYPE_TIMER 101

void CTeleSerialSocketProcessor::ThreadProc()
{
    SOCKET_MSG_NODE msg;

    while (m_bLoop)
    {
        //__trip;
        if (FALSE == RecvSocketMessage(&msg, TRUE))
        {
            __trip ;
            sleep(1);
            continue;
        }

        // 收到消息，在此加入处理代码
        // 下列代码打印
        {
            __fline;
            printf("CTeleSerialSocketProcessor::ThreadProc RecvSocketMessage\n");

            char buf[1024];
            struct sockaddr_in sa;
            memcpy((char *)&sa,&msg.sa_from,msg.sa_len);
            ipaddr_t a;
            memcpy(&a,&sa.sin_addr,4);
            std::string ip;
            Utility::l2ip(a,ip);

            snprintf(buf, sizeof(buf), "Received %d bytes from: %s:%d\n", msg.iLen,ip.c_str(),ntohs(sa.sin_port));
            printf(buf);
            if (msg.pData)
            {
                __fline;
                printf("%s\n",static_cast<std::string>(msg.pData).substr(0,msg.iLen).c_str());

                // 返回数据示例
                if (msg.pSocket)
                {
                    //sendto(msg.pSocket->GetSocket(), buf, strlen(buf), 0, sa_from, sa_len);
                    sendto(msg.pSocket->GetSocket(), static_cast<std::string>(msg.pData).substr(0,msg.iLen).c_str(), static_cast<std::string>(msg.pData).substr(0,msg.iLen).size(), 0, &msg.sa_from,msg.sa_len);
                }
            }
            else
            {
                __trip;
            }
        }

        switch(msg.msg)
        {
            case MSG_TYPE_PRODUCER:
            {
                printf("\nReceive msg from PRODUCER\n");
                g_ThreadManager.DumpThreads();

                break;
            }
            case MSG_TYPE_TIMER:
            {
                printf("\nReceive msg from TIMER\n");

                break;
            }
            default:
            {
                break;
            }
        }

        // 这个对象申请的，故而这个对象删除
        if (msg.pData)
        {
            //__trip; // dbg
            delete []msg.pData;
        }

        // __trip;
    }
}

BOOL  CTeleSerialSocketProcessor::SendSocketMsg(unsigned int iMsg,Socket *pSocket, const char *pData,size_t lDataLen,struct sockaddr *sa_from/*=NULL*/,socklen_t sa_len/*=0*/)
{

    char *pBuff;
    if(pData&&lDataLen>0)
    {
        pBuff = new char[lDataLen];
        memcpy(pBuff, pData, lDataLen);
    }
    else
    {
        pBuff = NULL;
    }

    if(m_pSocketMsgQue)
    {
        return m_pSocketMsgQue->SendMessage( iMsg, pSocket, pBuff, lDataLen, sa_from, sa_len);
    }
    return FALSE;
}

BOOL  CTeleSerialSocketProcessor::RecvSocketMessage (SOCKET_MSG_NODE *pMsg, BOOL wait/* = TRUE*/)
{
    if(m_pSocketMsgQue)
    {
        return m_pSocketMsgQue->RecvMessage(pMsg, wait);
    }
    return FALSE;
}

