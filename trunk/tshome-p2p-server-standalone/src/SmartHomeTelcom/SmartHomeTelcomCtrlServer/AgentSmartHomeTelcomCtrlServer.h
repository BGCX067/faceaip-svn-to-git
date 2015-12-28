/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentSmartHomeTelcomCtrlServerCtrlServer.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: AgentSmartHomeTelcomCtrlServerCtrlServer.h 5884 2012-09-06 03:33:55Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-06 03:33:55  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _AgentSmartHomeTelcomCtrlServerCtrlServer_H
#define _AgentSmartHomeTelcomCtrlServerCtrlServer_H

#include "CommonInclude.h"
//#include "GeneralAgentDef.h" /* port etc */
#include "EZThread.h"
#include "IniFile.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

// 反应堆
#include "HandlerSmartHomeTelcomCtrlServer.h"
#include "../../common/DoubleOutLog.h"

// 
#include "HttpServerSmartHomeTelcomCtrlServer.h"
#include "../SmartHomeTelcomDef.h"

typedef struct __AgentSmartHomeTelcomCtrlServerCfg
{
#ifdef USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
    unsigned short HttpServicePort; // 如果不启用，则设置为0
#endif //USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
}
AgentSmartHomeTelcomCtrlServerCfg_T;

#define g_AgentSmartHomeTelcomCtrlServer (*CAgentSmartHomeTelcomCtrlServer::instance())

class CAgentSmartHomeTelcomCtrlServer : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentSmartHomeTelcomCtrlServer);
    CAgentSmartHomeTelcomCtrlServer();
    virtual ~CAgentSmartHomeTelcomCtrlServer();
    ////////

    BOOL Start();
    BOOL Stop();
    void ThreadProc();
    void SetDefaultConfig();

#ifdef USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
    // 设置服务，
    void SetGeneralHttpAgent(unsigned short  port);
#endif
    void ConnectAll();

    void ConnectAllSync();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void DisconnectAll();

    void Run();

    int Send2Socket(std::string strSocketName, const char *pData, size_t Len);

private:
    AgentSmartHomeTelcomCtrlServerCfg_T m_ConfigGeneralAgent;
    CHandlerSmartHomeTelcomCtrlServer *m_pHandlerSmartHomeTelcomCtrlServer;
    CDoubleOutLog m_Log;

#ifdef USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER

    ListenSocket<CHttpServerSmartHomeTelcomCtrlServer> *m_pCHttpServerSmartHomeTelcomCtrlServer;
#endif // USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER

}
; //CAgentSmartHomeTelcomCtrlServer
#endif //_AgentSmartHomeTelcomCtrlServerCtrlServer_H
