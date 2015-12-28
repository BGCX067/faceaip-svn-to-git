/*
 * AQM_1000.cpp
 *
 *  Created on: Sep 4, 2012
 *      Author: kyy
 */

#include "AQM_1000.h"
#include "common/ts_err_code.h"
#include <unistd.h>

AQM_1000_CLS *pThis = NULL;
CIrcProtocolIface * CreateInstance()
{
	if(pThis != NULL)
	{
		return pThis;
	}
	
	pThis = new AQM_1000_CLS;
	return pThis;
}

void DeleteInstance(CIrcProtocolIface *p)
{
	if(pThis != NULL)
	{
		delete pThis;
		pThis = NULL;
	}
}

AQM_1000_CLS::AQM_1000_CLS()
{
	memset((char *) m_data, 0, MAX_DATA_COUNT * sizeof(com_data));

	m_handleThread = 0;
	m_sendThread = 0;
	
	INITIALIZE_MUTEX(&m_handleMutex);
	COND_INIT(&m_handleCond);
	
	INITIALIZE_MUTEX(&m_sendMutex);
	COND_INIT(&m_sendCond);
	
	m_bRun = false;
	m_nReadIndex = 0;
	m_nWriteIndex = 0;
	
	StartHandleAndSendThread();
}

AQM_1000_CLS::~AQM_1000_CLS()
{
	EndHandleAndSendThread();
	
	DESTROY_MUTEX(&m_handleMutex);
	COND_DESTROY(m_handleCond);
	
	DESTROY_MUTEX(&m_sendMutex);
	COND_DESTROY(m_sendCond);
}

void AQM_1000_CLS::StartHandleAndSendThread()
{
	memset((char *) buf_data, 0, MAX_BUFFER_LEN);
	nCurrentBufLen = 0;
	
	if(m_handleThread == 0)
	{
		m_handleThread = CREATE_THREAD(HandlerProcess, this);
	}

	if(m_sendThread == 0)
	{
		m_handleThread = CREATE_THREAD(SendMsgProcess, this);
	}
}

void AQM_1000_CLS::EndHandleAndSendThread()
{
	m_bRun = FALSE;
	if(m_handleThread != 0)
	{
		LOCK_MUTEX(&m_handleMutex);
		SET_SIGNAL(m_handleCond);
		UNLOCK_MUTEX(&m_handleMutex);
		WAIT_THREAD_EXIT(m_handleThread);
		m_handleThread = 0;
	}
	
	if(m_sendThread != 0)
	{
		LOCK_MUTEX(&m_sendMutex);
		SET_SIGNAL(m_sendCond);
		UNLOCK_MUTEX(&m_sendMutex);
		WAIT_THREAD_EXIT(m_sendThread);
		m_sendThread = 0;
	}
}

int AQM_1000_CLS::WaitForHandleSignal()
{
	struct timespec timeout;
	timeout.tv_sec = time(0) + 3;
	timeout.tv_nsec = 0;
	
	LOCK_MUTEX(&m_handleMutex);
	int nRes = pthread_cond_timedwait(&m_handleCond, &m_handleMutex, &timeout);
	UNLOCK_MUTEX(&m_handleMutex);

	return nRes;
}

int AQM_1000_CLS::WaitForSendSignal()
{
	int nWaitSec = 3;
	if(m_nDeviceAddressCount > 100)
	{
		nWaitSec = 2;
	}
	else if(m_nDeviceAddressCount > 200)
	{
		nWaitSec = 1;
	}
		
	struct timespec timeout;
	timeout.tv_sec = time(0) + nWaitSec;
	timeout.tv_nsec = 0;
	
	LOCK_MUTEX(&m_sendMutex);
	int nRes = pthread_cond_timedwait(&m_sendCond, &m_sendMutex, &timeout);
	UNLOCK_MUTEX(&m_sendMutex);

	return nRes;
}

void AQM_1000_CLS::AddData(int nComId, void *pBuf, int nLen)
{
	int nCopyLen = nLen > (MAX_DATA_LENGTH - 1) ? (MAX_DATA_LENGTH - 1) : nLen;
	memcpy((char *) m_data[m_nWriteIndex].data, pBuf, nCopyLen);
	m_data[m_nWriteIndex].len = nCopyLen;
	m_data[m_nWriteIndex].index = nComId;
	
	m_nWriteIndex++;
	if(m_nWriteIndex >= MAX_DATA_COUNT)
	{
		m_nWriteIndex = 0;
	}
	
	LOCK_MUTEX(&m_handleMutex);
	SET_SIGNAL(m_handleCond);
	UNLOCK_MUTEX(&m_handleMutex);
}

