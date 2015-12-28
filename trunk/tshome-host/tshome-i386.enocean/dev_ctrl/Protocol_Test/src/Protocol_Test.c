/*
 ============================================================================
 Name        : Protocol_Test.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

#include "dev_ctrl/Conversion.h"
#include "common/remote_def.h"
#include "common/common_define.h"

#include <errno.h>
#include <string.h>
#include <netinet/in.h>





void CB_OnCtrlResponse(int nType, void *buf, int nLen)
{
	if(nType == ACTION_KNX_EXTRA_RES)
	{
		knx_control_report_t *pKnxRes = (knx_control_report_t *) buf;


		knx_control_report_t *pTest = (knx_control_report_t *) buf;
		printf("CB_OnCtrlResponse in Protocol_Test.c [bufLen = %d]\n", nLen);
		printf("ACTION_KNX_EXTRA_RES(start=%d, protocol_type=%d, function_code=%d, comm_type=%d, comm_source=%d)\n",
				pTest->kic_head.header.start,
				pTest->kic_head.header.protocol_type,
				pTest->kic_head.header.function_code,
				pTest->kic_head.header.comm_type,
				pTest->kic_head.header.comm_source);

		printf("ACTION_KNX_EXTRA_RES(nBodyLen=%d, session_id=%d, device_type=%d, home_device_id=%d, cmd_type=%d, address=%d, data=%d, result_code=%d, data_len=%d, data_type=%d)\n",
				ntohs(pTest->kic_head.header.length),
				ntohl(pTest->kic_head.session_id),
				pTest->kic_head.device_type,
				ntohl(pTest->kic_head.home_device_id),
				pTest->cmd_type,
				ntohs(pTest->address),
				ntohl(*((int *)pTest->data)),
				pTest->result_code,
				pTest->data_len,
				pTest->data_type);

		int nGrpAddr = ntohs(pKnxRes->address);
		printf("ACTION_KNX_EXTRA_RES [%d]: (%d, %d, %d)\n",
				pKnxRes->result_code,
				(nGrpAddr >> 11) & 0x0F,
				(nGrpAddr >> 8) & 0x07,
				nGrpAddr & 0xFF);
	}
	else if(nType == ACTION_KNX_READ_RES)
	{
		knx_control_report_t *pKnxRes = (knx_control_report_t *) buf;

		int nGrpAddr = ntohs(pKnxRes->address);
		printf("ACTION_KNX_READ_RES [%d]: (%d, %d, %d : %d)\n",
				pKnxRes->result_code,
				(nGrpAddr >> 11) & 0x0F,
				(nGrpAddr >> 8) & 0x07,
				nGrpAddr & 0xFF,
				ntohl(*((int *)pKnxRes->data)) );
	}
	else if(nType == IC_DIRECT_IRSTUDY_RES)
	{
		ir_study_res_t *pIrStudy = (ir_study_res_t *) buf;

		printf("IRSTUDY_RES [type=%d, %d]: (gatewayId=%d, fun_code=%d, channel=%d)\n",
				nType,
				pIrStudy->res_code,
				ntohl(pIrStudy->gateway_id),
				ntohs(pIrStudy->function_code),
				pIrStudy->channel);
	}
	else
	{
		com_control_res_t *pComRes = (com_control_res_t *) buf;

		printf("ACTION_WIFI_RES [type = %d, %d]: (%d)\n",
				nType,
				pComRes->res_code,
				pComRes->com_ctrl_type);
	}
}

void CB_OnSearchResponse(int nGatewayIndex, char *serverIp, int serverPort, int nStatus)
{
	printf("SEARCH_RES: [%d, %s:%d]\n", nGatewayIndex, serverIp, serverPort);
}

void CB_OnReadResponse(int nGatewayIndex, KNX_GroupData grpData)
{
	double dVal = 0;
	int nMidAddr = (grpData.nGroupAddr >> 8) & 0x07;
	if( nMidAddr != 2 && (grpData.nGroupAddr & 0xFF) < 20 )
	{
		if(grpData.Data.BinData)
		{
			dVal = 1;
		}
		else
		{
			dVal = 0;
		}
	}
	else
	{
		dVal = grpData.Data.AnaData;
	}

	printf("READ_RES: [%d, (%d, %d, %d), %g)]\n",
		nGatewayIndex,
		(grpData.nGroupAddr >> 11) & 0x0F,
		(grpData.nGroupAddr >> 8) & 0x07,
		grpData.nGroupAddr & 0xFF,
		dVal);
}

void CB_OnWriteResponse(int nGatewayIndex, KNX_GroupData grpData)
{
	double dVal = 0;
	int nMidAddr = (grpData.nGroupAddr >> 8) & 0x07;
	if( nMidAddr != 2 && (grpData.nGroupAddr & 0xFF) < 20 )
	{
		if(grpData.Data.BinData)
		{
			dVal = 1;
		}
		else
		{
			dVal = 0;
		}
	}
	else
	{
		dVal = grpData.Data.AnaData;
	}

	printf("WRITE_RES: [%d, (%d, %d, %d), %g)]\n",
		nGatewayIndex,
		(grpData.nGroupAddr >> 11) & 0x0F,
		(grpData.nGroupAddr >> 8) & 0x07,
		grpData.nGroupAddr & 0xFF,
		dVal);
}

void CB_OnKnxConnect(int nGatewayIndex, int nSuccess, int expId, char *strErr)
{
	if(expId == 0)
	{
		printf("KNX_CONNECT:[%d, ok]\n",
				nGatewayIndex);
	}
	else
	{
		printf("KNX_CONNECT:[%d, failed, %d, %s]\n",
				nGatewayIndex,
				expId,
				strErr);
	}
}

void CB_OnKnxDisconnect(int nGatewayIndex)
{
	printf("KNX_Disconnect:[%d]\n", nGatewayIndex);
}

void CB_OnRWError(int nGatewayIndex, int exceptionId, char *exceptionStr, int read_write)
{
	printf("RW_ERROR: [%d, %d, %s, %s]\n",
			nGatewayIndex,
			exceptionId,
			exceptionStr,
			read_write == ACTION_KNX_WRITE ? "Write" : "Read");
}




int main(void)
{
	void *handle = dlopen(GATEWAY_SO_PATH, RTLD_LAZY);
	if(handle == NULL)
	{
		printf("[%s]\n", dlerror());
		return -1;
	}
	else
	{
		printf("libGatewayManager.so Opened OK\n");
	}

	int nRes = -1;



// dynamic library
// -----------------------------------------------------------------------------------------------
	typedef void (*set_knx_callbacks) (OnSearchResponse, OnReadResponse,
			OnWriteResponse, OnKnxConnect, OnKnxDisconnect, OnRWError);

	set_knx_callbacks set_knx_backs = (set_knx_callbacks) dlsym(handle, "protocol_set_knx_callbacks");

	set_knx_backs(CB_OnSearchResponse, CB_OnReadResponse,
			CB_OnWriteResponse, CB_OnKnxConnect, CB_OnKnxDisconnect, CB_OnRWError);

	typedef void (* set_ctrl_response) (OnCtrlResponse ctrl_response);
	set_ctrl_response setCtrlResponse = dlsym(handle, "protocol_set_ctrl_response");
	setCtrlResponse(CB_OnCtrlResponse);

	typedef int (*INT_FUN_VOID)();
	INT_FUN_VOID startManager = (INT_FUN_VOID) dlsym(handle, "protocol_start_manager");
	nRes = startManager();

	// Connect all knx gateways
	INT_FUN_VOID connectAll = (INT_FUN_VOID) dlsym(handle, "protocol_connect_all");
	nRes = connectAll();

	// Search KNX Gateways
	typedef int (*INT_FUN_CHAR)();
	INT_FUN_CHAR knx_search = (INT_FUN_CHAR) dlsym(handle, "protocol_search_server");
	char searchErr[64] = {'\0'};
	nRes = knx_search(searchErr);
	if(nRes != 0)
	{
		printf("SEARCH_FAILED: [%s]\n", searchErr);
	}

	typedef int (*INT_FUN_VOIDP_INT)(void*, int);
	INT_FUN_VOIDP_INT request = (INT_FUN_VOIDP_INT) dlsym(handle, "protocol_request");




	// my test packet
	int nBodyLen = 0;

	int nAppPackHeaderLen = sizeof(remote_header_t);

	int nKnxReqLen = sizeof(knx_control_report_t);
	knx_control_report_t *pKnxReq = (knx_control_report_t *) malloc(nKnxReqLen);
	memset(pKnxReq, 0, nKnxReqLen);

	int nComReqLen = sizeof(com_control_req_t);
	com_control_req_t *pComReq = (com_control_req_t *) malloc(nComReqLen);
	memset(pComReq, 0, nComReqLen);

	int nIrStudyReqLen = sizeof(ir_study_req_t);
	ir_study_req_t *pIrStudyReq = (ir_study_req_t *) malloc(nIrStudyReqLen);
	memset(pIrStudyReq, 0, nIrStudyReqLen);

	int nIrCtrlReqLen = sizeof(wifi_control_req_t);
	wifi_control_req_t *pIrCtrlReq = (wifi_control_req_t *) malloc(nIrCtrlReqLen);
	memset(pIrCtrlReq, 0, nIrCtrlReqLen);

	int nSceneReqLen = sizeof(scene_control_report_t);
	scene_control_report_t *pSceneReq = (scene_control_report_t *) malloc(nSceneReqLen);
	memset(pSceneReq, 0, nSceneReqLen);

	kic_head_t kic_head;
	kic_head.header.start = HEAD_START;
	kic_head.header.protocol_type = 0;
	kic_head.header.length = htons(0);
	kic_head.header.serial_number = 0;
	kic_head.header.function_code = DEVICE_OPERATION;
	kic_head.header.comm_type = REQUEST;
	kic_head.header.comm_source = HOST_S;

	kic_head.session_id = 0;
	kic_head.device_type = 0;
	kic_head.home_device_id = htonl(0);
	
	
	
	int num = -1;
	int nParam1 = 0;
	int nParam2 = 0;
	int nParam3 = 0;
	printf("input 1 num(1-KnxWriteSwitch, 2-KnxRead, 3-ComCtrl, 4-CusComCtrl, 5-irStudy, 6-irCtrl, 7-sceneCtrl):\n");
	while(1!= scanf("%d", &num));
	while(num == 1 || num == 2 ||num == 3 ||num == 4 ||num == 5 ||num == 6 ||num == 7)
	{
		printf("--------------before switch----scanf num(%d)\n", num);

		switch(num)
		{
		case 1:
			nBodyLen = nKnxReqLen - nAppPackHeaderLen;
			kic_head.header.length = htons(nBodyLen);

			kic_head.device_type = KNX;

			printf("input 3 numbers for (dev_id, group_addr, data):\n");
			while(3 != scanf("%d %d %d", &nParam1, &nParam2, &nParam3));

			kic_head.home_device_id = htonl(nParam1);
			memcpy(pKnxReq, &kic_head, sizeof(kic_head));

			pKnxReq->cmd_type = WRITE;
			pKnxReq->address = htons(nParam2);

			pKnxReq->data_len = 4;
			pKnxReq->data_type = DPT_Boolean;
			if(nParam2 == 2581)
			{
				pKnxReq->data_type = DPT_8BitUnSign;
			}
			*((int *) pKnxReq->data) = htonl(nParam3);

			nRes = request(pKnxReq, nKnxReqLen);
			printf("...knx ctrl...nRes = %d\n", nRes);
			break;
		case 2:
			nBodyLen = nKnxReqLen - nAppPackHeaderLen;
			kic_head.header.length = htons(nBodyLen);

			kic_head.device_type = KNX;

			printf("input 2 numbers for (dev_id, group_addr):\n");
			while(2 != scanf("%d %d", &nParam1, &nParam2));

			kic_head.home_device_id = htonl(nParam1);
			memcpy(pKnxReq, &kic_head, sizeof(kic_head));

			pKnxReq->cmd_type = READ;
			pKnxReq->address = htons(nParam2);
			memset((char *) pKnxReq->data, 0, MAX_DATA_AREASIZE);
			pKnxReq->data_len = 4;
			pKnxReq->data_type = DPT_Boolean;

			nRes = request(pKnxReq, nKnxReqLen);
			printf("...knx ctrl...nRes = %d\n", nRes);
			break;
		case 3:
			nBodyLen = nComReqLen - nAppPackHeaderLen;
			kic_head.header.length = htons(nBodyLen);

			kic_head.device_type = COM;

			printf("input 3 numbers for (dev_id, ctrl_type, data):\n");
			while(3 != scanf("%d %d %d", &nParam1, &nParam2, &nParam3));

			kic_head.home_device_id = htonl(nParam1);
			memcpy(pComReq, &kic_head, sizeof(kic_head));

			pComReq->com_ctrl_type = nParam2;
			pComReq->data = htonl(nParam3);

			nRes = request(pComReq, nComReqLen);
			printf("...com ctrl...nRes = %d\n", nRes);
			break;
		case 4:
			nBodyLen = nComReqLen - nAppPackHeaderLen;
			kic_head.header.length = htons(nBodyLen);

			kic_head.device_type = CUSTOMER_COM;

			printf("input 2 numbers for (dev_id, key_id):\n");
			while(2 != scanf("%d %d", &nParam1, &nParam2));

			kic_head.home_device_id = htonl(nParam1);
			memcpy(pComReq, &kic_head, sizeof(kic_head));

			pComReq->com_ctrl_type = 0;
			pComReq->data = htonl(nParam2);

			nRes = request(pComReq, nComReqLen);
			printf("...customer com ctrl...nRes = %d\n", nRes);
			break;
		case 5:
			nBodyLen = nIrStudyReqLen - nAppPackHeaderLen;
			kic_head.header.length = htons(nBodyLen);

			kic_head.home_device_id = htonl(0);
			kic_head.device_type = IR_STUDY;

			printf("input 2 numbers for (gateway_id, key_id):\n");
			while(2 != scanf("%d %d", &nParam1, &nParam2));

			memcpy(pIrStudyReq, &kic_head, sizeof(kic_head));

			pIrStudyReq->gateway_id = htonl(nParam1);
			pIrStudyReq->channel = 1;
			pIrStudyReq->function_code = htons(nParam2);

			nRes = request(pIrStudyReq, nIrStudyReqLen);
			printf("...ir study...nRes = %d\n", nRes);
			break;
		case 6:
			nBodyLen = nIrCtrlReqLen - nAppPackHeaderLen;
			kic_head.header.length = htons(nBodyLen);

			kic_head.device_type = INFRARED;

			printf("input 2 numbers for (dev_id, key_id):\n");
			while(2 != scanf("%d %d", &nParam1, &nParam2));

			kic_head.home_device_id = htonl(nParam1);
			memcpy(pIrCtrlReq, &kic_head, sizeof(kic_head));

			pIrCtrlReq->function_code = htons(nParam2);

			nRes = request(pIrCtrlReq, nIrCtrlReqLen);
			printf("...ir ctrl...nRes = %d\n", nRes);
			break;
		case 7:
			nBodyLen = nSceneReqLen - nAppPackHeaderLen;
			kic_head.header.length = htons(nBodyLen);

			kic_head.device_type = SCENE;

			printf("input 1 numbers for (scene_id):\n");
			while(1 != scanf("%d", &nParam1));

			kic_head.home_device_id = htonl(0);
			memcpy(pSceneReq, &kic_head, sizeof(kic_head));

			pSceneReq->scene = htonl(nParam1);

			nRes = request(pSceneReq, nSceneReqLen);
			printf("...scene ctrl...nRes = %d\n", nRes);
			break;
		}
		printf("input 1 num(1-KnxWriteSwitch, 2-KnxRead, 3-ComCtrl, 4-CusComCtrl, 5-irStudy, 6-irCtrl, 7-sceneCtrl):\n");
		//nRes = scanf("%d", &num);
		while((scanf("%d", &num)) != 1);
		printf("-------------after switch-----scanf num(%d), errno(%d)\n", num, errno);
	}

	free(pKnxReq);
	free(pComReq);
	free(pIrStudyReq);
	free(pIrCtrlReq);
// -----------------------------------------------------------------------------------------------


//	static library
//	protocol_set_knx_callbacks(CB_OnSearchResponse, CB_OnReadResponse,
//			CB_OnWriteResponse, CB_OnKnxConnect, CB_OnKnxDisconnect, CB_OnRWError);
//	printf("nRes = %d\n", nRes);

	typedef void (*StopManager)();
	StopManager stopManager = (StopManager) dlsym(handle, "protocol_stop_manager");

	stopManager();

	if(handle != NULL)
	{
		nRes = dlclose(handle);
		if(nRes == 0)
		{
			printf("libGatewayManager.so closed\n");
		}
	}

	return SUCCESS;
}
