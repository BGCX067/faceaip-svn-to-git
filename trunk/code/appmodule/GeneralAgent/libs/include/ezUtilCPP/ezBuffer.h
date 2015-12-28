/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ezBuffer.h - _explain_
 *
 * Copyright (C) 2005 QiYang Technologies, All Rights Reserved.
 *
 * $Id: ABuffer.h 5884 2006-12-11 05:56:42Z wu_junjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2005-09-221 17:19:24   Create
 *		wjj 2005-04-02 15:16:48
 *			Create
 *
 *		wjj 2005-04-21 18:16:28
 *			use malloc/free manage memory
 *
 *		wjj 2005-05-15 14:12:25
 *			add pre alloc mem opr in Append(NULL, size)
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _ezBUFFER_H
#define _ezBUFFER_H

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

class CezBuffer
{
public:
	CezBuffer();
	virtual ~CezBuffer();
	/**
	 * Append - 
	 * @ pszStr : 
	 *		pointer of data.
	 * @ stSize : 
	 *		data size.
	 * Return : 
	 *		< 0 : error
	 *		>=0 data size
	 *
	 * append data to buffer
	 */
	int Append(unsigned char * pszStr, const unsigned int stSize);
	/**
	 * Get - 
	 * @ pszStr : 
	 *		pointer of buf.
	 * @ stSize : 
	 *		data size.
	 * Return : 
	 *		< 0 : error
	 *		0 : Success
	 *
	 * Get data from buffer
	 */
	int Get(unsigned char * pszStr, const unsigned int stSize);
	/**
	 * Pop - 
	 * @ pszStr : 
	 *		pointer of buf.
	 * @ stSize : 
	 *		data size.
	 * Return : 
	 *		< 0 : error
	 *		0 : Success
	 *
	 * Pop data from buffer
	 */
	int Pop(unsigned char * pszStr, const unsigned int stSize);
	/**
	 * Pour - 
	 * @ pszStr : 
	 *		pointer of buf.
	 * @ stSize : 
	 *		data size.
	 * Return : 
	 *		< 0 : error
	 *		0 : Success
	 *
	 * Pour data from buffer
	 */
	int Pour(unsigned char * pszStr, const unsigned int stSize);
	/**
	 * Reset - 
	 * @ isClear : 
	 *		true : free buff
	 *		false : clear data only
	 *
	 * Reset buf
	 */
	void Reset(const bool isClear = false);
	/**
	 * Size - 
	 *
	 * Return : 
	 *		size of data
	 *
	 * get size of data
	 */
	unsigned int Size();
	/**
	 * Buf - 
	 * Return : 
	 *		pointer of data
	 *
	 * get the pointer of data
	 */
	unsigned const char * Buf() const;
	/**
	 * Reset - 
	 * @ IncreaseSize : 
	 *		alloc mem step when buf full
	 *
	 * set Increase step
	 */
	void SetIncreaseSize(const unsigned int IncreaseSize);

	/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

private:
	/**
	 * Initialize - 
	 *
	 * Initialize
	 */
	void Initialize();
	/* buffer pointer */
	unsigned char * m_pBuf;
	/* data size */
	unsigned int  m_iDataSize;
	/* current buffer size */
	unsigned int m_iBufSize;
	/* 
	 * the step to alloc mem when need 
	 * the default value is 32 Byte
	 */
	unsigned int m_iIncreaseSize;
	/* 
	 * Do not use now
	 * you shoule judge the buff if too max yourself
	 */
	unsigned int m_iMaxSize;
};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#endif //_ezBUFFER_H
