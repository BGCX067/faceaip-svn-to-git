/**
 **	\file HttpBaseSocket.h
 **	\date  2007-10-05

**/
#ifndef _HttpBaseSocket_H
#define _HttpBaseSocket_H

#include "HTTPSocket.h"
#include "HttpRequest.h"
#include "IHttpServer.h"
#include "HttpResponse.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class HttpResponse;

class HttpBaseSocket : public HTTPSocket, public IHttpServer
{
public:
	HttpBaseSocket(ISocketHandler& h);
	~HttpBaseSocket();

	void OnFirst();
	void OnHeader(const std::string& key,const std::string& value);
	void OnHeaderComplete();
	void OnData(const char *,size_t);

	// implements IHttpServer::Respond
	void IHttpServer_Respond(const HttpResponse& res);

	void OnTransferLimit();

protected:
	HttpBaseSocket(const HttpBaseSocket& s) : HTTPSocket(s) {} // copy constructor
	//
	HttpRequest m_req;
	HttpResponse m_res;
	void Reset();

private:
	HttpBaseSocket& operator=(const HttpBaseSocket& ) { return *this; } // assignment operator
	void Execute();
	//
	size_t m_body_size_left;
	bool m_b_keepalive;
};




#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

#endif // _HttpBaseSocket_H

