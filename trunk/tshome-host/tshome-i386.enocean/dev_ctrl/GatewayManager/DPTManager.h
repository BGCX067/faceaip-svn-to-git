//
//  DPTManager.h
//  tsbus_knx
//
//  Created by kyy on 11-5-20.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//
#include "common/remote_def.h"


	class CDPTManager
	{
	public:
		// Get the variable type(one of the 5) from the 17 ones.
		KNX_DATA_TYPE GetVarTypeFromDPT(int nDPT);
		
		// Translate the varible data into knx formate and fill it into the buffer.
		int DPT2Byte(KNX_GroupData grpData, BYTE * pBuf, int &nLen);
		
		// Get the knx data from buffer, translate it into variable formate, and save it into grpData.
		int Byte2DPT(BYTE *pBuf, int nBufSize, KNX_GroupData &grpData);	
		
		// Get the byte size of data area in the grpData.
		int GetDataAreaSize(KNX_GroupData grpData);
	};
