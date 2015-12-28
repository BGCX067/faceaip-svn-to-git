/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ez_socket.h - socket API
 *
 * Copyright (C) 2005 QiYang Technologies, All Rights Reserved.
 *
 * $Id: ez_socket.h 16364 2008-04-25 03:59:51Z guan_shiyong $
 *
 * socket api, fit for the past and the future.
 * Platform:
 * 		linux+gcc / win32+vc6 / cygwin
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2005-04-02 17:19:24 WuJunjie  Create
 *		wjj 2005-04-02 15:16:48
 *			Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef	__EZ_SOCKET_H
#define	__EZ_SOCKET_H

#ifdef  __cplusplus
extern "C" {
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/* configuration options for current OS */
#include	"../ez_config.h"

/* debug and test parameter */
#include	"../ez_appdef.h" 

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* Following shortens all the typecasts of pointer arguments: */
#define	SA	struct sockaddr

#ifdef WIN32
static signed char __initialized = 0;
#endif

#ifndef	HAVE_SYSLOG_H
#define syslog(x, y)
#endif

int		ez_snprintf(char *buf, size_t size, const char *fmt, ...);
#define	ez_bzero(ptr,n)		memset(ptr, 0, n)

#define	__ez_min(a,b)	((a) < (b) ? (a) : (b))
#define	__ez_max(a,b)	((a) > (b) ? (a) : (b))

int ez_sleep(int second, int microsecond);
int ez_log2file(char *pFilePathName, char *pLog, char *pMod);
char * get_date_time_string();
char * get_date_time_string_compress();
int get_pub_ip_from_mb(char *buf, size_t count);
// 2012-2-23 8:34:33 qq's url not valid
//int get_pub_ip_from_qq(char *pPubIP, size_t count);
int get_pub_ip_from_sina(char *pPubIP, size_t count);
int get_pub_ip_from_faceaip(char *pPubIP, size_t count);
int get_pub_ip(char *pPubIP, size_t count);
// ret 0 seccess, <0 failed
int ez_ivs_updateip(
    const char *pLogin		 ,
    const char *pPasswd      ,
    const char *pPuid        ,
    const char *pPuip        ,
    const char *pPuPort      ,
    const char *pHttpPort    ,
    const char *pPuname      ,
    const char *pMac		 ,
    const char *pAttr		 ,
    char *recvline			 , 
    size_t count);
// ret 0 seccess, <0 failed
int ez_get_ivs_service(
    const char * const pPuid ,
    const char * const pService,
    char *recvline, 
    size_t count);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

ez_socket_t	 ez_tcp_server(const ez_port_t, const int);
ez_socket_t	 ez_tcp_connect(const char * hostip, const ez_port_t port, const long usec_timeout);
ez_socket_t	 ez_tcp_client(char *, char *);

ez_socket_t	 ez_udp_server(const ez_port_t);
ez_socket_t	 ez_udp_connect(const char * hostip, const ez_port_t port);
ez_socket_t	 ez_udp_client(const char * hostip, const ez_port_t port, SA *saptr, ez_socklen_t *lenp);
ez_socket_t ez_udp_connect_host(char *hname, char *sname);

int			 ez_mcast_set_loop(ez_socket_t, unsigned char);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+- raw socket api-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

ez_socket_t	 ez_ip_snif_socket(const char *);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+basic socket api-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

ez_socket_t	 ez_socket(int, int, int);
int		 ez_bind(ez_socket_t, const SA *, ez_socklen_t);
int		 ez_listen(ez_socket_t, int);
ez_socket_t	 ez_accept(ez_socket_t, SA *, ez_socklen_t *);
int		 ez_connect(ez_socket_t, const SA *, ez_socklen_t);
int ez_read_with_timeout(int sockfd, void *buf, size_t count, struct timeval *ptime_out);

int		 ez_socket_init(void);
int		 ez_socket_cleanup(void);
int		 ez_close(ez_socket_t fd);
int		 ez_close_socket(ez_socket_t *fd);

struct hostent * ez_gethostbyname(const char * name);
int		 ez_getpeername(ez_socket_t, SA *, ez_socklen_t *);
int		 ez_getsockname(ez_socket_t, SA *, ez_socklen_t *);
int		 ez_gethostname(char *name, size_t len);

int		 ez_getsockopt(ez_socket_t, int, int, void *, ez_socklen_t *);
int		 ez_setsockopt(ez_socket_t, int, int, const void *, ez_socklen_t);
int		 ez_shutdown(ez_socket_t, int);
int		 ez_select(int, fd_set *, fd_set *, fd_set *, struct timeval *);

en_bool	 ez_set_nodelay(ez_socket_t s);
en_bool	 ez_set_nonblock(en_bool bNb, ez_socket_t s);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+- socket io   -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int ez_writen(ez_socket_t fd, const void *vptr, size_t n);
int ez_writeton( ez_socket_t fd, const void *vptr, size_t n, const SA *saRemote, int struct_size);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+- socket addr -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int		 ez_inet_aton(const char *cp, struct in_addr *ap);
int		 ez_inet_pton(int family, const char *strptr, void *addrptr);
int		 ez_set_address(char *hname,
					 char *sname,
					 struct sockaddr_in *sap,
					 char *protocol );
#define HTTP_MAXLINE 4096
#define HTTP_MAXSUB  1024
#define DEFAULT_HTTP_PORT "80"
ssize_t ez_http_get(ez_socket_t sockfd, char *host, char *page, char *recvline, int recvbuflen);
ssize_t ez_http_post(ez_socket_t sockfd, char *host, char *page, char *poststr, char *recvline, int recvbuflen);

en_bool	 ez_is_valid_ip4(const char *str);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ez_pthread_create(ez_pthread_t *	phThread,
                      ez_pthread_attr_t	thread_attr,
                      ez_threadfunc_t (WINAPI *pStartAddress)(ez_threadparam_t),
                      ez_threadparam_t	pParameter);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if (defined(EZ_ERROR_FUNCTION) && (EZ_ERROR_FUNCTION != 0))
	void	 ez_err_dump(const char *, ...);
	void	 ez_err_msg(const char *, ...);
	void	 ez_err_quit(const char *, ...);
	void	 ez_err_ret(const char *, ...);
	void	 ez_err_sys(const char *, ...);
#else
	#ifdef PSOS
		#define ez_err_sys myprintf
	#else
		#define ez_err_sys printf
	#endif
/*	#define ez_err_ret(format, a...) do{ printf(format, ## a); } while(0)
	#define ez_err_sys(format, a...) do{ printf(format, ## a); exit(1); } while(0)
	#define ez_err_dump(format, a...) do{ printf(format, ## a); exit(1); } while(0)
	#define ez_err_msg(format, a...) do{ printf(format, ## a); } while(0)
	#define ez_err_quit(format, a...) do{ printf(format, ## a); exit(1); } while(0)*/
#endif	/* EZ_ERROR_FUNCTION */ 

//#define __trip __ez_pritnf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
//#define __fline __ez_pritnf("%s(%d)--", __FILE__, __LINE__);

#ifdef  __cplusplus
}
#endif

#endif	/* ADSOCKET_H */

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- end -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

