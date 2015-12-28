/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * nvrnetapi.cpp - _explain_
 *
 * Copyright (C) 2011 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: nvrnetapi.cpp 5884 2012-11-23 02:37:26Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-11-23 02:37:26  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "nvrnetapi.h"

#include "GeneralAgent.h"
#ifdef USE_GENERALSOCKETPROCESSOR
#include "GeneralSocketProcessor.h"
#endif

#define CGENERALAGENTKEEPCONNECTOR_SOCKET_NAME "NVRKeepConnector"
CGeneralAgentTcpSocketConnector *GetNvrDlg(std::string strName, int waitTimes=5);

typedef struct __connector_attr
{
    int iConnTimeOut;//超时时间
    int iConnTimes; //连接尝试次数（保留）

    int iInterval; //[in] 重连间隔，单位毫秒，参数默认值为30秒
    int bEnableRecon ;//[in] 是否重连，0-不重连，1-重连，参数默认值为1
}
CONNECTOR_ATTR;

CONNECTOR_ATTR __gConnAttr = {0};
GENERALSERVERCFG_T __NvrServer;
GENERALAGENTCFG_T g_ConnPara;


//用户回调接口
void(CALLBACK *g_RealDataCallBack) (LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)  = NULL;

// 类中的回调接口
//void( *__m_RealDataCallBack) (char *pBuffer, int dwBufSize, int ch, int stream);
void CALLBACK __m_RealDataCallBack(char *pBuffer, int dwBufSize, int ch, int stream)
{
    DBG_CODE(
        __fline;
        printf(" g_RealDataCallBack_V30 Get data at:%ld, the size is:%d.\n", time(NULL), dwBufSize);
    );

    if (g_RealDataCallBack)
    {
        g_RealDataCallBack(0, 0, (BYTE *)pBuffer, dwBufSize, (void *)0);
    }
}


NET_DVR_API BOOL __stdcall NET_DVR_Init()
{
    return 0;
}
NET_DVR_API BOOL __stdcall NET_DVR_Cleanup()
{
    return 0;
}

/* reserved */
NET_DVR_API BOOL __stdcall NET_DVR_SetConnectTime(DWORD dwWaitTime/* = 3000*/, DWORD dwTryTimes/* = 3*/)
{
    //__gConnAttr.
    __NvrServer.iConnTimeOut = dwWaitTime;
    __NvrServer.iConnTimes = dwTryTimes;

    return 0;
}

NET_DVR_API BOOL __stdcall NET_DVR_SetReconnect(DWORD dwInterval/* = 30000*/, BOOL bEnableRecon/* = TRUE*/)
{
    __NvrServer.bEnableRecon = bEnableRecon;
    __NvrServer.iInterval = dwInterval;

    return 0;
}

NET_DVR_API LONG __stdcall NET_DVR_Login_V30(char *sDVRIP, WORD wDVRPort, char *sUserName, char *sPassword, LPNET_DVR_DEVICEINFO_V30 lpDeviceInfo)
{
    g_ConnPara.Peer.clear();
    //g_ConnPara.PeerHttp.clear();

    __NvrServer.strHost = sDVRIP;
    __NvrServer.Port = wDVRPort;
    __NvrServer.strUName = sUserName;
    __NvrServer.strUPwd = sPassword;
    __NvrServer.strServerName = CGENERALAGENTKEEPCONNECTOR_SOCKET_NAME;

    g_ConnPara.Peer.push_back(__NvrServer);

    g_GeneralAgent.Start(&g_ConnPara);

    CGeneralAgentTcpSocketConnector *p = GetNvrDlg(CGENERALAGENTKEEPCONNECTOR_SOCKET_NAME);

    if (p)
    {
        if (p->IsLogined())
        {
            return 0;
        }
    }

    return -1;
}

NET_DVR_API BOOL __stdcall NET_DVR_Logout_V30(LONG lUserID)
{
    g_GeneralAgent.Stop();

    return 0;
}

NET_DVR_API DWORD __stdcall NET_DVR_GetLastError()
{
    return 0;
}

