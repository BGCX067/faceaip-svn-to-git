   //
//  UDPManager.h
//  tsbus_knx
//
//  Created by kyy on 11-5-20.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#ifndef _UDPMANAGER_H
#define _UDPMANAGER_H

#include "common/thread.h"
#include "common/remote_def.h"
#include "KNXCondef.h"
#include "NetworkIface.h"

#define BUFFER_SIZE		4096
int RecvProcess(void *pParam);

class DECLSPEC_DLL_EXPORT CUDPManager
{
public:
	CUDPManager();
	virtual ~CUDPManager();
	virtual int InitSocket(char *localIp, int nClientPort);					// init socket
	virtual int InitServer(char * strServerIp, int nServerPort);			// init server
    void SetKnxPtr (NetworkIface *pKnxPtr);									// pointer of knx kernel
	int  UDPSend (BYTE *pSendBuf, int nLen);								// send udp message
    int  UDPRecv (BYTE *pRecvBuf, int nLen);								// receive udp message
    int  UDPClose ();														// close udp

	bool IsRunning();
    void SetRunning(bool isRun);
	
	struct sockaddr_in GetServerAddr();
	struct sockaddr_in GetClientAddr(); 
	int GetClientSocket();
	
	NetworkIface * GetKnxPtr();
	
	BYTE *GetRecvBuf();	
	void RecvBufFlush();
	
	virtual int StartRecvThread();
	virtual int EndRecvThread();
	THREAD_HANDLE GetRecvThreadHandle();

	char *GetServerIp();
	int GetServerPort();
	int GetClientPort();
	
	void SetServerIp(char *pBuf);
	void SetServerPort(int nPort);
	void SetClientPort(int nPort);
	
protected:
	// about udp socket
	char m_strServerIp[MAX_IP_LEN];
	int m_nServerPort;
	int m_nClientPort;
	struct sockaddr_in m_serverAddr;
	struct sockaddr_in m_clientAddr;
	struct sockaddr_in m_recvAddr;
	int m_nClientSocket;
	
	//multicast
	struct ip_mreq m_mreq;
	
	// interface--callback function pointer
	NetworkIface * m_pKnxPtr;
	
	// members about cyclical receving processing thread
	THREAD_HANDLE	m_hRecvThread;
	BYTE			m_pRecvBuf[BUFFER_SIZE];
	
	bool			m_bRunProcess;
	
public:
	bool			m_bWaitRecvStart;
	THREAD_MUTEX	m_bufMutex;
};

struct CallbackStruct
{
	CallbackStruct()
	{
		pUdpMan = NULL;
		pBuf = NULL;
		nBufLen = 0;
	}
	
	CUDPManager *pUdpMan;
	BYTE *pBuf;
	int nBufLen;
};

#endif
