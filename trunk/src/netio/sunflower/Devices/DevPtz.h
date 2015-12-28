/*
**	********************************************************************************
**                                     DevPtz
**                          Sunflower - The goal of a device
**
**   (c) Copyright 1992-2004, QiYang Technologies.
**                            All Rights Reserved
**
**	File		: DevPtz.h
**	Description	: 
**	Modify		: 2005/3/5		WuJunjie		Create the file
**	********************************************************************************
*/

#ifndef __DEV_PTZ_H__
#define __DEV_PTZ_H__

#include "../APIs/Ptz.h"
#include "../APIs/Comm.h"
#include "../MultiTask/Thread.h"
#include "../System/Object.h"
#include "../System/Signals.h"
#include "../Include/config-x.h"

typedef struct __ext_io_status_t 
{
	unsigned int ext_io_status_in;
	unsigned int ext_io_status_out;
}EXT_IO_STATUS_T;

class CDevPtz : public CThread
{	
public:
	CDevPtz();
	virtual ~CDevPtz();
	typedef TSignal2<void *, int>::SigProc SIG_DEV_PTZ_DATA;
	
	BOOL Start(CObject * pObj, SIG_DEV_PTZ_DATA pProc);
	BOOL Stop(CObject * pObj, SIG_DEV_PTZ_DATA pProc);
	unsigned int GetExtIO_In();
	unsigned int GetExtIO_Out();

	int Open(void);
	int SetAttribute(PTZ_ATTR *pAttr);
	int SetPTZHead(void *sbuf,int size);
	int Write(void* pData, DWORD len);
	int Read(void *pData, int len);
	int Close(void);
	int sio001_ext_io_opr(int chn, unsigned int on_off);
	unsigned int getExtIO_SIO001();
//#ifdef ALM_SUPPORT_1604C_ANNUNCIATOR
	int ReadAlm(void *pData, int len);
	int WriteAlm(void* pData, DWORD len);
//#endif
	void SetDebug(BOOL bNeedDebug);
	static CDevPtz* instance(void); 
private:
	void ThreadProc();
	int print_buf(unsigned char *buf, int buflen);

	unsigned int make_sum(unsigned char *buf, int buflen);
	int sio001_ext_io_create();
	int sio001_ext_io_close();
	int sio001_ext_io_status(unsigned int *in, unsigned int *out);
	
	EXT_IO_STATUS_T m_CurStatus;
	CMutex *m_pMutex485Cmd;
	CMutex m_Mutex485Create;


private:
	BYTE m_Buf[64];
	int m_Len;
	PTZ_ATTR	m_Attr;

	TSignal2<void *, int> m_sigData;

	CSemaphore		m_Semaphore;
#ifdef TRANSPARENCE_485
	CSemaphore		m_SemaphoreRWCtrl;
	volatile BOOL 	isReading;
#endif
	BOOL			m_bNeedBlocked;
	BOOL			m_debug;
	static CDevPtz* _instance; 
};

#endif// __DEV_PTZ_H__

