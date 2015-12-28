/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ezSms.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ezSms.h 5884 2013-07-16 07:30:00Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-16 07:30:00 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ezSms_H__
#define __ezSms_H__

#include "EZThread.h"
#include "EZTimer.h"

#include "../Configs/ConfigGeneral.h"

class CezSms : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CezSms);
	
	BOOL Start();
	BOOL Stop();
	void ThreadProc();
private:
	CezSms();
	virtual ~CezSms();

	time_t m_ttLastOn;

CConfigGeneral m_cfgGeneral;
 };
 #define g_ezSms (*CezSms::instance())

#endif //__ezSms_H__
