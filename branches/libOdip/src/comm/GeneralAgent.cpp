/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgent.cpp - _explain_
 *
 * Copyright (C) 2011 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
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
#include "GeneralAgent.h"

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
    //m_pHandlerGeneralAgent = new GeneralAgentHandler(&m_Log);
    //assert(m_pHandlerGeneralAgent);

    /////////////////
    //初始化配置信息
    // 本机服务端口
#ifdef USE_GENERALAGENTTCPSOCKETSERVER

    m_ConfigGeneralAgent.ServicePort = 0; // 如果不启用，则设置为0
#endif
#ifdef USE_GENERALAGENTHTTPSERVER

    m_ConfigGeneralAgent.HttpServicePort = 0; // 如果不启用，则设置为0
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
    //std::vector<GENERALSERVERCFG_T> PeerHttp;
#endif
    //memset(&m_ConfigGeneralAgent, 0, sizeof(GENERALAGENTCFG_T));
    /////////////////////
#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR

    //m_Connector.clear();
#endif

#ifdef USE_GENERALAGENTTCPSOCKETSERVER

    m_pGeneralAgentTcpSocketServer = NULL;
#endif
#ifdef USE_GENERALAGENTHTTPSERVER

    m_pGeneralAgentHttpServer = NULL;
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

#ifdef USE_GENERALAGENTHTTPSERVER
void CGeneralAgent::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigGeneralAgent.HttpServicePort = port;
}
#endif

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

    GENERALSERVERCFG_T __peer;
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

    GENERALSERVERCFG_T __peer;
    __peer.Port = nPort;
    __peer.strHost = strHost;
    __peer.strServerName = strServerName;

    m_ConfigGeneralAgent.PeerHttp.push_back(__peer);
}

#endif // USE_GENERALAGENTHTTPKEEPCONNECTOR

int CGeneralAgent::Send2Socket(std::string strServerName, const char *pData, size_t Len)
{
    if (m_pHandlerGeneralAgent==NULL)
    {
        return -1;
    }

    Socket *p = m_pHandlerGeneralAgent->GetSocketByName(strServerName);

    if (p)
    {
        p->SendBuf(pData, Len);
    }
    else
    {
        __trip;
        printf("Send2Socket %s, Len:%d failed\n", strServerName.c_str(), Len);
        return -2;
    }

    return 0;
}

Socket * CGeneralAgent::GetSocketByName(std::string strServerName)
{
    if (m_pHandlerGeneralAgent==NULL)
    {
    __trip;
        return NULL;
    }

    return m_pHandlerGeneralAgent->GetSocketByName(strServerName);
}

int CGeneralAgent::ConnectAll()
{
    if (m_pHandlerGeneralAgent==NULL)
    {
        return -1;
    }

#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR
    for (unsigned int ii=0; ii<m_ConfigGeneralAgent.Peer.size(); ii++)
    {
        CGeneralAgentTcpSocketConnector *_pConnector = new CGeneralAgentTcpSocketConnector(*m_pHandlerGeneralAgent
                , m_ConfigGeneralAgent.Peer[ii].strServerName
                , m_ConfigGeneralAgent.Peer[ii].iConnTimeOut
                , m_ConfigGeneralAgent.Peer[ii].bEnableRecon);

        _pConnector->SetAccount(m_ConfigGeneralAgent.Peer[ii].strUName, m_ConfigGeneralAgent.Peer[ii].strUPwd);

        if (_pConnector)
        {
            //                            DEB_CODE(
            printf("Connector[%s]:%d %s:%d Open "
                   , m_ConfigGeneralAgent.Peer[ii].strServerName.c_str()
                   , ii
                   , m_ConfigGeneralAgent.Peer[ii].strHost.c_str()
                   , m_ConfigGeneralAgent.Peer[ii].Port);
            //			);

            bool bRet = _pConnector->Open(m_ConfigGeneralAgent.Peer[ii].strHost.c_str(), m_ConfigGeneralAgent.Peer[ii].Port);
            if (bRet)
            {
                //                            DEB_CODE(
                printf("Succeeded.\n");
                m_pHandlerGeneralAgent->Add(_pConnector);
                //			);
            }
            else
            {
                //                 DEB_CODE(
                printf("Failed.\n");
                //		);
                // 失败处理
                delete _pConnector;
                return -1;

            }
        }
        else
        {
            __trip;
            return -1;
            //失败处理
        }
    }
#endif

    return 0;
}


