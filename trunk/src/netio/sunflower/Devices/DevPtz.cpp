/*
**	********************************************************************************
**                                     DevPtz
**                          Sunflower - The goal of a device
**
**   (c) Copyright 1992-2004, QiYang Technologies.
**                            All Rights Reserved
**
**	File		: DevPtz.cpp
**	Description	: 
**	Modify		: 2005/3/5		WuJunjie		Create the file
**	********************************************************************************
*/

#include "../APIs/System.h"
#include "DevPtz.h"

//#define ALM_SUPPORT_1604C_ANNUNCIATOR
#define DAELE_EXT_BOARD_CMD_LEN 8

//#ifdef ALM_SUPPORT_1604C_ANNUNCIATOR

//以下变量为应用层引用的全局变量，这些全局变量分别对应不同的串口功能扩展

//云台和报警解码器同时使用时，用来控制云台
//extern UART_EXTEND PtzControl;
//云台和报警解码器同时使用时，用来控制报警解码器
extern UART_EXTEND AlarmControl;
UART_EXTEND *pUartExtPtzAlm = &AlarmControl;

//控制红苹果矩阵
//extern UART_EXTEND RedAppleMatrix;

//控制温度湿度报警器
//extern UART_EXTEND WenShiDu;
//#endif

CDevPtz* CDevPtz::_instance = NULL;

CDevPtz* CDevPtz::instance(void)
{
	if(NULL == _instance)
	{
		_instance = new CDevPtz();
	}
	return _instance;
}

void CDevPtz::ThreadProc()
{
	unsigned int in, out;
	int ret;
	ret = sio001_ext_io_create();
	
	ret = sio001_ext_io_status(&m_CurStatus.ext_io_status_in, &m_CurStatus.ext_io_status_out);

	while (m_bLoop) 
	{
		ret = sio001_ext_io_status(&in, &out);
		if (ret==0)
		{
			if (m_CurStatus.ext_io_status_in!=in || m_CurStatus.ext_io_status_out!=out)
			{
				printf("in:%x, out:%x\n", in, out);

				m_CurStatus.ext_io_status_in = in;
				m_CurStatus.ext_io_status_out = out;
				m_sigData(&m_CurStatus, sizeof(EXT_IO_STATUS_T));
			}
			else
			{}
		}

		//暂时无用
		//tracepoint();
		SystemSleep(500);
	}
}

CDevPtz::CDevPtz(): CThread("DevPtz", TP_PTZ), m_sigData(32)
{
	//PtzCreate();
	memset((void*)m_Buf, 0, 64);
	memset((void *)&m_Attr, 0, sizeof(PTZ_ATTR));
	m_Len = 0;
	m_debug = FALSE;
	//CommCreate();
	//Open();
	CreateThread();
#ifdef TRANSPARENCE_485
	isReading = FALSE;
#endif

	m_CurStatus.ext_io_status_in = 0;
	m_CurStatus.ext_io_status_out = 0;

	m_pMutex485Cmd = new CMutex;
//	CMutex m_pMutex485Cmd;
}

CDevPtz::~CDevPtz()
{
	//DestroyThread();
	//Close();
	//PtzDestory();
}

unsigned int CDevPtz::GetExtIO_In()
{
	printf("in:%x, out:%x\n", m_CurStatus.ext_io_status_in, m_CurStatus.ext_io_status_out);
	return m_CurStatus.ext_io_status_in;
}

unsigned int CDevPtz::GetExtIO_Out()
{
	printf("in:%x, out:%x\n", m_CurStatus.ext_io_status_in, m_CurStatus.ext_io_status_out);
	return m_CurStatus.ext_io_status_out;
}

BOOL CDevPtz::Start(CObject * pObj, SIG_DEV_PTZ_DATA pProc)
{
	if(m_sigData.Attach(pObj, pProc) < 0)
	{
		trace("attach error\n");
		return FALSE;
	}
	m_bNeedBlocked = FALSE;
	m_Semaphore.Post();

	return TRUE;
}

