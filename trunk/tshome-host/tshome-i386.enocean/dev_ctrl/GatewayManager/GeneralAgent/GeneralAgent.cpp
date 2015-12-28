/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgent.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgent.cpp 0001 2012-04-06 09:58:43Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#include <time.h>
#ifndef _DEBUG_THIS
	#define _DEBUG_THIS
#endif

#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x) x
	#define DBG(x)
#endif
#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) &x;

#include <ListenSocket.h>

#include "GeneralAgentHandler.h"
#include "GeneralAgentTcpSocketServer.h"
#include "GeneralAgentTcpSocketConnector.h"

#define DEFAULT_NEXT_HOST "192.168.10.56"
#define DEFAULT_NEXT_HOST_PORT 60000



typedef struct __generalservercfg_t
{
    unsigned short IsUse;
    unsigned short Port;
    char strIP[16];
}
GENERALSERVERCFG_T;

//最多连接的服务器数量
//理论上仅受内存等容量限制 (max 800 allowed)
#define N_WIFI_GW 8

typedef struct __GeneralAgentCfg
{
    // 本机服务端口
    unsigned short ServicePort; // 如果不启用，则设置为0
    // 下位机
    // 暂时写为32个
    GENERALSERVERCFG_T PeerService[N_WIFI_GW];
}
GENERALAGENTCFG_T;

////////////////////////////////////////////////////
// 新加
class __GeneralAgentThread;
__GeneralAgentThread *g_pGeneralAgentThread = NULL;
GENERALAGENTCFG_T g_GeneralAgentConfig = {0};
GeneralAgentHandler g_GeneralAgentHandler;

ListenSocket<CGeneralAgentTcpSocketServer> *g_CGeneralAgentTcpSocketServer;
CGeneralAgentTcpSocketConnector *g_pConnector[N_WIFI_GW];

////////
// 初始化
void InitializeGeneralAgent();
void ExitGeneralAgent();

// 设置服务，
void SetGeneralAgent(unsigned short  port);
////////////

void AddWifiGateway(int nKey, char* szIP, int nPort);


void RemoveWifiGateway(int nKey);


void ConnectAll();

void ConnectAllSync();

// 内部需要一定的时间自行处理
// 调用之后， 10s之后再调用Connect，
void DisconnectAll();

int main(int argc, char *argv[])
{
    int ii;
    InitializeGeneralAgent();

    SetGeneralAgent(DEFAULT_NEXT_HOST_PORT);

    for (ii=0; ii<N_WIFI_GW; ii++)
    {
        AddWifiGateway(ii, (char *)DEFAULT_NEXT_HOST, (unsigned short)(DEFAULT_NEXT_HOST_PORT+ii));
    }

    // 测试连接、断开
    while (1)
    {
        printf("ConnectAll\n");
        ConnectAll();

        while(1)
        {
            sleep(6);
        }

        printf("DisConnectAll\n");
        DisconnectAll();
        sleep(10);
    }

    ExitGeneralAgent();

    return 0;
}

class __GeneralAgentThread:public Thread
{

public:
    void Run()
    {

        __fline;
        printf("thread begin.\n");

        while (IsRunning())
        {
            if (!g_GeneralAgentHandler.GetCount())
            {
                sleep(1);
                __fline;
                printf("idle.\n");
                continue;
            }

            g_GeneralAgentHandler.Select(1,0);
        }


        __fline;
        printf("thread end.\n");

        return;
    }

};
void InitializeGeneralAgent()
{
    int ii;
    memset(&g_GeneralAgentConfig, 0, sizeof(GENERALAGENTCFG_T));
    for (ii=0; ii<N_WIFI_GW; ii++)
    {
        g_pConnector[ii] = NULL;
    }
    g_CGeneralAgentTcpSocketServer = NULL;

    // 线程启动后，不必关，直至进程关闭
    if (g_pGeneralAgentThread == NULL)
    {
        g_pGeneralAgentThread = new __GeneralAgentThread;
    }
}
void ExitGeneralAgent()
{
    if (g_pGeneralAgentThread)
    {
        delete g_pGeneralAgentThread;
        g_pGeneralAgentThread=NULL;
    }
}

