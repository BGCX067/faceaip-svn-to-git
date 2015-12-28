#if !defined(__GETDATAHTTPPOSTSOCKET_H__)
#define __GETDATAHTTPPOSTSOCKET_H__

#include "HttpPostSocket.h"
#include "SocketHandler.h"
#include "StdoutLog.h"

class GetDataHttpPostSocket : public HttpPostSocket
{
public:
	GetDataHttpPostSocket(ISocketHandler& h, const std::string& host);
	GetDataHttpPostSocket(ISocketHandler& h, const std::string& host, std::string &strCallBack);
	virtual ~GetDataHttpPostSocket();
	const std::string& GetOnData();
private:
	void OnData(const char *buf, size_t sz);

	std::string *m_buf;
};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
char *GetPubIP(char *pPubIP, int iLen);
int UpdateIP_UsePPSocket(
    char *pLogin				,
    char *pPasswd      ,
    char *pPuid        ,
    char *pPuip        ,
    char *pPuPort      ,
    char *pHttpPort    ,
    char *pPuname      ,
    char *pMac		   ,
    char *pAttr
);
int GetService(
    const char * const pPuid        ,
    const char * const pService,
    std::string & strService);

int Log2File(char *pFilePathName, char *pLog, char *pMod);

#endif // !defined(__GETDATAHTTPPOSTSOCKET_H__)
