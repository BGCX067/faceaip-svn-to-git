/**
 **	\file SocketStream.h
 **	\date  2008-12-20
**/

#ifndef _SOCKETS_SocketStream_H
#define _SOCKETS_SocketStream_H

#include "IStream.h"

#include "socket_include.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class ISocketHandler;
class TcpSocket;

class SocketStream : public IStream
{
public:
	SocketStream(ISocketHandler& h, TcpSocket *sock);

	size_t IStreamRead(char *buf, size_t max_sz);

	void IStreamWrite(const char *buf, size_t sz);

private:
	ISocketHandler& m_handler;
	TcpSocket *m_socket;
	socketuid_t m_socket_uid;
};

#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_SocketStream_H
