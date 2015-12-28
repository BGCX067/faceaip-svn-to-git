/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZSemaphore.h - _explain_
 *
 * Copyright (C) 2011 joy.woo@hotmail.com, All Rights Reserved.
 *
 * $Id: EZSemaphore.h 5884 2012-05-16 09:10:06Z WuJunjie $
 *
 *  Explain:
 *     -–≈∫≈¡ø-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __EZSEMAPHORE_H__
#define __EZSEMAPHORE_H__

#include <assert.h>
#include "api_semaphore.h"

class CEZSemaphore
{
public:
	inline CEZSemaphore(unsigned int dwInitialCount = 0)
		:m_hSemphore(0)
	{
		SemaphoreCreate(&m_hSemphore, dwInitialCount);
		assert(m_hSemphore);
	};
	inline ~CEZSemaphore()
	{
		assert(m_hSemphore);
		SemaphoreDestory(m_hSemphore);
	};
	inline int Pend()
	{
		assert(m_hSemphore);
		return SemaphorePend(m_hSemphore);
	};
	inline int Post()
	{
		assert(m_hSemphore);
		return SemaphorePost(m_hSemphore);
	};
protected:
private:
	EZ_HANDLE m_hSemphore;
};


#endif //__SEMAPHORE_H__
//
// End of "$Id: Semaphore.h 6920 2007-01-26 02:54:59Z ren_xusheng $"
//
