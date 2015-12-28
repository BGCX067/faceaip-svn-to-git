/*
 * ISTV.cpp
 *
 *  Created on: Sep 4, 2012
 *      Author: kyy
 */

#include "ISTV.h"
#include "common/ts_err_code.h"

ISTV_041 *pThis = NULL;
CIrcProtocolIface * CreateInstance()
{
	if(pThis != NULL)
	{
		return pThis;
	}
	
	pThis = new ISTV_041;
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

int GenerateMessage(int nStart, int nCmd, int nDataLen, int nData, char *pMsg, int &nLen)
{
	pMsg[0] = (unsigned char) nStart;
	pMsg[1] = (unsigned char) nCmd;
	pMsg[2] = (unsigned char) nDataLen;
	if(nDataLen == 0)
	{
		GetCheckCode(pMsg, 3, pMsg[3]);
		nLen = 4;
	}
	else if(nDataLen == 1)
	{
		pMsg[3] = (unsigned char) nData;
		GetCheckCode(pMsg, 4, pMsg[4]);
		nLen = 5;
	}
	else
	{
		return FAILED;
	}
	return SUCCESS;
}

int ISTV_041::GetSerialDeviceMessage(int nFunType,
			int nAddress, int nData, char *pMsg, int &nLen)
{
	if(pMsg == NULL)
	{
		return ERR_PROTROL_PARAMETERS;
	}

	int nStart = 0xA5;
	int nCmd = 0;
	int nDataLen = 0;
	int nDataBody = 0;

	printf("----ISTV nFunType=%d, nAddress=%d, nData=%d\n", nFunType, nAddress, nData);
	
	if(nFunType == CTRL_SWITCH)
	{
		if(nData == SWITCH_ON)
		{
			nCmd = 0x0B;
		}
		else if(nData == SWITCH_OFF)
		{
			nCmd = 0x0B;
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
			nCmd = 0x22;
		}
		else if(nData == PAUSE)
		{
			nCmd = 0x22;
		}
		else if(nData == STOP)
		{
			nCmd = 0x23;
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
			nCmd = 0x0E;
		}
		else if(nData == VOLUME_DOWN)
		{
			nCmd = 0x10;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_CHANNEL_UPDOWN)
	{
		if(nData == CHANNEL_UP)
		{
			nCmd = 0x0F;
		}
		else if(nData == CHANNEL_DOWN)
		{
			nCmd = 0x11;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_CHANNEL)
	{
		nCmd = 0x11;
		nDataLen = 1;
		nDataBody = nData;
	}
	else if(nFunType == CTRL_SOURCE)
	{
		switch(nData)
		{
		case SOURCE_AV:
			nCmd = 0x12;
			break;
		case SOURCE_TV:
			nCmd = 0x13;
			break;
		case SOURCE_DVD:
			nCmd = 0x30;
			break;
		default:
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_MUTE)
	{
		if(nData == MUTE_ON)
		{
			nCmd = 0x0A;
		}
		else if(nData == MUTE_OFF)
		{
			nCmd = 0x0A;
		}
		else
		{
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_TV_MODE)
	{
		nCmd = 0x3E;
		nDataLen = 1;
		switch(nData)
		{
		case TV_MODE_WELCOME:
			nCmd = 0x3F;
			nDataLen = 0;
			break;
		case TV_MODE_3D_0:
			nDataBody = 0;
			break;
		case TV_MODE_3D_1:
			nDataBody = 1;
			break;
		case TV_MODE_3D_2:
			nDataBody = 2;
			break;
		case TV_MODE_3D_3:
			nDataBody = 3;
			break;
		case TV_MODE_3D_4:
			nDataBody = 4;
			break;
		case TV_MODE_3D_5:
			nDataBody = 5;
			break;
		case TV_MODE_3D_6:
			nDataBody = 6;
			break;
		case TV_MODE_3D_7:
			nDataBody = 7;
			break;
		default:
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else if(nFunType == CTRL_FUNCTION_KEY)
	{
		switch(nData)
		{
		case FUN_DIRECTION_UP:
			nCmd = 0x17;
			break;
		case FUN_DIRECTION_DOWN:
			nCmd = 0x19;
			break;
		case FUN_DIRECTION_LEFT:
			nCmd = 0x16;
			break;
		case FUN_DIRECTION_RIGHT:
			nCmd = 0x18;
			break;
		case FUN_FORWARD_1:
			nCmd = 0x1E;
			break;
		case FUN_FORWARD_2:
			nCmd = 0x20;
			break;
		case FUN_BACKWARD_1:
			nCmd = 0x1F;
			break;
		case FUN_BACKWARD_2:
			nCmd = 0x21;
			break;
		case FUN_OK:
			nCmd = 0x1A;
			break;
		case FUN_HOME:
			nCmd = 0x0D;
			break;
		case FUN_MENU:
			nCmd = 0x15;
			break;
		case FUN_EPG:
			nCmd = 0x0C;
			break;
		case FUN_RECALL:
			nCmd = 0x24;
			break;
		case FUN_BACK:
			nCmd = 0x1B;
			break;
		case FUN_LANGUAGE:
			nCmd = 0x1C;
			break;
		case FUN_CAPTION:
			nCmd = 0x1D;
			break;
		default:
			return ERR_PROTROL_PARAMETERS;
		}
	}
	else
	{
		return ERR_PROTROL_PARAMETERS;
	}

	if(nDataLen == 0 || nDataLen == 1)
	{
		GenerateMessage(nStart, nCmd, nDataLen, nDataBody, pMsg, nLen);
	}
	else
	{
		return ERR_PROTROL_PARAMETERS;
	}
	
	return SUCCESS;
}

