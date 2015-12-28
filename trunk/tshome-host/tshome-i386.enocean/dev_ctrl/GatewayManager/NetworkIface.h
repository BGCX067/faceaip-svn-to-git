//
//  NetworkIface.h
//  tsbus_knx
//
//  Created by kyy on 11-5-23.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#include "KNXWinVarType.h"
#include "KNXCondef.h"


#ifdef _WIN32
#else
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#endif


#ifndef _NETWORKIFACE_H
#define _NETWORKIFACE_H

#ifdef _WIN32
	int _GetLocalIp(char *ip);
#else
	void _GetLocalIp(char *ip);
#endif

class DECLSPEC_DLL_EXPORT NetworkIface
{
	public:
	virtual ~NetworkIface() {};
	virtual int ExecRecvMsgAction(BYTE *pRecvBuf, int nLen) = 0;
	virtual void ExecDisconnectAction() = 0;

	virtual void SetIfaceType(int nIfaceType) { m_nIfaceType = nIfaceType; }
	virtual int GetIfaceType() { return m_nIfaceType; }
	
	int GetGatewayKey() { return m_nGatewayKey; }
	protected:
		int m_nIfaceType;
		// Gateway Key
		int m_nGatewayKey;
};

#endif
