/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * smart_webs.h - brief
 *
 * Copyright (C) 2013 FaceMetro.com, All Rights Reserved.
 *
 * $Id: smart_webs.h 5884 2013-06-24 10:20:33Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-06-24 10:20:33  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _smart_webs_H
#define _smart_webs_H 1


#ifdef __cplusplus
extern "C"
{
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// 定制信息
#define LoginPrompt_t  T("SmartPorts")	/* smart port */
#define S_PRT_DEFAULT_HOME		T("index.htm") /* Default home page */
#define S_PRT_DEFAULT_PORT		880		/* Default HTTP port */
#define S_PRT_DEFAULT_SSL_PORT	4433		/* Default HTTPS port */
#define S_PRT_DEFAULT_RETRIES		5		/* Server port g_iretries */
#define rootWeb  T("./spages")			/* Root web directory */
#define password  T("")				/* Security password */

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int smart_webs_init();

int smart_webs_open(int port, int retries);

int smart_webs_proc();

int smart_webs_close();

int smart_webs_cleanup();

#ifdef __cplusplus
}
#endif

#endif /* _smart_webs_H */
