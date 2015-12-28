//
//  GatewayMananger.h
//  TSBusStack
//
//  Created by Gengh on 11-08-20.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

//#include "WifiException.h"
#include "GatewayManager.h"
#include "db/ts_db_protocol_infos.h"

#include "enocean/esp.h"
#include "enocean/enocean_mgr.h"
#include "CommonInclude.h"

#include <dlfcn.h>

#include <utility>
#include <signal.h>

FILE * g_log = NULL; // log file descriptor
bool g_bCheckWifi = false; // flag indicates whether need send datagram to check wifi connections

#define ACTIVE_LOAD_PROTOCOL_COUNT	1


//	// Notify wifi manager to check wifi connections, called by app layer.
//	void DECLSPEC_DLL_EXPORT NotifyWifiManager()
//	{
//		CGatewayManager* pMan = CGatewayManager::Instance();
//		g_bCheckWifi = true;
//		THREAD_HANDLE handle = CREATE_THREAD(NotifyManagerThread, pMan);
//		DETACH_THREAD(handle);
//	}


/*
void CGatewayManager::AddEnoceanRequest(knx_control_report_t *pKnxReq)
{
	if(pKnxReq == NULL)
	{
		return ;
	}
	
	for(int index = 0; index < MAX_ENOCEAN_COUNT; index++)
	{
		if(m_enocean_data[index].kic_head.home_device_id <= 0)
		{
			m_enocean_data[index] = *pKnxReq;
		}
	}
}
*/

/*
void CGatewayManager::HandleEnoceanResponse(int nDevId, int nData)
{
	if(nDevId <= 0)
	{
		return ;
	}

	knx_control_report_t knxReq;
	for(int index = 0; index < MAX_ENOCEAN_COUNT; index++)
	{
		knxReq = m_enocean_data[index];
		if( (int)ntohl(knxReq.kic_head.home_device_id)*/ /*(int)ntohl(knxReq.kic_head.home_device_id) == nDevId */ /* )
		{
			knxReq.result_code = 0;
			sprintf((char *) knxReq.data, "%d", ntohl(nData) );
			
			if(GetCtrlResponse() != NULL)
			{
				GetCtrlResponse()(ACTION_KNX_WRITE_RES, (void *) &knxReq, sizeof(knx_control_report_t));
   			}
			memset((char *) &m_enocean_data[index], 0, sizeof(knx_control_report_t));
			break;
		}
	}
}
*/

void CGatewayManager::UpdateDatapointData(int nDataPointId, char * strValue)
{
	if(strValue == NULL)
	{
		return ;
	}

	db_set_dp_curr_value(nDataPointId, strValue);
}

int CGatewayManager::GetDatapointData(int nDataPointId, char * strValue)
{
	if(strValue == NULL)
	{
		return ERR_PROTROL_INNER_FAILED;
	}

	int nRes = db_get_dp_curr_value(nDataPointId, strValue);
	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	return SUCCESS;
}


int CGatewayManager::AddEnoceanRequest(device_control_req *pEnoceanReq, int nLen)
{
	int nRes = m_EnoceanLogic.AddRequestNode(IC_DIRECT, (void *) pEnoceanReq, nLen);
	return nRes;
}

void CGatewayManager::HandleEnoceanAutoBack(unsigned int nEnoceanId, int nData)
{
	// 1. get all of the data-point by EnoceanId from db.
	LIST_HEAD(all_datapoint_list);
	ts_dps_list_t * pDataPoint = NULL;
	ts_dps_list_t * pDataPointTemp = NULL;
	int nRes = db_get_dp_by_enocean_id(nEnoceanId, &all_datapoint_list, pDataPoint);
	if(nRes != EXIT_SUCCESS)
	{
		return ;
	}

	int nEnoceanOnOffDatapointId = -1;
	int nEnoceanDimmingDatapointId = -1;
	// 2. check whether it has a dimming datapoint in the device.
	list_for_each_entry_safe(pDataPoint, pDataPointTemp, &all_datapoint_list, list)
	{
		if(pDataPoint != NULL && pDataPoint->dp_info.ctrl_type == cut_1_LightDimmer)
		{
			nEnoceanDimmingDatapointId = pDataPoint->dp_info.dp_id;
		}
		else if(pDataPoint != NULL && pDataPoint->dp_info.ctrl_type == cut_1_LightOnOff)
		{
			nEnoceanOnOffDatapointId= pDataPoint->dp_info.dp_id;
		}
	}
			
	list_for_each_entry_safe(pDataPoint, pDataPointTemp, &all_datapoint_list, list)
	{
		if(pDataPoint != NULL)
		{
			list_del(&pDataPoint->list);
			free(pDataPoint);
		}
	}

	if(nEnoceanOnOffDatapointId < 0 && nEnoceanDimmingDatapointId < 0)
	{
		// no legal datapoint in this device, ignore the data.
		return ;
	}
	
	char strValue[8] = {0};
	sprintf((char *) strValue, "%d", nData);

	// has dimming datapoint in the device.
	if(nEnoceanDimmingDatapointId > 0)
	{
		UpdateDatapointData(nEnoceanDimmingDatapointId, strValue);
	}
	else
	{
		// update this on_off datapoint value with nData.
		UpdateDatapointData(nEnoceanOnOffDatapointId, strValue);
	}

	int nDataLen = 0;
	int nMsgLen = 0;
	device_control_res *pEnoceanAutoBack = NULL;

	if(nEnoceanOnOffDatapointId > 0 || nEnoceanDimmingDatapointId > 0)
	{
		nDataLen = 1;
		nMsgLen = sizeof(device_control_res) + nDataLen;
		pEnoceanAutoBack = (device_control_res *) new char[nMsgLen];
		memset(pEnoceanAutoBack, 0, nMsgLen);

		pEnoceanAutoBack->header.start = HEAD_START;
		pEnoceanAutoBack->header.protocol_type = 0;
		pEnoceanAutoBack->header.length = nMsgLen - sizeof(remote_header_t);
		pEnoceanAutoBack->header.serial_number = 0;
		pEnoceanAutoBack->header.function_code = DEVICE_CONTROL;
		pEnoceanAutoBack->header.comm_type = RESPONSE;
		pEnoceanAutoBack->header.comm_source = 0;
		pEnoceanAutoBack->header.proto_ver = 0;

		pEnoceanAutoBack->session_id = 0;
		int nDataPointId = nEnoceanDimmingDatapointId > 0 ? nEnoceanDimmingDatapointId : nEnoceanOnOffDatapointId;
		pEnoceanAutoBack->datapoint_id = htonl(nDataPointId);
		pEnoceanAutoBack->read_write = AUTOBACK;
		pEnoceanAutoBack->res_code = 0;
		pEnoceanAutoBack->data_len = 1;
		*( (char *) pEnoceanAutoBack->data) = nData;
			
__fline printf("------------ HandleEnoceanAutoBack nEnoceanId=%ud, nDataPointId=%d, value=%d\n", nEnoceanId, nDataPointId, nData);
		if(WhoAmI() == TS_PRO_TYPE_GATEWAY)
		{
			if(m_EnoceanLogic.GetICIface() != NULL)
			{
				m_EnoceanLogic.GetICIface()->Send(ACTION_WIFI_RESPONSE, pEnoceanAutoBack, nMsgLen);
			}
		}
		else if(WhoAmI() == TS_PRO_TYPE_HOST && GetCtrlResponse() != NULL)
		{
			GetCtrlResponse()(IC_DIRECT, pEnoceanAutoBack, nMsgLen);
		}

		delete pEnoceanAutoBack;
		pEnoceanAutoBack = NULL;
	}
}

/*
void CGatewayManager::UpdateAndSendEnoceanRelativedDatapoints(int nDataPointType, 
	int nEnoceanOnOffDatapointId, int nEnoceanDimmingDatapointId, 
	device_control_res *pEnoceanCtrlRes, int nMsgLen, int nDataLen)
{
	if(nDataLen <= 0)
	{
		return ;
	}
	
	sprintf((char *) strValue, "%d", nValueAfterCtrl);
	pGatewayMan->UpdateDatapointData(nDataPointId, strValue);
				
	if(*nDataPointType == cut_1_LightOnOff && nEnoceanDimmingDatapointId < 0)
	{
		// just on off.
	}
	else if(*nDataPointType == cut_1_LightOnOff && nEnoceanDimmingDatapointId > 0)
	{
		// ctrl on off, but also dimming
		nDataPointId = ntohl(nEnoceanDimmingDatapointId);
		nValueAfterCtrl = nValueAfterCtrl == 1 ? 100 : 0;
		*((char*) pEnoceanCtrlRes->data) = nValueAfterCtrl;
		pGatewayMan->GetCtrlResponse()(IC_DIRECT, pBuf, nLen);
				
		sprintf((char *) strValue, "%d", nValueAfterCtrl);
		pGatewayMan->UpdateDatapointData(nEnoceanDimmingDatapointId, strValue);
	}
	else if(*nDataPointType == cut_1_LightDimmer&& nEnoceanOnOffDatapointId < 0)
	{
		// just dimming.
	}
	else if(*nDataPointType == cut_1_LightDimmer && nEnoceanOnOffDatapointId > 0)
	{
		// ctrl dimming, but also on off
		nDataPointId = ntohl(nEnoceanOnOffDatapointId);
		nValueAfterCtrl = nValueAfterCtrl > 0 ? 1 : 0;
		*((char*) pEnoceanCtrlRes->data) = nValueAfterCtrl;
		pGatewayMan->GetCtrlResponse()(IC_DIRECT, pBuf, nLen);
				
		sprintf((char *) strValue, "%d", nValueAfterCtrl);
		pGatewayMan->UpdateDatapointData(nEnoceanOnOffDatapointId, strValue);
	}
}
*/

#ifdef DEV_ENOCEAN_SUPPORT
void EnOceanCallBack(ENOCEAN_CALLBACK_PARAM_T *param)
{
    ENOCEAN_CALLBACK_PARAM_T *enoceanParam = (ENOCEAN_CALLBACK_PARAM_T *)param;

    __fline;
    printf("u32DeviceID:%ld, bOn:%d\n", enoceanParam->u32DeviceID, enoceanParam->bOn);
	
    CGatewayManager *pGatewayMan = CGatewayManager::Instance();
    pGatewayMan->HandleEnoceanAutoBack(enoceanParam->u32DeviceID, enoceanParam->bOn);
}
#endif


