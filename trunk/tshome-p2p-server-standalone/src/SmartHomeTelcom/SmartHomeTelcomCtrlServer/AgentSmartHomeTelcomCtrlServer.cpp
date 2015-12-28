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
#include "TypeDefSmartHomeTelcomCtrlServer.h"

#include "AgentSmartHomeTelcomCtrlServer.h"
#include "../../Logs.h"
#include "../../Configs/ConfigSmartHomeTelcom.h"

PATTERN_SINGLETON_IMPLEMENT(CAgentSmartHomeTelcomCtrlServer);
void CAgentSmartHomeTelcomCtrlServer::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerSmartHomeTelcomCtrlServer->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerSmartHomeTelcomCtrlServer->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    m_pHandlerSmartHomeTelcomCtrlServer->Select(1,0);
    m_pHandlerSmartHomeTelcomCtrlServer->Update();
}

CAgentSmartHomeTelcomCtrlServer::CAgentSmartHomeTelcomCtrlServer():CEZThread("CAgentSmartHomeTelcomCtrlServer", TP_DEFAULT, 10)
{
#ifdef USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
    m_ConfigGeneralAgent.HttpServicePort = 0; // 如果不启用，则设置为0
#endif

    m_pHandlerSmartHomeTelcomCtrlServer = new CHandlerSmartHomeTelcomCtrlServer(&m_Log);
    assert(m_pHandlerSmartHomeTelcomCtrlServer);

    ///////////////////////////////////////////////
}
CAgentSmartHomeTelcomCtrlServer::~CAgentSmartHomeTelcomCtrlServer()
{
    m_ConfigGeneralAgent.HttpServicePort = 0;
    // handler里面自管理
    m_pCHttpServerSmartHomeTelcomCtrlServer = NULL;
    
    if (m_pHandlerSmartHomeTelcomCtrlServer)
    {
        delete m_pHandlerSmartHomeTelcomCtrlServer;
    }
}


void CAgentSmartHomeTelcomCtrlServer::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigGeneralAgent.HttpServicePort = port;
}


int CAgentSmartHomeTelcomCtrlServer::Send2Socket(std::string strServerName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerSmartHomeTelcomCtrlServer->GetSocketByName(strServerName);

    if (p)
    {
        p->SendBuf(pData, Len);
    }
    else
    {
        __trip;
        printf("Send2Socket %s, Len:%d failed\n", strServerName.c_str(), Len);
    }

    return 0;
}


void CAgentSmartHomeTelcomCtrlServer::ConnectAll()
{
#ifdef USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
    if (m_ConfigGeneralAgent.HttpServicePort > 0)
    {
        DBG_CODE(
        __fline;
        printf("new m_pCHttpServerSmartHomeTelcomCtrlServer:%d\n", m_ConfigGeneralAgent.HttpServicePort);
         );

        m_pCHttpServerSmartHomeTelcomCtrlServer = new ListenSocket<CHttpServerSmartHomeTelcomCtrlServer>(*m_pHandlerSmartHomeTelcomCtrlServer);

        if (m_pCHttpServerSmartHomeTelcomCtrlServer)
        {
            if (m_pCHttpServerSmartHomeTelcomCtrlServer->Bind(m_ConfigGeneralAgent.HttpServicePort) < 0)
            {
                __trip;
                printf("SmartHomeTelcomCtrlServer->Bind :%d error\n", m_ConfigGeneralAgent.HttpServicePort);
            }
            else
            {
                m_pHandlerSmartHomeTelcomCtrlServer->Add(m_pCHttpServerSmartHomeTelcomCtrlServer);
                // DBG_CODE(
                //__fline;
                printf("SmartHomeTelcomCtrlServer On:%d\n", m_ConfigGeneralAgent.HttpServicePort);
                // );
            }
        }
    }
#endif //USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
}

void CAgentSmartHomeTelcomCtrlServer::DisconnectAll()
{

#ifdef USE_HTTPKEEPCONNECTORSMARTHOME
    // 关http客户端
#endif

}

BOOL CAgentSmartHomeTelcomCtrlServer::Start()
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

BOOL CAgentSmartHomeTelcomCtrlServer::Stop()
{
    return TRUE;
}

void CAgentSmartHomeTelcomCtrlServer::SetDefaultConfig()
{
#ifdef USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
    CConfigSmartHomeTelcom __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iRegisterServerPort));
#endif //USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
}

void CAgentSmartHomeTelcomCtrlServer::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
