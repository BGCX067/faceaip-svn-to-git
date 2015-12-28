/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ez_webs.h - _explain_
 *
 * Copyright (c) 2004-2010 Sergey Lyubka.
 *
 * $Id: ez_webs.h 5884 2011-11-05 09:57:31Z WuJunjie $
 *
 *  Explain:
 *     -powered by shttpd-
 *
 *  Update:
 *     2011-10-13 22:00:11   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __EZ_WEBS_H
#define  __EZ_WEBS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//
int ez_webs_start(int ihttp_port, int iapp_port, int ilan, const char *root, const char* webfile);
int ez_webs_stop();
int ez_webs_restart();
int ez_webs_status();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __EZ_WEBS_H
