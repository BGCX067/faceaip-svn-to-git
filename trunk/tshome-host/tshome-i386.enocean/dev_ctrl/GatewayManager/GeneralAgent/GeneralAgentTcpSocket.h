/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocket.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocket.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _GENERALAGENTTCPSOCKET_H
#define _GENERALAGENTTCPSOCKET_H

#include <stdio.h>

#include <TcpSocket.h>
#include <ISocketHandler.h>
#include "common/remote_def.h"

//#include "../WifiKernel.h"
class CWifiKernel;

//!wifi ���Э�飬 ͨ�ò���
//typedef struct __wifi_agent_protocol_head
//{
//	unsigned char u8CmdOrStatus;
//	unsigned char u8AddOrCommand;
//	unsigned char u8DataLen;
//}
//WIFI_AGENT_PROTOCOL_HEAD;

#define WIFI_AGENT_PROTOCOL_DATA_TIMEOUT 10 // SECOND
#define WIFI_AGENT_PROTOCOL_DATA_MAX_LEN 64 // ��󻺴�

class CGeneralAgentTcpSocket : public TcpSocket
{
public:
	CGeneralAgentTcpSocket(ISocketHandler& );	
	virtual void OnRawData(const char *buf,size_t len);

	// ͳһread
	// һ��ϵͳ��Э����ϵӦ����һ��ģ� wujunjie 2012-4-9 15:35:31
	virtual void OnRead();
	// ͳһcommand
	// һ��ϵͳ��Э����ϵӦ����һ��ģ� wujunjie 2012-4-9 15:35:31
	virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);
	
	// ͨ��ͷ pCmd
	// ��� pData�� ���ȼ� pCmd->u8DataLen
//	virtual void OnCommand_WifiAgent(WIFI_AGENT_PROTOCOL_HEAD *pCmd, unsigned char *pData);
	virtual void OnCommand_WifiAgent(remote_header_t *pCmd, unsigned char *pData);

	virtual void SetWifiPtr(CWifiKernel *pWifi);
	
protected:
	//  �����ϴ�δ�����ͷ�� 
	remote_header_t m_headLastOn;
	time_t m_tOnData; ///< last Time in seconds when this socket was on data

	CWifiKernel *m_pWifi;
};
#endif // _GENERALAGENTTCPSOCKET_H
