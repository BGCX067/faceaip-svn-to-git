/**
 **	\file SSLInitializer.h
 **	\date  2007-04-30
**/
#ifndef __SOCKETS_SSLInitializer_H
#define __SOCKETS_SSLInitializer_H
#include "sockets-config.h"
#ifdef HAVE_OPENSSL

#ifdef _WIN32
#include <winsock2.h>
#endif
#include <openssl/ssl.h>
#include <string>
#include <map>
#include "Mutex.h"


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class SSLInitializer
{
public:
	/**
		init openssl
		bio_err
		create random file
	*/
	SSLInitializer();

	/**
		remove random file
	*/
	~SSLInitializer();

	void DeleteRandFile();

	/** SSL; mutex locking function callback. */
static	void SSL_locking_function(int mode, int n, const char *file, int line);

	/** Return thread id. */
static	unsigned long SSL_id_function();

	BIO *bio_err;

private:
	std::string m_rand_file;
	long m_rand_size;
static	std::map<int, IMutex *> *m_mmap;
static	Mutex *m_mmap_mutex;

static	std::map<int, IMutex *>& MMap();
static	Mutex& MMapMutex();

};




#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
#endif // HAVE_OPENSSL
#endif // __SOCKETS_SSLInitializer_H

