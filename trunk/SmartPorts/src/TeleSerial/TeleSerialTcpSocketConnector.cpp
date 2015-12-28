/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerialTcpSocketServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerialTcpSocketServer.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -Tcp客户端实例-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <string.h>
#include "TeleSerialTcpSocketConnector.h"

CTeleSerialTcpSocketConnector::CTeleSerialTcpSocketConnector(ISocketHandler& h
	, std::string strSocketName
	, time_t Timeout
        , bool bReconn)
        :CTeleSerialTcpSocket(h, strSocketName)
        , m_strSocketName(strSocketName)
        ,m_b_connected(false)
{
    // initial connection timeout setting
    SetConnectTimeout(5);

    SetDeleteByHandler();
    
    SetSockName(strSocketName);
}


CTeleSerialTcpSocketConnector::~CTeleSerialTcpSocketConnector()
{}

CTeleSerialTcpSocketConnector *CTeleSerialTcpSocketConnector::Reconnect()
{
    std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
    CTeleSerialTcpSocketConnector *p = new CTeleSerialTcpSocketConnector(Handler(), m_strSocketName, m_ttTimeout, m_bReconn);
    p -> SetDeleteByHandler();
    p -> Open(*ad);
    Handler().Add(p);
    return p;
}


void CTeleSerialTcpSocketConnector::OnConnectFailed()
{
    CTeleSerialTcpSocketConnector *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CTeleSerialTcpSocketConnector::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}

void CTeleSerialTcpSocketConnector::SetCloseAndDelete()
{
    CTeleSerialTcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CTeleSerialTcpSocketConnector::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    // do not modify, wujj 2012-4-9 15:31:44 end
    ////////// add your code here

    printf("CTeleSerialTcpSocketConnector::OnConnect()\n");
}
void CTeleSerialTcpSocketConnector::OnReconnect()
{
    printf("CTeleSerialTcpSocketConnector::OnReconnect()\n");
}

void CTeleSerialTcpSocketConnector::OnDisconnect()
{
    printf("CTeleSerialTcpSocketConnector::OnDisconnect()\n");
}

void CTeleSerialTcpSocketConnector::OnConnectTimeout()
{
    printf("CTeleSerialTcpSocketConnector::OnConnectTimeout()\n");
}

void CTeleSerialTcpSocketConnector::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG_CODE( printf("CTeleSerialTcpSocketConnector::OnCommand, len:%d\n", ulCmdDataLen); );
    // 测试用， 关闭自己
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='q')
    {
        SetCloseAndDelete();
        m_b_connected = false;

        DBG_CODE( printf("CTeleSerialTcpSocketConnector::OnCommand q, SetCloseAndDelete\n"); );
    }

    // 测试用， echo
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='e')
    {
        char strEchoBuf[128];
        sprintf(strEchoBuf, "CTeleSerialTcpSocketConnector echo,cur time:%ld\n", time(NULL));
        SendBuf(strEchoBuf, strlen(strEchoBuf)+1 );
    }
    //////////
}

    #ifdef TeleSerial_FUNC_DATAPARSE_EXAMPLE
// 通用头 pCmd
// 数据 pData， 长度见 pCmd->u8DataLen
void CTeleSerialTcpSocketConnector::OnCommand_WifiAgent(WIFI_AGENT_PROTOCOL_HEAD *pCmd, unsigned char *pData)
{
    CTeleSerialTcpSocket::OnCommand_WifiAgent(pCmd, pData);
    printf("CTeleSerialTcpSocketConnector::OnCommand_WifiAgent\n");
}
#endif
