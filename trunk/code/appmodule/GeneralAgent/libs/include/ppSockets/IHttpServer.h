/**
 **	\file IHttpServer.h
 **	\date  2007-10-05
**/
#ifndef _SOCKETS_IHttpServer_H
#define _SOCKETS_IHttpServer_H

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class HttpRequest;
class HttpResponse;

class IHttpServer
{
public:
	virtual ~IHttpServer() {}

	/** Complete request has been received and parsed. Send response
	    using the Respond() method. */
	virtual void IHttpServer_OnExec(const HttpRequest& req) = 0;

	/** Send response. */
	virtual void IHttpServer_Respond(const HttpResponse& res) = 0;

	/** Called when the body part of the response has been sent. */
	virtual void IHttpServer_OnResponseComplete() = 0;
};




#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

#endif // _SOCKETS_IHttpServer_H

