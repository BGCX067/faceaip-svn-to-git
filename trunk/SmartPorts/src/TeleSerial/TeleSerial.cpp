/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerial.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerial.cpp 5884 2013-07-27 11:45:11Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-27 11:45:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <iostream>

#include "TeleSerial.h"

#ifdef TeleSerial_FUNC_SOCKETPROCESSOR
#include "GeneralSocketProcessor.h"
#endif

#ifdef EXAMPLE_EZCONFIG
#include "Configs/ConfigManager.h"
#endif //EXAMPLE_EZCONFIG

#include <ez_fs.h>
#include "../Logs.h"

#include "DevSerial.h"

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

PATTERN_SINGLETON_IMPLEMENT(CTeleSerial);
void CTeleSerial::Run()
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

CTeleSerial::CTeleSerial():CEZThread("GeneralAgent", TP_DEFAULT, 10)
#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
        , m_sigData(1)
#endif //TeleSerial_FUNC_SIGNAL_ONDATA
{
    m_pHandlerGeneralAgent = new CTeleSerialHandler(&m_Log);
    assert(m_pHandlerGeneralAgent);

    /////////////////
    //初始化配置信息
    // 本机服务端口
#ifdef TeleSerial_FUNC_TCPSOCKETSERVER

    m_ConfigGeneralAgent.PortTcpServer = 0; // 如果不启用，则设置为0
#endif
#ifdef TeleSerial_FUNC_HTTPSERVER

    m_ConfigGeneralAgent.PortHttpServer = 0; // 如果不启用，则设置为0
#endif
#ifdef TeleSerial_FUNC_UDPSOCKETSERVER

    m_ConfigGeneralAgent.UdpServerPort = 0; // 如果不启用，则设置为0
    m_pGeneralAgentUdpSocketServer = NULL;
#endif
    // 本机状态服务端口
#ifdef TeleSerial_FUNC_STATUS_AGENT

    m_ConfigGeneralAgent.StatusServicePort = 0; // 如果不启用，则设置为0
#endif
    //调试命令行接口， 0无效
#ifdef TeleSerial_FUNC_CONSOLE_OVERTCP

    m_ConfigGeneralAgent.ConsoleOverTcpPort = 0; // 调试命令行接口， 0无效
#endif
#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR
    // tcp 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    //m_ConfigGeneralAgent.Peer.clear();
#endif

#ifdef TeleSerial_FUNC_HTTPKEEPCONNECTOR
    // http 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    //std::vector<GENERALSERVERCFG_T> PeerHttp;
#endif
    //memset(&m_ConfigGeneralAgent, 0, sizeof(GENERALAGENTCFG_T));
    /////////////////////
#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR

    //m_Connector.clear();
#endif

#ifdef TeleSerial_FUNC_TCPSOCKETSERVER

    m_pGeneralAgentTcpSocketServer = NULL;
#endif
#ifdef TeleSerial_FUNC_HTTPSERVER

    m_pGeneralAgentHttpServer = NULL;
#endif


    ////////////////////////

    ///////////////////////////////////////////////
#ifdef ATTATCH_FUNC_OF_GENERAL_AGENT

    m_iRunPeriod = 0;
    m_ttStartTime = time(NULL);
#endif // ATTATCH_FUNC_OF_GENERAL_AGENT
}
CTeleSerial::~CTeleSerial()
{
    if (m_pHandlerGeneralAgent)
    {
        delete m_pHandlerGeneralAgent;
    }
}


#ifdef TeleSerial_FUNC_TCPSOCKETSERVER
void CTeleSerial::SetTcpServer(unsigned short  port)
{
    m_ConfigGeneralAgent.PortTcpServer = port;
}
#endif //TeleSerial_FUNC_TCPSOCKETSERVER

#ifdef TeleSerial_FUNC_HTTPSERVER
void CTeleSerial::SetHttpServer(unsigned short  port)
{
    m_ConfigGeneralAgent.PortHttpServer = port;
}
#endif

#ifdef TeleSerial_FUNC_UDPSOCKETSERVER
void CTeleSerial::SetUdpServer(unsigned short  port)
{
    m_ConfigGeneralAgent.UdpServerPort = port;
}
#endif//TeleSerial_FUNC_UDPSOCKETSERVER

#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR
void CTeleSerial::AddTcpHost(std::string strHost, int nPort, std::string strServerName)
{
    // todo 检查不要重复加

    GENERALSERVERCFG_T __peer;
    __peer.Port = nPort;

    __peer.strHost = strHost;
    __peer.strServerName = strServerName;

    m_ConfigGeneralAgent.TcpHost.push_back(__peer);
}
#endif // TeleSerial_FUNC_TCPSOCKETCONNECTOR

