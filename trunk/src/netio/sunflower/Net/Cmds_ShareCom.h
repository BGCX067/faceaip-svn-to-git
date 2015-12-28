/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Cmds_ShareCom.h - _explain_
 *
 * Copyright (C) 2005 QiYang Technologies, All Rights Reserved.
 *
 * $Id: Cmds_ShareCom.h, v 1.0.0.1 2005-08-11 08:30:10 wjj Exp $
 *
 * 双工透明串口.
 * 
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _CMDS_SHARECOM_H
#define _CMDS_SHARECOM_H

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "Operation.h"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

class CNetWorkService;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#define TOTAL_COM_NUM 1

class CmdsShareCom : public COperation
{
public:
	CmdsShareCom(conn *pProc);
	virtual ~CmdsShareCom();

	int Do(CQueueNode *pMsg, CS32 iThrdIdx);
	unsigned int  CmdCode();
    
    virtual int Initialize();
    virtual int DeInitialize();

	void OnComData(void *pData, int iDataLen);
	void DeComData(void *pData, int iDataLen);

private:
	int OnOpStEndDataSafe(CS32 iThrdIdx);

	int OnOpStSetCom(CS32 iThrdIdx);
	int OnOpStSendData2Com(CS32 iThrdIdx);

	int OnOpStSendBackComData(CS32 iThrdIdx);
	bool StartGetData(CS32 iChannel);
	bool StopGetData(CS32 iChannel);

	int OnOpStBookExtIO(CS32 iThrdIdx);
	int OnOpStQueryExtIO(CS32 iThrdIdx);
	int OnOpStSetExtIO(CS32 iThrdIdx);
	bool StartExtIO(CS32 iChannel);
	bool StopExtIO(CS32 iChannel);
	void OnExtIOData(void *pData, int iDataLen);

	// 某连接是否需要某串口数据
	bool m_bNeedTransComData[C_MAX_TCP_CONN_NUM_DEVIP];
	// 是否已经OnComData
	bool m_bIsOnComData;

	bool m_bIsOnExtIO;
};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#endif // _CMDS_SHARECOM_H
