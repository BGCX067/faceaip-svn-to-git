/**
 **	\file EZSemaphore.h
 **	\date  2007-04-13
**/
#ifndef _SOCKETS_CEZSemaphore_H
#define _SOCKETS_CEZSemaphore_H

#include "ezthread-config.h"

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


#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

#ifdef _WIN32
typedef LONG ez_libs_value_t;
#else
typedef unsigned int ez_libs_value_t;
#endif

/** pthread semaphore wrapper.
	\ingroup threading */
class CEZSemaphore
{
public:
	CEZSemaphore(ez_libs_value_t start_val = 0);
	~CEZSemaphore();

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
	CEZSemaphore(const CEZSemaphore& ) {} // copy constructor
	CEZSemaphore& operator=(const CEZSemaphore& ) { return *this; } // assignment operator
#ifdef _WIN32
	HANDLE m_handle;
#else
	sem_t m_sem;
#endif
};




#ifdef EZLIBS_NAMESPACE
} // namespace EZLIBS_NAMESPACE {
#endif
#endif // _SOCKETS_CEZSemaphore_H

