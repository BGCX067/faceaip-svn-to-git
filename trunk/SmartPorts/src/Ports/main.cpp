/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgent.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgent.cpp 0001 2012-04-06 09:58:43Z WuJunjie $
 *
 *  Explain:
 *     -GeneralAgent 自创建线程， 不使用main线程-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "CommonInclude.h"

#include <EZThread.h>
#include <EZTimer.h>

#ifdef USE_EZCONFIG
#include "ConfigManager.h"
#endif //USE_EZCONFIG
#ifdef USE_MY_WEBSERVICE
#include "WebService/AgentWebService.h"
#endif
#include "GeneralAgent.h"

#ifdef USE_EZCONFIG
// config fname
const char * const ConfigFile_1= "u_ports.cfg";
const char * const ConfigFile_2 = "u_ports.cfg.duplicate";
#endif //USE_EZCONFIG

const char * const copy_right = "@(#) Copyright (c) 2013 ezlibs.com.  All rights reserved";
const char * const version_string = "v 1.0.0 - ";
const char * const desc="P_2013-5-20.SmartGates.1000";

int main(int argc, char *argv[])
{
    //ShowVersion();

    g_TimerManager.Start();
    g_ThreadManager.RegisterMainThread(ThreadGetID());
#ifdef USE_EZCONFIG

    g_Config.initialize(ConfigFile_1, ConfigFile_2);

#endif //USE_EZCONFIG

#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR

    GENERALAGENTCFG_T g_ConnPara;

    GENERALSERVERCFG_T __NvrServer;
    //连接超时属性 针对tcp客户端
    __NvrServer.iConnTimeOut = 5;
    __NvrServer.iConnTimes = 0;
    //重连属性 针对tcp客户端
    __NvrServer.bEnableRecon = true;
    __NvrServer.iInterval = 3;
    // 对方服务器属性 针对tcp客户端
    __NvrServer.strHost = "192.168.10.24";
    __NvrServer.Port = 6000;
    __NvrServer.strUName = "admin";
    __NvrServer.strUPwd = "12345";
    __NvrServer.strServerName = "NVR_KEEPCONNECTOR_SOCKET_NAME";

    g_ConnPara.Peer.push_back(__NvrServer);

    //g_GeneralAgent.Start(&g_ConnPara);
    g_GeneralAgent.Initialize(argc, argv);
    g_GeneralAgent.Start(NULL);
#else
    g_GeneralAgent.Initialize(argc, argv);

    // 开启
    g_GeneralAgent.Start(NULL);
#endif // #ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR

#ifdef USE_MY_WEBSERVICE
    g_AgentWebService.Start();
#endif

    // main线程等待
    while (1)
    {
        //        __fline;
        //        printf("main thread\n");
        sleep(5);
    }

    return 0;
}
