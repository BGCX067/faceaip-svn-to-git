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
#include "CommonInclude.h"
#include "../Logs.h"
#include <pjlib.h>
#include "GeneralAgent.h"
#include "../Configs/ConfigManager.h"

#ifdef USE_GENERALSOCKETPROCESSOR
#include "GeneralSocketProcessor.h"
#endif

PATTERN_SINGLETON_IMPLEMENT(CGeneralAgent);
void CGeneralAgent::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerGeneralAgent->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerGeneralAgent->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    // 读写和维护socket
    m_pHandlerGeneralAgent->Select(1,0);
    // 其他操作，自定义
    m_pHandlerGeneralAgent->Update();
}

CGeneralAgent::CGeneralAgent():CEZThread("GeneralAgent", TP_DEFAULT, 10)
{
    m_pHandlerGeneralAgent = new GeneralAgentHandler(&m_Log);
    assert(m_pHandlerGeneralAgent);

    /////////////////
    //初始化配置信息
    // 本机服务端口
#ifdef USE_GENERALAGENTTCPSOCKETSERVER
    m_ConfigGeneralAgent.ServicePort = 0; // 如果不启用，则设置为0
#endif
#ifdef USE_GENERALAGENTUDPSOCKETSERVER
    m_ConfigGeneralAgent.UdpServicePort = 0; // 如果不启用，则设置为0
    m_pGeneralAgentUdpSocketServer = NULL;
#endif
    // 本机状态服务端口
#ifdef USE_SOCKET_STATUS_AGENT

    m_ConfigGeneralAgent.StatusServicePort = 0; // 如果不启用，则设置为0
#endif
    //调试命令行接口， 0无效
#ifdef USE_GENERALCONSOLE_OVERTCP

    m_ConfigGeneralAgent.ConsoleOverTcpPort = 0; // 调试命令行接口， 0无效
#endif
#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR
    // tcp 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    //m_ConfigGeneralAgent.Peer.clear();
#endif

#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR
    // http 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    //std::vector<GENERALSERVERCFG_T_TSIPC> PeerHttp;
#endif
    //memset(&m_ConfigGeneralAgent, 0, sizeof(GENERALAGENTCFG_T_TSIPC));
    /////////////////////
#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR

    //m_Connector.clear();
#endif

#ifdef USE_GENERALAGENTTCPSOCKETSERVER

    m_pGeneralAgentTcpSocketServer = NULL;
#endif

    ////////////////////////
#ifdef USE_SOCKET_STATUS_AGENT

    m_pSocketStatusAgent = NULL;
    m_ConfigGeneralAgent.StatusServicePort = THE_STATUS_AGENT_PORT;
#endif

    // 调试终端
#ifdef USE_GENERALCONSOLE_OVERTCP

    m_pGeneralConsoleOverTcp = NULL;
    m_ConfigGeneralAgent.ConsoleOverTcpPort = PORT_GENERAL_CONSOLE_OVER_TCP;
#endif

    ///////////////////////////////////////////////
}
CGeneralAgent::~CGeneralAgent()
{
    if (m_pHandlerGeneralAgent)
    {
        delete m_pHandlerGeneralAgent;
    }
}


#ifdef USE_GENERALAGENTTCPSOCKETSERVER
void CGeneralAgent::SetGeneralAgent(unsigned short  port)
{
    m_ConfigGeneralAgent.ServicePort = port;
}
#endif //USE_GENERALAGENTTCPSOCKETSERVER

#ifdef USE_GENERALAGENTUDPSOCKETSERVER
void CGeneralAgent::SetGeneralUdpAgent(unsigned short  port)
{
    m_ConfigGeneralAgent.UdpServicePort = port;
}
#endif//USE_GENERALAGENTUDPSOCKETSERVER

#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR
void CGeneralAgent::AddSubHost(std::string strHost, int nPort, std::string strServerName)
{
    // todo 检查不要重复加

    GENERALSERVERCFG_T_TSIPC __peer;
    __peer.Port = nPort;

    __peer.strHost = strHost;
    __peer.strServerName = strServerName;

    m_ConfigGeneralAgent.Peer.push_back(__peer);
}
#endif // USE_GENERALAGENTTCPSOCKETCONNECTOR

