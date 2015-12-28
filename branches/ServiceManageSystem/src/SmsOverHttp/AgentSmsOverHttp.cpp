/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentSmsOverHttp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentSmsOverHttp.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

//#include "CommonInclude.h"
#include "TypeDefSmsOverHttp.h"

#include "AgentSmsOverHttp.h"
#include "../Logs.h"
#include "../Configs/ConfigSmsOverHttp.h"
#ifdef _DEBUG
	#define DEB(x)
	#define DBG(x)
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


PATTERN_SINGLETON_IMPLEMENT(CAgentSmsOverHttp);
void CAgentSmsOverHttp::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerSmsOverHttp->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerSmsOverHttp->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    m_pHandlerSmsOverHttp->Select(1,0);
    m_pHandlerSmsOverHttp->Update();
}

CAgentSmsOverHttp::CAgentSmsOverHttp():CEZThread("CAgentSmsOverHttp", TP_DEFAULT, 10)
{
    m_ConfigOfSmsOverHttp.HttpServicePort = 60089; // 如果不启用，则设置为0

    m_pHandlerSmsOverHttp = new CHandlerSmsOverHttp(&m_Log);
    assert(m_pHandlerSmsOverHttp);

    ///////////////////////////////////////////////
}
CAgentSmsOverHttp::~CAgentSmsOverHttp()
{
    m_ConfigOfSmsOverHttp.HttpServicePort = 0;
    // handler里面自管理
    m_pCHttpServerSmsOverHttp = NULL;
    
    if (m_pHandlerSmsOverHttp)
    {
        delete m_pHandlerSmsOverHttp;
    }
}

CHandlerSmsOverHttp * CAgentSmsOverHttp::GetSmsOverHttpHandler()
{
	return m_pHandlerSmsOverHttp;
}

void CAgentSmsOverHttp::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfSmsOverHttp.HttpServicePort = port;
}

int CAgentSmsOverHttp::Send2Socket(std::string strServerName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerSmsOverHttp->GetSocketByName(strServerName);

    if (p)
    {
        p->SendBuf(pData, Len);
    }
    else
    {
        __trip;
        printf("Send2Socket %s, Len:%d failed\n", strServerName.c_str(), (int)Len);
    }

    return 0;
}


void CAgentSmsOverHttp::ConnectAll()
{
    if (m_ConfigOfSmsOverHttp.HttpServicePort > 0)
    {
        DBG(
        __fline;
        printf("new m_pCHttpServerSmsOverHttp: %d\n", m_ConfigOfSmsOverHttp.HttpServicePort);
         );

        m_pCHttpServerSmsOverHttp = new ListenSocket<CHttpServerSmsOverHttp>(*m_pHandlerSmsOverHttp);

        if (m_pCHttpServerSmsOverHttp)
        {
            if (m_pCHttpServerSmsOverHttp->Bind(m_ConfigOfSmsOverHttp.HttpServicePort) < 0)
            {
                __trip;
                printf("SmsOverHttp->Bind :%d error\n", m_ConfigOfSmsOverHttp.HttpServicePort);
            }
            else
            {
                m_pHandlerSmsOverHttp->Add(m_pCHttpServerSmsOverHttp);
                // DBG_CODE(
                //__fline;
                printf("SmsOverHttp     Ready:%d\n", m_ConfigOfSmsOverHttp.HttpServicePort);
                // );
                LOG4CPLUS_INFO(LOG_WEBSERVICES, "SmsOverHttp     Ready:"<<m_ConfigOfSmsOverHttp.HttpServicePort);
            }
        }
    }
}

void CAgentSmsOverHttp::DisconnectAll()
{

}

BOOL CAgentSmsOverHttp::Start()
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

BOOL CAgentSmsOverHttp::Stop()
{
    return TRUE;
}

void CAgentSmsOverHttp::SetDefaultConfig()
{
#ifdef USE_EZCONFIG
    CConfigSmsOverHttp __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iSmsOverHttpPort));
#else
    SetGeneralHttpAgent((unsigned short)(SMARTHOMETELCOMCTRLSERVER_PORT));

#endif
}

void CAgentSmsOverHttp::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