int GetValue_Buf2Grp(BYTE *pBuf, int data_len, KNX_GroupData &grpData)
{
		switch (grpData.nGrpDPT)
		{
			case DPT_Boolean:
				grpData.Data.BinData = ntohl( *((UINT *)pBuf) ) == 1 ? 1 : 0;
				break;

			case DPT_1BitCtrl:
			case DPT_3BitCtrl:
			case DPT_CharSet:
			case DPT_8BitUnSign:
			case DPT_8BitSign:
			case DPT_2OctUnSign:
			case DPT_2OctSign:
			case DPT_4OctUnSign:
			case DPT_4OctSign:
			case DPT_Access:
				grpData.Data.AnaData = ntohl( *((int *)pBuf) );
				break;

			case DPT_2OctFloat:
			case DPT_4OctFloat:
				grpData.Data.AnaData = *((float*) pBuf);
				break;

			case DPT_Time:
			{
				time_data_t *pData = (time_data_t *) pBuf;

				grpData.Data.TimeData.year = 2000;
				grpData.Data.TimeData.month = 1;
				grpData.Data.TimeData.day = 1;
				grpData.Data.TimeData.weekday = 6;
				grpData.Data.TimeData.hour = pData->hour;
				grpData.Data.TimeData.minute = pData->minute;
				grpData.Data.TimeData.second = pData->second;
				break;
			}

			case DPT_Date:
			{
				time_data_t *pData = (time_data_t *) pBuf;

				grpData.Data.TimeData.year = ntohs(pData->year);
				grpData.Data.TimeData.month = pData->month;
				grpData.Data.TimeData.day = pData->day;
				grpData.Data.TimeData.weekday = pData->weekday;
				grpData.Data.TimeData.hour = 0;
				grpData.Data.TimeData.minute = 0;
				grpData.Data.TimeData.second = 0;
			}
				break;

			case DPT_String:
			{
				memset(&grpData.Data.TxtData, 0, TXT_DATA_SIZE);
				int nCpyLen = TXT_DATA_SIZE < data_len ? TXT_DATA_SIZE : data_len;
				memcpy(&grpData.Data.TxtData, pBuf, nCpyLen);
				break;
			}

			case DPT_Unknown:
			default:
				break;
		}

		return 0;
}

int GetValue_Grp2Buf(BYTE *pBuf, KNX_GroupData &grpData)
{
		switch (grpData.nGrpDPT)
		{
			case DPT_Boolean:
				sprintf((char*)pBuf, "%d", grpData.Data.BinData);
				break;

			case DPT_CharSet:
			case DPT_2OctSign:
			case DPT_4OctSign:
				sprintf((char*)pBuf, "%d", (int)grpData.Data.AnaData);
				break;

			case DPT_1BitCtrl:
			case DPT_3BitCtrl:
			case DPT_8BitSign:
			case DPT_8BitUnSign:
			case DPT_2OctUnSign:
			case DPT_4OctUnSign:
				sprintf((char*)pBuf, "%u", (int)grpData.Data.AnaData);
				break;

			case DPT_2OctFloat:
			case DPT_4OctFloat:
				sprintf((char*)pBuf, "%f", grpData.Data.AnaData);
				break;

			case DPT_Time:
			{
				sprintf((char*)pBuf, "%d-%d-%d %d:%d:%d",
						2000, 1, 1,
						grpData.Data.TimeData.hour,
						grpData.Data.TimeData.minute,
						grpData.Data.TimeData.second);
				break;
			}

			case DPT_Date:
			{
				sprintf((char*)pBuf, "%d-%d-%d %d:%d:%d",
						htons(grpData.Data.TimeData.year),
						grpData.Data.TimeData.month,
						grpData.Data.TimeData.day,
						0, 0, 0);
/*
				pData->year = grpData.Data.TimeData.year;
				pData->month = grpData.Data.TimeData.month;
				pData->day = grpData.Data.TimeData.day;
				pData->weekday = grpData.Data.TimeData.weekday;
				pData->hour = 0;
				pData->minute = 0;
				pData->second = 0;
*/
			}
				break;


			case DPT_Access:
				sprintf((char*)pBuf, "%d", (int)grpData.Data.AnaData);
				break;

			case DPT_String:
			{
				memcpy(pBuf, &grpData.Data.TxtData, TXT_DATA_SIZE);
				break;
			}

			case DPT_Unknown:
			default:
				break;
		}
		return 0;
}

