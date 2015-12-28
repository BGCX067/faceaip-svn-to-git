/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentHandler.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentHandler.cpp 0001 2012-04-06 09:58:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "GeneralAgentHandler.h"
#include "GeneralAgentTcpSocketServer.h"

GeneralAgentHandler::GeneralAgentHandler()
:SocketHandler()
{
	;
}


#define SIZE 5000
void GeneralAgentHandler::tprintf(TcpSocket *p,const char *format, ...)
{
	va_list ap;
//	size_t n;
	char tmp[SIZE];

	va_start(ap,format);
#ifdef _WIN32
	vsprintf(tmp,format,ap);	//n =
#else
	vsnprintf(tmp,SIZE - 1,format,ap);	//n =
#endif
	va_end(ap);

	p -> SendBuf(tmp, strlen(tmp));
}


void GeneralAgentHandler::List(TcpSocket *sendto)
{
	tprintf(sendto, "Handler Socket List\n");
	for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
	{
		Socket *p = (*it).second;
		TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
		//CGeneralAgentTcpSocket *p4 = dynamic_cast<CGeneralAgentTcpSocket *>(p);
		//CGeneralAgentTcpSocketServer *pServer = dynamic_cast<CGeneralAgentTcpSocketServer *>(p);
		tprintf(sendto, " %s:%d",p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
		tprintf(sendto, "  %s  ",p -> Ready() ? "Ready" : "NOT_Ready");
//		if (pServer)
//		{
//			//if (pServer->GetRemoteAddress()==JJJ)
//			//{
//				pServer->hfhfhh(buf, len);
//
//			//}
//			tprintf(sendto, "CGeneralAgentTcpSocket");
//		}
		//return ;
		tprintf(sendto, "\n");
		tprintf(sendto, "  Uptime:  %d days %02d:%02d:%02d\n",
			p -> Uptime() / 86400,
			(p -> Uptime() / 3600) % 24,
			(p -> Uptime() / 60) % 60,
			p -> Uptime() % 60);
		if (p3)
		{
			tprintf(sendto, "    Bytes Read: %9lu\n",p3 -> GetBytesReceived());
			tprintf(sendto, "    Bytes Sent: %9lu\n",p3 -> GetBytesSent());
		}
	}
	tprintf(sendto, "\n");
}

void GeneralAgentHandler::SendToAllHost(char * pBuf, int nLen)
{
	Socket *pSocket = NULL;
	CGeneralAgentTcpSocketServer *pServer = NULL;
	for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
	{
		pSocket= it->second;
		pServer = dynamic_cast<CGeneralAgentTcpSocketServer *>(pSocket);
		if(pServer)
		{
			pServer->SendToHost(pBuf, nLen);
		}
	}
}

void GeneralAgentHandler::Update()
{
	for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
	{
		Socket *p0 = (*it).second;
		TcpSocket *p = dynamic_cast<TcpSocket *>(p0);
		if (p)
		{
			List(p);
		}
	}
}

