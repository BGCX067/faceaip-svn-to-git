/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocketServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocketServer.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -tcp服务器实例-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _GENERALAGENTTCPSOCKETSERVER_H
#define _GENERALAGENTTCPSOCKETSERVER_H

#include "TeleSerialTcpSocket.h"
#include <ISocketHandler.h>

class CTeleSerialTcpSocketServer : public CTeleSerialTcpSocket
{
public:
	CTeleSerialTcpSocketServer(ISocketHandler& , std::string strSocketName="TeleSerialTcpSocketServer");
	
	virtual void OnAccept();
	virtual void OnDisconnect();
	virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);
#ifdef TeleSerial_FUNC_DATAPARSE_EXAMPLE
	// 通用头 pCmd
	// 数据 pData， 长度见 pCmd->u8DataLen
	virtual void OnCommand_WifiAgent(WIFI_AGENT_PROTOCOL_HEAD *pCmd, unsigned char *pData);
#endif
};

#endif //_GENERALAGENTTCPSOCKETSERVER_H
