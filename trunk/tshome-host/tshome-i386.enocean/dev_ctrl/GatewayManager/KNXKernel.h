   //
//  KNXKernel.h
//  tsbus_knx
//
//  Created by kyy on 11-5-20.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#ifndef _KNXKERNEL_H
#define _KNXKERNEL_H

#include "UDPMulticast.h"
#include "UDPUnicast.h"

#include "KNXException.h"
#include "KNXCondef.h"
#include "knxdef.h"
#include "NetworkIface.h"
#include "DPTManager.h"

#include "NodeList.h"

class CGatewayManager;

int DetectBreath(void *pParam);
int KnxSendListProcess(void *pParam);
int KnxHandleListProcess(void *pParam);


typedef enum
{
	TIMER_MULTI = 0,
	TIMER_CONNECT,
	TIMER_CONNECTSTATE,
	TIMER_DISCONNECT,
	TIMER_TUNNEL
}Event_Timeout_Timer;

typedef enum
{
	BEAT_CONNECTSTATE = 0,
	BEAT_CONNECTSTATE_EXIT
}Beat_Cond;


	class DECLSPEC_DLL_EXPORT CKNXKernel : public NetworkIface
	{
	public:
		CUDPUnicast m_udpCtrl;
		CUDPUnicast m_udpData;
		CUDPMulticast m_udpMulti;
		
		// Event_Timeout timer
		TIMER_COND m_beatCond[2];
		TIMER_COND m_timerCond[5];
		THREAD_MUTEX m_timerMutex[5];
		
#ifdef _WIN32
		DWORD m_timeout[5];
#else
		struct timespec m_timeout[5];
#endif
		bool m_bWaitObject[5];
		int m_nStatus[5];
		bool m_bConStateOn;
		bool m_bExitBeat;
		
		// count the connection state timeout times
		int m_nStateTimeoutCount;
		
		BYTE m_conStateMsg[32];
		int m_nConStateMsgLen;
		
		// about request sending thread
		CNodeList m_sendList;
		bool m_bSendThread;
		
		THREAD_MUTEX m_SendListMutex;
		
		// read and write group request mutex
		THREAD_MUTEX m_TunnelRequest;
		THREAD_MUTEX m_UIRequest;
		
		// call back of tunnel request failed
		OnRWError m_pErrorFun;

	private:
		CDPTManager m_dptManager;
		CGatewayManager *m_pGatewayMan;

		//pointers of callbacks. see above "callback functions".
		GetKnxDpt m_pGetKnxDpt;
		OnSearchResponse m_pSearchResponse;
		OnReadResponse m_pReadResponse;
		OnWriteResponse m_pWriteResponse;
		OnKnxConnect m_pConnect;
		OnKnxDisconnect m_pDisconnect;
		
		// sequence counter
		int m_nInCounter;
		int m_nOutCounter;
		
		// current tunnel request
		int m_nCurAction;
		int m_nCurGroup;
		
		UINT nSockStartId;
		
		// server address ip/port
		char m_strLocalIp[MAX_IP_LEN];
		char m_strServerIp[MAX_IP_LEN];
		int m_nServerPort;

		//members about ctrl endpoint
		char m_strSearchServerIp[MAX_IP_LEN];
		int m_nSearchServerPort;
		int m_nChannelId;
		WORD m_nIndivAddrss;
		THREAD_HANDLE m_hBreathThread;
		
		//members about data endpoint
		char m_strDataServerIp[MAX_IP_LEN];
		int m_nDataServerPort;
		
		// sending thread handle
		THREAD_HANDLE m_hSendThread;

		// protocol handling thread
		THREAD_HANDLE m_hHandleThread;
		THREAD_MUTEX m_HandleListMutex;
		TIMER_COND m_CondHandleList;
		CNodeList m_handleList;
		bool m_bHandleThread;

		// Auto connect flag
		bool m_bAutoConnect;

		bool m_bConnectLock;
		int m_nReconnCount;

		OnNotifyKnxManager m_notify;
		
	public:
		CKNXKernel(int nKey, char * strLocalIp, char * strServerIp, int nPort, CGatewayManager *pGatewayMan);
		void Reset();
		virtual ~CKNXKernel();
		
		//set callbacks
		void SetCallBacks(
						  GetKnxDpt pGetKnxDpt,
						  OnSearchResponse pSearchResponse, 
						  OnReadResponse pReadResponse,
						  OnWriteResponse pWriteResponse,
						  OnKnxConnect pConnect,
						  OnKnxDisconnect pDisconnect,
						  OnRWError pErrorFun);

		void SetSearchResponse(OnSearchResponse pSearchResponse);

		void SetReConnCallBack(OnNotifyKnxManager notify) { m_notify = notify; }

		void SetServerAddr(char * serverIp, int serverPort);
		
		/***********************************************************************
		 **  Function : Search server to get the IP getway ip address,
						ctrl endpoint and data endpoint.
						need parameters: multicast ip address and port,
						there are "224.0.23.12" and "3671" default.
		 **    return : 0 if OK. must to catch the CKNXException exception.
		 ************************************************************************/		
		int SearchServer();
		
		/***********************************************************************
		 **  Function : Init the UDP socket & start the receiving thread.
		 **     Input : server IP, server port, local port
		 **    return : 0 if OK. must to catch the CKNXException exception.
		 ************************************************************************/
		int UDPSocketInit();
		int UDPSocketInit(char * serverIp, int serverPort);
		
		/***********************************************************************
		 **  Function : Send a message to request group value.
		 **     Input : main group address, middle group address, group address
		 **    return : 0 if OK. must to catch the CKNXException exception.
		 ************************************************************************/
		int ReadGrpVal (int nGroupAddr);
		
		/***********************************************************************
		 **  Function : Send a message to set group value.
		 **     Input : KNX_GroupData instance.
		 **    return : 0 if OK. must to catch the CKNXException exception.
		 ************************************************************************/
		int WriteGrpVal (void *pBuf, int nLen, int nPrefixLen);
		
		// To disconnect the current tunnel link.
		void ToDisconnect(bool bWaitResponse = true, bool bReCon = false, 
						  bool bStopDetectThread = true);
		
		void IncrementReconnCount();
		void ClearSendList();

		void ToReConnect(){ m_notify(); }
        
        void CallOnDisconnectCallBack()
        {
            if(m_pDisconnect != NULL)
			{
				m_pDisconnect(m_nGatewayKey);
			}
        }
		
		void WaitNull(int nMillSec);
		
	public:
		/***********************************************************************
		 **  Function : (Interface to UDP) When UDP module has received a message,
						it should to call this function.
		 **     Input : Buffer and length of received message. 
		 **    return : 0
		 ************************************************************************/
		virtual int ExecRecvMsgAction(BYTE *pRecvBuf, int nLen);

		int AddHandleListNode(int nResType, void *pBuf, int nLen);
		void ClearHandleList();
		
		// Callback when socket disconnected.
		virtual void ExecDisconnectAction() {};

		/***********************************************************************
		 **  Function : generatting the message respectively.
		 **     Input : [in]		Buffer
						[in/out]	buf length & message length.
		 **    return : 0
		 ************************************************************************/
		int GenerateConnectStateMsg(BYTE *pBuf, int &nLen);
		
		// about breathing detecting of ctrl endpoint		
		int ConnectStateTimer(int nTimeout = 60);
		
		void GetSearchIp(char *pBuf);
		int GetSearchPort();

		OnReadResponse GetReadRes() { return m_pReadResponse; }
		OnWriteResponse GetWriteRes() { return m_pWriteResponse; }
		CGatewayManager *GetGatewayManager() { return m_pGatewayMan; }
        
		int GetChannelId(){ return m_nChannelId;}
		int GetInCount(){ return m_nInCounter;}
		int GetOutCount(){ return m_nOutCounter;}

		// init & uninit win32sock
		void InitWin32Sock();
		void UnInitWin32Sock();
		
		// Do read and write request
		int DoReadGrpVal (int nGroupAddr);
		int DoWriteGrpVal (KNX_GroupData grpData);
		
		// About Re-connect
		bool IsAutoConnect() { return m_bAutoConnect; }
		void SetAutoConnect(bool bAuto) { m_bAutoConnect = bAuto; }

		void WaitForAllThreadsExit();
		
		THREAD_MUTEX * GetHandleListMutex() { return &m_HandleListMutex; }
		TIMER_COND & GetHandleListCond() { return m_CondHandleList; }
		CNodeList * GetHandleList() { return &m_handleList; }
		void SetHandleProcess(bool bRun) { m_bHandleThread = bRun; }
		bool IsHandleProcess() { return m_bHandleThread; }

		// do handle the legal frames
		// moving the pointer to a appropriate position
		// and getting these information from the byte buffer to local parameters.
		int HandleSearchResponse(BYTE *pHandle, int nLen);
		int HandleConnectResponse(BYTE *pHandle, int nLen);
		int HandleConnectStateResponse(BYTE *pHandle, int nLen);
		int HandleDisconnectResponse(BYTE *pHandle, int nLen);

	private:
		// when disconnected
		void Disconnection(bool bReconnect = false, bool bStopDetectThread = true);
		
	private:
		/***********************************************************************
		 **  Function : generatting the message respectively.
		 **     Input : [in]		Buffer
						[in/out]	buf length & message length.
		 **    return : 0
		 ************************************************************************/
		int GenerateSearchMsg(BYTE *pBuf, int &nLen);
		int GenerateConnectMsg(BYTE *pBuf, int &nLen);
		int GenerateDisConnectMsg(BYTE *pBuf, int &nLen);
		int GenerateReadValueMsg(BYTE *pBuf, int nGroupAddr, int &nLen);
		int GenerateWriteValueMsg(BYTE *pBuf, KNX_GroupData &grpData, int &nLen, WORD &wDesAddr);
		int GenerateTunnelAckMsg(BYTE *pBuf, int &nLen, 
								 int nStatus = E_NO_ERROR, bool bWithThisCount = false, int nThisCount = 0);
		int SendTunnelAck(int nStatus = E_NO_ERROR, bool bWithThisCount = false, int nThisCount = 0);
		
		
		int GroupValueRequest(BYTE *pBuf, int nLen, WORD wAddr, int nCurAction);
		int HandleTunnelResponse(BYTE *pHandle, int nLen);
		int HandleTunnelAck(BYTE *pHandle, int nLen);
		int HandleReadResponse(BYTE *pHandle, int nLen);
		int HandleWriteResponse(BYTE *pHandle, int nLen);
		
		// about breathing detecting of ctrl endpoint
		int StartDetectThread();
		int EndDetectThread();

		int StartSendThread();
		int EndSendThread(bool bDelAll);

		int StartHandleThread();
		int EndHandleThread(bool bDelAll);

		void SetLocalIp(char * localIp);
		void GetLocalIp(char * ip);

		//Generate the odd parity code. (NOT XOR)
		BYTE GetOddParity(BYTE *pParityBuf, int nParityLen);

	};//end class CKNXKernel


#endif
