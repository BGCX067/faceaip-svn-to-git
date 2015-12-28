/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerialTcpSocketServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerialTcpSocketServer.cpp 0001 2012-04-06 09:58:37Z WuJunjie $
 *
 *  Explain:
 *     -tcp服务器实例-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

//#include "CommonInclude.h"
#include <string.h>

#include "TeleSerialTcpSocketServer.h"

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

CTeleSerialTcpSocketServer::CTeleSerialTcpSocketServer(ISocketHandler& h, std::string strSocketName)
        :CTeleSerialTcpSocket(h, strSocketName)
{
    SetDeleteByHandler();
}

void CTeleSerialTcpSocketServer::OnAccept()
{
    printf("CTeleSerialTcpSocketServer::OnAccept:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}
void CTeleSerialTcpSocketServer::OnDisconnect()
{
    printf("CTeleSerialTcpSocketServer::OnDisconnect()\n");
}

void CTeleSerialTcpSocketServer::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG( printf("CTeleSerialTcpSocketServer::OnCommand, len:%d\n", ulCmdDataLen); );
    // 测试用， 关闭自己
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='q')
    {
        SetCloseAndDelete();

        DBG( printf("CTeleSerialTcpSocketServer::OnCommand q, SetCloseAndDelete\n"); );
    }

    // 测试用， echo
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='e')
    {
        char strEchoBuf[128];
        sprintf(strEchoBuf, "CTeleSerialTcpSocketServer echo,cur time:%ld\n", time(NULL));
        SendBuf(strEchoBuf, strlen(strEchoBuf)+1 );
    }
    //////////
}

#ifdef TeleSerial_FUNC_DATAPARSE_EXAMPLE
// 通用头 pCmd
// 数据 pData， 长度见 pCmd->u8DataLen
void CTeleSerialTcpSocketServer::OnCommand_WifiAgent(WIFI_AGENT_PROTOCOL_HEAD *pCmd, unsigned char *pData)
{
    CTeleSerialTcpSocket::OnCommand_WifiAgent(pCmd, pData);
    printf("CTeleSerialTcpSocketServer::OnCommand_WifiAgent\n");
}
#endif
