/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * WebsSmartPorts.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: WebsSmartPorts.h 5884 2013-07-01 04:32:32Z WuJunjie $
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

#ifndef _WebsSmartPorts_H
#define _WebsSmartPorts_H 1

#include "EZThread.h"
#include "EZTimer.h"
#include <string>
#define g_WebsSmartPorts (*CWebsSmartPorts::instance())

class CWebsSmartPorts : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CWebsSmartPorts);
	
	BOOL Start();
	BOOL Stop();
	void ThreadProc();
private:
	CWebsSmartPorts();
	virtual ~CWebsSmartPorts();
    // ÐÂÔö
#ifdef USE_SMARTWEBS

    int StartSmartWebs();
    int StopSmartWebs();
#endif//USE_SMARTWEBS

 };
#endif /* _WebsSmartPorts_H */
