/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerial.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerial.h 5884 2013-07-27 11:45:18Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-27 11:45:18 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _TeleSerial_H
#define _TeleSerial_H

#include <time.h>
#include <vector>
#include "TeleSerialDef.h" /* default def */

#include "ListenSocket.h"

#include "EZThread.h"
#include "EZTimer.h"

// 反应堆
#include "TeleSerialHandler.h"
// tcp server
#ifdef TeleSerial_FUNC_TCPSOCKETSERVER
#include "TeleSerialTcpSocketServer.h"
#endif
#ifdef TeleSerial_FUNC_HTTPSERVER
#include "TeleSerialHttpServer.h"
#endif
#ifdef TeleSerial_FUNC_UDPSOCKETSERVER
#include "TeleSerialUdpSocket.h"
#endif
// tcp client, auto conn
#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR
#include "TeleSerialTcpSocketConnector.h"
#endif
// http client, auto conn when brocken
#ifdef TeleSerial_FUNC_HTTPKEEPCONNECTOR
#include "TeleSerialHttpKeepConnector.h"
#endif

#ifdef TeleSerial_FUNC_STATUS_AGENT
#include "TeleSerialStatusAgent.h"
#endif
#ifdef TeleSerial_FUNC_CONSOLE_OVERTCP
#include "TeleSerialConsoleOverTcp.h"
#endif

typedef struct __generalservercfg_t
{
    unsigned short IsUse;
    unsigned short Port;
    std::string strHost;
    // will find by name
    std::string strServerName;

    int iConnTimeOut;//超时时间
    int iConnTimes; //连接尝试次数（保留）

    int iInterval; //[in] 重连间隔，单位毫秒，参数默认值为30秒
    int bEnableRecon ;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

    std::string strUName;
    std::string strUPwd;

    __generalservercfg_t()
    {
        IsUse = 0;
        Port = 0;
        strHost = "";
        // will find by name
        strServerName="";

        iConnTimeOut = 0;//超时时间
        iConnTimes = 0; //连接尝试次数（保留）

        iInterval = 0; //[in] 重连间隔，单位毫秒，参数默认值为30秒
        bEnableRecon  = 0;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

        strUName = "";
        strUPwd = "";
    }
}GENERALSERVERCFG_T;


typedef struct __GeneralAgentCfg
{
    // TCP服务端口
#ifdef TeleSerial_FUNC_TCPSOCKETSERVER
    unsigned short PortTcpServer; // 如果不启用，则设置为0
#endif
#ifdef TeleSerial_FUNC_HTTPSERVER
    unsigned short PortHttpServer; // 如果不启用，则设置为0
#endif
    // UDP服务端口
#ifdef TeleSerial_FUNC_UDPSOCKETSERVER
    unsigned short UdpServerPort; // 如果不启用，则设置为0
#endif
    // 状态服务端口
#ifdef TeleSerial_FUNC_STATUS_AGENT
    unsigned short StatusServicePort; // 如果不启用，则设置为0
#endif
    //调试命令行接口， 0无效
#ifdef TeleSerial_FUNC_CONSOLE_OVERTCP
    unsigned short ConsoleOverTcpPort; // 调试命令行接口， 0无效
#endif
#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR
    // tcp 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    std::vector<GENERALSERVERCFG_T> TcpHost;
#endif

#ifdef TeleSerial_FUNC_HTTPKEEPCONNECTOR
    // http 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    std::vector<GENERALSERVERCFG_T> HttpHost;
#endif

}
GENERALAGENTCFG_T;

// Socket *pSocket, void *pDat, int len
//typedef TSignal3<Socket *, void *, int> SIG_TeleSerialHandler_DATA;

#define g_TeleSerial (*CTeleSerial::instance())

class CTeleSerial : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CTeleSerial);
    CTeleSerial();
    virtual ~CTeleSerial();
    ////////
    void Initialize(int argc, char * argv[]);
	
	// 设置配置参数
    void SetConfig(GENERALAGENTCFG_T *pConfig=NULL);

