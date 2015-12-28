 //
//  knxdef.h
//  tsbus_knx
//
//  Created by kyy on 11-6-7.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//


#include "KNXWinVarType.h"

#pragma pack(1)

// values of knx frame
#define	KNX_VAL_HEADER_SIZE_10		0x06
#define	KNX_VAL_VERSION10			0x10
	
// service type
#define SERVICE_SEARCH_REQ			0x0201
#define SERVICE_SEARCH_RES			0x0202
#define SERVICE_CONNECT_REQ			0x0205
#define SERVICE_CONNECT_RES			0x0206
#define SERVICE_CONNECTSTATE_REQ	0x0207
#define SERVICE_CONNECTSTATE_RES	0x0208
#define SERVICE_DISCONNECT_REQ		0x0209
#define SERVICE_DISCONNECT_RES		0x020A
#define TUNNELLING_REQUEST			0x0420
#define TUNNELLING_ACK				0x0421

// connection type
#define CONNECT_TYPE_TUNNEL			0x04
	
// protocol
#define PROTOCOL_UDP_IPV4			0x01
#define PROTOCOL_TCP_IPV4			0x02
	
#define KNX_VAL_MCODE_LDATA_REQ		0x11
#define KNX_VAL_ADDIL_TP1			0x00
	
// values of control field 1 (CF1)
//	the higest bit -- the 8th bit
#define CF_76_EXTENDED_FRAME		0x00									//0000 0000b
#define CF_76_STANDARD_FRAME		0x80									//1000 0000b
// the 6th bit
#define CF_5_REPEAT					0x00									//0000 0000b
#define CF_5_NOREPEAT				0x20									//0010 0000b
// the 5th bit
#define CF_4_SYS_BROADCAST			0x00									//0000 0000b
#define CF_4_BROADCAST				0x10									//0001 0000b
// the 3rd & 4th bits
#define CF_32_PRIOR_SYSTEM			0x00									//0000 0000b
#define CF_32_PRIOR_NORMAL			0x04									//0000 0100b
#define CF_32_PRIOR_URGENT			0x08									//0000 1000b
#define CF_32_PRIOR_LOW				0x0c									//0000 1100b
// the 2th bit
#define CF_1_NOACK					0x00									//0000 0000b
#define CF_1_ACK					0x02									//0000 0010b
// the lowest -- the 1st bit
#define CF_0_NOERROR				0x00									//0000 0000b
#define CF_0_ERROR					0x01									//0000 0001b
	
// the values of the 2nd ctrl field
#define CF2_7_ADDRESS_INDIV			0x00									//0000 0000b
#define CF2_7_ADDRESS_GROUP			0x80									//1000 0000b
#define CF2_654_HOP_COUNT_6			0x60									//0110 0000b
#define CF2_3210_EFF_0				0x00									//0000 0000b
	
	
// the high 6 bits value of the TPCI octet.
// It is T_Data_Group-PDU when destination address != 0.
// else it is T_Data_Broadcast-PDU
#define TCPI_T_DATA_GROUP			0x00									//0000 0000b
	
// the high 2 bits value of APCI
// It is A_GroupValue_Read-PDU, A_GroupValue_Response-PDU
// and A_GroupValue_Write-PDU individually
// when the lowest 2 bits of TCPI is 00b.
#define APCI_GROUP_VALUE_READ		0x00									//0000 0000b
#define APCI_GROUP_VALUE_RES		0x40									//0100 0000b
#define APCI_GROUP_VALUE_WRITE		0x80									//1000 0000b

#define L_DATA_REQ					0x11
#define L_DATA_CON					0x2E
#define L_DATA_IND					0x29
	
#define TIME_OUT_2					-2
	
	
// about the 2 octets: group address
// 3 segements:
#define MAIN_ADDR_BITS_3			4
#define MID_ADDR_BITS_3				3
#define GRP_ADDR_BITS_3				8
#define MAIN_ADDR_MASK_3			0x7800					// 0111 1000 0000 0000
#define MID_ADDR_MASK_3				0x0700					// 0000 0111 0000 0000
#define GRP_ADDR_MASK_3				0x00FF					// 0000 0000 1111 1111
// 2 segements:
#define MAIN_ADDR_BITS_2			4
#define GRP_ADDR_BITS_2				11
#define MAIN_ADDR_MASK_2			0x7800					// 0111 1000 0000 0000
#define GRP_ADDR_MASK_2				0x07FF					// 0000 0111 1111 1111
	
// multicast constant
#define GROUP_IP					"224.0.23.12"
#define GROUP_PORT					3671

