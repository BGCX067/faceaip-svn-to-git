/*
 * ICLogical.cpp
 *
 *  Created on: Apr 18, 2012
 *      Author: kyy
 */

#include "ICLogical.h"
#include "common/remote_def.h"
#include "common/ts_err_code.h"
#include "KNXCondef.h"
#include "GatewayManager.h"
#include "db/ts_db_protocol_infos.h"
#include <string.h>
#include <sys/ioctl.h>

#include "CommonInclude.h"

#include "enocean/enocean_mgr.h"

// do not modified-----------------
#define IR_DERIVER_LEARN	128
#define IR_DERIVER_SEND		129
// do not modified-----------------


CICLogical::CICLogical()
{
    m_hSendThread = 0;
    m_bSendThreadRun = false;

    ResetCurInfo();

    INITIALIZE_MUTEX(&m_sendListMutex);
	INITIALIZE_MUTEX(&m_sendTimer.timerMutex);
	INITIALIZE_MUTEX(&m_tcpCTimer.timerMutex);
	INITIALIZE_MUTEX(&m_tcpSTimer.timerMutex);
	INITIALIZE_MUTEX(&m_icTimer.timerMutex);
#ifdef _WIN32
	m_sendTimer.timerCond = COND_INIT();
	m_tcpCTimer.timerCond = COND_INIT();
	m_tcpSTimer.timerCond = COND_INIT();
	m_icTimer.timerCond = COND_INIT();
#else
	COND_INIT(&m_sendTimer.timerCond);
	COND_INIT(&m_tcpCTimer.timerCond);
	COND_INIT(&m_tcpSTimer.timerCond);
	COND_INIT(&m_icTimer.timerCond);
#endif
}

CICLogical::~CICLogical()
{
	StopSendThread();

	DESTROY_MUTEX(&m_sendTimer.timerMutex);
	DESTROY_MUTEX(&m_tcpCTimer.timerMutex);
	DESTROY_MUTEX(&m_tcpSTimer.timerMutex);
	DESTROY_MUTEX(&m_icTimer.timerMutex);

	COND_DESTROY(m_sendTimer.timerCond);
	COND_DESTROY(m_tcpCTimer.timerCond);
	COND_DESTROY(m_tcpSTimer.timerCond);
	COND_DESTROY(m_icTimer.timerCond);
}

void CICLogical::ResetCurInfo()
{
	m_nDevId = -1;
	m_nChannel = -1;
	memset(m_strProtocol, 0, 32);
	m_nCtrlType = -1;
	m_nCtrlData = -1;
}

void CICLogical::SetCurInfo(int nDevId, int nChannel,
			char *strProtocol, int nCtrlType, int nCtrlData)
{
	m_nDevId = nDevId;
	m_nChannel = nChannel;
	if(strProtocol != NULL)
	{
		strcpy(m_strProtocol, strProtocol);
	}
	m_nCtrlType = nCtrlType;
	m_nCtrlData = nCtrlData;
}

int CICLogical::WaitSendCond()
{
	m_sendTimer.timeout.tv_sec = time(0) + 1;
	m_sendTimer.timeout.tv_nsec = 0;
	LOCK_MUTEX(&m_sendTimer.timerMutex);
	int nRes = pthread_cond_timedwait(
			&m_sendTimer.timerCond,
			&m_sendTimer.timerMutex,
			&m_sendTimer.timeout);
	UNLOCK_MUTEX(&m_sendTimer.timerMutex);
	return nRes;
}

int CICLogical::HandleMessage()
{
	LOCK_MUTEX(&m_sendListMutex);
	SendNode *pNode = (SendNode *) m_sendList.GetAndSeperateHeader();
	UNLOCK_MUTEX(&m_sendListMutex);

	int nRes = SUCCESS;
	int nCtrlType = 0;

	while(m_bSendThreadRun && pNode != NULL)
	{
		switch(pNode->nAction)
		{
		case IC_WIFI_CLIENT:
		case ACTION_WIFI_REQUEST:
			{
				nRes = DoHandleTcpClientMsg(pNode->buf, pNode->nLen);

				// TCP Client in Host.
				kic_head_t * pKicHead = (kic_head_t *) pNode->buf;
				nCtrlType = pKicHead->device_type;
				
				if(nCtrlType == COM || nCtrlType == CUSTOMER_COM)
				{
					com_control_res_t *pComRes = (com_control_res_t *) pNode->buf;
					pComRes->res_code = nRes;
				}
				else if(nCtrlType == INFRARED)
				{
					wifi_control_res_t *pWifiRes = (wifi_control_res_t *) pNode->buf;
					pWifiRes->res_code = nRes;
				}
				else if(nCtrlType == IR_STUDY)
				{
					ir_study_res_t *pIrStudyRes = (ir_study_res_t *) pNode->buf;
					pIrStudyRes->res_code = nRes;
				}
				
				if(pKicHead->header.comm_type == REQUEST)
				{
					CGatewayManager::Instance()->GetCtrlResponse()(
						ACTION_WIFI_RESPONSE, pNode->buf, pNode->nLen);
				}
			}
			break;
		case IC_WIFI_SERVER:
		case ACTION_WIFI_RESPONSE:
			{
				// TCP Server in Wifi.
				nRes = DoHandleTcpServerMsg(pNode->buf, pNode->nLen);
			}
			break;
		case IC_DIRECT:
			nRes = DoHandleIcDirectMsg(pNode->buf, pNode->nLen);
			break;
		}

		if(pNode != NULL)
		{
			delete pNode;
			LOCK_MUTEX(&m_sendListMutex);
			pNode = (SendNode *) m_sendList.GetAndSeperateHeader();
			UNLOCK_MUTEX(&m_sendListMutex);
		}
	}

	return 0;
}

