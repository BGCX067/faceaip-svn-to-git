/**
  *Apple Notification Service
  *
  *2012-11-19 10:26 by sunzq
*/

#ifndef _MYAPNS_H
#define _MYAPNS_H
 
#ifdef linux
#include <assert.h>
#include "openssl/pem.h"
#include "openssl/rsa.h"
#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/rand.h"
 
#include "errno.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "unistd.h"
#include <arpa/inet.h>
#include <netdb.h>
 
#include "Utility.h"

#include "EZThread.h"
 
#define APNS_DEV
#if defined(APNS_DEV)
 
#define CA_CERT_PATH    	"/home/sunzq/tshome-p2p-server-standalone/src/service/notification/pem"
#define RSA_CLIENT_CERT     "/home/sunzq/tshome-p2p-server-standalone/src/service/notification/pem/apns-dev-cert.pem"
#define RSA_CLIENT_KEY      "/home/sunzq/tshome-p2p-server-standalone/src/service/notification/pem/apns-dev-key-noenc.pem"
/* Development Connection Infos */
#define APPLE_HOST          "gateway.sandbox.push.apple.com"
#define APPLE_PORT          2195
 
#define APPLE_FEEDBACK_HOST "feedback.sandbox.push.apple.com"
#define APPLE_FEEDBACK_PORT 2196
 
#else
#define CA_CERT_PATH    "./pem"
#define RSA_CLIENT_CERT     "./pem/apns-dev-noenc.pem"
#define RSA_CLIENT_KEY      "./pem/apns-dev-noenc.pem"
#define APPLE_HOST          "gateway.push.apple.com"
#define APPLE_PORT          2195
#define APPLE_FEEDBACK_HOST "feedback.push.apple.com"
#define APPLE_FEEDBACK_PORT 2196
#endif

 
class CSSLComm
{
 public:
    CSSLComm();
    ~CSSLComm();
 
    bool connected();
    bool ssl_connect(const char *host, int port, const char *certfile, const char *keyfile, const char* capath);
    void PushNotification(const char *pToken,const char *pMsg);
 
    void GenPushData(const char *pToken);
    int GenPayloadData(int badgeNum,const char *pMsg = NULL);
 
    private:
    void Reset();
 
    private:
 
    SSL_CTX         *m_pctx;
    SSL             *m_pssl;
    const SSL_METHOD      *m_pmeth;
    X509            *m_pserver_cert;
    EVP_PKEY        *m_pkey;
 
    /* Socket Communications */
    struct sockaddr_in   m_server_addr;
    struct hostent      *m_phost_info;
 
    int                  m_sockfd;
    uint16_t m_tokenLen;
    struct PUSHDATA
    {
        char szToken[1+2+32];
        char szPayload[2+256];
    }m_data;
	
    CEZMutex m_lock;
};
 
#endif
 
#endif // _MYAPNS_H