// communication sockets
#define SOCKET_MIN					2000
#define SOCKET_MAX					60000
#define SOCKET_SCAPE				2000
	
	// frame header
	typedef struct _Header
	{
		BYTE	size;
		BYTE	version;
		WORD	serviceId;
		WORD	totalLen;
	}Header;
	
	// host protocol address information
	typedef struct _HPAI
	{
		BYTE	len;
		BYTE	protocol;
		UINT	ip;
		WORD	port;
	}HPAI;
	
	// connection request information
	typedef struct _CRI
	{
		BYTE	len;
		BYTE	type;
		BYTE	layer;
		BYTE	reserved;
	}CRI;
	
	// connection response data block
	typedef struct _CRD
	{
		BYTE	len;
		BYTE	type;
		WORD	indivAddr;
	}CRD;
	
	// description information block: hardware
	typedef struct _DIB_HD
	{
		BYTE	len;
		BYTE	desType;
		BYTE	medium;
		BYTE	deviceStatus;
		WORD	indivAddr;
		WORD	installId;
		BYTE	serialNum[6];
		UINT	ip;
		BYTE	macAddr[6];
		char	name[30];
	}DIB_HD;
	
	// description information block: supported service family
	typedef struct _DIB_SSF
	{
		// variable length
		// like this:
//		BYTE	len;
//		BYTE	desType;
//		BYTE	familyId1;
//		BYTE	version1;
//		BYTE	familyId2;
//		BYTE	version2;
//		......
//		BYTE	familyId(len-2)/2;
//		BYTE	version(len-2)/2;
	}DIB_SSF;
	
	// connection header
	typedef struct _ConHead
	{
		BYTE	len;
		BYTE	channelId;
		BYTE	counter;
		BYTE	status;							//it is reserved in request message
	}ConHead;
	
	// cemi frame
	typedef struct _Cemi_Frame
	{
		//variable length
		//like this:
//		BYTE	mCode;
//		BYTE	addIL;
//		
//		...length addIL start....
//		additional information length
//		...length addIL length...
//		
//		service information: variable length
	}Cemi_Frame;
	
	
	
	// sesearch request
	typedef struct _SearchReq
	{
		Header	head;
		HPAI	hpaiCtrlL;						//ctrl endpoint of local
	}SearchReq;
	
	// sesearch response
	typedef struct _SearchRes
	{
		Header	head;
		HPAI	hpaiCtrlR;						//ctrl endpoint of remote
		DIB_HD	dib_hd;
		
		// DIB_SSF: variable length
		
	}SearchRes;
	
	
	// tunnel connection request
	typedef struct _ConnectReq
	{
		Header	head;
		HPAI	hpaiCtrlL;						//ctrl endpoint of local
		HPAI	hpaiDataL;						//data endpoint of local
		CRI		cri;
	}ConnectReq;
	
	// tunnel connection response
	typedef struct _ConnectRes
	{
		Header	head;
		BYTE	channelId;
		BYTE	status;
		HPAI	hpaiDataR;						//data endpoint of remote
		CRD		crd;
	}ConnectRes;
	
	
	// tunnel connection state request
	// tunnel disconnection request
	typedef struct _ConnectStateReq
	{
		Header	head;
		BYTE	channelId;
		BYTE	reserved;
		HPAI	hpaiCtrlL;						//ctrl endpoint of local
	}ConnectStateReq, DisConnectReq;
	
	// tunnel connection state response
	typedef struct _ConnectStateRes
	{
		Header	head;
		BYTE	channelId;
		BYTE	status;
	}ConnectStateRes, DisConnectRes;
	
	
	// tunnel request
	typedef struct _TunnelReq
	{
		Header	head;
		ConHead	conHead;
		//Cemi_Frame: variable length
	}TunnelReq;
	
	// tunnel response
	typedef struct _TunnelAck
	{
		Header	head;
		ConHead	conHead;
	}TunnelAck;
	
	
	// functions
	int Ip2Uint(char *strIp, UINT &ip);
	int FillHeader(Header &head, int nServiceId, int nWholeLen);
	int FillHPAI(HPAI &hpai, char *strIp, int nPort);
	int FillCRI(CRI &cri, int type, int layer);
	int FillConHead(ConHead &conHead, int channelId, int counter, int nStatus = 0);
	
	int GrpAddr2Word(int nMainAddr, int nMidAddr, int nGrpAddr, WORD &wAddr);	
	int Word2GrpAddr(WORD wAddr, int nSegements, int &nMainAddr, int &nMidAddr, int &nGrpAddr);
	
	

#pragma pack()
