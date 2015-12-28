/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigSmartHomeTelcom.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: ConfigSmartHomeTelcom.cpp 5884 2012-09-07 10:46:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:00  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "ConfigSmartHomeTelcom.h"

template<>
void exchangeTable<ConfigSmartHomeTelcom>(CConfigExchange& xchg, CConfigTable& table, ConfigSmartHomeTelcom& config, int index, int app)
{
    xchg.exchange(table, "Host"    , config.strHost    , "202.102.108.171");
    xchg.exchange(table, "Port"    , config.iPort    , 0, 65535 ,8080);

    xchg.exchange(table, "ProductID"    , config.ProductID    , "61");
    xchg.exchange(table, "ProductName"  , config.ProductName  , "TS-Home-Server-JSTelecom@Nanjing Tiansu Automation Control System Co., Ltd");
    xchg.exchange(table, "AuthUser"     , config.AuthUser     , "tiansu");
    xchg.exchange(table, "AuthPassword" , config.AuthPassword , "111111");

    // 以下服务器端口暂只用 iRegisterServerPort， 其余备用
    xchg.exchange(table, "URLRegisterServerHost", config.strURLRegisterServerHost, "http://218.95.39.172:60080/SmartHomeTelcom/");
    xchg.exchange(table, "RegisterServerPort"    , config.iRegisterServerPort, 0, 65535 ,60080);
    xchg.exchange(table, "ReportDeviceConfigParamServerPort"    , config.iReportDeviceConfigParamServerPort, 0, 65535 ,60080);
    xchg.exchange(table, "CtrlDeviceStateServerPort"    , config.iCtrlDeviceStateServerPort, 0, 65535 ,60080);
    xchg.exchange(table, "DeviceStateList"    , config.iDeviceStateList, 0, 65535 ,60080);
    xchg.exchange(table, "ControlDeviceServerPort"    , config.iControlDeviceServerPort, 0, 65535 ,60080);

    //this module 上报类的url
    xchg.exchange(table, "requestAuth", config.strURL_requestAuth, "/smartHome/servlet/requestAuth");
    xchg.exchange(table, "initDeviceConfigParam", config.strURL_initDeviceConfigParam, "/smartHome/servlet/initDeviceConfigParam");
    xchg.exchange(table, "reportCtrlDeviceState", config.strURL_reportCtrlDeviceState, "/smartHome/servlet/reportCtrlDeviceState");
    xchg.exchange(table, "reportDeviceStateList", config.strURL_reportDeviceStateList, "/smartHome/servlet/reportDeviceStateList");
    xchg.exchange(table, "reportAlarm", config.strURL_reportAlarm          , "/smartHome/servlet/reportAlarm");
	
    // this module 收到的
    xchg.exchange(table, "register", config.strURL_register, "/SmartHomeTelcom/register");
    xchg.exchange(table, "requestReportDeviceConfigParam", config.strURL_requestReportDeviceConfigParam, "/SmartHomeTelcom/requestReportDeviceConfigParam");
    xchg.exchange(table, "getCtrlDeviceState", config.strURL_getCtrlDeviceState, "/SmartHomeTelcom/getCtrlDeviceState");
    xchg.exchange(table, "getDeviceStateList", config.strURL_getDeviceStateList, "/SmartHomeTelcom/getDeviceStateList");
    xchg.exchange(table, "controlDevice", config.strURL_controlDevice, "/SmartHomeTelcom/controlDevice");
}
