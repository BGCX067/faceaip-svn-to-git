/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DoubleOutLog.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DoubleOutLog.h 5884 2012-09-19 11:46:08Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-19 11:46:08  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __DOUBLEOUTLOG_H
#define __DOUBLEOUTLOG_H

#include "sockets-config.h"
#include "StdLog.h"

/** error level enum. */
/*
typedef enum
{
	LOG_LEVEL_INFO = 0,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
} loglevel_t;
*/
/** StdLog implementation, logs to stdout. 
	\ingroup logging */ 
class CDoubleOutLog : public StdLog
{
public:
	CDoubleOutLog(loglevel_t min_level = LOG_LEVEL_ERROR) : m_min_level(min_level) {}
	void error(ISocketHandler *,Socket *,const std::string& call,int err,const std::string& sys_err,loglevel_t);

private:
	loglevel_t m_min_level;
};

#endif // __DOUBLEOUTLOG_H

