/** \file EZLock.h
 **	\date  2005-08-22
**/
#ifndef _SOCKETS_CLock_H
#define _SOCKETS_CLock_H

#include "ezthread-config.h"
#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

class CEZMutex;

/** ICMutex encapsulation class. 
	\ingroup threading */
class CEZLock
{
public:
	CEZLock(const CEZMutex&);
	~CEZLock();

private:
	const CEZMutex& m_mutex;
};




#ifdef EZLIBS_NAMESPACE
}
#endif
#endif // _SOCKETS_CLock_H

