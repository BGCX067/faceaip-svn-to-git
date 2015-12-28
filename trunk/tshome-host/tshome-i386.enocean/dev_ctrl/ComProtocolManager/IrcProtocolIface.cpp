/*
 * IrcProtocolIface.cpp
 *
 *  Created on: Apr 25, 2012
 *      Author: kyy
 */

#include "IrcProtocolIface.h"
CIrcProtocolIface::CIrcProtocolIface()
{
	m_pOnParsedData = NULL;
	m_pToSendComDataByComIndex = NULL;
	m_pToSendSComDataByProtoAddr = NULL;

	memset(m_comBuf, 0, TOTAL_COM_NUM * sizeof(char *));
	memset(m_nCurLen, 0, TOTAL_COM_NUM * sizeof(int));
	memset(m_bNeedParse, false, TOTAL_COM_NUM * sizeof(bool));
	memset(m_bComIndex, false, TOTAL_COM_NUM * sizeof(bool));
	
	m_nDeviceAddressCount = 0;
	for(int index = 0; index < TOTAL_COM_NUM; index++)
	{
		INITIALIZE_MUTEX(&m_timer[index].timerMutex);
		COND_INIT(&m_timer[index].timerCond);

		// must -1 here. because 0 is legal as the address of a device.
		memset((char *) m_nDeviceAddress[index], -1, MAX_DEVICE_COUNT * sizeof(int));
	}
}

CIrcProtocolIface::~CIrcProtocolIface()
{
	int index = 0;
	for(index = 0; index < TOTAL_COM_NUM; index++)
	{
		if(m_comBuf[index] != NULL)
		{
			LOCK_MUTEX(&m_timer[index].timerMutex);
			delete m_comBuf[index];
			m_comBuf[index] = NULL;
			m_nCurLen[index] = 0;
			UNLOCK_MUTEX(&m_timer[index].timerMutex);
		}
	}

	for(index = 0; index < TOTAL_COM_NUM; index++)
	{
		DESTROY_MUTEX(&m_timer[index].timerMutex);
		COND_DESTROY(m_timer[index].timerCond);
	}
}

void CIrcProtocolIface::AddData(int nComId, void *pBuf, int nLen)
{
	if(m_pOnParsedData == NULL || nComId < 0 || nComId > TOTAL_COM_NUM - 1)
	{
		return;
	}

	if(m_comBuf[nComId] == NULL)
	{
		m_nCurLen[nComId] = 0;
		m_comBuf[nComId] = new char[COM_BUF_ZISE];
		memcpy(m_comBuf[nComId], pBuf, nLen);
		m_nCurLen[nComId] = nLen;
		m_bNeedParse[nComId] = true;
		return ;
	}

	LOCK_MUTEX(&m_timer[nComId].timerMutex);
	memcpy(m_comBuf[nComId] + m_nCurLen[nComId], pBuf, nLen);
	m_nCurLen[nComId] += nLen;
	m_bNeedParse[nComId] = true;
	UNLOCK_MUTEX(&m_timer[nComId].timerMutex);
}

void CIrcProtocolIface::AddComIndex(int nComIndex)
{
	if(nComIndex <= 0 || nComIndex > TOTAL_COM_NUM)
	{
		return;
	}

	m_bComIndex[nComIndex - 1] = true;
}

void CIrcProtocolIface::AddDeviceAddress(int nComIndex, int nDevAddr)
{
	if(nComIndex <= 0 || nDevAddr <= -1 || nComIndex > TOTAL_COM_NUM)
	{
		return;
	}

	for(int index = 0; index < MAX_DEVICE_COUNT; index++)
	{
		if(m_nDeviceAddress[nComIndex - 1][index] == nDevAddr)
		{
			break;
		}

		if(m_nDeviceAddress[nComIndex - 1][index] == -1)
		{
			m_nDeviceAddress[nComIndex - 1][index] = nDevAddr;
			m_nDeviceAddressCount++;
			break;
		}
	}
}

