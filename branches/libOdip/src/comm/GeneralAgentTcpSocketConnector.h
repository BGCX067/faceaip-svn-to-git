/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocketConnector.h - _explain_
 *
 * Copyright (C) 2011 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocketConnector.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -Tcp客户端实例-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _GENERALAGENTTCPSOCKETCONNECTOR_H
#define _GENERALAGENTTCPSOCKETCONNECTOR_H

#include "GeneralAgentTcpSocket.h"
#include <ISocketHandler.h>

#include <ISocketHandler.h>

#include "EZMutex.h"
#include "EZLock.h"


//事务处理状态
typedef enum en_odip_logic_status
{
    enAppLogicStatus_Begin  =  0,
    enAppLogicStatus_Logining  =  1,
    enAppLogicStatus_Logined  =  2,

    enAppLogicStatus_Exiting,
    enAppLogicStatus_End,
}enAppLogicStatus;

//#define

class MON_STATUS_T
{
public:
    int MonType;
    int Channel; // 通道号, 0-n n+1-2n ...
    int Stream;
    int Frames;
    int OnOff;
    int TransProtocol;
    std::string RemoteIP;
    int RemotePort;

    time_t ttReqed; // 请求发出时间
    time_t ttLastOndata; // 请求发出时间

    MON_STATUS_T()
    {
        MonType = -1;
        Channel = -1;
        Stream = -1;
        Frames = -1;
        OnOff = -1;
        TransProtocol = -1;
        RemoteIP = "N/A";
        RemotePort = -1;

        ttReqed = 0;
        ttLastOndata = 0;
    }

    bool operator==(const MON_STATUS_T &t1)const
    {
        return (    Channel == t1.Channel
                    && Stream == t1.Stream
                    && TransProtocol == t1.TransProtocol
               );
    }

    int Struct2String(std::string &strMon);
};

class CGeneralAgentTcpSocketConnector : public CGeneralAgentTcpSocket
{
public:
    CGeneralAgentTcpSocketConnector(ISocketHandler&
                                    , std::string strSocketName="GeneralAgentTcpSocketConnector"
                                                                , time_t Timeout= 5
                                                                                  , bool bReconn = false);
    ~CGeneralAgentTcpSocketConnector();

    void SetCloseAndDelete();
    void OnConnectFailed();
    void OnDelete();

    // 心跳
    int Update();

	int GetOdipStatus();

    // 登录
    int Odip_ReqLogin();
    // 心跳
    int Odip_ReqQueryStat();
    // 监视
    int Odip_ReqRealPlay(MON_STATUS_T &MonData);
    int Odip_ReqRealPlay(std::vector<MON_STATUS_T> &RealPlayStatus);
    int Odip_ReqStopRealPlay(int iHandle);

    int Odip_RemoteRecordCtrl(unsigned int BitMASK);

    int Odip_ReqSnap(MON_STATUS_T &MonData);

	// 0x08 - version
    int Odip_ReqRemoteInfo(int iInfo);

    bool IsLogined();

	int GetRemoteVersion(char *pVer, unsigned int bufLen);

    /////////////////////////////
    // 加入命令解析
    virtual void OnConnect();
    virtual void OnReconnect();
    virtual void OnDisconnect();
    virtual void OnConnectTimeout();
    //virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);
#ifdef INTERNAL_ODIP_DATA_PARSE
    // 通用头 pCmd
    // 数据 pData， 长度见 pCmd->u8DataLen
    virtual void OnCommand_ODIP(const ODIP_HEAD_T *pCmd, const unsigned char *pData);
#endif
    /////////////////////////////

    bool IsReConnect();

    void SetAccount(std::string strUName, std::string strUPwd);
    void SetRealDataCallBack(void( *RealDataCallBack) (char *pBuffer, int dwBufSize, int ch, int stream));

    void ReNew();

private:
    /** Create a new instance and reconnect */
    // void warning :m_b_connected’ will be initialized after
    CGeneralAgentTcpSocketConnector *Reconnect();
    bool m_b_connected;

    CGeneralAgentTcpSocketConnector(const CGeneralAgentTcpSocketConnector& s) : CGeneralAgentTcpSocket(s)
    {
        m_strUName = "";
        m_strUPwd = "";

        m_enOdipStatus = enAppLogicStatus_Begin;

        m_tOnOdip = time(NULL);
        m_RealDataCallBack = NULL;

        m_ttOdipTimeOut = 2;

        m_RealPlayStatus = s.m_RealPlayStatus;
        for (size_t ii=0; ii<m_RealPlayStatus.size(); ii++)
        {
            m_RealPlayStatus[ii].ttReqed = 0;
            m_RealPlayStatus[ii].ttLastOndata = 0;
        }

        m_iChannel = -1;
    } // copy constructor
    CGeneralAgentTcpSocketConnector& operator=(const CGeneralAgentTcpSocketConnector& s)
    {
        return *this;
    } // assignment operator


    ///////////
    std::string m_strSocketName;
    time_t m_ttTimeout;
    bool m_bReconn;
    ///////////


    int m_iLoginStatus;

    time_t m_ttConnectTimeOut;
    time_t m_ttOdipTimeOut; // 命令超时时间
    bool m_bReConnect;

    enAppLogicStatus m_enOdipStatus;

    time_t m_tOnOdip;

    ////////////////////////
    std::string m_strUName;
    std::string m_strUPwd;
    int m_iChannel;


    std::vector<MON_STATUS_T> m_RealPlayStatus;
    CEZMutex m_MutexRealPlayStatus;

    int FindRealPlayStatus(int ch, int stream);

    void( *m_RealDataCallBack) (char *pBuffer, int dwBufSize, int ch, int stream);

	std::string m_strRemoteVersion;
};

#endif // _GENERALAGENTTCPSOCKETCONNECTOR_H
