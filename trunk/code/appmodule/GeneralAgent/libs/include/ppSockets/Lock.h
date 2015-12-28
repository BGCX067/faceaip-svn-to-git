/** \file Lock.h
 **	\date  2005-08-22
**/
#ifndef _SOCKETS_Lock_H
#define _SOCKETS_Lock_H

#include "sockets-config.h"
#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class IMutex;

/** IMutex encapsulation class. 
	\ingroup threading */
class Lock
{
public:
	Lock(const IMutex&);
	~Lock();

private:
	const IMutex& m_mutex;
};




#ifdef SOCKETS_NAMESPACE
}
#endif
#endif // _SOCKETS_Lock_H

