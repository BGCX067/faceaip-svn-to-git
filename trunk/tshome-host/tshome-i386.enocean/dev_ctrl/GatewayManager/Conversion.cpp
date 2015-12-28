#include "Conversion.h"
#include "enocean/esp.h"

#include "GatewayManager.h"
//#include "remote_def.h"
//#include "list.h"
#include "db/ts_db_protocol_infos.h"
#include "common/ts_err_code.h"

#include "common/remote_def.h"
#include "common/common_define.h"

#include "enocean/enocean_mgr.h"
#include "CommonInclude.h"


// inner function
int get_knx_dpt(int gateway_id, int group_address)
{
	int dpt = DPT_Unknown;

	int nRes = -1;

	// get dpt form db
	ts_knx_control_info_t knxData;
	knxData.group_addr = group_address;
	knxData.data_type = DPT_Unknown;
	nRes = db_get_knx_group_type(gateway_id, &knxData);
	if(nRes != EXIT_SUCCESS)
	{
		return DPT_Unknown;
	}

	dpt = knxData.data_type;

//	// just for test
//	if( ((group_address >> 8) & 0x07) < 2 )
//	{
//		dpt = DPT_Boolean;
//	}
//	else
//	{
//		dpt = DPT_8BitUnSign;
//	}

	if(dpt < DPT_Boolean || dpt > DPT_String)
	{
		dpt = DPT_Unknown;
	}

	return dpt;
}

void protocol_set_knx_callbacks(
		OnSearchResponse search_response,
		OnReadResponse knx_read_res,
		OnWriteResponse knx_write_res,
		OnKnxConnect knx_connect,
		OnKnxDisconnect knx_disconnect,
		OnRWError error_fun)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->SetKnxCallbacks(
		get_knx_dpt,
		search_response,
		knx_read_res,
		knx_write_res,
		knx_connect,
		knx_disconnect,
		error_fun);
}

void protocol_set_wifi_callbacks(
			OnWifiConnect conn_fun,
			OnWifiDisconnect disconn_fun)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->SetWifiCallbacks(conn_fun, disconn_fun);
}

void protocol_set_ctrl_response(OnCtrlResponse ctrl_response)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->SetCtrlResponse(ctrl_response);
}

// inner function
// Init all KNX/Wifi lines
int InitAllLines()
{
	LIST_HEAD(all_lines);
	ts_passageway_list_t *pLine = NULL;

	int nRes = db_get_all_lines(&all_lines, pLine);

	if(nRes != SQLITE_OK)
	{
		return nRes;
	}

	// Traversal the lines_list and add all of them into gateway-manager
	ts_passageway_list_t *pPassway = NULL;
	ts_passageway_list_t *pPasswayTmp = NULL;
	ts_gateway_list_t *pGateway = NULL;
	ts_gateway_list_t *pGatewayTmp = NULL;
	char interface_ip[32] = {'\0'};
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	list_for_each_entry_safe(pPassway, pPasswayTmp, &all_lines, list)
	{
		if(pPassway == NULL)
		{
			continue;
		}

		//ip of interface
		strcpy(interface_ip, (char *) pPassway->passageway.host_interface.info);

//		__fline;
//		printf("interface_ip == %s, gateway_list = %s\n",
//				interface_ip,
//				pPassway->passageway.gateway_list == NULL ? "null" : "not null");

		if(pPassway->passageway.gateway_list != NULL)
		{
//			printf("Interface Info: %s\n", interface_ip);

			pGateway = pPassway->passageway.gateway_list;

			list_for_each_entry_safe(pGateway, pGatewayTmp, &pPassway->passageway.gateway_heead, list)
			{
				if(pGateway->gateway.getewayType == TS_GATEWAY_TYPE_KNX
					||pGateway->gateway.getewayType == TS_GATEWAY_TYPE_WIFI)
				{
					pGatewayMan->AddGateway(
							pGateway->gateway.getewayType,
							pGateway->gateway.id,
							interface_ip,
							(char *) pGateway->gateway.gatewayIP,
							pGateway->gateway.gatewayPort);

					printf("		ADD %s: (%d, %s:%d)\n",
							pGateway->gateway.getewayType == TS_GATEWAY_TYPE_KNX ? "KNX" : "WIFI",
							pGateway->gateway.id,
							(char *) pGateway->gateway.gatewayIP,
							pGateway->gateway.gatewayPort);
				}
			}
		}
	}

	// delete the list
	list_for_each_entry_safe(pPassway, pPasswayTmp, &all_lines, list)
	{
		if(pPassway->passageway.gateway_list != NULL)
		{
			list_del(&pPassway->passageway.gateway_list->list);
			free(pPassway->passageway.gateway_list);
		}

		if(pPassway != NULL)
		{
			list_del(&pPassway->list);
			free(pPassway);
		}
	}

	return SUCCESS;
}

