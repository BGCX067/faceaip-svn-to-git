/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * typedef_ezthread.h - _explain_
 *
 * Copyright (C) 2011 joy.woo@hotmail.com, All Rights Reserved.
 *
 * $Id: typedef_ezthread.h 5884 2012-05-16 09:16:43Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __TYPEDEF_EZTHREAD_H__
#define __TYPEDEF_EZTHREAD_H__

#include <stdio.h>

typedef unsigned int			PARAM;
typedef void *					EZ_HANDLE;
typedef int						BOOL;

#define TRUE					1
#define FALSE					0

#define trace printf
#define tracepoint() 			do {trace("tracepoint: %s,%d\n",__FILE__,__LINE__); } while(0)

enum xm_msg_t {
	XM_MSG_SYSTEM = 0x000000,
		XM_QUIT,

	XM_SYSTEM_END
};

#define TP_COM			 1		//1
#define TP_TIMER		 2		//1
#define TP_ALARM		 3		//1
#define TP_CAPTURE	20		//16
#define TP_PTZ			26
#define TP_SMTP     30
#define WATCH_DOG   31
#define TP_RTP			32	
#define TP_BACKUP		35		//1
#define TP_SVR			37		//2
#define TP_NET			39		//16
#define TP_TEL			55		//1
#define TP_MOUSE		57		//1
#define TP_FTP			58
#define TP_WEB          58
#define TP_DEFAULT		64
#define TP_PRE			 127

typedef const char* PCSTR;

#endif //__TYPEDEF_EZTHREAD_H__
