/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentSmsOverHttp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentSmsOverHttp.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentSmsOverHttp_H
#define _AgentSmsOverHttp_H

//#include "CommonInclude.h"
#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

// 反应堆
#include "HandlerSmsOverHttp.h"
#include "../DoubleOutLog.h"

// 
#include "HttpServerSmsOverHttp.h"

typedef struct __AgentSmsOverHttpCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0
}
AgentSmsOverHttpCfg_T;

#define g_AgentSmsOverHttp (*CAgentSmsOverHttp::instance())

class CAgentSmsOverHttp : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentSmsOverHttp);
    CAgentSmsOverHttp();
    virtual ~CAgentSmsOverHttp();
    ////////

    BOOL Start();
    BOOL Stop();
    void ThreadProc();
    void SetDefaultConfig();

    // 设置服务，
    void SetGeneralHttpAgent(unsigned short  port);
    void ConnectAll();

    void ConnectAllSync();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void DisconnectAll();

    void Run();

    int Send2Socket(std::string strSocketName, const char *pData, size_t Len);
    	
    CHandlerSmsOverHttp *GetSmsOverHttpHandler();

private:
    AgentSmsOverHttpCfg_T m_ConfigOfSmsOverHttp;
    CHandlerSmsOverHttp *m_pHandlerSmsOverHttp;
    CDoubleOutLog m_Log;

    ListenSocket<CHttpServerSmsOverHttp> *m_pCHttpServerSmsOverHttp;

}
; //CAgentSmsOverHttp
#endif //_AgentSmsOverHttp_H
