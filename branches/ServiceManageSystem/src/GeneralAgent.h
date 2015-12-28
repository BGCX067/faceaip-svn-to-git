/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgent.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
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
#ifndef _GENERALAGENT_H
#define _GENERALAGENT_H

#include "CommonInclude.h"
#include <time.h>
#include <vector>
#include "GeneralAgentDef.h" /* default def */

#include "EZThread.h"
#include "EZTimer.h"
#include "ListenSocket.h"

// 反应堆
#include "GeneralAgentHandler.h"
// tcp server
#ifdef USE_GENERALAGENTTCPSOCKETSERVER
#include "GeneralAgentTcpSocketServer.h"
#endif
#ifdef USE_GENERALAGENTHTTPSERVER
#include "GeneralAgentHttpServer.h"
#endif
#ifdef USE_GENERALAGENTUDPSOCKETSERVER
#include "GeneralAgentUdpSocket.h"
#endif
// tcp client, auto conn
#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR
#include "GeneralAgentTcpSocketConnector.h"
#endif
// http client, auto conn when brocken
#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR
#include "GeneralAgentHttpKeepConnector.h"
#endif

#ifdef USE_SOCKET_STATUS_AGENT
#include "SocketStatusAgent.h"
#endif
#ifdef USE_GENERALCONSOLE_OVERTCP
#include "GeneralConsoleOverTcp.h"
#endif

typedef struct __generalservercfg_t
{
    unsigned short IsUse;
    unsigned short Port;
    std::string strHost;
    // will find by name
    std::string strServerName;

    int iConnTimeOut;//超时时间
    int iConnTimes; //连接尝试次数（保留）

    int iInterval; //[in] 重连间隔，单位毫秒，参数默认值为30秒
    int bEnableRecon ;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

    std::string strUName;
    std::string strUPwd;

    __generalservercfg_t()
    {
        IsUse = 0;
        Port = 0;
        strHost = "";
        // will find by name
        strServerName="";

        iConnTimeOut = 0;//超时时间
        iConnTimes = 0; //连接尝试次数（保留）

        iInterval = 0; //[in] 重连间隔，单位毫秒，参数默认值为30秒
        bEnableRecon  = 0;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

        strUName = "";
        strUPwd = "";
    }
}GENERALSERVERCFG_T;


typedef struct __GeneralAgentCfg
{
    // TCP服务端口
#ifdef USE_GENERALAGENTTCPSOCKETSERVER
    unsigned short ServicePort; // 如果不启用，则设置为0
#endif
#ifdef USE_GENERALAGENTHTTPSERVER
    unsigned short HttpServicePort; // 如果不启用，则设置为0
#endif
    // UDP服务端口
#ifdef USE_GENERALAGENTUDPSOCKETSERVER
    unsigned short UdpServicePort; // 如果不启用，则设置为0
#endif
    // 状态服务端口
#ifdef USE_SOCKET_STATUS_AGENT
    unsigned short StatusServicePort; // 如果不启用，则设置为0
#endif
    //调试命令行接口， 0无效
#ifdef USE_GENERALCONSOLE_OVERTCP
    unsigned short ConsoleOverTcpPort; // 调试命令行接口， 0无效
#endif
#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR
    // tcp 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    std::vector<GENERALSERVERCFG_T> Peer;
#endif

#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR
    // http 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    std::vector<GENERALSERVERCFG_T> PeerHttp;
#endif

}
GENERALAGENTCFG_T;

#define g_GeneralAgent (*CGeneralAgent::instance())

class CGeneralAgent : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CGeneralAgent);
    CGeneralAgent();
    virtual ~CGeneralAgent();
    ////////
    void Initialize(int argc, char * argv[]);

    BOOL Start(GENERALAGENTCFG_T *pConfig=NULL);
    BOOL Stop();
    void ThreadProc();
    void SetDefaultConfig();

    // 设置服务，
#ifdef USE_GENERALAGENTTCPSOCKETSERVER
    void SetGeneralAgent(unsigned short  port);
#endif
#ifdef USE_GENERALAGENTHTTPSERVER
    void SetGeneralHttpAgent(unsigned short  port);
#endif
#ifdef USE_GENERALAGENTUDPSOCKETSERVER
    void SetGeneralUdpAgent(unsigned short  port);
#endif

#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR

    void AddSubHost(std::string strHost, int nPort, std::string strName="");
#endif //USE_GENERALAGENTTCPSOCKETCONNECTOR
#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR

    void AddSubHttpHost(std::string strHost, int nPort, std::string strServerName=CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME);
#endif

    int ConnectorCreate();

    // 内部需要一定的时间自行处理
    // 为确保内部销毁完成，一般调用之后， 10s之后再调用Connect，
    int ConnectorDestroy();

    void Run();

    int Send2Socket(std::string strSocketName, const char *pData, size_t Len);
    Socket * GetSocketByName(std::string strServerName);


    void TimerProcMaintain();
    const std::string &GetVersion();
    unsigned int GetRunPeriod();
    time_t GetStartTime();

private:
    GENERALAGENTCFG_T m_ConfigGeneralAgent;
    CDoubleOutLog m_Log;
    GeneralAgentHandler *m_pHandlerGeneralAgent;

#ifdef USE_SOCKET_STATUS_AGENT

    ListenSocket<CSocketStatusAgent> *m_pSocketStatusAgent;
#endif // USE_SOCKET_STATUS_AGENT

#ifdef USE_GENERALCONSOLE_OVERTCP

    ListenSocket<CGeneralConsoleOverTcp> *m_pGeneralConsoleOverTcp;
#endif // USE_GENERALCONSOLE_OVERTCP

#ifdef USE_GENERALAGENTTCPSOCKETSERVER

    ListenSocket<CGeneralAgentTcpSocketServer> *m_pGeneralAgentTcpSocketServer;
#endif // USE_GENERALAGENTTCPSOCKETSERVER
#ifdef USE_GENERALAGENTHTTPSERVER

    ListenSocket<CGeneralAgentHttpServer> *m_pGeneralAgentHttpServer;
#endif // USE_GENERALAGENTHTTPSERVER


#ifdef USE_GENERALAGENTUDPSOCKETSERVER
	CGeneralAgentUdpSocket *m_pGeneralAgentUdpSocketServer;
#endif

    int InitializeLogs();
    CEZTimer          m_TimerMaintain;          //保活功能定时器
    // 启动时间
    unsigned int m_iRunPeriod;
    time_t m_ttStartTime;

}
; //CGeneralAgent

#endif //_GENERALAGENT_H
