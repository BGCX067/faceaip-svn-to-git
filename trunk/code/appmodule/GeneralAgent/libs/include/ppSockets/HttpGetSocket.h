/** \file HttpGetSocket.h
 **	\date  2004-02-13
**/
#ifndef _SOCKETS_HttpGetSocket_H
#define _SOCKETS_HttpGetSocket_H

#include "sockets-config.h"
#include "HttpClientSocket.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


/** Get http page. 
	\ingroup http */
class HttpGetSocket : public HttpClientSocket
{
public:
	HttpGetSocket(ISocketHandler&);
	HttpGetSocket(ISocketHandler&,const std::string& url,const std::string& to_file = "");
	HttpGetSocket(ISocketHandler&,const std::string& host,port_t port,const std::string& url,const std::string& to_file = "");
	~HttpGetSocket();

	void OnConnect();

protected:
	HttpGetSocket& operator=(const HttpGetSocket& ) { return *this; }
	HttpGetSocket(const HttpGetSocket& s) : HttpClientSocket(s) {}
};




#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_HttpGetSocket_H