int HandlerProcess(void *param)
{
	if(param == NULL)
	{
		return -1;
	}

	AQM_1000_CLS *pAqm = (AQM_1000_CLS *) param;
	pAqm->SetRun(true);

	while(pAqm->GetRun())
	{
		pAqm->WaitForHandleSignal();
		if(!pAqm->GetRun())
		{
			break;
		}
		
		pAqm->DoHandleWork();
	}

	return 0;
}

int SendMsgProcess(void *param)
{
	if(param == NULL)
	{
		return -1;
	}

	AQM_1000_CLS *pAqm = (AQM_1000_CLS *) param;
	pAqm->SetRun(true);

	while(pAqm->GetRun())
	{
		pAqm->WaitForSendSignal();
		if(!pAqm->GetRun())
		{
			break;
		}
		
		pAqm->DoSendMsgWork();
	}

	return 0;
}

void AQM_1000_CLS::DoHandleWork()
{
	if(m_nReadIndex == m_nWriteIndex)
	{
		return ;
	}

	if(m_nReadIndex > m_nWriteIndex)
	{
		while(m_nReadIndex < MAX_DATA_COUNT && m_bRun)
		{
			HandleMessage( & m_data[m_nReadIndex]);
			memset((char *) m_data[m_nReadIndex].data, 0, MAX_DATA_LENGTH);
			m_data[m_nReadIndex].len = 0;
			
			m_nReadIndex++;
			if(m_nReadIndex >= MAX_DATA_COUNT)
			{
				m_nReadIndex = 0;
				break;
			}
		}
	}

	while(m_nReadIndex < m_nWriteIndex && m_bRun)
	{
		HandleMessage(&m_data[m_nReadIndex]);
		memset((char *) m_data[m_nReadIndex].data, 0, MAX_DATA_LENGTH);
		m_data[m_nReadIndex].len = 0;
		m_nReadIndex++;
	}
}

void AQM_1000_CLS::DoSendMsgWork()
{
	if(m_nDeviceAddressCount <= 0 || m_pToSendComDataByComIndex == NULL)
	{
		return ;
	}
	
	char message[16] = {0};
	int nLen = 0;
	int nRes = -1;

	int nDevAddr = 0;
	for(int nComIndex = 0; nComIndex < TOTAL_COM_NUM && m_bRun; nComIndex++)
	{
		if(m_bComIndex[nComIndex] == true)
		{
			for(int nDevAddrIdx = 0; nDevAddrIdx < MAX_DEVICE_COUNT && m_bRun; nDevAddrIdx++)
			{
				nDevAddr = m_nDeviceAddress[nComIndex][nDevAddrIdx];
				if(nDevAddr > -1)
				{
					// Get message
					nRes = GetSerialDeviceMessage(0, nDevAddr, 0, (char *) message, nLen);
					if(nRes == SUCCESS && nLen > 0)
					{
						m_pToSendComDataByComIndex(nComIndex + 1, (char *) message, nLen);

						usleep(20 * 1000);
					}
				}
				else
				{
					// first -1 in arrary.
					break;
				}
			}
		}
	}
}

int GetCheckCode(char *pStart, int nLen, char &nRes)
{
	nRes = 0;
	unsigned char ch = 0;
	for(int index = 0; index < nLen; index++)
	{
		ch = pStart[index];
		nRes += ch;
	}
	return SUCCESS;
}

