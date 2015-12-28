/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocketServer.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocketServer.h 0001 2012-04-06 09:58:28Z WuJunjie $
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
#include "GeneralAgentTcpSocketConnector.h"

#include "common/ts_err_code.h"
#include "../WifiKernel.h"
#include "../GatewayManager.h"

CGeneralAgentTcpSocketConnector::CGeneralAgentTcpSocketConnector(ISocketHandler& h)
        :CGeneralAgentTcpSocket(h)
        ,m_b_connected(false)
{
	m_pWifi = NULL;

    // initial connection timeout setting
    SetConnectTimeout(5);

    SetDeleteByHandler();
}

CGeneralAgentTcpSocketConnector::~CGeneralAgentTcpSocketConnector()
{}

CGeneralAgentTcpSocketConnector *CGeneralAgentTcpSocketConnector::Reconnect()
{
    std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
    CGeneralAgentTcpSocketConnector *p = new CGeneralAgentTcpSocketConnector(Handler());
    p -> SetDeleteByHandler();
    p -> Open(*ad);
    Handler().Add(p);
    return p;
}


void CGeneralAgentTcpSocketConnector::OnConnectFailed()
{
    CGeneralAgentTcpSocketConnector *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CGeneralAgentTcpSocketConnector::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}

void CGeneralAgentTcpSocketConnector::SetCloseAndDelete()
{
    CGeneralAgentTcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CGeneralAgentTcpSocketConnector::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    // do not modify, wujj 2012-4-9 15:31:44 end

    printf("CGeneralAgentTcpSocketConnector::OnConnect()\n");
    if(m_pWifi != NULL)
    {
    	m_pWifi->OnWifiConnected(m_b_connected ? SUCCESS : ERR_PROTROL_CONNECT_FAILED);
    }
}
void CGeneralAgentTcpSocketConnector::OnReconnect()
{
    printf("CGeneralAgentTcpSocketConnector::OnReconnect()\n");
    if(m_pWifi != NULL)
    {
    	m_pWifi->OnWifiConnected(m_b_connected ? SUCCESS : ERR_PROTROL_CONNECT_FAILED);
    }
}

void CGeneralAgentTcpSocketConnector::OnDisconnect()
{
    printf("CGeneralAgentTcpSocketConnector::OnDisconnect()\n");
    if(m_pWifi != NULL)
    {
    	m_pWifi->OnWifiDisconnected();
    }
}

void CGeneralAgentTcpSocketConnector::OnConnectTimeout()
{
    //printf("CGeneralAgentTcpSocketConnector::OnConnectTimeout()\n");
    if(m_pWifi != NULL)
    {
    	m_pWifi->OnWifiConnected(ERR_PROTROL_TIMEOUT);
    }
}

void CGeneralAgentTcpSocketConnector::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG_CODE( printf("CGeneralAgentTcpSocketConnector::OnCommand, len:%d\n", ulCmdDataLen); );
    // �����ã� �ر��Լ�
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='q')
    {
        SetCloseAndDelete();
        m_b_connected = false;

        DBG_CODE( printf("CGeneralAgentTcpSocketConnector::OnCommand q, SetCloseAndDelete\n"); );
    }

    // �����ã� echo
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='e')
    {
        char strEchoBuf[128];
        sprintf(strEchoBuf, "Get echo command, cur time:%ld\n", time(NULL));
        SendBuf(strEchoBuf, strlen(strEchoBuf)+1 );
    }
    //////////
}

// ͨ��ͷ pCmd
// ��� pData�� ���ȼ� pCmd->u8DataLen
void CGeneralAgentTcpSocketConnector::OnCommand_WifiAgent(remote_header_t *pCmd, unsigned char *pData)
{
    CGeneralAgentTcpSocket::OnCommand_WifiAgent(pCmd, pData);
    //printf("CGeneralAgentTcpSocketConnector::OnCommand_WifiAgent\n");

    int nHeadLen = sizeof(remote_header_t);
    size_t nDataLen = ntohs(pCmd->length);
    int nTotalLen = nHeadLen + nDataLen;
    if(m_pWifi != NULL)
    {
        char *pRes = new char[nTotalLen];
        memcpy(pRes, pCmd, nHeadLen);
        if(pData != NULL)
        {
            memcpy(pRes + nHeadLen, pData, nDataLen);
        }

        // if set_signal ok, client will 
        kic_head_t * pKicHead = (kic_head_t *) pRes;
		bool bFlag = m_pWifi->GetLogicalPtr()->SetTcpCSignal(pKicHead);
		if(bFlag == true)
		{
			return ;
		}
		
        if( m_pWifi->GetLogicalPtr() != NULL )
        {
        	if(pKicHead->header.comm_type == REQUEST || pKicHead->header.comm_type == RESPONSE)
       		{
           		CGatewayManager::Instance()->GetCtrlResponse()(
            			ACTION_WIFI_RESPONSE, pRes, nTotalLen);
        	}
        }
		
		// to add a scene input node
		if(pKicHead->header.comm_type == REQUEST
			|| pKicHead->header.comm_type == RESPONSE
			/*|| pKicHead->header.comm_type == SERIAL_RES*/)
		{
			m_pWifi->GetLogicalPtr()->ToAddSceneInputNode(pKicHead);
		}

    	delete pRes;
    	pRes = NULL;
    }
}

