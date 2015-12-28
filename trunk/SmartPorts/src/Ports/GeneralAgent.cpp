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
#include "GeneralAgent.h"

#ifdef USE_GENERALSOCKETPROCESSOR
#include "GeneralSocketProcessor.h"
#endif

#include "ConfigManager.h"
#include <ez_fs.h>
#include "../Logs.h"

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

int CGeneralAgent::SetSubHostOpened(size_t ii, int bOpened)
{
    if (m_ConfigGeneralAgent.Peer.size() <= ii)
    {
        return -1;
    }

    CEZLock __lock(m_mutexConfigGeneralAgent);
    m_ConfigGeneralAgent.Peer[ii].IsOpened = bOpened;

    return 0;
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
        printf("Send2Socket %s, Len:%ul failed\n", strServerName.c_str(), Len);
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

int CGeneralAgent::ConnectorCreate()
{
#ifdef USE_GENERALAGENTHTTPKEEPCONNECTOR
    for (unsigned int ii=0; ii<m_ConfigGeneralAgent.PeerHttp.size(); ii++)
    {
        CGeneralAgentHttpKeepConnector *_pConnector
        = new CGeneralAgentHttpKeepConnector(*m_pHandlerGeneralAgent, m_ConfigGeneralAgent.PeerHttp[ii].strServerName);
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
        // 耗时操作，放在定时器中维护
        // dns配置有问题的时候比较费时间
#if 1
        SetSubHostOpened(ii, 0);
        continue;
#else

        CGeneralAgentTcpSocketConnector *_pConnector = new CGeneralAgentTcpSocketConnector(*m_pHandlerGeneralAgent
                , m_ConfigGeneralAgent.Peer[ii].strServerName
                , m_ConfigGeneralAgent.Peer[ii].iConnTimeOut
                , m_ConfigGeneralAgent.Peer[ii].bEnableRecon);

        if (_pConnector)
        {
            //                            DEB_CODE(
            printf("Connector[%s]:%d Host--%s:%d Open "
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
                //			);

                m_pHandlerGeneralAgent->Add(_pConnector);

                SetSubHostOpened(ii, 1);
            }
            else
            {
                //                 DEB_CODE(
                printf("Failed.\n");
                //		);
                __trip;
                SetSubHostOpened(ii, 0);

                // 失败处理
                delete _pConnector;
            }
        }
        else
        {
            __trip;
            //失败处理
        }
#endif // 耗时操作

    }
#endif

#ifdef USE_SOCKET_STATUS_AGENT
    if(m_ConfigGeneralAgent.StatusServicePort>0
       && m_pSocketStatusAgent == NULL)
    {
        m_pSocketStatusAgent = new ListenSocket<CSocketStatusAgent>(*m_pHandlerGeneralAgent);

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
                //DBG(
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
                printf("m_pGeneralConsoleOverTcp->Bind :%d error\n", m_ConfigGeneralAgent.ConsoleOverTcpPort);
            }
            else
            {
                m_pHandlerGeneralAgent->Add(m_pGeneralConsoleOverTcp);
                //DBG(
                __fline;
                printf("m_pGeneralConsoleOverTcp->Bind :%d OK\n", m_ConfigGeneralAgent.ConsoleOverTcpPort);
                //);
            }
        }
    }// if(m_pGeneralConsoleOverTcp == NULL)
#endif //USE_GENERALCONSOLE_OVERTCP

#ifdef USE_GENERALAGENTTCPSOCKETSERVER
    if (m_ConfigGeneralAgent.ServicePort > 0)
    {
        DBG(
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
                m_pHandlerGeneralAgent->Add(m_pGeneralAgentTcpSocketServer);
                // DBG(
                __fline;
                printf("m_pGeneralAgentTcpSocketServer->Bind :%d OK\n", m_ConfigGeneralAgent.ServicePort);
                // );
            }
        }
    }
#endif
#ifdef USE_GENERALAGENTHTTPSERVER
    if (m_ConfigGeneralAgent.HttpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pGeneralAgentHttpServer:%d\n", m_ConfigGeneralAgent.ServicePort);
        );

        m_pGeneralAgentHttpServer = new ListenSocket<CGeneralAgentHttpServer>(*m_pHandlerGeneralAgent);

        if (m_pGeneralAgentHttpServer)
        {
            if (m_pGeneralAgentHttpServer->Bind(m_ConfigGeneralAgent.HttpServicePort) < 0)
            {
                __trip;
                printf("m_pGeneralAgentHttpServer->Bind :%d error\n", m_ConfigGeneralAgent.HttpServicePort);
            }
            else
            {
                m_pHandlerGeneralAgent->Add(m_pGeneralAgentHttpServer);
                // DBG(
                __fline;
                printf("m_pGeneralAgentHttpServer->Bind :%d OK\n", m_ConfigGeneralAgent.HttpServicePort);
                // );
            }
        }
    }
