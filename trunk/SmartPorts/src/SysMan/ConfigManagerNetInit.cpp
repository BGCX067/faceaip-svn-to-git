/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigManagerNetInit.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigManagerNetInit.cpp 5884 2012-07-02 09:17:36Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-07-02 09:17:36  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigManagerNetInit.h"

#ifdef _DEBUG
	#define DEB_CODE(x) x
	#define DBG_CODE(x) x
#else
	#define DEB_CODE(x) x
	#define DBG_CODE(x) x
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int CConfigManagerNetInit::SetDefaultConfig(int iConfigType)
{
    int iRet = 0;
    if ((iConfigType > CFG_IDX_ALL) || (iConfigType <= CFG_IDX_BEGIN))
    {
        tracepoint();
        return CONFIG_APPLY_FILE_ERROR;
    }

    switch(iConfigType)
    {
        case CFG_IDX_Net:
        {
            iRet |= recallConfig(CFG_NAME_Net);

            break;
        }
        //        case CFG_IDX_DataBase:
        //        {
        //            //普通设置
        //            iRet |= recallConfig(CFG_NAME_DataBase);
        //
        //            break;
        //        }
        //        case CFG_IDX_TcpSocketServer:
        //        {
        //            //TCP服务器
        //            iRet |= recallConfig(CFG_NAME_TcpSocketServer);
        //
        //            break;
        //        }
        //        case CFG_IDX_TcpSocketConnector:
        //        {
        //            //TCP客户端连接
        //            iRet |= recallConfig(CFG_NAME_TcpSocketConnector);
        //
        //            break;
        //        }
        //        case CFG_IDX_ConsoleOverTcp:
        //        {
        //            //调试终端
        //            iRet |= recallConfig(CFG_NAME_ConsoleOverTcp);
        //
        //            break;
        //        }
        //        case CFG_IDX_WebService:
        //        {
        //            iRet |= recallConfig(CFG_NAME_WebService);
        //
        //            break;
        //        }
        case CFG_IDX_NetworkInterface:
        {
            DBG_CODE(
                __fline;
                printf("recallConfig CFG_IDX_NetworkInterface \n");
            );

            iRet |= recallConfig(CFG_NAME_NetworkInterface);

            break;
        }
        //        case CFG_IDX_SmartWebs:
        //        {
        //            iRet |= recallConfig(CFG_NAME_SmartWebs);
        //
        //            break;
        //        }
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

PATTERN_SINGLETON_IMPLEMENT(CConfigManagerNetInit);

int CConfigManagerNetInit::initialize(std::string mfile, std::string sfile)
{
    int iret = 0;
    iret = CConfigMan::initialize(mfile, sfile);
    if (iret<0)
    {
        // do not ini cfg here, so need
        return -1;
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 添加通用配置
    //    m_mapTranslate.insert(valueType(CFG_NAME_General, CFG_IDX_General));
    setupConfig(CFG_NAME_Net, CFG_IDX_Net, m_configNet);
    //
    //    m_mapTranslate.insert(valueType(CFG_NAME_TcpSocketServer, CFG_IDX_TcpSocketServer));
    //    setupConfig(CFG_NAME_TcpSocketServer, m_configTcpSocketServer);
    //
    //    m_mapTranslate.insert(valueType(CFG_NAME_TcpSocketConnector, CFG_IDX_TcpSocketConnector));
    //    setupConfig(CFG_NAME_TcpSocketConnector, m_configTcpSocketConnector);
    //    m_mapTranslate.insert(valueType(CFG_NAME_ConsoleOverTcp, CFG_IDX_ConsoleOverTcp));
    //    setupConfig(CFG_NAME_ConsoleOverTcp, m_configConsoleOverTcp);
    //
    //    m_mapTranslate.insert(valueType(CFG_NAME_DataBase, CFG_IDX_DataBase));
    //    setupConfig(CFG_NAME_DataBase, m_configDB);
    //
    //    m_mapTranslate.insert(valueType(CFG_NAME_WebService, CFG_IDX_WebService));
    //    setupConfig(CFG_NAME_WebService, m_configWebService);

    //    m_mapTranslate.insert(valueType(CFG_NAME_NetworkInterface, CFG_IDX_NetworkInterface));
    setupConfig(CFG_NAME_NetworkInterface, CFG_IDX_NetworkInterface, m_ConfigNetworkInterface);

    //    m_mapTranslate.insert(valueType(CFG_NAME_SmartWebs, CFG_IDX_SmartWebs));
    //    setupConfig(CFG_NAME_SmartWebs, m_ConfigSmartWebs);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 初始化完毕, 保存一次配置
    CConfigMan::saveFile();

    return 0;
}