int protocol_start_manager()
{
	//stop_manager();

	int nRes = SUCCESS;
	nRes = InitAllLines();

	// just for test-------start---------------------------------
//	CGatewayManager::Instance()->AddGateway(
//			TS_GATEWAY_TYPE_KNX, 1, "192.168.10.234,", "192.168.1.13", 3671);
//	CGatewayManager::Instance()->AddGateway(
//			TS_GATEWAY_TYPE_WIFI, 2, "192.168.10.234,", "192.168.10.121", 60000);
	// just for test-------end---------------------------------

	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->StartGatewayManager();

	return nRes;
}

void protocol_stop_manager()
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->StopGatewayManager();
	sc_close();
}

int protocol_search_server(char *strErr)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	int res = pGatewayMan->SearchServer(strErr);
	if(res == 0)
	{
		return SUCCESS;
	}
	else
	{
		return ERR_PROTROL_SEARCH_FAILED;
	}
}

int protocol_add_gateway(int gateway_type, int key, char *local_ip, char* ip, int port)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	bool res = pGatewayMan->AddGateway(gateway_type, key, local_ip, ip, port);

	if(res)
	{
		return SUCCESS;
	}
	else
	{
		return ERR_PROTROL_GATEWAY_EXIST;
	}
}

void protocol_remove_gateway(int gateway_type, int key)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->RemoveGateway(gateway_type, key);
}

int protocol_connect_all()
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->ConnectAll();
	return SUCCESS;
}

int protocol_gateway_open(int gateway_id, int on_off)
{
	int nGatewayType = GATEWAY_TYPE_KNX;
	int nRes = 0;

	// get gateway type by gateway_id
	nRes = db_get_gateway_by_id(gateway_id, &nGatewayType);
	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	if(nGatewayType == TS_GATEWAY_TYPE_WIFI
			|| nGatewayType == TS_GATEWAY_TYPE_KNX)
	{
		if(on_off == SWITCH_ON)
		{
			nRes = protocol_connect_gateway_sync(nGatewayType, gateway_id);
		}
		else if(on_off == SWITCH_OFF)
		{
			protocol_disconnect_gateway(nGatewayType, gateway_id);
		}
	}
	return nRes;
}

int protocol_is_gateway_open(int gateway_id, int *on_off)
{
	if(!on_off)
	{
		return FAILED;
	}

	int nGatewayType = GATEWAY_TYPE_KNX;
	*on_off = SWITCH_OFF;

	int nRes = db_get_gateway_by_id(gateway_id, &nGatewayType);
	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	if(nGatewayType == TS_GATEWAY_TYPE_WIFI
			|| nGatewayType == TS_GATEWAY_TYPE_KNX)
	{
		*on_off = protocol_is_gateway_connected(nGatewayType, gateway_id);
		return SUCCESS;
	}
	return FAILED;
}

void protocol_connect_gateway(int gateway_type, int key)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->ConnectGateway(gateway_type, key);
}

int protocol_connect_gateway_sync(int gateway_type, int key)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	int nRes = pGatewayMan->ConnectGatewaySync(gateway_type, key);
	if(nRes == 0)
	{
		nRes = SUCCESS;
	}
	else
	{
		nRes = ERR_PROTROL_CONNECT_FAILED;
	}
	return nRes;
}

