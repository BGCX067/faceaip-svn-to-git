/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZMutex.h - _explain_
 *
 * Copyright (C) 2011 joy.woo@hotmail.com, All Rights Reserved.
 *
 * $Id: EZMutex.h 5884 2012-05-16 09:14:43Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __EZMUTEX_H__
#define __EZMUTEX_H__

#include <assert.h>
#include "api_mutex.h"

class CEZMutex
{
public:
	inline CEZMutex(int nType = MUTEX_FAST)
	{
		m_hMutex = 0;
		MutexCreate(&m_hMutex, nType) ;
		assert(m_hMutex);
	};
	inline ~CEZMutex()
	{
		MutexDestory(m_hMutex);
	};
	inline BOOL Enter()
	{
		assert(m_hMutex);
	
		return MutexEnter(m_hMutex) == 0 ? TRUE : FALSE;
	};
	inline BOOL Leave()
	{
		assert(m_hMutex);

		return MutexLeave(m_hMutex) == 0 ? TRUE : FALSE; 
	};
protected:
private:
	EZ_HANDLE m_hMutex;
};

#endif //__EZMUTEX_H__
