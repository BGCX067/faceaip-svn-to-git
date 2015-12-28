/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerWebService.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerWebService.cpp 5884 2013-01-29 03:21:10Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:10  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define _DEBUG
#include "CommonInclude.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "HandlerWebService.h"

#include "HttpServerWebService.h"

CHandlerWebService::CHandlerWebService(CDoubleOutLog *pLog)
        :SocketHandler(pLog)
{
    ;
}


#define SIZE 5000
void CHandlerWebService::tprintf(TcpSocket *p,const char *format, ...)
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


void CHandlerWebService::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "HandlerWebService Socket List\n");
    tprintf(sendto, "---------------------------------------------------\n");
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
        tprintf(sendto, "\n");
        tprintf(sendto, "\tUptime:  %d days %02d:%02d:%02d\n",
                p -> Uptime() / 86400,
                (p -> Uptime() / 3600) % 24,
                (p -> Uptime() / 60) % 60,
                p -> Uptime() % 60);
        if (p3)
        {
            tprintf(sendto, "\tBytes Read: %9lu\n",p -> GetBytesReceived());
            tprintf(sendto, "\tBytes Sent: %9lu\n",p -> GetBytesSent());
        }
    }
    tprintf(sendto, "\n");
}


void CHandlerWebService::Update()
{}

Socket * CHandlerWebService::GetSocketByName(std::string strServerName)
{
    //int ii=0;
    //printf("Handler GetSocketByName\n");
    //printf("------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        //Socket *p3 = dynamic_cast<Socket *>(p);

        if (p->GetSockName()==strServerName
            && p->IsConnected())
        {
            return p;
        }
        else
        {}

        //tprintf(sendto, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
        //tprintf(sendto, "  %9s  %s", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str());
    }

    DEB_CODE(printf("CHandlerWebService::GetSocketByName(%s) failed.\n", strServerName.c_str()););

    return NULL;
}


Socket * CHandlerWebService::GetSocketBySessionID(std::string strSessionID)
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CHttpServerWebService *p3 = dynamic_cast<CHttpServerWebService *>(p);

        if (p3 && p3->getMySessionId()==strSessionID)
        {
            //p3->SetPJ_turn_srv(m_Pj_turn_srv);
            return p;
        }
        else
        {}
    }

    DEB_CODE(printf("CHandlerWebService::GetSocketBySessionID(%s) failed.\n", strSessionID.c_str()););

    return NULL;
}

int CHandlerWebService::newSession(const CSession &newss)
{
    DBG_CODE(
        __fline;
        printf("newSession, m_strSession:%s \n", newss.m_strSession.c_str());
    );

    m_requestSession.newSession(newss);
    return 0;
}

// 鉴权
int CHandlerWebService::AuthSession(std::string strSession, int iAccessId)
{
    DBG_CODE(
        __fline;
        printf("newSession, strSession:%s, iAccessId:%d \n", strSession.c_str(), iAccessId);
    );

    int iRet;

    CSession __Session;

    iRet = m_requestSession.findSession(strSession, __Session);

    // find
    if (iRet==0)
    {
        // 检查数据
        if (time(NULL)-__Session.m_iTimeout > __Session.m_ttLastRequestTime)
        {
            m_requestSession.Maintain();

            return -1;
        }

        return 0;


    }

    return -1;
}