BOOL CDevPtz::Stop(CObject * pObj, SIG_DEV_PTZ_DATA pProc)
{
	if (m_sigData.Detach(pObj, pProc) == 0)
	{
		m_bNeedBlocked = TRUE;
	}

	return TRUE;
}

int CDevPtz::Open(void)
{
	return 0;
	return !PtzOpen();
}

int CDevPtz::SetAttribute(PTZ_ATTR *pattr)
{
	return 0;
	if (memcmp((void *)&m_Attr, pattr, sizeof(PTZ_ATTR)) == 0)
	{
		return TRUE;
	}
	
	if (!Close())
	{
		return FALSE;
	}

	if (!Open())
	{
		return FALSE;
	}
	if (PtzSetAttribute(pattr))
	{
		return FALSE;
	}	

	memcpy((void *)&m_Attr, pattr, sizeof(PTZ_ATTR));	
	
	return TRUE;
}

int CDevPtz::SetPTZHead(void *sbuf,int length)
{
	if(!sbuf)
	{
		return FALSE;
	}
	memcpy((void *)m_Buf,sbuf,length);
	m_Len = length;
	return TRUE;
}

int CDevPtz::Write(void* pData, DWORD len)
{
	return 0;
	int result;

	if(!pData)
	{
		return FALSE;
	}
#ifndef TRANSPARENCE_485
	memcpy((void *)&m_Buf[m_Len],pData,(int)len);
	if (m_debug)
	{
		trace("Run<%d> : ",(int)len+m_Len);
		for(int i = 0; i < (int)len+m_Len; i++)
		{
			trace("0x%02x, ",m_Buf[i]);
		}
		trace("\n");
	}

	if ((result = PtzWrite((void *)m_Buf,(int)len+m_Len)) > 0)
	{
		return result;	
	}
	else 
	{
		trace("*************** Ptz Write fail*********\n");
	}
#else
	if(isReading == TRUE)
	{
		trace("485 is busy...\n");
		return FALSE;
	}

	if ((result = PtzWrite((void *)pData,(int)len)) > 0)
	{
		m_SemaphoreRWCtrl.Post();
		return result;	
	}
	else 
	{
		trace("*************** Ptz Write fail*********\n");
	}
#endif

	return FALSE;
}

void CDevPtz::SetDebug(BOOL bNeedDebug)
{
	m_debug = bNeedDebug;
}
int CDevPtz::Read(void *pData, int len)
{
	return 0;
	return PtzRead(pData, len);
}

int CDevPtz::Close(void)
{
	return 0;
	return !PtzClose();
}

#ifdef ALM_SUPPORT_1604C_ANNUNCIATOR
int CDevPtz::WriteAlm(void* pData, DWORD len)
{
	return 0;
	int result;

	if(!pData)
	{
		return FALSE;
	}

	if ((result = pUartExtPtzAlm->Write(pData, len) > 0))
	{
		return result;	
	}
	
	else 
	{
		trace("*************** Ptz Write fail*********\n");
	}

	return FALSE;
}
int CDevPtz::ReadAlm(void *pData, int len)
{
	return 0;
	return pUartExtPtzAlm->Read(pData, len);
}
#endif//ALM_SUPPORT_1604C_ANNUNCIATOR

int CDevPtz::print_buf(unsigned char *buf, int buflen)
{
	int ii;

	printf(":[");

	for (ii=0; ii<8; ii++)
	{
		printf("%0x ", buf[ii]);
	}
	printf("]\n");

	return 0;
}
unsigned int CDevPtz::make_sum(unsigned char *buf, int buflen)
{
	unsigned int sum = 0;
	int ii;
	for (ii=0; ii<buflen; ii++)
	{
		sum += buf[ii];
	}

	return sum;
}