int CGeneralAgent::CloseAll()
{
    if (m_pHandlerGeneralAgent==NULL)
    {
        return -1;
    }

    m_pHandlerGeneralAgent->SocketSetCloseAndDelete();

    return 0;
}

int CGeneralAgent::DisconnectAll()
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

    return 0;
}

BOOL CGeneralAgent::Start(GENERALAGENTCFG_T *pConfig)
{
    int iRet = 0;
    // 已经启动
    if (m_bLoop)
    {
        return TRUE;
    }
    //__trip;

    if (pConfig)
    {
        m_ConfigGeneralAgent = *pConfig;
    }
    else
    {
        // 调试的时候使用默认参数
        SetDefaultConfig();
    }
    //__trip;

    if (m_pHandlerGeneralAgent==NULL)
    {
        //__trip;
        m_pHandlerGeneralAgent = new GeneralAgentHandler(&m_Log);
        assert(m_pHandlerGeneralAgent);
    }
    //__trip;

    iRet = ConnectAll();
    if (iRet != 0)
    {
        //__trip;
        printf("ConnectAll failed:%d\n", iRet);
        return FALSE;
    }
    //__trip;
	#if 0
    // 启动处理线程
#ifdef USE_GENERALSOCKETPROCESSOR
    g_GeneralSocketProcessor.Start();
#endif
#endif
   // __trip;

    iRet = CreateThread();
    if (iRet < 0)
    {
        __trip;
        return FALSE;
    }

    //__trip;
    return TRUE;
}

BOOL CGeneralAgent::Stop()
{
    if (m_bLoop == FALSE)
    {
        //__trip;
        return TRUE;
    }
   // __trip;

    CloseAll();
    //__trip;
    // 启动处理线程
//#ifdef USE_GENERALSOCKETPROCESSOR

    //g_GeneralSocketProcessor.Stop();
//#endif

    int ret = DestroyThread();

    sleep(1);
    //__trip;

    if (m_pHandlerGeneralAgent!=NULL)
    {
        //__trip;
        delete m_pHandlerGeneralAgent;
        m_pHandlerGeneralAgent= NULL;
        //__trip;
    }
    //__trip;
    ARG_USED(ret);

    return TRUE;
}

void CGeneralAgent::SetDefaultConfig()
{
#ifdef USE_GENERALAGENTTCPSOCKETSERVER
    SetGeneralAgent(THE_TCPSVR_AGENT_PORT);
#endif
#ifdef USE_GENERALAGENTHTTPSERVER

    SetGeneralHttpAgent(THE_HTTP_SVR_AGENT_PORT);
#endif

#ifdef USE_GENERALAGENTUDPSOCKETSERVER

    SetGeneralUdpAgent(THE_UDPSVR_AGENT_PORT);
#endif
#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR

    for (int ii=0; ii<1; ii++)
    {
        AddSubHost(DEFAULT_NEXT_HOST, (unsigned short)(DEFAULT_NEXT_HOST_PORT));
    }
#endif
#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR
    for (int ii=0; ii<1; ii++)
    {
        AddSubHttpHost(DEFAULT_NEXT_HTTP_HOST, (unsigned short)(DEFAULT_NEXT_HTTP_PORT));
    }
#endif //USE_GENERALAGENTHTTPKEEPCONNECTOR
}

void CGeneralAgent::ThreadProc()
{
    while (m_bLoop)
    {
        Run();

    } //while (m_bLoop)
}
