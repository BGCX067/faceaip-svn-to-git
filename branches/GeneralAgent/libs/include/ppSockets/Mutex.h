/** \file Mutex.h
 **	\date  2004-10-30
**/
#ifndef _SOCKETS_Mutex_H
#define _SOCKETS_Mutex_H

#include "sockets-config.h"
#ifndef _WIN32
#include <pthread.h>
#else
#include "socket_include.h"
#include <windows.h>
#endif
#include "IMutex.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

/** Mutex container class, used by Lock. 
	\ingroup threading */
class Mutex : public IMutex
{
public:
	Mutex();
	~Mutex();

	virtual void Lock() const;
	virtual void Unlock() const;

private:
#ifdef _WIN32
	HANDLE m_mutex;
#else
	mutable pthread_mutex_t m_mutex;
#endif
};


#ifdef SOCKETS_NAMESPACE
}
#endif
#endif // _SOCKETS_Mutex_H

