/** \file StdLog.h
 **	\date  2004-06-01
**/
#ifndef _SOCKETS_StdLog_H
#define _SOCKETS_StdLog_H

#include "sockets-config.h"
#include <string>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

/** error level enum. */
typedef enum
{
	LOG_LEVEL_INFO = 0,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
} loglevel_t;


class ISocketHandler;
class Socket;

/** \defgroup logging Log help classes */
/** Log class interface. 
	\ingroup logging */
class StdLog
{
public:
	virtual ~StdLog() {}

	virtual void error(ISocketHandler *,Socket *,
		const std::string& user_text,
		int err,
		const std::string& sys_err,
		loglevel_t = LOG_LEVEL_WARNING) = 0;
};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_StdLog_H

