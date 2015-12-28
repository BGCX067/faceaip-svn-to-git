   //
//  UDPManager.mm
//  tsbus_knx
//
//  Created by kyy on 11-5-20.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#include "KNXException.h"


#include "UDPManager.h"
#include "knxdef.h"
#include "NodeList.h"

#include <stdio.h>
#include <string.h>

#include<errno.h>
#include <time.h>

#ifndef _WIN32
#include <sys/fcntl.h>
#endif


CUDPManager::CUDPManager()
{
	memset(m_strServerIp, 0, sizeof(m_strServerIp));
	m_hRecvThread = 0;
	
	m_nServerPort = 0;
	m_nClientSocket = 0;
	m_nClientPort = 0;
	
	m_bRunProcess = false;
	m_pKnxPtr = 0;
	m_bWaitRecvStart = false;

	INITIALIZE_MUTEX(&m_bufMutex);
}

CUDPManager::~CUDPManager()
{
	UDPClose();
	DESTROY_MUTEX(&m_bufMutex);
}

int HandleRecvMsg(void *pParam)
{
	CallbackStruct *pStruct = (CallbackStruct *) pParam;
	int nHandleLen = 0;
	CUDPManager *pUdpMan = (CUDPManager *) pStruct->pUdpMan;
	NetworkIface *pKnxPtr = pUdpMan->GetKnxPtr();

	if(pKnxPtr != 0)
	{
		nHandleLen = pKnxPtr->ExecRecvMsgAction(pStruct->pBuf, pStruct->nBufLen);
	}
	
//	delete pStruct->pBuf;
	pStruct->pBuf = 0;

	delete pStruct;
	pStruct = 0;
	
	return nHandleLen;
}

class CKNXKernel;

// callback function of recveiving udp message thread.
int RecvProcess(void *pParam)
{
	if(pParam == 0)
	{
		return -1;
	}
	
	printf("Recv Thread Start \n");

	CUDPManager *pUDPMan = (CUDPManager *) pParam;
	int nRecvLen = 0;
	NetworkIface *pKnxPtr = 0;
	pUDPMan->m_bWaitRecvStart = true;
	
	int nRes = 0;
	fd_set set;
	struct timeval timeout;

	while (pUDPMan->IsRunning())
	{
#ifdef _WIN32
		set.fd_count = 1;
		set.fd_array[0] = pUDPMan->GetClientSocket();
		nRes = select(0, &set, 0, 0, 0);
#else
		FD_ZERO(&set);
		FD_SET(pUDPMan->GetClientSocket(), &set);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		nRes = select(pUDPMan->GetClientSocket() + 1, &set, 0, 0, &timeout);
#endif

		if(nRes < 0)
		{
			// error, to exit
			break;
		}
		else if(nRes == 0)
		{
			// timed out
			continue;
		}

	//	printf("I am try to receive from %s:%d \n", inet_ntoa(pUDPMan->GetClientAddr().sin_addr), pUDPMan->GetClientPort());
		pUDPMan->RecvBufFlush();

		nRecvLen = pUDPMan->UDPRecv(pUDPMan->GetRecvBuf(), BUFFER_SIZE);

		if(nRecvLen < 0)
		{
			break;
		}
		else if(nRecvLen == 0)
		{
			continue;
		}

		
//#ifdef _WIN32
//		char str[32] = {'\0'};
//		sprintf(str, " [%d] recvLen:%d\n",
//			pUDPMan->GetKnxPtr()->GetGatewayKey(),
//			nRecvLen);
//		OutputDebugStringA(str);
//#else
//		printf(" [%d] recvLen:%d\n",
//			pUDPMan->GetKnxPtr()->GetGatewayKey(),
//			nRecvLen);
//#endif
		
//		printf("----------------from udp recv thread recv len = %d----------------\n", nRecvLen);
		
		pKnxPtr = pUDPMan->GetKnxPtr();
		
		if(pKnxPtr != 0)
		{		
/*
			CallbackStruct *pStruct = new CallbackStruct;
			BYTE *pBuf = new BYTE;
			pStruct->pUdpMan = pUDPMan;
			pStruct->pBuf = pBuf;
			memcpy(pBuf, pUDPMan->GetRecvBuf(), nRecvLen);
			pStruct->nBufLen = nRecvLen;
			CREATE_THREAD(HandleRecvMsg, pStruct);
*/

			pKnxPtr->ExecRecvMsgAction(pUDPMan->GetRecvBuf(), nRecvLen);

		}
	}
	
#ifdef _WIN32
	OutputDebugString(_T("--------Recv Thread END--------\n"));
#else
	printf("--------Recv Thread END--------\n");
#endif
	return 0;
}

