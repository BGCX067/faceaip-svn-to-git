/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * mainTeleSerial.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: mainTeleSerial.cpp 5884 2013-07-27 11:19:20Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-27 11:19:20 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <EZThread.h>
#include <EZTimer.h>

#ifdef USE_EZCONFIG
#include "Configs/ConfigManager.h"
#endif //USE_EZCONFIG
#ifdef TeleSerial_FUNC_WEBSERVICE
#include "WebService/AgentWebService.h"
#endif
#include "TeleSerial.h"

#ifdef USE_EZCONFIG
// config fname
const char * const ConfigFile_1= "gm.cfg";
const char * const ConfigFile_2 = "gm.cfg.duplicate";
#endif //USE_EZCONFIG

const char * const copy_right = "@(#) Copyright (c) 2013 ezlibs.com.  All rights reserved";
const char * const version_string = "v 1.3.2 - ";
const char * const desc="P_2013-5-20.gm.1000";


int main(int argc, char *argv[])
{
    //ShowVersion();

    g_TimerManager.Start();
    g_ThreadManager.RegisterMainThread(ThreadGetID());
#ifdef USE_EZCONFIG

    g_Config.initialize(ConfigFile_1, ConfigFile_2);

#endif //USE_EZCONFIG

#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR

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

    g_ConnPara.TcpHost.push_back(__NvrServer);

    //g_TeleSerial.Start(&g_ConnPara);
    g_TeleSerial.Initialize(argc, argv);
    g_TeleSerial.Start();
#else

    g_TeleSerial.Initialize(argc, argv);

    // 开启
    g_TeleSerial.Start();
#endif // #ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR

#ifdef TeleSerial_FUNC_WEBSERVICE

    g_AgentWebService.Start();
#endif

    // main线程等待
    while (1)
    {
    	sleep(100);
    	continue;
    	
        printf("press any key to g_TeleSerial.ConnectorDestroy ...\n");
        getchar();

        g_TeleSerial.ConnectorDestroy();
        printf("g_TeleSerial.ConnectorDestroy DONE.\n");

        printf("press any key to g_TeleSerial.ConnectorCreate ...\n");
        getchar();
        g_TeleSerial.ConnectorCreate();
        printf("g_TeleSerial.ConnectorCreate DONE.\n");
    }

    return 0;
}
