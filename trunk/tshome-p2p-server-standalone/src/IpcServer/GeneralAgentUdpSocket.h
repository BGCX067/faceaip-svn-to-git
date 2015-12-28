/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentUdpSocket.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentUdpSocket.h 5884 2012-07-19 04:20:52Z WuJunjie $
 *
 *  Explain:
 *     -udp实例-
 *
 *  Update:
 *     2012-4-11 9:32:49 WuJunjie Create
 *     2012-7-19 16:19:02 支持向处理线程发送消息的模型
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _GENERALAGENTUDPSOCKET_H
#define _GENERALAGENTUDPSOCKET_H

#include <UdpSocket.h>

#define CGENERALAGENTUDPSOCKET_SOCKET_NAME "CGeneralAgentUdpSocket"

// 客户端失效时间 30分钟
#define INVALID_PEER_TIMEOUT 60*30
// 单元测试用， 20s
//#define INVALID_PEER_TIMEOUT 20
class IPCPEER_T
{
public:
	std::string strName;// 暂时无用
	struct sockaddr PeerAddr;
	socklen_t PeerAddrLen;
	time_t ttLastOnData;
	IPCPEER_T()
	{
		strName = "";
		memset(&PeerAddr, 0, sizeof(struct sockaddr));
		PeerAddrLen = sizeof(struct sockaddr);
		
		ttLastOnData = 0;
	}
	~IPCPEER_T()
	{;}
	
};


class CGeneralAgentUdpSocket : public UdpSocket
{
public:
	CGeneralAgentUdpSocket(ISocketHandler&, std::string strSocketName=CGENERALAGENTUDPSOCKET_SOCKET_NAME);
	// 内部维护数据
	void Update();

	void OnRawData(const char *,size_t,struct sockaddr *,socklen_t);

	// 向所有已知客户端发送消息
	int SendtoAllPeer( const char *pData, size_t lDataLen);

	// ret >= 0 peer num; <0 error
	int GetPeer(std::vector<IPCPEER_T> &dstPeer);
	//int GetPeer(IPCPEER_T &dstPeer, std::string strHostID);
	//int GetPeer(std::vector<IPCPEER_T> &dstPeer, std::string strHost, unsigned short usPort);
	
 	/** Outgoing traffic counter. */
	virtual uint64_t GetBytesSent(bool clear = false);

	/** Incoming traffic counter. */
	virtual uint64_t GetBytesReceived(bool clear = false);

private:
	void OnPeer(struct sockaddr *pPeer=NULL, socklen_t iSaLen=0);

	// 存放同ipc 通讯的所有客户端
	std::vector<IPCPEER_T> m_IpcPeer;
	std::vector<IPCPEER_T>::iterator it;

	uint64_t m_BytesSent;
	uint64_t m_BytesReceived;
};

#endif // _GENERALAGENTUDPSOCKET_H
