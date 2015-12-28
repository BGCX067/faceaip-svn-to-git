/** \file ResolvServer.h
 **	\date  2005-03-24
**/
#ifndef _SOCKETS_ResolvServer_H
#define _SOCKETS_ResolvServer_H
#include "sockets-config.h"
#ifdef ENABLE_RESOLVER
#include "socket_include.h"
#include "Thread.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

/** \defgroup async Asynchronous DNS */
/** Async DNS resolver thread. 
	\ingroup async */
class ResolvServer : public Thread
{
public:
	ResolvServer(port_t);
	~ResolvServer();

	void Run();
	void Quit();

	bool Ready();

private:
	ResolvServer(const ResolvServer& ) {} // copy constructor
	ResolvServer& operator=(const ResolvServer& ) { return *this; } // assignment operator

	bool m_quit;
	port_t m_port;
	bool m_ready;
};




#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // ENABLE_RESOLVER
#endif // _SOCKETS_ResolvServer_H

