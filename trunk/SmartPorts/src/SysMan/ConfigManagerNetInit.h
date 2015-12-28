/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigManagerNetInit.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigManagerNetInit.h 5884 2012-07-02 09:15:02Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __ConfigManagerNetInit_H__
#define __ConfigManagerNetInit_H__

#include "ConfigMan.h"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../Configs/ConfigNet.h"
//
//#include "ConfigTcpSocketServer.h"
//#include "ConfigTcpSocketConnector.h"
//#include "ConfigConsoleOverTcp.h"
//
//#include "ConfigDatabase.h"
//#include "ConfigWebService.h"

#include "../Configs/ConfigNetworkInterface.h"
//#include "ConfigSmartWebs.h"

typedef enum __cfg_index_t {
    CFG_IDX_BEGIN =  0,	// 起始， 实际没有这种配置，便于统一代码
    CFG_IDX_Net,	
//    CFG_IDX_TcpSocketServer,				// TcpSocketServer 配置
//    CFG_IDX_TcpSocketConnector,				// TcpSocketConnector 配置
//    CFG_IDX_ConsoleOverTcp,				// ConsoleOverTcp 配置
//    CFG_IDX_DataBase,				// 数据库配置
//    CFG_IDX_WebService,				//
    CFG_IDX_NetworkInterface,				//网络接口配置
//    CFG_IDX_SmartWebs,				//网络接口配置
    CFG_IDX_ALL,			// 实际没有这种配置， 用于恢复全部配置
}CFG_INDEX;

#define CFG_NAME_General "General"
#define CFG_NAME_TcpSocketServer "TcpSocketServer"
#define CFG_NAME_TcpSocketConnector "TcpSocketConnector"
#define CFG_NAME_ConsoleOverTcp "ConsoleOverTcp"
#define CFG_NAME_DataBase "DataBase"
#define CFG_NAME_Net "Net"
#define CFG_NAME_WebService "WebService"
#define CFG_NAME_NetworkInterface "NetworkInterface"
#define CFG_NAME_SmartWebs "SmartWebs"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

class CConfigManagerNetInit : public CConfigMan
{
public:
    PATTERN_SINGLETON_DECLARE(CConfigManagerNetInit);

    //! 初始化
    // 不传参数使用默认名字，且为单文件方式
    // 传一个参数使用该名字，为单文件方式
    // 传两个参数使用该名字，为双文件方式
    int initialize(std::string mfile="", std::string sfile="");

    //! 设置默认配置，供GUI界面和网络模块调用
    int SetDefaultConfig(int iConfigType);

private:
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //各个子配置
    CConfigNet		m_configNet;	/*!< 普通配置 */
//
//    CConfigTcpSocketServer			m_configTcpSocketServer;	/*!< dsd */
//    CConfigTcpSocketConnector			m_configTcpSocketConnector;	/*!< dsd */
//    CConfigConsoleOverTcp			m_configConsoleOverTcp;	/*!< dsd */
//
//    CConfigDatabase			m_configDB;	/*!< dsd */
//    CConfigWebService			m_configWebService;	/*!< WebService */
    CConfigNetworkInterface			m_ConfigNetworkInterface;	/*!< IF */
//    CConfigSmartWebs			m_ConfigSmartWebs;	/*!< IF */

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

private:
    CConfigManagerNetInit(){};
    ~CConfigManagerNetInit(){};
};

#define g_Config (*CConfigManagerNetInit::instance())

#endif //__ConfigManagerNetInit_H__
