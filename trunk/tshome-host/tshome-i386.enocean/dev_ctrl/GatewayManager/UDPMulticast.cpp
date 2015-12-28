   //
//  UDPMulticast.cpp
//  tsbus_knx
//
//  Created by kyy on 11-6-3.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#include "KNXException.h"
#include <string.h>

#include "UDPMulticast.h"

int CUDPMulticast::InitServer(char * strServerIp, int nServerPort)
{
	if(strServerIp == NULL || !(nServerPort > 0 && nServerPort < 65536))
	{
		throw(new CKNXInputException);
	}
	
	strncpy(m_strServerIp, strServerIp, MAX_IP_LEN);
	m_nServerPort = nServerPort;

	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_addr.s_addr = inet_addr(m_strServerIp);	
	m_serverAddr.sin_port = htons(nServerPort);

	//multicast
	memset(&m_mreq, 0, sizeof(m_mreq));
	m_mreq.imr_multiaddr.s_addr = inet_addr(strServerIp);
	m_mreq.imr_interface.s_addr = INADDR_ANY;//htonl(INADDR_ANY);
	int nRes = setsockopt(m_nClientSocket, IPPROTO_IP, IP_MULTICAST_LOOP /*IP_ADD_MEMBERSHIP*/, (const char *)&m_mreq, sizeof(m_mreq));
	if( nRes != 0)
	{
		throw(new CKNXMultiInitException());
	}

	return 0;
}

int CUDPMulticast::StartRecvThread()
{
	m_bRunProcess = true;
	m_hRecvThread = CREATE_THREAD(RecvProcess, this);
	if(m_hRecvThread <= 0)
	{
		throw(new CKNXMultiThreadException());
	}
		
	return 0;
}
