//
//  UDPUnicast.h
//  tsbus_knx
//
//  Created by kyy on 11-6-3.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#include "UDPManager.h"

class DECLSPEC_DLL_EXPORT CUDPUnicast : public CUDPManager
{
public:
	virtual int InitServer(char * strServerIp, int nServerPort);
	virtual int StartRecvThread();
};