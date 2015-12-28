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
#ifndef _GENERALAGENTTCPSOCKETSERVER_H
#define _GENERALAGENTTCPSOCKETSERVER_H

#include "GeneralAgentTcpSocket.h"
#include <ISocketHandler.h>

class CGeneralAgentTcpSocketServer : public CGeneralAgentTcpSocket
{
public:
	CGeneralAgentTcpSocketServer(ISocketHandler& , bool bSendHost = true);

	virtual void OnAccept();
	virtual void OnDisconnect();
	virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);
	virtual void SendToHost(char *pBuf, int nLen);
	// ͨ��ͷ pCmd
	// ��� pData�� ���ȼ� pCmd->u8DataLen
	virtual void OnCommand_WifiAgent(remote_header_t *pCmd, unsigned char *pData);

	bool m_bSendHost;
};

#endif //_GENERALAGENTTCPSOCKETSERVER_H