int CICLogical::DoHandleTcpClientMsg(void *pBuf, int nLen)
{
	int nRes = -1;

	//	1. Send request data to Server
	if(m_pICIface != NULL)
	{
		nRes = m_pICIface->Send(IC_WIFI_CLIENT, pBuf, nLen);
	}
	if(nRes != SUCCESS)
	{
		return nRes;
	}

	kic_head_t *pKicHead = (kic_head_t *) pBuf;
	int nDevId = ntohl(pKicHead->home_device_id);
	int nCtrlType = pKicHead->device_type;
	if(nCtrlType == COM || nCtrlType == CUSTOMER_COM)
	{
		com_control_req_t *pComReq = (com_control_req_t *) pBuf;
		SetCurInfo(nDevId, 0, NULL, nCtrlType, ntohl(pComReq->data) );
	}
	else if(nCtrlType == INFRARED)
	{
		wifi_control_req_t *pWifiReq = (wifi_control_req_t *) pBuf;
		SetCurInfo(nDevId, 0, NULL, INFRARED, ntohs(pWifiReq->function_code) );
	}
	else if(nCtrlType == IR_STUDY)
	{
		ir_study_req_t *pIrStudyReq = (ir_study_req_t *) pBuf;
		// nDevId is gateway id in ir_study_req_t
		SetCurInfo(nDevId, pIrStudyReq->channel, NULL, INFRARED, ntohs(pIrStudyReq->function_code) );
	}
	else
	{
		return ERR_PROTROL_NOT_SUPPORT;
	}

	//	2. wait for cond (when recved data & match to request info)
	m_tcpCTimer.timeout.tv_sec = time(0) + 3;
	if(nCtrlType == IR_STUDY)
	{
		m_tcpCTimer.timeout.tv_sec = time(0) + 5;
	}
	m_tcpCTimer.timeout.tv_nsec = 0;
	LOCK_MUTEX(&m_tcpCTimer.timerMutex);
	nRes = pthread_cond_timedwait(
			&m_tcpCTimer.timerCond,
			&m_tcpCTimer.timerMutex,
			&m_tcpCTimer.timeout);
	UNLOCK_MUTEX(&m_tcpCTimer.timerMutex);
	ResetCurInfo();
	if(nRes != 0)
	{
		return nRes;
	}
	else
	{
		return SUCCESS;
	}
}

bool CICLogical::SetTcpCSignal(kic_head_t * pRes)
{
	if(pRes == NULL || 
		(pRes->header.comm_type != REQUEST && pRes->header.comm_type != RESPONSE) )
	{
		return false;
	}

	int nDevId = ntohl(pRes->home_device_id);
	int nCtrlType = pRes->device_type;
	bool bFlag = false;
	if(m_nCtrlType == nCtrlType && nDevId == m_nDevId)
	{
		if(nCtrlType == COM || nCtrlType == CUSTOMER_COM)
		{
			com_control_res_t *pComRes = (com_control_res_t *) pRes;
			if( m_nCtrlData == (int) ntohl(pComRes->data) )
			{
				bFlag = true;
			}
			else
			{
				return false;
			}
		}
		else if(nCtrlType == INFRARED)
		{
			wifi_control_res_t *pWifiRes = (wifi_control_res_t *) pRes;
			if(m_nCtrlData == (int) ntohs(pWifiRes->function_code))
			{
				bFlag = true;
			}
			else
			{
				return false;
			}
		}
		else if(nCtrlType == IR_STUDY)
		{
			ir_study_res_t *pIrStudyRes = (ir_study_res_t *) pRes;
			if(m_nChannel == pIrStudyRes->channel && m_nCtrlData == ntohs(pIrStudyRes->function_code) )
			{
				bFlag = true;
			}
			else
			{
				return false;
			}
		}
	}
	
	if(bFlag == true)
	{
		LOCK_MUTEX(&m_tcpCTimer.timerMutex);
		SET_SIGNAL(m_tcpCTimer.timerCond);
		UNLOCK_MUTEX(&m_tcpCTimer.timerMutex);
		return true;
	}
	
	return false;
}

int CICLogical::ToAddSceneInputNode(kic_head_t * pKicHead)
{
	if(pKicHead == NULL)
	{
		return FAILED;
	}

	// add a scene input node.
	SceneInputNode inputNode;
	memset((char *) &inputNode, 0, sizeof(SceneInputNode));

	int nCtrlType = pKicHead->device_type;
	bool bFlag = false;
	int nRes = FAILED;
	switch(nCtrlType)
	{
	case COM:
	case CUSTOMER_COM:
		{
			com_control_res_t *pComRes = (com_control_res_t *) pKicHead;
			nRes = pComRes->res_code;
			if(nRes == SUCCESS)
			{
				inputNode.scene_in_info.dev_id = ntohl(pComRes->kic_head.home_device_id);
				inputNode.scene_in_info.param = ntohl(pComRes->data);
				//memset(inputNode.scene_in_info.value, 0, sizeof(inputNode.scene_in_info.value));
				inputNode.main_type = nCtrlType;
				// inputNode.sub_type = 0;
				//memset((char *) inputNode.original_value, 0, sizeof(inputNode.original_value));

				bFlag = true;
			}
		}
		break;
	case INFRARED:
		{
			wifi_control_res_t *pIrCtrlRes = (wifi_control_res_t *) pKicHead;
			nRes = pIrCtrlRes->res_code;
			if(nRes == SUCCESS)
			{
				inputNode.scene_in_info.dev_id = ntohl(pIrCtrlRes->kic_head.home_device_id);
				inputNode.scene_in_info.param = ntohs(pIrCtrlRes->function_code);
				//memset(inputNode.scene_in_info.value, 0, sizeof(inputNode.scene_in_info.value));
				inputNode.main_type = INFRARED;
				// inputNode.sub_type = 0;
				//memset((char *) inputNode.original_value, 0, sizeof(inputNode.original_value));

				bFlag = true;
			}
		}
		break;
	case SERIAL_RES:
/*		{
			serial_initiative_res_t *pAutoRes = (serial_initiative_res_t *) pKicHead;
			inputNode.scene_in_info.dev_id = ntohl(pAutoRes->kic_head.home_device_id);

			switch(pAutoRes->response_type)
			{
			case BACK_BACKAUDIO:
				{
					backaudio_back_t *pBack = (backaudio_back_t *) pAutoRes.data;
					inputNode.scene_in_info.param = ntohl(pAutoRes->data);
					sprintf(inputNode.scene_in_info.value, "%d", pBack.source);
				}
				break;
			case BACK_AUXDIO_02:
				{
					auxdio_back_02_t *pBack_02 = (auxdio_back_02_t *) pAutoRes.data;
					inputNode.scene_in_info.param = ntohl(pAutoRes->data);
					sprintf(inputNode.scene_in_info.value, "%d", pBack_02.source);
				}
				break;
			}
				
			inputNode.main_type = SERIAL_RES;
			// inputNode.sub_type = 0;
			//memset((char *) inputNode.original_value, 0, sizeof(inputNode.original_value));

			bFlag = true;
		}*/
		break;
	}

	if(bFlag == true)
	{
		(CGatewayManager::Instance())->m_sceneMan.AddSceneInputNode(inputNode);
	}

	return SUCCESS;
}