#endif

#ifdef USE_GENERALAGENTUDPSOCKETSERVER
    if (m_ConfigGeneralAgent.UdpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pGeneralAgentUdpSocketServer:%d\n", m_ConfigGeneralAgent.UdpServicePort);
        );

        m_pGeneralAgentUdpSocketServer = new CGeneralAgentUdpSocket(*m_pHandlerGeneralAgent);

        if (m_pGeneralAgentUdpSocketServer)
        {
            if (m_pGeneralAgentUdpSocketServer->Bind(m_ConfigGeneralAgent.UdpServicePort, 10) < 0)
            {
                __trip;
                printf("m_pGeneralAgentUdpSocketServer->Bind :%d failed\n", m_ConfigGeneralAgent.UdpServicePort);
            }
            else
            {
                m_pHandlerGeneralAgent->Add(m_pGeneralAgentUdpSocketServer);
                // DBG(
                __fline;
                printf("m_pGeneralAgentUdpSocketServer->Bind :%d OK\n", m_ConfigGeneralAgent.UdpServicePort);
                // );
            }
        }
    }
#endif

    return 0;
}

int CGeneralAgent::ConnectorDestroy()
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
        DBG(
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
    // 已经启动
    if (m_bLoop)
    {
        return TRUE;
    }

    if (pConfig)
    {
        //__trip;
        m_ConfigGeneralAgent = *pConfig;
    }
    else
    {
#ifdef USE_EZCONFIG
        // 调试的时候使用默认参数
        //__fline;
        //printf("SetDefaultConfig\n");

        SetDefaultConfig();

#ifdef USE_GENERALCONSOLE_OVERTCP

        CConfigConsoleOverTcp __cfgConsoleOverTcp;
        __cfgConsoleOverTcp.update();
        m_ConfigGeneralAgent.ConsoleOverTcpPort = __cfgConsoleOverTcp.getConfig().SPort;
#endif // USE_GENERALCONSOLE_OVERTCP

#ifdef USE_GENERALAGENTTCPSOCKETSERVER

        CConfigTcpSocketServer __cfgTcpSocketServer;
        __cfgTcpSocketServer.update();
        SetGeneralAgent(__cfgTcpSocketServer.getConfig().SPort);
#endif

        CConfigTcpSocketConnector __cfgTcpSocketConnector;
        __cfgTcpSocketConnector.update();

#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR

        GENERALSERVERCFG_T __peer;
        __peer.Port = __cfgTcpSocketConnector.getConfig().iSMSPort;

        __peer.strHost = __cfgTcpSocketConnector.getConfig().strSMSHost;
        __peer.strServerName = __cfgTcpSocketConnector.getConfig().strSMSName;

        m_ConfigGeneralAgent.Peer.clear();
        m_ConfigGeneralAgent.Peer.push_back(__peer);
#endif // USE_GENERALAGENTTCPSOCKETCONNECTOR

#else
        // 调试的时候使用默认参数
        __fline;
        printf("SetDefaultConfig\n");

        SetDefaultConfig();
#endif //USE_EZCONFIG

    }

    ConnectorCreate();
    // 启动处理线程
#ifdef USE_GENERALSOCKETPROCESSOR

    g_GeneralSocketProcessor.Start();
#endif

    int ret = CreateThread();

    ARG_USED(ret);
    m_TimerMaintain.Start(this
                          , (TIMERPROC)&CGeneralAgent::TimerProcMaintain
                          , 0
                          , 1 * 1000 /*每秒执行一次，请勿修改*/
                          , 0
                          , 10*1000
                         );

    return TRUE;
}

BOOL CGeneralAgent::Stop()
{
    return TRUE;
}

