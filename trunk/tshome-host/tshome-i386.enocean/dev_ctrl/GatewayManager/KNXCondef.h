//
//  condef.h
//  tsbus_knx
//
//  Created by kyy on 11-5-20.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//


#ifndef _KNX_GROUPDATA
#define _KNX_GROUPDATA

#include "KNXWinVarType.h"
#include "common/thread.h"

#undef DECLSPEC_DLL_EXPORT
#ifdef WIN32
#ifdef _USRDLL
#define DECLSPEC_DLL_EXPORT __declspec(dllexport)
#else
#define DECLSPEC_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DECLSPEC_DLL_EXPORT
#endif

// invalid middle address is used to distinguish
// the current group is a 2 or 3 segements group.
// if(middle_address == INVALID_MID_ADDR), it is 2 segements.
// else it is 3 segements.
#define		INVALID_MID_ADDR		-1

// current project is 2 or 3 segements.
#define		PROJECT_SEGEMENTS_3		0
#define		PROJECT_SEGEMENTS_2		1

#define		TXT_DATA_SIZE			16
#define		MAX_DATA_AREASIZE		14


// Error codes
// common error
#define		E_NO_ERROR				0x00
#define		E_HOST_PROTOCOL_TYPE	0x01
#define		E_VERSION_NOT_SUPPORTED	0x02
#define		E_SEQUENCE_NUMBER		0x04

// connect response error
#define		E_CONNECTION_TYPE		0x22
#define		E_CONNECTION_OPTION		0x23
#define		E_NO_MORE_CONNECTIONS	0x24

// connect state response error
#define		E_CONNECTION_ID			0x21
#define		E_DATA_CONNECTION		0x26
#define		E_KNX_CONNECTION		0x27

// tunnel connect error
#define		E_TUNNELING_LAYER		0x29


#define LOG_TO_FILE // print log to file

// print log
#ifdef LOG_TO_FILE
#define DEBUG_LOG0(cls,msg) {\
	time_t tt;\
	struct tm *t;\
	time(&tt);\
	t = localtime(&tt);\
	if (g_log)\
	fprintf (g_log, "["#cls"(%d)]: %02d/%02d/%02d %02d:%02d:%02d - "#msg, __LINE__, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);\
}
#define DEBUG_LOG1(cls,msg,arg1) {\
	time_t tt;\
	struct tm *t;\
	time(&tt);\
	t = localtime(&tt);\
	if (g_log)\
	fprintf (g_log, "["#cls"(%d)]: %02d/%02d/%02d %02d:%02d:%02d - "#msg, __LINE__, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, arg1);\
}
#define DEBUG_LOG2(cls,msg,arg1,arg2) {\
	time_t tt;\
	struct tm *t;\
	time(&tt);\
	t = localtime(&tt);\
	if (g_log)\
	fprintf (g_log, "["#cls"(%d)]: %02d/%02d/%02d %02d:%02d:%02d - "#msg, __LINE__, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, arg1, arg2);\
}
#define DEBUG_LOG3(cls,msg,arg1,arg2,arg3) {\
	time_t tt;\
	struct tm *t;\
	time(&tt);\
	t = localtime(&tt);\
	if (g_log)\
	fprintf (g_log, "["#cls"(%d)]: %02d/%02d/%02d %02d:%02d:%02d - "#msg, __LINE__, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, arg1, arg2, arg3);\
}
#else
	#ifdef LOG_TO_STDOUT
	#define DEBUG_LOG0(cls,msg) {\
		time_t tt;\
		struct tm *t;\
		time(&tt);\
		t = localtime(&tt);\
		printf ("["#cls"(%d)]: %02d/%02d/%02d %02d:%02d:%02d - "#msg, __LINE__, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);\
	}
	#define DEBUG_LOG1(cls,msg,data) {\
		time_t tt;\
		struct tm *t;\
		time(&tt);\
		t = localtime(&tt);\
		printf ("["#cls"(%d)]: %02d/%02d/%02d %02d:%02d:%02d - "#msg, __LINE__, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, data);\
	}
	#define DEBUG_LOG2(cls,msg,arg1,arg2) {\
		time_t tt;\
		struct tm *t;\
		time(&tt);\
		t = localtime(&tt);\
		printf ("["#cls"(%d)]: %02d/%02d/%02d %02d:%02d:%02d - "#msg, __LINE__, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, arg1, arg2);\
	}
	#define DEBUG_LOG3(cls,msg,arg1,arg2,arg3) {\
		time_t tt;\
		struct tm *t;\
		time(&tt);\
		t = localtime(&tt);\
		printf ("["#cls"(%d)]: %02d/%02d/%02d %02d:%02d:%02d - "#msg, __LINE__, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, arg1, arg2, arg3);\
		}
	#else
	#define DEBUG_LOG0(cls,msg)
	#define DEBUG_LOG1(cls,msg,arg1)
	#define DEBUG_LOG2(cls,msg,arg1,arg2)
	#define DEBUG_LOG3(cls,msg,arg1,arg2,arg3)
	#endif
