/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentHttpKeepConnector.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentHttpKeepConnector.h 5884 2012-07-10 04:10:20Z WuJunjie $
 *
 *  Explain:
 *     -
 *      http 连接器，
 *      保持连接，断开后自动再次连接
 *     -
 *
 *  Update:
 *     2012-07-10 04:10:20  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _GeneralAgentHttpKeepConnector_H
#define _GeneralAgentHttpKeepConnector_H

#include "SmartHomeTelcomDef.h"

#include "TcpSocket.h"
#include <ISocketHandler.h>
/*ezutil/http_parser.h*/
#include <http_parser.h>

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>


#include "TeHomeIF.h"

// 线程属性
//#define CGENERALAGENTHTTPKEEPCONNECTOR_THREAD_ATTR "CHttpKeepConnectorSmartHomeTelcom", TP_DEFAULT, 100

#define CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME "GeneralAgentHttpKeepConnector"
#define HTTP_USER_AGENT "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.1 (KHTML, like Gecko) Chrome/22.0.1207.1 Safari/537.1"

typedef enum __EnCommunicationStatus_t {
    EnCommunicationStatus_Initialize =  0,	// 初始化
    EnCommunicationStatus_Connecting,	// 连接
    EnCommunicationStatus_Connected,	// 已经连接
    EnCommunicationStatus_FactoryAuthing,	// 认证
    EnCommunicationStatus_FactoryAuthed,	// 认证成功

    EnCommunicationStatus_initDeviceConfigParamed		,
    EnCommunicationStatus_initDeviceConfigParaming    ,
    EnCommunicationStatus_reportCtrlDeviceStateed     ,
    EnCommunicationStatus_reportCtrlDeviceStateing    ,
    EnCommunicationStatus_reportDeviceStateListed     ,
    EnCommunicationStatus_reportDeviceStateListing    ,
    EnCommunicationStatus_reportAlarmed               ,
    EnCommunicationStatus_reportAlarming              ,

    EnCommunicationStatus_Waite,	// 普通
}EN_COMMUNICATION_STATUS_T;


class CHttpKeepConnectorSmartHomeTelcom : public TcpSocket//,public CEZThread
{
public:
    CHttpKeepConnectorSmartHomeTelcom(ISocketHandler&, const std::string &strSocketName=CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME);
    ~CHttpKeepConnectorSmartHomeTelcom();

    void SetCloseAndDelete();
    void OnConnectFailed();
    void OnDelete();

    // 从 TcpSocket::OnRead(); 中调用
    virtual void OnRawData(const char *buf,size_t len);

    /////////////////////////////
    // 加入命令解析
    virtual void OnConnect();
    virtual void OnReconnect();
    virtual void OnDisconnect();
    virtual void OnConnectTimeout();
    virtual void OnBody(char *pCmdData, unsigned int ulCmdDataLen);
    virtual void OnURL(std::string strURL);
    const std::string& GetUrl();
    virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);

    /////////////////////////////

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    void Update();
    int SetCommunicationStatus(int CommunicationStatus);

    // 出现了错误，需要重新认证
    int IniStatus();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 通讯接口
    int requestAuth();
    int initDeviceConfigParam();
    // 上报数据点 当数据点新加或者删除的时候调用此接口
    // UpDown 1 上线 0 下线
    int reportDeviceList(CAgentMsg_initDeviceConfigParam &Device, int UpDown=1);

    // 上报主机状态
    // OnOFF 1 开 0 关
    int reportCtrlDeviceState(std::string strDeviceID, int OnOFF);
    // 上报设备状态信息
    int reportDeviceStateList(std::string strDeviceID	\
                    , std::string strOperate_id	\
                    , std::string strOperate_value	\
                    , std::string strOperate_ranage	\
                    , std::string strOperate_type	\
                    , std::string strOperate_explain	\
                   );
    // 上报 报警状态信息
    int reportAlarm(std::string strDeviceID	\
                    , std::string strOperate_id	\
                    , std::string strOperate_value	\
                    , std::string strOperate_ranage	\
                    , std::string strOperate_type	\
                    , std::string strOperate_explain	\
                   );

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    int reportDeviceStateListExample(); // == reportAlarm
    int reportCtrlDeviceState();
    int reportAlarmExample();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

private:
    CHttpKeepConnectorSmartHomeTelcom(const CHttpKeepConnectorSmartHomeTelcom& s) : TcpSocket(s)//,CEZThread(CHttpKeepConnectorSmartHomeTelcom_THREAD_ATTR)
    {} // copy constructor
    CHttpKeepConnectorSmartHomeTelcom& operator=(const CHttpKeepConnectorSmartHomeTelcom& )
    {
        return *this;
    } // assignment operator
    /** Create a new instance and reconnect */
    CHttpKeepConnectorSmartHomeTelcom *Reconnect();
    ssize_t SendPostStr(const std::string &page, const std::string &poststr);

    bool m_b_connected;
    time_t m_tOnData; ///< last Time in seconds when this socket was on data

    /////////////
    http_parser *m_httpParser;
    http_parser_settings *m_httpSettings;

    // 状态, 具体见EN_COMMUNICATION_STATUS_T的定义
    int m_iCommunicationStatus;

    CEZMutex m_mutexCommunicationStatus;

    std::string m_strSessionId;
    time_t m_ttLastSendData;

    //
    unsigned int m_iInitDeviceConfigParamNo;
    CEZMutex m_mutexInitDeviceConfigParamNo;

    std::vector<CAgentMsg_initDeviceConfigParam> deviceVec;
    // 返回设备总数
    //如果reload = 0表示不需要查数据库，否则重查数据库
    ssize_t getDeviceNum(int reload=0);
    //获取设备, 如果no为-1， 则为顺序取下一个设备
    //如果no >= 0,则为取指定位置的值。
    // return: 0- 成功 -1-未取到 -2 - 输入的no超出范围
    int getDevice(CAgentMsg_initDeviceConfigParam &DeviceCfg, int No=-1);

    std::string m_strCurrentURL;

    // this module 上报类的url
    std::string m_strURL_requestAuth					;
    std::string m_strURL_initDeviceConfigParam			;
    std::string m_strURL_reportCtrlDeviceState			;
    std::string m_strURL_reportDeviceStateList			;
    std::string m_strURL_reportAlarm					;

    std::string m_strURL_register						  ;
    std::string m_strURL_requestReportDeviceConfigParam   ;
    std::string m_strURL_getCtrlDeviceState               ;
    std::string m_strURL_getDeviceStateList               ;
    std::string m_strURL_controlDevice                    ;

	int m_iConnTimes;
};

#endif // _GeneralAgentHttpKeepConnector_H
