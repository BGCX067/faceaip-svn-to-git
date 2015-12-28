/**
 **	\file SocketHandlerThread.h
 **	\date  2010-03-21
**/

#ifndef _SOCKETHANDLERTHREAD_H
#define _SOCKETHANDLERTHREAD_H

#include "Thread.h"
#include "Semaphore.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class ISocketHandler;

class SocketHandlerThread : public Thread
{
public:
	SocketHandlerThread(ISocketHandler& parent);
	~SocketHandlerThread();

	virtual void Run();

	ISocketHandler& Handler();

	void Wait();

private:
	ISocketHandler& m_parent;
	ISocketHandler *m_handler;
	Semaphore m_sem;
};



#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

#endif // _SOCKETHANDLERTHREAD_H