#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
    BOOL Start(CEZObject * pObj=NULL, SIG_TeleSerialHandler_DATA::SigProc pProc=NULL);
    BOOL Stop(CEZObject * pObj=NULL, SIG_TeleSerialHandler_DATA::SigProc pProc=NULL);
#else
    BOOL Start();
    BOOL Stop();
#endif //TeleSerial_FUNC_SIGNAL_ONDATA
    void ThreadProc();

    // 设置服务，
#ifdef TeleSerial_FUNC_TCPSOCKETSERVER
// port =0 disable, others use it
    void SetTcpServer(unsigned short  port);
#endif
#ifdef TeleSerial_FUNC_HTTPSERVER
    void SetHttpServer(unsigned short  port);
#endif
#ifdef TeleSerial_FUNC_UDPSOCKETSERVER
    void SetUdpServer(unsigned short  port);
#endif

#ifdef TeleSerial_FUNC_TCPSOCKETCONNECTOR

    void AddTcpHost(std::string strHost, int nPort, std::string strName="");
#endif //TeleSerial_FUNC_TCPSOCKETCONNECTOR
#ifdef TeleSerial_FUNC_HTTPKEEPCONNECTOR

    void AddHttpHost(std::string strHost, int nPort, std::string strServerName=CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME);
#endif

    int ConnectorCreate();

    // 内部需要一定的时间自行处理
    // 为确保内部销毁完成，一般调用之后， 10s之后再调用Connect，
    int ConnectorDestroy();

    void Run();

    int Send2Socket(std::string strSocketName, const char *pData, size_t Len);
    Socket * GetSocketByName(std::string strServerName);


    void TimerProcMaintain();
    const std::string &GetVersion();
    unsigned int GetRunPeriod();
    time_t GetStartTime();

#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
    BOOL OnData(Socket *pSocket, const char *pdat, size_t len);
#endif //TeleSerial_FUNC_SIGNAL_ONDATA
    BOOL OnSerial(int id, const char *pdat, size_t len);

private:
    GENERALAGENTCFG_T m_ConfigGeneralAgent;
    CTeleSerialLog m_Log;
    CTeleSerialHandler *m_pHandlerGeneralAgent;

#ifdef TeleSerial_FUNC_STATUS_AGENT

    ListenSocket<CTeleSerialStatusAgent> *m_pSocketStatusAgent;
#endif // TeleSerial_FUNC_STATUS_AGENT

#ifdef TeleSerial_FUNC_CONSOLE_OVERTCP

    ListenSocket<CTeleSerialConsoleOverTcp> *m_pGeneralConsoleOverTcp;
#endif // TeleSerial_FUNC_CONSOLE_OVERTCP

#ifdef TeleSerial_FUNC_TCPSOCKETSERVER

    ListenSocket<CTeleSerialTcpSocketServer> *m_pGeneralAgentTcpSocketServer;
#endif // TeleSerial_FUNC_TCPSOCKETSERVER
#ifdef TeleSerial_FUNC_HTTPSERVER

    ListenSocket<CTeleSerialHttpServer> *m_pGeneralAgentHttpServer;
#endif // TeleSerial_FUNC_HTTPSERVER


#ifdef TeleSerial_FUNC_UDPSOCKETSERVER
	CTeleSerialUdpSocket *m_pGeneralAgentUdpSocketServer;
#endif

    int InitializeLogs();
#ifdef ATTATCH_FUNC_OF_GENERAL_AGENT
    CEZTimer          m_TimerMaintain;          //保活功能定时器
    // 启动时间
    unsigned int m_iRunPeriod;
    time_t m_ttStartTime;
#endif //#ifdef ATTATCH_FUNC_OF_GENERAL_AGENT

#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
    CEZMutex m_MutexSig;
    SIG_TeleSerialHandler_DATA m_sigData;
#endif //TeleSerial_FUNC_SIGNAL_ONDATA
}
; //CTeleSerial

#endif //_TeleSerial_H
