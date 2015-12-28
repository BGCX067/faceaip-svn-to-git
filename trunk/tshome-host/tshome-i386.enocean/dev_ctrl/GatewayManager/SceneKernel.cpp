/*
 * SceneKernel.cpp
 *
 *  Created on: May 18, 2012
 *      Author: kyy
 */

#include "SceneKernel.h"
#include "GatewayManager.h"
#include "common/list.h"
#include "Conversion.h"
#include <netinet/in.h>

void * SceneInputProcess(void * pParam)
{
	__fline printf("Scene input thread Start\n");

	if(pParam == NULL)
	{
		return NULL;
	}

	CSceneKernel *pScene = (CSceneKernel *) pParam;

	while(pScene->IsRun())
	{
		sleep(1);

		while(pScene->IsRun() && pScene->GetInputReqList()->nNodeCount > 0)
		{
			usleep(100 * 1000);
			pScene->DoInputRequest();
		}
	}

	pScene->DeleteDelayReqList();
	__fline printf("Scene input thread Ended\n");

	return NULL;
}


void * SendSceneProcess(void * pParam)
{
	__fline printf("Scene Thread  Start\n");

	if(pParam == NULL)
	{
		return NULL;
	}

	CSceneKernel *pScene = (CSceneKernel *) pParam;
	pScene->SetRun(true);
	SceneNode *pNode = NULL;

	while(pScene->IsRun())
	{
		pScene->WaitForSignal();

		while(pScene->IsRun())
		{
			pNode = pScene->GetListHead();
			if(pNode == NULL)
			{
				break;
			}
			pScene->DoRequest(pNode);
		}
	}

	pScene->DeleteReqList();
	__fline printf("Scene Thread  Ended\n");
	return NULL;
}


void * SendDelayItemProcess(void * pParam)
{
	__fline printf("Scene Delay Item Start\n");

	if(pParam == NULL)
	{
		return NULL;
	}

	CSceneKernel *pScene = (CSceneKernel *) pParam;
	pScene->SetRun(true);

	while(pScene->IsRun())
	{
		sleep(1);

		while(pScene->IsRun() && pScene->GetDelayItemList()->nNodeCount > 0)
		{
			usleep(100 * 1000);
			pScene->DoDelayItemRequest();
		}
	}

	pScene->DeleteDelayReqList();
	__fline printf("Scene Delay Item Ended\n");
	return NULL;
}

CSceneKernel::CSceneKernel()
{
	m_nReqThread = 0;
	m_nDelayReqThread = 0;
	m_nInputReqThread = 0;
	m_bRun = false;

	pthread_cond_init(&m_reqTimerCond.timerCond, NULL);
	pthread_mutex_init(&m_reqTimerCond.timerMutex, NULL);
	pthread_mutex_init(&m_reqListMutex, NULL);
	pthread_mutex_init(&m_delayReqListMutex, NULL);
	pthread_mutex_init(&m_inputReqListMutex, NULL);
}


CSceneKernel::~CSceneKernel()
{
	StopSceneThread();

	pthread_cond_destroy(&m_reqTimerCond.timerCond);
	pthread_mutex_destroy(&m_reqTimerCond.timerMutex);
	pthread_mutex_destroy(&m_reqListMutex);
	pthread_mutex_destroy(&m_delayReqListMutex);
	pthread_mutex_destroy(&m_inputReqListMutex);
}

int CSceneKernel::StartSceneThread()
{
	pthread_create(&m_nReqThread, NULL, SendSceneProcess, this);
	if(m_nReqThread <= 0)
	{
		return FAILED;
	}

	pthread_create(&m_nDelayReqThread, NULL, SendDelayItemProcess, this);
	if(m_nDelayReqThread <= 0)
	{
		return FAILED;
	}

	pthread_create(&m_nInputReqThread, NULL, SceneInputProcess, this);
	if(m_nInputReqThread <= 0)
	{
		return FAILED;
	}

	return SUCCESS;
}

