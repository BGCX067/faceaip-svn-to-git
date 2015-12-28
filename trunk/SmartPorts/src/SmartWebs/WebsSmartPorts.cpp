/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * WebsSmartPorts.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: WebsSmartPorts.cpp 5884 2013-07-01 04:32:32Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-01 04:32:32 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*sleep*/
#include <iostream>

#include "../Logs.h"

#include "../Configs/ConfigSmartWebs.h"
#include	"./smart_webs.h"

#include "WebsSmartPorts.h"

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

PATTERN_SINGLETON_IMPLEMENT(CWebsSmartPorts);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CWebsSmartPorts::CWebsSmartPorts() :CEZThread("CWebsSmartPorts", TP_DEFAULT)
{
    DBG(
        trace("CWebsSmartPorts Enter--------\n");
    );
}

CWebsSmartPorts::~CWebsSmartPorts()
{
    DBG(
        trace("CWebsSmartPorts Leave--------\n");
    );
}

BOOL CWebsSmartPorts::Start()
{
    if (m_bLoop)
    {
        return TRUE;
    }

#ifdef USE_SMARTWEBS

    LOG4CPLUS_INFO(LOG_SOLAR, "SmartWebs starting ...");
    StartSmartWebs();
#endif//USE_SMARTWEBS


    int ret = CreateThread();

    return ret;
}

BOOL CWebsSmartPorts::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = FALSE;
        DestroyThread();
    }

#ifdef USE_SMARTWEBS

    StopSmartWebs();
#endif//USE_SMARTWEBS

    return TRUE;
}
void CWebsSmartPorts::ThreadProc()
{
    while (m_bLoop)
    {
        smart_webs_proc();
    }
}

#ifdef USE_SMARTWEBS
int CWebsSmartPorts::StartSmartWebs()
{
    CConfigSmartWebs __cfgTmp;
    __cfgTmp.update();

    smart_webs_init();

    smart_webs_open(__cfgTmp.getConfig().SPort, __cfgTmp.getConfig().iRetries);
    LOG4CPLUS_INFO(LOG_SOLAR, "smart_webs_open:" << __cfgTmp.getConfig().SPort<<":"<< __cfgTmp.getConfig().iRetries);

    std::cout << "smart_webs_open:" << __cfgTmp.getConfig().SPort<</*":"<< __cfgTmp.getConfig().iRetries <<*/ std::endl;

    return 0;
}

int CWebsSmartPorts::StopSmartWebs()
{
    smart_webs_close();

    smart_webs_cleanup();

    return 0;
}
#endif//USE_SMARTWEBS