void CGeneralAgent::TimerProcMaintain()
{

    // 尝试连接
#ifdef USE_GENERALAGENTTCPSOCKETCONNECTOR
    for (unsigned int ii=0; ii<m_ConfigGeneralAgent.Peer.size(); ii++)
    {
        // 将连接失败的连接再次尝试
        if (m_ConfigGeneralAgent.Peer[ii].IsOpened != 1)
        {
            LOG4CPLUS_INFO(LOG_PORTS, "Connector "
                           <<m_ConfigGeneralAgent.Peer[ii].strServerName.c_str()
                           << ":" << ii
                           << " Host --"
                           <<m_ConfigGeneralAgent.Peer[ii].strHost.c_str() << ":"
                           << m_ConfigGeneralAgent.Peer[ii].Port
                           << " ...");

            CGeneralAgentTcpSocketConnector *_pConnector = new CGeneralAgentTcpSocketConnector(*m_pHandlerGeneralAgent
                    , m_ConfigGeneralAgent.Peer[ii].strServerName
                    , m_ConfigGeneralAgent.Peer[ii].iConnTimeOut
                    , m_ConfigGeneralAgent.Peer[ii].bEnableRecon);

            if (_pConnector)
            {
                DBG(
                    __fline;
                    printf("Connector[%s]:%d Host--%s:%d Open "
                           , m_ConfigGeneralAgent.Peer[ii].strServerName.c_str()
                           , ii
                           , m_ConfigGeneralAgent.Peer[ii].strHost.c_str()
                           , m_ConfigGeneralAgent.Peer[ii].Port);
                );
                bool bRet = _pConnector->Open(m_ConfigGeneralAgent.Peer[ii].strHost.c_str(), m_ConfigGeneralAgent.Peer[ii].Port);
                if (bRet)
                {
                    //                            DEB_CODE(
                    DBG(
                        printf("Succeeded.\n");
                    );

                    m_pHandlerGeneralAgent->Add(_pConnector);

                    SetSubHostOpened(ii, 1);
                    LOG4CPLUS_INFO(LOG_PORTS, "Succeeded.");
                }
                else
                {
                    DBG(
                        printf("Failed.\n");
                        __trip;
                    );
                    SetSubHostOpened(ii, 0);

                    // 失败处理
                    delete _pConnector;

                    LOG4CPLUS_INFO(LOG_PORTS, "Failed.");
                }
            }
            else
            {
                __trip;
                //失败处理
                LOG4CPLUS_INFO(LOG_PORTS, "new CGeneralAgentTcpSocketConnector Failed.");
            }


        }
    }
#endif

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

    //for (int ii=0; ii<6; ii++)// dbg only
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

            Send2Socket(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME, http_req, strlen(http_req));
            __fline;
            printf("%d sended\n", strlen(http_req)+1);
        }
#endif //USE_GENERALAGENTHTTPKEEPCONNECTOR

    } //while (m_bLoop)
}

void CGeneralAgent::Initialize(int argc, char * argv[])
{
    //    ShowVersion();
    //    //SetPlatform();
    //
    //    g_TimerManager.Start();
    //    g_ThreadManager.RegisterMainThread(ThreadGetID());
    //
    //    //g_Config.useDoubleCfgFile(1);
    //    g_Config.initialize(ConfigFileSolar_1, ConfigFileSolar_2);
#ifdef U_BUILD_PORTS
    InitializeLogs();
#endif //U_BUILD_PORTS

    LOG4CPLUS_INFO(LOG_SOLAR, "Starting(" << "CGeneralAgent::Initialize" << ") ...");

    //    g_Config.saveFile();
}

#ifdef U_BUILD_PORTS
int CGeneralAgent::InitializeLogs()
{
    //	初始化日志系统
    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    log4cplus::tstring pattern = LOG4CPLUS_TEXT("%D %-5p %-5c - %m [%l]%n"); // c , model
    SharedAppenderPtr append_1;
    if (__cfgGeneral.getConfig().strLogFileName.size()<=0)
    {
        append_1=new ConsoleAppender();
        __trip;
    }
    else
    {
        std::string strFilePathName = __cfgGeneral.getConfig().strLogFilePath +"/"+  __cfgGeneral.getConfig().strLogFileName;
        printf("Initialize Logs:%s, MaxSize:%d Files:%d - "
               , strFilePathName.c_str()
               , __cfgGeneral.getConfig().maxFileSize
               , __cfgGeneral.getConfig().maxBackupIndex);
        int iret = ez_creat_dir(__cfgGeneral.getConfig().strLogFilePath.c_str());
        if (iret<0)
            printf("failed\n");
        else
            printf("success\n");

        append_1 = new RollingFileAppender(LOG4CPLUS_TEXT(strFilePathName)
                                           , __cfgGeneral.getConfig().maxFileSize
                                           , __cfgGeneral.getConfig().maxBackupIndex);
    }
    append_1->setName(LOG4CPLUS_TEXT("TsHome"));
    append_1->setLayout( std::auto_ptr<Layout>(new PatternLayout(pattern)) );
    Logger::getRoot().addAppender(append_1);
    NDCContextCreator _context(LOG4CPLUS_TEXT("tss"));

    return 0;
}
#endif //U_BUILD_PORTS