void CSceneKernel::StopSceneThread()
{
	m_bRun = false;
	if(m_nReqThread > 0)
	{
		pthread_mutex_lock(&m_reqTimerCond.timerMutex);
		pthread_cond_signal(&m_reqTimerCond.timerCond);
		pthread_mutex_unlock(&m_reqTimerCond.timerMutex);
		pthread_join(m_nReqThread, NULL);
		m_nReqThread = 0;
	}
	
	if(m_nDelayReqThread > 0)
	{
		pthread_join(m_nDelayReqThread, NULL);
		m_nDelayReqThread = 0;
	}
	
	if(m_nInputReqThread > 0)
	{
		pthread_join(m_nInputReqThread, NULL);
		m_nInputReqThread = 0;
	}
}

int CSceneKernel::WaitForSignal()
{
	pthread_mutex_lock(&m_reqTimerCond.timerMutex);
	m_reqTimerCond.timeout.tv_sec = time(0) + 5;
	m_reqTimerCond.timeout.tv_nsec = 0;
 	int nRes = pthread_cond_timedwait(
			&m_reqTimerCond.timerCond,
			&m_reqTimerCond.timerMutex,
			&m_reqTimerCond.timeout);
	pthread_mutex_unlock(&m_reqTimerCond.timerMutex);
	return nRes;
}

SceneNode * CSceneKernel::GetListHead()
{
	SceneNode *pNode = NULL;

	pthread_mutex_lock(&m_reqListMutex);
	pNode = (SceneNode *) m_reqList.GetAndSeperateHeader();
	pthread_mutex_unlock(&m_reqListMutex);

	return pNode;
}

DelayReqItem * CSceneKernel::GetDelayItemListHead()
{
	DelayReqItem *pItem = NULL;

	pthread_mutex_lock(&m_delayReqListMutex);
	pItem = (DelayReqItem *) m_delayReqList.GetAndSeperateHeader();
	pthread_mutex_unlock(&m_delayReqListMutex);

	return pItem;
}

SceneInputNode * CSceneKernel::GetInputListHead()
{
	SceneInputNode *pInputNode = NULL;

	pthread_mutex_lock(&m_inputReqListMutex);
	pInputNode = (SceneInputNode *) m_inputReqList.GetAndSeperateHeader();
	pthread_mutex_unlock(&m_inputReqListMutex);

	return pInputNode;
}


int CSceneKernel::DoRequest(SceneNode *pNode)
{
	if(pNode != NULL)
	{
		SceneRequest(pNode->nSceneId);
	}
	return SUCCESS;
}

int CSceneKernel::DoDelayItemRequest()
{
	CNodeList m_currList;

	pthread_mutex_lock(&m_delayReqListMutex);
	DelayReqItem * pItem = (DelayReqItem *) m_delayReqList.GetAndSeperateHeader();

	if(pItem == NULL)
	{
		pthread_mutex_unlock(&m_delayReqListMutex);
		return 0;
	}

	while(pItem != NULL && m_bRun)
	{
		if(pItem->time <= time(0))
		{
__fline printf("------------------------------------------delay_time=%ld, time(0)=%ld, dev_id=%d, param=%d, value=%s\n",
		pItem->time, time(0), pItem->reqInfo.dev_id, pItem->reqInfo.param, pItem->reqInfo.value);

			// send request
			SceneItemRequest(pItem->reqInfo.dev_id,
					pItem->reqInfo.param, pItem->reqInfo.value);

			// delete node
			delete pItem;
			pItem = NULL;
		}
		else
		{
			m_currList.AddTail( (NodeBase *) pItem);
		}

		pItem = (DelayReqItem *) m_delayReqList.GetAndSeperateHeader();
	}
	m_delayReqList = m_currList;
	pthread_mutex_unlock(&m_delayReqListMutex);

	return SUCCESS;
}

