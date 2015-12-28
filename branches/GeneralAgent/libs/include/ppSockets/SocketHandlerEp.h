/** \file SocketHandlerEp.h
 **	\date  2010-02-13
**/

#ifndef _SOCKETHANDLEREP_H
#define _SOCKETHANDLEREP_H

#include "SocketHandler.h"
#ifdef LINUX
#include <sys/epoll.h>

#define MAX_EVENTS_EP_WAIT 100
#endif // LINUX

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class SocketHandlerEp : public SocketHandler
{
public:
	/** SocketHandler constructor.
		\param log Optional log class pointer */
	SocketHandlerEp(StdLog *log = NULL);

	/** SocketHandler threadsafe constructor.
		\param mutex Externally declared mutex variable
		\param log Optional log class pointer */
	SocketHandlerEp(IMutex& mutex,StdLog *log = NULL);

	SocketHandlerEp(IMutex&, ISocketHandler& parent, StdLog * = NULL);

	~SocketHandlerEp();

	ISocketHandler *Create(StdLog * = NULL);
	ISocketHandler *Create(IMutex&, ISocketHandler&, StdLog * = NULL);

#ifdef LINUX

	/** Set read/write/exception file descriptor sets (fd_set). */
	void ISocketHandler_Add(Socket *,bool bRead,bool bWrite);
	void ISocketHandler_Mod(Socket *,bool bRead,bool bWrite);
	void ISocketHandler_Del(Socket *);

	size_t MaxCount() {
		return 10000; // %!
	}

protected:
	/** Actual call to select() */
	int ISocketHandler_Select(struct timeval *);
#endif // LINUX

private:
	int m_epoll; ///< epoll file descriptor
#ifdef LINUX
	struct epoll_event m_events[MAX_EVENTS_EP_WAIT];

#endif // LINUX

};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETHANDLEREP_H
