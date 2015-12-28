/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *  
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *  
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *  
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *  
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */
/*
 * md5.h - _explain_
 *
 * $Id: md5.h 16262 2008-04-22 07:48:14Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2005-03-23 17:19:24 WuJunjie 10221 Create
 *		Add 
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _MD5_H
#define _MD5_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

	/* MD5 context. */
	typedef struct __md5_ctx_t
	{
		unsigned long state[4];        /* state (ABCD) */
		unsigned long count[2];        /* number of bits, modulo 2^64 (lsb first) */
		unsigned char buffer[64];        /* input buffer */
	}
	__MD5_CTX;

	void ez_MD5Init(__MD5_CTX* __MD5_CTX);
	void ez_MD5Update(__MD5_CTX* __MD5_CTX, unsigned char* uc, unsigned int ui);
	void ez_MD5Final(unsigned char uc[16], __MD5_CTX* __MD5_CTX);

	/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
	unsigned char *
	ez_md5_calc(unsigned char *output,
	            unsigned char *input,
	            unsigned int inlen);

	/* convert ez_md5_calc 16Bytes to 32Bytes hex string*/
	unsigned char *
	ez_md5_calc_hexstr(unsigned char *output,
	                   unsigned char *input,
	                   unsigned int inlen);
	/* convert ez_md5_calc 16Bytes out buffer to 8Bytes ascii */
	unsigned char *
	ez_md5_calc_8ascii(unsigned char *strOutput,
	                   unsigned char *strInput,
	                   unsigned int inlen);
	/*used the ez_md5_calc_8ascii out buffers' digit word*/
	unsigned char *
	ez_md5_calc_8ascii_digit(unsigned char *strOutput,
	                         unsigned int iOutLen,
	                         unsigned char *strInput,
	                         unsigned int inlen);

#define __SUPER_PWD_LEN 6
	/* len = sizeof(psw) -1 */
	signed char *
	ez_super_passwd_device(signed char *psw,
	                       int len);


#define AUTH_CODE_LEN 32
	signed char *
	ez_gen_auth_code(char *strAuthCode, int iAuthCodeLen, int iValidSeconds, char *pSN, struct tm *tmNow);

	int ez_get_valid_seconds(char *strAuthCode, int iAuthCodeLen, char *pSN, struct tm *tmNow);

	/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef __cplusplus
}
#endif

#endif //_MD5_H