int CSceneKernel::DoInputRequest()
{
	pthread_mutex_lock(&m_inputReqListMutex);
	SceneInputNode * pInputNode = (SceneInputNode *) m_inputReqList.GetAndSeperateHeader();

	if(pInputNode == NULL)
	{
		pthread_mutex_unlock(&m_inputReqListMutex);
		return 0;
	}

	ts_scene_info_t *pScene_infos = NULL;
	int nCount = 0;
	int nIndex = 0;
	
	while(pInputNode != NULL && m_bRun)
	{
		nCount = 0;
		if(SUCCESS == GetTriggerSceneList(pInputNode, &pScene_infos, &nCount))
		{
			for(nIndex = 0; nIndex < nCount; nIndex++)
			{
				AddSceneRequestNode((pScene_infos + nIndex)->id);
			}
		}

		if(pScene_infos != NULL)
		{
			free(pScene_infos);
			pScene_infos = NULL;
		}
		
		pInputNode = (SceneInputNode *) m_inputReqList.GetAndSeperateHeader();
	}
	pthread_mutex_unlock(&m_delayReqListMutex);
	return SUCCESS;
}

int CSceneKernel::GetTriggerSceneList(SceneInputNode * pInputNode, ts_scene_info_t ** pScene_infos, int * pCount)
{
	// 1. get all of the scenes ralatived of pInputNode.
	*pCount = 0;
	int nTotalCount = 0;
	
	// It should back the value of scene_in_info->threshold.
	int nRes = db_get_scene_id( &(pInputNode->scene_in_info), &nTotalCount, pScene_infos, TS_SCENE_IS_USE);
	if(nRes != EXIT_SUCCESS || nTotalCount <= 0 || *pScene_infos == NULL)
	{
		return nRes;
	}

	// 2. if the condition is not true, return.
	nRes = IsTheValueChangedAndMatch(pInputNode);

	// condition didn't change.
	if(nRes != SUCCESS)
	{
		return SUCCESS;
	}

	// 3. filter those scenes who couldn't be triggered.
	int nIndex = 0;
	*pCount = 0;
	bool bTrigger = false;
	for(nIndex = 0; m_bRun && nIndex < nTotalCount; nIndex++)
	{
		bTrigger = false;
		if( (*pScene_infos + nIndex)->is_use == TS_SCENE_IS_USE)
		{
 			if((*pScene_infos + nIndex)->and_or == TS_CONDITION_OR)
			{
				bTrigger = true;
			}
			else if((*pScene_infos + nIndex)->and_or == TS_CONDITION_AND)
			{
				// get all conditions and test whether they are matched respectively.
				nRes = IsAllAndConditionsMatch((*pScene_infos + nIndex)->id, true, pInputNode);
				if(nRes == SUCCESS)
				{
					bTrigger = true;
				}
			}

			// 4. it's confirmed to trigger
			if(*pCount != nIndex && bTrigger == true)
			{
				*(*pScene_infos + *pCount) = *(*pScene_infos + nIndex);
				(*pCount)++;
			}
		}
	}

	return SUCCESS;
}

int CSceneKernel::IsTheValueChangedAndMatch(SceneInputNode * pInputNode)
{
	if(pInputNode == NULL)
	{
		return FAILED;
	}	

	int nRes = FAILED;
	switch(pInputNode->main_type)
	{
	case KNX:
		nRes = IsKnxConditionMatch(pInputNode, false);
		break;
	case INFRARED:
	case COM:
	case CUSTOMER_COM:
		nRes = SUCCESS;
		break;
	case SERIAL_RES:
		// not used
		//nRes = IsSerialResConditionMatch(pInputNode);
		break;
	}
	
	return nRes;
}

