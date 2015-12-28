/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZGuard.h - _explain_
 *
 * Copyright (C) 2011 joy.woo@hotmail.com, All Rights Reserved.
 *
 * $Id: EZGuard.h 5884 2012-05-16 09:14:55Z WuJunjie $
 *
 *  Explain:
 *     - ÿª§¿‡-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __EZGUARD_H__
#define __EZGUARD_H__

#include "EZMutex.h"

class CEZMutex;
class CEZGuard
{
public:
	inline CEZGuard(CEZMutex& Mutex)
		:m_Mutex(Mutex)
	{
		m_Mutex.Enter();
	};
	
	inline ~CEZGuard()
	{
		m_Mutex.Leave();
	};
protected:
private:
	CEZMutex &m_Mutex;
};

#endif //__EZGUARD_H__