int GetValue_DbBuf2KnxBuf(ts_knx_control_info_t *pKnxData)
{
	if(pKnxData == NULL)
	{
		return FAILED;
	}

	int nLen = strlen((char*)pKnxData->data);
	int nVal = 0;
	float fVal = 0;
	switch (pKnxData->data_type)
	{
		case DPT_Boolean:
		case DPT_CharSet:
		case DPT_2OctSign:
		case DPT_4OctSign:
		case DPT_1BitCtrl:
		case DPT_3BitCtrl:
		case DPT_8BitSign:
		case DPT_8BitUnSign:
		case DPT_2OctUnSign:
		case DPT_4OctUnSign:
		case DPT_Access:
			nVal = atol((char *)pKnxData->data);
			memset(pKnxData->data, 0, nLen);
			*((int*) pKnxData->data) = htonl(nVal);
			break;

		case DPT_2OctFloat:
		case DPT_4OctFloat:
			fVal = atof((char *)pKnxData->data);
			memset(pKnxData->data, 0, nLen);
			*((float*) pKnxData->data) = fVal;
			break;

		case DPT_Time:
		case DPT_Date:
		case DPT_String:
			// do nothing : it is it's self
			break;

		case DPT_Unknown:
		default:
			break;
	}
	return 0;
}



	
	// Notify wifi manager to check knx connection, called by stack itself.
	void DECLSPEC_DLL_EXPORT NotifyKnxManagerInStack()
	{
		CGatewayManager* pMan = CGatewayManager::Instance();
		THREAD_HANDLE handle = CREATE_THREAD(NotifyManagerThread, pMan);
		DETACH_THREAD(handle);
	}

	// Notify wifi manager to check wifi connections, called by stack itself.
	void DECLSPEC_DLL_EXPORT NotifyWifiManagerInStack(void* wifi)
	{
		CGatewayManager* pMan = CGatewayManager::Instance();
		
/*		// Switch to alive connection
		//LOCK_MUTEX(& (pMan->GetWifiGatewayMutex()));
		CWifiKernel* pWifi = (CWifiKernel*) wifi;
		WIFI_GATEWAY_MAP* pGateways = NULL;

		if (pWifi->IsActive())
		{
			pGateways = pMan->GetCandidateMap();

			WIFI_GATEWAY_MAP_ITER iter = pGateways->find(pWifi->GetGatewayKey());
			if (pGateways->end() != iter && iter->second->IsRunning())
			{
				// another wifi connection is ok, so active it
				iter->second->SetActive(true);
				pWifi->SetActive(false);
				DEBUG_LOG2(WifiManager, Switch to active WIFI gateway %s.%s\n,
					iter->second->GetIP(), iter->second->GetStatusLog());
			}
		}
*/
		
		// Notify wifi manager thread
		THREAD_HANDLE handle = CREATE_THREAD(NotifyManagerThread, pMan);
		DETACH_THREAD(handle);
	}

	int NotifyManagerThread(void *pParam)
	{
		CGatewayManager *pMan = (CGatewayManager *) pParam;

		// Singal main thread to check all knx & wifi connections
#ifdef _WIN32
//		SET_SIGNAL(pMan->GetReconnWifiCond()->timerCond);
		SET_SIGNAL(pMan->GetReconnKnxCond()->timerCond);
#else
//		LOCK_MUTEX(&pMan->GetReconnWifiCond()->timerMutex);
//		SET_SIGNAL(pMan->GetReconnWifiCond()->timerCond);
//		UNLOCK_MUTEX(&pMan->GetReconnWifiCond()->timerMutex);
		
		LOCK_MUTEX(&pMan->GetReconnKnxCond()->timerMutex);
		SET_SIGNAL(pMan->GetReconnKnxCond()->timerCond);
		UNLOCK_MUTEX(&pMan->GetReconnKnxCond()->timerMutex);
#endif

		return 0;
	}
	
	// SIGPIPE handler
	void sig_handler(int signo)
	{
        return;
	}


	// Singleton
	CGatewayManager* CGatewayManager::Instance()
	{
		static CGatewayManager _instance;
		return &_instance ;
	}

	CGatewayManager::CGatewayManager()
	{
		m_nConnectKey = -1;
		m_nWifiHandle = 0;
		m_nKnxHandle = 0;
		m_nKnxReadThread = 0;
		m_nKnxReadDbThread = 0;
		m_nKnxClearCount = KNX_RECONNECT_INTERVAL_NOTIFY;
		
		m_pGetKnxDpt = 0;
		m_pSearchResponse = 0;
		m_pReadResponse = 0;
		m_pWriteResponse = 0;
		m_pKnxConnect = 0;
		m_pKnxDisconnect = 0;
		m_pErrorFun = 0;

		// wifi tcp server
		g_TcpServer = 0;
		g_pSockerServer = NULL;
		m_pServerLogical = NULL;
		memset(g_pSerial, 0, TOTAL_COM_NUM * sizeof(ezserial_t *));
		memset(g_com_infos, 0, TOTAL_COM_NUM * sizeof(com_info_t));
		g_com_count = 0;

		// infrared manager
		g_irDeviceHandler = -1;
		memset(g_bIrLearnReverse, false, sizeof(bool) * TOTAL_INFRARED_NUM);
		memset(g_bIrSendReverse, false, sizeof(bool) * TOTAL_INFRARED_NUM);

		m_nWhoAmI = TS_PRO_TYPE_HOST;

		m_pKnx = new CKNXKernel(0, 0, (char *) "127.0.0.1", 3671, this);

		INITIALIZE_MUTEX(&m_reconnWifi.timerMutex);
		INITIALIZE_MUTEX(&m_reconnKnx.timerMutex);

		INITIALIZE_MUTEX(&m_wifiGatewayMutex);
		INITIALIZE_MUTEX(&m_knxGatewayMutex);
		INITIALIZE_MUTEX(&m_knxReadCondStruct.timerMutex);
		INITIALIZE_MUTEX(&m_KnxReadDbCondStruct.timerMutex);
#ifdef _WIN32
		m_reconnWifi.timerCond = COND_INIT();
		m_reconnKnx.timerCond = COND_INIT();
		m_knxReadCondStruct.timerCond = COND_INIT();
		m_KnxReadDbCondStruct.timerCond = COND_INIT();
#else
		COND_INIT(&m_reconnWifi.timerCond);
		COND_INIT(&m_reconnKnx.timerCond);
		COND_INIT(&m_knxReadCondStruct.timerCond);
		COND_INIT(&m_KnxReadDbCondStruct.timerCond);
#endif
#ifndef _WIN32
		struct sigaction act;
		act.sa_handler = sig_handler;
		sigaction(SIGPIPE, &act, NULL);		
#endif	
	}
	
	CGatewayManager::~CGatewayManager()
	{
		if(m_pKnx)
		{
			delete m_pKnx;
			m_pKnx = NULL;
		}
		
		// stop main thread
		
		StopGatewayManager();

		RemoveAllGateways();

		DESTROY_MUTEX(&m_reconnWifi.timerMutex);
		DESTROY_MUTEX(&m_reconnKnx.timerMutex);
		DESTROY_MUTEX(&m_wifiGatewayMutex);
		DESTROY_MUTEX(&m_knxReadCondStruct.timerMutex);
		DESTROY_MUTEX(&m_KnxReadDbCondStruct.timerMutex);

		COND_DESTROY(m_reconnKnx.timerCond);
		COND_DESTROY(m_reconnWifi.timerCond);
		COND_DESTROY(m_knxReadCondStruct.timerCond);
		COND_DESTROY(m_KnxReadDbCondStruct.timerCond);

		if (g_log)
		{
			fclose(g_log);
		}

	}

	void CGatewayManager::SetCtrlResponse(OnCtrlResponse pCtrlResponse)
	{
		m_pCtrlResponse = pCtrlResponse;
	}
	
	bool CGatewayManager::SetKnxClearTime(int nSecond)
	{
		if(nSecond < 5)
		{
			return false;
		}
		
		m_nKnxClearCount = nSecond / 5;
		
		return true;
	}

	// Functions
	void CGatewayManager::StartGatewayManager()
	{
		db_get_who_am_i(&m_nWhoAmI);
		if(m_nWhoAmI != TS_PRO_TYPE_GATEWAY)
		{
			m_nWhoAmI = TS_PRO_TYPE_HOST;
		}

		m_bRunning = true;

		// start knx & wifi client manager
		// start scene manager
		
		if(m_nWhoAmI == TS_PRO_TYPE_HOST)
		{
			m_nKnxHandle = CREATE_THREAD(KnxThread, this);
			m_nKnxReadThread = CREATE_THREAD(KnxReadThread, this);
			m_nKnxReadDbThread = CREATE_THREAD(KnxReadDbThread, this);
			m_nWifiHandle = CREATE_THREAD(WifiThread, this);

			m_sceneMan.StartSceneThread();

			db_reset_knx_curr();
			db_reset_serial_dev_curr();
		}

		// start wifi server manager
		if(m_nWhoAmI == TS_PRO_TYPE_GATEWAY)
		{
			m_pServerLogical = new CICLogical();
			m_pServerLogical->StartSendThread();
			
			db_reset_serial_dev_curr();
		}
		
     	StartComManager();
		StartInfraredManager();

		ActiveLoadProtocols();
	}

	void CGatewayManager::ActiveLoadProtocols()
	{
		char activeLoadProtocol[ACTIVE_LOAD_PROTOCOL_COUNT][32];
		int index = 0;
		for(index = 0; index < ACTIVE_LOAD_PROTOCOL_COUNT; index++)
		{
			memset((char *) activeLoadProtocol[index], 0, 32);
		}
		strcpy((char *) activeLoadProtocol[0], "libAQM_1000.so");

		ComSoStruct * pComSo = NULL;

		LIST_HEAD(all_the_protocol_devices);
		ts_serial_protrol_dev_list_t *pDevices = NULL;
		ts_serial_protrol_dev_list_t *pDevicesTemp = NULL;
		int nRes = 0;

		for(index = 0; index < ACTIVE_LOAD_PROTOCOL_COUNT; index++)
		{			
			if(strlen(activeLoadProtocol[index]) > 0)
			{
				nRes = db_get_serial_devs_by_protrol(&all_the_protocol_devices, pDevices, activeLoadProtocol[index]);	
				if(nRes != EXIT_SUCCESS)
				{
					continue ;
				}
				
				list_for_each_entry_safe(pDevices, pDevicesTemp, &all_the_protocol_devices, list)
				{
					if(pDevices != NULL)
					{
						pComSo = GetProtocolSoStruct( (char *) activeLoadProtocol[index]);
						if(pComSo == NULL)
						{
							continue ;
						}
						pComSo->pInstance->AddComIndex(pDevices->devs.com_id);
						pComSo->pInstance->AddDeviceAddress(pDevices->devs.com_id, pDevices->devs.area);
					}
				}
				
				list_for_each_entry_safe(pDevices, pDevicesTemp, &all_the_protocol_devices, list)
				{
					if(pDevices != NULL)
					{
						list_del(&pDevices->list);
						free(pDevices);
					}
				}
			}
		}
	}

	void CB_ComRecvedData(int nIndex, unsigned char *pBuf, int nLen)
	{
		// adde data to the protocol handler of the com port

		//1. find the protocol name from db through com port id;
		int nDevType = -1;
		char strProtocol[32] = {'\0'};
		db_get_devtype_and_protrol(nIndex, strProtocol, &nDevType);

//		__rnline printf("(nIndex=%d, strProtocol=%s, nDevType=%d, COM[%d])\n",
//				nIndex, strProtocol, nDevType, COM);

		if(nDevType != COM)
		{
			// maybe COMSTOMER_COM, do not handler
			return ;
		}

		// 2. get the protocol handler
		CGatewayManager *pGatewayMan = CGatewayManager::Instance();
		ComSoStruct *pComSo = pGatewayMan->GetProtocolSoStruct(strProtocol);

		if(pComSo == NULL)
		{
			return ;
		}

		// 3. add the data into the protocol handler
		int nComId = -1;
		for(int index = 0; index < pGatewayMan->g_com_count; index++)
		{
			if(pGatewayMan->g_com_infos[index].id == nIndex)
			{
				nComId = index;
				break;
			}
		}

		if(nComId >= 0 && nComId < pGatewayMan->g_com_count)
		{
			if(pComSo->pInstance != NULL)
			{
				pComSo->pInstance->AddData(nComId, pBuf, nLen);
			}
		}
	}

	ComSoStruct * CGatewayManager::GetProtocolSoStruct(char *strProtocol)
	{
		if(strProtocol == NULL || strlen(strProtocol) == 0)
		{
			return NULL;
		}

		// 1. find the handler pointer in map
		CHARP_COMSOSTRUCT_MAP_ITER iter = m_mapSoStruct.begin();
		for(; iter != m_mapSoStruct.end(); iter++ )
		{
			if(strcmp(iter->first, strProtocol) == 0)
			{
				return iter->second;
			}
		}

		ComSoStruct *pNewStruct = new ComSoStruct();
		// load the so from COM_SO_PATH/libXXXXX.so
		// and load all them functions && initialization
		char strFile[256] = {'\0'};
		sprintf(strFile, "%s%s", COM_SO_PATH, strProtocol);
		pNewStruct->handler = dlopen(strFile, RTLD_NOW);
		if(pNewStruct->handler == NULL)
		{
			printf("[%s]\n", dlerror());
			delete pNewStruct;
			return NULL;
		}

		pNewStruct->pCreateInstance = (CreateSoInstance) dlsym(pNewStruct->handler, "CreateInstance");
		pNewStruct->pDeleteInstance = (DeleteSoInstance) dlsym(pNewStruct->handler, "DeleteInstance");
		pNewStruct->pInstance = pNewStruct->pCreateInstance();

		if(pNewStruct->pCreateInstance == NULL || pNewStruct->pDeleteInstance == NULL
				|| pNewStruct->pInstance == NULL)
		{
			__fline printf("Some functions loaded failed in [%s, %d](%d, %d, %d).\n",
					strFile, (int) pNewStruct->handler,
					(int) pNewStruct->pCreateInstance, (int) pNewStruct->pDeleteInstance,
					(int) pNewStruct->pInstance);
			dlclose(pNewStruct->handler);
			delete pNewStruct;
			return NULL;
		}

		pNewStruct->pInstance->SetProtocolName(strProtocol);
		pNewStruct->pInstance->SetCallbacks(CB_ProtocolHandled, 
			CB_ProtocolSendComDataByComIndex, CB_ProtocolSendComDataByProtoAddr);

		m_mapSoStruct.insert(CHARP_COMSOSTRUCT_PAIR(strProtocol, pNewStruct));

		return pNewStruct;
	}

	void CB_ProtocolSendComDataByProtoAddr(char *strProtocol, int nAddress, char * pMsg, int nLen)
	{
		// do nothing now.
	}

	void CB_ProtocolSendComDataByComIndex(int nComIndex, char * pMsg, int nLen)
	{
		if(pMsg == NULL)
		{
			return ;
		}

		CGatewayManager *pGatewayMan = CGatewayManager::Instance();
		for(int index = 0; index < pGatewayMan->g_com_count; index++)
		{
			if(pGatewayMan->g_com_infos[index].id == nComIndex)
			{
				if(pGatewayMan->g_pSerial[index] != NULL)
				{
					ezserial_putdata(pGatewayMan->g_pSerial[index], (unsigned char *) pMsg, nLen);
				}
				break;
			}
		}
	}

	void CB_ProtocolHandled(int nType, int nComId, char *strProtocolName, void *pBuf, int nLen)
	{
		if(nType != BACK_BACKAUDIO && nType != BACK_AUXDIO_02 && nType != BACK_AUXDIO_03
			&& nType != BACK_AUXDIO_04 && nType != BACK_AUXDIO_05 && BACK_AQM_1000)
		{
			return ;
		}
	
		CGatewayManager *pGatewayMan = CGatewayManager::Instance();

/*		// don't send to terminate
		if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_GATEWAY && pGatewayMan->m_pServerLogical != NULL)
		{
			// just in wifi gateway
			pGatewayMan->m_pServerLogical->SetIcSignal(nComId + 1, strProtocolName);

		}
		else if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST)
		{
			// just in host
			pGatewayMan->g_comLogical[nComId].SetIcSignal(nComId + 1, strProtocolName);
		}
*/

		if(nLen > 0 && pBuf != NULL)
		{
			int nMsgLen = sizeof(serial_initiative_res_t) + nLen;
			serial_initiative_res_t *pAutoRes = (serial_initiative_res_t *) new char[nMsgLen];
			memset(pAutoRes, 0, nMsgLen);

			pAutoRes->kic_head.header.start = HEAD_START;
			pAutoRes->kic_head.header.length = htons(nMsgLen - sizeof(remote_header_t));
			pAutoRes->kic_head.header.function_code = DEVICE_OPERATION;
			pAutoRes->kic_head.header.comm_type = RESPONSE;
			pAutoRes->kic_head.header.comm_source = HOST_S;

			pAutoRes->kic_head.session_id = 0;
			pAutoRes->kic_head.device_type = SERIAL_RES;
			// get the device_id by nComId
			int nDevId = 0;
			int nRes = db_get_dev_by_serial(pGatewayMan->g_com_infos[nComId].id, &nDevId);
			if(nRes != EXIT_SUCCESS)
			{
				return;
			}
			pAutoRes->kic_head.home_device_id = htonl(nDevId);

			// has deleted 2012/6/27
			//pAutoRes->channel = pGatewayMan->g_com_infos[nComId].id;
			pAutoRes->response_type = nType;
			pAutoRes->data_len = nLen;
			memcpy( ((char *)pAutoRes) + nMsgLen - nLen, pBuf, nLen);

			/*
			// do not send to terminate
			if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_GATEWAY && pGatewayMan->g_pSockerServer != NULL)
			{
				pGatewayMan->g_pSockerServer->SendToHost((char *) pAutoRes, nMsgLen);
			}
			else if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST && pGatewayMan->GetCtrlResponse() != NULL)
			{
				pGatewayMan->GetCtrlResponse()(nType, pAutoRes, nMsgLen);
			}
			*/
			

			// get the device's current state
			char curr_state[CURRENT_STATE_LEN] = {0};
    		int nComDevType_DB = UNKNOWN_DEVICE;
    		int nArea_DB = TS_ZONE_UNUSED;
    		nRes = db_get_serial_device_curr(nDevId, &nComDevType_DB, &nArea_DB, (char *) curr_state);
    		if(nRes != EXIT_SUCCESS && nRes != ERR_DB_NO_RECORDS)
    		{
    		    return ;
    		}
			
			// record current state
			int nArea = 0;
			// not used----------
			int comm_id = 0, gateway_id = 0;
			char file_name[64] = {0};
			char comm_port[32] = {0};
			// not used----------
			nRes = db_get_serial_device_info(nDevId, &comm_id, &gateway_id, &nArea, (char *) file_name, (char *) comm_port);
		
			int nComDevType = UNKNOWN_DEVICE;
			if(strcmp(file_name, "libEpson_Esc_Vp21.so") == 0)
			{
				nComDevType = EPSON_ESC_VP21;
			}
			else if(strcmp(file_name, "libAuxdio_8150.so") == 0)
			{
				nComDevType = AUXDIO_8150;
			}
			else if(strcmp(file_name, "libBackaudio_A61L.so") == 0)
			{
				nComDevType = BACKAUDIO_A61L;
			}
			else if(strcmp(file_name, "libYodar_4028B.so.so") == 0)
			{
				nComDevType = YODAR_4028B;
			}
			else if(strcmp(file_name, "libIstv.so") == 0)
			{
				nComDevType = ISTV;
			}
			else if(strcmp(file_name, "libIstv.so") == 0)
			{
				nComDevType = AQM_1000;
			}
			else
			{
				if(pAutoRes != NULL)
				{
					delete pAutoRes;
					pAutoRes = NULL;
				}
				return ;
			}

			// if it is old data, but it isn't current device's data.
            if(nArea != nArea_DB || nComDevType_DB != nComDevType)
            {
                memset(curr_state, 0, CURRENT_STATE_LEN);
            }

			// not used.
			// bool bSceneFlag = false;
			switch(nComDevType)
            {
			case EPSON_ESC_VP21:
				if(nType == BACK_EPSON)
				{
					// do nothing
                	break;
				}
            case BACKAUDIO_A61L:
				{
					backaudio_a61l_state * back_a61l = (backaudio_a61l_state *) curr_state;
					if( nType == BACK_BACKAUDIO)
					{
						backaudio_back_t *pBack = (backaudio_back_t *) pAutoRes;
						if(back_a61l->back.source != pBack->source)
						{
							//bSceneFlag = true;
						}
						back_a61l->back = *pBack;
            		    break;
					}
            	}
            case AUXDIO_8150:
				{
             		auxdio_8150_state * auxdio_8150 = (auxdio_8150_state *) curr_state;
					switch(nType)
					{
					case BACK_AUXDIO_02:
						{
							auxdio_back_02_t *pBack_02 = (auxdio_back_02_t *) pAutoRes;
							if(auxdio_8150->back_02.source != pBack_02->source)
							{
								//bSceneFlag = true;
							}					
                			auxdio_8150->back_02 = *pBack_02;
               				break;
            			}
					case BACK_AUXDIO_03:
						{
             		   		auxdio_8150->back_03 = *((auxdio_back_03_t *) pAutoRes);
              		  		break;
						}
           			case BACK_AUXDIO_04:
              		 	{
                			auxdio_8150->back_04 = *((auxdio_back_04_t *) pAutoRes);
                			break;
						}
           			case BACK_AUXDIO_05:
               			{
                			auxdio_8150->back_05 = *((auxdio_back_05_t *) pAutoRes);
                			break;
						}
					}

					break;
            	}
			case YODAR_4028B:
				{
					yodar_4028b_state * yodar_state = (yodar_4028b_state *) curr_state;
					general_single_state *pGeneral = (general_single_state *) pAutoRes;
					
					switch(pGeneral->type)
					{
					case BACK_SWITCH:
                		yodar_state->on_off = pGeneral->value;
						break;
					case BACK_PLAY:
                		yodar_state->play_pause = pGeneral->value;
						break;
					case BACK_VOLUME:
                		yodar_state->volume= pGeneral->value;
						break;
					case BACK_SOURCE:
                		yodar_state->source= pGeneral->value;
						break;
					case BACK_EQ:
                		yodar_state->eq= pGeneral->value;
						break;
					case BACK_MUTE:
                		yodar_state->mute= pGeneral->value;
						break;
					case BACK_SONG_NAME:
						general_string_state *pString = (general_string_state *) pAutoRes;
                		strncpy((char *) yodar_state->song_name, (char *)pString->value, pString->len < 255 ? pString->len : 255);
						break;
					}
				}
				break;
			case AQM_1000:
				{
					*((aqm_1000_state *) curr_state) = *((aqm_1000_state *) pAutoRes);
				}
				break;
			default:
				if(pAutoRes != NULL)
				{
					delete pAutoRes;
					pAutoRes = NULL;
				}
				return ;
			}
			nRes = db_set_serial_device_curr(nDevId, nComDevType, nArea, (char *) curr_state);

			/*
			// try to trigger scene. not used.
			if(bSceneFlag == true)
			{
				if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_GATEWAY)
				{
					pGatewayMan->g_pSockerServer->SendToHost((char *) pAutoRes, nMsgLen);
				}
				else if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST && pGatewayMan->g_comLogical[nComId] != NULL)
				{
					pGatewayMan->g_comLogical[nComId]->ToAddSceneInputNode((kic_head_t *) pAutoRes);
				}
			}
			*/

			if(pAutoRes != NULL)
			{
				delete pAutoRes;
				pAutoRes = NULL;
			}
		}
	}
	
	int CGatewayManager::InitAllCom()
	{
		memset(g_com_infos, 0, sizeof(com_info_t) * TOTAL_COM_NUM);
		if (TOTAL_COM_NUM < 1)
		{
			printf("TOTAL_COM_NUM:%d, should 1 or more.\n", TOTAL_COM_NUM);
			return ERR_PROTROL_PARAMETERS;
		}

		LIST_HEAD(all_serials);
		ts_host_interface_serial_info_list *pSerialList = NULL;
		int nCount = -1;
		int nRes = db_get_all_serial_details(&all_serials, pSerialList, &nCount);
		if(nRes != EXIT_SUCCESS)
		{
			printf("Failed Start Serials when get_all_serial_details() errorno = %d.\n", nRes);
			return nRes;
		}

		if(TOTAL_COM_NUM != nCount)
		{
			printf("Serial count is %d in db, but it's max count is %d.\n", nCount, TOTAL_COM_NUM);
		}

		pSerialList = NULL;
		ts_host_interface_serial_info_list *pSerialListTmp = NULL;
		g_com_count = TOTAL_COM_NUM > nCount ? nCount : TOTAL_COM_NUM;
		int iCount = 0;
		list_for_each_entry_safe(pSerialList, pSerialListTmp, &all_serials, list)
		{
			if(iCount > g_com_count -1)
			{
				break;
			}

			if(pSerialList == NULL)
			{
				continue;
			}

			// copy com name to g_com_infos
			int nLen = strlen(pSerialList->host_serial.comm_port);
			g_com_infos[iCount].id = pSerialList->host_serial.id;
			memcpy(g_com_infos[iCount].com_name, pSerialList->host_serial.comm_port,
					nLen > COM_NAME_LENGTH ? COM_NAME_LENGTH : nLen);
			g_com_infos[iCount].baud = pSerialList->host_serial.baud_rate;
			g_com_infos[iCount].data_bits = pSerialList->host_serial.data_bit;
			g_com_infos[iCount].parity = pSerialList->host_serial.parity_check;
			float fStopBit = pSerialList->host_serial.stop_bit;
			if(fStopBit != 1 && fStopBit != 2)
			{
				g_com_infos[iCount].stop_bits = 15;
			}
			else
			{
				g_com_infos[iCount].stop_bits = (int) fStopBit;
			}

			iCount++;
		}

		list_for_each_entry_safe(pSerialList, pSerialListTmp, &all_serials, list)
		{
			if(pSerialList == NULL)
			{
				list_del(&pSerialList->list);
				free(pSerialList);
			}
		}
		
		return SUCCESS;
	}

	void CGatewayManager::StartComManager()
	{
		if(InitAllCom() != SUCCESS)
		{
            printf("init com ports failed.\n");
            return ;
		}

		ezserial_init();

		int nIndex = 0;
		int nFailedCount = 0;
		char enoceanComName[COM_NAME_LENGTH] = {0};
		
		for(nIndex = 0; nIndex < g_com_count; nIndex++)
		{
			__fline printf("(name=%s, id=%d, baud=%d, data_bits=%d, parity=%d, stop_bits=%d)\n",
					g_com_infos[nIndex].com_name,
					g_com_infos[nIndex].id,
					g_com_infos[nIndex].baud,
					g_com_infos[nIndex].data_bits,
					g_com_infos[nIndex].parity,
					g_com_infos[nIndex].stop_bits);

			if(g_com_infos[nIndex].id == PROTOCOL_ENOCEAN_SERIAL_DB_ID)
			{
				// drop over the enocean port.
				strncpy(enoceanComName, g_com_infos[nIndex].com_name, COM_NAME_LENGTH);
				continue;
			}

			g_pSerial[nIndex] = ezserial_open(
					g_com_infos[nIndex].com_name,
					CB_ComRecvedData, g_com_infos[nIndex].id,
					g_com_infos[nIndex].baud,
					g_com_infos[nIndex].data_bits,
					g_com_infos[nIndex].parity,
					g_com_infos[nIndex].stop_bits);

			if(g_pSerial[nIndex] == NULL)
			{
				nFailedCount++;
			}
			else
			{
				g_pSerial[nIndex]->id = g_com_infos[nIndex].id;
				g_comLogical[nIndex].StartSendThread();
			}
		}

		if(nFailedCount > 0)
		{
			printf("(%d/%d) com ports open failed.\n", nFailedCount, g_com_count);
		}

        /* start enocean manager, modified by keyy on 2012/09/17 */
#ifdef DEV_ENOCEAN_SUPPORT
		if(strlen((char *)enoceanComName) <= 3)
		{
			strcpy(enoceanComName, DEFAULT_ENOCEAN_DEVICE_INTERFACE);
		}
		
strcpy(enoceanComName, DEFAULT_ENOCEAN_DEVICE_INTERFACE);

        enocean_uart_init(enoceanComName);
        enocean_uart_callback(EnOceanCallBack);
		enocean_uart_open();
		m_EnoceanLogic.StartSendThread();
#endif
		/* enocean end */
	}

	void CGatewayManager::EndComManager()
	{
		int nIndex = 0;

		for(nIndex = 0; nIndex < g_com_count; nIndex++)
		{
			if(g_pSerial[nIndex] != NULL)
			{
				ezserial_close(g_pSerial[nIndex]);
				g_pSerial[nIndex] = NULL;
			}
			g_comLogical[nIndex].StopSendThread();
		}
		
        /* end enocean manager, modified by keyy on 2012/09/17 */
#ifdef DEV_ENOCEAN_SUPPORT
		m_EnoceanLogic.StopSendThread();

		enocean_uart_close();
    	ezserial_cleanup();
#endif
        /* enocean end */
		ezserial_cleanup();

		CHARP_COMSOSTRUCT_MAP_ITER iter = m_mapSoStruct.begin();
		while(iter != m_mapSoStruct.end())
		{
			if(iter->second)
			{
				if(iter->second->handler)
				{
					if(iter->second->pDeleteInstance)
					{
						iter->second->pDeleteInstance(iter->second->pInstance);
					}
					dlclose(iter->second->handler);
				}
				delete iter->second;
			}
			iter++;
		}
		m_mapSoStruct.clear();
	}

	int CGatewayManager::IsOpenComI(int *on_off, int index)
	{
		if(on_off == NULL)
		{
			return FAILED;
		}

		// query enocean serial port status
		if(index == PROTOCOL_ENOCEAN_SERIAL_DB_ID)
		{
#ifdef DEV_ENOCEAN_SUPPORT
			int nRes = enocean_uart_status();
			if(nRes == 0)
			{
				*on_off = SWITCH_ON;
			}
			else
			{
				*on_off = SWITCH_OFF;
			}
#endif
			*on_off = SWITCH_OFF;
			return SUCCESS;
		}
	
		int nComId = -1;
		int nIndex = 0;

		// query another serial port status
		for(nIndex = 0; nIndex < g_com_count; nIndex++)
		{
			if(g_com_infos[nIndex].id == index)
			{
				nComId = nIndex;
				break;
			}
		}
	
		if(nComId < 0)
		{
			return ERR_PROTROL_NO_DEVICE;
		}
		
		if(g_pSerial[nIndex] == NULL)
		{
			*on_off = SWITCH_OFF;
		}
		else
		{
			*on_off = SWITCH_ON;
		}
		return SUCCESS;
	}	
	
	int CGatewayManager::OpenComI(int on_off, int index)
	{
		// operate enocean serial port
		if(index == PROTOCOL_ENOCEAN_SERIAL_DB_ID)
		{
#ifdef DEV_ENOCEAN_SUPPORT
			int nRes = enocean_uart_status();
			if(nRes == 0)
			{
				if( on_off == SWITCH_OFF)
				{
					m_EnoceanLogic.StopSendThread();
					enocean_uart_close();
				}
			}
			else if(on_off == SWITCH_ON)
			{
				char enoceanComName[COM_NAME_LENGTH] = {0};
				int nIndex = 0;
				//todo : to get the enocean serial port name by PROTOCOL_ENOCEAN_SERIAL_DB_ID
				for(nIndex = 0; nIndex < g_com_count; nIndex++)
				{
					if(g_com_infos[nIndex].id == index)
					{
						strncpy((char *)enoceanComName, g_com_infos[nIndex].com_name, COM_NAME_LENGTH);
						break;
					}
				}

				if(strlen((char *)enoceanComName) <= 3)
				{
					strcpy(enoceanComName, DEFAULT_ENOCEAN_DEVICE_INTERFACE);
				}
		
       			enocean_uart_init(enoceanComName);
        		enocean_uart_callback(EnOceanCallBack);
				enocean_uart_open();
				m_EnoceanLogic.StartSendThread();
			}
#endif
			return SUCCESS;
		}
	
		int nComId = -1;
		int nIndex = 0;
		for(nIndex = 0; nIndex < g_com_count; nIndex++)
		{
			if(g_com_infos[nIndex].id == index)
			{
				nComId = nIndex;
				break;
			}
		}
		
		if(nComId < 0)
		{
			return ERR_PROTROL_NO_DEVICE;
		}
		
		if(on_off == SWITCH_ON)
		{
			g_pSerial[nIndex] = ezserial_open(
										   g_com_infos[nIndex].com_name,
										   CB_ComRecvedData, g_com_infos[nIndex].id,
										   g_com_infos[nIndex].baud,
										   g_com_infos[nIndex].data_bits,
										   g_com_infos[nIndex].parity,
										   g_com_infos[nIndex].stop_bits);
			
			if(g_pSerial[nIndex] == NULL)
			{
				return ERR_PROTROL_CONNECT_FAILED;
			}
			else
			{
				g_pSerial[nIndex]->id = g_com_infos[nIndex].id;
				g_comLogical[nIndex].StartSendThread();
				return SUCCESS;
			}
		}
		else
		{
			if(g_pSerial[nIndex] != NULL)
			{
				ezserial_close(g_pSerial[nIndex]);
				g_pSerial[nIndex] = NULL;
			}
			g_comLogical[nIndex].StopSendThread();
			return SUCCESS;
		}
	}

	void CGatewayManager::OpenInfraredDev()
	{
		if(g_irDeviceHandler == -1)
		{
			char irDriverName[32] = {0};
			int nRes = db_get_infr_driver( (char *) irDriverName);
			if(nRes != EXIT_SUCCESS || strlen(irDriverName) <= 3)
			{
				strcpy(irDriverName, "/dev/IR_driver0");
			}
			
			g_irDeviceHandler = open(irDriverName, O_RDWR);
			if(g_irDeviceHandler < 0)
			{
				printf("Infrared device [%s] open failed.\n", irDriverName);
			}
		}
	}

	void CGatewayManager::StartInfraredManager()
	{
		OpenInfraredDev();
		g_irLogical.StartSendThread();
	}

	int CGatewayManager::GetInfraredHandler()
	{
		// if didn't open, reopen it.
		OpenInfraredDev();
		return g_irDeviceHandler;
	}

	void CGatewayManager::EndInfraredManager()
	{
		g_irLogical.StopSendThread();
		if(g_irDeviceHandler != -1)
		{
			close(g_irDeviceHandler);
			g_irDeviceHandler = -1;
		}
	}

	void CGatewayManager::SetLogPath(const char* szLogPath)
	{
#ifdef LOG_TO_FILE
		g_log = fopen( szLogPath, "w" );
		setbuf (g_log, NULL);
#endif
	}
	
	int KnxThread(void *pParam)
	{
		__fline printf("KnxThread   start\n");
		CGatewayManager *pMan = (CGatewayManager *) pParam;
		TimerStruct* pKnxTimer = pMan->GetReconnKnxCond();
		int nRescanInterval = KNX_RECONNECT_INTERVAL_GEN;

		while(pMan->IsRunning())
		{
			// waiting for re-connect signal, or timeout in 2 minutes.
			//int nRes = 0;
			LOCK_MUTEX(&pKnxTimer->timerMutex); // protect knx thread condition
#ifdef _WIN32
			pKnxTimer->timeout = nRescanInterval * 1000;
			WaitForSingleObject(pKnxTimer->timerCond, pKnxTimer->timeout);
#else
			pKnxTimer->timeout.tv_sec = time(0) + nRescanInterval;
			pKnxTimer->timeout.tv_nsec = 0;

			pthread_cond_timedwait(&pKnxTimer->timerCond,
				&pKnxTimer->timerMutex, &pKnxTimer->timeout);
#endif
			UNLOCK_MUTEX(&pKnxTimer->timerMutex);

			if(pMan->IsRunning() == false)
			{
				// exit thread
				break;
			}

            DEBUG_LOG0(GatewayManager, ========== Check connection to KNX gateway\n);
            
			// check KNX connection
			bool bAllConnected = true;

			KNX_GATEWAY_MAP_ITER iter = pMan->GetKnxGatewayMap()->begin();
			for (; iter != pMan->GetKnxGatewayMap()->end() && pMan->IsRunning(); iter ++)
			{
				if(!iter->second->m_udpCtrl.IsRunning() && iter->second->IsAutoConnect())
				{
					try
					{
						iter->second->UDPSocketInit();
						if(pMan->GetKnxConn() != NULL)
						{
							pMan->GetKnxConn()(iter->second->GetGatewayKey(), E_NO_ERROR, KNX_ERR_NO, NULL);
						}
					}
					catch (CKNXException *e)
					{
						if(pMan->GetKnxConn() != NULL)
						{
							pMan->GetKnxConn()(iter->second->GetGatewayKey(), E_NO_ERROR,
								e->GetExceptionId(), e->GetExceptionString());
						}
						delete e;
						bAllConnected = false;
					}
				}
			}

			// Set rescan interval
			if (bAllConnected)
			{
				nRescanInterval = KNX_RECONNECT_INTERVAL_GEN;
			}
			else
			{
				nRescanInterval = KNX_RECONNECT_INTERVAL_NOTIFY; // if reconnect failed, do it in 5 seconds later
			}
		}
		
		__fline printf("KnxThread   Ended\n");
		return 0;
	}

	int WifiThread(void *pParam)
	{
        __fline printf("wifi thread begin.\n");

        CGatewayManager *pGatewayMan = (CGatewayManager *) pParam;

        while (pGatewayMan->IsRunning())
        {
            if (!pGatewayMan->g_TcpSocketHandler.GetCount())
            {
//                __fline;
//                printf("g_TcpSocketHandler.GetCount() = %d.\n", pGatewayMan->g_TcpSocketHandler.GetCount());

                sleep(1);
                continue;
            }

            pGatewayMan->g_TcpSocketHandler.Select(1,0);
        }

        __fline printf("wifi thread end.\n");
        return 0;
	}
	
	int KnxReadThread(void *pParam)
	{
		__fline printf("Knx Read Thread  start\n");
		CGatewayManager *pGatewayMan = (CGatewayManager *) pParam;
		TimerStruct *pTimer = pGatewayMan->GetKnxReadCondStruct();

		int nCurKnxReadGrpId = 0;
		int nGatewayId = 0;
		short nGrpAddr = 0;
		CKNXKernel *pKnx = NULL;
		int nRes = 0;

		while (pGatewayMan->IsRunning())
		{
			pTimer->timeout.tv_sec = time(0) + 3;
			pTimer->timeout.tv_nsec = 0;
			pthread_cond_timedwait(
					&pTimer->timerCond,
					&pTimer->timerMutex,
					&pTimer->timeout);

			if(!pGatewayMan->IsRunning())
			{
				// exit thread
				break;
			}

			// add a interface in ts_db_tools
			nRes = db_get_next_readable_group(&nCurKnxReadGrpId, &nGatewayId, &nGrpAddr);

			if(nRes != SUCCESS)
			{
				continue;
			}

			pKnx = (CKNXKernel *) pGatewayMan->GetGatewayByKey(GATEWAY_TYPE_KNX, nGatewayId);
			if(pKnx != NULL && pKnx->m_udpCtrl.IsRunning())
			{
				try
				{
					pKnx->ReadGrpVal(nGrpAddr);
				}
				catch(CKNXException *e)
				{
					delete e;
					continue;
				}
			}
		}

		__fline printf("Knx Read Thread  Ended\n");
		return 0;
	}

	void CGatewayManager::WaitKnxReadDb()
	{
		m_KnxReadDbCondStruct.timeout.tv_sec = time(0) + 3;
		m_KnxReadDbCondStruct.timeout.tv_nsec = 0;
		LOCK_MUTEX(&m_KnxReadDbCondStruct.timerMutex);
		pthread_cond_timedwait(
				&m_KnxReadDbCondStruct.timerCond,
				&m_KnxReadDbCondStruct.timerMutex,
				&m_KnxReadDbCondStruct.timeout);
		UNLOCK_MUTEX(&m_KnxReadDbCondStruct.timerMutex);
	}

	void CGatewayManager::KnxReadDbListPro()
	{
		SendNode *pNode = NULL;
		knx_control_report_t *pReqData = NULL;
		ts_knx_control_info_t knxData;
		int nRes = 0;

		while(IsRunning())
		{
			pNode = (SendNode *) m_knxReadDbList.GetAndSeperateHeader();
			if(pNode != NULL)
			{
				// 1. get group value from db
				pReqData = (knx_control_report_t *) pNode->buf;
				memset(&knxData, 0, sizeof(ts_knx_control_info_t));
				knxData.home_device_id = ntohl(pReqData->kic_head.home_device_id);
				knxData.group_addr = ntohs(pReqData->address);
				knxData.data_type = pReqData->data_type;

				// get group value from (home_dev_id && group_address)
				nRes = db_get_knx_device_curr_value(&knxData);

				__fline printf("get knx value nRes = %d, [(%d, %d, %d) : %s]\n",
						nRes,
						(knxData.group_addr >> 11) & 0x0F,
						(knxData.group_addr >> 8) & 0x07,
						knxData.group_addr & 0xFF,
						(char *) knxData.data);

				GetValue_DbBuf2KnxBuf(&knxData);

				// copy the group value to buf
				pReqData->result_code = nRes;
				if(nRes == 0)
				{
					memcpy(pReqData->data, knxData.data, TXT_DATA_SIZE);
				}

				// 3. call m_pCtrlResponse
				if(m_pCtrlResponse != NULL)
				{
					m_pCtrlResponse(ACTION_KNX_READ_RES, pNode->buf, sizeof(knx_control_report_t));
				}

				delete pNode;

				usleep(100 * 1000);
			}
			else
			{
				break ;
			}
		}
	}

	int KnxReadDbThread(void *pParam)
	{
		__fline printf("Knx Read Db Thread start\n");
		CGatewayManager *pGatewayMan = (CGatewayManager *) pParam;

		while(pGatewayMan->IsRunning())
		{
			pGatewayMan->WaitKnxReadDb();
			
			if(!pGatewayMan->IsRunning())
			{
				// exit thread
				break;
			}
			pGatewayMan->KnxReadDbListPro();
		}

		__fline printf("Knx Read Db Thread Ended\n");
		return 0;
	}

	void CGatewayManager::StopGatewayManager()
	{
		m_bRunning = false;

		DisconnectAll();

		sleep(2);

		if (m_nKnxHandle > 0)
		{
#ifdef _WIN32
			SET_SIGNAL(m_reconnKnx.timerCond);
			WAIT_THREAD_EXIT(m_nKnxHandle);
			CLOSE_THREAD(m_nKnxHandle);
#else
			LOCK_MUTEX(&m_reconnKnx.timerMutex);
			SET_SIGNAL(m_reconnKnx.timerCond);
			UNLOCK_MUTEX(&m_reconnKnx.timerMutex);
			WAIT_THREAD_EXIT(m_nKnxHandle);
#endif
			m_nKnxHandle = 0;
		}

		if(m_nWifiHandle > 0)
		{
			WAIT_THREAD_EXIT(m_nWifiHandle);
			m_nWifiHandle = 0;
		}

		if(m_nKnxReadThread > 0)
		{
			WAIT_THREAD_EXIT(m_nKnxReadThread);
			m_nKnxReadThread = 0;
		}

		m_knxReadDbList.DeleteAll();
		if(m_nKnxReadDbThread > 0)
		{
			WAIT_THREAD_EXIT(m_nKnxReadDbThread);
			m_nKnxReadDbThread = 0;
		}

/*
		if (m_nWifiHandle > 0)
		{
#ifdef _WIN32
			WAIT_THREAD_EXIT(m_nWifiHandle);
			CLOSE_THREAD(m_nWifiHandle);
#else
			pthread_cancel(m_nWifiHandle);
			WAIT_THREAD_EXIT(m_nWifiHandle);
#endif
			m_nWifiHandle = 0;
		}

		if(m_nKnxReadThread > 0)
		{
#ifdef _WIN32
			WAIT_THREAD_EXIT(m_nKnxReadThread);
			CLOSE_THREAD(m_nKnxReadThread);
#else
			pthread_cancel(m_nKnxReadThread);
			WAIT_THREAD_EXIT(m_nKnxReadThread);
#endif
			m_nKnxReadThread = 0;
		}

		m_knxReadDbList.DeleteAll();

		if(m_nKnxReadDbThread > 0)
		{
#ifdef _WIN32
			SET_SIGNAL(m_KnxReadDbCondStruct.timerCond);
			WAIT_THREAD_EXIT(m_nKnxReadDbThread);
			CLOSE_THREAD(m_nKnxReadDbThread);
#else
			LOCK_MUTEX(&m_KnxReadDbCondStruct.timerMutex);
			SET_SIGNAL(m_KnxReadDbCondStruct.timerCond);
			UNLOCK_MUTEX(&m_KnxReadDbCondStruct.timerMutex);
			pthread_cancel(m_nKnxReadDbThread);
			WAIT_THREAD_EXIT(m_nKnxReadDbThread);
#endif
			m_nKnxReadDbThread = 0;
		}
*/
		
		m_sceneMan.StopSceneThread();

		if(m_pServerLogical != NULL)
		{
			// g_serverLogical.StopSendThread() in ~CICLogical()
			delete m_pServerLogical;
			m_pServerLogical = NULL;
		}

		EndComManager();
		EndInfraredManager();
	}

	void CGatewayManager::SetWifiCallbacks(OnWifiConnect connFun,
		OnWifiDisconnect disconnFun)
	{
		m_onWifiConn = connFun;
		m_onWifiDisconn = disconnFun;
	}

	void CGatewayManager::SetKnxCallbacks(
							  GetKnxDpt pGetKnxDpt,
							  OnSearchResponse pSearchResponse,
							  OnReadResponse pReadResponse,
							  OnWriteResponse pWriteResponse,
							  OnKnxConnect pKnxConnect,
							  OnKnxDisconnect pKnxDisconnect,
							  OnRWError pErrorFun)
	{
		m_pGetKnxDpt = pGetKnxDpt;
		m_pSearchResponse = pSearchResponse;
		m_pReadResponse = pReadResponse;
		m_pWriteResponse = pWriteResponse;
		m_pKnxConnect = pKnxConnect;
		m_pKnxDisconnect = pKnxDisconnect;
		m_pErrorFun = pErrorFun;
		
		// set the multicast response
		m_pKnx->SetSearchResponse(pSearchResponse);
	}

	bool CGatewayManager::AddGateway(int nGatewayType, int nKey, char *localIp, char* szIP, int nPort)
	{
		if(nGatewayType == GATEWAY_TYPE_KNX)
		{
			CKNXKernel* pKernel = new CKNXKernel(nKey, localIp, szIP, nPort, this);
			pKernel->SetCallBacks(
					m_pGetKnxDpt,
					m_pSearchResponse,
					m_pReadResponse,
					m_pWriteResponse,
					m_pKnxConnect,
					m_pKnxDisconnect,
					m_pErrorFun);
			pKernel->SetReConnCallBack(NotifyKnxManagerInStack);

			LOCK_MUTEX(&m_knxGatewayMutex);
			std::pair<KNX_GATEWAY_MAP_ITER, bool> ret = m_knxGateways.insert(KNX_GATEWAY_PAIR(nKey, pKernel));

			if(ret.second == false) // already exists
			{
				UNLOCK_MUTEX(&m_knxGatewayMutex);
				return false;
				//m_knxGateways.erase(nKey);
				//m_knxGateways.insert(KNX_GATEWAY_PAIR(nKey, pKernel));
			}
			UNLOCK_MUTEX(&m_knxGatewayMutex);
		}
		else
		{
			// add wifi gateway to map
			CWifiKernel* pWifi = new CWifiKernel();
			pWifi->nIndex = nKey;
			memcpy(pWifi->strIP, szIP, 32);
			pWifi->Port = nPort;
			pWifi->IsUse = 0;

			LOCK_MUTEX(&m_wifiGatewayMutex);
			std::pair<WIFI_GATEWAY_MAP_ITER, bool> ret = m_wifiGateways.insert(WIFI_GATEWAY_PAIR(nKey, pWifi));

			if(ret.second == false) // already exists
			{
				UNLOCK_MUTEX(&m_wifiGatewayMutex);
				return false;
				//m_wifiGateways.erase(nKey);
				//m_wifiGateways.insert(WIFI_GATEWAY_PAIR(nKey, pKernel));
			}
			UNLOCK_MUTEX(&m_wifiGatewayMutex);
		}
		return true;
	}
	
	void CGatewayManager::RemoveGateway(int nGatewayType, int nKey)
	{
		if(nGatewayType == GATEWAY_TYPE_KNX)
		{
			LOCK_MUTEX(&m_knxGatewayMutex);
			KNX_GATEWAY_MAP_ITER iter = m_knxGateways.find(nKey);
			if (m_knxGateways.end() != iter)
			{
				iter->second->ToDisconnect(true, false, true);
				iter->second->WaitForAllThreadsExit();

				delete iter->second;
				m_knxGateways.erase(nKey);
			}
			UNLOCK_MUTEX(&m_knxGatewayMutex);
		}
		else
		{
			LOCK_MUTEX(&m_wifiGatewayMutex);
			WIFI_GATEWAY_MAP_ITER iter = m_wifiGateways.find(nKey);
			if (m_wifiGateways.end() != iter)
			{
				if(iter->second->pConnector != NULL)
				{
					iter->second->pConnector->CloseAndDelete();
				}
				delete iter->second;
				m_wifiGateways.erase(nKey);
			}
			UNLOCK_MUTEX(&m_wifiGatewayMutex);
		}
	}

	void CGatewayManager::ConnectAll()
	{
		THREAD_HANDLE handle = CREATE_THREAD(ConnectAllThread, this);	
		DETACH_THREAD(handle);
	}
	
	int ConnectAllThread(void *pParam)
	{
		CGatewayManager *pMan = (CGatewayManager *) pParam;
		pMan->ConnectAllWifiSync();
		pMan->ConnectAllKnxSync();
		return 0;
	}
	
	void CGatewayManager::ConnectAllKnxSync()
	{
		LOCK_MUTEX(&m_knxGatewayMutex);
		KNX_GATEWAY_MAP_ITER iter = m_knxGateways.begin();
		for (; iter != m_knxGateways.end(); iter ++)
		{
			try
			{
				iter->second->UDPSocketInit();
				if(m_pKnxConnect != NULL)
				{
					m_pKnxConnect(iter->second->GetGatewayKey(), E_NO_ERROR, KNX_ERR_NO, NULL);
				}
			}
			catch (CKNXException *e)
			{
				if(m_pKnxConnect != NULL)
				{
					m_pKnxConnect(iter->second->GetGatewayKey(), E_NO_ERROR,
						e->GetExceptionId(), e->GetExceptionString());
				}
				delete e;
			}
		}
		UNLOCK_MUTEX(&m_knxGatewayMutex);
	}

	void CGatewayManager::ConnectAllWifiSync()
	{
		LOCK_MUTEX(&m_wifiGatewayMutex);
		WIFI_GATEWAY_MAP_ITER iter = m_wifiGateways.begin();
		for (; iter != m_wifiGateways.end(); iter ++)
		{
			if(iter->second->pConnector != NULL)
			{
				iter->second->pConnector->CloseAndDelete();
			}
			iter->second->pConnector = new CGeneralAgentTcpSocketConnector(g_TcpSocketHandler);
			iter->second->SetCallbacks(m_onWifiConn, m_onWifiDisconn, m_pCtrlResponse);
			iter->second->pConnector->SetWifiPtr(iter->second);
			bool bOK = iter->second->pConnector->Open(
					iter->second->strIP, iter->second->Port);
			if(bOK)
			{
				g_TcpSocketHandler.Add(iter->second->pConnector);
				bOK = iter->second->pConnector->Connecting();

				__fline;
				printf("try to connect (%s:%d)\n",
						iter->second->strIP, iter->second->Port);
			}
		}
		UNLOCK_MUTEX(&m_wifiGatewayMutex);
	}

	void CGatewayManager::ConnectGateway(int nGatewayType, int nKey)
	{
		//todo: need be locked.
		m_nConnectKey = nKey;
		
		THREAD_HANDLE handle = 0;
		if(nGatewayType == GATEWAY_TYPE_KNX)
		{
			handle = CREATE_THREAD(ConnectKnxThread, this);
		}
		else
		{
			handle = CREATE_THREAD(ConnectWifiThread, this);
		}
		DETACH_THREAD(handle);
	}
	
	int ConnectKnxThread(void *pParam)
	{
		CGatewayManager *pMan = (CGatewayManager *) pParam;
		int nRes = 0;
		try
		{
			nRes = pMan->ConnectGatewaySync(GATEWAY_TYPE_KNX, pMan->GetConnectKey());
			if(pMan->GetKnxConn() != NULL)
			{
				pMan->GetKnxConn()(pMan->GetConnectKey(), E_NO_ERROR, KNX_ERR_NO, NULL);
			}
		}
		catch (CKNXException* e)
		{
			if(pMan->GetKnxConn() != NULL)
			{
				pMan->GetKnxConn()(pMan->GetConnectKey(), E_NO_ERROR,
					e->GetExceptionId(), e->GetExceptionString());
			}
			nRes = e->GetExceptionId();
			delete e;
			return nRes;
		}

		return nRes;
	}

	int ConnectWifiThread(void *pParam)
	{
		CGatewayManager *pGatewayMan = (CGatewayManager *) pParam;
		pGatewayMan->ConnectGatewaySync(GATEWAY_TYPE_WIFI, pGatewayMan->GetConnectKey());
		return 0;
	}

	int CGatewayManager::ConnectGatewaySync(int nGatewayType, int nKey)
	{
		int nRes = 0;
		if(nGatewayType == GATEWAY_TYPE_KNX)
		{
			LOCK_MUTEX(&m_knxGatewayMutex);
			KNX_GATEWAY_MAP_ITER iter = m_knxGateways.find(nKey);
			if (m_knxGateways.end() != iter)
			{
				// connect
				try
				{
					nRes = iter->second->UDPSocketInit();
				}
				catch (CKNXException * e)
				{
					UNLOCK_MUTEX(&m_knxGatewayMutex);
					nRes = e->GetExceptionId();
					delete e;
					return nRes;
				}
			}
			UNLOCK_MUTEX(&m_knxGatewayMutex);
		}
		else
		{
			LOCK_MUTEX(&m_wifiGatewayMutex);
			WIFI_GATEWAY_MAP_ITER iter = m_wifiGateways.find(nKey);
			if (m_wifiGateways.end() != iter)
			{
				if(iter->second->pConnector != NULL)
				{
					iter->second->pConnector->CloseAndDelete();
				}
				iter->second->pConnector = new CGeneralAgentTcpSocketConnector(g_TcpSocketHandler);
				iter->second->SetCallbacks(m_onWifiConn, m_onWifiDisconn, m_pCtrlResponse);
				iter->second->pConnector->SetWifiPtr(iter->second);
				bool bOK = iter->second->pConnector->Open(
						iter->second->strIP, iter->second->Port);
				if(bOK)
				{
					g_TcpSocketHandler.Add(iter->second->pConnector);
					bOK = iter->second->pConnector->Connecting();

					__fline;
					printf("connect to (%s:%d) %s\n",
							iter->second->strIP, iter->second->Port,
							bOK ? "OK" : "Failed");
				}
			}
			UNLOCK_MUTEX(&m_wifiGatewayMutex);
		}

		return nRes;
	}

	void CGatewayManager::DisconnectAll()
	{
		DisconnectAllWifi();
		DisconnectAllKnx();
	}

	void CGatewayManager::DisconnectAllKnx()
	{
		LOCK_MUTEX(&m_knxGatewayMutex);
		KNX_GATEWAY_MAP_ITER iter = m_knxGateways.begin();
		for (; iter != m_knxGateways.end(); iter ++)
		{
			iter->second->ToDisconnect();
			if(m_pKnxDisconnect != NULL)
			{
				m_pKnxDisconnect(iter->second->GetGatewayKey());
			}
		}
		UNLOCK_MUTEX(&m_knxGatewayMutex);
	}

	void CGatewayManager::DisconnectAllWifi()
	{
		LOCK_MUTEX(&m_wifiGatewayMutex);
		WIFI_GATEWAY_MAP_ITER iter = m_wifiGateways.begin();
		for (; iter != m_wifiGateways.end(); iter ++)
		{
			if(iter->second->pConnector != NULL)
			{
				iter->second->pConnector->CloseAndDelete();
			}
		}
		UNLOCK_MUTEX(&m_wifiGatewayMutex);
	}

	void CGatewayManager::DisconnectGateway(int nGatewayType, int nKey)
	{
		if(nGatewayType == GATEWAY_TYPE_KNX)
		{
			LOCK_MUTEX(&m_knxGatewayMutex);
			KNX_GATEWAY_MAP_ITER iter = m_knxGateways.find(nKey);
			if (m_knxGateways.end() != iter)
			{
				// disconnect
				iter->second->ToDisconnect();

				if(m_pKnxDisconnect != NULL)
				{
					m_pKnxDisconnect(iter->second->GetGatewayKey());
				}
			}
			UNLOCK_MUTEX(&m_knxGatewayMutex);
		}
		else
		{
			LOCK_MUTEX(&m_wifiGatewayMutex);
			WIFI_GATEWAY_MAP_ITER iter = m_wifiGateways.find(nKey);
			if (m_wifiGateways.end() != iter)
			{
				// disconnect
				if(iter->second->pConnector != NULL)
				{
					iter->second->pConnector->CloseAndDelete();
				}
			}
			UNLOCK_MUTEX(&m_wifiGatewayMutex);
		}
	}

	bool CGatewayManager::IsGatewayConnected(int nGatewayType, int nKey)
	{
		bool bConnected = false;
		if(nGatewayType == GATEWAY_TYPE_KNX)
		{
			LOCK_MUTEX(&m_knxGatewayMutex);
			KNX_GATEWAY_MAP_ITER iter = m_knxGateways.find(nKey);
			if (m_knxGateways.end() != iter)
			{
				bConnected = iter->second->m_udpCtrl.IsRunning();
			}
			UNLOCK_MUTEX(&m_knxGatewayMutex);
		}
		else
		{
			LOCK_MUTEX(&m_wifiGatewayMutex);
			WIFI_GATEWAY_MAP_ITER iter = m_wifiGateways.find(nKey);
			if (m_wifiGateways.end() != iter)
			{
				if(iter->second->pConnector != NULL)
				{
					bConnected = iter->second->pConnector->IsConnected();
				}
			}
			UNLOCK_MUTEX(&m_wifiGatewayMutex);
		}

		return bConnected;
	}
	
	void CGatewayManager::RemoveAllGateways()
	{
		RemoveAllWifiGateways();
		RemoveAllKnxGateways();
	}

	void CGatewayManager::RemoveAllWifiGateways()
	{
		// delete all wifi kernels
		LOCK_MUTEX(&m_wifiGatewayMutex);
		WIFI_GATEWAY_MAP_ITER wifiIter = m_wifiGateways.begin();
		for (; wifiIter != m_wifiGateways.end(); wifiIter ++)
		{
			if(wifiIter->second->pConnector != NULL)
			{
				wifiIter->second->pConnector->CloseAndDelete();
			}
			delete wifiIter->second;
		}
		// remove all
		m_wifiGateways.clear();
		UNLOCK_MUTEX(&m_wifiGatewayMutex);
	}

	void CGatewayManager::RemoveAllKnxGateways()
	{
		LOCK_MUTEX(&m_knxGatewayMutex);
		KNX_GATEWAY_MAP_ITER knxIter = m_knxGateways.begin();
		for (; knxIter != m_knxGateways.end(); knxIter ++)
		{
			knxIter->second->ToDisconnect();
			delete knxIter->second;
		}
		// remove all
		m_knxGateways.clear();
		UNLOCK_MUTEX(&m_knxGatewayMutex);
	}

	void* CGatewayManager::GetGatewayByKey(int nGatewayType, int nKey)
	{
		void *pGateway = NULL;

		if(nGatewayType == GATEWAY_TYPE_KNX)
		{
			LOCK_MUTEX(&m_knxGatewayMutex);
			KNX_GATEWAY_MAP_ITER iter = m_knxGateways.find(nKey);
			if (m_knxGateways.end() != iter)
			{
				pGateway = iter->second;
			}
			UNLOCK_MUTEX(&m_knxGatewayMutex);
		}
		else
		{
			LOCK_MUTEX(&m_wifiGatewayMutex);
			WIFI_GATEWAY_MAP_ITER iter = m_wifiGateways.find(nKey);
			if (m_wifiGateways.end() != iter)
			{
				pGateway = iter->second;
			}
			UNLOCK_MUTEX(&m_wifiGatewayMutex);
		}

		return pGateway;
	}

	int CGatewayManager::GetGatewayKey(int nGatewayType, void *pGateway)
	{
		int nKey = -1;
		if(nGatewayType == GATEWAY_TYPE_KNX)
		{
			nKey = ((CKNXKernel *) pGateway)->GetGatewayKey();
		}
		else
		{
			nKey = ((CWifiKernel *) pGateway)->nIndex;
		}
		return nKey;
	}


	void CGatewayManager::SetServerAddr(int nGatewayType, int nKey, char * serverIp, int serverPort)
	{
		if(nGatewayType == GATEWAY_TYPE_KNX)
		{
			CKNXKernel *pKnx = (CKNXKernel *) GetGatewayByKey(nGatewayType, nKey);
			if(pKnx)
			{
				pKnx->SetServerAddr(serverIp, serverPort);
			}
		}
		else
		{
			CWifiKernel *pWifi = (CWifiKernel *) GetGatewayByKey(nGatewayType, nKey);
			if(pWifi)
			{
				strcpy(pWifi->strIP, serverIp);
				pWifi->Port = serverPort;
			}
		}
	}

	int CGatewayManager::SearchServer(char *strErr)
	{
		if(m_pKnx == NULL)
		{
			m_pKnx = new CKNXKernel(0, NULL, (char *) "127.0.0.1", 3671, this);
			m_pKnx->SetSearchResponse(m_pSearchResponse);
		}

		int nRes = -1;
		try
		{
			nRes = m_pKnx->SearchServer();
		}
		catch(CKNXException *e)
		{
			nRes = e->GetExceptionId();
			strcpy(strErr, e->GetExceptionString());
			delete e;
			return nRes;
		}

		return nRes;
	}

	int CGatewayManager::ReadGrpVal (int nKey, int nGroupAddr)
	{
		CKNXKernel *pKnx = (CKNXKernel *) GetGatewayByKey(GATEWAY_TYPE_KNX, nKey);
		int nRes = -1;
		if(pKnx)
		{
			try
			{
				nRes = pKnx->ReadGrpVal(nGroupAddr);
			}
			catch(CKNXException *e)
			{
				nRes = e->GetExceptionId();
				delete e;
			}
		}
		return nRes;
	}
	
	void CGatewayManager::ReadGrpVal(int nGroupAddr)
	{
		KNX_GATEWAY_MAP_ITER iter;
		LOCK_MUTEX(&m_knxGatewayMutex);
		iter = m_knxGateways.begin();
		for (; iter != m_knxGateways.end(); iter++)
		{
			try
			{
				iter->second->ReadGrpVal(nGroupAddr);
			}
			catch(CKNXException *e)
			{
				delete e;
			}
		}
		UNLOCK_MUTEX(&m_knxGatewayMutex);
	}

	int CGatewayManager::WriteGrpVal (int nKey, void *pBuf, int nTotalLen, int nPrefixLen)
	{
		CKNXKernel *pKnx = (CKNXKernel *) GetGatewayByKey(GATEWAY_TYPE_KNX, nKey);
		int nRes = -1;
		if(pKnx)
		{
			try
			{
				nRes = pKnx->WriteGrpVal(pBuf, nTotalLen, nPrefixLen);
			}
			catch(CKNXException *e)
			{
				nRes = e->GetExceptionId();
				delete e;
			}
		}
		return nRes;
	}
	
	void CGatewayManager::WriteGrpVal (void *pBuf, int nTotalLen, int nPrefixLen)
	{
		KNX_GATEWAY_MAP_ITER iter;
		LOCK_MUTEX(&m_knxGatewayMutex);
		iter = m_knxGateways.begin();
		for (; iter != m_knxGateways.end(); iter++)
		{
			try
			{
				iter->second->WriteGrpVal(pBuf, nTotalLen, nPrefixLen);
			}
			catch(CKNXException *e)
			{
				delete e;
			}
		}
		UNLOCK_MUTEX(&m_knxGatewayMutex);
	}

	int CGatewayManager::AddToKnxReadDbList(int nGatewayId, void *reqBuf, int nTotalLen, int nPrefixLen)
	{
		if(m_knxReadDbList.GetCount() < 500);
		{
			SendNode *pNode = new SendNode();
			pNode->pNext = 0;
			memset(pNode, 0, sizeof(SendNode));
			memcpy(pNode->buf, reqBuf, nTotalLen);
			pNode->nLen = nTotalLen;
			nPrefixLen = 0;
			pNode->nAction = READ;

			m_knxReadDbList.AddTail((NodeBase *)pNode);

			LOCK_MUTEX(&m_KnxReadDbCondStruct.timerMutex);
			SET_SIGNAL(m_KnxReadDbCondStruct.timerCond);
			UNLOCK_MUTEX(&m_KnxReadDbCondStruct.timerMutex);

			return 0;
		}

		return -1;
	}

	void CGatewayManager::WriteKnxValueToDB(int nKey, KNX_GroupData *pGrpData)
	{
		ts_knx_control_info_t knxData;
		knxData.group_addr = pGrpData->nGroupAddr;
		GetValue_Grp2Buf(knxData.data, *pGrpData);
		knxData.is_first_use = TS_KNX_VALUE_NOT_FIRST_USE;

__fline printf("upd knx value[nKey=%d, grpAddr=(%d, %d, %d), strVal=%s]\n",
		nKey,
		(knxData.group_addr >> 11) & 0x0F,
		(knxData.group_addr >> 8) & 0x07,
		knxData.group_addr & 0xFF,
		knxData.data);

		db_upd_knx_device_curr_value(knxData, nKey);
	}

	int CGatewayManager::AddWifiRequest(int nKey, void *buf, int len)
	{
		CWifiKernel *pWifi = (CWifiKernel *) GetGatewayByKey(GATEWAY_TYPE_WIFI, nKey);
		int nRes = -1;
		if(pWifi)
		{
			nRes = pWifi->AddRequestNode(ACTION_WIFI_REQUEST, buf, len);
			return nRes;
		}
		else
		{
			return ERR_PROTROL_NO_GATEWAY;
		}
	}