#endif


#define REQ_RES_BUF_SIZE		256

#pragma pack(1)

typedef enum
{
	PROTOCOL_NULL = 0,
	PROTOCOL_AUXDIO_8150,
	PROTOCOL_BLURAY,
	PROTOCOL_EPSON_ESC_VP21
}Device_Protocol_Type;

typedef enum
{
	GATEWAY_TYPE_UNKNOWN = 0,
	GATEWAY_TYPE_KNX,
	GATEWAY_TYPE_WIFI,
	GATEWAY_TYPE_HOST
} GATEWAY_TYPE;


typedef struct _TimerStruct
{
	TIMER_COND timerCond;
	THREAD_MUTEX timerMutex;

	// time out value
	#ifdef _WIN32
		DWORD timeout;
	#else
		struct timespec timeout;
	#endif

	int bWaitObject;	// bool
} TimerStruct;


typedef struct _NodeBase
{
	struct _NodeBase *pNext;
}NodeBase;

typedef struct _SendNode
{
	NodeBase *pNext;
	BYTE buf[REQ_RES_BUF_SIZE];
	int nLen;
	int nAction;
	int nDelaySecond;

	int nPrefixLen;
}SendNode;

typedef struct _RecvNode
{
	NodeBase *pNext;
	int nResType;
	BYTE buf[REQ_RES_BUF_SIZE];
	int nLen;
	int nErrorNo;
}RecvNode;

typedef enum
{
	ACTION_NULL = 0,

	// knx request types
	ACTION_KNX_READ,
	ACTION_KNX_WRITE,

	// knx response types
	ACTION_KNX_SEARCH_RES,
	ACTION_KNX_READ_RES,
	ACTION_KNX_WRITE_RES,
	ACTION_KNX_EXTRA_RES,

	ACTION_WIFI_REQUEST,
	ACTION_WIFI_RESPONSE,
	ACTION_WIFI_AUTO,

	IC_UNKNOWN,
	IC_DIRECT,
	IC_WIFI_CLIENT,
	IC_WIFI_SERVER,

	IC_DIRECT_COM_RES,
	IC_DIRECT_COM_CUSTOMER_RES,
	IC_DIRECT_IRCTRL_RES,
	IC_DIRECT_IRSTUDY_RES,

	// current state of predefined com devices
	IC_DIRECT_COM_STATE,
}Action_Type;

typedef enum
{
	BAUDRATE_2400 = 0x00,
	BAUDRATE_4800,
	BAUDRATE_9600,
	BAUDRATE_19200,
	BAUDRATE_38400,
	BAUDRATE_57600,
	BAUDRATE_115200
} COM_BAUDRATE;

typedef enum
{
	PARITY_NONE = 0,
	PARITY_ODD,
	PARITY_EVEN
} COM_PARITY;

typedef enum
{
	DATABIT_5 = 0x00,
	DATABIT_6,
	DATABIT_7,
	DATABIT_8
} COM_DATABIT;

