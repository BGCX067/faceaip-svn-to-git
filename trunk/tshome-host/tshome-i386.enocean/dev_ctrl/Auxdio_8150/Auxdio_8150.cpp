/*
 * Auxdio_8150.cpp
 *
 *  Created on: May 14, 2012
 *      Author: kyy
 */

#include "Auxdio_8150.h"
#include "common/ts_err_code.h"
#include <netinet/in.h>

CAuxdio_8150 *pThis = NULL;
CIrcProtocolIface * CreateInstance()
{
	if(pThis != NULL)
	{
		return pThis;
	}
	pThis = new CAuxdio_8150;
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
		nRes += ch;
	}
	return SUCCESS;
}

void CAuxdio_8150::AddData(int nComId, void *pBuf, int nLen)
{
	CIrcProtocolIface::AddData(nComId, pBuf, nLen);
	ParseMessage();
}

int CAuxdio_8150::ParseMessage()
{
	if(m_pOnParsedData == NULL)
	{
		return 0;
	}

	int index = 0;
	int nLeaveLen = 0;
	int nHandleCount = 0;
	char chCheckCode = 0;

	int nCurBodyLen = 0;
	int nAddress = 0;
	int nFunCode = 0;

	int nBackType = -1;
	auxdio_back_02_t *pBack02 = NULL;
	auxdio_back_03_t *pBack03 = NULL;
	auxdio_back_04_t *pBack04 = NULL;
	auxdio_back_05_t *pBack05 = NULL;
	char *pMsg = NULL;
	int nMsgLen = 0;

	for(index = 0; index < TOTAL_COM_NUM; index++)
	{
		if(m_bNeedParse[index])
		{
			nLeaveLen = m_nCurLen[index];
			nHandleCount = 0;

			LOCK_MUTEX(&m_timer[index].timerMutex);
			while(nLeaveLen >= 7 && m_comBuf[index] != NULL && nHandleCount < m_nCurLen[index])
			{
				nBackType = 0;
				nCurBodyLen = 0;

				if( m_comBuf[index][nHandleCount] != 0xFF)
				{
					// the first Char in reply message is not correct, ignore it.
					nLeaveLen--;
					nHandleCount++;
					continue;
				}

				nCurBodyLen = m_comBuf[index][nHandleCount + 1];
				if(nLeaveLen < nCurBodyLen + 4)
				{
					break;
				}

				GetCheckCode(m_comBuf[index] + nHandleCount + 2, nCurBodyLen, chCheckCode);
				if( m_comBuf[index][nHandleCount + nCurBodyLen + 2] != chCheckCode
						|| m_comBuf[index][nHandleCount + nCurBodyLen + 3] != 0xFE)
				{
					// check code incorrect, ignore the first char.
					// stop char incorrect, ignore the first char.
					nLeaveLen--;
					nHandleCount++;
				}
				else
				{
					nAddress = m_comBuf[index][nHandleCount + 2];
					nFunCode = m_comBuf[index][nHandleCount + 3];
					switch(nFunCode)
					{
					case 2:
						if(nCurBodyLen == 6)
						{
							nBackType = BACK_AUXDIO_02;

							int nSourceOnOff = m_comBuf[index][nHandleCount + 4];
							int nSource = 0;
							switch(nSourceOnOff & 0x0F)
							{
							case 1:
								nSource = SOURCE_MP3;
								break;
							case 2:
								nSource = SOURCE_TNR;
								break;
							case 3:
								nSource = SOURCE_DVD;
								break;
							case 4:
								nSource = SOURCE_PC;
								break;
							case 5:
								nSource = SOURCE_TV;
								break;
							case 6:
								nSource = SOURCE_AUX;
								break;
							case 7:
								nSource = SOURCE_IPOD;
								break;
							}

							pBack02 = new auxdio_back_02_t;
							nMsgLen = sizeof(auxdio_back_02_t);
							memset(pBack02, 0, nMsgLen);

							pBack02->address = nAddress;
							pBack02->source = nSource;
							pBack02->on_off = (nSourceOnOff & 0x80) > 0 ? SWITCH_ON : SWITCH_OFF;
							pBack02->main_volume = m_comBuf[index][nHandleCount + 5];
							pBack02->treble_volume = m_comBuf[index][nHandleCount + 6];
							pBack02->bass_volume = m_comBuf[index][nHandleCount + 7];
							pMsg = (char *) pBack02;
						}
						break;
					case 3:
					{
						int nNameLen = m_comBuf[index][nHandleCount + 4];
						if(nCurBodyLen != nNameLen + 3)
						{
							nBackType = BACK_AUXDIO_03;

							nMsgLen = sizeof(auxdio_back_03_t) + nNameLen;
							pBack03 = (auxdio_back_03_t *) new char[nMsgLen];
							memset(pBack03, 0, nMsgLen);

							pBack03->address = nAddress;
							pBack03->len = nNameLen;
							memcpy(pBack03->music_name, m_comBuf[index] + nHandleCount + 5, nNameLen);
							pMsg = (char *) pBack03;
						}
						break;
					}
					case 4:
						if(nCurBodyLen == 8)
						{
							nBackType = BACK_AUXDIO_04;

							pBack04 = new auxdio_back_04_t;
							nMsgLen = sizeof(auxdio_back_04_t);
							memset(pBack04, 0, nMsgLen);

							pBack03->address = nAddress;
							pBack04->has_u_disk = (m_comBuf[index][nHandleCount + 4] == 1) ? SWITCH_ON : SWITCH_ON;
							int nPlayStatus = m_comBuf[index][nHandleCount + 5];
							switch(nPlayStatus)
							{
							case 0:
								pBack04->play_status = PAUSE;
								break;
							case 1:
								pBack04->play_status = PLAY;
								break;
							case 2:
								pBack04->play_status = STOP;
								break;
							}
							int nTotalSec = (m_comBuf[index][nHandleCount + 6] << 8)
									+ m_comBuf[index][nHandleCount + 7];
							pBack04->total_sec = htons(nTotalSec);

							int nCurrSec = (m_comBuf[index][nHandleCount + 8] << 8)
									+ m_comBuf[index][nHandleCount + 9];
							pBack04->current_sec = htons(nCurrSec);

							pMsg = (char *) pBack04;
						}
						break;
					case 5:
						if(nCurBodyLen == 6)
						{
							nBackType = BACK_AUXDIO_05;

							pBack05 = new auxdio_back_05_t;
							nMsgLen = sizeof(auxdio_back_05_t);
							memset(pBack05, 0, nMsgLen);

							int nValue1 = m_comBuf[index][nHandleCount + 4];
							pBack03->address = nAddress;
							pBack05->am_fm = (nValue1 & 0x01) == 1 ? SOURCE_FM : SOURCE_AM;
							pBack05->mute = (nValue1 & 0x02) ? SWITCH_ON : SWITCH_OFF;
							pBack05->stereo = (nValue1 & 0x04) ? SWITCH_ON : SWITCH_OFF;
							pBack05->channel = m_comBuf[index][nHandleCount + 7];
							int nFrequency = (m_comBuf[index][nHandleCount + 5] << 8)
									+ m_comBuf[index][nHandleCount + 6];

							if(pBack05->am_fm == SOURCE_FM)
							{
								pBack05->frequency_100_times = htons(nFrequency);
							}
							else
							{
								pBack05->frequency_100_times = htons(nFrequency * 100);
							}

							pMsg = (char *) pBack05;
						}
						break;
					default:
						nHandleCount += nCurBodyLen + 4;
						nLeaveLen -= (nCurBodyLen + 4);
						break;
					}

					if(nBackType != -1)
					{
						// auto response
						m_pOnParsedData(nBackType, index, m_strProtocolName, pMsg, nMsgLen);
						nBackType = -1;
						if(pMsg != NULL)
						{
							pMsg = NULL;

							if(pBack02 != NULL)
							{
								delete pBack02;
								pBack02 = NULL;
							}
							else if(pBack03 != NULL)
							{
								delete pBack03;
								pBack03 = NULL;
							}
							else if(pBack04 != NULL)
							{
								delete pBack04;
								pBack04 = NULL;
							}
							else if(pBack05 != NULL)
							{
								delete pBack05;
								pBack05 = NULL;
							}
						}
					}
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

int GenerateMessage(int nAddress, int nSource, int nCmd, int nParam, char *pMsg, int &nLen)
{
	pMsg[0] = 0xFF;
	pMsg[1] = nAddress;
	pMsg[2] = nSource;
	pMsg[3] = nCmd;
	pMsg[4] = nParam;
	GetCheckCode(pMsg + 1, 4, pMsg[5]);
	pMsg[6] = 0xFE;
	nLen = 7;
	return SUCCESS;
}

int CAuxdio_8150::GetSerialDeviceMessage(int nFunType,
		int nAddress, int nData, char *pMsg, int &nLen)
{
	if(pMsg == NULL)
	{
		return ERR_PROTROL_PARAMETERS;
	}

	int nSource = 0;
	int nCmd = 0;
	int nParam = 0;

	if(nFunType == CTRL_SWITCH)
	{
		if(nData == SWITCH_ON)
		{
			nCmd = 0xA1;
		}
		else if(nData == SWITCH_OFF)
		{
			nCmd = 0xA0;
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
			nCmd = 0xA2;
		}
		else if(nData == PAUSE)
		{
			nCmd = 0xA3;
		}
		else if(nData == STOP)
		{
			nCmd = 0xA4;
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
			nCmd = 0xA5;
		}
		else if(nData == NEXT)
		{
			nCmd = 0xA6;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_VOLUME)
	{
		nCmd = 0xA7;
		if(nData == VOLUME_UP)
		{
			nParam = 1;
		}
		else if(nData == VOLUME_DOWN)
		{
			nParam = 0;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_TREBLE_VOL)
	{
		nCmd = 0xA9;
		if(nData == VOLUME_UP)
		{
			nParam = 1;
		}
		else if(nData == VOLUME_DOWN)
		{
			nParam = 0;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_BASS_VOL)
	{
		nCmd = 0xAB;
		if(nData == VOLUME_UP)
		{
			nParam = 1;
		}
		else if(nData == VOLUME_DOWN)
		{
			nParam = 0;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_SOURCE)
	{
		nCmd = 0xA7;
		if(m_bVolumeUp[nAddress])
		{
			nParam = 1;
			m_bVolumeUp[nAddress] = false;
		}
		else
		{
			nParam = 0;
			m_bVolumeUp[nAddress] = true;
		}

		switch(nData)
		{
		case SOURCE_MP3:
			nSource = 1;
			break;
		case SOURCE_TNR:
			nSource = 2;
			break;
		case SOURCE_DVD:
			nSource = 3;
			break;
		case SOURCE_PC:
			nSource = 4;
			break;
		case SOURCE_TV:
			nSource = 5;
			break;
		case SOURCE_AUX:
			nSource = 6;
			break;
		case SOURCE_IPOD:
			nSource = 7;
			break;
		default:
			return ERR_PROTROL_PARAMETERS;
			break;
		}
	}
	else if(nFunType == CTRL_TNR_TYPE)
	{
		nCmd = 0xAD;
	}
	else if(nFunType == CTRL_CHANNEL_FM || nFunType == CTRL_CHANNEL_AM)
	{
		if(nFunType == CTRL_CHANNEL_FM)
		{
			nCmd = 0xAE;
		}
		else
		{
			nCmd = 0xAF;
		}

		if(nData < 0)
		{
			nParam = 0;
		}
		else if(nData > 40)
		{
			nParam = 40;
		}
		else
		{
			nParam = nData;
		}
	}
	else if(nFunType == CTRL_DIRECTORY)
	{
		switch(nData)
		{
		case PREV:
			nCmd = 0xB0;
			break;
		case NEXT:
			nCmd = 0xB1;
			break;
		default:
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else
	{
		return ERR_PROTROL_PARAMETERS;
	}
	
	GenerateMessage(nAddress, nSource, nCmd, nParam, pMsg, nLen);

	return SUCCESS;
}
