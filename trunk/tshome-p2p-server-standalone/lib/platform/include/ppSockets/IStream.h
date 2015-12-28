/** \file IStream.h
 **	\date  2008-11-15
**/

#ifndef _SOCKETS_IStream_H
#define _SOCKETS_IStream_H

#include "sockets-config.h"
#include <string>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class IStream
{
public:
	virtual ~IStream() {}

	/** Try to read 'buf_sz' number of bytes from source.
	    \return Number of bytes actually read. */
	virtual size_t IStreamRead(char *buf, size_t buf_sz) = 0;

	/** Write 'sz' bytes to destination. */
	virtual void IStreamWrite(const char *buf, size_t sz) = 0;

};

#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_IStream_H