int CSceneKernel::IsKnxConditionMatch(SceneInputNode * pInputNode, bool bJustMatchCurrVal)
{
	if(pInputNode->main_type != KNX)
	{
		return FAILED;
	}

	ts_knx_control_info_t knxData;
	memset(&knxData, 0, sizeof(ts_knx_control_info_t));
    knxData.home_device_id = pInputNode->scene_in_info.dev_id;
    knxData.group_addr = pInputNode->scene_in_info.param;
    int nRes = db_get_knx_device_curr_value(&knxData);

	// if get current value failed or the group type has changed,
	// then do nothing.
	if(nRes != EXIT_SUCCESS 
		|| (bJustMatchCurrVal == false && pInputNode->sub_type != knxData.data_type))
	{
		return FAILED;
	}
					
	switch(knxData.data_type)
	{
	case DPT_Boolean:
	case DPT_1BitCtrl:
	case DPT_3BitCtrl:
	case DPT_8BitUnSign:
	case DPT_8BitSign:
	case DPT_2OctUnSign:
	case DPT_2OctSign:
	case DPT_2OctFloat:
	case DPT_4OctUnSign:
	case DPT_4OctSign:
	case DPT_4OctFloat:
	case DPT_Access:
		{
			float fCurrVal = atof( (char *) knxData.data);

			// get the symbol from the value string. value's format is like ">1", ">=10", "=100".
			int nSymbol = EQUAL;
			int nSymbolCount = 0;
			if(pInputNode->scene_in_info.value[0] == '>')
			{
				if(pInputNode->scene_in_info.value[1] == '=')
				{
					nSymbol = GREATER_EQUAL;
					nSymbolCount = 2;
				}
				else
				{
					nSymbol = GREATER_THAN;
					nSymbolCount = 1;
				}
			}
			else if(pInputNode->scene_in_info.value[0] == '<')
			{
				if(pInputNode->scene_in_info.value[1] == '=')
				{
					nSymbol = LESS_EQUAL;
					nSymbolCount = 2;
				}
				else
				{
					nSymbol = LESS_THAN;
					nSymbolCount = 1;
				}
			}
 			else if(pInputNode->scene_in_info.value[0] == '!' && pInputNode->scene_in_info.value[1] == '=')
			{
				nSymbol = NOT_EQUAL;
				nSymbolCount = 2;
			}
			else if(pInputNode->scene_in_info.value[0] == '=')
			{
				nSymbol = EQUAL;
				nSymbolCount = 1;
			}
			else
			{
				return FAILED;
			}

			// get the match value.
			float fMatchVal = atof((char *) pInputNode->scene_in_info.value + nSymbolCount);
				
			if( pInputNode->is_first_use == TS_KNX_VALUE_FIRST_USE
				|| bJustMatchCurrVal == true)
			{
				switch(nSymbol)
				{
				case GREATER_THAN:
					if(fCurrVal > fMatchVal + pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}
				case LESS_THAN:
					if(fCurrVal < fMatchVal - pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}
				case EQUAL:
					if(fCurrVal >= fMatchVal - pInputNode->scene_in_info.threshold
						&& fCurrVal <= fMatchVal + pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}
				case NOT_EQUAL:
					if(fCurrVal < fMatchVal - pInputNode->scene_in_info.threshold
						&& fCurrVal > fMatchVal + pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}
				case GREATER_EQUAL:
					if(fCurrVal >= fMatchVal - pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}
				case LESS_EQUAL:
					if(fCurrVal <= fMatchVal + pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}				
				}
			}
			else if(pInputNode->is_first_use == TS_KNX_VALUE_NOT_FIRST_USE)
			{
				float fOrigVal = atof(pInputNode->original_value);
				switch(nSymbol)
				{
				case GREATER_THAN:
					if(fOrigVal <= fMatchVal + pInputNode->scene_in_info.threshold
						&& fCurrVal > fMatchVal + pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}
				case LESS_THAN:
					if(fOrigVal >= fMatchVal - pInputNode->scene_in_info.threshold
					&& fCurrVal < fMatchVal - pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}
				case EQUAL:
					if( (fOrigVal < fMatchVal - pInputNode->scene_in_info.threshold
					|| fOrigVal > fMatchVal + pInputNode->scene_in_info.threshold)
						&& fCurrVal >= fMatchVal - pInputNode->scene_in_info.threshold
						&& fCurrVal <= fMatchVal + pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}
				case NOT_EQUAL:
					if( (fOrigVal >= fMatchVal - pInputNode->scene_in_info.threshold
							&& fOrigVal <= fMatchVal + pInputNode->scene_in_info.threshold)
						&& (fCurrVal < fMatchVal - pInputNode->scene_in_info.threshold
							|| fCurrVal > fMatchVal + pInputNode->scene_in_info.threshold))
					{
						return SUCCESS;
					}
				case GREATER_EQUAL:
					if(fOrigVal < fMatchVal - pInputNode->scene_in_info.threshold
						&& fCurrVal >= fMatchVal - pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}
				case LESS_EQUAL:
					if(fOrigVal > fMatchVal + pInputNode->scene_in_info.threshold
						&& fCurrVal <= fMatchVal + pInputNode->scene_in_info.threshold)
					{
						return SUCCESS;
					}
				}
			}
		}
			break;
		case DPT_Time:
		case DPT_Date:
			break;
		case DPT_String:
			break;
	}
	return FAILED;
}