int CUDPManager::StartRecvThread()
{
	return -1;
}

int CUDPManager::EndRecvThread()
{
	m_bRunProcess = false;

	if(m_hRecvThread > 0)
	{
		WAIT_THREAD_EXIT(m_hRecvThread);
		m_hRecvThread = 0;
	}

/*
	if (m_hRecvThread)
	{
#ifdef _WIN32
		WAIT_THREAD_EXIT(m_hRecvThread);
		CLOSE_THREAD(m_hRecvThread);
#else

		printf("pthread_cancel \n");
		pthread_cancel(m_hRecvThread);
		printf("after pthread_cancel \n");
		WAIT_THREAD_EXIT(m_hRecvThread);
		printf("after WAIT_THREAD_EXIT \n");
#endif
		m_hRecvThread = 0;
	}
*/
	
	return 0;
}

// init socket
int  CUDPManager::InitSocket(char *localIp, int nClientPort)
{
	if(!(nClientPort > 0 && nClientPort < 65536))
	{
		return -1;
	}

	if(m_nClientSocket > 0)
	{
		UDPClose();
	}
	
	m_bRunProcess = false;
	m_nClientPort = nClientPort;
	
	
	m_clientAddr.sin_family = AF_INET;
	m_clientAddr.sin_port = htons(m_nClientPort);
#ifdef WIN32
	m_clientAddr.sin_addr.s_addr = inet_addr(localIp);//INADDR_ANY;//
#else
	m_clientAddr.sin_addr.s_addr = INADDR_ANY;//inet_addr(localIp);//
	bzero(&(m_clientAddr.sin_zero), 8);
#endif

	m_nClientSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if(m_nClientSocket == -1)
	{
		perror("m_nClientSocket create error");
//		DEBUG_LOG1(UDPManager, create socket error:%d\n, errno);
		return -1;
	}

#ifdef _WIN32
	int nMode = 1;
	if(ioctlsocket(m_nClientSocket, FIONBIO, (u_long*) &nMode) != 0)
	{
		return -1;
	}
#endif

	// deleted by keyy 2012/03/22
	// look my tiansu note book
//	int flag = 1;
//	setsockopt(m_nClientSocket, SOL_SOCKET, SO_REUSEADDR, (const char *) &flag, sizeof(int));
	
	if(bind(m_nClientSocket, (struct sockaddr *) &m_clientAddr,	sizeof(m_clientAddr)) != 0)
	{
#ifdef WIN32
		//shutdown(m_nClientSocket, SD_BOTH);
		closesocket(m_nClientSocket);
#else
		//shutdown(m_nClientSocket, SHUT_RDWR);
		close(m_nClientSocket);
#endif
		printf("m_nClientSocket(%d) Bind failed(%d) \n", m_nClientSocket, errno);
		perror("why ");
		return -1;
	}
	
	return 0;
}

// init server
int  CUDPManager::InitServer(char * strServerIp, int nServerPort)
{
	return -1;
}

// set the pointer of the KNXUdpIface's sub-class.
void CUDPManager::SetKnxPtr (NetworkIface *pKnxPtr)
{
	m_pKnxPtr = pKnxPtr;
}