#ifdef TeleSerial_FUNC_HTTPKEEPCONNECTOR
void CTeleSerial::AddHttpHost(std::string strHost, int nPort, std::string strServerName)
{
    // todo 检查不要重复加

    GENERALSERVERCFG_T __peer;
    __peer.Port = nPort;
    __peer.strHost = strHost;
    __peer.strServerName = strServerName;

    m_ConfigGeneralAgent.HttpHost.push_back(__peer);
}

#endif // TeleSerial_FUNC_HTTPKEEPCONNECTOR

int CTeleSerial::Send2Socket(std::string strServerName, const char *pData, size_t Len)
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

Socket * CTeleSerial::GetSocketByName(std::string strServerName)
{
    if (m_pHandlerGeneralAgent==NULL)
    {
        __trip;
        return NULL;
    }

    return m_pHandlerGeneralAgent->GetSocketByName(strServerName);
}

int CTeleSerial::ConnectorCreate()
{
#ifdef TeleSerial_FUNC_HTTPKEEPCONNECTOR
    for (unsigned int ii=0; ii<m_ConfigGeneralAgent.HttpHost.size(); ii++)
    {
        CTeleSerialHttpKeepConnector *_pConnector
        = new CTeleSerialHttpKeepConnector(*m_pHandlerGeneralAgent, m_ConfigGeneralAgent.HttpHost[ii].strServerName);
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigGeneralAgent.HttpHost[ii].strHost.c_str(), m_ConfigGeneralAgent.HttpHost[ii].Port);
            if (bRet)
            {
                printf("Connector:%d %s:%d Open\n"
                       , ii
                       , m_ConfigGeneralAgent.HttpHost[ii].strHost.c_str()
                       , m_ConfigGeneralAgent.HttpHost[ii].Port);

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
#endif //TeleSerial_FUNC_HTTPKEEPCONNECTOR

#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR
    for (unsigned int ii=0; ii<m_ConfigGeneralAgent.TcpHost.size(); ii++)
    {
        CTeleSerialTcpSocketConnector *_pConnector = new CGeneralAgentTcpSocketConnector(*m_pHandlerGeneralAgent
                , m_ConfigGeneralAgent.TcpHost[ii].strServerName
                , m_ConfigGeneralAgent.TcpHost[ii].iConnTimeOut
                , m_ConfigGeneralAgent.TcpHost[ii].bEnableRecon);
        if (_pConnector)
        {
            //                            DEB_CODE(
            printf("Connector[%s]:%d Host--%s:%d Open "
                   , m_ConfigGeneralAgent.TcpHost[ii].strServerName.c_str()
                   , ii
                   , m_ConfigGeneralAgent.TcpHost[ii].strHost.c_str()
                   , m_ConfigGeneralAgent.TcpHost[ii].Port);
            //			);
            bool bRet = _pConnector->Open(m_ConfigGeneralAgent.TcpHost[ii].strHost.c_str(), m_ConfigGeneralAgent.TcpHost[ii].Port);
            if (bRet)
            {
                //                            DEB_CODE(
                printf("Succeeded.\n");
                //			);

                m_pHandlerGeneralAgent->Add(_pConnector);
            }
            else
            {
                //                 DEB_CODE(
                printf("Failed.\n");
                //		);
                __trip;
                // 失败处理
                delete _pConnector;
            }
        }
        else
        {
            __trip;
            //失败处理
        }
    }
#endif

#ifdef TeleSerial_FUNC_STATUS_AGENT
    if(m_ConfigGeneralAgent.StatusServicePort>0)
    {
        m_pSocketStatusAgent = new ListenSocket<CTeleSerialStatusAgent>(*m_pHandlerGeneralAgent);

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

#ifdef TeleSerial_FUNC_CONSOLE_OVERTCP
    if(m_ConfigGeneralAgent.ConsoleOverTcpPort>0)
    {
        m_pGeneralConsoleOverTcp = new ListenSocket<CTeleSerialConsoleOverTcp>(*m_pHandlerGeneralAgent);

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
                //DBG_CODE(
                __fline;
                printf("m_pGeneralConsoleOverTcp->Bind :%d OK\n", m_ConfigGeneralAgent.ConsoleOverTcpPort);
                //);
            }
        }
    }// if(m_pGeneralConsoleOverTcp == NULL)
#endif //TeleSerial_FUNC_CONSOLE_OVERTCP

#ifdef TeleSerial_FUNC_TCPSOCKETSERVER
    DBG(
        __fline;
        printf("m_ConfigGeneralAgent.PortTcpServer:%d\n", m_ConfigGeneralAgent.PortTcpServer);
    );
    if (m_ConfigGeneralAgent.PortTcpServer > 0)
    {

        m_pGeneralAgentTcpSocketServer = new ListenSocket<CTeleSerialTcpSocketServer>(*m_pHandlerGeneralAgent);

        if (m_pGeneralAgentTcpSocketServer)
        {
            if (m_pGeneralAgentTcpSocketServer->Bind(m_ConfigGeneralAgent.PortTcpServer) < 0)
            {
                __trip;
                printf("m_pGeneralAgentTcpSocketServer->Bind :%d error\n", m_ConfigGeneralAgent.PortTcpServer);
            }
            else
            {
                m_pHandlerGeneralAgent->Add(m_pGeneralAgentTcpSocketServer);
                // DBG_CODE(
                __fline;
                printf("m_pGeneralAgentTcpSocketServer->Bind :%d OK\n", m_ConfigGeneralAgent.PortTcpServer);
                // );
            }
        }
    }

#endif
#ifdef TeleSerial_FUNC_HTTPSERVER
    if (m_ConfigGeneralAgent.PortHttpServer > 0)
    {
        DBG(
            __fline;
            printf("new m_pGeneralAgentHttpServer:%d\n", m_ConfigGeneralAgent.PortTcpServer);
        );

        m_pGeneralAgentHttpServer = new ListenSocket<CTeleSerialHttpServer>(*m_pHandlerGeneralAgent);

        if (m_pGeneralAgentHttpServer)
        {
            if (m_pGeneralAgentHttpServer->Bind(m_ConfigGeneralAgent.PortHttpServer) < 0)
            {
                __trip;
                printf("m_pGeneralAgentHttpServer->Bind :%d error\n", m_ConfigGeneralAgent.PortHttpServer);
            }
            else
            {
                m_pHandlerGeneralAgent->Add(m_pGeneralAgentHttpServer);
                // DBG_CODE(
                __fline;
                printf("m_pGeneralAgentHttpServer->Bind :%d OK\n", m_ConfigGeneralAgent.PortHttpServer);
                // );
            }
        }
    }
#endif

#ifdef TeleSerial_FUNC_UDPSOCKETSERVER
    if (m_ConfigGeneralAgent.UdpServerPort > 0)
    {
        DBG(
            __fline;
            printf("new m_pGeneralAgentUdpSocketServer:%d\n", m_ConfigGeneralAgent.UdpServerPort);
        );

        m_pGeneralAgentUdpSocketServer = new CTeleSerialUdpSocket(*m_pHandlerGeneralAgent);

        if (m_pGeneralAgentUdpSocketServer)
        {
            if (m_pGeneralAgentUdpSocketServer->Bind(m_ConfigGeneralAgent.UdpServerPort, 10) < 0)
            {
                __trip;
                printf("m_pGeneralAgentUdpSocketServer->Bind :%d failed\n", m_ConfigGeneralAgent.UdpServerPort);
            }
            else
            {
                m_pHandlerGeneralAgent->Add(m_pGeneralAgentUdpSocketServer);
                // DBG_CODE(
                __fline;
                printf("m_pGeneralAgentUdpSocketServer->Bind :%d OK\n", m_ConfigGeneralAgent.UdpServerPort);
                // );
            }
        }
    }
#endif

    return 0;
}