/*
int CSceneKernel::IsSerialResConditionMatch(SceneInputNode * pInputNode)
{
	return 0;
}
*/

int CSceneKernel::IsAllAndConditionsMatch(int nSceneId, bool bExceptSelf, SceneInputNode * pInputSelfNode)
{
	if(bExceptSelf == true && pInputSelfNode == NULL)
	{
		return FAILED;
	}
	
	int nCount = 0;
	ts_scene_io_info_t * in_infos = NULL;
	int nRes = db_get_all_scene_inputs(nSceneId, &nCount, &in_infos);
	if(nRes != EXIT_SUCCESS || nCount <= 0 || in_infos == NULL)
	{
		return FAILED;
	}

	int index = 0;
	int nDevId = 0;
	int nDevType = 0;
	bool bFlag = false;
	SceneInputNode * pInputNode = new SceneInputNode;
	if(pInputNode == NULL)
	{
		return FAILED;
	}
	memset(pInputNode, 0, sizeof(SceneInputNode));
	
	for(index = 0; index < nCount; index++)
	{
		nDevId = (in_infos + index)->dev_id;
		nDevType = 0;
		nRes = db_get_device_type_by_id(&nDevType, nDevId);

		// excepet itself
		if(bExceptSelf == true
			&& nDevId == pInputSelfNode->scene_in_info.dev_id
			&& (in_infos + index)->param == pInputSelfNode->scene_in_info.param
			&& strcmp((in_infos + index)->value, pInputSelfNode->scene_in_info.value) == 0
			&& (in_infos + index)->delay == pInputSelfNode->scene_in_info.delay
			&& (in_infos + index)->threshold == pInputSelfNode->scene_in_info.threshold)
		{
			// if it's only one condition which is "pInputSelfNode->scene_in_info" itself, it should be true.
			bFlag = true;
			continue;
		}
		
		switch(nDevType)
		{
		case KNX:
			pInputNode->scene_in_info = *(in_infos + index);
			nRes = IsKnxConditionMatch(pInputNode, true);
			if(nRes != SUCCESS)
			{
				// to break the for loop.
				index = nCount + 1;
				bFlag = FAILED;
			}
			else
			{
				bFlag = true;
			}
			break;
		case INFRARED:
		case COM:
		case CUSTOMER_COM:
		case SERIAL_RES:
			// to break the for loop.
			index = nCount + 1;
			bFlag = FAILED;
			break;
		}
	}

	if(pInputNode != NULL)
	{
		delete pInputNode;
		pInputNode = NULL;
	}

	if(in_infos != NULL)
	{
		free(in_infos);
		in_infos = NULL;
	}
	
	return bFlag;
}

int CSceneKernel::SceneRequest(int nSceneId)
{
	int nRes = 0;
	LIST_HEAD(scene_items);
	ts_scene_io_info_list_t *pItem = NULL;
	nRes = db_get_scene_infos(&scene_items, pItem, nSceneId);
	__fline printf("--------------db_get_scene_infos      nRes=%d\n", nRes);
	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	pItem = NULL;
	ts_scene_io_info_list_t *pItemTmp = NULL;

	list_for_each_entry_safe(pItem, pItemTmp, &scene_items, list)
	{
		if(!m_bRun)
		{
			break;
		}

		if(pItem != NULL)
		{
			if(pItem->scene_info.delay > 0)
			{
				// add to delay-item list
				AddDelayItemRequestNode(&pItem->scene_info);
			}
			else
			{
				// send to host
				usleep(100 * 1000);
				SceneItemRequest(pItem->scene_info.dev_id,
						pItem->scene_info.param,
						pItem->scene_info.value);
			}
		}
	}

	list_for_each_entry_safe(pItem, pItemTmp, &scene_items, list)
	{
		if(pItem != NULL)
		{
			list_del(&pItem->list);
			free(pItem);
		}
	}

	return SUCCESS;
}