NET_DVR_API char* __stdcall NET_DVR_GetErrorMsg(LONG *pErrorNo /*= NULL*/)
{
    return (char *)"OK";
}


NET_DVR_API LONG __stdcall NET_DVR_RealPlay_V30(LONG lUserID, LPNET_DVR_CLIENTINFO lpClientInfo, void(CALLBACK *fRealDataCallBack_V30) (LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser) /*= NULL*/, void* pUser /*= NULL*/, BOOL bBlocked /*= FALSE*/)
{
    CGeneralAgentTcpSocketConnector *p = GetNvrDlg(CGENERALAGENTKEEPCONNECTOR_SOCKET_NAME);
    int iRet = -1;

    if (p)
    {
        MON_STATUS_T __mon;
        __mon.Channel = lpClientInfo->lChannel;
        __mon.Stream = lpClientInfo->lLinkMode;
        __mon.OnOff = 1;

        g_RealDataCallBack = fRealDataCallBack_V30;

        // 设置实时数据回调函数
        p->SetRealDataCallBack(__m_RealDataCallBack);

        iRet = p->Odip_ReqRealPlay(__mon);
    }

    return iRet;
}

NET_DVR_API LONG __stdcall NET_DVR_RemoteSnap(LONG lUserID, LPNET_DVR_CLIENTINFO lpClientInfo, void(CALLBACK *fRealDataCallBack_V30) (LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser) /*= NULL*/, void* pUser /*= NULL*/, BOOL bBlocked /*= FALSE*/)
{
    CGeneralAgentTcpSocketConnector *p = GetNvrDlg(CGENERALAGENTKEEPCONNECTOR_SOCKET_NAME);
    int iRet = -1;

    if (p)
    {
        MON_STATUS_T __mon;
        __mon.Channel = lpClientInfo->lChannel;
        __mon.Stream = lpClientInfo->lLinkMode;
        __mon.OnOff = 1;

        g_RealDataCallBack = fRealDataCallBack_V30;

        // 设置实时数据回调函数
        p->SetRealDataCallBack(__m_RealDataCallBack);

        iRet = p->Odip_ReqSnap(__mon);
    }
    else
    {
        iRet = -101;
    }

    return iRet;
}


NET_DVR_API BOOL __stdcall NET_DVR_StopRealPlay(LONG lRealHandle)
{
    CGeneralAgentTcpSocketConnector *p = GetNvrDlg(CGENERALAGENTKEEPCONNECTOR_SOCKET_NAME);
    int iRet = -1;

    if (p)
    {
        iRet = p->Odip_ReqStopRealPlay(lRealHandle);
    }

    return (iRet>=0);
}


NET_DVR_API BOOL __stdcall NET_DVR_RemoteRecordCtrl(DWORD BitMASK)
{
    CGeneralAgentTcpSocketConnector *p = GetNvrDlg(CGENERALAGENTKEEPCONNECTOR_SOCKET_NAME);
    int iRet = -1;

    if (p)
    {
        iRet = p->Odip_RemoteRecordCtrl(BitMASK);
    }
    else
    {
        //__trip;
    }

    return (iRet>=0);

}

NET_DVR_API LONG __stdcall NET_DVR_GetRemoteVersion(char *pVer, DWORD bufLen)
{
    CGeneralAgentTcpSocketConnector *p = GetNvrDlg(CGENERALAGENTKEEPCONNECTOR_SOCKET_NAME);
    int iRet = -101;

    if (p)
    {
        iRet = p->GetRemoteVersion(pVer, bufLen);
    }
    else
    {
        //__trip;
    }

    return iRet;
}

CGeneralAgentTcpSocketConnector *GetNvrDlg(std::string strName, int waitTimes)
{
    Socket *p0 = NULL;
    static int ___times=0;
    while(p0==NULL)
    {
        p0 = g_GeneralAgent.GetSocketByName(strName);
        if (p0==NULL 
			&& ___times>waitTimes)
        {
            //__trip;
            return NULL;
        }

        sleep(++___times);
    }

    ___times = 0;

    CGeneralAgentTcpSocketConnector *p = dynamic_cast<CGeneralAgentTcpSocketConnector *>(p0);
    return p;
}