int AQM_1000_CLS::HandleMessage(com_data *data)
{
	if(m_pOnParsedData == NULL || data == NULL)
	{
		return 0;
	}
	
	aqm_1000_state *pState = NULL;

	int nCopyLen = data->len;
	if(nCopyLen > (MAX_BUFFER_LEN - nCurrentBufLen - 1) )
	{
		nCopyLen = (MAX_BUFFER_LEN - nCurrentBufLen - 1);
	}
	memcpy((char *)buf_data + nCurrentBufLen, data->data, nCopyLen);
	nCurrentBufLen += nCopyLen;

	int nCurrIndex = 0;
//	unsigned char nAddress = 0;		// will used later
	unsigned char nInstalled = 0;
	char chSum = 0;
	

	while(nCurrentBufLen - nCurrIndex >= 12)
	{
		if(buf_data[nCurrIndex] != 0xFA)
		{
			nCurrIndex++;
			continue;
		}
		
		if(buf_data[nCurrIndex + 1] != 0xF0)
		{
			nCurrIndex += 2;
			continue;
		}
		
		if(buf_data[nCurrIndex + 11] != 0x05)
		{
			nCurrIndex += 2;
			continue;
		}

		GetCheckCode((char *) buf_data + nCurrIndex, 10, chSum);
		if(buf_data[nCurrIndex + 10] != chSum)
		{
			nCurrIndex += 12;
			continue;
		}

		// will used later
		// nAddress = buf_data[nCurrIndex + 2];
		nInstalled = buf_data[nCurrIndex + 3];

		if( (nInstalled & 0x0F) > 0)
		{
			pState = new aqm_1000_state;
			memset(pState, 0, sizeof(aqm_1000_state));

			if( (nInstalled & 0x08) > 0)
			{
				pState->co2_concentration = ( ((int) buf_data[nCurrIndex + 4]) << 8) + (int) buf_data[nCurrIndex + 5];
			}
			else
			{
				pState->co2_concentration = 0;
			}
			
			if( (nInstalled & 0x04) > 0)
			{
				int nTemperature = (((int) buf_data[nCurrIndex + 6]) << 8) + (int) buf_data[nCurrIndex + 7];
				pState->temperature = (nTemperature * 1.0) / 10;
			}
			else
			{
				pState->temperature = 0;
			}
			
			if( (nInstalled & 0x02) > 0)
			{
				int nHumidity = buf_data[nCurrIndex + 8];
				pState->humidity_per = (nHumidity * 1.0) / 100;
			}
			else
			{
				pState->humidity_per = 0;
			}
			
			if( (nInstalled & 0x01) > 0)
			{
				if(buf_data[nCurrIndex + 9] == 0x10)
				{
					pState->voc_level = VOC_LEVEL_HIGHT;
				}
				else if(buf_data[nCurrIndex + 9] == 0x40)
				{
					pState->voc_level = VOC_LEVEL_VERY_HIGHT;
				}
				else //if(buf_data[nCurrIndex + 9] == 0x00)
				{
					pState->voc_level = VOC_LEVEL_NORMAL;
				}
			}
			else
			{
				pState->voc_level = VOC_LEVEL_NORMAL;
			}
		}

		if(pState != NULL)
		{
			m_pOnParsedData(BACK_AQM_1000, data->index, m_strProtocolName, pState, sizeof(aqm_1000_state));
			delete pState;
			pState = NULL;
		}
		nCurrIndex += 12;
	}

	if(nCurrIndex < nCurrentBufLen)
	{
		char left_data[MAX_BUFFER_LEN] = {0};
		int nLeftLen = nCurrentBufLen - nCurrIndex;
		memcpy((char *) left_data, (char *) buf_data + nCurrIndex, nLeftLen);
		memset((char *) buf_data, 0, MAX_BUFFER_LEN);
		memcpy((char *) buf_data, (char *) left_data, nLeftLen);
		nCurrentBufLen = nLeftLen;
	}
	else
	{
		memset(buf_data, 0, MAX_BUFFER_LEN);
		nCurrentBufLen = 0;
	}

	return 0;
}

int GenerateMessage(int nStart1, int nStart2, int nAddress, char *pMsg, int &nLen)
{
	pMsg[0] = (unsigned char) nStart1;
	pMsg[1] = (unsigned char) nStart2;
	pMsg[2] = (unsigned char) nAddress;
	GetCheckCode(pMsg, 3, pMsg[3]);
	pMsg[4] = 0x05;
	nLen = 5;
	return SUCCESS;
}

int AQM_1000_CLS::GetSerialDeviceMessage(int nFunType,
			int nAddress, int nData, char *pMsg, int &nLen)
{
	if(pMsg == NULL)
	{
		return ERR_PROTROL_PARAMETERS;
	}

	int nStart1 = 0xF5;
	int nStart2 = 0xFC;

	printf("----AQM_1000 nAddress=%d----\n", nAddress);

	GenerateMessage(nStart1, nStart2, nAddress, pMsg, nLen);
	
	return SUCCESS;
}

