/**
 **	\file Semaphore.h
 **	\date  2007-04-13
**/
#ifndef _SOCKETS_Semaphore_H
#define _SOCKETS_Semaphore_H

#include "sockets-config.h"
#ifdef _WIN32
#include "socket_include.h"
#include <windows.h>
#else
#include <pthread.h>
#ifdef MACOSX
#include <sys/semaphore.h>
#else
#include <semaphore.h>
#endif
#endif

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

#ifdef _WIN32
typedef LONG value_t;
#else
typedef unsigned int value_t;
#endif

/** pthread semaphore wrapper.
	\ingroup threading */
class Semaphore
{
public:
	Semaphore(value_t start_val = 0);
	~Semaphore();

	/** \return 0 if successful */
	int Post();
	/** Wait for Post
	    \return 0 if successful */
	int Wait();

	/** Not implemented for win32 */
	int TryWait();

	/** Not implemented for win32 */
	int GetValue(int&);

private:
	Semaphore(const Semaphore& ) {} // copy constructor
	Semaphore& operator=(const Semaphore& ) { return *this; } // assignment operator
#ifdef _WIN32
	HANDLE m_handle;
#else
	sem_t m_sem;
#endif
};




#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
#endif // _SOCKETS_Semaphore_H