void protocol_disconnect_all()
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->DisconnectAll();
}

void protocol_disconnect_gateway(int gateway_type, int key)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->DisconnectGateway(gateway_type, key);
}

int protocol_is_gateway_connected(int gateway_type, int key)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	bool bConnect = pGatewayMan->IsGatewayConnected(gateway_type, key);
	if(bConnect)
	{
		return SWITCH_ON;
	}
	else
	{
		return SWITCH_OFF;
	}
}

void protocol_remove_all_gateways()
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->RemoveAllGateways();
}

// inner function
int AnalyseRequest(void * pBuf, int nLen, int &nFunType)
{
	int nHeadLen = sizeof(remote_header_t);
	if(nLen < nHeadLen || nLen > REPORT_MAX_LEN)
	{
		return ERR_PROTROL_MESSAGE_LEN;
	}

	remote_header_t *pHead = (remote_header_t *) pBuf;
	if(pHead->start != HEAD_START)
	{
		return ERR_PROTROL_MESSAGE_ERR;
	}

	short nDataLen = ntohs(pHead->length);
	if(nLen != nHeadLen + nDataLen)
	{
		return ERR_PROTROL_MESSAGE_LEN;
	}

	nFunType = pHead->function_code;
	
	return SUCCESS;
}

int protocol_request(void * buf, int len)
{
	__fline printf("up level message [%d][", len);
	for(int i = 0; i < len; i++)
	{
		printf("%.2x ", *((char *)buf + i));
	}
	printf("]\n");

	// ananlyse the request buf
	int nFunCode = 0;
	int nRes = AnalyseRequest(buf, len, nFunCode);
	if(nRes != SUCCESS)
	{
		__fline printf("AnalyseRequest nRes = %d\n", nRes);
		return nRes;
	}

	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	
	int nDevId = 0;
	int nDevType = 0;
	int nGatewayId = 0;
	int nGatewayType = 0;

	if(nFunCode == DEVICE_OPERATION)
	{
		kic_head_t*pKicHead = (kic_head_t *) buf;
		nDevId = ntohl(pKicHead->home_device_id);
		nDevType = pKicHead->device_type;
		
		printf("----current nFunType = DEVICE_OPERATION, device_id = %d, nDevType = %d----\n", nDevId, nDevType);
		
		// query db for more information
		switch(nDevType)
		{
		case KNX:
			{
				knx_control_report_t *pKnxReq = (knx_control_report_t *) buf;
				
				// Enocean device operation
				// group 15/0/0 is 30720
				/*
				int nGrpAddr = ntohs(pKnxReq->address);
				
				if(pKnxReq->cmd_type == WRITE && nGrpAddr > 30720)
				{
					pGatewayMan->AddEnoceanRequest(pKnxReq);
					int nValue = ntohl( *((int *) pKnxReq->data) );
					enocean_device_onoff(  16878053, 1, nValue);
					enocean_device_onoff(  16878053, 1, nValue);
					enocean_device_onoff(  16878053, 1, nValue);
					break;
				}
				*/
			
			__fline printf("case KNX(session_id=%d, device_type=%d, home_device_id=%d, cmd_type=%d, address=%d, data=%d, result_code=%d, data_len=%d, data_type=%d)\n",
					ntohl(pKnxReq->kic_head.session_id),
					pKnxReq->kic_head.device_type,
					ntohl(pKnxReq->kic_head.home_device_id),
					pKnxReq->cmd_type,
					ntohs(pKnxReq->address),
					ntohl(*((int *)pKnxReq->data)),
					pKnxReq->result_code,
					pKnxReq->data_len,
					pKnxReq->data_type);

			// get witch gateway to current device_id
			nRes = db_get_interface_gateway(nDevId, &nGatewayType, &nGatewayId);
			
			__fline printf("db_get_interface_gateway nRes = %d\n", nRes);
			if(nRes != EXIT_SUCCESS)
			{
				return nRes;
			}

			KNX_GroupData grpData;
			grpData.nGroupAddr = ntohs(pKnxReq->address);
			grpData.nGrpDPT = pKnxReq->data_type;
			GetValue_Buf2Grp(pKnxReq->data, pKnxReq->data_len, grpData);


			char reqBuf[REQ_RES_BUF_SIZE] = {'\0'};
			memcpy(reqBuf, buf, len);
			memcpy(reqBuf + len, &grpData, sizeof(grpData));
			int nTotalLen = len + sizeof(grpData);

			// add request into the send list of the gateway
			if(pKnxReq->cmd_type == WRITE)
			{
				//nGatewayId = 6;		// just for test
				nRes = pGatewayMan->WriteGrpVal(nGatewayId, reqBuf, nTotalLen, len);
			}
			else
			{
				nRes = pGatewayMan->AddToKnxReadDbList(nGatewayId, reqBuf, nTotalLen, len);
			}
			
			__fline printf("add request into the send list of the gateway nRes = %d\n", nRes);

			// if operate failed
			if(nRes != 0)
			{
				pKnxReq->result_code = nRes;
				if(len > 0)
				{
					memset(reqBuf + len, 0, nTotalLen - len);
				}
				pGatewayMan->GetCtrlResponse()(ACTION_KNX_EXTRA_RES, reqBuf, len);
			}
			break;
		}
#if 0
		case WIFI:
			{
				com_control_req_t *pWifiReq = (com_control_req_t *) buf;
				
				int nGatewayId = 0;
				int nGatewayType = 0;
				nRes = db_get_interface_gateway(nDevId, &nGatewayType, &nGatewayId);
				
				int nDevType = -1;
				nRes = db_get_device_type_by_id(&nDevType, nDevId);
				if(nRes != EXIT_SUCCESS)
				{
					return nRes;
				}
				
				if(nDevType != COM && nDevType != INFRARED
					&& nDevType != IR_STUDY && nDevType != CUSTOMER_COM)
				{
					return ERR_PROTROL_PARAMETERS;
				}
				
				nRes = pGatewayMan->AddWifiRequest(
					nGatewayId, buf, len);
				break;
			}
#endif
		case COM:
		case CUSTOMER_COM:
			{
				nRes = db_get_interface_gateway(nDevId, &nGatewayType, &nGatewayId);
				printf("current device_id = %d, nGatewayType = %d, nGatewayId = %d\n", nDevId, nGatewayType, nGatewayId);
				if(nRes != EXIT_SUCCESS)
				{
					return nRes;
				}
				
				if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST
					&& nGatewayType == TS_GATEWAY_TYPE_WIFI)
				{
					nRes = pGatewayMan->AddWifiRequest(
						nGatewayId, buf, len);
					return nRes;
				}
				
				int nComId = -1;
				int nArea =  -1;
				char protocol_file[32] = {'\0'};
				char com_name[32] = {'\0'};
				
				// get info of device
				nRes = db_get_serial_device_info(nDevId, &nComId,
					&nGatewayId, &nArea, protocol_file, com_name);
				printf("nDevId = %d, nComId = %d\n", nDevId, nComId);
				if(nRes != EXIT_SUCCESS)
				{
					return nRes;
				}
				
				nRes = ERR_PROTROL_COM_ID;
				for(int index = 0; index < pGatewayMan->g_com_count; index++)
				{
					if(pGatewayMan->g_com_infos[index].id == nComId)
					{
						nRes = pGatewayMan->g_comLogical[index].AddRequestNode(
							IC_DIRECT, buf, len);
						break;
					}
				}
				
				if(nRes != SUCCESS)
				{
					return nRes;
				}
				break;
			}
		case INFRARED:
			{
				nRes = db_get_interface_gateway(nDevId, &nGatewayType, &nGatewayId);
				if(nRes != EXIT_SUCCESS)
				{
					return nRes;
				}
				
				if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST
					&& nGatewayType == TS_GATEWAY_TYPE_WIFI)
				{
					nRes = pGatewayMan->AddWifiRequest(
						nGatewayId, buf, len);
					return nRes;
				}
				
				nRes = pGatewayMan->g_irLogical.AddRequestNode(IC_DIRECT, buf, len);
				if(nRes != SUCCESS)
				{
					return nRes;
				}
				break;
			}
		case IR_STUDY:
			{
				ir_study_req_t *pIrStudy = (ir_study_req_t *) buf;
				nGatewayId = ntohl(pIrStudy->gateway_id);
				nRes = db_get_gateway_by_id(nGatewayId, &nGatewayType);
				if(nRes != SUCCESS)
				{
					return nRes;
				}
				
				if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST
					&& nGatewayType == TS_GATEWAY_TYPE_WIFI)
				{
					nRes = pGatewayMan->AddWifiRequest(
						nGatewayId, buf, len);
					return nRes;
				}
				
				nRes = pGatewayMan->g_irLogical.AddRequestNode(IC_DIRECT, buf, len);
				if(nRes != SUCCESS)
				{
					return nRes;
				}
				break;
			}
		case SCENE:
			{
				scene_control_report_t *pScene = (scene_control_report_t *) buf;
				nRes = pGatewayMan->m_sceneMan.AddSceneRequestNode(ntohl(pScene->scene));
				break;
			}
		default:
			__fline printf("case default nRes = %d\n", ERR_PROTROL_NOT_SUPPORT);
			nRes = ERR_PROTROL_NOT_SUPPORT;
			break;
		}
	}
	else if(nFunCode == DEVICE_CONTROL)
	{
		int nDataPointId = 0;
		device_control_req *pDevCtrlReq = (device_control_req *) buf;
		nDataPointId = ntohl(pDevCtrlReq->datapoint_id);
		
		printf("----current nFunType = DEVICE_CONTROL, nDataPointId = %d----\n", nDataPointId);

		if(pDevCtrlReq->header.comm_type != REQUEST  
			&& pDevCtrlReq->header.comm_type != NOTRESPONSE)
		{
			return ERR_PROTROL_MESSAGE_ERR;
		}
		
		// handle the request
		// 1. get device_id, device type, gateway_id & gateway type
		// by datapoint id
		nRes = db_get_device_infos_by_dp(nDataPointId, &nDevId, &nDevType, &nGatewayId, &nGatewayType);
		if(nRes != EXIT_SUCCESS)
		{
			printf("db_get_device_infos_by_dp failed\n");
			return nRes;
		}

		switch(nDevType)
		{
			case ENOCEAN:
				// 2. add the request to logical module or to the wifi manager module
				if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST 
					&& nGatewayType == TS_GATEWAY_TYPE_WIFI)
				{
					nRes = pGatewayMan->AddWifiRequest(nGatewayId, buf, len);
					return nRes;
				}
				pGatewayMan->AddEnoceanRequest(pDevCtrlReq, len);
				break;
			default:
				return ERR_PROTROL_NOT_SUPPORT;
		}
	}
	else if(nFunCode == DEVICE_STATE)
	{
		if(pGatewayMan->GetCtrlResponse() == NULL)
		{
			return ERR_PROTROL_INNER_FAILED;
		}
	
		device_state_req *pDevStateReq = (device_state_req *) buf;
		nDevId = ntohl(pDevStateReq->home_device_id);
		
		printf("----current nFunType = DEVICE_STATE, nDevId = %d----\n", nDevId);

		if(pDevStateReq->header.comm_type != REQUEST)
		{
			return ERR_PROTROL_MESSAGE_ERR;
		}

		// handle the request
		// 1. get device type
		nRes = db_get_device_type_by_id(&nDevType, nDevId);
		if(nRes != EXIT_SUCCESS || nDevType != COM)
		{
			return nRes;
		}

		// 2. only support pre-defined serial device, if not return not support.
		switch(nDevType)
		{
		case COM:
			{
				// 3. query current device state from db through device id & return it to client.
				int nComDevType = 0;
				int nArea = 0;
				char currState[256] = {0};
				nRes = db_get_serial_device_curr(nDevId, &nComDevType, &nArea, (char *) currState);

				device_state_res *pDevStateRes = NULL;
				int nDataLen = strlen(currState) > (CURRENT_STATE_LEN - 1) ? (CURRENT_STATE_LEN - 1) : strlen(currState);
				int nMsgLen = sizeof(device_state_res) + nDataLen;
				
				pDevStateRes = (device_state_res *) new char[nMsgLen];

				pDevStateRes->header = pDevStateReq->header;
				pDevStateRes->session_id = pDevStateRes->session_id;
				pDevStateRes->home_device_id = pDevStateRes->home_device_id;
				pDevStateRes->res_code = nRes;
				pDevStateRes->data_len = nDataLen;

				// modify the value from original message.
				pDevStateRes->header.length = nMsgLen - sizeof(remote_header_t);
				pDevStateRes->header.comm_type = RESPONSE;
				
				if(nDataLen > 0)
				{
					strncpy((char *) pDevStateRes->data, (char *)currState, nDataLen);
				}

				if(pGatewayMan->GetCtrlResponse() != NULL)
				{
					pGatewayMan->GetCtrlResponse()(IC_DIRECT_COM_STATE, pDevStateRes, nMsgLen);
				}

				delete pDevStateRes;
				pDevStateRes = NULL;
				
				break;
			}
		default:
			return ERR_PROTROL_NOT_SUPPORT;
		}
	}
	else
	{
		return ERR_PROTROL_NOT_SUPPORT;
	}
	
	return nRes;
}


