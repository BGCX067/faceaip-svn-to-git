/** \file FileLog.h
 **	\date  2004-06-01
**/
#ifndef _SOCKETS_FileLog_H
#define _SOCKETS_FileLog_H

#include "sockets-config.h"
#include "StdLog.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


/** StdLog implementation, logs to stdout. 
	\ingroup logging */
class FileLog : public StdLog
{
public:
	FileLog(loglevel_t min_level = LOG_LEVEL_INFO) : m_min_level(min_level) {}
	void error(ISocketHandler *,Socket *,const std::string& call,int err,const std::string& sys_err,loglevel_t, int file_line=-1, char *pfile=NULL);

private:
	loglevel_t m_min_level;
};




#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_FileLog_H