int CSceneKernel::SceneItemRequest(int nDevId, int nParam, char * strValue)
{
	int nDevType = -1;
	int nRes = db_get_device_type_by_id(&nDevType, nDevId);
__fline printf("--------------db_get_device_type_by_id      nRes=%d\n", nRes);
	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	char *pMsg = NULL;
	int nMsgLen = 0;
	int nBodyLen = 0;
	knx_control_report_t *pKnxReq = NULL;
	com_control_req_t *pComReq = NULL;
	wifi_control_req_t *pInfraredReq = NULL;

	kic_head_t kic_head;
	kic_head.header.start = HEAD_START;
	kic_head.header.protocol_type = 0;
	kic_head.header.length = 0;
	kic_head.header.serial_number = 0;
	kic_head.header.function_code = DEVICE_OPERATION;
	kic_head.header.comm_type = NOTRESPONSE;
	kic_head.header.comm_source = HOST_S;

	kic_head.session_id = 0;
	kic_head.device_type = nDevType;
	kic_head.home_device_id = htonl(nDevId);


__fline printf("--------SceneRequest=(nDevId=%d, nDevType=%d, param=%d, strvalue=%s)\n",
		nDevId,
		kic_head.device_type,
		nParam,
		strValue);

	nRes = ERR_PROTROL_NOT_SUPPORT;
	switch(nDevType)
	{
	case KNX:
		nBodyLen = sizeof(knx_control_report_t) - sizeof(remote_header_t);
		kic_head.header.length = htons(nBodyLen);

		pKnxReq = new knx_control_report_t;
		memset(pKnxReq, 0, sizeof(knx_control_report_t));
		memcpy(pKnxReq, &kic_head, sizeof(kic_head));

		pKnxReq->cmd_type = WRITE;
		pKnxReq->address = htons(nParam);
		*((int *) pKnxReq->data) = htonl(atoi(strValue));
		pKnxReq->data_len = 4;

		// get knx data type from db
		ts_knx_control_info_t knxInfo;
		knxInfo.home_device_id = nDevId;
		knxInfo.group_addr = nParam;
		nRes = db_get_knx_device_curr_value(&knxInfo);
__fline printf("--------------db_get_knx_device_curr_value      nRes=%d\n", nRes);
		if(nRes != EXIT_SUCCESS)
		{
			delete pKnxReq;
			pKnxReq = NULL;
			return nRes;
		}

		knxInfo.home_device_id = htonl(nDevId);
		knxInfo.group_addr = htons(nParam);
		pKnxReq->data_type = knxInfo.data_type;

		pMsg = (char *) pKnxReq;
		nMsgLen = sizeof(knx_control_report_t);
		break;
	case COM:
	case CUSTOMER_COM:
	{
		nBodyLen = sizeof(com_control_req_t) - sizeof(remote_header_t);
		kic_head.header.length = htons(nBodyLen);

		pComReq = new com_control_req_t;
		memset(pComReq, 0, sizeof(com_control_req_t));
		memcpy(pComReq, &kic_head, sizeof(kic_head));

		if(nDevType == COM)
		{
			int nCtrlType = 0;
			int nData = 0;
			nRes = GetCtrlTypeAndValue(nParam, nCtrlType, nData, strValue);
			if(nRes != SUCCESS)
			{
				return nRes;
			}
			pComReq->com_ctrl_type = nCtrlType;
			pComReq->data = htonl(nData);
		}
		else
		{
			pComReq->data = htonl(nParam);
		}

		pMsg = (char *) pComReq;
		nMsgLen = sizeof(com_control_req_t);
		break;
	}
	case INFRARED:
		nBodyLen = sizeof(wifi_control_req_t) - sizeof(remote_header_t);
		kic_head.header.length = htons(nBodyLen);

		pInfraredReq = new wifi_control_req_t;
		memset(pInfraredReq, 0, sizeof(wifi_control_req_t));
		memcpy(pInfraredReq, &kic_head, sizeof(kic_head));

		pInfraredReq->function_code = ntohs(nParam);

		pMsg = (char *) pInfraredReq;
		nMsgLen = sizeof(wifi_control_req_t);
		break;
	}

	if(pMsg != NULL && nMsgLen > 0)
	{
		protocol_request(pMsg, nMsgLen);

		if(pKnxReq != NULL)
		{
			delete pKnxReq;
			pKnxReq = NULL;
		}
		else if(pComReq != NULL)
		{
			delete pComReq;
			pComReq = NULL;
		}
		else if(pInfraredReq != NULL)
		{
			delete pInfraredReq;
			pInfraredReq = NULL;
		}
	}

	return SUCCESS;
}

