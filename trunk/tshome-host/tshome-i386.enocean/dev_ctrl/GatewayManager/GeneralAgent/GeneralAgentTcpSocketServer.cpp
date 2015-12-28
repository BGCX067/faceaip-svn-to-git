/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocketServer.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocketServer.cpp 0001 2012-04-06 09:58:37Z WuJunjie $
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

#include "GeneralAgentTcpSocketServer.h"
#include "../WifiKernel.h"
#include "../GatewayManager.h"

CGeneralAgentTcpSocketServer::CGeneralAgentTcpSocketServer(ISocketHandler& h, bool bSendHost)
        :CGeneralAgentTcpSocket(h)
{
    SetDeleteByHandler();
    m_bSendHost = bSendHost;
}

void CGeneralAgentTcpSocketServer::OnAccept()
{
    printf("CGeneralAgentTcpSocketServer::OnAccept:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}
void CGeneralAgentTcpSocketServer::OnDisconnect()
{
    printf("CGeneralAgentTcpSocketServer::OnDisconnect()\n");
}

void CGeneralAgentTcpSocketServer::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG_CODE( printf("CGeneralAgentTcpSocketServer::OnCommand, len:%d\n", ulCmdDataLen); );
    // �����ã� �ر��Լ�
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='q')
    {
        SetCloseAndDelete();

        DBG_CODE( printf("CGeneralAgentTcpSocketServer::OnCommand q, SetCloseAndDelete\n"); );
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
void CGeneralAgentTcpSocketServer::OnCommand_WifiAgent(remote_header_t *pCmd, unsigned char *pData)
{
    CGeneralAgentTcpSocket::OnCommand_WifiAgent(pCmd, pData);
    printf("CGeneralAgentTcpSocketServer::OnCommand_WifiAgent\n");

    CGatewayManager *pGatwayMan = CGatewayManager::Instance();
    int nHeadLen = sizeof(remote_header_t);
    int nDataLen = ntohs(pCmd->length);
    int nTotalLen = nHeadLen + nDataLen;

    char *pNode = new char[nTotalLen];
    memcpy(pNode, pCmd, nHeadLen);
    if(pData != NULL)
    {
     	memcpy(pNode + nHeadLen, pData, nDataLen);
	}

    if(pGatwayMan->m_pServerLogical != NULL)
    {
    	pGatwayMan->m_pServerLogical->AddRequestNode(IC_WIFI_SERVER, pNode, nTotalLen);
    }
	delete pNode;
	pNode = NULL;
}

void CGeneralAgentTcpSocketServer::SendToHost(char *pBuf, int nLen)
{
	if (m_bSendHost)
	{
		Send(pBuf, nLen);
	}
}
