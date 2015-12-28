/** \file EventTime.h
 **	\date  2005-12-07
**/
#ifndef _SOCKETS_EventTime_H
#define _SOCKETS_EventTime_H

#include "sockets-config.h"
#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


#if defined( _WIN32) && !defined(__CYGWIN__)
typedef __int64 mytime_t;
#else
#include <inttypes.h> // int64_t
typedef int64_t mytime_t;
#endif


/** \defgroup timer EventTimer event handling */

/** EventTime primitive, returns current time as a 64-bit number.
	\ingroup timer */
class EventTime
{
public:
	EventTime();
	EventTime(mytime_t sec,long usec);
	~EventTime();

	static mytime_t Tick();

	operator mytime_t () { return m_time; }
	EventTime operator - (const EventTime& x) const;
	bool operator < (const EventTime& x) const;

private:
	EventTime(const EventTime& ) {} // copy constructor
	EventTime& operator=(const EventTime& ) { return *this; } // assignment operator
	mytime_t m_time;
};



#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_EventTime_H