int CSceneKernel::GetCtrlTypeAndValue(int nKeyFun, int &nCtrlType, int &nData, char * strValue)
{
	// get key function name from db.
	char strKeyName[MAX_KEY_FUN_NAME_LEN] = {'\0'};
	int nRes = -1;
	nRes = db_get_key_func_name(nKeyFun, strKeyName);
	if(nRes != EXIT_SUCCESS)
	{
		return nRes;
	}

	if(strcmp(strKeyName, "KEY_OPEN") == 0)
	{
		nCtrlType = CTRL_SWITCH;
		nData = SWITCH_ON;
	}
	else if(strcmp(strKeyName, "KEY_CLOSE") == 0)
	{
		nCtrlType = CTRL_SWITCH;
		nData = SWITCH_OFF;
	}
	else if(strcmp(strKeyName, "KEY_PREVIOUS") == 0)
	{
		nCtrlType = CTRL_PREV_NEXT;
		nData = PREV;
	}
	else if(strcmp(strKeyName, "KEY_NEXT") == 0)
	{
		nCtrlType = CTRL_PREV_NEXT;
		nData = NEXT;
	}
	else if(strcmp(strKeyName, "KEY_PLAY") == 0)
	{
		nCtrlType = CTRL_PLAY;
		nData = PLAY;
	}
	else if(strcmp(strKeyName, "KEY_PAUSE") == 0)
	{
		nCtrlType = CTRL_PLAY;
		nData = PAUSE;
	}
	else if(strcmp(strKeyName, "KEY_STOP") == 0)
	{
		nCtrlType = CTRL_PLAY;
		nData = STOP;
	}
	else if(strcmp(strKeyName, "KEY_TUNEUP") == 0)
	{
		nCtrlType = CTRL_VOLUME;
		nData = VOLUME_UP;
	}
	else if(strcmp(strKeyName, "KEY_TUNEDOWN") == 0)
	{
		nCtrlType = CTRL_VOLUME;
		nData = VOLUME_DOWN;
	}
	else if(strcmp(strKeyName, "KEY_TUNEUP_TREBLE") == 0)
	{
		nCtrlType = CTRL_TREBLE_VOL;
		nData = VOLUME_UP;
	}
	else if(strcmp(strKeyName, "KEY_TUNEDOWN_TREBLE") == 0)
	{
		nCtrlType = CTRL_TREBLE_VOL;
		nData = VOLUME_DOWN;
	}
	else if(strcmp(strKeyName, "KEY_TUNEUP_BASS") == 0)
	{
		nCtrlType = CTRL_BASS_VOL;
		nData = VOLUME_UP;
	}
	else if(strcmp(strKeyName, "KEY_TUNEDOWN_BASS") == 0)
	{
		nCtrlType = CTRL_BASS_VOL;
		nData = VOLUME_DOWN;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_MP3") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_MP3;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_TNR") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_TNR;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_DVD") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_DVD;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_PC") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_PC;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_PC2") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_PC_2;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_TV") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_TV;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_AUX") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_AUX;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_IPOD") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_IPOD;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_AM") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_AM;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_FM") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_FM;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_DVI_HDMI") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_DVI_HDMI;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_HDMI") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_HDMI;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_VIDEO") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_VIDEO;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_S_VIDEO") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_S_VIDEO;
	}
	else if(strcmp(strKeyName, "KEY_SOURCE_BNC") == 0)
	{
		nCtrlType = CTRL_SOURCE;
		nData = SOURCE_BNC;
	}
	else if(strcmp(strKeyName, "KEY_EQ_NORMAL") == 0)
	{
		nCtrlType = CTRL_EQ_MODE;
		nData = EQ_NORMAL;
	}
	else if(strcmp(strKeyName, "KEY_EQ_POP") == 0)
	{
		nCtrlType = CTRL_EQ_MODE;
		nData = EQ_POP;
	}
	else if(strcmp(strKeyName, "KEY_EQ_SOFT") == 0)
	{
		nCtrlType = CTRL_EQ_MODE;
		nData = EQ_SOFT;
	}
	else if(strcmp(strKeyName, "KEY_EQ_CLASSIC") == 0)
	{
		nCtrlType = CTRL_EQ_MODE;
		nData = EQ_CLASSIC;
	}
	else if(strcmp(strKeyName, "KEY_EQ_JAZZ") == 0)
	{
		nCtrlType = CTRL_EQ_MODE;
		nData = EQ_JAZZ;
	}
	else if(strcmp(strKeyName, "KEY_EQ_METAL") == 0)
	{
		nCtrlType = CTRL_EQ_MODE;
		nData = EQ_METAL;
	}
	else if(strcmp(strKeyName, "KEY_CHANNEL_AM") == 0)
	{
		nCtrlType = CTRL_CHANNEL_AM;
		nData = *((int *) strValue);
	}
	else if(strcmp(strKeyName, "KEY_CHANNEL_FM") == 0)
	{
		nCtrlType = CTRL_CHANNEL_FM;
		nData = *((int *) strValue);
	}
	else
	{
		return FAILED;
	}

	return SUCCESS;
}

