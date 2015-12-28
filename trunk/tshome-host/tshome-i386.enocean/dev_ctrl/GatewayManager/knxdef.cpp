 //
//  knxdef.cpp
//  tsbus_knx
//
//  Created by kyy on 11-6-7.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#include "KNXCondef.h"

#include <string.h>

#include "knxdef.h"

#ifdef WIN32
#include <WINSOCK2.H>
#include <WS2TCPIP.H>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#endif

	int FillHeader(Header &head, int nServiceId, int nWholeLen)
	{
		head.size = KNX_VAL_HEADER_SIZE_10;
		head.version = KNX_VAL_VERSION10;
		head.serviceId = htons(nServiceId);
		head.totalLen = htons(nWholeLen);
		
		return 0;
	}
	
	int FillHPAI(HPAI &hpai, char *strIp, int nPort)
	{
		hpai.len = 0x08;
		hpai.protocol = PROTOCOL_UDP_IPV4;
		UINT uIp = 0;
		Ip2Uint(strIp, uIp);
		hpai.ip = htonl(uIp);
		hpai.port = htons(nPort);
		
		return 0;
	}
	
	int FillCRI(CRI &cri, int type, int layer)
	{
		cri.len = 0x04;
		cri.type = type;
		cri.layer = layer;
		cri.reserved = 0;
		
		return 0;
	}	
	
	int FillConHead(ConHead &conHead, int channelId, int counter, int nStatus)
	{
		conHead.len = 0x04;
		conHead.channelId = channelId;
		conHead.counter = counter;
		conHead.status = nStatus;
		
		return 0;
	}
	
	int GrpAddr2Word(int nMainAddr, int nMidAddr, int nGrpAddr, WORD &wAddr)
	{
		int nAddr = 0;
		if(nMidAddr == INVALID_MID_ADDR)
		{
			nAddr = ( (nMainAddr<<GRP_ADDR_BITS_2) & MAIN_ADDR_MASK_2) + 
					(nGrpAddr & GRP_ADDR_MASK_2);
		}
		else
		{
			nAddr = ( (nMainAddr<<(MID_ADDR_BITS_3 + GRP_ADDR_BITS_3)) & MAIN_ADDR_MASK_3) + 
					( (nMidAddr<<GRP_ADDR_BITS_3) & MID_ADDR_MASK_3) +
					(nGrpAddr & GRP_ADDR_MASK_3);
		}
		wAddr = (WORD)nAddr;
		
		return 0;
	}
	
	int Word2GrpAddr(WORD wAddr, int nSegements, int &nMainAddr, int &nMidAddr, int &nGrpAddr)
	{
		if(nSegements == PROJECT_SEGEMENTS_2)
		{
			nMainAddr = ( (wAddr & MAIN_ADDR_MASK_2) >> GRP_ADDR_BITS_2 );
			nMidAddr = INVALID_MID_ADDR;
			nGrpAddr = ( wAddr & GRP_ADDR_MASK_2 );
		}
		else
		{
			nMainAddr = ( (wAddr & MAIN_ADDR_MASK_3) >> (MID_ADDR_BITS_3 + GRP_ADDR_BITS_3) );
			nMidAddr = ( (wAddr & MID_ADDR_MASK_3) >> GRP_ADDR_BITS_3 );
			nGrpAddr = ( wAddr & GRP_ADDR_MASK_3 );
		}
		
		return 0;
	}
	
	int Ip2Uint(char *strIp, UINT &uIp)
	{
		char ip[32] = {'\0'};
		strcpy(ip, strIp);
		int nLen = strlen(ip);
		
		UINT uRes = 0;
		UINT uVal = 0;
		uIp = 0;
		int ipCount = 0;
		int index = 0;
		for(index = 0; ; index++)
		{
			if(ip[index] >= '0' && ip[index] <= '9')
			{
				uVal = uVal * 10 + (UINT) (ip[index] - '0');
			}
			else
			{
				uRes = (uRes<<8) + (uVal & 0xFF);
				ipCount++;
				if(ipCount >= 4 || index >= nLen)
					break;
				
				uVal = 0;
			}
		}
		
		uIp = uRes;
		
		if(ipCount != 4)
			return -1;
		
		return 0;
	}
