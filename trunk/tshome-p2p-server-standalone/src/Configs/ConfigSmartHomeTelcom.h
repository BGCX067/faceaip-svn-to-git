/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigSmartHomeTelcom.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: ConfigSmartHomeTelcom.h 5884 2012-09-07 10:46:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/



#ifndef __ConfigSmartHomeTelcom_H__
#define __ConfigSmartHomeTelcom_H__

#include "ConfigBase.h"
//!
typedef struct tagConfigSmartHomeTelcom
{
    std::string strHost;// 平台主机
    int iPort;// 端口
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 厂商配置
    std::string ProductID 		;//String 厂家编号，由智慧E 家平台给厂商分配
    std::string ProductName 	;//String 厂家名，如tiansu
    std::string AuthUser 		;//string 授权用户名
    std::string AuthPassword 	;//string 授权用户密码


    // 目前是在上报控制设备的时候上报给 平台,
    // 平台自己用 strURLRegisterServerHost+strURL_register来控制
    std::string strURLRegisterServerHost							;
    // 以下服务器端口暂只用 iRegisterServerPort， 其余备用
    int iRegisterServerPort;// 注册服务器端口
    int iReportDeviceConfigParamServerPort;// 服务器端口
    int iCtrlDeviceStateServerPort;// 服务器端口
    int iDeviceStateList;// 服务器端口
    int iControlDeviceServerPort;// 服务器端口

    //this module 上报类的url
    std::string strURL_requestAuth;
    std::string strURL_initDeviceConfigParam;
    std::string strURL_reportCtrlDeviceState;
    std::string strURL_reportDeviceStateList;
    std::string strURL_reportAlarm;

    // this module 收到的
    std::string strURL_register							;
    std::string strURL_requestReportDeviceConfigParam   ;
    std::string strURL_getCtrlDeviceState               ;
    std::string strURL_getDeviceStateList               ;
    std::string strURL_controlDevice                    ;

}
ConfigSmartHomeTelcom;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigSmartHomeTelcom, 1, 4> CConfigSmartHomeTelcom;

#endif //__ConfigSmartHomeTelcom_H__
