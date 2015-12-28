/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerialHandler.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerialHandler.cpp 0001 2012-04-06 09:58:05Z WuJunjie $
 *
 *  Explain:
 *     -反应堆-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "DevSerial.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "TeleSerialHandler.h"

#ifdef TeleSerial_FUNC_STATUS_AGENT
#include "TeleSerialStatusAgent.h"
#endif
#ifdef TeleSerial_FUNC_TCPSOCKETSERVER
#include "TeleSerialTcpSocketServer.h"
#endif
#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR
#include "TeleSerialTcpSocketConnector.h"
#endif
#ifdef TeleSerial_FUNC_UDPSOCKETSERVER
#include "TeleSerialUdpSocket.h"
#endif

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

CTeleSerialHandler::CTeleSerialHandler(CTeleSerialLog *pLog)
        :SocketHandler(pLog)
#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
        , m_sigData(1)
#endif //TeleSerial_FUNC_SIGNAL_ONDATA
{
    ;
}


#define SIZE 5000
void CTeleSerialHandler::tprintf(TcpSocket *p,const char *format, ...)
{
    va_list ap;
    size_t n;
    char tmp[SIZE];

    va_start(ap,format);
#ifdef _WIN32

    n = vsprintf(tmp,format,ap);
#else

    n = vsnprintf(tmp,SIZE - 1,format,ap);
#endif

    va_end(ap);

    p -> SendBuf(tmp, strlen(tmp));
}


void CTeleSerialHandler::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "Handler Socket List\n");
    tprintf(sendto, "------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
#ifdef TeleSerial_FUNC_UDPSOCKETSERVER

        CTeleSerialUdpSocket *pCGeneralAgentUdpSocket = dynamic_cast<CTeleSerialUdpSocket *>(p);
#else

        Socket *pCGeneralAgentUdpSocket = NULL;
#endif

        if (p->GetRemotePort()<=0 || pCGeneralAgentUdpSocket)
        {
            //__trip;
            continue;
        }

        tprintf(sendto, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
        tprintf(sendto, "  %9s  %s %s", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str(), p -> IsConnected() ? "Conned" : "NOT Conned");
#if 0

        CTeleSerialTcpSocketServer *pServer = dynamic_cast<CTeleSerialTcpSocketServer *>(p);
        CTeleSerialTcpSocketConnector *pConnector = dynamic_cast<CTeleSerialTcpSocketConnector *>(p);
#ifdef TeleSerial_FUNC_STATUS_AGENT

        CTeleSerialStatusAgent *p4 = dynamic_cast<CTeleSerialStatusAgent *>(p);
        if (p4)
        {
            tprintf(sendto, "CTeleSerialStatusAgent");
        }
        else
#endif
            if (pServer)
            {
                tprintf(sendto, "CTeleSerialTcpSocketServer");
            }
            else if (pConnector)
            {
                tprintf(sendto, "CTeleSerialTcpSocketConnector");
            }
            else
            {
                tprintf(sendto, "Socket");
            }
#endif
        tprintf(sendto, "\n");
        tprintf(sendto, "\tUptime:  %d days %02d:%02d:%02d\n",
                p -> Uptime() / 86400,
                (p -> Uptime() / 3600) % 24,
                (p -> Uptime() / 60) % 60,
                p -> Uptime() % 60);
        if (p3 || pCGeneralAgentUdpSocket)
        {
            tprintf(sendto, "\tBytes Read: %9lu\n",p -> GetBytesReceived());
            tprintf(sendto, "\tBytes Sent: %9lu\n",p -> GetBytesSent());
        }
    }
    tprintf(sendto, "\n");
}


void CTeleSerialHandler::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p0 = (*it).second;
        ARG_USED(p0);

#ifdef TeleSerial_FUNC_STATUS_AGENT

        TcpSocket *p = dynamic_cast<CTeleSerialStatusAgent *>(p0);
        if (p)
        {
            List(p);
        }
#endif

    }
}
#include <iostream>
Socket * CTeleSerialHandler::GetSocketByName(std::string strServerName)
{
    //int ii=0;
    //printf("Handler GetSocketByName\n");
    //printf("------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        TcpSocket *pTcp = dynamic_cast<TcpSocket *>(p);

        if (p->GetSockName()==strServerName )
        {
            if (!pTcp || (pTcp && p->IsConnected()))
            {
                return p;
            }
            else
            {
                std::cout << "find:" << strServerName << "but not connected"  << std::endl;
            }
        }
        else
        {
            DBG(
                std::cout << "want:" << strServerName << " Me:" << p->GetSockName() << std::endl;
            );
        }

        //tprintf(sendto, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
        //tprintf(sendto, "  %9s  %s", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str());
    }

    DBG(printf("CTeleSerialHandler::GetSocketByName(%s) failed.\n", strServerName.c_str()););

    return NULL;
}

int CTeleSerialHandler::CloseAndDeleteAll()
{
    int SocketNum = 0;

    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        p->SetCloseAndDelete();
        SocketNum++;
    }

    return SocketNum;
}

#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
#include "EZLock.h"
bool CTeleSerialHandler::AddSignal(CEZObject * pObj, SIG_TeleSerialHandler_DATA::SigProc pProc)
{
    CEZLock _lock(m_MutexSig);

    if(m_sigData.Attach(pObj, pProc) < 0)
    {
        printf("attach error\n");
        return false;
    }

    return true;
}

bool CTeleSerialHandler::DelSignal(CEZObject * pObj, SIG_TeleSerialHandler_DATA::SigProc pProc)
{
    CEZLock _lock(m_MutexSig);

    if(m_sigData.Detach(pObj, pProc) < 0)
    {
        printf("attach error\n");
        return false;
    }

    return false;

}

void CTeleSerialHandler::OnDataCall(Socket *pSocket, const char *buf,size_t len)
{
    if (buf!=NULL && len > 0)
    {
        m_sigData(pSocket, buf, len);
    } // if

DBG(
    __fline;
    printf("%s:%d\n", __FUNCTION__, len);
);
    // tcp server 模式
    CTeleSerialTcpSocketServer *pTcp = dynamic_cast<CTeleSerialTcpSocketServer *>(pSocket);

    if (pTcp && pTcp->IsConnected() && pTcp->GetParent()->GetPort()==60000 )
    {
        CDevSerial::instance()->SendData(33, (uint8_t *)buf, len);
    }

}
#endif //TeleSerial_FUNC_SIGNAL_ONDATA

void CTeleSerialHandler::OnSerial(int id, const char *pdat, size_t len)
{
DBG(
    __fline;
    printf("CTeleSerialHandler::OnSerial:id:%d, len:%d -- %c\n", id, len, pdat[0]);
);
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CTeleSerialTcpSocketServer *pTcp = dynamic_cast<CTeleSerialTcpSocketServer *>(p);

        if (pTcp && pTcp->IsConnected() && pTcp->GetParent()->GetPort()==60000/*who want this serial*/ )
        {
DBG(
            __fline;
            printf("port:%d\n", pTcp->GetParent()->GetPort());
);
            pTcp->SendBuf(pdat, len);
        }
    }
}
