/**
 **	\file sockets-config.h
 **	\date  2007-04-14
**/
#ifndef _SOCKETS_CONFIG_H
#define _SOCKETS_CONFIG_H

/* Limits */
#define TCP_LINE_SIZE 8192
#define MAX_HTTP_HEADER_COUNT 200
#define PPSOCKET_HTTP_VERSION "HTTP/1.0"
//#define PPSOCKET_HTTP_VERSION "HTTP/1.1"

#ifndef _RUN_DP
/* First undefine symbols if already defined. */
#undef HAVE_OPENSSL
#undef ENABLE_IPV6
#undef USE_SCTP
#undef NO_GETADDRINFO
#undef ENABLE_POOL
#undef ENABLE_SOCKS4
#undef ENABLE_RESOLVER
#undef ENABLE_RECONNECT
#undef ENABLE_DETACH
#undef ENABLE_EXCEPTIONS
#undef ENABLE_XML
#endif // _RUN_DP


/* OpenSSL support. */
//#define HAVE_OPENSSL


/* Ipv6 support. */
//#define ENABLE_IPV6


/* SCTP support. */
//#define USE_SCTP


/* Define NO_GETADDRINFO if your operating system does not support
   the "getaddrinfo" and "getnameinfo" function calls. */
#define NO_GETADDRINFO


/* Connection pool support. */
//#define ENABLE_POOL


/* Socks4 client support. */
//#define ENABLE_SOCKS4


/* Asynchronous resolver. */
#define ENABLE_RESOLVER


/* Enable TCP reconnect on lost connection.
	Socket::OnReconnect
	Socket::OnDisconnect
*/
#define ENABLE_RECONNECT


/* Enable socket thread detach functionality. */
#define ENABLE_DETACH


/* Enabled exceptions. */
#define ENABLE_EXCEPTIONS


/* XML classes. */
//#define ENABLE_XML


/* Resolver uses the detach function so either enable both or disable both. */
#ifndef ENABLE_DETACH
#undef ENABLE_RESOLVER
#endif


#endif // _SOCKETS_CONFIG_H
