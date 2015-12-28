/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerialLog.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerialLog.h 5884 2012-09-19 11:46:08Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-19 11:46:08  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __TeleSerialLog_H
#define __TeleSerialLog_H

#include "sockets-config.h"
#include "StdLog.h"


/** StdLog implementation, logs to stdout. 
	\ingroup logging */ 
class CTeleSerialLog : public StdLog
{
public:
	CTeleSerialLog(loglevel_t min_level = LOG_LEVEL_INFO) : m_min_level(min_level) {}
	void error(ISocketHandler *,Socket *,const std::string& call,int err,const std::string& sys_err,loglevel_t);

private:
	loglevel_t m_min_level;
};

#endif // __TeleSerialLog_H

