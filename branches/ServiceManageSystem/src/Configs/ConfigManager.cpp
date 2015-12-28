/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigManager.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigManager.cpp 5884 2012-07-02 09:17:36Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-07-02 09:17:36  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigManager.h"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int CConfigManager::SetDefaultConfig(int iConfigType)
{
    int iRet = 0;
    if ((iConfigType > CFG_IDX_ALL) || (iConfigType < CFG_IDX_General))
    {
        tracepoint();
        return CONFIG_APPLY_FILE_ERROR;
    }

    switch(iConfigType)
    {
        case CFG_IDX_General:
        {
            //普通设置
            iRet |= recallConfig(CFG_NAME_General);

            break;
        }
        case CFG_IDX_DataBase:
        {
            //普通设置
            iRet |= recallConfig(CFG_NAME_DataBase);

            break;
        }
        case CFG_IDX_TcpSocketServer:
        {
            //TCP服务器
            iRet |= recallConfig(CFG_NAME_TcpSocketServer);

            break;
        }
        case CFG_IDX_TcpSocketConnector:
        {
            //TCP客户端连接
            iRet |= recallConfig(CFG_NAME_TcpSocketConnector);

            break;
        }
        case CFG_IDX_ConsoleOverTcp:
        {
            //调试终端
            iRet |= recallConfig(CFG_NAME_ConsoleOverTcp);

            break;
        }
        case CFG_IDX_WebService:
        {
            iRet |= recallConfig(CFG_NAME_WebService);

            break;
        }
        case CFG_IDX_PubService:
        {
            iRet |= recallConfig(CFG_NAME_PubService);

            break;
        }
        case CFG_IDX_SmsOverHttp:
        {
            iRet |= recallConfig(CFG_NAME_SmsOverHttp);

            break;
        }
        case CFG_IDX_ALL:
        {
            //恢复全部,网络端
            trace("Recall all \n");
            iRet |= recallConfigAll();
            break;
        }
        default:
        {
            trace("###########default \n");
            break;
        }
    }

    return iRet;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

PATTERN_SINGLETON_IMPLEMENT(CConfigManager);

int CConfigManager::initialize(std::string mfile, std::string sfile)
{
    CConfigMan::initialize(mfile, sfile);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 添加通用配置
    setupConfig(CFG_NAME_General, CFG_IDX_General, m_configGeneral);

    setupConfig(CFG_NAME_TcpSocketServer, CFG_IDX_TcpSocketServer, m_configTcpSocketServer);

    setupConfig(CFG_NAME_TcpSocketConnector, CFG_IDX_TcpSocketConnector, m_configTcpSocketConnector);
    setupConfig(CFG_NAME_ConsoleOverTcp, CFG_IDX_ConsoleOverTcp, m_configConsoleOverTcp);

    setupConfig(CFG_NAME_DataBase, CFG_IDX_DataBase, m_configDB);

    setupConfig(CFG_NAME_WebService, CFG_IDX_WebService, m_configWebService);
    setupConfig(CFG_NAME_PubService, CFG_IDX_PubService, m_configPubService);
    setupConfig(CFG_NAME_SmsOverHttp, CFG_IDX_SmsOverHttp, m_configSmsOverHttp);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 初始化完毕, 保存一次配置
    CConfigMan::saveFile();

    return 0;
}

