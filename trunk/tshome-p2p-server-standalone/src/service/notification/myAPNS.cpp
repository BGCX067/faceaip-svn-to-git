#ifdef linux
 
#include "myAPNS.h"
#include "../CommonInclude.h"
#include <string>
 
CSSLComm::CSSLComm()
{
    //ctor
    m_sockfd = -1;
    m_pctx = NULL;
    m_pssl = NULL;
    m_pmeth = NULL;
    m_pserver_cert = NULL;
    m_pkey = NULL;
 
    m_tokenLen = htons(32);
    memset((void*)&m_data,0,sizeof(m_data));
 
}
 
CSSLComm::~CSSLComm()
{
    //dtor
    Reset();
 
}
void CSSLComm::Reset()
{
 
    if(m_pssl)
    {
        SSL_shutdown(m_pssl);
        SSL_free(m_pssl);
        m_pssl = NULL;
    }
    if(m_pctx)
    {
        SSL_CTX_free(m_pctx);
        m_pctx = NULL;
    }
    if(m_sockfd > 2)
    {
        close(m_sockfd);
        m_sockfd = -1;
    }
 
}
 
 
bool CSSLComm::connected()
{
    if(m_sockfd < 2) return false;
 
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    fd_set fdwrite;
    fd_set fdexcept;
    FD_ZERO(&fdwrite);
    FD_ZERO(&fdexcept);
    FD_SET(m_sockfd,&fdwrite);
    FD_SET(m_sockfd,&fdexcept);
    int ret = select(m_sockfd+1,NULL,&fdwrite,&fdexcept,&timeout);
    if(ret == -1)
        return false;
    if(ret > 0)
    {
        if(FD_ISSET(m_sockfd,&fdexcept))
            return false;
        else if(FD_ISSET(m_sockfd,&fdwrite))
        {
            int err = 0;
            socklen_t len = sizeof(err);
            int result = getsockopt(m_sockfd,SOL_SOCKET,SO_ERROR,(char*)&err,&len);
            if(result < 0 || err != 0)
                return false;
            return true;
        }
    }
    return false;
}


bool CSSLComm::ssl_connect(const char *host, int port, const char *certfile, const char *keyfile, const char* capath)
{
    Reset();
    int err;
 
    /* Load encryption & hashing algorithms for the SSL program */
    SSL_library_init();
 
    /* Load the error strings for SSL & CRYPTO APIs */
    SSL_load_error_strings();
 
    /* Create an SSL_METHOD structure (choose an SSL/TLS protocol version) */
    m_pmeth = SSLv3_method();
 
    /* Create an SSL_CTX structure */
    m_pctx = SSL_CTX_new(m_pmeth);
    if(!m_pctx)
    {
        printf("Could not get SSL Context\n");
        return false;
    }
 
    /* Load the CA from the Path */
    if(SSL_CTX_load_verify_locations(m_pctx, NULL, capath) <= 0)
    {
        /* Handle failed load here */
        printf("Failed to set CA location...\n");
        ERR_print_errors_fp(stderr);
        return false;
    }
 
    /* Load the client certificate into the SSL_CTX structure */
    if (SSL_CTX_use_certificate_file(m_pctx, certfile, SSL_FILETYPE_PEM) <= 0)
    {
        printf("Cannot use Certificate File\n");
        ERR_print_errors_fp(stderr);
        return false;
    }
 
    /* Load the private-key corresponding to the client certificate */
    if (SSL_CTX_use_PrivateKey_file(m_pctx, keyfile, SSL_FILETYPE_PEM) <= 0)
    {
        printf("Cannot use Private Key\n");
        ERR_print_errors_fp(stderr);
        return false;
    }
 
    /* Check if the client certificate and private-key matches */
    if (!SSL_CTX_check_private_key(m_pctx))
    {
        printf("Private key does not match the certificate public key\n");
        return false;
    }
 
    /* Set up a TCP socket */
    m_sockfd = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(m_sockfd == -1)
    {
        printf("Could not get Socket\n");
        return false;
    }
 
    memset (&m_server_addr, '\0', sizeof(m_server_addr));
    m_server_addr.sin_family      = AF_INET;
    m_server_addr.sin_port        = htons(port);       /* Server Port number */
    m_phost_info = gethostbyname(host);
    if(m_phost_info)
    {
        /* Take the first IP */
        struct in_addr *address = (struct in_addr*)m_phost_info->h_addr_list[0];
        m_server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*address)); /* Server IP */
 
    }
    else
    {
        printf("Could not resolve hostname %s\n", host);
        return false;
    }
 
    /* Establish a TCP/IP connection to the SSL client */
    err = connect(m_sockfd, (struct sockaddr*) &m_server_addr, sizeof(m_server_addr));
    if(err == -1)
    {
        printf("Could not connect\n");
        return false;
    }
 
    /* An SSL structure is created */
    m_pssl = SSL_new(m_pctx);
    if(!m_pssl)
    {
        printf("Could not get SSL Socket\n");
        return false;
    }
 
    /* Assign the socket into the SSL structure (SSL and socket without BIO) */
    SSL_set_fd(m_pssl, m_sockfd);
 
    /* Perform SSL Handshake on the SSL client */
    err = SSL_connect(m_pssl);
    if(err == -1)
    {
        printf("Could not connect to SSL Server\n");
        return false;
    }
    return true;
 
}
 
void CSSLComm::PushNotification(const char *pToken,const char *pMsg)
{
    CEZGuard guard(m_lock);
    if(!connected())
    {
        ssl_connect(APPLE_HOST, APPLE_PORT, RSA_CLIENT_CERT, RSA_CLIENT_KEY, CA_CERT_PATH);
    }
    int paylen = GenPayloadData(1,pMsg);
    GenPushData(pToken);
 	if(m_pssl)
 	{
	    int ret = SSL_write(m_pssl, (void*)&m_data, 35 + paylen);
 	}
}
 
void CSSLComm::GenPushData(const char *pToken)
{
	memset(m_data.szToken, 0, sizeof(m_data.szToken));
    memcpy(&m_data.szToken[1],&m_tokenLen,2);
    memcpy(&m_data.szToken[3],pToken,32);
}
 
int CSSLComm::GenPayloadData(int badgeNum,const char *pMsg)
{
	std::string json("{\"aps\":{");
	json += "\"alert\":\"";
	json += pMsg;
	json += "\",";
	json += "\"badge\":";
	json += "24";
	json += ",";
	json += "\"sound\":\"";
	json += "default\"";
	json += "}}";
 	memcpy(&m_data.szPayload[2], json.c_str(), json.length());
	
    int len = json.length();
    assert(len <= 256);
    uint16_t payload_len = htons(len);
    memcpy(m_data.szPayload,&payload_len,sizeof(payload_len));

    return len + 2;
}
 
#endif
