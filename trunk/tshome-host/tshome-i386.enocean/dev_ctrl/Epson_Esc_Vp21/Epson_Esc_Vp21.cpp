#include "Epson_Esc_Vp21.h"
#include "common/ts_err_code.h"

BYTE EpsonCtrlOn[] = { 0x50, 0x57, 0x52, 0x20, 0x4F, 0x4E, 0x0D};
BYTE EpsonCtrlOff[] = { 0x50, 0x57, 0x52, 0x20, 0x4F, 0x46, 0x46, 0x0D};

BYTE EpsonCtrlSource10[] = { 0x53, 0x4F, 0x55, 0x52, 0x43, 0x45, 0x20, 0x31, 0x30, 0x0D};
BYTE EpsonCtrlSource20[] = { 0x53, 0x4F, 0x55, 0x52, 0x43, 0x45, 0x20, 0x32, 0x30, 0x0D};
BYTE EpsonCtrlSource30[] = { 0x53, 0x4F, 0x55, 0x52, 0x43, 0x45, 0x20, 0x33, 0x30, 0x0D};
BYTE EpsonCtrlSource41[] = { 0x53, 0x4F, 0x55, 0x52, 0x43, 0x45, 0x20, 0x34, 0x31, 0x0D};
BYTE EpsonCtrlSource42[] = { 0x53, 0x4F, 0x55, 0x52, 0x43, 0x45, 0x20, 0x34, 0x32, 0x0D};
BYTE EpsonCtrlSourceB0[] = { 0x53, 0x4F, 0x55, 0x52, 0x43, 0x45, 0x20, 0x42, 0x30, 0x0D};

BYTE EpsonCtrlAvOn[] = { 0x4D, 0x55, 0x54, 0x45, 0x20, 0x4F, 0x4E, 0x0D};
BYTE EpsonCtrlAvOff[] = { 0x4D, 0x55, 0x54, 0x45, 0x20, 0x4F, 0x46, 0x46, 0x0D};

BYTE EpsonCtrlAvMsel00[] = { 0x4D, 0x53, 0x45, 0x4C, 0x20, 0x30, 0x30, 0x0D};
BYTE EpsonCtrlAvMsel01[] = { 0x4D, 0x53, 0x45, 0x4C, 0x20, 0x30, 0x31, 0x0D};
BYTE EpsonCtrlAvMsel02[] = { 0x4D, 0x53, 0x45, 0x4C, 0x20, 0x30, 0x32, 0x0D};

CEpsonEscVp21 *pThis = NULL;
CIrcProtocolIface * CreateInstance()
{
	if(pThis != NULL)
	{
		return pThis;
	}
	pThis = new CEpsonEscVp21;
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


void CEpsonEscVp21::AddData(int nComId, void *pBuf, int nLen)
{
	return;

	// not used any more.
	CIrcProtocolIface::AddData(nComId, pBuf, nLen);
	ParseMessage();
}

int CEpsonEscVp21::ParseMessage()
{
	if(m_pOnParsedData == NULL)
	{
		return 0;
	}

	int index = 0;
	int nLeaveLen = 0;
	int nHandleCount = 0;
	for(index = 0; index < TOTAL_COM_NUM; index++)
	{
		if(m_bNeedParse[index])
		{
			nHandleCount = 0;
			nLeaveLen = m_nCurLen[index];
			while(nLeaveLen > 0 && m_comBuf[index] != NULL && nHandleCount < m_nCurLen[index])
			{
				if(m_comBuf[index][nHandleCount] == 0x3a)	// success if Epson response 0x3a(':').
				{
					m_pOnParsedData(BACK_EPSON, index, m_strProtocolName, NULL, 0);
				}

				nHandleCount++;
				nLeaveLen--;
			}

			if(nLeaveLen == 0)
			{
				memset(m_comBuf[index], 0, COM_BUF_ZISE);
			}
			else if(nHandleCount > 0)
			{
				memcpy(m_comBuf[index], m_comBuf[index] + nHandleCount, nLeaveLen);
				memset(m_comBuf[index] + nLeaveLen, 0, nHandleCount);
			}
			m_nCurLen[index] = nLeaveLen;
			m_bNeedParse[index] = false;
			UNLOCK_MUTEX(&m_timer[index].timerMutex);
		}
	}

	return 0;
}

int CEpsonEscVp21::GetSerialDeviceMessage(int nFunType, int nAddress, int nData, char *pMsg, int &nLen)
{
	if(pMsg == NULL)
	{
		return ERR_PROTROL_PARAMETERS;
	}

	int nRes = SUCCESS;

	if(nFunType == CTRL_SWITCH)
	{
		if(nData == SWITCH_ON)
		{
			nLen = sizeof(EpsonCtrlOn);
			memcpy(pMsg, EpsonCtrlOn, nLen);
		}
		else if(nData == SWITCH_OFF)
		{
			nLen = sizeof(EpsonCtrlOff);
			memcpy(pMsg, EpsonCtrlOff, nLen);
		}
		else
		{
			nRes = ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_SOURCE)
	{
		switch(nData)
		{
		case SOURCE_PC:
			nLen = sizeof(EpsonCtrlSource10);
			memcpy(pMsg, EpsonCtrlSource10, nLen);
			break;
		case SOURCE_PC_2:
			nLen = sizeof(EpsonCtrlSource20);
			memcpy(pMsg, EpsonCtrlSource20, nLen);
			break;
		case SOURCE_DVI_HDMI:
			nLen = sizeof(EpsonCtrlSource30);
			memcpy(pMsg, EpsonCtrlSource30, nLen);
			break;
		case SOURCE_VIDEO:
			nLen = sizeof(EpsonCtrlSource41);
			memcpy(pMsg, EpsonCtrlSource41, nLen);
			break;
		case SOURCE_S_VIDEO:
			nLen = sizeof(EpsonCtrlSource42);
			memcpy(pMsg, EpsonCtrlSource42, nLen);
			break;
		case SOURCE_BNC:
			nLen = sizeof(EpsonCtrlSourceB0);
			memcpy(pMsg, EpsonCtrlSourceB0, nLen);
			break;
		default:
			nRes = ERR_PROTROL_PARAMETERS;
			break;
		}
	}
	else if(nFunType == CTRL_AV_SWITCH)
	{
		if(nData == SWITCH_ON)
		{
			nLen = sizeof(EpsonCtrlAvOn);
			memcpy(pMsg, EpsonCtrlAvOn, nLen);
		}
		else if(nData == SWITCH_OFF)
		{
			nLen = sizeof(EpsonCtrlAvOff);
			memcpy(pMsg, EpsonCtrlAvOff, nLen);
		}
		else
		{
			nRes = ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_AV_TYPE)
	{
		switch(nData)
		{
		case EPSON_AV_MSEL_00:
			nLen = sizeof(EpsonCtrlAvMsel00);
			memcpy(pMsg, EpsonCtrlAvMsel00, nLen);
			break;
		case EPSON_AV_MSEL_01:
			nLen = sizeof(EpsonCtrlAvMsel01);
			memcpy(pMsg, EpsonCtrlAvMsel01, nLen);
			break;
		case EPSON_AV_MSEL_02:
			nLen = sizeof(EpsonCtrlAvMsel02);
			memcpy(pMsg, EpsonCtrlAvMsel02, nLen);
			break;
		default:
			nRes = ERR_PROTROL_PARAMETERS;
			break;
		}
	}
	else
	{
		return ERR_PROTROL_NOT_SUPPORT;
	}

	return nRes;
}



