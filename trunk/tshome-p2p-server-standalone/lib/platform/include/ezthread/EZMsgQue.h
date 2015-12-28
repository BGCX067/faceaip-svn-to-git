/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZMsgQue.h - _explain_
 *
 * Copyright (C) 2011 joy.woo@hotmail.com, All Rights Reserved.
 *
 * $Id: EZMsgQue.h 5884 2012-05-16 09:14:47Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _EZMSGQUE_H_
#define _EZMSGQUE_H_

#if defined(WIN32)
	#pragma warning (disable : 4786)
#endif
#include <list>
#include "api_system.h"
#include "EZMutex.h"
#include "EZSemaphore.h"
#include "EZPoolAllocator.h"

#define MSG_PRIORITY	10

typedef struct tagMSG_NODE
{
    unsigned int		msg;
    PARAM		wpa;
    PARAM		lpa;
    unsigned int		time;
}
MSG_NODE;

typedef std::list<MSG_NODE, pool_allocator<MSG_NODE> > MSGQUEUE;

class CEZMsgQue
{
public:
    CEZMsgQue(int size = 1024);
    virtual ~CEZMsgQue();

    BOOL SendMessage (unsigned int msg, PARAM wpa = 0, PARAM lpa = 0, unsigned int priority = 0);
    BOOL RecvMessage (MSG_NODE *pMsg, BOOL wait = TRUE);
    void QuitMessage ();
    void ClearMessage();
    int GetMessageCount();
    int GetMessageSize();
    void SetMessageSize(int size);

protected:
private:
    MSGQUEUE m_Queue;
    BOOL m_bMsgFlg;
    CEZMutex m_Mutex;
    CEZSemaphore m_Semaphore;
    int m_nMsg;
    int m_nMaxMsg;
};

#endif// _EZMSGQUE_H_