void SetGeneralAgent(unsigned short  port)
{
    g_GeneralAgentConfig.ServicePort = port;
}

void AddWifiGateway(int nKey, char* szIP, int nPort)
{
    if (nKey<0 || nKey>=N_WIFI_GW)
    {
        printf("nKey overflow.\n");
        return;
    }

    g_GeneralAgentConfig.PeerService[nKey].IsUse = 1;
    g_GeneralAgentConfig.PeerService[nKey].Port = nPort;
    strcpy(g_GeneralAgentConfig.PeerService[nKey].strIP, szIP);
}

void RemoveWifiGateway(int nKey)
{
    if (nKey<0 || nKey>=N_WIFI_GW)
    {
        printf("nKey overflow.\n");
        return;
    }

    g_GeneralAgentConfig.PeerService[nKey].IsUse = 0;
}

void ConnectAll()
{
    int ii;
    for (ii=0; ii<N_WIFI_GW; ii++)
    {
        if (g_GeneralAgentConfig.PeerService[ii].IsUse==1)
        {
            DBG(
                __fline;
                printf("new CGeneralAgentTcpSocketConnector:%d\n", ii);
            );
            g_pConnector[ii] = new CGeneralAgentTcpSocketConnector(g_GeneralAgentHandler);
            if (g_pConnector[ii])
            {
                bool bRet = g_pConnector[ii]->Open(g_GeneralAgentConfig.PeerService[ii].strIP, g_GeneralAgentConfig.PeerService[ii].Port);
                if (bRet)
                {
                    g_GeneralAgentHandler.Add(g_pConnector[ii]);
                }
                else
                {
                    __trip;
                    // 失败处理
                }
            }
            else
            {
                __trip;
                //失败处理
            }

        }
    }

    if (g_GeneralAgentConfig.ServicePort > 0)
    {
        DBG(
            __fline;
            printf("new g_CGeneralAgentTcpSocketServer:%d\n", g_GeneralAgentConfig.ServicePort);
        );

        g_CGeneralAgentTcpSocketServer = new ListenSocket<CGeneralAgentTcpSocketServer>(g_GeneralAgentHandler);

        if (g_CGeneralAgentTcpSocketServer)
        {
            if (g_CGeneralAgentTcpSocketServer->Bind(g_GeneralAgentConfig.ServicePort) < 0)
            {
                __trip;
                printf("g_CGeneralAgentTcpSocketServer->Bind :%d error\n", g_GeneralAgentConfig.ServicePort);
            }
            else
            {
                g_GeneralAgentHandler.Add(g_CGeneralAgentTcpSocketServer);
                DBG(
                    __fline;
                    printf("g_CGeneralAgentTcpSocketServer->Bind :%d OK\n", g_GeneralAgentConfig.ServicePort);
                );
            }
        }
    }
}

void DisconnectAll()
{
    // 关闭客户端
    for (int ii=0; ii<N_WIFI_GW; ii++)
    {
        if (g_GeneralAgentConfig.PeerService[ii].IsUse==1)
        {
            // 错误
            if (g_pConnector[ii] == NULL)
            {
                __trip;
                continue;
            }
            // else

            DBG(
                __fline;
                printf("g_pConnector[%d]->SetCloseAndDelete()\n", ii);
            );

            g_pConnector[ii]->SetCloseAndDelete();
            //delete g_pConnector[ii];
            //g_pConnector[ii] = NULL; // auto, so do not = here.
        }
    }

    // 关掉服务器
    if (g_GeneralAgentConfig.ServicePort > 0
        && g_CGeneralAgentTcpSocketServer!=NULL)
    {
        DBG(
            __fline;
            printf("g_CGeneralAgentTcpSocketServer->SetCloseAndDelete()\n");
        );
        g_CGeneralAgentTcpSocketServer->SetCloseAndDelete();
        //g_CGeneralAgentTcpSocketServer[ii] = NULL; // auto, so do not = here.
    }
}
