#ifndef CWIFIKERNEL_H
#define CWIFIKERNEL_H

#include "ICIface.h"
#include "ListenSocket.h"
#include "GeneralAgent/GeneralAgentHandler.h"
#include "GeneralAgent/GeneralAgentTcpSocketServer.h"
#include "GeneralAgent/GeneralAgentTcpSocketConnector.h"
#include "common/remote_def.h"
#include "common/ts_err_code.h"
#include "KNXCondef.h"

int WifiSendThread(void *pParam);

class CWifiKernel : public CICIface
{
public:
	CWifiKernel();
	~CWifiKernel();

	void SetCallbacks(
			OnWifiConnect onWifiConn,
			OnWifiDisconnect onWifiDisconn,
			OnCtrlResponse onCtrlResponse);

	OnWifiConnect GetWifiOnConnect();
	OnWifiDisconnect GetWifiOnDisconnect();
	OnCtrlResponse GetOnCtrlResponse();

	void OnWifiConnected(int nErrorCode);
	void OnWifiDisconnected();
	void OnRecvMessage(void * pBuf, int nLen);

	int AddRequestNode(int nType, void *pBuf, int nLen);
	int Send(int nType, void *pBuf, int nLen);

public:
	unsigned int nIndex;
    unsigned short IsUse;
    unsigned short Port;
    char strIP[32];

    CGeneralAgentTcpSocketConnector *pConnector;
protected:
    char m_recvBuf[REPORT_MAX_LEN];
    int m_nCurMsgLen;

    OnWifiConnect m_onWifiConn;
    OnWifiDisconnect m_onWifiDisconn;
    OnCtrlResponse m_onCtrlResponse;
};

#endif
