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

#include "../Configs/ConfigSmartHomeTelcom.h"

#include "AgentSmartHomeTelcom.h"

PATTERN_SINGLETON_IMPLEMENT(CAgentSmartHomeTelcom);
void CAgentSmartHomeTelcom::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerSmartHomeTelcom->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerSmartHomeTelcom->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    m_pHandlerSmartHomeTelcom->Select(1,0);
    m_pHandlerSmartHomeTelcom->Update();
}

Socket * CAgentSmartHomeTelcom::GetSocketByName(std::string strServerName)
{
	if (m_pHandlerSmartHomeTelcom)
	{
	    return m_pHandlerSmartHomeTelcom->GetSocketByName(strServerName);
	}
	else
	{
		return NULL; 
	}
}

CAgentSmartHomeTelcom::CAgentSmartHomeTelcom():CEZThread("CAgentSmartHomeTelcom", TP_DEFAULT, 10)
{
    m_pHandlerSmartHomeTelcom = new CHandlerSmartHomeTelcom(&m_Log);
    assert(m_pHandlerSmartHomeTelcom);

    ///////////////////////////////////////////////
}
CAgentSmartHomeTelcom::~CAgentSmartHomeTelcom()
{
    if (m_pHandlerSmartHomeTelcom)
    {
        delete m_pHandlerSmartHomeTelcom;
    }
}


void CAgentSmartHomeTelcom::SetGeneralAgent(unsigned short  port)
{
    m_ConfigGeneralAgent.ServicePort = port;
}



#ifdef USE_HTTPKEEPCONNECTORSMARTHOME
void CAgentSmartHomeTelcom::AddSubHttpHost(char* szIP, int nPort, std::string strServerName)
{
    // todo 检查不要重复加

    GENERALSERVERCFG_T __peer;
    __peer.Port = nPort;
    strcpy(__peer.strIP, szIP);
    __peer.strServerName = strServerName;

    m_ConfigGeneralAgent.PeerHttp.push_back(__peer);
}

#endif // USE_HTTPKEEPCONNECTORSMARTHOME

int CAgentSmartHomeTelcom::Send2Socket(std::string strServerName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerSmartHomeTelcom->GetSocketByName(strServerName);

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

CHttpKeepConnectorSmartHomeTelcom* CAgentSmartHomeTelcom::GetHttpKeepConnector(std::string strSocketName)
{
    Socket *p = m_pHandlerSmartHomeTelcom->GetSocketByName(strSocketName);
	if (p){
        CHttpKeepConnectorSmartHomeTelcom *pHttpKeepConnector = dynamic_cast<CHttpKeepConnectorSmartHomeTelcom *>(p);
        if (pHttpKeepConnector)
        {
           return pHttpKeepConnector;
        }
		}
		return NULL;

}

void CAgentSmartHomeTelcom::ConnectAll()
{
#ifdef USE_HTTPKEEPCONNECTORSMARTHOME
    for (unsigned int ii=0; ii<m_ConfigGeneralAgent.PeerHttp.size(); ii++)
    {
        CHttpKeepConnectorSmartHomeTelcom *_pConnector
        = new CHttpKeepConnectorSmartHomeTelcom(*m_pHandlerSmartHomeTelcom, m_ConfigGeneralAgent.PeerHttp[ii].strServerName);
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigGeneralAgent.PeerHttp[ii].strIP, m_ConfigGeneralAgent.PeerHttp[ii].Port);
            if (bRet)
            {
                printf("HttpKeepConnectorSmartHomeTelcom(%d) To -> %s:%d\n"
                       , ii
                       , m_ConfigGeneralAgent.PeerHttp[ii].strIP
                       , m_ConfigGeneralAgent.PeerHttp[ii].Port);

                m_pHandlerSmartHomeTelcom->Add(_pConnector);
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
#endif //USE_HTTPKEEPCONNECTORSMARTHOME


}

void CAgentSmartHomeTelcom::DisconnectAll()
{

#ifdef USE_HTTPKEEPCONNECTORSMARTHOME
    // 关http客户端
#endif

}

BOOL CAgentSmartHomeTelcom::Start()
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

BOOL CAgentSmartHomeTelcom::Stop()
{
    return TRUE;
}

void CAgentSmartHomeTelcom::SetDefaultConfig()
{
#ifdef USE_HTTPKEEPCONNECTORSMARTHOME
    CConfigSmartHomeTelcom __cfg;
    __cfg.update();

    for (int ii=0; ii<1; ii++)
    {
        g_SmartHomeAgentTelcom.AddSubHttpHost((char *)__cfg.getConfig().strHost.c_str(), (unsigned short)(__cfg.getConfig().iPort));
    }
#endif //USE_HTTPKEEPCONNECTORSMARTHOME
}

void CAgentSmartHomeTelcom::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
