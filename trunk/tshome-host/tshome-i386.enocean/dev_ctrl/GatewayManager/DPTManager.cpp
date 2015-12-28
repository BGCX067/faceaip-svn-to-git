 //
//  DPTManager.mm
//  tsbus_knx
//
//  Created by kyy on 11-5-20.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//


#include "KNXWinVarType.h"

#include "KNXCondef.h"
#include "DPTManager.h"
#include <string.h>

	KNX_DATA_TYPE CDPTManager::GetVarTypeFromDPT(int nDPT)
	{
		KNX_DATA_TYPE nResult = DATA_TYPE_UNKNOWN;
		switch (nDPT)
		{
			case DPT_Boolean:
				nResult = DATA_TYPE_BINARY;
				break;
			case DPT_Date:
				nResult = DATA_TYPE_DATE;
				break;
			case DPT_Time:
				nResult = DATA_TYPE_TIME;
				break;
			case DPT_String:
				nResult = DATA_TYPE_TEXT;
				break;
			case DPT_1BitCtrl:
			case DPT_3BitCtrl:
			case DPT_CharSet:
			case DPT_8BitUnSign:
			case DPT_8BitSign:
			case DPT_2OctUnSign:
			case DPT_2OctSign:
			case DPT_2OctFloat:
			case DPT_4OctUnSign:
			case DPT_4OctSign:
			case DPT_4OctFloat:
			case DPT_Access:
				nResult = DATA_TYPE_ANALOG;
				break;
			default:
				nResult = DATA_TYPE_UNKNOWN;
				break;
		}
		return nResult;
	}
	
	/******************************************************************	 
	 params:[in] grpData
			[in/out] pBuf
			[in/out] nLen, [in] sizeof pBuf, [out]bytes of used in (pBuf)
	 ******************************************************************/
	int CDPTManager::DPT2Byte(KNX_GroupData grpData, BYTE * pBuf, int &nLen)
	{
		int nBufSize = nLen;
		nLen = 0;
		switch (grpData.nGrpDPT)
		{
			case DPT_Boolean:
				if(nBufSize < 1)
					return -1;
				
				*pBuf = grpData.Data.BinData == true ? 1 : 0;
				nLen = 0;
				break;
				
			case DPT_1BitCtrl:
			case DPT_3BitCtrl:
				if(nBufSize < 1)
					return -1;
				
				*pBuf = (BYTE) grpData.Data.AnaData;
				nLen = 0;
				break;
				
			case DPT_CharSet:
			case DPT_8BitUnSign:
				if(nBufSize < 1)
					return -1;
				
				*pBuf = (BYTE) grpData.Data.AnaData;
				nLen = 1;
				break;
				
			case DPT_8BitSign:
				if(nBufSize < 1)
					return -1;
				
				*pBuf = (BYTE) grpData.Data.AnaData;
				nLen = 1;
				break;
				
			case DPT_2OctUnSign:
				if(nBufSize < 2)
					return -1;
				
				*pBuf = (BYTE) ((WORD) grpData.Data.AnaData >> 8);
				*(pBuf + 1) = (BYTE) grpData.Data.AnaData;
				nLen = 2;
				break;
				
			case DPT_2OctSign:
				if(nBufSize < 2)
					return -1;
				
				*pBuf = (BYTE) ((short) grpData.Data.AnaData >> 8);
				*(pBuf + 1) = (BYTE) grpData.Data.AnaData;
				nLen = 2;
				break;
				
			case DPT_2OctFloat:
				/* 16-bit float
				 *  M EEEE MMM - MMMM MMMM
				 *  (0.01*M) * 2^E
				 */
			{
				if(nBufSize < 2)
					return -1;
				
				// 16-bit float => byte[2]
				int nVal = (int) grpData.Data.AnaData * 100;
				int nMax = 1 << 11;
				int nMin = - (1 << 11);
				int nExp = 0;
				if (nVal >= 0)
				{
					while (nVal >= nMax)
					{
						nExp ++;
						nVal = nVal >> 1;
						if (nExp == 15)
						{
							break;
						}
					}
					
					//max value, if it has exceeded the max capability
					if (nVal >= nMax)
					{
						nVal = nMax - 1;
					}
					
					*pBuf = (nExp << 3) | (nVal >> 8);
					*(pBuf + 1) = (BYTE) nVal;
				}
				else
				{
					while (nVal < nMin)
					{
						nExp ++;
						nVal = nVal >> 1;
						if (nExp == 15)
						{
							break;
						}
					}
					
					if (nVal < nMin)
					{
						nVal = nMin;
					}
					
					*pBuf = (nExp << 3) | ((nVal >> 8) & 0x87);
					*(pBuf + 1) = (BYTE) nVal;
				}
				
				nLen = 2;
			}
				break;
				
			case DPT_4OctUnSign:
				if(nBufSize < 4)
					return -1;
				
				*pBuf = (BYTE) ((UINT) grpData.Data.AnaData >> 24);
				*(pBuf + 1) = (BYTE) ((UINT) grpData.Data.AnaData >> 16);
				*(pBuf + 2) = (BYTE) ((UINT) grpData.Data.AnaData >> 8);
				*(pBuf + 3) = (BYTE) grpData.Data.AnaData;
				nLen = 4;
				break;
				
			case DPT_4OctSign:
				if(nBufSize < 4)
					return -1;
				
				*pBuf = (BYTE) ((int) grpData.Data.AnaData >> 24);
				*(pBuf + 1) = (BYTE) ((int) grpData.Data.AnaData >> 16);
				*(pBuf + 2) = (BYTE) ((int) grpData.Data.AnaData >> 8);
				*(pBuf + 3) = (BYTE) grpData.Data.AnaData;
				nLen = 4;
				break;
				
			case DPT_4OctFloat:
			{
				if(nBufSize < 4)
					return -1;
				
				float fVal = (float) grpData.Data.AnaData;
				if( ((int)fVal & 0x007fffff) > 8388607)
					return -1;
				
				memcpy(pBuf, &fVal, sizeof(float));
				nLen = sizeof(float);
			}
				break;
				
			case DPT_Time:
			{
				if(nBufSize < 3)
					return -1;
				
				if(grpData.Data.TimeData.hour > 23 || grpData.Data.TimeData.minute > 59
				   || grpData.Data.TimeData.second > 59 || grpData.Data.TimeData.weekday > 7)
					return -1;
				
				*pBuf = grpData.Data.TimeData.weekday<<5 | grpData.Data.TimeData.hour;
				*(pBuf + 1) = grpData.Data.TimeData.minute;
				*(pBuf + 2) = grpData.Data.TimeData.second;
				nLen = 3;
			}
				break;
				
			case DPT_Date:
			{
				if(nBufSize < 3)
					return -1;
				
				if(grpData.Data.TimeData.year < 1990 || grpData.Data.TimeData.year > 2089
				   || grpData.Data.TimeData.month < 1 || grpData.Data.TimeData.month > 12
				   || grpData.Data.TimeData.day < 1 || grpData.Data.TimeData.day > 31)
				{
					return -1;
				}
				
				if(grpData.Data.TimeData.year < 2000)
					*pBuf = grpData.Data.TimeData.year - 1900;
				else 
					*pBuf = 2089 - grpData.Data.TimeData.year;
				
				*(pBuf + 1) = grpData.Data.TimeData.month;
				*(pBuf + 2) = grpData.Data.TimeData.day;


				nLen = 3;
			}
				break;
				
			case DPT_Access:
			{
				if(nBufSize < 4)
					return -1;
				
				UINT nVal = (UINT) grpData.Data.AnaData;
				if( (nVal>>28 & 0x0F) > 9 || (nVal>>24 & 0x0F) > 9 ||(nVal>>20 & 0x0F) > 9
				   || (nVal>>16 & 0x0F) > 9 || (nVal>>12 & 0x0F) > 9 || (nVal>>8 & 0x0F) > 9)
				{
					return -1;
				}
				
				*pBuf = (BYTE) (nVal >> 24);
				*(pBuf + 1) = (BYTE) (nVal >> 16);
				*(pBuf + 2) = (BYTE) (nVal >> 8);
				*(pBuf + 3) = (BYTE) nVal;
				nLen = 4;
				break;
			}
				
			case DPT_String:
			{
				if(nBufSize < MAX_DATA_AREASIZE)
					return -1;
				
				memcpy(pBuf, grpData.Data.TxtData, MAX_DATA_AREASIZE);
				int nStrLen = strlen((char *) pBuf);
				if(nStrLen < nBufSize)
					memset(pBuf + nStrLen, 0, TXT_DATA_SIZE - nStrLen);
				nLen = MAX_DATA_AREASIZE;
				break;
			}
				
			case DPT_Unknown:
			default:
				return -1;
				break;
		}
		
		return 0;
	}
	
	/******************************************************************
	 params:[in] pBuf
			[in] nBufSize
			[in/out] grpData
	 ******************************************************************/
	int CDPTManager::Byte2DPT(BYTE *pBuf, int nBufSize, KNX_GroupData &grpData)
	{
		switch (grpData.nGrpDPT)
		{
			case DPT_Boolean:
				grpData.Data.BinData = (*pBuf == 1) ? 1 : 0;
				break;
				
			case DPT_1BitCtrl:
			case DPT_3BitCtrl:
			case DPT_CharSet:
			case DPT_8BitUnSign:
				grpData.Data.AnaData = *pBuf;
				break;
				
			case DPT_8BitSign:
				grpData.Data.AnaData = (char) (*pBuf);
				break;
				
			case DPT_2OctUnSign:
				grpData.Data.AnaData = (*pBuf)<<8 | *(pBuf + 1);
				break;
				
			case DPT_2OctSign:
				grpData.Data.AnaData = (short) (*pBuf<<8 | *(pBuf + 1));
				break;
				
			case DPT_2OctFloat:
				/* 16-bit float
				 *  M EEEE MMM - MMMM MMMM
				 *  (0.01*M) * 2^E
				 */
				
				// byte[2] => 16-bit float
				// value > 0 or < 0 ?
				grpData.Data.AnaData = (*pBuf>>7 == 0) ? 
				( ((*pBuf<<8 | *(pBuf + 1)) & 0x07FF) * (1 << (*pBuf>>3 & 0x0F)) * 0.01 ) :
				( - ( (((*pBuf<<8 | *(pBuf + 1)) & 0x07FF)^0x7FF) + 1 )  * (1 << (*pBuf>>3 & 0x0F)) * 0.01 );
				break;
				
			case DPT_4OctUnSign:
				grpData.Data.AnaData = (UINT) (*pBuf<<24 | *(pBuf + 1)<<16 | *(pBuf + 2)<<8 | *(pBuf + 3));
				break;
				
			case DPT_4OctSign:
				grpData.Data.AnaData = (int) (*pBuf<<24 | *(pBuf + 1)<<16 | *(pBuf + 2)<<8 | *(pBuf + 3));
				break;
				
			case DPT_4OctFloat:
			{
				grpData.Data.AnaData = *((float *) pBuf);
				
				/*DWORD dwVal = * (DWORD*) array;
				//dwVal = ntohl(dwVal);
				var.dblVal = * ((float*) &dwVal);*/
			}
				break;
				
			case DPT_Time:
				grpData.Data.TimeData.year = 2000;
				grpData.Data.TimeData.month = 1;
				grpData.Data.TimeData.day = 1;
				grpData.Data.TimeData.weekday = (*pBuf>>5 & 0x03) <= 7 ? (*pBuf>>5 & 0x03) : 0;
				grpData.Data.TimeData.hour = (*pBuf & 0x1F) <= 23 ? (*pBuf & 0x1F) : 0;
				grpData.Data.TimeData.minute = (*(pBuf + 1) & 0x3F) <= 59 ? (*(pBuf + 1) & 0x3F) : 0;
				grpData.Data.TimeData.second = (*(pBuf + 2) & 0x3F) <= 59 ? (*(pBuf + 2) & 0x3F) : 0;
				break;
				
			case DPT_Date:
			{
				int year = *pBuf & 0x7F;
				if(year >= 90)
					year = 1900 + year;
				else
					year = 2089 - year;
				
				int month = *(pBuf + 1) & 0x0F;
				if(month < 1 || month > 12)
					month = 1;
				
				int day = *(pBuf + 2) & 0x1F;
				if(day < 1 || day > 31)
					day = 1;
				
				grpData.Data.TimeData.year = year;
				grpData.Data.TimeData.month = month;
				grpData.Data.TimeData.day = day;
				grpData.Data.TimeData.weekday = 0;
				grpData.Data.TimeData.hour = 0;
				grpData.Data.TimeData.minute = 0;
				grpData.Data.TimeData.second = 0;
			}
				break;
				
				
			case DPT_Access:
				grpData.Data.AnaData = (UINT) (*pBuf<<24 | *(pBuf + 1)<<16 | *(pBuf + 2)<<8 | *(pBuf + 3));
				break;
				
			case DPT_String:
			{
				memset(&grpData.Data.TxtData, 0, TXT_DATA_SIZE);
				int nCpyLen = TXT_DATA_SIZE < nBufSize ? TXT_DATA_SIZE : nBufSize;
				memcpy(&grpData.Data.TxtData, pBuf, nCpyLen);
				break;
			}
				
			case DPT_Unknown:
			default:
				break;
		}
		
		return 0;
	}
	
	int CDPTManager::GetDataAreaSize(KNX_GroupData grpData)
	{
		KNX_DATA_TYPE dataType = GetVarTypeFromDPT(grpData.nGrpDPT);
		int nResult = 0;
		switch (dataType)
		{
			case DATA_TYPE_BINARY:
				nResult = sizeof(grpData.Data.BinData);
				nResult = nResult < 1 ? 1 : nResult;
				break;
			case DATA_TYPE_ANALOG:
				nResult = sizeof(grpData.Data.AnaData);
				break;
			case DATA_TYPE_TEXT:
				nResult = sizeof(grpData.Data.TxtData);
				break;
			case DATA_TYPE_DATE:
			case DATA_TYPE_TIME:
				nResult = sizeof(grpData.Data.TimeData);
				break;
			default:
				nResult = 0;
				break;
		}
		return nResult;
	}
