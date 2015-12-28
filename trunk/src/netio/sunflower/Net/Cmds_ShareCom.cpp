/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Cmds_ShareCom.cpp - _explain_
 *
 * Copyright (C) 2005 QiYang Technologies, All Rights Reserved.
 *
 * $Id: Cmds_ShareCom.cpp, v 1.0.0.1 2005-08-11 08:30:03 wjj Exp $
 *
 * 双工透明串口.
 * 
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifdef WIN32
  #pragma warning( disable : 4786)
#endif

#include "Cmds_ShareCom.h"

#include "NetWorkService.h"
#include "../Sunflower.h"
#include "../Configs/ConfigConversion.h"

#ifdef _USE_NET_MODULE
#include "../Functions/Comm_Transparence.h"
#endif

#include "../Devices/DevPtz.h"

#define BitGet(Number,pos) ((Number) >> (pos)&1) //用宏得到某数的某位
#define BitSet(Number,pos) ((Number) | 1<<(pos)) //把某位置1
#define BitClear(Number,pos) ((Number) & ~(1<<(pos))) //把某位置0
#define BitAnti(Number,pos) ((Number) ^ 1<<(pos)) //把Number的POS位取反

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _DEBUG_THIS
//#define _DEBUG_THIS
#endif

#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x) x
	#define DBG(x)
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

