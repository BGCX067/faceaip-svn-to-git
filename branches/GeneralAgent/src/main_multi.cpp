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
 *     2013-07-22 17:17:41 WuJunjie ver 1.3.2
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <EZThread.h>
#include <EZTimer.h>

#ifdef USE_EZCONFIG
#include "Configs/ConfigManager.h"
#endif //USE_EZCONFIG
#ifdef EXAMPLE_THE_WEBSERVICE
#include "WebService/AgentWebService.h"
#endif
#include "GeneralAgent.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

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

#ifdef abc//EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR

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

    //g_GeneralAgent.Start(&g_ConnPara);
    g_GeneralAgent.Initialize(argc, argv);
    g_GeneralAgent.Start();
#else

    g_GeneralAgent.Initialize(argc, argv);

    // 开启
    g_GeneralAgent.Start();
#endif // #ifdef EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR

#ifdef EXAMPLE_THE_WEBSERVICE

    g_AgentWebService.Start();
#endif

    // main线程等待
    while (1)
    {
        // 测试用
#ifdef TEST_CONN_CREATE_DESTROY
        printf("press any key to g_GeneralAgent.ConnectorDestroy ...\n");
        getchar();

        g_GeneralAgent.ConnectorDestroy();
        printf("g_GeneralAgent.ConnectorDestroy DONE.\n");

        printf("press any key to g_GeneralAgent.ConnectorCreate ...\n");
        getchar();
        g_GeneralAgent.ConnectorCreate();
        printf("g_GeneralAgent.ConnectorCreate DONE.\n");

        continue;
#endif //TEST_CONN_CREATE_DESTROY

#if defined(EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR) || defined(EXAMPLE_GENERALAGENTHTTPSOCKETCONNECTOR)
        //printf("press any key to POST ...\n");
        //getchar();
		sleep(1);

        static time_t ttLastSend = 0;
        static time_t ttNow = 0;
        // 测试用，暂时写在这里
        ttNow  = time(NULL);

        const char *pJson = "[{\"reason\":\"PostTest\",\"result\":\"0\"}]";
        //const char *pUri = "/SmsOverHttp/registerURL";
        const char *pUri = "/WebService/TestIF";

        const char *http_req_head =
            "POST %s HTTP/1.1\r\n"
            "Accept: */*\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\r\n"
            "Host: sms.ezlibs.com\r\n"
            "Connection: Keep-Alive\r\n"
            "Content-length: %d\r\n\r\n"
            "%s";

        char http_req[1024];
        sprintf(http_req, http_req_head, pUri, strlen(pJson), pJson);

        if (ttNow-ttLastSend>1)
        {
            ttLastSend  = time(NULL);
            // 不得加1
            //Send2Socket(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME, http_req, strlen(http_req)+1 );
            g_GeneralAgent.Send2Socket(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME, http_req, strlen(http_req));
            __fline;
            printf("%d sended at:%ld\n", strlen(http_req), time(NULL));
        }
        continue;
#endif //EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR    

    }

    return 0;
}
