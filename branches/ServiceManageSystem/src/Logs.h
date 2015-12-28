/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Logs.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Logs.h 5884 2012-08-13 01:54:12Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-08-13 01:54:12  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __LOGS_H__
#define __LOGS_H__

#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/ndc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/loggingmacros.h>
#include <iomanip>
using namespace log4cplus;
using namespace log4cplus::helpers;

// 日志所属模块
#define  LOG_ROOT  Logger::getRoot()
#define  LOG_SOLAR   Logger::getInstance(LOG4CPLUS_TEXT("MAIN"))
#define  LOG_SMS_OVER_HTTP Logger::getInstance(LOG4CPLUS_TEXT("HSMS"))

#define  LOG_IPC   Logger::getInstance(LOG4CPLUS_TEXT("IPC"))
#define  LOG_TURN  Logger::getInstance(LOG4CPLUS_TEXT("TURN"))
#define  LOG_DB  Logger::getInstance(LOG4CPLUS_TEXT("DB"))
#define  LOG_ALLOCATION  Logger::getInstance(LOG4CPLUS_TEXT("ALLOC"))

#define  LOG_SMARTHOMEJSTELCOM  Logger::getInstance(LOG4CPLUS_TEXT("JSTEL"))
// tshome webservice
#define  LOG_WEBSERVICES  Logger::getInstance(LOG4CPLUS_TEXT("WEBS"))
#define  LOG_OF_PJLIB  Logger::getInstance(LOG4CPLUS_TEXT("PJLIB"))

//日志等级
/*
        LOG4CPLUS_DEBUG(LOG_IPC, "debug log" << i);
        LOG4CPLUS_INFO(LOG_IPC, "This is a int: " << 1000);
        LOG4CPLUS_WARN(LOG_IPC, "This is the THIRD log message...");
        LOG4CPLUS_ERROR(LOG_IPC, "This is the FOURTH log message...");
        LOG4CPLUS_FATAL(LOG_IPC, "This is the FOURTH log message... %d");
*/
#endif //__LOGS_H__