int CICLogical::DoHandleTcpServerMsg(void *pBuf, int nLen)
{
	int nRes = -1;

	// 1.get operation type
	int nReqType = 0;
	remote_header_t *pHead = (remote_header_t *) pBuf;
	kic_head_t *pKicReq = NULL;
	if(pHead->function_code == DEVICE_CONTROL)
	{
		device_control_req *pDeviceCtrl = (device_control_req *) pBuf;
		int nDevId = 0;
		int nGatewayId = 0;
		int nGatewayType = 0;
		nRes = db_get_device_infos_by_dp( ntohl(pDeviceCtrl->datapoint_id), &nDevId, &nReqType, &nGatewayId, &nGatewayType);
		if(nRes != EXIT_SUCCESS)
		{
			return nRes;
		}
	}
	else
	{
		pKicReq = (kic_head_t *) pBuf;
		nReqType = pKicReq->device_type;
	}

	CGatewayManager *pGatewayMan = CGatewayManager::Instance();

	// 2.handler the details
	switch(nReqType)
	{
	case COM:
		{
			nRes = HandleComReq(pBuf, nLen);

			com_control_res_t *pComRes = (com_control_res_t *) pBuf;
			pComRes->res_code = nRes;
			if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_GATEWAY
					&& m_pICIface != NULL
				/*	&& pComRes->kic_head.header.comm_type == REQUEST */)
			{
				m_pICIface->Send(ACTION_WIFI_RESPONSE, pBuf, nLen);
				// here will trigger a scene at the host when it receive the response.
			}
			else if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST)
			{
				if(pGatewayMan->GetCtrlResponse() != NULL
					&& (pComRes->kic_head.header.comm_type == REQUEST 
					|| pComRes->kic_head.header.comm_type == RESPONSE))
				{
					pGatewayMan->GetCtrlResponse()(IC_DIRECT_COM_RES, pBuf, nLen);
				}

				if(nRes == SUCCESS)
				{
					ToAddSceneInputNode(pKicReq);
				}
			}
		}
		break;
	case CUSTOMER_COM:
		{
			nRes = HandleCustomerComReq(pBuf, nLen);

			com_control_res_t *pComRes = (com_control_res_t *) pBuf;
			pComRes->res_code = nRes;
			if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_GATEWAY
					&& m_pICIface != NULL
				/*	&& pComRes->kic_head.header.comm_type == REQUEST*/)
			{
				m_pICIface->Send(ACTION_WIFI_RESPONSE, pBuf, nLen);
				// here will trigger a scene at the host when it receive the response.
			}
			else if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST)
			{
				if(pGatewayMan->GetCtrlResponse() != NULL
					&& (pComRes->kic_head.header.comm_type == REQUEST 
					|| pComRes->kic_head.header.comm_type == RESPONSE))
				{
					pGatewayMan->GetCtrlResponse()(IC_DIRECT_COM_CUSTOMER_RES, pBuf, nLen);
					// here will trigger a scene at the host when it receive the response.
				}

				if(nRes == SUCCESS)
				{
					ToAddSceneInputNode(pKicReq);
				}
			}
		}
		break;
	case INFRARED:
		{
			nRes = HandleInfraredReq(pBuf, nLen);

			wifi_control_res_t *pIrCtrlRes = (wifi_control_res_t *) pBuf;
			pIrCtrlRes->res_code = nRes;
			if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_GATEWAY
					&& m_pICIface != NULL
					&& pIrCtrlRes->kic_head.header.comm_type == REQUEST)
			{
				m_pICIface->Send(ACTION_WIFI_RESPONSE, pBuf, nLen);
				// here will trigger a scene at the host when it receive the response.
			}
			else if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST)
			{
				if(pGatewayMan->GetCtrlResponse() != NULL
					&& (pIrCtrlRes->kic_head.header.comm_type == REQUEST
					|| pIrCtrlRes->kic_head.header.comm_type == RESPONSE))
				{
					pGatewayMan->GetCtrlResponse()(IC_DIRECT_IRCTRL_RES, pBuf, nLen);
				}

				if(nRes == SUCCESS)
				{
					ToAddSceneInputNode(pKicReq);
				}
			}
		}
		break;
	case IR_STUDY:
		{
			nRes = HandleIrStudyReq(pBuf, nLen);

			ir_study_res_t *pIrStudyRes = (ir_study_res_t *) pBuf;
			pIrStudyRes->res_code = nRes;
__fline printf("------------ study return nRes=%d\n", nRes);
			if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_GATEWAY
					&& m_pICIface != NULL
					&& pIrStudyRes->kic_head.header.comm_type == REQUEST)
			{
				m_pICIface->Send(ACTION_WIFI_RESPONSE, pBuf, nLen);
			}
			else if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST
					&& pGatewayMan->GetCtrlResponse() != NULL
					&& pIrStudyRes->kic_head.header.comm_type == REQUEST)
			{
				pGatewayMan->GetCtrlResponse()(IC_DIRECT_IRSTUDY_RES, pBuf, nLen);
			}
		}
		break;
	case ENOCEAN:
		{
			int nDataPointType = 0;
			int nValueAfterCtrl = 0;
			int nEnoceanOnOffDatapointId = -1;
			int nEnoceanDimmingDatapointId = -1;
			nRes = HandleEnoceanReq(pBuf, nLen, &nDataPointType, 
				&nValueAfterCtrl, nEnoceanOnOffDatapointId, nEnoceanDimmingDatapointId);

			int nDataLen = 0;
			if(nRes == SUCCESS)
			{
				if(nDataPointType == cut_1_LightOnOff|| nDataPointType == cut_1_LightDimmer)
				{
					nDataLen = 1;
				}
			}

			int nMsgLen = sizeof(device_control_res) + nDataLen;
			device_control_res *pEnoceanCtrlRes = (device_control_res *) new char[nMsgLen];

			device_control_req *pEnoceanCtrlReq = (device_control_req *) pBuf;
			
			pEnoceanCtrlRes->header = pEnoceanCtrlReq->header;
			pEnoceanCtrlRes->header.length = nMsgLen - sizeof(remote_header_t);
			pEnoceanCtrlRes->res_code = nRes;
			pEnoceanCtrlRes->data_len = nDataLen;
			if(nDataLen > 0)
			{
				*((char *)pEnoceanCtrlRes->data) = nValueAfterCtrl;
			}
			
__fline printf("------------ HandleEnoceanReq return nRes=%d\n", nRes);
			if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_GATEWAY
					&& m_pICIface != NULL
					&& pEnoceanCtrlReq->header.comm_type == REQUEST)
			{
				// send to host.
				m_pICIface->Send(ACTION_WIFI_RESPONSE, pBuf, nLen);
			}
			else if(pGatewayMan->WhoAmI() == TS_PRO_TYPE_HOST
					&& pGatewayMan->GetCtrlResponse() != NULL
					&& pEnoceanCtrlReq->header.comm_type == REQUEST)
			{
				// to update the current value in db.
				int nDataPointId = 0;
				char strValue[8] = {0};
				
				nDataPointId = ntohl(pEnoceanCtrlReq->datapoint_id);
				pGatewayMan->GetCtrlResponse()(IC_DIRECT, pBuf, nLen);

				// no value.
				if(nDataLen <= 0)
				{
					delete pEnoceanCtrlRes;
					pEnoceanCtrlRes = NULL;
					break;
				}
				
				sprintf((char *) strValue, "%d", nValueAfterCtrl);
				pGatewayMan->UpdateDatapointData(nDataPointId, strValue);
				
				if(nDataPointType == cut_1_LightOnOff && nEnoceanDimmingDatapointId < 0)
				{
					// just on off.
				}
				else if(nDataPointType == cut_1_LightOnOff && nEnoceanDimmingDatapointId > 0)
				{
					// ctrl on off, but also dimming
					nDataPointId = ntohl(nEnoceanDimmingDatapointId);
					nValueAfterCtrl = nValueAfterCtrl == 1 ? 100 : 0;
					*((char*) pEnoceanCtrlRes->data) = nValueAfterCtrl;
					pGatewayMan->GetCtrlResponse()(IC_DIRECT, pBuf, nLen);
				
					sprintf((char *) strValue, "%d", nValueAfterCtrl);
					pGatewayMan->UpdateDatapointData(nEnoceanDimmingDatapointId, strValue);
				}
				else if(nDataPointType == cut_1_LightDimmer&& nEnoceanOnOffDatapointId < 0)
				{
					// just dimming.
				}
				else if(nDataPointType == cut_1_LightDimmer && nEnoceanOnOffDatapointId > 0)
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

			delete pEnoceanCtrlRes;
			pEnoceanCtrlRes = NULL;
		}
		break;
	}

	return nRes;
}


