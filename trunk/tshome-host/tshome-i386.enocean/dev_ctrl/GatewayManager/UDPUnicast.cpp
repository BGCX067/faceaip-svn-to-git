//
//  UDPUnicast.cpp
//  tsbus_knx
//
//  Created by kyy on 11-6-3.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#include "KNXException.h"
#include "UDPUnicast.h"

#include <string.h>

int CUDPUnicast::InitServer(char * strServerIp, int nServerPort)
{
	if(strServerIp == NULL || !(nServerPort > 0 && nServerPort < 65536))
	{
		throw(new CKNXInputException);
	}
	
	strncpy(m_strServerIp, strServerIp, MAX_IP_LEN);
	m_nServerPort = nServerPort;
	
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(nServerPort);
	m_serverAddr.sin_addr.s_addr = inet_addr(strServerIp);	
	
	return 0;
}

int CUDPUnicast::StartRecvThread()
{
	m_bRunProcess = true;
	m_hRecvThread = CREATE_THREAD(RecvProcess, this);
	if(m_hRecvThread <= 0)
		throw(new CKNXUniThreadException());
	
	return 0;
}

