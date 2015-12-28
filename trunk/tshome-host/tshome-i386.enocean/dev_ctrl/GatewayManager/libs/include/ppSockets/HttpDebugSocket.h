/** \file HttpDebugSocket.h
 **	\date  2004-09-27
**/

#ifndef _SOCKETS_HttpDebugSocket_H
#define _SOCKETS_HttpDebugSocket_H

#include "sockets-config.h"
#include "HTTPSocket.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class ISocketHandler;

/** HTTP request "echo" class. This class echoes a http request/body
with a html formatted page. 
	\ingroup http */
class HttpDebugSocket : public HTTPSocket
{
public:
	HttpDebugSocket(ISocketHandler&);
	~HttpDebugSocket();

	void Init();

	void OnFirst();
	void OnHeader(const std::string& key,const std::string& value);
	void OnHeaderComplete();
	void OnData(const char *,size_t);
	void OnDataComplete();

private:
	HttpDebugSocket(const HttpDebugSocket& s) : HTTPSocket(s) {} // copy constructor
	HttpDebugSocket& operator=(const HttpDebugSocket& ) { return *this; } // assignment operator
	int m_content_length;
	int m_read_ptr;
};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_HttpDebugSocket_H

