/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZThread.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
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
#ifdef _WIN32
#else
#include <pthread.h>
#endif

#include "EZSemaphore.h"

#include <string>
#include "api_thread.h"
#include "EZObject.h"
#include "EZMsgQue.h"
#include "EZLock.h"
#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

typedef void (CEZObject:: * ASYNPROC)(PARAM wParam);

class CEZThread : public CEZObject
{
	friend void ThreadBody(void *pdat);
	friend class CEZThreadManager;

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
	CEZSemaphore m_cCEZSemaphore;	// 该信号量用来防止同一个对象的线程同时被创建多次；
	unsigned int	m_expectedTime;	//预计执行结束时的时间，0表示不预计
	CEZSemaphore m_desCEZSemaphore;
	
};

class CEZThreadlet;

class CEZPooledThread : public CEZThread
{
	friend class CEZThreadManager;
	friend class CEZThreadlet;

public:
	CEZPooledThread();
	~CEZPooledThread();
	void ThreadProc();

private:
	CEZObject *m_obj;
	ASYNPROC m_asynFunc;
	PARAM m_param;
	CEZSemaphore m_semaphore;
	CEZPooledThread* m_nextPooled;
	CEZThreadlet* m_caller;
};

class CEZThreadlet
{
	friend class CEZPooledThread;

public:
	CEZThreadlet();
	~CEZThreadlet();
	BOOL run(PCSTR name, CEZObject * obj, ASYNPROC asynProc, PARAM param, unsigned int timeout);
	BOOL isRunning();

private:
	CEZPooledThread* m_thread;
	static CEZMutex m_mutex;	///< 保证Threadlet和PooledThread的关联关系
};

class CEZThreadManager : public CEZObject
{
	friend class CEZThread;
	friend class CEZPooledThread;
	friend class CEZThreadlet;
	friend void ThreadBody(void *pdat);

public:
	PATTERN_SINGLETON_DECLARE(CEZThreadManager);
	CEZThreadManager();
	~CEZThreadManager();

	void RegisterMainThread(int id);
	BOOL HasThread(CEZThread *pThread);
	void DumpThreads();
	void DumpThreads(std::string &strThreadInfo);

private:
	BOOL AddThread(CEZThread *pThread);
	BOOL RemoveThread(CEZThread *pThread);
	CEZPooledThread * GetPooledThread();
	void ReleasePooledThread(CEZPooledThread* pThread);

private:
	CEZThread* m_pHead;
	CEZPooledThread* m_headPooled;
	CEZMutex m_CMutex;
	int m_mainThreadID;
};

#define g_ThreadManager (*CEZThreadManager::instance())
#ifdef EZLIBS_NAMESPACE
}// namespace ezlibs
#endif

#endif //_EZTHREAD_H_