typedef enum
{
	COM_STOPBIT_1 = 0,
	COM_STOPBIT_2
} COM_STOPBIT;


	typedef struct _TIMEDATA
	{
		int			year;
		int			month;
		int			day;
		int			hour;
		int			minute;
		int			second;
		int			weekday;
	}KNX_TIMEDATA;
	
	typedef union _DataArea 
	{
		int			BinData;	// bool
		double		AnaData;
		char		TxtData[TXT_DATA_SIZE];
		KNX_TIMEDATA	TimeData;
	}KNX_DataArea;
	
	typedef struct _GroupData
	{ 
		int			nGroupAddr;
		int			nGrpDPT;
		KNX_DataArea	Data;
	}KNX_GroupData;

	typedef void (* OnCtrlResponse)(int nType, void *buf, int nLen);

	/***********************************************************************
	 **  Function : (CallBack Function) Called when received a response
	 message of search request.
	 **     Input : KNX_GroupData instance.
	 ************************************************************************/
	typedef void (* OnSearchResponse)(int nGatewayIndex, char *serverIp, int serverPort, int nStatus);

	typedef void (* OnReadResponse)(int nHomeDevId, KNX_GroupData grpData);
	typedef void (* OnWriteResponse)(int nHomeDevId, KNX_GroupData grpData);

	typedef int (* GetKnxDpt)(int nGatewayIndex, int nGroupAddr);

	/***********************************************************************
	 **  Function : (CallBack Function) Called when received a response
	 message of disconnect tunnel.
	 **		Input : whether show tip or not
	 ************************************************************************/
	typedef void (* OnKnxConnect)(int nGatewayIndex, int nSucceed, int expId, char *strErr);
	typedef void (* OnKnxDisconnect)(int nGatewayIndex);

	// nReadOrWrite is: ACTION_KNX_WRITE or ACTION_KNX_READ
	typedef void (* OnRWError)(int nGatewayIndex, int nExceptionId, char *strException, int nReadOrWrite);

	//Called when disconnected and need notify manager thread to reconnect.
	typedef void (* OnNotifyKnxManager) ();

	typedef enum
	{
		TIMER_WIFI_CONNECT = 0,
		TIMER_SEND,
		TIMER_HEARTBEAT,
		TIMER_WIFI_DISCONNECT,
		TIMER_END
	} EVENT_TIMER;

	typedef struct _CondStruct
	{
		TIMER_COND cond;
		THREAD_MUTEX mutex;
		int bRun;	// bool
	} CondStruct;

	/********* CallBack Functions *********/
	//  Called when socket connected.
	typedef void (* OnWifiConnect)(int nGatewayIndex, int nSucceed);
	//  Called when socket disconnected. Don't call Disconnect() again
	//    in this callback.
	typedef void (* OnWifiDisconnect)(int nGatewayIndex);

	// Called when disconnected and need notify manager thread to reconnect.
	typedef void (* OnNotifyWifiManager) (void* pWifi);

	// Wifi Data Callback
	typedef void (* OnWifiResponse)(int nActionType, void *buf, int nLen);



#define WIFI_MSG_SIZE 5
#define WIFI_STATE_CODE 0x7F

#define WIFI_MAX_SEND 3			// send 3 times at max
#define WIFI_STUDY_TIME 8		// waiting 8 seconds for IR study
#define WIFI_MAX_TIME 3			// waiting other WIFI responses for 3 seconds
#define WIFI_CONNECT_TIME 5		// connecting timeout (3 seconds)
#define WIFI_MAX_SONGNAME 20	// max song name length
#define WIFI_HEART_INTERVAL 5	// interval time(seconds) of heart beat
#define WIFI_RECONNECT_INTERVAL 120
	
#define KNX_RECONNECT_TIME				36
#define KNX_RECONNECT_INTERVAL_GEN		120
#define KNX_RECONNECT_INTERVAL_NOTIFY	5
	
#define WIFI_MUSIC_START_CODE 0xFF
#define WIFI_MUSIC_STOP_CODE 0xFE

// added by keyy 2011/10/6
// define the Epson source type
#define WIFI_EPSON_SOURCE_10	1
#define WIFI_EPSON_SOURCE_20	2
#define WIFI_EPSON_SOURCE_30	3
#define WIFI_EPSON_SOURCE_41	4
#define WIFI_EPSON_SOURCE_42	5
#define WIFI_EPSON_SOURCE_B0	6
// define the Epson A/V type
#define WIFI_EPSON_AV_00		1
#define WIFI_EPSON_AV_01		2
#define WIFI_EPSON_AV_02		3


#pragma pack()

#endif
