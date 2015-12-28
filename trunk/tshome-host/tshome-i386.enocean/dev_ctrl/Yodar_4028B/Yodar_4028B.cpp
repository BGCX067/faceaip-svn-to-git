/*
 * Yodar_4028B.cpp
 *
 *  Created on: Aug 31, 2012
 *      Author: kyy
 */
	 
#include <pthread.h>

#include "Yodar_4028B.h"
#include "common/ts_err_code.h"

Yodar_4028B *pThis = NULL;
CIrcProtocolIface * CreateInstance()
{
	if(pThis != NULL)
	{
		return pThis;
	}
	pThis = new Yodar_4028B;
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

Yodar_4028B::Yodar_4028B()
{
	memset((char *) m_data, 0, MAX_DATA_COUNT * sizeof(com_data));

	m_handleThread = NULL;
	INITIALIZE_MUTEX(&m_handleMutex);
	COND_INIT(&m_handleCond);
	
	m_bRun = false;
	m_nReadIndex = 0;
	m_nWriteIndex = 0;
	
	StartHandleThread();
}

Yodar_4028B::~Yodar_4028B()
{
	EndHandleThread();
	
	DESTROY_MUTEX(&m_handleMutex);
	COND_DESTROY(m_handleCond);
}

void Yodar_4028B::StartHandleThread()
{
	if(m_handleThread == 0)
	{
		m_handleThread = CREATE_THREAD(HandlerProcess, this);
	}
}

void Yodar_4028B::EndHandleThread()
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
}

