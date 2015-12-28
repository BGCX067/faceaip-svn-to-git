/** \file IEventOwner.h
 **	\date  2005-12-07
**/
#ifndef _SOCKETS_IEventOwner_H
#define _SOCKETS_IEventOwner_H

#include "sockets-config.h"
#include "IEventHandler.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


/** Any class that wants to use timer events inherits this.
	\ingroup timer */
class IEventOwner
{
public:
	IEventOwner(IEventHandler& h);
	virtual ~IEventOwner();

	/** Schedule event.
		\param sec Seconds until event
		\param usec Microseconds until event
		\return Event ID */
	long AddEvent(long sec,long usec);
	/** Clear all events scheduled by this owner. */
	void ClearEvents();
	/** Remove one event scheduled by this owner.
		\param eid Event ID to remove */
	void RemoveEvent(long eid);
	/** Event callback will fire when time is up. */
	virtual void OnEvent(int) = 0;

	IEventHandler& GetEventHandler();
	void SetHandlerInvalid(bool x = true) { m_handler_invalid = x; }

private:
	IEventHandler& m_event_handler;
	bool m_handler_invalid;
};



#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_IEventOwner_H

