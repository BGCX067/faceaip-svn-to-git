/**
 **	\file Ajp13Socket.h
 **	\date  2007-10-05
**/
#ifndef _SOCKETS_Ajp13Socket_H
#define _SOCKETS_Ajp13Socket_H

#include "AjpBaseSocket.h"
#include "HttpRequest.h"
#include "IHttpServer.h"
#include "HttpResponse.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class HttpResponse;

class Ajp13Socket : public AjpBaseSocket, public IHttpServer
{
public:
	Ajp13Socket(ISocketHandler& h);

	void OnHeader( short id, short len );
	void OnPacket( const char *buf, size_t sz );

	// implements IHttpServer::Respond
	void IHttpServer_Respond(const HttpResponse& res);

	void OnTransferLimit();

	void SetReused(bool x = true);
	bool IsReused();

private:
	void ReceiveBody( const char *buf, size_t sz );
	void ReceiveForwardRequest( const char *buf, size_t sz );
	void ReceiveShutdown( const char *buf, size_t sz );
	void ReceivePing( const char *buf, size_t sz );
	void ReceiveCPing( const char *buf, size_t sz );
	void Execute();
	void Reset();
	//
	size_t m_body_size_left;
	HttpRequest m_req;
	HttpResponse m_res;
	bool m_b_reused;
};


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

#endif // _SOCKETS_Ajp13Socket_H