int CICLogical::HandleEnoceanReq(void *pBuf, int nLen, int *nDatapointType, 
	int *nValueAfterCtrl, int &nEnoceanOnOffDatapointId, int &nEnoceanDimmingDatapointId)
{
#ifdef DEV_ENOCEAN_SUPPORT
	if(pBuf == NULL || nDatapointType == NULL || nValueAfterCtrl == NULL)
	{
		return ERR_PROTROL_INNER_FAILED;
	}

	device_control_req *pEnoceanCtrlReq = (device_control_req *) pBuf;

	int nDataPointId = ntohl(pEnoceanCtrlReq->datapoint_id);
	int nRes = db_get_ctrltype_by_dp(nDataPointId, nDatapointType);
	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	unsigned int nEnoceanId = 0;
	int nEnoceanRocker = 0;
	nRes = db_get_enocean_id_by_dp(nDataPointId, &nEnoceanId, &nEnoceanRocker);

	int nReadWrite = pEnoceanCtrlReq->read_write;
	
	// get current value by nDataPointId from db.
	char strCurrentValue[8] = {0};
	int nCurrentValue = 0;
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	nRes = pGatewayMan->GetDatapointData(nDataPointId, strCurrentValue);
	if(nRes != EXIT_SUCCESS && nRes != ERR_DB_NO_RECORDS)
	{
		return nRes;
	}
	
	if(nRes == ERR_DB_NO_RECORDS)
	{
		nCurrentValue = 0;
	}
	else
	{
		nCurrentValue = atoi((char *) strCurrentValue);
	}
	
	if(nReadWrite == READ)
	{
		*nValueAfterCtrl = nCurrentValue;
		return SUCCESS;
	}
			
	// 1. to get all of the datapoint of the device.
	LIST_HEAD(all_datapoint_list);
	ts_dps_list_t *pDataPoint = NULL;
	nRes = db_get_dps_by_same_dev(nDataPointId, &all_datapoint_list, pDataPoint);
	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	// 2. to check whether there is a cut_1_LightDimmer in the device.
	nEnoceanOnOffDatapointId = -1;
	nEnoceanDimmingDatapointId = -1;
	ts_dps_list_t *pDataPointTemp = NULL;
	list_for_each_entry_safe(pDataPoint, pDataPointTemp, &all_datapoint_list, list)
	{
		if(pDataPoint != NULL && pDataPoint->dp_info.ctrl_type == cut_1_LightOnOff)
		{
			nEnoceanOnOffDatapointId = pDataPoint->dp_info.dp_id;
		}
		else if(pDataPoint != NULL && pDataPoint->dp_info.ctrl_type == cut_1_LightDimmer)
		{
			nEnoceanDimmingDatapointId = pDataPoint->dp_info.dp_id;
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
		return ERR_PROTROL_CONFIGURE_DATAPOINT;
	}
	
	switch(*nDatapointType)
	{
		case cut_1_LightOnOff:
		{
			int nValue = *((unsigned char *)pEnoceanCtrlReq->data);
			nValue = (nValue == 1) ? 1 : 0;			
			if(nEnoceanDimmingDatapointId < 0)
			{
				*nValueAfterCtrl = nValue;
				enocean_device_onoff(nEnoceanId, nEnoceanRocker, nValue);
				enocean_device_onoff(nEnoceanId, nEnoceanRocker, nValue);
				enocean_device_onoff(nEnoceanId, nEnoceanRocker, nValue);
			}
			else
			{
				*nValueAfterCtrl = nValue;
				enocean_device_dim_begin(nEnoceanId, nEnoceanRocker, nValue);
				enocean_device_dim_begin(nEnoceanId, nEnoceanRocker, nValue);
				enocean_device_dim_begin(nEnoceanId, nEnoceanRocker, nValue);

				// how long of the time, dpendent with the device.
				usleep(50 * 1000);
			
				enocean_device_dim_end(nEnoceanId, nEnoceanRocker, nValue);
				enocean_device_dim_end(nEnoceanId, nEnoceanRocker, nValue);
				enocean_device_dim_end(nEnoceanId, nEnoceanRocker, nValue);
			}
			
			return SUCCESS;
		}
		case cut_1_LightDimmer:
		{
			int nValue = *((unsigned char *)pEnoceanCtrlReq->data);

			if(nReadWrite == WRITE)
			{
				// get nMaxSteps and nMaxTime by nDataPointId.
				int nMaxSteps = 0;
				int nMaxTime = 0;
				nRes = db_get_dp_infos_by_dp(nDataPointId, &nMaxSteps, &nMaxTime);
				if(nRes != EXIT_SUCCESS)
				{
					return nRes;
				}

				// the time period from begin to end.
				int nMillSecToSleep = 0;
				int nDimmingValue = 0;

				// caculate total millseconds to sleep from dimming begin to end.
				// caculate actual value after ctrl.
 				if(nValue == ENOCEAN_DIMMING_DOWN || nValue == ENOCEAN_DIMMIMG_UP)
				{
					if(nMaxSteps == 0)
					{
						return ERR_PROTROL_CONFIGURE_MAXVALUE;
					}
	
					nMillSecToSleep = (nMaxTime * 1000.0) / nMaxSteps;
					if(nValue == ENOCEAN_DIMMIMG_UP)
					{
						nDimmingValue = 1;
						nValueAfterCtrl += nCurrentValue + 100 / nMaxSteps;
					}
					else
					{
						nDimmingValue = 0;
						nValueAfterCtrl -= nCurrentValue + 100 / nMaxSteps;
					}
				}
				else
				{
					if(nValue == 0)
					{
						nMillSecToSleep = nMaxTime;
						nDimmingValue = 0;
						*nValueAfterCtrl = nValue;
					}
					else if(nValue == 100)
					{
						nMillSecToSleep = nMaxTime;
						nDimmingValue = 1;
						*nValueAfterCtrl = nValue;
					}
					else if(nCurrentValue == nValue)
					{
						*nValueAfterCtrl = nCurrentValue;
						return SUCCESS;
					}
					else
					{
						int nJumpSteps = 0;
						if(nValue > nCurrentValue)
						{
							nJumpSteps = nValue - nCurrentValue;
							nDimmingValue = 1;
						}
						else
						{
							nJumpSteps = nCurrentValue - nValue;
							nDimmingValue = 0;
						}
						*nValueAfterCtrl = nValue;
					
						nMillSecToSleep = (nMaxTime * 1000.0 * nJumpSteps)/ nMaxSteps;
					}
				}


				// 4. to carry out the process of dimming: begin, sleep and end.
				enocean_device_dim_begin(nEnoceanId, nEnoceanRocker, nDimmingValue);
				enocean_device_dim_begin(nEnoceanId, nEnoceanRocker, nDimmingValue);
				enocean_device_dim_begin(nEnoceanId, nEnoceanRocker, nDimmingValue);

				usleep(nMillSecToSleep * 1000);
			
				enocean_device_dim_end(nEnoceanId, nEnoceanRocker, nDimmingValue);
				enocean_device_dim_end(nEnoceanId, nEnoceanRocker, nDimmingValue);
				enocean_device_dim_end(nEnoceanId, nEnoceanRocker, nDimmingValue);
			
				return SUCCESS;
			}

			return ERR_PROTROL_DATAPOINT_NOT_SUPPORT;
		}
		default:
			return ERR_PROTROL_DATAPOINT_NOT_SUPPORT;
	}
#else
	return ERR_PROTROL_DATAPOINT_NOT_SUPPORT;
#endif
}

int CICLogical::HandleComReq(void *pBuf, int nLen, bool bCustomerDev)
{
	com_control_req_t *pComReq = (com_control_req_t *) pBuf;
	int nDevId = ntohl(pComReq->kic_head.home_device_id);

	int nRes = -1;

	int nComIndex = -1;
	int nGatewayId = -1;
	int nArea = TS_ZONE_UNUSED;
	char protocol_file[32] = {'\0'};
	char com_name[32] = {'\0'};

	// get info of device
	nRes = db_get_serial_device_info(nDevId, &nComIndex,
			&nGatewayId, &nArea, protocol_file, com_name);

	// get message from .so
	ComSoStruct *pComSo = CGatewayManager::Instance()->GetProtocolSoStruct(protocol_file);
	if(!bCustomerDev && (pComSo == NULL || pComSo->pInstance == NULL))
	{
		return ERR_PROTROL_SO_FAILED;
	}

	// get message and send message to the com port logical manager
	char message[COM_MESSAGE_MAX_LEN] = {'\0'};
	int nMsgLen = 0;
	int nDevType = UNKNOWN_DEVICE;
	if(bCustomerDev)
	{
		int nKeyCode = ntohl(pComReq->data);
		// get message from db
		nRes = db_get_serial_datagram(nDevId, nKeyCode, message, &nMsgLen);
	}
	else
	{
		if(strcmp(protocol_file, "libEpson_Esc_Vp21.so") == 0
				|| strcmp(protocol_file, "libAuxdio_8150.so") == 0
				|| strcmp(protocol_file, "libBackaudio_A61L.so") == 0
				|| strcmp(protocol_file, "libYodar_4028B.so") == 0
				|| strcmp(protocol_file, "libISTV.so") == 0
				|| strcmp(protocol_file, "libAQM_1000.so") == 0)
		{
			if(strcmp(protocol_file, "libEpson_Esc_Vp21.so") == 0)
			{
				nDevType = EPSON_ESC_VP21;
			}
			else if(strcmp(protocol_file, "libAuxdio_8150.so") == 0)
			{
				nDevType = AUXDIO_8150;
			}
			else if(strcmp(protocol_file, "libBackaudio_A61L.so") == 0)
			{
				nDevType = BACKAUDIO_A61L;
			}
			else if(strcmp(protocol_file, "libYodar_4028B.so") == 0)
			{
				nDevType = YODAR_4028B;
			}
			else if(strcmp(protocol_file, "libISTV.so") == 0)
			{
				nDevType = ISTV;
			}
			else if(strcmp(protocol_file, "libAQM_1000.so") == 0)
			{
				nDevType = AQM_1000;
			}
			
			if(pComSo->pInstance != NULL)
			{
				nRes = pComSo->pInstance->GetSerialDeviceMessage(pComReq->com_ctrl_type,
						nArea, ntohl(pComReq->data), message, nMsgLen);
				printf("----message is [");
				for(int i = 0; i < nMsgLen; i++)
				{
					printf("%.2x ", (unsigned char) message[i]);
				}
				printf("]\n");
			}
			else
			{
				nRes = ERR_PROTROL_COM_PRO_FUN;
			}
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}

/*
		switch(pComReq->com_ctrl_type)
		{
		case EPSON_CTRL_SWITCH:
		case EPSON_CTRL_SOURCE:
		case EPSON_CTRL_AV_SWITCH:
		case EPSON_CTRL_AV_TYPE:
			if(pComSo->pInstance != NULL)
			{
				nRes = pComSo->pInstance->GetProjectorMessage(pComReq->com_ctrl_type,
						ntohl(pComReq->data), message, nMsgLen);
			}
			else
			{
				nRes = ERR_PROTROL_COM_PRO_FUN;
			}
			break;
		case BACK_CTRL_SWITCH:
		case BACK_CTRL_PLAY:
		case BACK_CTRL_PREV_NEXT:
		case BACK_CTRL_VOLUME:
		case BACK_CTRL_SOURCE:
		case BACK_CTRL_DIRECTORY:
		case AUXDIO_CTRL_SWITCH:
		case AUXDIO_CTRL_PLAY:
		case AUXDIO_CTRL_PREV_NEXT:
		case AUXDIO_CTRL_MAIN_VOL:
		case AUXDIO_CTRL_TREBLE_VOL:
		case AUXDIO_CTRL_BASS_VOL:
		case AUXDIO_CTRL_SOURCE:
		case AUXDIO_CTRL_TNR_TYPE:
		case AUXDIO_CTRL_CHANNEL_AM:
		case AUXDIO_CTRL_CHANNEL_FM:
		case AUXDIO_CTRL_DIRECTORY:
			if(pComSo->pInstance != NULL)
			{
				nRes = pComSo->pInstance->GetMusicHostMessage(pComReq->com_ctrl_type,
						nArea, ntohl(pComReq->data), message, nMsgLen);
			}
			else
			{
				nRes = ERR_PROTROL_COM_PRO_FUN;
			}
			break;
		default:
			nRes = ERR_PROTROL_PARAMETERS;
			break;
		}
*/
	}


	// send message to serial
	nRes = ERR_PROTROL_COM_SEND;
	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	for(int index = 0; index < pGatewayMan->g_com_count; index++)
	{
		if(pGatewayMan->g_com_infos[index].id == nComIndex)
		{
			if(pGatewayMan->g_pSerial[index] != NULL)
			{
				ezserial_putdata(pGatewayMan->g_pSerial[index], (unsigned char *) message, nMsgLen);
				nRes = SUCCESS;
				if(!bCustomerDev)
				{
					RefreshDeviceState(nDevType, nDevId, nArea, pComReq->com_ctrl_type, ntohl(pComReq->data));
				}
			}
			break;
		}
	}

	return nRes;

/*	if(nRes != SUCCESS)
	{
		return nRes;
	}

	// it is customer serial device, do not wait for reply.
	if(bCustomerDev)
	{
		return SUCCESS;
	}

	// Auxdio music host, do not wait for reply, it is auto response.
	if(strcmp(protocol_file, "libAuxdio_8150.so") == 0
		|| strcmp(protocol_file, "libBackaudio_A61L.so") == 0)
	{
		return SUCCESS;
	}

	SetCurInfo(-1, pComReq->channel, protocol_file, -1, -1);

	m_icTimer.timeout.tv_sec = time(0) + 2;
	m_icTimer.timeout.tv_nsec = 0;
	LOCK_MUTEX(&m_icTimer.timerMutex);
	nRes = pthread_cond_timedwait(
				&m_icTimer.timerCond,
				&m_icTimer.timerMutex,
				&m_icTimer.timeout);
	UNLOCK_MUTEX(&m_icTimer.timerMutex);
	if(nRes != 0)
	{
		return ERR_PROTROL_TIMEOUT;
	}
	else
	{
		return SUCCESS;
	}
*/
}

int CICLogical::RefreshDeviceState(int nComDevType, int nDevId, int nArea, int com_ctrl_type, int nData)
{
	//get the current state of the device with [nDevId]
	char curr_state[CURRENT_STATE_LEN] = {0};
	int nComDevType_DB = UNKNOWN_DEVICE;
	int nArea_DB = TS_ZONE_UNUSED;
	int nRes = db_get_serial_device_curr(nDevId, &nComDevType_DB, &nArea_DB, (char *) curr_state);
	if(nRes != EXIT_SUCCESS && nRes != ERR_DB_NO_RECORDS)
	{
		return nRes;
	}

	// set the device state with "com_ctrl_type" and "nData"
	if(nComDevType == EPSON_ESC_VP21)
	{
		// do nothing
		/*
		switch(com_ctrl_type)
		{
		case CTRL_SWITCH:
			break;
        case CTRL_SOURCE:
            break;
        case CTRL_AV_SWITCH:
            break;
        case CTRL_AV_TYPE:
            break;
		}*/
	}
	else if(nComDevType == BACKAUDIO_A61L)
	{
		if(nComDevType_DB != nComDevType || nArea_DB != nArea)
		{
			memset(curr_state, 0, CURRENT_STATE_LEN);
		}

		backaudio_a61l_state * back_a61l = (backaudio_a61l_state *) curr_state;
		back_a61l->device_id = nDevId;
		back_a61l->address = nArea;

		if(com_ctrl_type == CTRL_SWITCH && com_ctrl_type == CTRL_PLAY)
		{
			switch(com_ctrl_type)
			{
			case CTRL_SWITCH:
				back_a61l->power = nData;
				break;
    	    case CTRL_PLAY:
				back_a61l->play_state = nData;
    	        break;
			}
        	nRes = db_set_serial_device_curr(nDevId, nComDevType, nArea, (char *) curr_state);
		}
	}
	else if(nComDevType == AUXDIO_8150)
	{
		// do nothing
	}
	return nRes;
}

bool CICLogical::SetIcSignal(int nChannel, char *strProtocol)
{
/*	__fline printf("Channel(%d, %d)\tProtocol(%s,%s)\n",
			m_nChannel, nChannel,
			m_strProtocol, strProtocol);
*/
	if( strProtocol != NULL && m_nChannel == nChannel
			&& strcmp(m_strProtocol, strProtocol) == 0 )
	{
		LOCK_MUTEX(&m_icTimer.timerMutex);
		SET_SIGNAL(m_icTimer.timerCond);
		UNLOCK_MUTEX(&m_icTimer.timerMutex);
		return true;
	}
	return false;
}

int CICLogical::HandleCustomerComReq(void *pBuf, int nLen)
{
	return HandleComReq(pBuf, nLen, true);
}

int CICLogical::HandleInfraredReq(void *pBuf, int nLen)
{
	wifi_control_req_t *pIrReq = (wifi_control_req_t *) pBuf;
	int nDevId = ntohl(pIrReq->kic_head.home_device_id);

	int nIfaceId = -1;
	int nGatewayId = -1;
	int nChannelId = -1;
	int nRes = db_get_infrared_device_info(nDevId, &nIfaceId, &nGatewayId, &nChannelId);

	__fline printf("--------nDevId=%d ----- nRes=%d, nIfaceId=%d, nGatewayId=%d, nChannelId=%d \n",
			nDevId, nRes, nIfaceId, nGatewayId, nChannelId);
	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	if(nChannelId < 0 && nChannelId >= TOTAL_INFRARED_NUM)
	{
		return ERR_PROTROL_IR_CONF;
	}

	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->OpenInfraredDev();
	if(pGatewayMan->g_irDeviceHandler == -1)
	{
		return ERR_PROTROL_IR_OPEN;
	}

	int nFunKey = ntohs(pIrReq->function_code);
	unsigned short irCode[IR_MESSAGE_MAX_SHORT_LEN] = {'\0'};
	int nMsgLen = 0;

	int nIrCodeType = TS_INFRCODE_TYPE_0;
	if(pGatewayMan->g_bIrSendReverse[nChannelId])
	{
		nIrCodeType = TS_INFRCODE_TYPE_0;
		pGatewayMan->g_bIrSendReverse[nChannelId] = false;
	}
	else
	{
		nIrCodeType = TS_INFRCODE_TYPE_1;
		pGatewayMan->g_bIrSendReverse[nChannelId] = true;
	}

	nRes = db_get_infrared_code(nGatewayId, nFunKey, (char *) irCode, nIrCodeType, &nMsgLen);

	__fline printf("query infrared nMsgLen=%d: [", nMsgLen);
	for(int i=0; i < nMsgLen / 2; i++)
	{
		printf("%d ", irCode[i]);
	}
	printf("]\n");

	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	nRes = write(pGatewayMan->g_irDeviceHandler, (char *) irCode, nMsgLen);

/*	if(nIrCodeType == IR_CODE_TYPE_RC6)
	{
		if(pGatewayMan->g_bIrReverse)
		{
			unsigned short irCodeRev[IR_MESSAGE_MAX_SHORT_LEN] = {'\0'};
			memcpy(irCodeRev, irCode, 32);
			irCodeRev[16] = 1;
			irCodeRev[17] = 4180;
			irCodeRev[18] = 0;
			irCodeRev[19] = 4180;
			int index = 20;
			for(; index < (IR_MESSAGE_MAX_SHORT_LEN - 1); index++)
			{
				irCodeRev[index] = irCode[index + 4];
				if( (irCode[index + 6] == 0 || irCode[index + 6] == 65500)
						&& irCode[index + 4] == 0)
				{
					break;
				}
			}

			if(index == (IR_MESSAGE_MAX_SHORT_LEN - 1))
			{
				// loop exit by "index == (IR_MESSAGE_MAX_SHORT_LEN - 1)"
				nMsgLen = index * 2;
			}
			else
			{
				// loop exit by "break"
				nMsgLen = (index + 1) * 2;
			}
			nRes = write(pGatewayMan->g_irDeviceHandler, (char *) irCodeRev, nMsgLen);
		}
		else
		{
			nRes = write(pGatewayMan->g_irDeviceHandler, (char *) irCode, nMsgLen);
		}
	}
	else //if(nIrCodeType == IR_CODE_TYPE_STANDARD)
	{
		nRes = write(pGatewayMan->g_irDeviceHandler, (char *) irCode, nMsgLen);
	}
*/

	ioctl(pGatewayMan->g_irDeviceHandler, IR_DERIVER_SEND, nChannelId);
	unsigned char sendOk;
	read(pGatewayMan->g_irDeviceHandler, &sendOk, 1);
	int nSendTimes = 0;
	while(!sendOk && nSendTimes < 7)
	{
		usleep(50 * 1000);
		nSendTimes++;
		read(pGatewayMan->g_irDeviceHandler, &sendOk, 1);
	}

	if(sendOk != 1)
	{
		return ERR_PROTROL_IR_WRITE;
	}

	return 0;
}

unsigned short absc(unsigned short a, unsigned short b)
{
	if(a >= b)
	{
		return a-b;
	}
	else
	{
		return b-a;
	}
}

int CICLogical::HandleIrStudyReq(void *pBuf, int nLen)
{
	ir_study_req_t *pIrStudy = (ir_study_req_t *) pBuf;
	// it's gateway id but not device id.
	int nGatewayId = ntohl(pIrStudy->gateway_id);

	CGatewayManager *pGatewayMan = CGatewayManager::Instance();
	pGatewayMan->OpenInfraredDev();
	if(pGatewayMan->g_irDeviceHandler == -1)
	{
		return ERR_PROTROL_IR_OPEN;
	}

	int nFunKey = ntohs(pIrStudy->function_code);
	unsigned short irCode[IR_MESSAGE_MAX_SHORT_LEN] = {'\0'};

	__fline printf("--------------- study start ... nGatewayId=%d, nFunKey=%d \n",
		nGatewayId, nFunKey);

	ioctl(pGatewayMan->g_irDeviceHandler, IR_DERIVER_LEARN, 0);
	unsigned char learnEnd = 0;
	int nSec = 3;
	while(learnEnd == 0 && nSec > 0)
	{
		__fline printf("------------- study ing ... learnEnd=%d \n", learnEnd);

		sleep(1);
		nSec--;
		read(pGatewayMan->g_irDeviceHandler, &learnEnd, 1);
	}

	if(learnEnd != 1)
	{
		return ERR_PROTROL_IR_LEARN_TIMEOUT;
	}

	// read buf & save it into db
	unsigned int nMsgLen = read(pGatewayMan->g_irDeviceHandler, (char *) irCode, IR_MESSAGE_MAX_SHORT_LEN * 2);

	__fline printf("------------- study end read ... nMsgLen=%d \n", nMsgLen);

	// if it is jam.
	if(irCode[5] == 0)
	{
		return ERR_PROTROL_IR_LEARN_JAM;
	}

	int nRes = -1;
	int nChannel = pIrStudy->channel;
	int nIrCodeType = TS_INFRCODE_TYPE_0;

	if(pGatewayMan->g_bIrLearnReverse[nChannel])
	{
		nIrCodeType = TS_INFRCODE_TYPE_0;
		pGatewayMan->g_bIrLearnReverse[nChannel] = false;
	}
	else
	{
		nIrCodeType = TS_INFRCODE_TYPE_1;
		pGatewayMan->g_bIrLearnReverse[nChannel] = true;
	}

	unsigned int nMsgShortLen = 0;
	for(int i = 0; i < IR_MESSAGE_MAX_SHORT_LEN - 1; i++ )
	{
		if(irCode[i] == 0 && irCode[i + 1] == 0)
		{
			nMsgShortLen = i;
			break;
		}
	}

	nMsgLen = nMsgLen < (nMsgShortLen * 2) ? nMsgLen : (nMsgShortLen * 2);
	if(nMsgLen <= 0)
	{
		return ERR_PROTROL_IR_LEARN_LEN;
	}

	__fline printf("learned nMsgLen=%d: [", nMsgLen);
	for(unsigned int i=0; i < nMsgLen / 2; i++)
	{
		printf("%d ", irCode[i]);
	}
	printf("]\n");

	nRes = db_set_infrared_code(nGatewayId, nFunKey,
			(char *) irCode, nIrCodeType, nMsgLen);

	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

/*	int nIrCodeType = IR_CODE_TYPE_STANDARD;
	if( (absc(irCode[1],8331) <= 250) && (absc(irCode[3],2778) <= 300)
			&& (absc(irCode[5],1387) <= 260) && (absc(irCode[7],2778) <= 250)
			&& (absc(irCode[9],1387) <= 250) && (absc(irCode[11],1387) <= 250)
			&& (absc(irCode[13],1387) <= 250) &&  (absc(irCode[15],1387) <= 250) )
	{
		nIrCodeType = IR_CODE_TYPE_RC6;

		// if it is reverse code.
		if( (absc(irCode[17],4120) <= 350))
		{
			unsigned short irCodeRev[IR_MESSAGE_MAX_SHORT_LEN] = {'\0'};
			memcpy(irCodeRev, irCode, 32);
			irCodeRev[16] = 1;
			irCodeRev[17] = 1387;
			irCodeRev[18] = 0;
			irCodeRev[19] = 2778;
			irCodeRev[20] = 1;
			irCodeRev[21] = 2778;
			irCodeRev[22] = 0;
			irCodeRev[23] = 1387;

			int index = 24;
			for(; index < IR_MESSAGE_MAX_SHORT_LEN - 1; index++)
			{
				irCodeRev[index] = irCode[index-4];
				if((irCode[index-3] == 0 || irCode[index-3] == 65500) && irCode[index-2] == 0)
				{
				    break;
				}
			}

			if(index == IR_MESSAGE_MAX_SHORT_LEN - 1)
			{
				nMsgLen = 2 * index;
			}
			else
			{
				nMsgLen = 2 * (index + 1);
			}
			nRes = db_set_infrared_code(nGatewayId, nFunKey,
					(char *) irCodeRev, nIrCodeType, nMsgLen);
		}
		else
		{
			nRes = db_set_infrared_code(nGatewayId, nFunKey,
					(char *) irCode, nIrCodeType, nMsgLen);
		}
	}
	else
	{
		nIrCodeType = IR_CODE_TYPE_STANDARD;
		nRes = db_set_infrared_code(nGatewayId, nFunKey,
				(char *) irCode, nIrCodeType, nMsgLen);
	}

	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}
*/
	return 0;
}

int CICLogical::DoHandleIcDirectMsg(void *pBuf, int nLen)
{
	return DoHandleTcpServerMsg(pBuf, nLen);
}

int ICSendThread(void *pParam)
{
	CICLogical *pLogi = (CICLogical *) pParam;
	while(pLogi->IsSendThreadRun())
	{
		pLogi->WaitSendCond();
		if(!pLogi->IsSendThreadRun())
		{
			// exit thread
			break;
		}
		//__trip;
		pLogi->HandleMessage();
	}

	return 0;
}

int CICLogical::AddRequestNode(int nAction, void *pBuf, int nLen)
{
	if(m_sendList.GetCount() > 500)
	{
		__trip;

		return ERR_PROTROL_LIST_IS_FULL;
	}

	SendNode *pNode = new SendNode;
	memset(pNode, 0, sizeof(SendNode));
	memcpy(pNode->buf, pBuf, nLen);
	pNode->nLen = nLen;
	pNode->nAction = nAction;

	LOCK_MUTEX(&m_sendListMutex);
	m_sendList.AddTail((NodeBase *)pNode);
	UNLOCK_MUTEX(&m_sendListMutex);

	LOCK_MUTEX(&m_sendTimer.timerMutex);
	SET_SIGNAL(m_sendTimer.timerCond);
	UNLOCK_MUTEX(&m_sendTimer.timerMutex);

	return SUCCESS;
}

void CICLogical::StartSendThread()
{
	StopSendThread();

	m_bSendThreadRun = true;
	m_hSendThread = CREATE_THREAD(ICSendThread, this);
}

void CICLogical::StopSendThread()
{
	LOCK_MUTEX(&m_sendListMutex);
	m_sendList.DeleteAll();
	UNLOCK_MUTEX(&m_sendListMutex);

	m_bSendThreadRun = false;
	if(m_hSendThread > 0)
	{
		LOCK_MUTEX(&m_sendTimer.timerMutex);
		SET_SIGNAL(m_sendTimer.timerCond);
		UNLOCK_MUTEX(&m_sendTimer.timerMutex);
		WAIT_THREAD_EXIT(m_hSendThread);
		m_hSendThread = 0;
	}
}
