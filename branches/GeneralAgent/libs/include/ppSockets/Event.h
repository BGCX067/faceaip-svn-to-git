/** \file Event.h
 **	\date  2005-12-07
**/
#ifndef _SOCKETS_Event_H
#define _SOCKETS_Event_H

#include "sockets-config.h"
#ifdef _WIN32
#else
#include <sys/select.h>
#endif
#include "EventTime.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class IEventOwner;

/** Store information about a timer event.
	\ingroup timer */
class Event
{
public:
	Event(IEventOwner *,long sec,long usec,unsigned long data = 0);
	~Event();

	bool operator<(Event&);
	long GetID() const;
	const EventTime& GetTime() const;
	IEventOwner *GetFrom() const;
	unsigned long Data() const;

private:
	Event(const Event& ) {} // copy constructor
	Event& operator=(const Event& ) { return *this; } // assignment operator
	IEventOwner *m_from;
	unsigned long m_data;
	EventTime m_time;
	static long m_unique_id;
	long m_id;
};



#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_Event_H