int Yodar_4028B::WaitForSignal()
{
	struct timespec timeout;
	timeout.tv_sec = time(0) + 3;
	timeout.tv_nsec = 0;
	
	LOCK_MUTEX(&m_handleMutex);
	int nRes = pthread_cond_timedwait(&m_handleCond, &m_handleMutex, &timeout);
	UNLOCK_MUTEX(&m_handleMutex);

	return nRes;
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

void Yodar_4028B::AddData(int nComId, void *pBuf, int nLen)
{
	int nCopyLen = nLen > (MAX_DATA_LENGTH - 1) ? (MAX_DATA_LENGTH - 1) : nLen;
	memcpy((char *) m_data[m_nWriteIndex].data, pBuf, nCopyLen);
	m_data[m_nWriteIndex].len = nCopyLen;
	m_data[m_nWriteIndex].index = nComId;
	
	m_nWriteIndex++;
	m_nWriteIndex = m_nWriteIndex >= MAX_DATA_COUNT  ? 0 : m_nWriteIndex;
	
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

	Yodar_4028B *pYodar = (Yodar_4028B *) param;
	pYodar->SetRun(true);

	while(pYodar->GetRun())
	{
		pYodar->WaitForSignal();
		if(!pYodar->GetRun())
		{
			break;
		}
		
		pYodar->DoHandleWork();
	}

	return 0;
}

void Yodar_4028B::DoHandleWork()
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

int Yodar_4028B::HandleMessage(com_data *data)
{
	if(m_pOnParsedData == NULL || data == NULL)
	{
		return 0;
	}
	
	general_single_state *pGeneralState = NULL;
	general_string_state *pGeneralString = NULL;
	int nMsgLen = 0;

	int nTotalLen = data->len;
	int nCurrIndex = 0;
	char chRes = 0;
	int nTemp = 0;

	while(nTotalLen - nCurrIndex >= 5)
	{
		switch((unsigned char ) data->data[nCurrIndex])
		{
		//  message with fixed length of 5
		case 0xB9:
			GetCheckCode(((char *) data->data + nCurrIndex + 1), 3, chRes);
			if(chRes != data->data[nCurrIndex + 4])
			{
				// an error message, ignore
				nCurrIndex += 5;
				break;
			}

			switch(data->data[nCurrIndex + 2])
			{
			case 3:
				if( data->data[nCurrIndex + 3] == 0x80 )
				{
					// switch on & response
					pGeneralState = new general_single_state;
					pGeneralState->type = BACK_SWITCH;
					pGeneralState->value = SWITCH_ON;
				}
				break;
			case 4:
				if( data->data[nCurrIndex + 3] == 0x80 )
				{
					// switch off & response
					pGeneralState = new general_single_state;
					pGeneralState->type = BACK_SWITCH;
					pGeneralState->value = SWITCH_OFF;
				}
				break;
			case 5:
				// source
				nTemp = data->data[nCurrIndex + 3] & 0x7F;
				if( (data->data[nCurrIndex + 3] & 0x80) == 0x80 && nTemp <= 3)
				{
					pGeneralState = new general_single_state;
					pGeneralState->type = BACK_SOURCE;
					switch(nTemp)
					{
					case 0:
						pGeneralState->value = SOURCE_AUX;
						break;
					case 1:
						pGeneralState->value = SOURCE_FM;
						break;
					case 2:
						pGeneralState->value = SOURCE_MP3;
						break;
					case 3:
						pGeneralState->value = SOURCE_AUX2;
						break;
					}
				}
				break;
			case 7:
				// volume
				if( (data->data[nCurrIndex + 3] & 0x80) == 0x80 && (data->data[nCurrIndex + 3] & 0x7F) <= 31)
				{
					pGeneralState = new general_single_state;
					pGeneralState->type = BACK_VOLUME;
					pGeneralState->value = data->data[nCurrIndex + 3] & 0x1F;
				}
				break;
			case 0x0a:
				// equaliser
				nTemp = (data->data[nCurrIndex + 3] & 0x7F);
				if( (data->data[nCurrIndex + 3] & 0x80) == 0x80 && nTemp <= 5)
				{
					pGeneralState = new general_single_state;
					pGeneralState->type = BACK_EQ;
					switch(nTemp)
					{
					case 0:
						pGeneralState->value = EQ_NORMAL;
						break;
					case 1:
						pGeneralState->value = EQ_POP;
						break;
					case 2:
						pGeneralState->value = EQ_SOFT;
						break;
					case 3:
						pGeneralState->value = EQ_CLASSIC;
						break;
					case 4:
						pGeneralState->value = EQ_JAZZ;
						break;
					case 5:
						pGeneralState->value = EQ_METAL;
						break;
					}
				}
				break;
			}
			
			nCurrIndex += 5;
			break;
		case 0xBD:
			// current FM frequency, ignore
			nCurrIndex += 5;
			break;
		case 0xA3:
			break;
		case 0xE9:
			GetCheckCode(((char *) data->data + nCurrIndex + 1), 3, chRes);
			if(chRes != data->data[nCurrIndex + 4])
			{
				// an error message, ignore
				nCurrIndex += 5;
				break;
			}
			
			switch(data->data[nCurrIndex + 2])
			{
			case 2:
				pGeneralState->type= BACK_PLAY;
				nTemp = data->data[nCurrIndex + 3];
				if(nTemp == 0)
				{
					pGeneralState->value = PLAY;
				}
				else if(nTemp == 1)
				{
					pGeneralState->value = PAUSE;
				}
				break;
			case 4:
				pGeneralState->type= BACK_MUTE;
				nTemp = data->data[nCurrIndex + 3];
				if(nTemp == 0)
				{
					pGeneralState->value = MUTE_OFF;
				}
				else if(nTemp == 1)
				{
					pGeneralState->value = MUTE_ON;
				}
				break;
			}
			
			nCurrIndex += 5;
			break;
		case 0xD4:
			// FM number
			nCurrIndex += 5;
			break;
		case 0xD8:
			// total mp3 time
			nCurrIndex += 5;
			break;
		case 0xD5:
			// current mp3 time
			nCurrIndex += 5;
			break;
		case 0xD6:
			// U disk state
			nCurrIndex += 5;
			break;

		// variable length message
		case 0xF1:
			// song name
			nMsgLen = (unsigned char) data->data[nCurrIndex + 2];
			{
				if(nMsgLen == 0)
				{
					// illegal song name
					nCurrIndex += 6;
					break;
				}

				GetCheckCode(((char *) data->data + nCurrIndex + 1), nMsgLen - 2, chRes);
				if(chRes == data->data[nCurrIndex + nMsgLen - 1])
				{
					pGeneralString = new general_string_state;
					pGeneralString->type = BACK_SONG_NAME;
					int nSongNameLen = (nMsgLen - 5) > 255 ? 255 : (nMsgLen - 5);
					pGeneralString->len = nSongNameLen;
					memcpy((char *) pGeneralString->value, ((char *) data->data + nCurrIndex + 4), nSongNameLen);
				}
			}
			
			nCurrIndex += nMsgLen;
			break;
		case 0xF2:
			// album name
			{
				nMsgLen = data->data[nCurrIndex + 2];
				if(nMsgLen == 0)
				{
					// illegal song name
					nCurrIndex += 6;
					break;
				}
			}
			
			nCurrIndex += nMsgLen;
			break;
		case 0xFC:
			// current state
			{
				nMsgLen = data->data[nCurrIndex + 2];
				nCurrIndex += nMsgLen;
			}
			break;

		// don't known what message but fixed length message
		case 0xB0:
		case 0xD7:
		case 0xDE:
		case 0xA1:
			nCurrIndex += 5;
			break;

		default:
			// i don't known what the message is, ignore all.
			break;
		}

		if(pGeneralState != NULL)
		{
			m_pOnParsedData(pGeneralState->type, data->index, m_strProtocolName, pGeneralState, sizeof(general_single_state));
			delete pGeneralState;
			pGeneralState = NULL;
		}
		else if(pGeneralString != NULL)
		{
			m_pOnParsedData(pGeneralString->type, data->index, m_strProtocolName, pGeneralString, sizeof(general_string_state));
			delete pGeneralString;
			pGeneralString = NULL;
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

int Yodar_4028B::GetSerialDeviceMessage(int nFunType,
		int nAddress, int nData, char *pMsg, int &nLen)
{
	if(pMsg == NULL)
	{
		return ERR_PROTROL_PARAMETERS;
	}
	
	int nCmd = 0xB9;
	nAddress = 0x00;
	int nData1 = 0;
	int nData2 = 0;

	if(nFunType == CTRL_SWITCH)
	{
		if(nData == SWITCH_ON)
		{
			nData1 = 3;
		}
		else if(nData == SWITCH_OFF)
		{
			nData1 = 4;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_PLAY)
	{
		if(nData == PLAY || nData == PAUSE)
		{
			nCmd = 0xA3;
			nData1 = 2;
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
			nCmd = 0xA3;
			nData1 = 5;
		}
		else if(nData == NEXT)
		{
			nCmd = 0xA3;
			nData1 = 9;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_VOLUME)
	{
		nData1 = 7;
		if(nData >= 31)
		{
			nData2 = 31;
		}
		else if(nData <= 0)
		{
			nData2 = 0;
		}
		else
		{
			nData2 = nData;
		}
	}
	else if(nFunType == CTRL_SOURCE)
	{
		switch(nData)
		{
		case SOURCE_AUX:
			nData1 = 5;
			nData2 = 0;
			break;
		case SOURCE_FM:
			nData1 = 5;
			nData2 = 1;
			break;
		case SOURCE_MP3:
			nData1 = 5;
			nData2 = 2;
			break;
		case SOURCE_AUX2:
			nData1 = 5;
			nData2 = 3;
			break;
		default:
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_EQ_MODE)
	{
		nData1 = 0x0a;
		switch(nData)
		{
		case EQ_NORMAL:
			nData2 = 0;
			break;
		case EQ_POP:
			nData2 = 1;
			break;
		case EQ_SOFT:
			nData2 = 2;
			break;
		case EQ_CLASSIC:
			nData2 = 3;
			break;
		case EQ_JAZZ:
			nData2 = 4;
			break;
		case EQ_METAL:
			nData2 = 5;
			break;
		default:
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_MUTE)
	{
		nCmd = 0xA3;
		nData1 = 4;
	}
	else
	{
		return ERR_PROTROL_PARAMETERS;
	}

	GenerateMessage(nCmd, nAddress, nData1, nData2, pMsg, nLen);

	return SUCCESS;
}