CmdsShareCom::CmdsShareCom(conn *pProc)
{
	m_OprConn = pProc;
    Initialize();
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

CmdsShareCom::~CmdsShareCom()
{

    DeInitialize();
}

int CmdsShareCom::Initialize()
{
	int ii;

	for (ii=0; ii<C_MAX_TCP_CONN_NUM_DEVIP; ii++)
	{
		m_bNeedTransComData[ii] = false;
	}

	m_bIsOnComData = false;

	m_bIsOnExtIO = false;
    return 0;
}
int CmdsShareCom::DeInitialize()
{
	/*
	 * 停止正在进行中的串口读取
	 */
	if(m_bIsOnComData) 
	{
		StopGetData(0);
		SystemSleep(10);
	}

	if (true == m_bIsOnExtIO)
	{
		StopExtIO(0);
		SystemSleep(10);
	}

    return 0;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

unsigned int CmdsShareCom::CmdCode()
{
	return REQ_COMM_SEND;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int CmdsShareCom::Do(CQueueNode *pMsg, CS32 iThrdIdx)
{
	DBG(
	    __fline;
	    trace("GetState(%d):%d\n", iThrdIdx, GetState(iThrdIdx));
	);
	m_pMsg = pMsg;
	m_pDVRIPHead = (DVRIP_HEAD_T *)m_pMsg->cabProcMsgBuf.Buf();

	switch(GetState(iThrdIdx))
	{
		case OpStSetCom:
		{
			return OnOpStSetCom(iThrdIdx);
		}
		case OpStSendData2Com:
		{
			return OnOpStSendData2Com(iThrdIdx);
		}
		case OpStSendBackComData:
		{
			return OnOpStSendBackComData(iThrdIdx);
		}
		case OpStBookExtIO:
		{
			return OnOpStBookExtIO(iThrdIdx);
		}
		case OpStQueryExtIO:
		{
			return OnOpStQueryExtIO(iThrdIdx);
		}
		case OpStSetExtIO:
		{
			return OnOpStSetExtIO(iThrdIdx);
		}
		default:
		{
			DBG(
			    __trip;
			);
			return COperation::Do(NULL, iThrdIdx);
		}
	} //switch

}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int CmdsShareCom::OnOpStEndDataSafe(CS32 iThrdIdx)
{
	DBG( __trip; );


	switch(m_pDVRIPHead->dvrip.dvrip_p[1])
	{
		case 1: // set com
		{
			SetState(OpStSetCom, iThrdIdx);
			return OpStSetCom;
		}
		case 2: // send data 2 com
		{
			SetState(OpStSendData2Com, iThrdIdx);
			return OpStSendData2Com;
		}
		case 3: // send back data
		{
			SetState(OpStSendBackComData, iThrdIdx);
			return OpStSendBackComData;
		}
		case 4: // send back data
		{
			SetState(OpStBookExtIO, iThrdIdx);
			return OpStBookExtIO;
		}
		case 5: // send back data
		{
			SetState(OpStQueryExtIO, iThrdIdx);
			return OpStQueryExtIO;
		}
		case 6: // send back data
		{
			SetState(OpStSetExtIO, iThrdIdx);
			return OpStSetExtIO;
		}
		default:
		{
DBG(
			__fline;
			trace("Wrong com opr(%d)\n", m_pDVRIPHead->dvrip.dvrip_p[1]);
);
		}
	}

	SetState(OpStEnd, iThrdIdx);
	return OpStEnd;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

// 设置参数
int CmdsShareCom::OnOpStSetCom(CS32 iThrdIdx)
{
	SetState(OpStEnd, iThrdIdx);

	if (!GetOperator(m_pMsg->iDlgNo)->isValidAuthority(DVRG_AUTHORITY_CONTROL))
	{
DBG(
		__fline;
		trace("!!! [%s] has not the auth for com.\n",
		      GetOperator(m_pMsg->iDlgNo)->GetUser().u_name.c_str());
);
		return OpStEnd;
	}

	// 串口和485选择，现在无法选择,故不用
	// m_pDVRIPHead->dvrip.dvrip_p[0]
	DBG(
	    __fline;
	    trace("OnOpStSetCom\n");
	);

#ifdef _USE_NET_MODULE
	
	CONFIG_COMM cc;
	CConfigConversion CCfgCVS;
	CCfgCVS.LoadOldComm(&cc, FALSE);

	//COMM_ATTR comAttr;
	//
	// bandrate
	//
	switch(m_pDVRIPHead->dvrip.dvrip_p[2])
	{
		case 1:
		{
			cc.BaudBase = 1200;
			break;
		}
		case 2:
		{
			cc.BaudBase = 2400;
			break;
		}
		case 3:
		{
			cc.BaudBase = 4800;
			break;
		}
		case 4:
		{
			cc.BaudBase = 9600;
			break;
		}
		case 5:
		{
			cc.BaudBase = 19200;
			break;
		}
		case 6:
		{
			cc.BaudBase = 38400;
			break;
		}
		case 7:
		{
			cc.BaudBase = 57600;
			break;
		}
		case 8:
		{
			cc.BaudBase = 115200;
			break;
		}
		default:
		{
			//
			// invalid baudrate
			//
			__trip;
			return -1;
		}
	}

	cc.DataBits	=	m_pDVRIPHead->dvrip.dvrip_p[3];
	
	switch (m_pDVRIPHead->dvrip.dvrip_p[4])
	{
		case 1:
		{
			cc.StopBits	=	COMM_ONESTOPBIT;
			break;
		}
		case 2:
		{
			cc.StopBits	=	COMM_ONE5STOPBITS;
			break;
		}
		case 3:
		{
			cc.StopBits	=	COMM_TWOSTOPBITS;
			break;
		}
		default:
		{
DBG(
			__fline;
			trace("invalid stopbits\n");
);
			return FAILURE_RET;
		}
	}

	switch (m_pDVRIPHead->dvrip.dvrip_p[5])
	{
		case 1:
		{
			cc.Parity	= COMM_ODDPARITY;
			break;
		}
		case 2:
		{
			cc.Parity	=	COMM_EVENPARITY;
			break;
		}
		case 3:
		{
			cc.Parity	=	COMM_NOPARITY;
			break;
		}
		default:
		{
DBG(
			__fline
			trace("invalid parity\n");
);
			return FAILURE_RET;
		}
	}

	if(m_pDVRIPHead->dvrip.dvrip_p[0] == 1)
	{
		cc.Function = COM_PTZ_MATRIX;

		PTZ_ATTR ptz_attr;
		ptz_attr.parity = cc.Parity;
		ptz_attr.stopbits = cc.StopBits;
		ptz_attr.baudrate = cc.BaudBase;
		ptz_attr.databits = cc.DataBits;
		ptz_attr.reserved = 0;

		CDevPtz::instance()->SetAttribute(&ptz_attr);
		trace("set 485\n");
	}
	else
	{
		cc.Function = TRANS_COM;
		trace("set comm\n");
	}

	if (CCfgCVS.SaveOldComm(&cc) == CONFIG_APPLY_REBOOT)
	{
		g_Sunflower.Reboot();
	}

#endif

	return OpStEnd;
}

int CmdsShareCom::OnOpStSendData2Com(CS32 iThrdIdx)
{
	SetState(OpStEnd, iThrdIdx);

	if (!GetOperator(m_pMsg->iDlgNo)->isValidAuthority(DVRG_AUTHORITY_CONTROL))
	{
DBG(
		__fline;
		trace("!!! [%s] has not the auth for com.\n",
		      GetOperator(m_pMsg->iDlgNo)->GetUser().u_name.c_str());
);

		return OpStEnd;
	}

	// 串口和485选择，现在无法选择,故不用
	// m_pDVRIPHead->dvrip.dvrip_p[0]
	DBG(
	    __fline;
	    trace("OnOpStSendData2Com\n");
	);

#ifdef _USE_NET_MODULE

	if (m_pDVRIPHead->dvrip.dvrip_extlen == 0)
	{
		return 0;
	}

	if(m_pDVRIPHead->dvrip.dvrip_p[0] == 1)
	{
		trace("send data to 485\n");
		CDevPtz::instance()->Write((U8 *)(m_pMsg->cabProcMsgBuf.Buf() + DVRIP_HEAD_T_SIZE),
	                                  m_pDVRIPHead->dvrip.dvrip_extlen);
	}
	else
	{
		trace("send data to comm\n");
		CDevComm::instance()->Write( (U8 *)(m_pMsg->cabProcMsgBuf.Buf() + DVRIP_HEAD_T_SIZE),
	                                  m_pDVRIPHead->dvrip.dvrip_extlen);
	}

#endif

	return OpStEnd;
}

int CmdsShareCom::OnOpStSendBackComData(CS32 iThrdIdx)
{
	SetState(OpStEnd, iThrdIdx);

	DBG(
	    __fline;
	    trace("OnOpStSendBackComData\n");
	);
	U8 kk;

	//
	// need  data
	//
	if (1 == m_pDVRIPHead->dvrip.dvrip_p[6])
	{
		if (!GetOperator(m_pMsg->iDlgNo)->isValidAuthority(DVRG_AUTHORITY_CONTROL))
		{
DBG(
			__fline;
			trace("!!! [%s] has not the auth for com.\n",
			      GetOperator(m_pMsg->iDlgNo)->GetUser().u_name.c_str());
);
			return OpStEnd;
		}

		if (false == m_bIsOnComData)
		{
			m_bIsOnComData = StartGetData(0);
			trace("StartGetData:%d\n", m_bIsOnComData);
		}
		else
		{
			DBG( trace("StartGetData has been started\n"); );
		}

		m_bNeedTransComData[m_pMsg->iDlgNo] = m_bIsOnComData;

		// 每次都启动
		m_bIsOnComData = false;
	}
	//
	// stop trans
	//
	else if (true == m_bIsOnComData)
	{
		m_bNeedTransComData[m_pMsg->iDlgNo] = false;

		//
		// if all channel do not need
		//
		for (kk=0; kk<C_MAX_TCP_CONN_NUM_DEVIP; kk++)
		{
			if (true == m_bNeedTransComData[kk])
			{
				break;
			}
		}

		//
		// all dlg do not need monitor
		//
		if (kk >= C_MAX_TCP_CONN_NUM_DEVIP)
		{
			m_bIsOnComData = !(this->StopGetData(0));
			DBG( trace("StopGetData:%d\n", m_bIsOnComData); );
		}
		else
		{
			DBG( trace("MM has been used, do not stop here.\n"); );
		}
	}
	else
	{
		DBG( trace("OnOpStSendBackComData do nothing\n"); );
	}

	return OpStEnd;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

bool CmdsShareCom::StartGetData(CS32 iChannel)
{
	DBG(
	    trace("CmdsShareCom::StartGetData\n");
	);

#ifdef _USE_NET_MODULE

	if(m_pDVRIPHead->dvrip.dvrip_p[0] == 1)
	{
		trace("485 start...\n");
		CDevPtz::instance()->Start(this, (DATAPROC)&CmdsShareCom::OnComData);
	}
	else
	{
		trace("comm start...\n");
		g_Comm.GetWorkingComm()->Start(this, (DATAPROC)&CmdsShareCom::OnComData);
	}
	

	return true;
	// End task 5716
#endif

	return false;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

bool CmdsShareCom::StopGetData(CS32 iChannel)
{
	DBG(
	    trace("CmdsShareCom::StopGetData\n");
	);

#ifdef _USE_NET_MODULE
	if(m_pDVRIPHead->dvrip.dvrip_p[0] == 1)
	{
		trace("485 close...\n");
		CDevPtz::instance()->Stop(this, (DATAPROC)&CmdsShareCom::OnComData);
	}
	else
	{
		trace("comm close...\n");
		return g_Comm.GetWorkingComm()->Stop() == 0;
	}
	
#endif

	return false;
}

void CmdsShareCom::OnComData(void *pData, int iDataLen)
{
	DBG(
	    trace("OnComData Len:%d\n", iDataLen);
		char *a;
		a=(char *)pData;
		trace("%c",*a);
	);

	DVRIP_HEAD_T dvripHead;
	CABuffer DataBuf;
	ZERO_DVRIP_HEAD_T(&dvripHead);

	dvripHead.dvrip.dvrip_cmd = REQ_COMM_SEND;
	dvripHead.dvrip.dvrip_extlen = iDataLen;
	dvripHead.dvrip.dvrip_p[1] = 3;
	DataBuf.Append((U8 *) &dvripHead, DVRIP_HEAD_T_SIZE);
	DataBuf.Append((U8 *)pData, iDataLen);
	
#if 1  // Peter Wang
	SendData(0,
	         0,
	         (U8 *)DataBuf.Buf(),
	         DataBuf.Size(),
	         Cmdlevel_Sys);
	    
#else
	for (U8 kk=0; kk<C_MAX_TCP_CONN_NUM_DEVIP; kk++)
	{
		if (true == m_bNeedTransComData[kk] )
		{
			SendData(kk,
			         kk,
			         (U8 *)DataBuf.Buf(),
			         DataBuf.Size(),
			         Cmdlevel_Sys);
		}
	}
#endif	
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

void CmdsShareCom::DeComData(void *pData, int iDataLen)
{
	DBG(
	    trace("DeComData Len:%d\n", iDataLen)
	);

	return;
}

int CmdsShareCom::OnOpStBookExtIO(CS32 iThrdIdx)
{
	SetState(OpStEnd, iThrdIdx);

	//
	// need  data
	//
	if (1 == m_pDVRIPHead->dvrip.dvrip_p[6])
	{
		if (false == m_bIsOnExtIO)
		{
			m_bIsOnExtIO = StartExtIO(0);
			trace("m_bIsOnExtIO:%d\n", m_bIsOnExtIO);
		}
		else
		{
			DBG( trace("StartGetData has been started\n"); );
		}

	}
	else
	{
		if (true == m_bIsOnExtIO)
		{
			m_bIsOnExtIO = !StopExtIO(0);
		}
	}

	return OpStEnd;
}

int CmdsShareCom::OnOpStSetExtIO(CS32 iThrdIdx)
{
	SetState(OpStEnd, iThrdIdx);

	if (!GetOperator(m_pMsg->iDlgNo)->isValidAuthority(DVRG_AUTHORITY_CONTROL))
	{
DBG(
		__fline;
		trace("!!! [%s] has not the auth for com.\n",
		      GetOperator(m_pMsg->iDlgNo)->GetUser().u_name.c_str());
);

		return OpStEnd;
	}

	// 串口和485选择，现在无法选择,故不用
	// m_pDVRIPHead->dvrip.dvrip_p[0]
	DBG(
	    __fline;
	    trace("OnOpStSetExtIO\n");
	);

#ifdef _USE_NET_MODULE

	if (m_pDVRIPHead->dvrip.dvrip_extlen == 0)
	{
		__trip;
	}
	else
	{

		std::string strData((char *)&m_pDVRIPHead->dvrip.dvrip_p[24], m_pDVRIPHead->dvrip.dvrip_extlen);

		__fline;
		trace("OnOpStSetExtIO:%s\n", strData.c_str());

		CParse parse;
		parse.setSpliter("&&");
		parse.setTrim(true);
		parse.Parse(strData);
		for (int ii=0; ii<parse.Size(); ii++)
		{
			CParse __ch;
			__ch.setSpliter("||");
			__ch.setTrim(true);
			
			trace("parse.getWord(%d):%s\n", ii, parse.getWord(ii).c_str());
			
			__ch.Parse(parse.getWord(ii));
			if (__ch.Size()>=2)
			{
				int chn = __ch.getValue(0);
				unsigned int on_off = __ch.getValue(1);
				trace("chn:%d, on_off:%x\n", chn, on_off);
				CDevPtz::instance()->sio001_ext_io_opr(chn, on_off);
			}
			else
			{
				__trip;
			}
		}
	}
#endif
	EXT_IO_STATUS_T __st;
	__st.ext_io_status_in = CDevPtz::instance()->GetExtIO_In();
	__st.ext_io_status_out = CDevPtz::instance()->GetExtIO_Out();

	OnExtIOData(&__st, sizeof(EXT_IO_STATUS_T));

	return OpStEnd;
}

int CmdsShareCom::OnOpStQueryExtIO(CS32 iThrdIdx)
{
	SetState(OpStEnd, iThrdIdx);

	if (!GetOperator(m_pMsg->iDlgNo)->isValidAuthority(DVRG_AUTHORITY_CONTROL))
	{
DBG(
		__fline;
		trace("!!! [%s] has not the auth for com.\n",
		      GetOperator(m_pMsg->iDlgNo)->GetUser().u_name.c_str());
);

		return OpStEnd;
	}
	
	EXT_IO_STATUS_T __st;
	__st.ext_io_status_in = CDevPtz::instance()->GetExtIO_In();
	__st.ext_io_status_out = CDevPtz::instance()->GetExtIO_Out();

//	__fline;
//	printf("in:%x, out:%x\n", __st.ext_io_status_in, __st.ext_io_status_out);


	OnExtIOData(&__st, sizeof(EXT_IO_STATUS_T));
	return OpStEnd;
}

bool CmdsShareCom::StartExtIO(CS32 iChannel)
{
	DBG(
	    trace("CmdsShareCom::StartGetData\n");
	);

#ifdef _USE_NET_MODULE

	trace("485 start...\n");
	return CDevPtz::instance()->Start(this, (DATAPROC)&CmdsShareCom::OnExtIOData)==TRUE;

	//return true;
	// End task 5716
#endif

	return false;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

bool CmdsShareCom::StopExtIO(CS32 iChannel)
{
	DBG(
	    trace("CmdsShareCom::StopExtIO\n");
	);

#ifdef _USE_NET_MODULE
	trace("485 StopExtIO...\n");
	CDevPtz::instance()->Stop(this, (DATAPROC)&CmdsShareCom::OnExtIOData);
	return true;
#endif

	return false;
}

void CmdsShareCom::OnExtIOData(void *pData, int iDataLen)
{
	DBG(
	    trace("OnComData Len:%d\n", iDataLen);
		char *a;
		a=(char *)pData;
		trace("%c",*a);
	);

	EXT_IO_STATUS_T *__st = (EXT_IO_STATUS_T *)pData;

//	__fline; 
//	printf("in:%x, out:%x\n", __st->ext_io_status_in, __st->ext_io_status_out);
	//unsigned char in = __st->ext_io_status_in;
	//unsigned char out = __st->ext_io_status_out;
	//char buffer[128];
//	sprintf(buffer, "%d||%d&&%d||%d&&%d||%d&&%d||%d&&");
	//for (int ii=0; ii<);

	DVRIP_HEAD_T dvripHead;
	CABuffer DataBuf;
	ZERO_DVRIP_HEAD_T(&dvripHead);

	dvripHead.dvrip.dvrip_cmd = REQ_COMM_SEND;
	dvripHead.dvrip.dvrip_extlen = iDataLen;
	dvripHead.dvrip.dvrip_p[0] = 1; // extio
	dvripHead.dvrip.dvrip_p[1] = 4; // book data
	DataBuf.Append((U8 *) &dvripHead, DVRIP_HEAD_T_SIZE);
	DataBuf.Append((U8 *)__st, iDataLen);
	
#if 1  // Peter Wang
	SendData(0,
	         0,
	         (U8 *)DataBuf.Buf(),
	         DataBuf.Size(),
	         Cmdlevel_Sys);
	    
#else
	for (U8 kk=0; kk<C_MAX_TCP_CONN_NUM_DEVIP; kk++)
	{
		if (true == m_bNeedTransComData[kk] )
		{
			SendData(kk,
			         kk,
			         (U8 *)DataBuf.Buf(),
			         DataBuf.Size(),
			         Cmdlevel_Sys);
		}
	}
#endif	
}


