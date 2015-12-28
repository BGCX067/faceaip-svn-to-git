/** \file EventHandler.h
 **	\date  2005-12-07
**/
#ifndef _SOCKETS_EventHandler_H
#define _SOCKETS_EventHandler_H

#include "sockets-config.h"
#include "SocketHandler.h"
#include "IEventHandler.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class StdLog;
class IEventOwner;
class Event;
class TcpSocket;

/** SocketHandler implementing the IEventHandler interface.
	\ingroup timer */
class EventHandler : public SocketHandler,public IEventHandler
{
public:
	EventHandler(StdLog * = NULL);
	EventHandler(IMutex&,StdLog * = NULL);
	~EventHandler();

	bool GetTimeUntilNextEvent(struct timeval *tv);
	void CheckEvents();
	long AddEvent(IEventOwner *from,long sec,long usec);
	void ClearEvents(IEventOwner *from);
	void RemoveEvent(IEventOwner *from,long eid);

	/** SocketHandler while() loop implemented with event functionality. */
	void EventLoop();
	/** Stop event loop. */
	void SetQuit(bool = true);

	void Add(Socket *);

private:
	EventHandler(const EventHandler& ) {} // copy constructor
	EventHandler& operator=(const EventHandler& ) { return *this; } // assignment operator
	std::list<Event *> m_events;
	bool m_quit;
};



#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_EventHandler_H

