/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZThread.h - _explain_
 *
 * Copyright (C) 2011 joy.woo@hotmail.com, All Rights Reserved.
 *
 * $Id: EZThread.h 5884 2012-05-16 09:09:40Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _EZTHREAD_H_
#define _EZTHREAD_H_

#include <string>
#include "api_thread.h"
#include "EZObject.h"
#include "EZGuard.h"
#include "EZMsgQue.h"

typedef void (CEZObject:: * ASYNPROC)(PARAM wParam);

class CEZThread : public CEZObject
{
	friend void ThreadBody(void *pdat);
	friend class CThreadManager;

public:
	CEZThread(const char*pName, int nPriority, int nMsgQueSize = 0, unsigned int dwStackSize = 0);
	virtual ~CEZThread();
	BOOL CreateThread();
	BOOL DestroyThread(BOOL bWaited = FALSE);
	BOOL IsStoped();
	BOOL IsRunning();
	int	GetThreadID();
	BOOL SendMessage (unsigned int msg, PARAM wpa = 0, PARAM lpa = 0, unsigned int priority = 0);
	BOOL RecvMessage (MSG_NODE *pMsg, BOOL wait = TRUE);
	void QuitMessage ();
	void ClearMessage();
	unsigned int GetMessageCount();
	virtual void ThreadProc() = 0;
	int SetThreadName(const char*pName);
	int ShareSocket(int nSocket);
	void SetTimeout(unsigned int milliSeconds);
	BOOL IsTimeout();

protected:
	volatile BOOL	m_bLoop;
	volatile BOOL	m_bWaitThreadExit;
private:
	int		m_nPriority;
	unsigned int	m_dwStackSize;
	EZ_HANDLE	m_hThread;
	int		m_ID;
	char	m_Name[32];
	CEZMsgQue* m_pMsgQue;
	CEZThread* m_pPrev;		//上一个线程
	CEZThread* m_pNext;		//下一个线程
	CEZSemaphore m_cSemaphore;	// 该信号量用来防止同一个对象的线程同时被创建多次；
	unsigned int	m_expectedTime;	//预计执行结束时的时间，0表示不预计
	CEZSemaphore m_desSemaphore;
	
};

class Threadlet;

class PooledThread : public CEZThread
{
	friend class CThreadManager;
	friend class Threadlet;

public:
	PooledThread();
	~PooledThread();
	void ThreadProc();

private:
	CEZObject *m_obj;
	ASYNPROC m_asynFunc;
	PARAM m_param;
	CEZSemaphore m_semaphore;
	PooledThread* m_nextPooled;
	Threadlet* m_caller;
};

class Threadlet
{
	friend class PooledThread;

public:
	Threadlet();
	~Threadlet();
	BOOL run(PCSTR name, CEZObject * obj, ASYNPROC asynProc, PARAM param, unsigned int timeout);
	BOOL isRunning();

private:
	PooledThread* m_thread;
	static CEZMutex m_mutex;	///< 保证Threadlet和PooledThread的关联关系
};

class CThreadManager : public CEZObject
{
	friend class CEZThread;
	friend class PooledThread;
	friend class Threadlet;
	friend void ThreadBody(void *pdat);

public:
	PATTERN_SINGLETON_DECLARE(CThreadManager);
	CThreadManager();
	~CThreadManager();

	void RegisterMainThread(int id);
	BOOL HasThread(CEZThread *pThread);
	void DumpThreads();
	void DumpThreads(std::string &strThreadInfo);

private:
	BOOL AddThread(CEZThread *pThread);
	BOOL RemoveThread(CEZThread *pThread);
	PooledThread * GetPooledThread();
	void ReleasePooledThread(PooledThread* pThread);

private:
	CEZThread* m_pHead;
	PooledThread* m_headPooled;
	CEZMutex m_Mutex;
	int m_mainThreadID;
};

#define g_ThreadManager (*CThreadManager::instance())

#endif //_EZTHREAD_H_