/* enocean interface, added by gengh on 2012/05/22 */
#ifdef DEV_ENOCEAN_SUPPORT
void protocol_enocean_init(enocean_callback_status_t callback)
{
	enocean_uart_callback(callback);
}

int protocol_enocean_device_onoff(uint32 u32DeviceID, int rocker, boolean on)
{
	return enocean_device_onoff(0x0080919F, 2, 1);
}
#endif
/* enocean end */





int protocol_ifrared_open_all(int on_off)
{
	fprintf(stderr, "Your parameter is %d\n", on_off);
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	int nRes = FAILED;
	
	if(on_off == SWITCH_ON)
	{
		pGatewayMan->OpenInfraredDev();
		if(pGatewayMan->g_irDeviceHandler >= 0)
		{
			nRes = SUCCESS;
		}
	}
	else
	{
		pGatewayMan->EndInfraredManager();
		if(pGatewayMan->g_irDeviceHandler == -1)
		{
			nRes = SUCCESS;
		}
	}
	
	return nRes;
}

int protocol_ifrared_open (int channel, int on_off)
{
	int nRes = protocol_ifrared_open_all(on_off);
	fprintf(stderr, "Your parameters are (channel=%d, on_off=%d, nRes=%d)\n", channel, on_off, nRes);
	return nRes;
}

int protocol_is_ifrared_open (int channel, int *on_off)
{
	fprintf(stderr, "Your parameters are (%d, %d)\n", channel, *on_off);
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	if(pGatewayMan->g_irDeviceHandler >= 0)
	{
		*on_off = SWITCH_ON;
	}
	else
	{
		*on_off = SWITCH_OFF;
	}
	return SUCCESS;
}

int protocol_serial_open_all(int on_off)
{
	fprintf(stderr, "Your parameters are (%d)\n", on_off);
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	if(on_off == SWITCH_ON)
	{
		pGatewayMan->StartComManager();
	}
	else
	{
		pGatewayMan->EndComManager();
	}
	return SUCCESS;
}

int protocol_serial_open(int number, int on_off)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	int nRes = pGatewayMan->OpenComI(on_off, number);
	fprintf(stderr, "Your parameters are (number=%d, on_off=%d, nRes=%d)\n", number, on_off, nRes);
	return nRes;
}

int protocol_is_serial_open(int number, int *on_off)
{
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	int nRes = pGatewayMan->IsOpenComI(on_off, number);
	fprintf(stderr, "Your parameters are (number=%d, on_off=%d, nRes=%d)\n", number, *on_off, nRes);
	return nRes;
}