int CTeleSerial::ConnectorDestroy()
{
    assert(m_pHandlerGeneralAgent);

    return m_pHandlerGeneralAgent->CloseAndDeleteAll();

#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR
    // 关闭客户端
    for (size_t ii=0; ii<m_ConfigGeneralAgent.TcpHost.size(); ii++)
    {
        printf("Disconnect %d -- %s\n", ii, m_ConfigGeneralAgent.TcpHost[ii].strHost.c_str());
        // todo
    }
#endif
#ifdef TeleSerial_FUNC_HTTPKEEPCONNECTOR
    // 关http客户端
    for (size_t ii=0; ii<m_ConfigGeneralAgent.HttpHost.size(); ii++)
    {
        printf("Disconnect %d -- %s\n", ii, m_ConfigGeneralAgent.HttpHost[ii].strHost.c_str());
        // todo
    }
#endif
#ifdef TeleSerial_FUNC_TCPSOCKETSERVER
    // 关掉服务器
    if (m_ConfigGeneralAgent.PortTcpServer > 0
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

#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
BOOL CTeleSerial::Start(CEZObject * pObj, SIG_TeleSerialHandler_DATA::SigProc pProc)
#else
BOOL CTeleSerial::Start()
#endif //TeleSerial_FUNC_SIGNAL_ONDATA
{
#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
    if (pObj && pProc)
    {
        CEZLock _lock(m_MutexSig);

        if(m_sigData.Attach(pObj, pProc) < 0)
        {
            trace("attach error\n");
            return FALSE;
        }
    }
#endif //TeleSerial_FUNC_SIGNAL_ONDATA

    // 已经启动
    if (m_bLoop)
    {
        return TRUE;
    }
    CDevSerial::instance()->Start(this, (SIG_DevSerial_DATA::SigProc)&CTeleSerial::OnSerial);

    ConnectorCreate();
    // 启动处理线程
#ifdef TeleSerial_FUNC_SOCKETPROCESSOR

    g_GeneralSocketProcessor.Start();
#endif

    int ret = CreateThread();

    ARG_USED(ret);
#ifdef ATTATCH_FUNC_OF_GENERAL_AGENT

    m_TimerMaintain.Start(this
                          , (TIMERPROC)&CTeleSerial::TimerProcMaintain
                          , 0
                          , 1 * 1000 /*每秒执行一次，请勿修改*/
                          , 0
                          , 10*1000
                         );
#endif //ATTATCH_FUNC_OF_GENERAL_AGENT

    return TRUE;
}

#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
BOOL CTeleSerial::Stop(CEZObject * pObj, SIG_TeleSerialHandler_DATA::SigProc pProc)
#else
BOOL CTeleSerial::Stop()
#endif //TeleSerial_FUNC_SIGNAL_ONDATA
{
#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
    if (pObj && pProc)
    {
        CEZLock _lock(m_MutexSig);

        if(m_sigData.Detach(pObj, pProc) < 0)
        {
            trace("detach error\n");
            return FALSE;
        }
    }
#endif //TeleSerial_FUNC_SIGNAL_ONDATA

    CDevSerial::instance()->Stop(this, (SIG_DevSerial_DATA::SigProc)&CTeleSerial::OnSerial);

    m_bLoop = FALSE;

    return TRUE;
}

#ifdef ATTATCH_FUNC_OF_GENERAL_AGENT
void CTeleSerial::TimerProcMaintain()
{
    // 为了计数准确，不依赖系统时间，自己维护
    m_iRunPeriod ++;
}

unsigned int CTeleSerial::GetRunPeriod()
{
    return m_iRunPeriod;
}

time_t CTeleSerial::GetStartTime()
{
    return m_ttStartTime;
}
#endif //#ifdef ATTATCH_FUNC_OF_GENERAL_AGENT


void CTeleSerial::SetConfig(GENERALAGENTCFG_T *pConfig)
{
    if (pConfig)
    {
        //__trip;
        m_ConfigGeneralAgent = *pConfig;

        return;
    }

#ifdef USE_EZCONFIG
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifdef TeleSerial_FUNC_STATUS_AGENT

    m_pSocketStatusAgent = NULL;
    m_ConfigGeneralAgent.StatusServicePort = TeleSerial_FUNC_THE_STATUS_AGENT_PORT;
#endif

#ifdef TeleSerial_FUNC_CONSOLE_OVERTCP

    CConfigConsoleOverTcp __cfgConsoleOverTcp;
    __cfgConsoleOverTcp.update();
    m_ConfigGeneralAgent.ConsoleOverTcpPort = __cfgConsoleOverTcp.getConfig().SPort;
#endif // TeleSerial_FUNC_CONSOLE_OVERTCP

#ifdef TeleSerial_FUNC_TCPSOCKETSERVER

    CConfigTcpSocketServer __cfgTcpSocketServer;
    __cfgTcpSocketServer.update();
    SetTcpServer(__cfgTcpSocketServer.getConfig().SPort);
#endif

    CConfigTcpSocketConnector __cfgTcpSocketConnector;
    __cfgTcpSocketConnector.update();

#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR

    GENERALSERVERCFG_T __peer;
    __peer.Port = __cfgTcpSocketConnector.getConfig().iSMSPort;

    __peer.strHost = __cfgTcpSocketConnector.getConfig().strSMSHost;
    __peer.strServerName = __cfgTcpSocketConnector.getConfig().strSMSName;

    m_ConfigGeneralAgent.TcpHost.clear();
    m_ConfigGeneralAgent.TcpHost.push_back(__peer);
#endif // TeleSerial_FUNC_TCPSOCKETCONNECTOR

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#else //USE_EZCONFIG
    // 调试的时候使用默认参数
    __fline;
    printf("SetConfig\n");

#ifdef TeleSerial_FUNC_STATUS_AGENT

    m_pSocketStatusAgent = NULL;
    m_ConfigGeneralAgent.StatusServicePort = TeleSerial_FUNC_THE_STATUS_AGENT_PORT;
#endif

    // 调试终端
#ifdef TeleSerial_FUNC_CONSOLE_OVERTCP

    m_pGeneralConsoleOverTcp = NULL;
    m_ConfigGeneralAgent.ConsoleOverTcpPort = TeleSerial_FUNC_PORT_GENERAL_CONSOLE_OVER_TCP;
#endif

#ifdef TeleSerial_FUNC_TCPSOCKETSERVER

    SetTcpServer(TeleSerial_FUNC_THE_TCPSVR_AGENT_PORT);
#endif
#ifdef TeleSerial_FUNC_HTTPSERVER

    SetHttpServer(TeleSerial_FUNC_THE_HTTP_SVR_AGENT_PORT);
#endif

#ifdef TeleSerial_FUNC_UDPSOCKETSERVER

    SetUdpServer(TeleSerial_FUNC_THE_UDPSVR_AGENT_PORT);
#endif
#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR

    //for (int ii=0; ii<6; ii++)// dbg only
    {
        AddTcpHost(TeleSerial_FUNC_DEFAULT_NEXT_HOST, (unsigned short)(TeleSerial_FUNC_DEFAULT_NEXT_HOST_PORT));
    }
#endif
#ifdef TeleSerial_FUNC_HTTPKEEPCONNECTOR
    for (int ii=0; ii<1; ii++)
    {
        AddHttpHost(TeleSerial_FUNC_DEFAULT_NEXT_HTTP_HOST, (unsigned short)(TeleSerial_FUNC_DEFAULT_NEXT_HTTP_PORT));
    }
#endif //TeleSerial_FUNC_HTTPKEEPCONNECTOR
#endif //USE_EZCONFIG
}

void CTeleSerial::ThreadProc()
{
DBG(
    std::cout << ">>>>>>>>>>>>>> CTeleSerial::ThreadProc" << std::endl;
);
#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
    m_pHandlerGeneralAgent->AddSignal(this, (SIG_TeleSerialHandler_DATA::SigProc)&CTeleSerial::OnData);
#endif //TeleSerial_FUNC_SIGNAL_ONDATA

    while (m_bLoop)
    {
        Run();

        /////////////////
#ifdef TeleSerial_FUNC_HTTPKEEPCONNECTOR

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
#endif //TeleSerial_FUNC_HTTPKEEPCONNECTOR

    } //while (m_bLoop)

#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
    m_pHandlerGeneralAgent->DelSignal(this, (SIG_TeleSerialHandler_DATA::SigProc)&CTeleSerial::OnData);
#endif //TeleSerial_FUNC_SIGNAL_ONDATA

    std::cout << "CTeleSerial::ThreadProc >>>>>>>>>>>" << std::endl;
}

void CTeleSerial::Initialize(int argc, char * argv[])
{
    //    ShowVersion();
    //    //SetPlatform();
    //
    //    g_TimerManager.Start();
    //    g_ThreadManager.RegisterMainThread(ThreadGetID());
    //
    //    //g_Config.useDoubleCfgFile(1);
    //    g_Config.initialize(ConfigFileSolar_1, ConfigFileSolar_2);
    InitializeLogs();

    SetConfig();

    LOG4CPLUS_INFO(LOG_SOLAR, "Starting(" << "CTeleSerial::Initialize" << ") ...");

    //    g_Config.saveFile();
}

int CTeleSerial::InitializeLogs()
{
#if defined(TeleSerial_FUNC_USE_LOG4CPP)
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

    //pj_log_set_level(10);
    //pj_log_set_log_func( __pj_log_func );
#endif //TeleSerial_FUNC_USE_LOG4CPP

    return 0;
}

#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
BOOL CTeleSerial::OnData(Socket *pSocket, const char *pdat, size_t len)
{
    if((pdat == NULL) || (len <= 0))
    {
        return false;
    }

    // your code
    DBG(
        __fline;
        printf("TeleSerial_FUNC_SIGNAL_ONDATA OnData:%d -- %c\n", len, pdat[0]);
    );
    return TRUE;
}
#endif //TeleSerial_FUNC_SIGNAL_ONDATA


BOOL CTeleSerial::OnSerial(int id, const char *pdat, size_t len)
{
    if((pdat == NULL) || (len <= 0))
    {
        return false;
    }

    // your code
    DBG(
        __fline;
        printf("CTeleSerial::OnSerial:%d -- %c\n", len, pdat[0]);
    );

    m_pHandlerGeneralAgent->OnSerial(id, pdat, len);

    return TRUE;
}
