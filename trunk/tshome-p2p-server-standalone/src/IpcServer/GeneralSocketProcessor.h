/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralSocketProcessor.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralSocketProcessor.h 5884 2012-07-18 04:13:17Z WuJunjie $
 *
 *  Explain:
 *     -单独的消息处理对象-
 *
 *  Update:
 *     2012-07-18 04:13:17  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _GENERALSOCKETPROCESSOR_H
#define _GENERALSOCKETPROCESSOR_H

// pj
#include <errno.h>
#include "app.h"

#include "EZThread.h"
#include "SocketMsgQue.h"
#include <Socket.h>
#include "GeneralAgentUdpSocket.h"

// ipc最大包
#define IPC_DATA_MAX_SIZE 1024

#define g_GeneralSocketProcessor (*CGeneralSocketProcessor::instance())
class CGeneralSocketProcessor : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CGeneralSocketProcessor);

    BOOL Start();
    BOOL Stop();
    void ThreadProc();

    BOOL SendSocketMsg(unsigned int	 iMsg,Socket *pSocket=NULL, const char *pData=NULL,size_t lDataLen=0,struct sockaddr *sa_from=NULL,socklen_t sa_len=0);
    BOOL RecvSocketMessage (SOCKET_MSG_NODE *pMsg, BOOL wait = TRUE);

    // 向所有webserver发送数据
    int Send2Webs(unsigned int	 iMsg, const char *pData, size_t lDataLen);
    // 发送数据
    int Send2(Socket *pSocket, const char *pData, size_t lDataLen, struct sockaddr *sa_from,socklen_t sa_len);

    int IpcHandler(unsigned int	 iMsg,Socket *pSocket, const char *pData,size_t lDataLen,struct sockaddr sa_from,socklen_t sa_len);
    void SetPJ_turn_srv(pj_turn_srv *);
    /**added by sunzq*/
    pj_turn_srv * GetPJ_server();
    ipc_handler * GetIPC_hander();

  	int sendDeviceCtrl(char *ip, const int port, device_control_req *ctrl);
  	int sendDeviceCtrlEnocean(char *ip, const int port, device_control_req_enocean*ctrl);
	int sendHeartbeat(char *ip, const int port, heartbeat_req_t *req);

	int GetWebPeers(std::vector<IPCPEER_T> &dstPeer, std::string strHostID = "");
private:
    CGeneralSocketProcessor();
    virtual ~CGeneralSocketProcessor();
    CSocketMsgQue* m_pSocketMsgQue;

    pj_turn_srv *m_pPJ_TURN_SRV;
    ipc_handler m_pHandler;

    // 少量内存直接用栈空间
    char m_bufIPCBuffer[IPC_DATA_MAX_SIZE*2];
};
#endif // _GENERALSOCKETPROCESSOR_H
