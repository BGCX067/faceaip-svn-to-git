 //
//  KNXUdpIface.h
//  tsbus_knx
//
//  Created by kyy on 11-5-23.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#include "KNXWinVarType.h"

#ifndef _KNXUDPIFACE_H
#define _KNXUDPIFACE_H


	class KNXUdpIface
	{
	public:
		virtual int ExecRecvMsgAction(BYTE *pRecvBuf, int nLen) = 0;
		virtual ~KNXUdpIface() {}
	};

#endif
