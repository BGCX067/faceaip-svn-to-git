/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZTimer.h - _explain_
 *
 * Copyright (C) 2011 fesens.com, All Rights Reserved.
 *
 * $Id: EZTimer.h 5884 2012-05-16 09:08:50Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _EZTIMER_H_
#define _EZTIMER_H_

#include "api_system.h"
#include "EZObject.h"
#include "EZThread.h"
#include "EZMutex.h"
#include "EZGuard.h"

typedef void (CEZObject:: * TIMERPROC)(PARAM wParam);
#define TIMER_NAME_LEN 32

class TimerValue
{
public:
    unsigned int lo;
    unsigned int hi;
    void operator+= (unsigned int a);
};
int compare(TimerValue& a, TimerValue& b);
BOOL operator> (TimerValue a, TimerValue b);

class CEZTimerManager;
class CEZTimer : public CEZObject
{
    friend class CEZTimerManager;
public:
    CEZTimer(PCSTR pName = "Nonamed");
    virtual ~CEZTimer();
    //dwDueTime:延时时间；dwPriod：间隔时间（震荡频率）
    void Start(CEZObject * pObj
               , TIMERPROC pTimerFun
               , unsigned int dwDueTime
               , unsigned int dwPriod
               , PARAM param = 0
               , unsigned int timeout = 0);
    void Stop(BOOL bCallNow = FALSE);
    void run();
    PCSTR GetName();
    void SetName(PCSTR);
    BOOL IsStarted();
    BOOL IsCalled();

private:
    TimerValue m_CallTime;
    unsigned int m_Priod;
    unsigned int m_timeout;
    CEZObject *m_pObj;
    TIMERPROC m_pTimerFun;
    PARAM m_param;
    char  m_Name[TIMER_NAME_LEN+1];
    BOOL  m_Started;
    BOOL  m_Called;
    static CEZMutex m_Mutex;
    CEZTimer* m_pPrev;		//上一个定时器
    CEZTimer* m_pNext;		//下一个定时器
    Threadlet m_threadlet;
};

class CEZTimerManager : public CEZThread
{
    friend class CEZTimer;
public:
    PATTERN_SINGLETON_DECLARE(CEZTimerManager);
    CEZTimerManager();
    ~CEZTimerManager();
    void Start();
    BOOL AddTimer(CEZTimer * pTimer);
    BOOL RemoveTimer(CEZTimer * pTimer);
    void DumpTimers();
    void DumpTimers(std::string &strInfo);
    void ThreadProc();

protected:
private:
    TimerValue m_CurTime;	//毫秒计数
    CEZTimer* m_pHead;
};

#define g_TimerManager (*CEZTimerManager::instance())

#endif
