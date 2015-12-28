//
//  GatewayMananger.h
//  TSBusStack
//
//  Created by Gengh on 11-08-20.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#ifndef _GATEWAYMANAGER_H
#define _GATEWAYMANAGER_H

#ifdef _WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4251)
#endif


#include "KNXKernel.h"
#include "WifiKernel.h"
#include "ezserial.h"
#include "SceneKernel.h"

#include <map>

//#ifdef DEV_ENOCEAN_SUPPORT
#include "enocean/enocean.h"
//#endif


#define MAX_ENOCEAN_COUNT		20


#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#include <time.h>
#ifndef _DEBUG_THIS
	#define _DEBUG_THIS
#endif

#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x) x
	#define DBG(x)
#endif


/*
#define LOG_TO_CLI
#ifdef LOG_TO_CLI
	#define __PRINTF(...) printf(...)
#elif define LOG_TO_FILE
	#define __PRINTF(x) fprintf(g_log, x)
#else
	#define __PRINTF(x)
#endif
*/

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif
#ifndef __rnline
	#define __rnline printf("%s(%d)\n", __FILE__, __LINE__);
#endif

typedef std::pair<int, CWifiKernel*> WIFI_GATEWAY_PAIR;
typedef std::map<int, CWifiKernel*> WIFI_GATEWAY_MAP;
typedef std::map<int, CWifiKernel*>::iterator WIFI_GATEWAY_MAP_ITER;

typedef std::pair<int, CKNXKernel*> KNX_GATEWAY_PAIR;
typedef std::map<int, CKNXKernel*> KNX_GATEWAY_MAP;
typedef std::map<int, CKNXKernel*>::iterator KNX_GATEWAY_MAP_ITER;


#include "../ComProtocolManager/IrcProtocolIface.h"
#include "db/ts_db_protocol_infos.h"

typedef CIrcProtocolIface * (*CreateSoInstance) ();
typedef void (*DeleteSoInstance)(CIrcProtocolIface *p);
/*
typedef void (* VOID_CHARP_FUN) (char * strProtocolName);
typedef char * (* CHARP_VOID_FUN) ();
typedef void (* VOID_OnParsedData_FUN) (OnParsedData pOnParsedData);
typedef OnParsedData (* OnParsedData_VOID_FUN)();
//nComId = [0, (TOTAL_COM_NUM - 1)].
typedef void (* VOID_INT_VOIDP_INT_FUN) (int nComId, void *pBuf, int nLen);
typedef int (* INT_INT_CHARP_CHARP__INT_FUN)(int nFunType, char *pData, char *pMsg, int &nLen);
*/
typedef struct _ComSoStruct
{
	void *handler;
	CIrcProtocolIface *pInstance;
	CreateSoInstance pCreateInstance;
	DeleteSoInstance pDeleteInstance;

/*	VOID_CHARP_FUN SetProtocolName;
	CHARP_VOID_FUN GetProtocolName;
	VOID_OnParsedData_FUN SetCallback;
	OnParsedData_VOID_FUN GetOnParsedData;
	VOID_INT_VOIDP_INT_FUN AddData;
	INT_INT_CHARP_CHARP__INT_FUN GetProtocolMessage;
	*/
}ComSoStruct;
typedef std::pair<char *, ComSoStruct*> CHARP_COMSOSTRUCT_PAIR;
typedef std::map<char *, ComSoStruct*> CHARP_COMSOSTRUCT_MAP;
typedef std::map<char *, ComSoStruct*>::iterator CHARP_COMSOSTRUCT_MAP_ITER;

//extern GeneralAgentHandler g_TcpSocketHandler;
//extern ezserial_t *g_pSerial[TOTAL_COM_NUM];
//extern com_info_t g_com_infos[TOTAL_COM_NUM];
//extern int g_com_count;

// prototype in ezserial.h
// typedef void (*ezserial_callback_t)(int id, uint8_t *buffer, int len);
void CB_ComRecvedData(int nIndex, unsigned char *pBuf, int nLen);

