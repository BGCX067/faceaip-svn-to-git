/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CHandlerSmartHomeTelcom.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: CHandlerSmartHomeTelcom.cpp 0001 2012-04-06 09:58:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "CommonInclude.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "HandlerSmartHomeTelcom.h"

#ifdef USE_SOCKET_STATUS_AGENT
//#include "SocketStatusAgent.h"
#endif
//#include "GeneralAgentTcpSocketServer.h"
//#include "GeneralAgentTcpSocketConnector.h"

#include "HttpKeepConnectorSmartHomeTelcom.h"

CHandlerSmartHomeTelcom::CHandlerSmartHomeTelcom(CDoubleOutLog *pLog)
        :SocketHandler(pLog)
{
    ;
}


#define SIZE 5000
void CHandlerSmartHomeTelcom::tprintf(TcpSocket *p,const char *format, ...)
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


void CHandlerSmartHomeTelcom::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "Handler Socket List\n");
    tprintf(sendto, "------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);

        if (p->GetRemotePort()<=0)
        {
            continue;
        }

        tprintf(sendto, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
        tprintf(sendto, "  %9s  %s", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str());
#if 0

        CGeneralAgentTcpSocketServer *pServer = dynamic_cast<CGeneralAgentTcpSocketServer *>(p);
        CGeneralAgentTcpSocketConnector *pConnector = dynamic_cast<CGeneralAgentTcpSocketConnector *>(p);
#ifdef USE_SOCKET_STATUS_AGENT

        CSocketStatusAgent *p4 = dynamic_cast<CSocketStatusAgent *>(p);
        if (p4)
        {
            tprintf(sendto, "CSocketStatusAgent");
        }
        else
#endif
            if (pServer)
            {
                tprintf(sendto, "CGeneralAgentTcpSocketServer");
            }
            else if (pConnector)
            {
                tprintf(sendto, "CGeneralAgentTcpSocketConnector");
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
        if (p3)
        {
            tprintf(sendto, "\tBytes Read: %9lu\n",p3 -> GetBytesReceived());
            tprintf(sendto, "\tBytes Sent: %9lu\n",p3 -> GetBytesSent());
        }
    }
    tprintf(sendto, "\n");
}


void CHandlerSmartHomeTelcom::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p0 = (*it).second;
        ARG_USED(p0);

#ifdef USE_SOCKET_STATUS_AGENT

        TcpSocket *p = dynamic_cast<CSocketStatusAgent *>(p0);
        if (p)
        {
            List(p);
        }
#endif

        CHttpKeepConnectorSmartHomeTelcom *p = dynamic_cast<CHttpKeepConnectorSmartHomeTelcom *>(p0);
        if (p)
        {
            p->Update();
        }

    }
}

Socket * CHandlerSmartHomeTelcom::GetSocketByName(std::string strServerName)
{
    //int ii=0;
    //printf("Handler GetSocketByName\n");
    //printf("------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        //Socket *p3 = dynamic_cast<Socket *>(p);

        if (p->GetSockName()==strServerName
            /*&& p->IsConnected()*//*调试期间关闭*/)
        {
            return p;
        }
        else
        {}

        //tprintf(sendto, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
        //tprintf(sendto, "  %9s  %s", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str());
    }

    DEB_CODE(printf("CHandlerSmartHomeTelcom::GetSocketByName(%s) failed.\n", strServerName.c_str()););

    return NULL;
}