// send udp message
int  CUDPManager::UDPSend (BYTE *pSendBuf, int nLen)
{
	if (!IsRunning())
	{
		return -1;
	}
	
	int nSendLen = 0;
	
#ifdef WIN32
	int nAddrLen = 0;
#else
	socklen_t nAddrLen = 0;
#endif
	nAddrLen = sizeof(m_serverAddr);

	int nSendCount = 0;
	
	while(nSendCount++ < 3)
	{
#ifdef WIN32
		nSendLen = sendto(m_nClientSocket, (const char *)pSendBuf, nLen , 0, 
						  (struct sockaddr *) &m_serverAddr, nAddrLen);
#else
		nSendLen = sendto(m_nClientSocket, pSendBuf, nLen , 0, 
						  (struct sockaddr *) &m_serverAddr, nAddrLen);
#endif
		
		if(nSendLen == nLen)
			break;
	}
	
	if(nSendLen != nLen)
	{
		return -1;
	}
	
	return nSendLen;
}

// receive udp message.
// the function does not throw exception
// because the function will be used in a sub thread
int  CUDPManager::UDPRecv (BYTE *pRecvBuf, int nLen)
{
	if(!IsRunning())
	{
		return -1;
	}
	
	int nRecvLen = 0;
#ifdef _WIN32
	int nAddrLen = 0;
	nAddrLen = sizeof(m_recvAddr);
	nRecvLen = recvfrom(m_nClientSocket, (char *)pRecvBuf, nLen, 0,
						(struct sockaddr *) &m_recvAddr, &nAddrLen);
#else
	socklen_t nAddrLen = 0;
	nAddrLen = sizeof(m_recvAddr);
	nRecvLen = recvfrom(m_nClientSocket, pRecvBuf, nLen, 0,
						(struct sockaddr *) &m_recvAddr, &nAddrLen);
#endif
	return nRecvLen;
}

void CUDPManager::RecvBufFlush()
{
	LOCK_MUTEX(&m_bufMutex);
	memset(m_pRecvBuf, 0, sizeof(m_pRecvBuf));
	UNLOCK_MUTEX(&m_bufMutex);
}

bool CUDPManager::IsRunning()
{
//	printf("... %d ...Is Running ? %d......\n", m_nClientPort, m_bRunProcess ? 1 : 0);
	return m_bRunProcess;
}

void CUDPManager::SetRunning(bool isRun)
{
    m_bRunProcess = isRun;
}

THREAD_HANDLE CUDPManager::GetRecvThreadHandle()
{
	return m_hRecvThread;
}

char *CUDPManager::GetServerIp()
{
	return m_strServerIp;
}

int CUDPManager::GetServerPort()
{
	return m_nServerPort;
}

int CUDPManager::GetClientPort()
{
	return m_nClientPort;
}

void CUDPManager::SetServerIp(char *pBuf)
{
	strncpy(m_strServerIp, pBuf, MAX_IP_LEN);
}

void CUDPManager::SetServerPort(int nPort)
{
	m_nServerPort = nPort;
}

void CUDPManager::SetClientPort(int nPort)
{
	m_nClientPort = nPort;
}

struct sockaddr_in CUDPManager::GetServerAddr()
{
	return m_serverAddr;
}

struct sockaddr_in CUDPManager::GetClientAddr()
{
	return m_clientAddr;
}

int CUDPManager::GetClientSocket()
{
	return m_nClientSocket;
}

NetworkIface * CUDPManager::GetKnxPtr()
{
	return m_pKnxPtr;
}

BYTE *CUDPManager::GetRecvBuf()
{
	LOCK_MUTEX(&m_bufMutex);
	BYTE *pBuf = m_pRecvBuf;
	UNLOCK_MUTEX(&m_bufMutex);
	return pBuf;
}

// close udp
int  CUDPManager::UDPClose ()
{	
	if(m_nClientSocket != -1)
	{
#ifdef WIN32
		shutdown(m_nClientSocket, SD_BOTH);
		closesocket(m_nClientSocket);
#else
		shutdown(m_nClientSocket, SHUT_RDWR);
		close(m_nClientSocket);
#endif
		m_nClientSocket = -1;
	}
	
	if(m_bRunProcess)
	{
		EndRecvThread();
	}
	
	return 0;
}
