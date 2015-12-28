/*
 * Backaudio_A61L.cpp
 *
 *  Created on: May 14, 2012
 *      Author: kyy
 */

#include "Backaudio_A61L.h"
#include "common/ts_err_code.h"

CBackaudio_A61L *pThis = NULL;
CIrcProtocolIface * CreateInstance()
{
	if(pThis != NULL)
	{
		return pThis;
	}
	pThis = new CBackaudio_A61L;
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

int GetCheckCode(char *pStart, int nLen, char &nRes)
{
	nRes = 0;
	unsigned char ch = 0;
	for(int index = 0; index < nLen; index++)
	{
		ch = pStart[index];
		nRes ^= ch;
	}
	return SUCCESS;
}

void CBackaudio_A61L::AddData(int nComId, void *pBuf, int nLen)
{
	CIrcProtocolIface::AddData(nComId, pBuf, nLen);
	ParseMessage();
}

int CBackaudio_A61L::ParseMessage()
{
	if(m_pOnParsedData == NULL)
	{
		return 0;
	}

	int index = 0;
	int nLeaveLen = 0;
	int nHandleCount = 0;
	char chCheckCode = 0;

	int nVolume = 0;
	int nEqSource = 0;
	int nEq = 0;
	int nSource = 0;
	backaudio_back_t *pMsg = NULL;
	int nMsgLen = 0;

	for(index = 0; index < TOTAL_COM_NUM; index++)
	{
		if(m_bNeedParse[index])
		{
			nHandleCount = 0;
			nLeaveLen = m_nCurLen[index];
			LOCK_MUTEX(&m_timer[index].timerMutex);
			while(nLeaveLen >= 5 && m_comBuf[index] != NULL && nHandleCount < m_nCurLen[index])
			{
				GetCheckCode(m_comBuf[index + 1], 3, chCheckCode);
				if( (m_comBuf[index][nHandleCount] != 0xA3
						&& m_comBuf[index][nHandleCount] != 0xA4
						&& m_comBuf[index][nHandleCount] != 0xAD)
						|| m_comBuf[index][nHandleCount + 4] != chCheckCode )
				{
					// the first Char in reply message is not correct, ignore it.
					// check code incorrect, ignore the first char too.
					nHandleCount++;
					nLeaveLen--;
				}
				else
				{
					switch( (unsigned char) m_comBuf[index][nHandleCount])
					{
					case 0xA3:
					case 0xA4:
						nVolume = m_comBuf[index][nHandleCount + 1];
						if(m_comBuf[index][nHandleCount + 2] == 0xFF)
						{
							break;
						}
						nEqSource = m_comBuf[index][nHandleCount + 3];
						nEq = (nEqSource & 0xF0);
						switch(nEq)
						{
						case 0x00:
							nEq = EQ_NORMAL;
							break;
						case 0x10:
							nEq = EQ_POP;
							break;
						case 0x20:
							nEq = EQ_SOFT;
							break;
						case 0x30:
							nEq = EQ_CLASSIC;
							break;
						case 0x40:
							nEq = EQ_JAZZ;
							break;
						case 0x50:
							nEq = EQ_METAL;
							break;
						}
						nSource = (nEqSource & 0x0F);
						switch(nSource)
						{
						case 0x05:
							nSource = SOURCE_FM;
							break;
						case 0x03:
							nSource = SOURCE_MP3;
							break;
						case 0x06:
							nSource = SOURCE_DVD;
							break;
						}
						nMsgLen = sizeof(backaudio_back_t);
						pMsg = new backaudio_back_t;
						pMsg->volume = nVolume;
						pMsg->eq = nEq;
						pMsg->source = nSource;
						break;
					case 0xAD:
						break;
					}
					m_pOnParsedData(BACK_BACKAUDIO, index, m_strProtocolName, pMsg, nMsgLen);
					if(pMsg != NULL)
					{
						delete pMsg;
						pMsg = NULL;
						nMsgLen = 0;
					}

					nHandleCount += 5;
					nLeaveLen -= 5;
				}
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

int GenerateMessage(int nCmd, int nAddress, int nData1, int nData2, char *pMsg, int &nLen)
{
	pMsg[0] = nCmd;
	pMsg[1] = nAddress;
	pMsg[2] = nData1;
	pMsg[3] = nData2;
	GetCheckCode(pMsg + 1, 3, pMsg[4]);
	nLen = 5;
	return SUCCESS;
}

int CBackaudio_A61L::GetSerialDeviceMessage(int nFunType,
		int nAddress, int nData, char *pMsg, int &nLen)
{
	if(pMsg == NULL)
	{
		return ERR_PROTROL_PARAMETERS;
	}

	int nCmd = 0xA3;
	int nData1 = 0;
	int nData2 = 0;

	if(nFunType == CTRL_SWITCH)
	{
		if(nData == SWITCH_ON)
		{
			nData1 = 7;
		}
		else if(nData == SWITCH_OFF)
		{
			nData1 = 3;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_PLAY)
	{
		if(nData == PLAY)
		{
			nData1 = 2;
			nData2 = 0;
		}
		else if(nData == PAUSE)
		{
			nData1 = 2;
			nData2 = 1;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_PREV_NEXT)
	{
		if(nData == PREV)
		{
			nData1 = 5;
		}
		else if(nData == NEXT)
		{
			nData1 = 9;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_VOLUME)
	{
		if(nData == VOLUME_UP)
		{
			nData1 = 1;
		}
		else if(nData == VOLUME_DOWN)
		{
			nData1 = 8;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_SOURCE)
	{
		switch(nData)
		{
		case SOURCE_FM:
			nData1 = 4;
			nData2 = 1;
			break;
		case SOURCE_MP3:
			nData1 = 4;
			nData2 = 2;
			break;
		case SOURCE_DVD:
			nData1 = 6;
			nData2 = 0;
			break;
		default:
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_DIRECTORY)
	{
		nCmd = 0xAD;
		switch(nData)
		{
		case PREV:
			nData1 = 1;
			break;
		case NEXT:
			nData1 = 2;
			break;
		default:
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_EQ_MODE)
	{
		nCmd = 0xA4;
		switch(nData)
		{
		case EQ_NORMAL:
			nData2 = 0x00;
			break;
		case EQ_POP:
			nData2 = 0x10;
			break;
		case EQ_SOFT:
			nData2 = 0x20;
			break;
		case EQ_CLASSIC:
			nData2 = 0x30;
			break;
		case EQ_JAZZ:
			nData2 = 0x40;
			break;
		case EQ_METAL:
			nData2 = 0x50;
			break;
		default:
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else
	{
		return ERR_PROTROL_PARAMETERS;
	}

	GenerateMessage(nCmd, nAddress, nData1, nData2, pMsg, nLen);

	return SUCCESS;
}