// prototype in IrcTypedefStruct.h
// typedef void (* OnParsedData) (int nType, int nComId, char *strProtocolName, void *pBuf, int nLen);
void CB_ProtocolHandled(int nType, int nComId, char *strProtocolName, void *pBuf, int nLen);
void CB_ProtocolSendComDataByComIndex(int nComIndex, char * pMsg, int nLen);
void CB_ProtocolSendComDataByProtoAddr(char *strProtocol, int nAddress, char * pMsg, int nLen);



int GetValue_Grp2Buf(void *pBuf, KNX_GroupData &grpData);
int GetValue_Buf2Grp(BYTE *pBuf, int data_len, KNX_GroupData &grpData);
int GetValue_DbBuf2KnxBuf(ts_knx_control_info_t *pKnxData);

int KnxReadThread(void *pParam);
int KnxReadDbThread(void *pParam);
int WifiThread(void *pParam);
//class CWifiKernelThread : public Thread
//{
//public:
//    void Run()
//    {
//        __fline;
//        printf("thread begin.\n");
//
//        while (IsRunning())
//        {
//            if (!g_TcpSocketHandler.GetCount())
//            {
//                sleep(1);
//                __fline;
//                printf("idle.\n");
//                continue;
//            }
//
//            g_TcpSocketHandler.Select(1,0);
//        }
//
//        __fline;
//        printf("thread end.\n");
//        return;
//    }
//};

	// GatewayManager:
	//	  manages wifi connections
	//
	class DECLSPEC_DLL_EXPORT CGatewayManager
	{
	public:
		static CGatewayManager* Instance();
		virtual ~CGatewayManager();
		
		// Start manager thread.
		void StartGatewayManager();

		// Stop manager thread.
		void StopGatewayManager();

		void SetCtrlResponse(OnCtrlResponse pCtrlResponse);

		void SetLogPath(const char* szLogPath);

		void SetKnxCallbacks(
						  GetKnxDpt pGetKnxDpt,
						  OnSearchResponse pSearchResponse,
						  OnReadResponse pReadResponse,
						  OnWriteResponse pWriteResponse,
						  OnKnxConnect pKnxConnect,
						  OnKnxDisconnect pKnxDisconnect,
						  OnRWError pErrorFun);

		// Set all callbacks for wifi gateway connections.
		void SetWifiCallbacks(OnWifiConnect connFun,
			OnWifiDisconnect disconnFun);

		// Add a wifi gateway, or update an existing one if the key duplicates.
		// nKey [IN]:	Key, which should be unique.
		// szIP [IN]:	Gateway IP address.
		// nPort [IN]:	Gateway port.
		bool AddGateway(int nGatewayType, int nKey, char *localIp, char* szIP, int nPort);
		// Remove a wifi gateway.
		void RemoveGateway(int nGatewayType, int nKey);

		// Connect to all wifi gateways. Non-blocking.
		//   OnConnect(int, bool) callbacks will be notified later.
		void ConnectAll();
		// Blocking version.
		void ConnectAllWifiSync();
		void ConnectAllKnxSync();

		// Connect to a wifi gateway. Non-blocking.
		//   OnConnect(int, bool) callback will be notified later.
		void ConnectGateway(int nGatewayType, int nKey);

		// Blocking version. CWifiConnectException will be thrown if failed.
		int ConnectGatewaySync(int nGatewayType, int nKey);
		
		// Disconnect to all gateways. Non-blocking.
		//   OnDisonnect(int) callbacks will be notified later.
		void DisconnectAll();
		void DisconnectAllWifi();
		void DisconnectAllKnx();
		
		// Disconnect to the gateway. Non-blocking.
		void DisconnectGateway(int nGatewayType, int nKey);

		// Whether a gateway is connected or not.
		bool IsGatewayConnected(int nGatewayType, int nKey);

		void RemoveAllGateways();
		void RemoveAllWifiGateways();
		void RemoveAllKnxGateways();

		// Get the active wifi gateway. Return NULL if no such gateway found.
		void* GetGatewayByKey(int nGatewayType, int nKey);
		int GetGatewayKey(int nGatewayType, void *pGateway);


		void SetServerAddr(int nGatewayType, int nKey, char * serverIp, int serverPort);

		/***********************************************************************
		**  Function : Search server to get the IP getway ip address,
					   ctrl endpoint and data endpoint.
					   need parameters: multicast ip address and port,
					   there are "224.0.23.12" and "3671" default.
		**    return : 0 if OK. must to catch the CKNXException exception.
		************************************************************************/
		int SearchServer(char *strErr = NULL);

		/***********************************************************************
		 **  Function : Send a message to request group value.
		 **     Input : main group address, middle group address, group address
		 **    return : 0 if OK. must to catch the CKNXException exception.
		************************************************************************/
		int ReadGrpVal (int nKey, int nGroupAddr);
		void ReadGrpVal (int nGroupAddr);

		/***********************************************************************
		**  Function : Send a message to set group value.
		**     Input : GroupData instance.
		**    return : 0 if OK. must to catch the CKNXException exception.
		************************************************************************/
		int WriteGrpVal (int nKey, void *pBuf, int nTotalLen, int nPrefixLen = 0);
		void WriteGrpVal (void *pBuf, int nTotalLen, int nPrefixLen = 0);
		
		bool SetKnxClearTime(int nSecond);

		int AddWifiRequest(int nKey, void *buf, int len);

	public:
		int WhoAmI() { return m_nWhoAmI; }
		int GetConnectKey() { return m_nConnectKey; }
		int GetKnxClearCount() { return m_nKnxClearCount; }

		OnWifiConnect GetOnConnect() { return m_onWifiConn; }
		OnWifiDisconnect GetOnDisconnect() { return m_onWifiDisconn; }

		OnKnxConnect GetOnKnxConnect() { return m_pKnxConnect; }
		OnCtrlResponse GetCtrlResponse() { return m_pCtrlResponse; }
		
		int IsRunning() { return m_bRunning; }
		void SetRunning() { m_bRunning = false; }
		
//		TimerStruct* GetReconnWifiCond() { return &m_reconnWifi; }
		WIFI_GATEWAY_MAP* GetWifiGatewayMap() { return &m_wifiGateways; }
		THREAD_MUTEX& GetWifiGatewayMutex() { return m_wifiGatewayMutex; }
		OnWifiDisconnect GetWifiDisconn() { return m_onWifiDisconn; }
		OnWifiConnect GetWifiConn() { return m_onWifiConn; }

		TimerStruct* GetReconnKnxCond() { return &m_reconnKnx; }
		KNX_GATEWAY_MAP* GetKnxGatewayMap() { return &m_knxGateways; }
		THREAD_MUTEX& GetKnxGatewayMutex() { return m_knxGatewayMutex; }
		OnKnxConnect GetKnxConn() { return m_pKnxConnect; }
		OnKnxDisconnect GetKnxDisconn() { return m_pKnxDisconnect; }

		TimerStruct * GetKnxReadCondStruct() { return &m_knxReadCondStruct; }

		void WaitKnxReadDb();
		void KnxReadDbListPro();

		void WriteKnxValueToDB(int nKey, KNX_GroupData *pGrpData);
		int AddToKnxReadDbList(int nGatewayId, void *reqBuf, int nTotalLen, int nPrefixLen);
		
		void StartComManager();
		void EndComManager();
		int OpenComI(int on_off, int index);
		int IsOpenComI(int *on_off, int index);
		void StartInfraredManager();
		void EndInfraredManager();

		// Enocean proccess
		//void AddEnoceanRequest(knx_control_report_t *pKnxReq);
		//void HandleEnoceanResponse(int nDevId, int nData);
		
		int AddEnoceanRequest(device_control_req *pEnoceanReq, int nLen);
		void HandleEnoceanAutoBack(unsigned int nEnoceanId, int nData);

		void UpdateDatapointData(int nDataPointId, char * strValue);
		int GetDatapointData(int nDataPointId, char * strValue);
		

	public:
		// tcp server manager
		GeneralAgentHandler g_TcpSocketHandler;
		CGeneralAgentTcpSocketServer *g_pSockerServer;
		CICLogical *m_pServerLogical;

		// com manager
		ezserial_t *g_pSerial[TOTAL_COM_NUM];	// = {NULL}
		com_info_t g_com_infos[TOTAL_COM_NUM];
		int g_com_count;	// = 0
		CICLogical g_comLogical[TOTAL_COM_NUM];
		CHARP_COMSOSTRUCT_MAP m_mapSoStruct;

		ComSoStruct * GetProtocolSoStruct(char *strProtocol);

		// infrared manager
		CICLogical g_irLogical;
		int g_irDeviceHandler;
		bool g_bIrLearnReverse[TOTAL_INFRARED_NUM];
		bool g_bIrSendReverse[TOTAL_INFRARED_NUM];
		void OpenInfraredDev();
		
        /* enocean member, added by gengh on 2012/05/20 */
		CICLogical m_EnoceanLogic;
        /* enocean end */

		// scene manager
		CSceneKernel m_sceneMan;

	protected:
		CGatewayManager();
		void ActiveLoadProtocols();	

		int InitAllCom();
		int GetInfraredHandler();

		// init & uninit win32sock
		int InitWin32Sock();
		void UnInitWin32Sock();
		
	private:
		int m_nWhoAmI;

		OnCtrlResponse m_pCtrlResponse;

		OnWifiDisconnect m_onWifiDisconn;
		OnWifiConnect m_onWifiConn; // Wifi connect callback
//		OnWifiResponse m_onResponse;

		GetKnxDpt m_pGetKnxDpt;
		OnSearchResponse m_pSearchResponse;
		OnReadResponse m_pReadResponse;
		OnWriteResponse m_pWriteResponse;
		OnKnxConnect m_pKnxConnect;
		OnKnxDisconnect m_pKnxDisconnect;
		OnRWError m_pErrorFun;

		// wifi gateway connections
		ListenSocket<CGeneralAgentTcpSocketServer> *g_TcpServer;
		WIFI_GATEWAY_MAP m_wifiGateways;

		// KNX gateway connections
		KNX_GATEWAY_MAP m_knxGateways;
		
		// multi-cast object, used to search knx gateways
		CKNXKernel *m_pKnx;

		// key of the gateway to connect
		int m_nConnectKey;

		// We will clear the knx send and handle list when reconnect m_nKnxClearCount times;
		int m_nKnxClearCount;

		// main thread handle
		THREAD_HANDLE m_nWifiHandle;
		THREAD_HANDLE m_nKnxHandle;
		TimerStruct m_reconnWifi;
		TimerStruct m_reconnKnx;
		bool m_bRunning;
		THREAD_MUTEX m_wifiGatewayMutex;
		THREAD_MUTEX m_knxGatewayMutex;

		THREAD_HANDLE m_nKnxReadThread;
		TimerStruct m_knxReadCondStruct;

		THREAD_HANDLE m_nKnxReadDbThread;
		TimerStruct m_KnxReadDbCondStruct;
		CNodeList m_knxReadDbList;
	};//end class CWifiKernel
	
	// working routine of main manager thread.
	int MainThread(void *pParam);

	// working routine of KNX manager thread.
	int KnxThread(void *pParam);
	
	int NotifyManagerThread(void *pParam);

	// working routine of connecting threads.
	int ConnectWifiThread(void *pParam);
	int ConnectKnxThread(void *pParam);

	int ConnectAllThread(void *pParam);
	
	// Notify wifi manager to check wifi connections
	void DECLSPEC_DLL_EXPORT NotifyWifiManager(); // by app layer
	void DECLSPEC_DLL_EXPORT NotifyKnxManagerInStack(); // by knx stack itself
	void DECLSPEC_DLL_EXPORT NotifyWifiManagerInStack(void* pWifi); // by stack itself


#endif