#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR
void CGeneralAgent::AddSubHttpHost(std::string strHost, int nPort, std::string strServerName)
{
    // todo 检查不要重复加

    GENERALSERVERCFG_T_TSIPC __peer;
    __peer.Port = nPort;
    __peer.strHost = strHost;
    __peer.strServerName = strServerName;

    m_ConfigGeneralAgent.PeerHttp.push_back(__peer);
}

#endif // USE_GENERALAGENTHTTPKEEPCONNECTOR

Socket * CGeneralAgent::GetSocketByName(std::string strServerName)
{
    return m_pHandlerGeneralAgent->GetSocketByName(strServerName);
}

int CGeneralAgent::Send2Socket(std::string strServerName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerGeneralAgent->GetSocketByName(strServerName);
    CGeneralAgentUdpSocket *pUdpServer = dynamic_cast<CGeneralAgentUdpSocket *>(p);
    if (pUdpServer)
    {
        return pUdpServer->SendtoAllPeer(pData, Len);
    }
    else if (p)
    {
        p->SendBuf(pData, Len);
    }
    else
    {
        __trip;
        printf("Send2Socket:%s, Len:%ld failed\n", strServerName.c_str(), Len);

        return -1;
    }

    return Len;
}

void CGeneralAgent::ConnectAll()
{
#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR
    for (unsigned int ii=0; ii<m_ConfigGeneralAgent.PeerHttp.size(); ii++)
    {
        CGeneralAgentHttpKeepConnector *_pConnector
        = new CGeneralAgentHttpKeepConnector(m_pHandlerGeneralAgent, m_ConfigGeneralAgent.PeerHttp[ii].strServerName);
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigGeneralAgent.PeerHttp[ii].strHost.c_str(), m_ConfigGeneralAgent.PeerHttp[ii].Port);
            if (bRet)
            {
                printf("Connector:%d %s:%d Open\n"
                       , ii
                       , m_ConfigGeneralAgent.PeerHttp[ii].strHost.c_str()
                       , m_ConfigGeneralAgent.PeerHttp[ii].Port);

                m_pHandlerGeneralAgent->Add(_pConnector);
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
#endif //USE_GENERALAGENTHTTPKEEPCONNECTOR

#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR
    for (unsigned int ii=0; ii<m_ConfigGeneralAgent.Peer.size(); ii++)
    {
        CGeneralAgentTcpSocketConnector *_pConnector = new CGeneralAgentTcpSocketConnector(m_pHandlerGeneralAgent);
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigGeneralAgent.Peer[ii].strHost.c_str(), m_ConfigGeneralAgent.Peer[ii].Port);
            if (bRet)
            {
                printf("Connector:%d %s:%d Open\n"
                       , ii
                       , m_ConfigGeneralAgent.Peer[ii].strHost.c_str()
                       , m_ConfigGeneralAgent.Peer[ii].Port);

                m_pHandlerGeneralAgent->Add(_pConnector);
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
#endif

#ifdef USE_SOCKET_STATUS_AGENT
    if(m_ConfigGeneralAgent.StatusServicePort>0
       && m_pSocketStatusAgent == NULL)
    {
        m_pSocketStatusAgent = new ListenSocket<CSocketStatusAgent>(m_pHandlerGeneralAgent);

        if (m_pSocketStatusAgent)
        {
            if (m_pSocketStatusAgent->Bind(m_ConfigGeneralAgent.StatusServicePort) < 0)
            {
                __trip;
                printf("m_pSocketStatusAgent->Bind :%d error\n", m_ConfigGeneralAgent.StatusServicePort);
            }
            else
            {
                m_pHandlerGeneralAgent->Add(m_pSocketStatusAgent);
                //DBG_CODE(
                __fline;
                printf("m_pSocketStatusAgent->Bind :%d OK\n", m_ConfigGeneralAgent.StatusServicePort);
                //);
            }
        }
    }// if(m_pSocketStatusAgent == NULL)
#endif

#ifdef USE_GENERALCONSOLE_OVERTCP
    if(m_ConfigGeneralAgent.ConsoleOverTcpPort>0
       && m_pGeneralConsoleOverTcp == NULL)
    {
        m_pGeneralConsoleOverTcp = new ListenSocket<CGeneralConsoleOverTcp>(*m_pHandlerGeneralAgent);

        if (m_pGeneralConsoleOverTcp)
        {
            if (m_pGeneralConsoleOverTcp->Bind(m_ConfigGeneralAgent.ConsoleOverTcpPort) < 0)
            {
                __trip;
                printf("m_pGeneralConsoleOverTcp->Bind :%d FAILED\n", m_ConfigGeneralAgent.ConsoleOverTcpPort);
            }
            else
            {
                m_pHandlerGeneralAgent->Add(m_pGeneralConsoleOverTcp);
                //DBG_CODE(
                //__fline;
                printf("ConsoleOverTcp On:%d\n", m_ConfigGeneralAgent.ConsoleOverTcpPort);
                //);
                LOG4CPLUS_INFO(LOG_IPC, "ConsoleOverTcp On:"<<m_ConfigGeneralAgent.ConsoleOverTcpPort);
            }
        }
    }// if(m_pGeneralConsoleOverTcp == NULL)
    else
    {
                __fline;
                printf("do not m_pGeneralConsoleOverTcp->Bind :%d\n", m_ConfigGeneralAgent.ConsoleOverTcpPort);
    }
#endif //USE_GENERALCONSOLE_OVERTCP

#ifdef USE_GENERALAGENTTCPSOCKETSERVER
    if (m_ConfigGeneralAgent.ServicePort > 0)
    {
        DBG_CODE(
            __fline;
            printf("new m_pGeneralAgentTcpSocketServer:%d\n", m_ConfigGeneralAgent.ServicePort);
        );

        m_pGeneralAgentTcpSocketServer = new ListenSocket<CGeneralAgentTcpSocketServer>(*m_pHandlerGeneralAgent);

        if (m_pGeneralAgentTcpSocketServer)
        {
            if (m_pGeneralAgentTcpSocketServer->Bind(m_ConfigGeneralAgent.ServicePort) < 0)
            {
                __trip;
                printf("m_pGeneralAgentTcpSocketServer->Bind :%d error\n", m_ConfigGeneralAgent.ServicePort);
            }
            else
            {
                m_pHandlerGeneralAgent->Add(*m_pGeneralAgentTcpSocketServer);
                // DBG_CODE(
                __fline;
                printf("m_pGeneralAgentTcpSocketServer->Bind :%d OK\n", m_ConfigGeneralAgent.ServicePort);
                // );
            }
        }
    }
#endif

#ifdef USE_GENERALAGENTUDPSOCKETSERVER
    if (m_ConfigGeneralAgent.UdpServicePort > 0)
    {
        m_pGeneralAgentUdpSocketServer = new CGeneralAgentUdpSocket(*m_pHandlerGeneralAgent, THE_UDPSVR_AGENT_NAME);

        if (m_pGeneralAgentUdpSocketServer)
        {
            if (m_pGeneralAgentUdpSocketServer->Bind(m_ConfigGeneralAgent.UdpServicePort, 10) < 0)
            {
                LOG4CPLUS_ERROR(LOG_IPC, "IPCServer(UDP) Bind:"<<m_ConfigGeneralAgent.UdpServicePort << " failed");
            }
            else
            {
                m_pHandlerGeneralAgent->Add(m_pGeneralAgentUdpSocketServer);
                LOG4CPLUS_INFO(LOG_IPC, "IPCServer(UDP) Bind:"<<m_ConfigGeneralAgent.UdpServicePort << " OK");
                printf("IPCServer(UDP) On:%d\n", m_ConfigGeneralAgent.UdpServicePort);
            }
        }
        else
        {
            LOG4CPLUS_ERROR(LOG_IPC, "New IPCServer(UDP) failed");
        }
    }
#endif
}

void CGeneralAgent::DisconnectAll()
{
#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR
    // 关闭客户端
    for (size_t ii=0; ii<m_ConfigGeneralAgent.Peer.size(); ii++)
    {
        printf("Disconnect %d -- %s\n", ii, m_ConfigGeneralAgent.Peer[ii].strHost.c_str());
        // todo
    }
#endif
#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR
    // 关http客户端
    for (size_t ii=0; ii<m_ConfigGeneralAgent.PeerHttp.size(); ii++)
    {
        printf("Disconnect %d -- %s\n", ii, m_ConfigGeneralAgent.PeerHttp[ii].strHost.c_str());
        // todo
    }
#endif
#ifdef USE_GENERALAGENTTCPSOCKETSERVER
    // 关掉服务器
    if (m_ConfigGeneralAgent.ServicePort > 0
        && m_pGeneralAgentTcpSocketServer!=NULL)
    {
        DBG_CODE(
            __fline;
            printf("m_pGeneralAgentTcpSocketServer->SetCloseAndDelete()\n");
        );
        m_pGeneralAgentTcpSocketServer->SetCloseAndDelete();
        //m_pGeneralAgentTcpSocketServer[ii] = NULL; // auto, so do not = here.
    }
#endif
    // status server 不关闭
}

BOOL CGeneralAgent::Start(GENERALSERVERCFG_T_TSIPC *pConfig)
{
    // 已经启动
    if (m_bLoop)
    {
        return TRUE;
    }
    // 调试的时候使用默认参数
    SetDefaultConfig();

    ConnectAll();

    int ret = CreateThread();

    ARG_USED(ret);

    return TRUE;
}

BOOL CGeneralAgent::Stop()
{
    return TRUE;
}

void CGeneralAgent::SetDefaultConfig()
{
    CConfigIPC __cfgIPC;
    __cfgIPC.update();
	
    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();
#ifdef USE_GENERALAGENTTCPSOCKETSERVER

    SetGeneralAgent(THE_TCPSVR_AGENT_PORT);
#endif
#ifdef USE_GENERALAGENTUDPSOCKETSERVER
    SetGeneralUdpAgent(__cfgIPC.getConfig().IpcPort);
#endif
#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR

    for (int ii=0; ii<6; ii++)
    {
        g_GeneralAgentTSIPC.AddSubHost(DEFAULT_NEXT_HOST, (unsigned short)(DEFAULT_NEXT_HOST_PORT));
    }
#endif

#ifdef USE_GENERALCONSOLE_OVERTCP
	m_ConfigGeneralAgent.ConsoleOverTcpPort = __cfgGeneral.getConfig().portConsoleOverTcp;
#endif //USE_GENERALCONSOLE_OVERTCP

#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR
    for (int ii=0; ii<1; ii++)
    {
        g_GeneralAgentTSIPC.AddSubHttpHost(DEFAULT_NEXT_HTTP_HOST, (unsigned short)(DEFAULT_NEXT_HTTP_PORT));
    }
#endif //USE_GENERALAGENTHTTPKEEPCONNECTOR
}

void CGeneralAgent::ThreadProc()
{
    while (m_bLoop)
    {
        Run();

        /////////////////
#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR

        static time_t ttLastSend = 0;
        static time_t ttNow = 0;
        // 测试用，暂时写在这里
        ttNow  = time(NULL);
        const char *http_req =
            "GET /test/t_get.php?name=joy&age=100 HTTP/1.1\r\n"
            "Accept: */*\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\r\n"
            "Host: 192.168.10.246\r\n"
            "Connection: Keep-Alive\r\n"
            "\r\n";

        if (ttNow-ttLastSend>1)
        {
            ttLastSend  = time(NULL);
            // 不得加1
            //Send2Socket(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME, http_req, strlen(http_req)+1 );
            Send2Socket(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME, http_req, strlen(http_req));
            __fline;
            printf("%d sended\n", strlen(http_req)+1);
        }
#endif //USE_GENERALAGENTHTTPKEEPCONNECTOR

    } //while (m_bLoop)
}
