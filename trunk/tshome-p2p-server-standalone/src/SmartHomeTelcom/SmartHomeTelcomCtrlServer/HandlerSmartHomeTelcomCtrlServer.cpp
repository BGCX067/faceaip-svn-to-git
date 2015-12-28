/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerSmartHomeTelcomCtrlServer.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: HandlerSmartHomeTelcomCtrlServer.cpp 5884 2012-09-06 03:25:33Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-06 03:25:33  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "CommonInclude.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "HandlerSmartHomeTelcomCtrlServer.h"

#include "HttpServerSmartHomeTelcomCtrlServer.h"

CHandlerSmartHomeTelcomCtrlServer::CHandlerSmartHomeTelcomCtrlServer(CDoubleOutLog *pLog)
        :SocketHandler(pLog)
{
    ;
}


#define SIZE 5000
void CHandlerSmartHomeTelcomCtrlServer::tprintf(TcpSocket *p,const char *format, ...)
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


void CHandlerSmartHomeTelcomCtrlServer::List(TcpSocket *sendto)
{
}


void CHandlerSmartHomeTelcomCtrlServer::Update()
{

}

Socket * CHandlerSmartHomeTelcomCtrlServer::GetSocketByName(std::string strServerName)
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

    DEB_CODE(printf("CHandlerSmartHomeTelcomCtrlServer::GetSocketByName(%s) failed.\n", strServerName.c_str()););

    return NULL;
}
