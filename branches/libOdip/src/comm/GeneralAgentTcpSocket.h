/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocket.h - _explain_
 *
 * Copyright (C) 2011 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocket.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -tcp 统一写，便于统一协议解析-
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
#include "CommonInclude.h"
#include <iostream>
#include <string.h>
#include "Def_ODIP.h"
#include "base64.h"
#include "ezBuffer.h"
#include "des.h"
#include "md5.h"

#ifdef INTERNAL_ODIP_DATA_PARSE
#define ODIP_PROTOCOL_DATA_TIMEOUT 10 // SECOND
#define ODIP_PROTOCOL_DATA_MAX_LEN 1024*9 // 最大 包
#endif //INTERNAL_ODIP_DATA_PARSE

class CGeneralAgentTcpSocket : public TcpSocket
{
public:
    CGeneralAgentTcpSocket(ISocketHandler& , std::string strSocketName="GeneralAgentTcpSocket");
    
    // 从 TcpSocket::OnRead(); 中调用
    virtual void OnRawData(const char *buf,size_t len);
    // 统一command
    // 一个系统的协议体系应该是一样的， wujunjie 2012-4-9 15:35:31
    virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);

#ifdef INTERNAL_ODIP_DATA_PARSE
    // 统一read
    // 一个系统的协议体系应该是一样的， wujunjie 2012-4-9 15:35:31
    virtual void OnRead();
    // 通用头 pCmd
    // 数据 pData， 长度见 pCmd->u8DataLen
    virtual void OnCommand_ODIP(const ODIP_HEAD_T *pCmd, const unsigned char *pData);
private:
    //  保存上次未处理的头，
    //WIFI_AGENT_PROTOCOL_HEAD m_headLastOn;
	
    ODIP_HEAD_T m_headLastOn;
    // 下面的根据实际定义最大的
    unsigned char m_OdipDataBuffer[4096*2];
#endif

private:
    time_t m_tOnData; ///< last Time in seconds when this socket was on data



CezBuffer g_cabRecvBuff;
// 上次命令
//CezBuffer g_cabLastCMD;

//ODIP_HEAD_T g_Dvrip;

};
#endif // _GENERALAGENTTCPSOCKET_H
