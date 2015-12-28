      //
//  KNXKernel.cpp
//  tsbus_knx
//
//  Created by kyy on 11-5-20.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#include "KNXKernel.h"
#include <string.h>

#ifdef _WIN32
#else
#include <ifaddrs.h>
#include <errno.h>
#include <sys/time.h>
#endif

#include "GatewayManager.h"
#include "db/ts_db_protocol_infos.h"

#include <stdio.h>

extern FILE* g_log;


	int CKNXKernel::ConnectStateTimer(int nTimeout)
	{
		int nReturn = BEAT_CONNECTSTATE_EXIT;
		while(!m_bExitBeat)
		{
			int nRes = -1;

			LOCK_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
#ifdef _WIN32
			m_timeout[TIMER_CONNECTSTATE] = nTimeout * 1000;
			m_bWaitObject[TIMER_CONNECTSTATE] = true;

			nRes = WaitForMultipleObjects(2, m_beatCond, FALSE,
							m_timeout[TIMER_CONNECTSTATE]);

			m_bWaitObject[TIMER_CONNECTSTATE] = false;
			
			if(nRes == WAIT_OBJECT_0 + 1)
			{
				UNLOCK_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
				nReturn = BEAT_CONNECTSTATE_EXIT;
				break;
			}
			else if(nRes == WAIT_OBJECT_0)
			{
				nRes = 0;
			}
#else
			m_timeout[TIMER_CONNECTSTATE].tv_sec = time(0) + nTimeout;
			m_timeout[TIMER_CONNECTSTATE].tv_nsec = 0;
			m_bWaitObject[TIMER_CONNECTSTATE] = true;
			//m_bConStateOn = true;	// do not wait for the signal, just wait for nTimeout
			nRes = pthread_cond_timedwait(&m_timerCond[TIMER_CONNECTSTATE],
										  &m_timerMutex[TIMER_CONNECTSTATE], 
										  &m_timeout[TIMER_CONNECTSTATE]);
			m_bWaitObject[TIMER_CONNECTSTATE] = false;
			//m_bConStateOn = false;
			if(nRes == 0 && m_bExitBeat)
			{
				UNLOCK_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
				nReturn = BEAT_CONNECTSTATE_EXIT;
				break;
			}
#endif
			UNLOCK_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
			
			// if received a certain tunnel response successfully,
			// restart the timer.
			/*if(nRes == 0)
			{
				nTimeout = 60; // 60
				continue;
			}
			else
			{
				nReturn = BEAT_CONNECTSTATE;
				break;
			}*/
			nReturn = BEAT_CONNECTSTATE;
			break;
		}
		return nReturn;
	}
	
	int DetectBreath(void *pParam)
	{
		// Start a Event_Timeout timer
		// Event: received the response, send a tunnelling request message
		// if successful responsed, reset the timeout.
		// if failed 2 times, stop the connection.
		// timer: 60s
		if(pParam == 0)
		{
			return -1;
		}
		
		CKNXKernel *pKNXKernel = (CKNXKernel *) pParam;
		pKNXKernel->m_nConStateMsgLen = sizeof(pKNXKernel->m_conStateMsg);
		pKNXKernel->GenerateConnectStateMsg(pKNXKernel->m_conStateMsg, pKNXKernel->m_nConStateMsgLen);
		int nRes = -1;
		while(pKNXKernel->m_udpCtrl.IsRunning() && !pKNXKernel->m_bExitBeat)
		{
			// send a breath detecting message.
			// send success or not, we will wait until
			// pKNXKernel->m_nStateTimeoutCount equals 2 to disconnect.
			LOCK_MUTEX(&pKNXKernel->m_timerMutex[TIMER_CONNECTSTATE]);
			nRes = pKNXKernel->m_udpCtrl.UDPSend(pKNXKernel->m_conStateMsg, pKNXKernel->m_nConStateMsgLen);
            
            // if send heart beat failed, re-connect
            if (-1 == nRes)
            {
#ifndef _WIN32
                DEBUG_LOG0(KNXKernel, Heart beat datagram sended failed & try to re-connect!);
#endif
				UNLOCK_MUTEX(&pKNXKernel->m_timerMutex[TIMER_CONNECTSTATE]);
                pKNXKernel->ToDisconnect(true, true);
                break;
            }
			
#ifdef _WIN32
			pKNXKernel->m_timeout[TIMER_CONNECTSTATE] = 30 * 1000;
			pKNXKernel->m_bWaitObject[TIMER_CONNECTSTATE] = true;

			nRes = WaitForMultipleObjects(2, pKNXKernel->m_beatCond, FALSE,
										  pKNXKernel->m_timeout[TIMER_CONNECTSTATE]);

			pKNXKernel->m_bWaitObject[TIMER_CONNECTSTATE] = false;

			if(nRes == WAIT_OBJECT_0 + 1)
			{
				//exit
				UNLOCK_MUTEX(&pKNXKernel->m_timerMutex[TIMER_CONNECTSTATE]);
				break;
			}
			else if(nRes == WAIT_OBJECT_0)
			{
				nRes = 0;
			}
#else
			pKNXKernel->m_timeout[TIMER_CONNECTSTATE].tv_sec = time(0) + 30;
			pKNXKernel->m_timeout[TIMER_CONNECTSTATE].tv_nsec = 0;
			pKNXKernel->m_bWaitObject[TIMER_CONNECTSTATE] = true;
			nRes = pthread_cond_timedwait(&pKNXKernel->m_timerCond[TIMER_CONNECTSTATE],
										  &pKNXKernel->m_timerMutex[TIMER_CONNECTSTATE], 
										  &pKNXKernel->m_timeout[TIMER_CONNECTSTATE]);
			pKNXKernel->m_bWaitObject[TIMER_CONNECTSTATE] = false;
			if(nRes == 0 && pKNXKernel->m_bExitBeat)
			{
				UNLOCK_MUTEX(&pKNXKernel->m_timerMutex[TIMER_CONNECTSTATE]);
				break;
			}
#endif
			UNLOCK_MUTEX(&pKNXKernel->m_timerMutex[TIMER_CONNECTSTATE]);

			
			printf("----[%d] heart beat [%d]--------\n", pKNXKernel->GetGatewayKey(), nRes);
			
			// if we have received a connection state response message,
			// or we have received some other responses.
			if(nRes == 0 && pKNXKernel->m_nStatus[TIMER_CONNECTSTATE] == E_NO_ERROR)
			{
				// oh, it's alive.
				// reset the count and restart event_timeout timer.
				pKNXKernel->m_nStateTimeoutCount = 0;
				
				// modified by keyy 2011/12/31
#ifdef _WIN32
				nRes = pKNXKernel->ConnectStateTimer(30);
#else
				nRes = pKNXKernel->ConnectStateTimer(
					pKNXKernel->m_timeout[TIMER_CONNECTSTATE].tv_sec - time(0)); // 30
#endif
				if(nRes == BEAT_CONNECTSTATE_EXIT)
				{
					// exit
					break;
				}
			}
			else
			{
				// timeout or response error
				pKNXKernel->m_nStatus[TIMER_CONNECTSTATE] = E_NO_ERROR;
				pKNXKernel->m_nStateTimeoutCount++;
				if(pKNXKernel->m_nStateTimeoutCount < 4)
				{
					// modified by keyy 2011/12/31
					// wait 0 second, if timeout
					continue;
					/*
					// if we received a response, but it had some errors.
					// including (nRes == 0 && m_nStatus[TIMER_CONNECTSTATE] != E_NO_ERROR),
					// timeout, and other situations.
#ifdef _WIN32
					nRes = pKNXKernel->ConnectStateTimer(50);  // 50
#else
					nRes = pKNXKernel->ConnectStateTimer(50);
					//nRes = pKNXKernel->ConnectStateTimer(m_timeout[TIMER_CONNECTSTATE].tv_sec - time(0) + 50);  // 50
#endif
					if(nRes == BEAT_CONNECTSTATE_EXIT)
					{
						// exit
						break;
					}
					*/
				}
				else 
				{
					pKNXKernel->ToDisconnect(true, true, false);
					break;
				}
			}
		}
		
#ifdef _WIN32
		OutputDebugString(_T("--------heart beat END--------\n"));
#else
		printf("--------heart beat END--------\n");
#endif
		return 0;
	}
	
	void CKNXKernel::WaitNull(int nMillSec)
	{
#ifdef _WIN32
		Sleep(nMillSec);
#else
		usleep(nMillSec * 1000);
#endif
	}
	
	int KnxSendListProcess(void *pParam)
	{
		__fline printf("--------Knx Send Thread Start--------\n");
		if(pParam == 0)
		{
			return -1;
		}
		
		CKNXKernel *pKNXKernel = (CKNXKernel *) pParam;
		int nRes = 0;
		KNX_GroupData *pGrpData = 0;

		while(pKNXKernel->m_bSendThread)
		{
			LOCK_MUTEX(&pKNXKernel->m_SendListMutex);
			SendNode *pNode = (SendNode *) pKNXKernel->m_sendList.GetAndSeperateHeader();
			UNLOCK_MUTEX(&pKNXKernel->m_SendListMutex);
			
			if(pNode == 0)
			{
				sleep(1);
				continue;
			}
			
			try
			{
				if(pNode->nPrefixLen > 0)
				{
					pGrpData = (KNX_GroupData *) (pNode->buf + pNode->nPrefixLen);
				}
				else
				{
					pGrpData = (KNX_GroupData *) pNode->buf;
				}

				if(pNode->nAction == READ)
				{
//					if(pNode->nPrefixLen > 0)
//					{
//						// has no this operation here.
//						nRes = CGatewayManager::Instance()->GetKnxDataFromDB(
//								pKNXKernel->GetGatewayKey(), pGrpData);
//
//						memset(pNode->buf + pNode->nPrefixLen, 0, sizeof(KNX_GroupData));
//						((knx_control_report_t *)pNode->buf)->result_code = nRes;
//						if(nRes == 0)
//						{
//							SetGroupdataValue(
//									((knx_control_report_t *)pNode->buf)->data, *pGrpData);
//						}
//						pKNXKernel->AddHandleListNode(ACTION_KNX_EXTRA_RES,
//								pNode->buf, pNode->nLen - pNode->nPrefixLen);
//					}

					if(pNode->nPrefixLen == 0)
					{
						nRes = pKNXKernel->DoReadGrpVal(pGrpData->nGroupAddr);
					}
				}
				else if(pNode->nAction == WRITE)
				{
					nRes = pKNXKernel->DoWriteGrpVal(*pGrpData);
				}

				// added:
				if(pNode->nPrefixLen > 0)
				{
					// add a type of nodes : ACTION_KNX_EXTRA_RES
					memset(pNode->buf + pNode->nPrefixLen, 0, sizeof(KNX_GroupData));
					((knx_control_report_t *)pNode->buf)->result_code = E_NO_ERROR;

					if(((knx_control_report_t *)pNode->buf)->kic_head.header.comm_type
							== REQUEST)
					{
						pKNXKernel->AddHandleListNode(ACTION_KNX_EXTRA_RES,
								pNode->buf, pNode->nPrefixLen);
					}
				}

				if(TIME_OUT_2 == nRes)
				{
#ifdef _WIN32
					OutputDebugString(_T("--------Send Thread TIME_OUT_2--------\n"));
#else
					printf("--------Send Thread TIME_OUT_2--------\n");
#endif
					// Re-read the pNode when connected again.
					LOCK_MUTEX(&pKNXKernel->m_SendListMutex);
					pKNXKernel->m_sendList.AddHead((NodeBase *)pNode);
					UNLOCK_MUTEX(&pKNXKernel->m_SendListMutex);

					// exit thread
					break;
				}
			}
			catch(CKNXException *e)
			{
				delete pNode;
				pNode = 0;
				printf("    ----request failed : %s----    ", e->GetExceptionString());
				pKNXKernel->m_bSendThread = false;

				if(pNode->nPrefixLen > 0)
				{
					// add a type of nodes : ACTION_KNX_EXTRA_RES
					memset(pNode->buf + pNode->nPrefixLen, 0, sizeof(KNX_GroupData));
					((knx_control_report_t *)pNode->buf)->result_code = e->GetExceptionId();
					if(((knx_control_report_t *)pNode->buf)->kic_head.header.comm_type
						== REQUEST)
					{
						pKNXKernel->AddHandleListNode(ACTION_KNX_EXTRA_RES,
								pNode->buf, pNode->nLen - pNode->nPrefixLen);
					}
				}
				else if(pNode->nAction == WRITE && pKNXKernel->m_pErrorFun != 0)
				{
					pKNXKernel->m_pErrorFun(pKNXKernel->GetGatewayKey(),
							e->GetExceptionId(),
							e->GetExceptionString(),
							pNode->nAction);
				}

				pKNXKernel->ToDisconnect(true, true);
				break;
			}
			
			delete pNode;
			pNode = 0;

			pKNXKernel->WaitNull(100);
		}
		
#ifdef _WIN32
		OutputDebugString(_T("--------Knx Send Thread END--------\n"));
#else
		printf("--------Knx Send Thread END--------\n");
#endif
		return 0;
	}
	
	int KnxHandleListProcess(void *pParam)
	{
		__fline printf("--------Knx Handle Thread Start--------\n");
		if(pParam == 0)
		{
			return -1;
		}

		CKNXKernel *pKnx = (CKNXKernel *) pParam;
		pKnx->SetHandleProcess(true);

		CGatewayManager *pGetwayMan = CGatewayManager::Instance();
		int nRes = -1;

#ifndef _WIN32
		struct timespec timeSpec;
#endif

		while(pKnx->IsHandleProcess())
		{
			LOCK_MUTEX(pKnx->GetHandleListMutex());
#ifdef _WIN32
			WaitForSingleObject(pKnx->GetHandleListCond(), 5 * 1000);
#else
			timeSpec.tv_sec = time(0) + 5;
			timeSpec.tv_nsec = 0;
			pthread_cond_timedwait(&pKnx->GetHandleListCond(),
										  pKnx->GetHandleListMutex(), &timeSpec);
#endif
			UNLOCK_MUTEX(pKnx->GetHandleListMutex());

			if(!pKnx->IsHandleProcess())
			{
				break;
			}

			LOCK_MUTEX(pKnx->GetHandleListMutex());
			RecvNode *pRecvNode = (RecvNode *) pKnx->GetHandleList()->GetAndSeperateHeader();
			UNLOCK_MUTEX(pKnx->GetHandleListMutex());

			while(pKnx->IsHandleProcess() && pRecvNode != 0)
			{
				switch(pRecvNode->nResType)
				{
				case ACTION_KNX_SEARCH_RES:
					pKnx->HandleSearchResponse(pRecvNode->buf, pRecvNode->nLen);
					break;
				case ACTION_KNX_READ_RES:
				{
					KNX_GroupData *pGrpData = (KNX_GroupData *) pRecvNode->buf;

					ts_knx_control_info_t knx_control;
					knx_control.group_addr = pGrpData->nGroupAddr;
					nRes = db_get_knx_group_type(pKnx->GetGatewayKey(), &knx_control);

					if(nRes == EXIT_SUCCESS)
					{
						if(pKnx->GetReadRes() != 0)
						{
							// get knx device id by gateway id and group address
							pKnx->GetReadRes()(knx_control.home_device_id, *pGrpData);
						}

						// do not Trigger a scene
					}
					break;
				}
				case ACTION_KNX_WRITE_RES:
				{
					KNX_GroupData *pGrpData = (KNX_GroupData *) pRecvNode->buf;

					ts_knx_control_info_t knx_control;
					knx_control.group_addr = pGrpData->nGroupAddr;

					// 0. get device_id by gateway_id & group_addr
					nRes = db_get_knx_group_type(pKnx->GetGatewayKey(), &knx_control);
					if(nRes != EXIT_SUCCESS)
					{
						printf("....IN KnxHandleListProcess(): db_get_knx_group_type() Failed....\n");
						return ERR_PROTROL_NO_DEVICE;
					}

					// 1. get knx current value by (home_dev_id && group_address)
					ts_knx_control_info_t knxData;
					memset(&knxData, 0, sizeof(ts_knx_control_info_t));
                	knxData.home_device_id = knx_control.home_device_id;
                	knxData.group_addr = pGrpData->nGroupAddr;
                	knxData.data_type = pGrpData->nGrpDPT;
                	nRes = db_get_knx_device_curr_value(&knxData);

					// 2. write knx new value to db
					pGetwayMan->WriteKnxValueToDB(pKnx->GetGatewayKey(), pGrpData);

					if(nRes == EXIT_SUCCESS)
					{
						// 3. callback data
						if(pKnx->GetWriteRes() != 0)
						{
								pKnx->GetWriteRes()(knx_control.home_device_id, *pGrpData);
						}

						// 4. Trigger a scene
						SceneInputNode inputNode;
						memset((char *) &inputNode, 0, sizeof(SceneInputNode));
						inputNode.scene_in_info.dev_id = knx_control.home_device_id;
						inputNode.scene_in_info.param = knx_control.group_addr;
						
						if(knx_control.data_type == DPT_Boolean
    						|| knx_control.data_type == DPT_1BitCtrl
    						|| knx_control.data_type == DPT_3BitCtrl
                            || knx_control.data_type == DPT_CharSet
                            || knx_control.data_type == DPT_8BitUnSign
                            || knx_control.data_type == DPT_8BitSign
                            || knx_control.data_type == DPT_2OctUnSign
                            || knx_control.data_type == DPT_2OctSign
                            || knx_control.data_type == DPT_2OctFloat
                            || knx_control.data_type == DPT_4OctUnSign
                            || knx_control.data_type == DPT_4OctSign
                            || knx_control.data_type == DPT_4OctFloat
                            || knx_control.data_type == DPT_Access
							)
						{
							float fData = 0;
							float fOrigVal = atof((char *) knxData.data);
							if(knx_control.data_type == DPT_Boolean)
							{
								fData = pGrpData->Data.BinData ? 1 : 0;
							}
							else
							{
								fData = pGrpData->Data.AnaData;
							}

							if(	knxData.is_first_use == TS_KNX_VALUE_FIRST_USE
								|| (knxData.is_first_use == TS_KNX_VALUE_NOT_FIRST_USE
									&& fData != fOrigVal))
							{
								sprintf(inputNode.scene_in_info.value, "%g", fData);

								inputNode.main_type = KNX;
								inputNode.sub_type = knx_control.data_type;
								strcpy((char *) inputNode.original_value, (char *) knxData.data);
							
								pGetwayMan->m_sceneMan.AddSceneInputNode(inputNode);
							}
						}
					}
					break;
				}
				case ACTION_KNX_EXTRA_RES:
					if(pKnx->GetGatewayManager() != 0)
					{
						if(pKnx->GetGatewayManager()->GetCtrlResponse() != 0)
						{
							pKnx->GetGatewayManager()->GetCtrlResponse()(
									ACTION_KNX_EXTRA_RES,
									pRecvNode->buf,
									pRecvNode->nLen);
						}

					}
					break;
				}

				delete pRecvNode;
				pRecvNode = 0;
				LOCK_MUTEX(pKnx->GetHandleListMutex());
				pRecvNode = (RecvNode *) pKnx->GetHandleList()->GetAndSeperateHeader();
				UNLOCK_MUTEX(pKnx->GetHandleListMutex());
			}
		}
		
		LOCK_MUTEX(pKnx->GetHandleListMutex());
		pKnx->GetHandleList()->DeleteAll();
		UNLOCK_MUTEX(pKnx->GetHandleListMutex());
		
#ifdef _WIN32
		OutputDebugString(_T("--------Knx Handle Thread END--------\n"));
#else
		printf("--------Knx Handle Thread END--------\n");
#endif
		return 0;
	}
	
	int CKNXKernel::StartDetectThread()
	{
		m_hBreathThread = CREATE_THREAD(DetectBreath, this);
		if(m_hBreathThread == 0)
			throw(new CKNXBreathThreadException);
		return 0;
	}
	
	int CKNXKernel::EndDetectThread()
	{
		if(m_hBreathThread != 0)
		{
#ifdef _WIN32
			SET_SIGNAL(m_beatCond[1]);
			WAIT_THREAD_EXIT(m_hBreathThread);
			CLOSE_THREAD(m_hBreathThread);
#else			
			m_bExitBeat = true;
			LOCK_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
			SET_SIGNAL(m_timerCond[TIMER_CONNECTSTATE]);
//			pthread_cancel(m_hBreathThread);
			UNLOCK_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
			WAIT_THREAD_EXIT(m_hBreathThread);
#endif
			m_hBreathThread = 0;
		}
		
		return 0;
	}
	
	int CKNXKernel::StartSendThread()
	{
		m_bSendThread = true;
		m_hSendThread = CREATE_THREAD(KnxSendListProcess, this);
		if(m_hSendThread == 0)
		{
			throw(new CKNXSendInitException);
		}

#ifdef _WIN32
		SetThreadPriority((void *) m_hSendThread, THREAD_PRIORITY_ABOVE_NORMAL);
#endif

		return 0;
	}
	
	int CKNXKernel::EndSendThread(bool bDelAll)
	{
		m_bSendThread = false;

		if(m_hSendThread != 0)
		{
#ifdef _WIN32
			WAIT_THREAD_EXIT(m_hSendThread);
			CLOSE_THREAD(m_hSendThread);
#else
			WAIT_THREAD_EXIT(m_hSendThread);
#endif
			m_hSendThread = 0;
		}

		if(bDelAll)
		{
			ClearSendList();
		}

		return 0;
	}
	
	int CKNXKernel::StartHandleThread()
	{
		m_bHandleThread = false;
		m_hHandleThread = CREATE_THREAD(KnxHandleListProcess, this);
		if(m_hHandleThread == 0)
		{
			throw(new CKNXHandleInitException);
		}

		return 0;
	}

	int CKNXKernel::EndHandleThread(bool bDelAll)
	{
		m_bHandleThread = false;

		if(bDelAll)
		{
			ClearHandleList();
		}

		if(m_hHandleThread != 0)
		{
#ifdef _WIN32
			SET_SIGNAL(m_CondHandleList);
			WAIT_THREAD_EXIT(m_hHandleThread);
			CLOSE_THREAD(m_hHandleThread);
#else
			WAIT_THREAD_EXIT(m_hHandleThread);
#endif
			m_hHandleThread = 0;
		}
		return 0;
	}

	
	void CKNXKernel::ToDisconnect(bool bWaitResponse, bool bReCon, bool bStopDetectThread)
	{
		m_bAutoConnect = bReCon;
		
		if(m_udpCtrl.IsRunning() && m_udpCtrl.GetClientSocket() > 0)
		{
			// send a disconnect message without wait for the response.
			// disconnect current communication,
			// and call the callback function of OnDisconnect		
			BYTE disconnectBuf[32] = {'\0'};
			int nMsgLen = sizeof(disconnectBuf);
			GenerateDisConnectMsg(disconnectBuf, nMsgLen);
			int nRes = -1;
			int nTimes = 0;
			while(nTimes < 3)
			{
				nTimes++;
				LOCK_MUTEX(&m_timerMutex[TIMER_DISCONNECT]);
				nRes = m_udpCtrl.UDPSend(disconnectBuf, nMsgLen);
				
				if(nRes == nMsgLen && bWaitResponse == true)
				{
					// if send disconnection request message successfully
					// and should wait for disconnection response,
					// start a event_timeout timer wait for the response.
					// timeout: 1s
					int nRes = -1;
#ifdef _WIN32
					m_timeout[TIMER_DISCONNECT] = 1 * 1000;
					nRes = WaitForSingleObject(m_timerCond[TIMER_DISCONNECT], 
											   m_timeout[TIMER_DISCONNECT]);
#else
					m_timeout[TIMER_DISCONNECT].tv_sec = time(0) + 1;
					m_timeout[TIMER_DISCONNECT].tv_nsec = 0;
					nRes = pthread_cond_timedwait(&m_timerCond[TIMER_DISCONNECT], 
												  &m_timerMutex[TIMER_DISCONNECT],
												  &m_timeout[TIMER_DISCONNECT]);
#endif
					if(nRes == 0)
					{
						UNLOCK_MUTEX(&m_timerMutex[TIMER_DISCONNECT]);
						break;
					}
				}
				else if(nRes == nMsgLen)
				{
					UNLOCK_MUTEX(&m_timerMutex[TIMER_DISCONNECT]);
					break;
				}
				UNLOCK_MUTEX(&m_timerMutex[TIMER_DISCONNECT]);
				// whether event ok (nRes == 0) or not
			}
			
			if(bStopDetectThread)
			{
				EndDetectThread();
			}
		}
		
		Disconnection(bReCon, bStopDetectThread);
	}
	
	void CKNXKernel::Disconnection(bool bReconnect, bool bStopDetectThread)
	{
		EndHandleThread(!bReconnect);
		EndSendThread(!bReconnect);

		if(bStopDetectThread)
		{
			EndDetectThread();
		}
		
		m_udpCtrl.UDPClose();
		m_udpData.UDPClose();
		
		int nRes = -1;
		if(bReconnect)
		{
			for(int iTimes = 0; iTimes < 1; iTimes++)
			{
				try
				{
					nRes = UDPSocketInit(m_udpCtrl.GetServerIp(), m_udpCtrl.GetServerPort());
					bReconnect = true;
				}
				catch(CKNXException *e)
				{
					bReconnect = false;
					delete e;
					m_notify();
					continue;
				}
				
				if(nRes == 0)
					break;
			}
		}
		
		if( !bReconnect || (bReconnect && nRes != 0))
		{
			if(m_pDisconnect != 0)
			{
				m_pDisconnect(m_nGatewayKey);
			}
			Reset();
		}
	}
	
	void CKNXKernel::Reset()
	{
		m_bWaitObject[TIMER_MULTI] = false;
		m_bWaitObject[TIMER_CONNECT] = false;
		m_bWaitObject[TIMER_CONNECTSTATE] = false;
		m_bWaitObject[TIMER_TUNNEL] = false;
		m_bConStateOn = false;
		m_bExitBeat = false;
		m_bHandleThread = false;

		// the status of response
		m_nStatus[TIMER_MULTI] = E_NO_ERROR;
		m_nStatus[TIMER_CONNECT] = E_NO_ERROR;
		m_nStatus[TIMER_CONNECTSTATE] = E_NO_ERROR;
		m_nStatus[TIMER_TUNNEL] = E_NO_ERROR;
		
		m_nChannelId = 0;
		m_nConStateMsgLen = 0;
		m_hBreathThread = 0;
		m_nStateTimeoutCount = 0;
		
		m_nInCounter = 0;
		m_nOutCounter = 0;
		
		m_nCurAction = ACTION_NULL;
		m_nCurGroup = 0;

		m_hSendThread = 0;
		m_hHandleThread = 0;
		
		// can not do this, because it will be 0 when reconnect call reset
		//m_nReconnCount = 0;
		
		memset(m_strSearchServerIp, 0, sizeof(m_strSearchServerIp));
		m_nSearchServerPort = 0;
		
#ifdef _WIN32
		m_timerMutex[TIMER_MULTI].LockCount = -1;
		m_timerMutex[TIMER_CONNECT].LockCount = -1;
		m_timerMutex[TIMER_CONNECTSTATE].LockCount = -1;
		m_timerMutex[TIMER_DISCONNECT].LockCount = -1;
		m_timerMutex[TIMER_TUNNEL].LockCount = -1;
		
		m_SendListMutex.LockCount = -1;
		m_TunnelRequest.LockCount = -1;
		m_UIRequest.LockCount = -1;
		m_HandleListMutex.LockCount = -1;
		
#else
		UNLOCK_MUTEX(&m_timerMutex[TIMER_MULTI]);
		UNLOCK_MUTEX(&m_timerMutex[TIMER_CONNECT]);
		UNLOCK_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
		UNLOCK_MUTEX(&m_timerMutex[TIMER_DISCONNECT]);
		UNLOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
		
		UNLOCK_MUTEX(&m_SendListMutex);
		UNLOCK_MUTEX(&m_TunnelRequest);
		UNLOCK_MUTEX(&m_UIRequest);
		UNLOCK_MUTEX(&m_HandleListMutex);
#endif
	}
	
	CKNXKernel::CKNXKernel(int nKey, char *strLocalIp, char * strServerIp, int nPort, CGatewayManager *pGatewayMan)
	{
		m_nIfaceType = GATEWAY_TYPE_KNX;
		m_pGatewayMan = pGatewayMan;

		nSockStartId = 0;
		Reset();

		m_pGetKnxDpt = 0;
		m_pSearchResponse = 0;
		m_pReadResponse = 0;
		m_pWriteResponse = 0;
		m_pConnect = 0;
		m_pDisconnect = 0;
		m_pErrorFun = 0;
		
		m_bSendThread = true;
		m_bAutoConnect = false;
		m_bConnectLock = false;

		m_nGatewayKey = nKey;
		SetServerAddr(strServerIp, nPort);
		SetLocalIp(strLocalIp);
		
		INITIALIZE_MUTEX(&m_timerMutex[TIMER_MULTI]);
		INITIALIZE_MUTEX(&m_timerMutex[TIMER_CONNECT]);
		INITIALIZE_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
		INITIALIZE_MUTEX(&m_timerMutex[TIMER_DISCONNECT]);
		INITIALIZE_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
		
		INITIALIZE_MUTEX(&m_UIRequest);
		INITIALIZE_MUTEX(&m_TunnelRequest);
		INITIALIZE_MUTEX(&m_SendListMutex);
		INITIALIZE_MUTEX(&m_HandleListMutex);
		
#ifdef _WIN32
		m_timerCond[TIMER_MULTI] = COND_INIT();
		m_timerCond[TIMER_CONNECT] = COND_INIT();
		m_timerCond[TIMER_CONNECTSTATE] = COND_INIT();
		m_timerCond[TIMER_DISCONNECT] = COND_INIT();
		m_timerCond[TIMER_TUNNEL] = COND_INIT();

		m_beatCond[0] = COND_INIT();
		m_beatCond[1] = COND_INIT();
		m_CondHandleList = COND_INIT();
#else
		COND_INIT(&m_timerCond[TIMER_MULTI]);
		COND_INIT(&m_timerCond[TIMER_CONNECT]);
		COND_INIT(&m_timerCond[TIMER_CONNECTSTATE]);
		COND_INIT(&m_timerCond[TIMER_DISCONNECT]);
		COND_INIT(&m_timerCond[TIMER_TUNNEL]);
		
		COND_INIT(&m_beatCond[0]);
		COND_INIT(&m_beatCond[1]);
		
		COND_INIT(&m_CondHandleList);
#endif

		InitWin32Sock();
	}
	
	CKNXKernel::~CKNXKernel()
	{
		EndHandleThread(false);
		EndDetectThread();
		m_udpMulti.UDPClose();
		m_udpCtrl.UDPClose();
		m_udpData.UDPClose();
		
		UnInitWin32Sock();

		DESTROY_MUTEX(&m_timerMutex[TIMER_MULTI]);
		DESTROY_MUTEX(&m_timerMutex[TIMER_CONNECT]);
		DESTROY_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
		DESTROY_MUTEX(&m_timerMutex[TIMER_DISCONNECT]);
		DESTROY_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
		
		DESTROY_MUTEX(&m_UIRequest);
		DESTROY_MUTEX(&m_TunnelRequest);
		DESTROY_MUTEX(&m_SendListMutex);
		DESTROY_MUTEX(&m_HandleListMutex);

		COND_DESTROY(m_timerCond[TIMER_MULTI]);
		COND_DESTROY(m_timerCond[TIMER_CONNECT]);
		COND_DESTROY(m_timerCond[TIMER_CONNECTSTATE]);
		COND_DESTROY(m_timerCond[TIMER_DISCONNECT]);
		COND_DESTROY(m_timerCond[TIMER_TUNNEL]);
		
		COND_DESTROY(m_beatCond[0]);
		COND_DESTROY(m_beatCond[1]);
		
		COND_DESTROY(m_CondHandleList);
	}

	void CKNXKernel::InitWin32Sock()
	{
#ifdef _WIN32
		WORD wVersion;
		WSADATA wsaData;
		wVersion = MAKEWORD(2, 0);
		
		int nRes = WSAStartup(wVersion, &wsaData);
		if(nRes != 0)
			return;
		
		if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
			UnInitWin32Sock();
#endif
	}
	
	void CKNXKernel::UnInitWin32Sock()
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}
	
	void CKNXKernel::SetServerAddr(char * serverIp, int serverPort)
	{
		strncpy(m_strServerIp, serverIp, MAX_IP_LEN);
		m_nServerPort = serverPort;
	}

	void CKNXKernel::SetLocalIp(char * localIp)
	{
		if(localIp == 0 || strcmp(localIp, "") == 0)
		{
			_GetLocalIp(m_strLocalIp);
		}
		else
		{
			strncpy(m_strLocalIp, localIp, MAX_IP_LEN);
		}
	}

	void CKNXKernel::GetLocalIp(char *ip)
	{
		strncpy(ip, m_strLocalIp, MAX_IP_LEN);
	}

	void CKNXKernel::GetSearchIp(char *pBuf)
	{
		strncpy(pBuf, m_strSearchServerIp, MAX_IP_LEN);
	}
	
	int CKNXKernel::GetSearchPort()
	{
		return m_nSearchServerPort;
	}
	
	void CKNXKernel::SetCallBacks(
			  	  	  	  	  	  GetKnxDpt pGetKnxDpt,
								  OnSearchResponse pSearchResponse, 
								  OnReadResponse pReadResponse,
								  OnWriteResponse pWriteResponse,
								  OnKnxConnect pConnect,
								  OnKnxDisconnect pDisconnect,
								  OnRWError pErrorFun)
	{
		m_pGetKnxDpt = pGetKnxDpt,
		m_pSearchResponse = pSearchResponse;
		m_pReadResponse = pReadResponse;
		m_pWriteResponse = pWriteResponse;
		m_pConnect = pConnect;
		m_pDisconnect = pDisconnect;
		m_pErrorFun = pErrorFun;
	}

	void CKNXKernel::SetSearchResponse(OnSearchResponse pSearchResponse)
	{
		m_pSearchResponse = pSearchResponse;
	}
	
	int CKNXKernel::GenerateSearchMsg(BYTE *pBuf, int &nLen)
	{
		int nWholeLen = sizeof(SearchReq);
		
		if(nLen < nWholeLen)
			return -1;
		
		SearchReq *pSearch = (SearchReq *) pBuf;
		FillHeader(pSearch->head, SERVICE_SEARCH_REQ, nWholeLen);
		
		// inet_ntoa(m_udpMulti.GetClientAddr().sin_addr)
		char strLocalIp[MAX_IP_LEN] = {'\0'};
		GetLocalIp(strLocalIp);
		FillHPAI(pSearch->hpaiCtrlL, strLocalIp, 
				 m_udpMulti.GetClientPort());
		
		nLen = nWholeLen;
		
		return 0;
	}
	
	
	int CKNXKernel::GenerateConnectMsg(BYTE *pBuf, int &nLen)
	{
		int nWholeLen = sizeof(ConnectReq);
		if(nLen < nWholeLen)
			return -1;
		
		ConnectReq *pConnect = (ConnectReq *) pBuf;
		FillHeader(pConnect->head, SERVICE_CONNECT_REQ, nWholeLen);
		
		char strLocalIp[MAX_IP_LEN] = {'\0'};
		GetLocalIp(strLocalIp);
		
		FillHPAI(pConnect->hpaiCtrlL, strLocalIp,
				 m_udpCtrl.GetClientPort());
		
		FillHPAI(pConnect->hpaiDataL, strLocalIp, 
				 m_udpData.GetClientPort());
		
		FillCRI(pConnect->cri, CONNECT_TYPE_TUNNEL, 0x02);
		
		nLen = nWholeLen;
		
		return 0;
	}
	
	
	int CKNXKernel::GenerateConnectStateMsg(BYTE *pBuf, int &nLen)
	{
		int nWholeLen = sizeof(ConnectStateReq);
		if(nLen < nWholeLen)
			return -1;
		
		ConnectStateReq *pConnectState = (ConnectStateReq *) pBuf;
		FillHeader(pConnectState->head, SERVICE_CONNECTSTATE_REQ, nWholeLen);
		
		pConnectState->channelId = m_nChannelId;
		pConnectState->reserved = 0;
		
		FillHPAI(pConnectState->hpaiCtrlL, inet_ntoa(m_udpCtrl.GetClientAddr().sin_addr), 
				 m_udpData.GetClientPort());
		
		nLen = nWholeLen;
		
		return 0;
	}
	
	int CKNXKernel::GenerateDisConnectMsg(BYTE *pBuf, int &nLen)
	{
		int nWholeLen = sizeof(ConnectStateReq);
		if(nLen < nWholeLen)
			return -1;
		
		ConnectStateReq *pConnectState = (ConnectStateReq *) pBuf;
		
		FillHeader(pConnectState->head, SERVICE_DISCONNECT_REQ, nWholeLen);
		
		pConnectState->channelId = m_nChannelId;
		pConnectState->reserved = 0;
		
		FillHPAI(pConnectState->hpaiCtrlL, inet_ntoa(m_udpCtrl.GetClientAddr().sin_addr), 
				 m_udpData.GetClientPort());
		
		nLen = nWholeLen;
		
		return 0;
	}
	
	int CKNXKernel::GenerateReadValueMsg(BYTE *pBuf, int nGroupAddr, int &nLen)
	{
		// length of cemi extended frame, didn't include the odd parity octet
		// but including the APCI octet
		WORD nDataLen = 1;
		WORD nCemiLen = 8 + nDataLen;
		
		// TunnelReq(head + conHead) + 2 octet additional information + length of cemi
		WORD nWholeLen = sizeof(TunnelReq) + 2 + nCemiLen;
		
		TunnelReq *pTunnelReq = (TunnelReq *) pBuf;
		FillHeader(pTunnelReq->head, TUNNELLING_REQUEST, nWholeLen);
		
		FillConHead(pTunnelReq->conHead, m_nChannelId, m_nOutCounter);
		
		BYTE *pCemi = (BYTE *) (pTunnelReq + 1);
		
		// m_code
		*pCemi++ = L_DATA_REQ;
		
		// additinal information length
		*pCemi++ = 0x00;
		
		// ctrl field 1
		*pCemi++ = CF_76_STANDARD_FRAME | CF_5_REPEAT | 
		CF_4_SYS_BROADCAST | CF_32_PRIOR_LOW | CF_1_NOACK | CF_0_NOERROR;
		
		// ctrl field 2
		*pCemi++ = CF2_7_ADDRESS_GROUP | CF2_654_HOP_COUNT_6 | CF2_3210_EFF_0;
		
		// source address
		*( (WORD *) pCemi) = htons(0);
		pCemi += 2;
		
		// destination address
		*((WORD *) pCemi) = htons(nGroupAddr);
		pCemi += 2;
		
		// length of data, from the APCI octet to the octet before odd parity
		*pCemi++ = (BYTE)nDataLen;
		
		// TPCI
		*pCemi++ = TCPI_T_DATA_GROUP;
		
		// APCI
		if(nDataLen > 0)
		{
			*pCemi++ = APCI_GROUP_VALUE_READ;
		}
		
		//wholeLen (+ 1) octet odd parity
		nLen = nWholeLen;
		
		return 0;
	}
	
	int CKNXKernel::GenerateWriteValueMsg(BYTE *pBuf, KNX_GroupData &grpData, int &nLen, WORD &wDesAddr)
	{
		BYTE dataBuf[TXT_DATA_SIZE] = {'\0'};
		int nDataLen = sizeof(dataBuf);
		int nRes = m_dptManager.DPT2Byte(grpData, dataBuf, nDataLen);
		if(nRes < 0)
			throw(new CKNXDPTConvertException);
		
		// if data < 0100 0000b
		// fill it into the apci/data octet.
		/*if(nDataLen == 1 && *dataBuf < (1<<6))
		{
			nDataLen = 0;
		}*/
		
		//length of cemi extended frame, didn't include the odd parity octet.
		// 8 octets before APCI + 1 octet APCI + length of databody
		WORD nCemiLen = 8 + 1 + nDataLen;
		
		// TunnelReq(head + conHead) + 2 octet additional information + length of cemi
		WORD nWholeLen = sizeof(TunnelReq) + 2 + nCemiLen;
		
		
		TunnelReq *pTunnelWrite = (TunnelReq *) pBuf;
		
		//KNX frame header
		FillHeader(pTunnelWrite->head, TUNNELLING_REQUEST, nWholeLen);
		
		// connect header
		FillConHead(pTunnelWrite->conHead, m_nChannelId, m_nOutCounter);
		
		BYTE *pCemi = (BYTE *) (pTunnelWrite + 1);
		
		//KNX frame additional information
		*pCemi++ = 0x11;
		*pCemi++ = 0x00;
		
		// ctrl field 1
		*pCemi++ = CF_76_STANDARD_FRAME | CF_5_REPEAT | 
		CF_4_SYS_BROADCAST | CF_32_PRIOR_LOW | CF_1_NOACK | CF_0_NOERROR;
		
		// ctrl field 2
		*pCemi++ = CF2_7_ADDRESS_GROUP | CF2_654_HOP_COUNT_6 | CF2_3210_EFF_0;
		
		//source address
		*((WORD *) pCemi) = htons(0);
		pCemi += 2;
		
		//destination address
		wDesAddr = grpData.nGroupAddr;
		*((WORD *) pCemi) = htons(grpData.nGroupAddr);
		pCemi += 2;
		
		//length of data, from the APCI octet to the octet before odd parity
		// 1 octet APCI + nDataLen
		*pCemi++ = 1 + nDataLen;
		
		//TPCI
		*pCemi++ = TCPI_T_DATA_GROUP;
		
		//APCI
		*pCemi = APCI_GROUP_VALUE_WRITE;
		
		//data, 0
		if(nDataLen == 0)
		{
			// fill the data into the APCI/data octet.
			*pCemi += (*dataBuf & 0x3F);
			pCemi++;
		}
		else
		{
			pCemi++;
			memcpy(pCemi, dataBuf, nDataLen);
		}
		
		//odd parity (NOT XOR)
		//*pBuf = GetOddParity(pBuf + KNX_VAL_HEADER_SIZE_10 + 2, nCemiLen);
		
		//wholeLen (+ 1) octet odd parity
		nLen = nWholeLen;
		
		return 0;
	}
	
	int CKNXKernel::GenerateTunnelAckMsg(BYTE *pBuf, int &nLen, 
										 int nStatus, bool bWithThisCount, int nThisCount)
	{
		int nWholeLen = sizeof(TunnelAck);
		if(nLen < nWholeLen)
			return -1;
		
		TunnelAck *pTunnelAck = (TunnelAck *) pBuf;
		FillHeader(pTunnelAck->head, TUNNELLING_ACK, nWholeLen);
		
		int nCount = m_nInCounter;
		if(bWithThisCount)
		{
			nCount = nThisCount;
		}
		FillConHead(pTunnelAck->conHead, m_nChannelId, nCount, nStatus);
		
		nLen = nWholeLen;
		
		return 0;
	}
	
	int CKNXKernel::SendTunnelAck(int nStatus, bool bWithThisCount, int nThisCount)
	{
		BYTE ackBuf[32] = {'\0'};
		int nMsgLen = sizeof(ackBuf);

		GenerateTunnelAckMsg(ackBuf, nMsgLen, nStatus);
		
		int nRes = -1;
		nRes = m_udpData.UDPSend(ackBuf, nMsgLen);
		
		return nRes;
	}
	
	int CKNXKernel::SearchServer()
	{
		memset(m_strSearchServerIp, 0, sizeof(m_strSearchServerIp));
		m_nSearchServerPort = 0;
		
		if(m_udpMulti.IsRunning())
		{
			m_udpMulti.UDPClose();
		}

		int nRes = -1;
		if(!m_udpMulti.IsRunning())
		{
			m_udpMulti.SetKnxPtr(this);
			
			if(m_bWaitObject[TIMER_MULTI] == true)
			{
				throw(new CKNXBusyException());
			}
			
			char strGroupIp[MAX_IP_LEN] = GROUP_IP;
			int nGroupPort = GROUP_PORT;
			int nLocalPort = GROUP_PORT;
			
			int nMaxPort = (GROUP_PORT + 999) > 65535 ? 65535 : (GROUP_PORT + 999);
			
			// to find a legal local multicast port
			char strLocalIp[MAX_IP_LEN] = {'\0'};
			GetLocalIp(strLocalIp);
			for(nLocalPort = GROUP_PORT; nLocalPort < nMaxPort; nLocalPort++)
			{
				nRes = m_udpMulti.InitSocket(strLocalIp, nLocalPort);
				if(nRes == 0)
					break;
			}
			
			// if socket failed throw an exception
			if(nRes != 0)
			{
				throw(new CKNXMultiInitException);
			}
			
			// if InitServer() failed, it will throw an exception.
			// else, return nRes == 0.
			nRes = m_udpMulti.InitServer(strGroupIp, nGroupPort);
			
			// else multicast ok
			// start recveiving thread
			// if StartRecvThread() failed, it will throw an exception.
			// else, return nRes == 0.
			try
			{
				nRes = m_udpMulti.StartRecvThread();
			}
			catch(CKNXException *e)
			{
				throw(e);
			}
		}
		 
		//Send a search server message
		BYTE searchBuf[16] = {'\0'};
		int nMsgLen = sizeof(searchBuf);
		nRes = GenerateSearchMsg(searchBuf, nMsgLen);
		if(nRes != 0)
			throw(new CKNXInnerException);
		
		//send the search message
		LOCK_MUTEX(&m_timerMutex[TIMER_MULTI]);
		nRes = m_udpMulti.UDPSend(searchBuf, nMsgLen);
		
		if(nRes != nMsgLen)
		{
			UNLOCK_MUTEX(&m_timerMutex[TIMER_MULTI]);
			throw(new CKNXSendException);
		}
		
		// if the search message is sent successfully,
		// start a event_timer.
		// event: recveived the search response within timeout
		// timeout: 3s

#ifdef _WIN32
		m_timeout[TIMER_MULTI] = 3 * 1000;
		m_bWaitObject[TIMER_MULTI] = true;
		nRes = WaitForSingleObject(m_timerCond[TIMER_MULTI], 
										m_timeout[TIMER_MULTI]);
		m_bWaitObject[TIMER_MULTI] = false;
#else
		m_timeout[TIMER_MULTI].tv_sec = time(0) + 3;
		m_timeout[TIMER_MULTI].tv_nsec = 0;
		m_bWaitObject[TIMER_MULTI] = true;
		nRes = pthread_cond_timedwait(&m_timerCond[TIMER_MULTI], 
					&m_timerMutex[TIMER_MULTI], &m_timeout[TIMER_MULTI]);
		m_bWaitObject[TIMER_MULTI] = false;
#endif
		UNLOCK_MUTEX(&m_timerMutex[TIMER_MULTI]);
		
		// if we have recieved the response successfully
		if(nRes == 0)
		{
			if(m_pSearchResponse == 0)
			{
				m_udpMulti.UDPClose();
				throw(new CKNXCallbackException);
			}
		}
		else
		{
			// timeout
			m_udpMulti.UDPClose();
			throw(new CKNXTimeoutException);
		}
		
		return 0;
	}

	int CKNXKernel::UDPSocketInit()
	{
		int nRes = 0;
		try
		{
			nRes = UDPSocketInit(m_strServerIp, m_nServerPort);
			
			// here: set m_bAutoConnect true must after called ToDisconnect()
			m_bAutoConnect = true;
		}
		catch(CKNXException *e)
		{
			throw(e);
		}

		return nRes;
	}

	int CKNXKernel::UDPSocketInit(char * szServerIp, int nServerPort)
	{
		if(m_bConnectLock)
		{
			throw(new CKNXBusyException);
		}
		m_bConnectLock = true;

		IncrementReconnCount();

		if(m_udpCtrl.IsRunning())
			ToDisconnect(true, false, false);
		else
			Reset();

		m_udpCtrl.SetKnxPtr(this);
		strncpy(m_strServerIp, szServerIp, sizeof(m_strServerIp));
		m_nServerPort = nServerPort;
		
		// init ctrl and data endpoint socket
		bool bInitOk = false;
		
		nSockStartId += SOCKET_SCAPE;
		while(nSockStartId < SOCKET_MIN || nSockStartId > SOCKET_MAX)
		{
			nSockStartId += SOCKET_SCAPE;
		}
		
		int nSockId = 0;
		int nSockEndId = nSockStartId + SOCKET_SCAPE - 1;
		
		// to find two adjacent socket ports,
		// the smaller for ctrl endpoint
		// the lagger for dzata endpoint
		char strLocalIp[MAX_IP_LEN] = {'\0'};
		GetLocalIp(strLocalIp);

		for(nSockId = nSockStartId; nSockId < nSockEndId; nSockId++)
		{
			bInitOk = m_udpCtrl.InitSocket(strLocalIp, nSockId) == 0;
			
			if(bInitOk)
			{
				bInitOk = m_udpData.InitSocket(strLocalIp, nSockId + 1) == 0;
				if(bInitOk == true)
				{
					break;
				}
				else
				{
					m_udpCtrl.UDPClose();
					nSockId++;
				}
			}			
		}
		
		if(!bInitOk)
		{
			m_bConnectLock = false;
			throw (new CKNXUniInitException);
		}
		
		// init server
		// if InitServer() failed, it will throw an exception.
		// else, return nRes == 0.
		int nRes = -1;
		try
		{
			nRes = m_udpCtrl.InitServer(m_strServerIp, m_nServerPort);
		}
		catch (CKNXException *e)
		{
			m_bConnectLock = false;
			e->GetExceptionId();
			m_udpCtrl.UDPClose();
			m_udpData.UDPClose();
			throw(e);
		}
		
		// start response handle thread
		try
		{
			nRes = StartHandleThread();
		}
		catch (CKNXException *e)
		{
			m_bConnectLock = false;
			e->GetExceptionId();
			m_udpCtrl.UDPClose();
			m_udpData.UDPClose();
			throw(e);
		}

		// start the receiving thread
		// if StartRecvThread() failed, it will throw an exception.
		// else, return nRes == 0.
		m_udpCtrl.m_bWaitRecvStart = false;	// added by keyy 2011/12/13
		try
		{
			nRes = m_udpCtrl.StartRecvThread();
		}
		catch (CKNXException *e)
		{
			m_bConnectLock = false;
			e->GetExceptionId();
			EndHandleThread(false);
			m_udpCtrl.UDPClose();
			m_udpData.UDPClose();
			throw(e);
		}
		
		// added by keyy 2011/12/13
		int nWaitTimes = 4;
		while (!m_udpCtrl.m_bWaitRecvStart && nWaitTimes > 0)
		{
			nWaitTimes--;
			
			WaitNull(500);
		}
		
		if(!m_udpCtrl.m_bWaitRecvStart)
		{
			m_bConnectLock = false;
			EndHandleThread(false);
			m_udpCtrl.UDPClose();
			m_udpData.UDPClose();
			throw(new CKNXBusyException);
		}
		
		BYTE connectBuf[32] = {'\0'};
		int nMsgLen = 32;
		nRes = GenerateConnectMsg(connectBuf, nMsgLen);
		if(nRes != 0)
		{
			m_bConnectLock = false;
			EndHandleThread(false);
			m_udpCtrl.UDPClose();
			m_udpCtrl.UDPClose();
			throw(new CKNXInnerException);
		}
		
		LOCK_MUTEX(&m_timerMutex[TIMER_CONNECT]);
		// send the tunnel connecting request message
		nRes = m_udpCtrl.UDPSend(connectBuf, nMsgLen);
		
		if(nRes != nMsgLen)
		{
			m_bConnectLock = false;
			UNLOCK_MUTEX(&m_timerMutex[TIMER_CONNECT]);
			EndHandleThread(false);
			m_udpCtrl.UDPClose();
			m_udpCtrl.UDPClose();
			throw(new CKNXSendException);
		}
		
//		printf("----------------from knx kernel thread send len = %d----------------\n", nRes);
		
		// if the connection message was sent successfully,
		// start a event_timer.
		// event: recveived the connect response within timeout
		// timeout: 10s
#ifdef _WIN32
		m_timeout[TIMER_CONNECT] = 3 * 1000;
		
		m_bWaitObject[TIMER_CONNECT] = true;
		nRes = WaitForSingleObject(m_timerCond[TIMER_CONNECT], 
			m_timeout[TIMER_CONNECT]);
#else
		m_timeout[TIMER_CONNECT].tv_sec = time(0) + 3;
		m_timeout[TIMER_CONNECT].tv_nsec = 0;
		
		m_bWaitObject[TIMER_CONNECT] = true;
		nRes = pthread_cond_timedwait(&m_timerCond[TIMER_CONNECT], 
									  &m_timerMutex[TIMER_CONNECT],
									  &m_timeout[TIMER_CONNECT]);
#endif
		m_bWaitObject[TIMER_CONNECT] = false;
		
		UNLOCK_MUTEX(&m_timerMutex[TIMER_CONNECT]);
		
//		printf("after timewait mutex: %x, nRes=%d, m_nStatus=%d\n", 
//			   (int)&m_timerMutex[TIMER_CONNECT], nRes, m_nStatus[TIMER_CONNECT]);
		
#ifdef _WIN32
		char str[32] = {'\0'};
		sprintf(str, ("-----nRes = %d---\n"), nRes);
		OutputDebugStringA(str);
#endif
		
		// we have recieved the response successfully
		if(nRes == 0 && m_nStatus[TIMER_CONNECT] == E_NO_ERROR)
		{
			// 1. start the data endpoint server and receiving thread
			//    with the received ip and port of the data endpoint.
			try
			{
				m_udpData.InitServer(m_strDataServerIp, m_nDataServerPort);
			}
			catch(CKNXException *e)
			{
				m_bConnectLock = false;
				e->GetExceptionId();

				EndHandleThread(false);
				m_udpCtrl.UDPClose();
				m_udpCtrl.UDPClose();
				throw(e);
			}
			
			m_udpData.SetKnxPtr(this);

			try
			{
				m_udpData.StartRecvThread();
				StartSendThread();
			}
			catch(CKNXException *e)
			{
				m_bConnectLock = false;
				e->GetExceptionId();

				EndHandleThread(false);
				EndSendThread(false);
				m_udpCtrl.UDPClose();
				m_udpData.UDPClose();
				throw(e);
			}

			// 2. start the breadthing detecting thread.
			// if StartDetectThread() failed, it will throw an exception,
			// else, return value is 0.
			try
			{
				StartDetectThread();
			}
			catch(CKNXException *e)
			{
				m_bConnectLock = false;
				e->GetExceptionId();

				EndHandleThread(false);
				EndSendThread(false);
				m_udpCtrl.UDPClose();
				m_udpData.UDPClose();
				throw(e);
			}
		}
		else
		{
			// timeout or some other error
			m_bConnectLock = false;
			EndHandleThread(false);
			EndSendThread(false);
			m_udpCtrl.UDPClose();
			m_udpData.UDPClose();
			
			int nTimeOut = 0;
#ifdef _WIN32
			nTimeOut = WAIT_TIMEOUT;
#else
			nTimeOut = ETIMEDOUT;
#endif			
			if(nRes == 0 && m_nStatus[TIMER_CONNECT] != E_NO_ERROR)
			{
				m_nStatus[TIMER_CONNECT] = E_NO_ERROR;
				throw(new CKNXGatewayStatusException);
			}
			else if(nRes == nTimeOut)
			{
				m_nStatus[TIMER_CONNECT] = E_NO_ERROR;
				throw(new CKNXTimeoutException);
			}
			else
			{
				m_nStatus[TIMER_CONNECT] = E_NO_ERROR;
				throw(new CKNXOtherException);
			}
		}

		m_bConnectLock = false;
		m_nReconnCount = 0;

		return 0;
	}
	
	void CKNXKernel::IncrementReconnCount()
	{
		m_nReconnCount++;
		if((m_pGatewayMan == 0 && m_nReconnCount == KNX_RECONNECT_INTERVAL_NOTIFY)
			|| (m_pGatewayMan != 0 && m_nReconnCount == m_pGatewayMan->GetKnxClearCount()))
		{
			ClearSendList();
			ClearHandleList();
		}
	}

	void CKNXKernel::ClearSendList()
	{
		LOCK_MUTEX(&m_SendListMutex);
		printf("--------Delete KNX Send List---------\n");
		m_sendList.DeleteAll();
		UNLOCK_MUTEX(&m_SendListMutex);
	}

	// NOT XOR
	BYTE CKNXKernel::GetOddParity(BYTE *pParityBuf, int nParityLen)
	{
		BYTE bRes = 0x00;
		for(int i = 0; i < nParityLen; i++)
		{
			bRes ^= *(pParityBuf + i);
		}
		
		return ~bRes;
	}
	
	int CKNXKernel::DoReadGrpVal (int nGroupAddr)
	{
		if(!m_udpData.IsRunning())
		{
			throw(new CKNXDisConnectedException);
		}
		/*
		if(m_nCurAction != ACTION_NULL)
		{
			throw(new CKNXBusyException);
		}*/
		
		BYTE readBuf[100] = {'\0'};
		int nMsgLen = sizeof(readBuf);
		int nRes = -1;
		nRes = GenerateReadValueMsg(readBuf, nGroupAddr, nMsgLen);
		if(nRes != 0)
			throw(new CKNXInnerException);
		
		// if GroupValueRequest() failed, it will throw an exception,
		// else, return send length.
		nRes = GroupValueRequest(readBuf, nMsgLen, nGroupAddr, READ);
		
		return nRes;
	}
	
	int CKNXKernel::DoWriteGrpVal (KNX_GroupData grpData)
	{
		if(!m_udpData.IsRunning())
		{
			throw(new CKNXDisConnectedException);
		}
		/*
		if(m_nCurAction != ACTION_NULL)
		{
			throw(new CKNXBusyException);
		}
		 */
		
		BYTE writeBuf[100] = {'\0'};
		int nMsgLen = sizeof(writeBuf);
		WORD wDesAddr = 0;
		int nRes = -1;
		nRes = GenerateWriteValueMsg(writeBuf, grpData, nMsgLen, wDesAddr);

		// if GroupValueRequest() failed, it will throw an exception,
		// else, return send length.
		nRes = GroupValueRequest(writeBuf, nMsgLen, wDesAddr, WRITE);
		
		return nRes;		
	}	
	
	int CKNXKernel::ReadGrpVal (int nGroupAddr)
	{
		LOCK_MUTEX(&m_UIRequest);
		if(m_hSendThread == 0 || !m_udpData.IsRunning())
		{
			UNLOCK_MUTEX(&m_UIRequest);
			throw(new CKNXDisConnectedException);
		}
		
		int nCount = 0;
		LOCK_MUTEX(&m_SendListMutex);
		nCount = m_sendList.GetCount();
		printf("m_sendList.GetCount() = %d\n", nCount);
		int nTimes = 0;
		while(m_sendList.GetCount() > 100 && nTimes < 20)
		{
			nTimes++;
			UNLOCK_MUTEX(&m_SendListMutex);
			
			WaitNull(50);
			
			LOCK_MUTEX(&m_SendListMutex);
		}
		char strTimes[100];
		sprintf(strTimes, "----------------------[nTimes: %d] [nCount: %d]\n", nTimes, nCount);
#ifdef _WIN32
		OutputDebugStringA(strTimes);
#else
	//	printf(strTimes);
#endif
		if(nTimes >= 20)
		{
			UNLOCK_MUTEX(&m_SendListMutex);	
			UNLOCK_MUTEX(&m_UIRequest);
			return -1;
		}

		SendNode *pNode = new SendNode();
		pNode->pNext = 0;
		KNX_GroupData grpData;
		grpData.nGroupAddr = nGroupAddr;
		memcpy(pNode->buf, (void *) &grpData, sizeof(grpData));
		pNode->nAction = READ;

		m_sendList.AddTail((NodeBase *)pNode);
		/*
		if(m_sendList.GetCount() < 100)
		{
			m_sendList.AddTail(pNode);
		}
		else
		{
			delete pNode;
			pNode = 0;
		}
		*/
		UNLOCK_MUTEX(&m_SendListMutex);
		
		UNLOCK_MUTEX(&m_UIRequest);
		return nCount;
	}
	
	int CKNXKernel::WriteGrpVal (void *pBuf, int nLen, int nPrefixLen)
	{
		LOCK_MUTEX(&m_UIRequest);
		if(m_bSendThread == false || m_hSendThread == 0 || !m_udpData.IsRunning())
		{
			UNLOCK_MUTEX(&m_UIRequest);
			throw(new CKNXDisConnectedException);
		}
		
		int nCount = 0;
		LOCK_MUTEX(&m_SendListMutex);
		nCount = m_sendList.GetCount();
		printf("m_sendList.GetCount() = %d\n", nCount);
		
		int nTimes = 0;
		while(m_sendList.GetCount() > 100 && nTimes < 20)
		{
			nTimes++;
			UNLOCK_MUTEX(&m_SendListMutex);
			
			WaitNull(50);
			
			LOCK_MUTEX(&m_SendListMutex);
		}

		if(nTimes >= 20)
		{
			UNLOCK_MUTEX(&m_SendListMutex);	
			UNLOCK_MUTEX(&m_UIRequest);
			throw(new CKNXTimeoutException);
		}

		SendNode *pNode = new SendNode();
		pNode->pNext = 0;
		memcpy(pNode->buf, pBuf, nLen);
		pNode->nAction = WRITE;
		pNode->nPrefixLen = nPrefixLen;

		m_sendList.AddTail((NodeBase *)pNode);

		UNLOCK_MUTEX(&m_SendListMutex);
		UNLOCK_MUTEX(&m_UIRequest);
		return nCount;
	}
	
	int CKNXKernel::GroupValueRequest(BYTE *pBuf, int nLen, WORD wAddr, int nCurAction)
	{
		/*
		BYTE *pp = pBuf;
		printf("[");
		for(int iTest = 0; iTest < nLen; iTest++)
		{
			printf("%.2x, ", *pp++);
		}
		printf("]\n");
		*/
				
		LOCK_MUTEX(&m_TunnelRequest);
		
		int nSendLen = -1;
		int nRes = -1;
		int nTimeoutCount = 0;
		
		while(nTimeoutCount < 2)
		{
			// if send ok, start a event_timeout timer
			// event: the response to (m_nCurGroup, m_nCurAction)
			// timeout: 1s
			// send times: 2
			
#ifdef _WIN32
			SYSTEMTIME now;
			GetSystemTime(&now);//= CTime::GetCurrentTime();
            printf("------wait signal lock --%.2d:%.2d:%.2d -----\n",
				   now.wHour, now.wMinute, now.wSecond);
#else
//			struct tm *ptm;
//			long ts = time(0);
//			ptm = localtime(&ts);
//			printf("------wait signal lock --%.2d:%.2d:%.2d value=%g------\n", 
//				   ptm->tm_hour, ptm->tm_min, ptm->tm_sec, dValue);
#endif			
			// exception should caugth by the upper layer
			
			printf("-----------Request Group (%d, %d, %d)----------\n", 
				   (wAddr >> 11) & 0x0F, (wAddr >> 8) & 0x07, wAddr & 0xFF);
			
			LOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
			
			nSendLen = m_udpData.UDPSend(pBuf, nLen);
			if(nSendLen <= 0)
			{
				UNLOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
				break;
			}
			else if(nSendLen > 0 && nSendLen != nLen)
			{
				UNLOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
				continue;
			} 
			
			m_nCurAction = nCurAction;
			m_nCurGroup = wAddr;
			
#ifdef _WIN32
			m_timeout[TIMER_TUNNEL] = 1 * 1000;
			m_bWaitObject[TIMER_TUNNEL] = true;
			nRes = WaitForSingleObject(m_timerCond[TIMER_TUNNEL],
									   m_timeout[TIMER_TUNNEL]);
			m_bWaitObject[TIMER_TUNNEL] = false;
#else
			m_timeout[TIMER_TUNNEL].tv_sec = time(0) + 1;
			m_timeout[TIMER_TUNNEL].tv_nsec = 0;
			m_bWaitObject[TIMER_TUNNEL] = true;
			nRes = pthread_cond_timedwait(&m_timerCond[TIMER_TUNNEL], 
										  &m_timerMutex[TIMER_TUNNEL], 
										  &m_timeout[TIMER_TUNNEL]);
			
			m_bWaitObject[TIMER_TUNNEL] = false;
#endif
			UNLOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
			
//			printf("-------wait signal unlock value=%g nRes=%d-------\n", dValue, nRes);
			
			if(nRes == 0)
			{
				// the value of m_nOutCounter after added 1 will be used
				// in the receiving thread, and will set the signal if success.
				m_nOutCounter++;
				break;
			}
			
			// if timeout, resend with the same counter as the front frame.
			nTimeoutCount++;
		}
		UNLOCK_MUTEX(&m_TunnelRequest);
		
		// if failed 2 times, disconnect the connection.
		// reference protocol file 03_08_04 page 8.
		if(nRes != 0 && nTimeoutCount == 2)
		{
			printf("---------Request timeout 2 times-----------\n");
			ToDisconnect(true, true);
			return TIME_OUT_2;
		}
		
		return nSendLen;
	}
	
	int CKNXKernel::AddHandleListNode(int nResType, void *pBuf, int nLen)
	{
		if(!m_bHandleThread)
		{
			return -1;
		}

		LOCK_MUTEX(&m_HandleListMutex);

		int nTimes = 0;
		while(m_handleList.GetCount() > 100 && nTimes < 20)
		{
			UNLOCK_MUTEX(&m_HandleListMutex);
			WaitNull(50);
			nTimes++;

			LOCK_MUTEX(&m_HandleListMutex);
		}

		if(nTimes >= 20)
		{
			UNLOCK_MUTEX(&m_HandleListMutex);
			return -1;
		}

		RecvNode *pNewRecvNode = new RecvNode();
		pNewRecvNode->pNext = 0;
		pNewRecvNode->nResType = nResType;
		pNewRecvNode->nLen = nLen;
		memset(pNewRecvNode->buf, 0, REQ_RES_BUF_SIZE);
		memcpy(pNewRecvNode->buf, pBuf, nLen);

		m_handleList.AddTail((NodeBase *)pNewRecvNode);

		UNLOCK_MUTEX(&m_HandleListMutex);

		if(IsHandleProcess())
		{
			SET_SIGNAL(m_CondHandleList);
		}
		return 0;
	}

	void CKNXKernel::ClearHandleList()
	{
		printf("--------Delete KNX Handle List---------\n");
		m_handleList.DeleteAll();
		UNLOCK_MUTEX(&m_HandleListMutex);
	}

	void CKNXKernel::WaitForAllThreadsExit() 
	{
#ifdef _WIN32
		void * cond[4] = { (void *) m_hSendThread, 
			(void *) m_hHandleThread, 
			(void *) m_hBreathThread, 
			(void *) m_udpCtrl.GetRecvThreadHandle() };
		WaitForMultipleObjects(4, cond, true, 5 * 1000);
		
		CString str;
		str.Format(_T("..........[%d, %d, %d, %d].............\n"),
			m_hSendThread ? 1 : 0,
			m_hHandleThread ? 1 : 0,
			m_hBreathThread ? 1 : 0,
			m_udpCtrl.IsRunning() ? 1 : 0);
		OutputDebugString(str);
#else
		while(m_hSendThread || m_hHandleThread || m_hBreathThread || m_udpCtrl.GetRecvThreadHandle())
		{
			sleep(1);
		}
#endif
	}

	int CKNXKernel::ExecRecvMsgAction(BYTE *pRecvBuf, int nLen)
	{
		BYTE *pHandle = pRecvBuf;
		int nHandledLen = 0;
		WORD nServiceId = 0;
		int nCurFrmTotalLen = 0;

		while (nLen - nHandledLen > KNX_VAL_HEADER_SIZE_10)
		{
			// if it isn't a frame header
			// skiping the bytes and anaylzing the buf start the following
			if(*pHandle != KNX_VAL_HEADER_SIZE_10)
			{
				nHandledLen++;
				pHandle++;
				continue;
			}
			
			if(*(pHandle + 1) != KNX_VAL_VERSION10)
			{
				nHandledLen += 2;
				pHandle += 2;
				continue;
			}
			
			nServiceId = ntohs( *((WORD *) (pHandle + 2)) );
			
			nCurFrmTotalLen = 0;		
			nCurFrmTotalLen = ntohs( *((WORD *) (pHandle+4)) );
			
	
			if(nLen - nHandledLen < nCurFrmTotalLen)
			{
				// if the whole length of the buffer is less than
				// the length of current legal frame,
				// skip the buffer and break the while cycle.
				nHandledLen = nLen;
				break;
			}
#ifdef _WIN32
			char str[64] = {'\0'};
			sprintf(str, "SrvId=0x%.4X, nLen=%d\n", nServiceId, nLen);
			OutputDebugStringA(str);
#else
			printf("SrvId=0x%.4X, nLen=%d\n", nServiceId, nLen);
#endif

			// knx frame
			switch (nServiceId)
			{
				case SERVICE_SEARCH_REQ:
					// skip
					break;
					
				case SERVICE_SEARCH_RES:
					// search response, wake the signal if it is waitting
					
					//if(m_bWaitObject[TIMER_MULTI] == true)
					{
#ifndef _WIN32
						LOCK_MUTEX(&m_timerMutex[TIMER_MULTI]);
#endif
						if(m_bWaitObject[TIMER_MULTI] == true)
						{
							SET_SIGNAL(m_timerCond[TIMER_MULTI]);
						}
#ifndef _WIN32
						UNLOCK_MUTEX(&m_timerMutex[TIMER_MULTI]);
#endif
						HandleSearchResponse(pHandle, nCurFrmTotalLen);

						// add to handle list
						//AddHandleListNode(ACTION_KNX_SEARCH_RES, (void *) pHandle, nCurFrmTotalLen);
					}
					break;
					
				case SERVICE_CONNECT_RES:
					// connection response, wake the signal if it is waitting
					//if(m_bWaitObject[TIMER_CONNECT] == true)
					{
						HandleConnectResponse(pHandle, nCurFrmTotalLen);
#ifndef _WIN32
						LOCK_MUTEX(&m_timerMutex[TIMER_CONNECT]);
						SET_SIGNAL(m_timerCond[TIMER_CONNECT]);
						UNLOCK_MUTEX(&m_timerMutex[TIMER_CONNECT]);
#else
						SET_SIGNAL(m_timerCond[TIMER_CONNECT]);
#endif
					}
					break;
					
				case SERVICE_CONNECTSTATE_RES:
					// connection state response, wake the signal if it is waitting
					//if(m_bWaitObject[TIMER_CONNECTSTATE] == true)
					{
						HandleConnectStateResponse(pHandle, nCurFrmTotalLen);

#ifdef _WIN32
						//if(m_bWaitObject[TIMER_CONNECTSTATE])
						{
							SET_SIGNAL(m_beatCond[0]);
						}
#else
						LOCK_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
						if(m_bWaitObject[TIMER_CONNECTSTATE])
						{
							m_bExitBeat = false;
							SET_SIGNAL(m_timerCond[TIMER_CONNECTSTATE]);
						}
						UNLOCK_MUTEX(&m_timerMutex[TIMER_CONNECTSTATE]);
#endif
					}
					break;
					
				case SERVICE_DISCONNECT_REQ:
					Disconnection(m_bAutoConnect);
					break;
					
				case SERVICE_DISCONNECT_RES:
#ifndef _WIN32
					LOCK_MUTEX(&m_timerMutex[TIMER_DISCONNECT]);
					m_bExitBeat = true;
					SET_SIGNAL(m_timerCond[TIMER_DISCONNECT]);
					UNLOCK_MUTEX(&m_timerMutex[TIMER_DISCONNECT]);
#else					
					SET_SIGNAL(m_beatCond[1]);	
					SET_SIGNAL(m_timerCond[TIMER_DISCONNECT]);
#endif
					HandleDisconnectResponse(pHandle, nCurFrmTotalLen);
					break;
					
				case TUNNELLING_REQUEST:
					/*if(m_bConStateOn)
					{
						SET_SIGNAL(m_timerCond[TIMER_CONNECTSTATE]);
					}*/
					HandleTunnelResponse(pHandle, nCurFrmTotalLen);
					break;
					
				case TUNNELLING_ACK:
					/*if(m_bConStateOn)
					{
						SET_SIGNAL(m_timerCond[TIMER_CONNECTSTATE]);
					}
					*/
					HandleTunnelAck(pHandle, nCurFrmTotalLen);
					break;

			}
			
			nHandledLen += nCurFrmTotalLen;
			pHandle += nCurFrmTotalLen;
		}
		
		return nHandledLen;
	}
	
	int CKNXKernel::HandleSearchResponse(BYTE *pHandle, int nLen)
	{
		SearchRes *pSearchRes = (SearchRes *) pHandle;
		
		// SearchRes:		head + hpaiCtrlR + dib_hd
		// buf of pHandle:	head + hpaiCtrlR + dib_hd + dib_ssf(variable length)
		
		UINT uIp = ntohl(pSearchRes->hpaiCtrlR.ip);
		sprintf(m_strSearchServerIp, "%d.%d.%d.%d", (uIp>>24) & 0xFF, 
				(uIp>>16) & 0xFF, (uIp>>8) & 0xFF, uIp & 0xFF);
		
		m_nSearchServerPort = ntohs(pSearchRes->hpaiCtrlR.port);

		if(m_pSearchResponse != 0)
		{
			m_pSearchResponse(m_nGatewayKey, m_strSearchServerIp, m_nSearchServerPort, E_NO_ERROR);
		}
		
		// the dib_ssf (variable length)
		BYTE *pDibssf = (BYTE *) pSearchRes++;
		int nHandledLen = sizeof(SearchRes);
		if(nLen > nHandledLen)
		{
			// the *pDibssf is the length of dib_ssf
			// the *(pDibssf + 1) is the length of dib_ssf
			pDibssf += 2;
			nHandledLen += 2;
		}
		
		int nIndex = 0;
		while(nLen > nHandledLen)
		{
			// the *(pDibssf + nIndex) is the service family id:
			// 02 for KNXnet/IP Core
			// 03 for KNXnet/Device Mgmt
			// 04 for KNXnet/IP Tunnelling
			// 05 for KNXnet/IP Routing
			// the *(pDibssf + nIndex + 1) is the service family version
			
			nIndex += 2;
			nHandledLen += 2;
		}
		
		return 0;
	}	
	
	int CKNXKernel::HandleConnectResponse(BYTE *pHandle, int nLen)
	{
		ConnectRes *pConnectRes = (ConnectRes *) pHandle;
		
		// ConnectRes: head + channelId + status + hpaiDataR + crd
		m_nChannelId = pConnectRes->channelId;
		
		m_nStatus[TIMER_CONNECT] = pConnectRes->status;
		if(pConnectRes->status != E_NO_ERROR)
			return 0;
		
		UINT uIp = ntohl(pConnectRes->hpaiDataR.ip);
		sprintf(m_strDataServerIp, "%d.%d.%d.%d", (uIp>>24) & 0xFF, 
				(uIp>>16) & 0xFF, (uIp>>8) & 0xFF, uIp & 0xFF);
		m_nDataServerPort = ntohs(pConnectRes->hpaiDataR.port);
		
		m_nIndivAddrss = ntohs(pConnectRes->crd.indivAddr);
		
		return 0;
	}
	
	int CKNXKernel::HandleConnectStateResponse(BYTE *pHandle, int nLen)
	{
		ConnectStateRes *pConnectStateRes = (ConnectStateRes *) pHandle;
		
		//ConnectStateRes: head + channelId + status
		m_nStatus[TIMER_CONNECTSTATE] = pConnectStateRes->status;
		
		return 0;
	}
	
	int CKNXKernel::HandleDisconnectResponse(BYTE *pHandle, int nLen)
	{
		// do nothing
		// Disconnection(false);
		return 0;
	}
	
	int CKNXKernel::HandleTunnelAck(BYTE *pHandle, int nLen)
	{
		TunnelAck *pTunnelAck = (TunnelAck *) pHandle;
//		int nOutCount = pTunnelAck->conHead.counter;
		int nError = pTunnelAck->conHead.status;
		
		if(nError != E_NO_ERROR)
		{
			return -1;
		}
		
#ifndef _WIN32
		LOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
#endif
		if(m_bWaitObject[TIMER_TUNNEL] == true /*&& 
		   (nOutCount == (BYTE) m_nOutCounter || nOutCount == (BYTE) m_nOutCounter - 1)*/ )
		{
			m_bWaitObject[TIMER_TUNNEL] = false;
			m_nCurAction = ACTION_NULL;
			m_nCurGroup = 0;
//			printf(" -- -- RECV ACK OK -- -- \n");
			SET_SIGNAL(m_timerCond[TIMER_TUNNEL]);
		}
		else
		{
//			printf(" -- -- RECV ACK Failed (%d, %d) -- -- \n", m_nCurAction, m_nCurGroup);
		}
#ifndef _WIN32
		UNLOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
#endif	
		
		return 0;
	}
	
	int CKNXKernel::HandleTunnelResponse(BYTE *pHandle, int nLen)
	{
		TunnelReq *pTunnelReq = (TunnelReq *) pHandle;
		int nInCounter = pTunnelReq->conHead.counter;
		BYTE *pBuf = (BYTE *) (pTunnelReq + 1);
		int m_code = *pBuf++;
		
		int addIL = *pBuf++;
		
		// skip the additional information
		pBuf += addIL;
		
		// skip (cf1,1 octet) + (cf2,1 octet) + (source address, 2ctets)
		pBuf += 4;
		
		// destination address
		WORD wDa = htons( *(( WORD *)pBuf) );
		pBuf += 2;
		KNX_GroupData grpData;
		grpData.nGroupAddr = 0;
		grpData.nGrpDPT = DATA_TYPE_ANALOG;
		grpData.Data.AnaData = 0;
		
		grpData.nGroupAddr = wDa;

		if(m_pGetKnxDpt != 0)
		{
			grpData.nGrpDPT = m_pGetKnxDpt(m_nGatewayKey, grpData.nGroupAddr);
		}
		
		// data len: calculate from APCI octet
		int nDataLen = (*pBuf & 0x0F);			// does it is like this	???
		pBuf++;
		
		int nTpci = *pBuf++;
		
		int nApci = 0;
		if(nDataLen > 0)
		{
			nApci = *pBuf++;
		}
		
		BYTE dataBuf[TXT_DATA_SIZE] = {'\0'};
		if(nDataLen == 1)
		{
			// get data from APCI octet
			dataBuf[0] = (nApci & 0x3F);
		}
		else if(nDataLen > 1)
		{
			// get data from the following (nDataLen -1) octets
			memcpy(dataBuf, pBuf, nDataLen - 1);
		}
		m_dptManager.Byte2DPT(dataBuf, TXT_DATA_SIZE, grpData);

		// if(nInCounter == m_nInCounter), then ack and accept the frame
		// if(nInCounter == m_nInCounter - 1), then ack and discard
		// else, not ack and diacard

		bool bNpciIs0 = false;
		bNpciIs0 = (nTpci & 0x03) == 0 ? true : false;
		int nAction = (nApci & 0xC0);
				
		m_nInCounter = (m_nInCounter & 0xFFFFFF00) + nInCounter;


//		printf("---Recv (%d, %d, %d) value = %g---\n",
//			   grpData.nMainAddr, grpData.nMidAddr, grpData.nGrpAddr,
//			   grpData.Data.AnaData);



		BYTE nInCnt = (BYTE) (m_nInCounter & 0xFF);
		if(nInCounter == nInCnt || nInCounter == nInCnt - 1)
		{
			// ack
			//printf("******bNpciIs0 = %d, nAction = %d (m_nInCounter = %d, nInCounter = %d)******\n",
			//	   bNpciIs0 ? 0 : 1, nAction, nInCnt, nInCounter);
			
			SendTunnelAck(E_NO_ERROR);
			// accept the message
			if(nInCounter == nInCnt)
			{
				if( (m_nInCounter & 0xFF) == 255)
				{
					m_nInCounter++;
				}
			}
			else
			{
				return 0;
			}

			// handle the response
			if(m_code == L_DATA_CON)
			{
				if( bNpciIs0 && (nAction == APCI_GROUP_VALUE_READ) )
				{
/*					// A_GROUP_VALUE_READ
#ifndef _WIN32
					LOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
#endif
					printf("------l_data_con lock----");
					if(m_bWaitObject[TIMER_TUNNEL] == true && m_nCurAction == ACTION_KNX_READ && m_nCurGroup == wDa)
					{
						m_bWaitObject[TIMER_TUNNEL] = false;
						m_nCurAction = ACTION_NULL;
						m_nCurGroup = 0;
						printf(" -- -- OK\n");
						SET_SIGNAL(m_timerCond[TIMER_TUNNEL]);
					}
					else
					{
						printf(" -- -- Failed (%d, %d), (%u, %u)\n", m_nCurAction, ACTION_KNX_READ, m_nCurGroup, wDa);
					}
					printf("---------------l_data_con unlock----\n");
#ifndef _WIN32
					UNLOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
#endif				
*/					// deleted by kyy, 2011/08/03
					/*if(m_pReadResponse != 0)
					{
						m_pReadResponse(grpData, E_NO_ERROR);
					}
					 */
					
//					printf("Read CON (%d)-----\n", grpData.nGroupAddr);
					
				}
				else if( bNpciIs0 && (nAction == APCI_GROUP_VALUE_RES) )
				{
					// A_GROUP_VALUE_RESPONSE	
					// do nothing except ack
				}
				else if( bNpciIs0 && (nAction == APCI_GROUP_VALUE_WRITE) )
				{
/*					// A_GROUP_VALUE_WRITE	
#ifndef _WIN32
					LOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
#endif
					printf("------l_data_con lock----");
					if(m_bWaitObject[TIMER_TUNNEL] == true && m_nCurAction == ACTION_KNX_WRITE && m_nCurGroup == wDa)
					{
						m_bWaitObject[TIMER_TUNNEL] = false;
						m_nCurAction = ACTION_NULL;
						m_nCurGroup = 0;
						printf(" -- -- OK\n");
						SET_SIGNAL(m_timerCond[TIMER_TUNNEL]);
					}
					else
					{
						printf(" -- -- Failed (%d, %d), (%u, %u)\n", m_nCurAction, ACTION_KNX_WRITE, m_nCurGroup, wDa);
					}
					printf("---------------l_data_con unlock----\n");	
#ifndef _WIN32
					UNLOCK_MUTEX(&m_timerMutex[TIMER_TUNNEL]);
#endif
*/

					// add to handle list
					AddHandleListNode(ACTION_KNX_WRITE_RES, (void *) &grpData, sizeof(grpData));

					/*
					if(m_pWriteResponse != 0)
					{
						m_pWriteResponse(m_nGatewayKey, grpData, E_NO_ERROR);
					}
					*/
				}
			}
			else if(m_code == L_DATA_IND)
			{
				if( bNpciIs0 && (nAction == APCI_GROUP_VALUE_READ) )
				{	
					// do nothing except ack
				}
				else if( bNpciIs0 && (nAction == APCI_GROUP_VALUE_RES) )
				{
					// A_GROUP_VALUE_RESPONSE
					// do not add to handle list, but update db
					// AddHandleListNode(ACTION_KNX_READ_RES, (void *) &grpData, sizeof(grpData));
					CGatewayManager::Instance()->WriteKnxValueToDB(m_nGatewayKey, &grpData);
				}
				else if( bNpciIs0 && (nAction == APCI_GROUP_VALUE_WRITE) )
				{
					// add to handle list
					AddHandleListNode(ACTION_KNX_WRITE_RES, (void *) &grpData, sizeof(grpData));

					/*
					// A_GROUP_VALUE_WRITE	
					if(m_pWriteResponse != 0)
					{
						m_pWriteResponse(m_nGatewayKey, grpData, E_NO_ERROR);
					}
					*/
				}
			}
		}
		else
		{
			// don't ack and discard the message
		}

		return 0;
	}
	
	int CKNXKernel::HandleReadResponse(BYTE *pHandle, int nLen)
	{
		return 0;
	}
	
	int CKNXKernel::HandleWriteResponse(BYTE *pHandle, int nLen)
	{
		return 0;
	}
	

