/** \file HttpPutSocket.h
 **	\date  2004-10-30
**/
#ifndef _SOCKETS_HttpPutSocket_H
#define _SOCKETS_HttpPutSocket_H

#include "sockets-config.h"
#include "HttpClientSocket.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class ISocketHandler;

/** Put http page. 
	\ingroup http */
class HttpPutSocket : public HttpClientSocket
{
public:
	HttpPutSocket(ISocketHandler&);
	/** client constructor, 
		\param url_in = 'http://host:port/resource' */
	HttpPutSocket(ISocketHandler&,const std::string& url_in);
	~HttpPutSocket();

	// these must be specified before connecting / adding to handler
	/** Set filename to send. */
	void SetFile(const std::string& );
	/** Set mimetype of file to send. */
	void SetContentType(const std::string& );

	/** connect to host:port derived from url in constructor */
	void Open();

	/** http put client implemented in OnConnect */
	void OnConnect();

private:
	HttpPutSocket(const HttpPutSocket& s) : HttpClientSocket(s) {} // copy constructor
	HttpPutSocket& operator=(const HttpPutSocket& ) { return *this; } // assignment operator
	//
	std::string m_filename;
	std::string m_content_type;
	long m_content_length;
};




#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_HttpPutSocket_H

