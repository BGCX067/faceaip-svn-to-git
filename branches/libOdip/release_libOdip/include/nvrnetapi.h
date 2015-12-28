/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * nvrnetapi.h - _explain_
 *
 * Copyright (C) 2012 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: nvrnetapi.h 5884 2012-11-23 02:37:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-11-23 02:37:21  Create 0.5
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _NVR_NET_API_H_
#define _NVR_NET_API_H_
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if (defined(_WIN32)) //windows
	#define NET_DVR_API  extern "C"__declspec(dllimport)
#elif defined(__linux__) //linux
	#define NET_DVR_API extern "C"
	typedef     unsigned int    DWORD;
	typedef     unsigned short  WORD;
	typedef     unsigned short  USHORT;
	typedef     int            LONG;
	typedef  	unsigned char	BYTE ;
	typedef     int BOOL;
	typedef     unsigned int   	UINT;
	typedef 	void* 			LPVOID;
	typedef 	void* 			HANDLE;
	typedef     unsigned int*  LPDWORD; 
	typedef  unsigned long long UINT64;
	
	#ifndef    TRUE
	#define    TRUE	1
	#endif
	#ifndef    FALSE
	#define	   FALSE 0
	#endif
	#ifndef    NULL
	#define	   NULL 0
	#endif

#endif //  windows/linux ...

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//宏定义
#define MAX_NAMELEN			    16		//DVR本地登陆名
#define MAX_RIGHT			    32		//设备支持的权限（1-12表示本地权限，13-32表示远程权限）
#define NAME_LEN			    32      //用户名长度
#define PASSWD_LEN			    16      //密码长度
#define SERIALNO_LEN		    48      //序列号长度
#define MACADDR_LEN			    6       //mac地址长度
#define MAX_ETHERNET		    2       //设备可配以太网络
#define MAX_NETWORK_CARD        4       //设备可配最大网卡数目
#define PATHNAME_LEN		    128     //路径长度
#define MAX_PRESET_V13          16      //预置点

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

//NET_DVR_Login_V30()参数结构
typedef struct
{
    BYTE sSerialNumber[SERIALNO_LEN];  //序列号
    BYTE byAlarmInPortNum;		        //报警输入个数
    BYTE byAlarmOutPortNum;		        //报警输出个数
    BYTE byDiskNum;				    //硬盘个数
    BYTE byDVRType;				    //设备类型, 1:DVR 2:ATM DVR 3:DVS ......
    BYTE byChanNum;				    //模拟通道个数
    BYTE byStartChan;			        //起始通道号,例如DVS-1,DVR - 1
    BYTE byAudioChanNum;                //语音通道数
    BYTE byIPChanNum;					//最大数字通道个数  
	BYTE byZeroChanNum;			//零通道编码个数 //2010-01-16
	BYTE byMainProto;			//主码流传输协议类型 0-private, 1-rtsp
	BYTE bySubProto;				//子码流传输协议类型0-private, 1-rtsp
    BYTE bySupport;        //能力，位与结果为0表示不支持，1表示支持，
                            //bySupport & 0x1, 表示是否支持智能搜索
                            //bySupport & 0x2, 表示是否支持备份
                            //bySupport & 0x4, 表示是否支持压缩参数能力获取
                            //bySupport & 0x8, 表示是否支持多网卡
                            //bySupport & 0x10, 表示支持远程SADP
                            //bySupport & 0x20  表示支持Raid卡功能
                            //bySupport & 0x40  表示支持IPSAN 目录查找
    BYTE byRes1[20];					//保留
}NET_DVR_DEVICEINFO_V30, *LPNET_DVR_DEVICEINFO_V30;
//软解码预览参数
typedef struct
{
	LONG lChannel;//通道号
	LONG lLinkMode;//最高位(31)为0表示主码流，为1表示子，0－30位表示码流连接方式: 0：TCP方式,1：UDP方式,2：多播方式,3 - RTP方式，4-音视频分开(TCP)
	//HWND hPlayWnd;//播放窗口的句柄,为NULL表示不播放图象
	char* sMultiCastIP;//多播组地址
}NET_DVR_CLIENTINFO, *LPNET_DVR_CLIENTINFO;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#define __stdcall 
#define CALLBACK  
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

NET_DVR_API BOOL __stdcall NET_DVR_Init();
NET_DVR_API BOOL __stdcall NET_DVR_Cleanup();

NET_DVR_API BOOL __stdcall NET_DVR_SetConnectTime(DWORD dwWaitTime = 3000, DWORD dwTryTimes = 3);
NET_DVR_API BOOL __stdcall NET_DVR_SetReconnect(DWORD dwInterval = 30000, BOOL bEnableRecon = TRUE);

NET_DVR_API LONG __stdcall NET_DVR_Login_V30(char *sDVRIP, WORD wDVRPort, char *sUserName, char *sPassword, LPNET_DVR_DEVICEINFO_V30 lpDeviceInfo);
NET_DVR_API BOOL __stdcall NET_DVR_Logout_V30(LONG lUserID);

NET_DVR_API DWORD __stdcall NET_DVR_GetLastError();
NET_DVR_API char* __stdcall NET_DVR_GetErrorMsg(LONG *pErrorNo = NULL);


NET_DVR_API LONG __stdcall NET_DVR_RealPlay_V30(LONG lUserID, LPNET_DVR_CLIENTINFO lpClientInfo, void(CALLBACK *fRealDataCallBack_V30) (LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser) = NULL, void* pUser = NULL, BOOL bBlocked = FALSE);
NET_DVR_API BOOL __stdcall NET_DVR_SetRealDataCallBack(LONG lRealHandle,void(CALLBACK *fRealDataCallBack) (LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser),DWORD dwUser);
NET_DVR_API BOOL __stdcall NET_DVR_StopRealPlay(LONG lRealHandle);

#endif //  _NVR_NET_API_H_
