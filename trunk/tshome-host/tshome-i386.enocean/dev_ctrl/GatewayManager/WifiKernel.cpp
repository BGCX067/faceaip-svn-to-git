#include "WifiKernel.h"
#include "GatewayManager.h"

CWifiKernel::CWifiKernel()
{
	nIndex = 0;
	IsUse = 0;
	Port = 0;
	memset(strIP, 0, 32);
	pConnector = NULL;

	m_onWifiConn = NULL;
	m_onWifiDisconn = NULL;
	m_onCtrlResponse = NULL;

	m_nCurMsgLen = 0;

	m_pIcLogical->SetICIface(this);
}

CWifiKernel::~CWifiKernel()
{
}


int CWifiKernel::AddRequestNode(int nType, void *pBuf, int nLen)
{
	if(m_pIcLogical != NULL)
	{
		m_pIcLogical->AddRequestNode(nType, pBuf, nLen);
	}
	return SUCCESS;
}

int CWifiKernel::Send(int nType, void *pBuf, int nLen)
{
	CGatewayManager * pGatewayMan = CGatewayManager::Instance();

	if(pBuf == NULL)
	{
		return ERR_PROTROL_PARAMETERS;
	}

	if(nType == IC_WIFI_CLIENT)
	{
		if(pConnector != NULL)
		{
			pConnector->Send( (char *) pBuf, nLen);
			return SUCCESS;
		}
		else
		{
			return ERR_PROTROL_NO_CONNECT;
		}
	}
	else if(nType == IC_WIFI_SERVER)
	{
		pGatewayMan->g_TcpSocketHandler.SendToAllHost((char *) pBuf, nLen);
	}
	else if(nType == IC_DIRECT)
	{
		;
	}

	return ERR_PROTROL_NO_CONNECT;
}

void CWifiKernel::SetCallbacks(
		OnWifiConnect onWifiConn,
		OnWifiDisconnect onWifiDisconn,
		OnCtrlResponse onCtrlResponse)
{
	m_onWifiConn = onWifiConn;
	m_onWifiDisconn = onWifiDisconn;
	m_onCtrlResponse = onCtrlResponse;
}

OnWifiConnect CWifiKernel::GetWifiOnConnect()
{
	return m_onWifiConn;
}

OnWifiDisconnect CWifiKernel::GetWifiOnDisconnect()
{
	return m_onWifiDisconn;
}

OnCtrlResponse CWifiKernel::GetOnCtrlResponse()
{
	return m_onCtrlResponse;
}

void CWifiKernel::OnWifiConnected(int nErrorCode)
{
	if(m_onWifiConn != NULL)
	{
		m_onWifiConn(nIndex, nErrorCode);
	}
}
void CWifiKernel::OnWifiDisconnected()
{
	if(m_onWifiDisconn != NULL)
	{
		m_onWifiDisconn(nIndex);
	}
}

void CWifiKernel::OnRecvMessage(void * pBuf, int nLen)
{
}