int CSceneKernel::AddSceneInputNode(SceneInputNode inputNode)
{
	if(!m_bRun)
	{
		return FAILED;
	}

	// new a scene input request node
	SceneInputNode *pInputNode = new SceneInputNode;
	if(pInputNode == NULL)
	{
		return FAILED;
	}

	memset(pInputNode, 0, sizeof(SceneInputNode));
	*pInputNode = inputNode;

	pthread_mutex_lock(&m_inputReqListMutex);
	m_inputReqList.AddTail( (NodeBase *) pInputNode);
	pthread_mutex_unlock(&m_inputReqListMutex);

	return SUCCESS;
}

int CSceneKernel::AddSceneRequestNode(int nSceneId)
{
	if(m_bRun)
	{
		SceneNode *pNode = new SceneNode();
		memset(pNode, 0, sizeof(SceneNode));
		pNode->nSceneId = nSceneId;

		pthread_mutex_lock(&m_reqListMutex);
		m_reqList.AddTail((NodeBase*) pNode);
		pthread_mutex_unlock(&m_reqListMutex);

		__fline printf("Trigger a scene id = %d\n", pNode->nSceneId);

		pthread_mutex_lock(&m_reqTimerCond.timerMutex);
		pthread_cond_signal(&m_reqTimerCond.timerCond);
		pthread_mutex_unlock(&m_reqTimerCond.timerMutex);
		return SUCCESS;
	}

	return FAILED;
}

int CSceneKernel::AddDelayItemRequestNode(ts_scene_io_info_t *pItem)
{
	if(m_bRun)
	{
		DelayReqItem *pNewItem = new DelayReqItem();
		memset(pNewItem, 0, sizeof(DelayReqItem));
		pNewItem->reqInfo.dev_id = pItem->dev_id;
		pNewItem->reqInfo.param = pItem->param;
		strncpy(pNewItem->reqInfo.value, pItem->value, 32);
		pNewItem->reqInfo.delay = pItem->delay;

		pNewItem->time = time(0) + pItem->delay;

		pthread_mutex_lock(&m_delayReqListMutex);
		m_delayReqList.AddTail((NodeBase*) pNewItem);
		pthread_mutex_unlock(&m_delayReqListMutex);

		return SUCCESS;
	}

	return FAILED;
}

int CSceneKernel::DeleteReqList()
{
	pthread_mutex_lock(&m_reqListMutex);
	m_reqList.DeleteAll();
	pthread_mutex_unlock(&m_reqListMutex);
	return SUCCESS;
}

int CSceneKernel::DeleteDelayReqList()
{
	pthread_mutex_lock(&m_delayReqListMutex);
	m_delayReqList.DeleteAll();
	pthread_mutex_unlock(&m_delayReqListMutex);
	return SUCCESS;
}