int CDevPtz::sio001_ext_io_create()
{
	CGuard guard(m_Mutex485Create);
	int ret;
	
	ret = PtzCreate();
	if (ret !=0)
	{
		return ret;
	}
	ret = PtzOpen();
	if (ret !=0)
	{
		return ret;
	}

	PTZ_ATTR pattr;
	pattr.baudrate = 9600;
	pattr.databits = 8;
	pattr.parity = COMM_NOPARITY;
	pattr.stopbits = COMM_ONESTOPBIT;

	ret = PtzSetAttribute(&pattr);

	return ret;
}

int CDevPtz::sio001_ext_io_close()
{
	CGuard guard(m_Mutex485Create);
	int ret;

	ret = PtzClose();
	ret = PtzDestory();
	
	return ret;
}

int CDevPtz::sio001_ext_io_status(unsigned int *in, unsigned int *out)
{
	CGuard guard(*m_pMutex485Cmd);
	int ret;
	// 查询指令
	unsigned char cmd[DAELE_EXT_BOARD_CMD_LEN] = {0x55, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x66};

	ret = pUartExtPtzAlm->Write(cmd, DAELE_EXT_BOARD_CMD_LEN);
	if (ret < 0)
	{
		printf("PtzWrite faield\n");
	}
	else
	{
		//ez_sleep(0, 30);
		ret = pUartExtPtzAlm->Read(cmd, DAELE_EXT_BOARD_CMD_LEN);
		if (ret < DAELE_EXT_BOARD_CMD_LEN)
		{
			printf("cmd read error(%d).\n", ret);
		}
		else
		{
			printf("Cmd");
			print_buf(cmd, 8);
			if (cmd[7] == make_sum(cmd, 7))
			{
				*in = cmd[4]&0xf;
				*out = cmd[6]&0xf;
				return 0;
			}
			else
			{
				printf("check sum error.\n");
				
				sio001_ext_io_close();
				sio001_ext_io_create();
			}
		} // read
	}
	return -1;
}

//chn传-1 操作所有通道， 0-n， 则操作具体通道
int CDevPtz::sio001_ext_io_opr(int chn, unsigned int on_off)
{
	CGuard guard(*m_pMutex485Cmd);

	int ret;
	unsigned char cmd[8] = {0x55, 0x01, 0x12, 0x00, 0x00, 0x00, 0x01, 0x69};

	if (chn == -1)
	{
		//buf[1] = boardno;
		cmd[2] = 0x13; // cmd use data
		cmd[6] = on_off; //0x0f; // 4路的此位开始表示数据
	}
	else
	{
		//55 01 12 00 00 00 01 69
		// opr
		cmd[2] = 0x11+on_off;
		// cmd
		cmd[6] = chn+1;
	}
		//sum
		cmd[7] = make_sum(cmd, 7);

	ret = pUartExtPtzAlm->Write(cmd, sizeof(cmd));

	if (ret < 0)
	{
		printf("PtzWrite faield\n");
	}
	else
	{
		// 读取返回值
		ret = pUartExtPtzAlm->Read(cmd, DAELE_EXT_BOARD_CMD_LEN);
		if (ret < DAELE_EXT_BOARD_CMD_LEN)
		{
			printf("cmd read error(%d).\n", ret);
		}
		else
		{
			printf("cmd");
			print_buf(cmd, 8);
			if (cmd[7] == make_sum(cmd, 7))
			{
				m_CurStatus.ext_io_status_in = cmd[4]&0xf;
				m_CurStatus.ext_io_status_out = cmd[6]&0xf;
				
				return 0;
			}
			else
			{
				printf("check sum error.\n");
				
				sio001_ext_io_close();
				sio001_ext_io_create();
			}
		} // read
	}

	return -1;
}

unsigned int CDevPtz::getExtIO_SIO001() 
{	
	return m_CurStatus.ext_io_status_in;
}

