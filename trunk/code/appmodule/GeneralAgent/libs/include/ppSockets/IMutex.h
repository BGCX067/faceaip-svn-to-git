/** \file IMutex.h
 **	\date  2008-10-25
**/
#ifndef _SOCKETS_IMutex_H
#define _SOCKETS_IMutex_H

#include "sockets-config.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

/** IMutex interface.
	\ingroup threading */
class IMutex
{
public:
	virtual ~IMutex() {}

	virtual void Lock() const = 0;
	virtual void Unlock() const = 0;
};


#ifdef SOCKETS_NAMESPACE
}
#endif
#endif // _SOCKETS_IMutex_H

