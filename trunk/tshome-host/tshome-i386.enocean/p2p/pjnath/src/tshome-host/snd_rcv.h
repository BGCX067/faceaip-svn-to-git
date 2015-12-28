/* $Id: snd_rcv.h 3603 2012-02-27 01:53:35Z renjy $ */
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */
#ifndef __SND_RCV_H__
#define __SND_RCV_H__

#include <stdio.h>
#include <stdlib.h>
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjnath.h>
#include <unistd.h>


/* For this demo app, configure longer STUN keep-alive time
 * so that it does't clutter the screen output.
 */
#define KA_INTERVAL 300


/* This is our global variables */
struct app_t
{
    /* Command line options are stored here */
    struct options
    {
	unsigned    comp_cnt;
	pj_str_t    ns;
	int	    max_host;
	pj_bool_t   regular;
	pj_str_t    stun_srv;
	pj_str_t    turn_srv;
	pj_bool_t   turn_tcp;
	pj_str_t    turn_username;
	pj_str_t    turn_password;
	pj_bool_t   turn_fingerprint;
	const char *log_file;
    } opt;

    /* Our global variables */
    pj_caching_pool	 cp;
    pj_pool_t		*pool;
    pj_thread_t		*thread;
    pj_bool_t		 thread_quit_flag;
    pj_ice_strans_cfg	 ice_cfg;
    pj_ice_strans	*icest;
    FILE		*log_fhnd;

    /* Variables to store parsed remote ICE info */
    struct rem_info
    {
	char		 ufrag[80];
	char		 pwd[80];
	unsigned	 comp_cnt;
	pj_sockaddr	 def_addr[PJ_ICE_MAX_COMP];
	unsigned	 cand_cnt;
	pj_ice_sess_cand cand[PJ_ICE_ST_MAX_CAND];
    } rem;

};

extern struct app_t *icedemo;
extern unsigned clients;

extern unsigned pjsip_stop_flag;

/* Utility to display error messages */
void icedemo_perror(const char *title, pj_status_t status);

/* Utility: display error message and exit application (usually
 * because of fatal error.
 */
void err_exit(const char *title, pj_status_t status);

/*#define CHECK(expr)	status=expr; \
			if (status!=PJ_SUCCESS) { \
			    err_exit(#expr, status); \
			}*/

/*
 * This function checks for events from both timer and ioqueue (for
 * network events). It is invoked by the worker thread.
 */
pj_status_t handle_events(unsigned max_msec, unsigned *p_count, int k);

/*
 * This is the worker thread that polls event in the background.
 */
int icedemo_worker_thread(void *unused);

/*
 * This is the callback that is registered to the ICE stream transport to
 * receive notification about incoming data. By "data" it means application
 * data such as RTP/RTCP, and not packets that belong to ICE signaling (such
 * as STUN connectivity checks or TURN signaling).
 */
#ifndef TSHOME
void cb_on_rx_data(pj_ice_strans *ice_st,
                   unsigned comp_id, 
		   void *pkt, pj_size_t size,
		   const pj_sockaddr_t *src_addr,
		   unsigned src_addr_len);
#else
void cb_on_rx_data(pj_ice_strans *ice_st,
	           unsigned comp_id, 
		   void *pkt, pj_size_t size,
		   const pj_sockaddr_t *src_addr,
		   unsigned src_addr_len,
                   void *result,
                   unsigned *result_len);
#endif

/*
 * This is the callback that is registered to the ICE stream transport to
 * receive notification about ICE state progression.
 */
void cb_on_ice_complete(pj_ice_strans *ice_st, 
			       pj_ice_strans_op op,
			       pj_status_t status);

/* log callback to write to file */
void log_func(int level, const char *data, int len);

/*
 * This is the main application initialization function. It is called
 * once (and only once) during application initialization sequence by 
 * main().
 */
pj_status_t icedemo_init(void);

/*
 * Create ICE stream transport instance, invoked from the menu.
 */
pj_status_t icedemo_create_instance(unsigned k, unsigned inner_mode);

/* Utility to nullify parsed remote info */
void reset_rem_info(unsigned k);

/*
 * Destroy ICE stream transport instance, invoked from the menu.
 */
void icedemo_destroy_instance(void);

/*
 * Create ICE session, invoked from the menu.
 */
pj_status_t icedemo_init_session(unsigned k,
                                 unsigned rolechar,
                                 unsigned inner_mode);

/*
 * Stop/destroy ICE session, invoked from the menu.
 */
void icedemo_stop_session(void);
void icedemo_stop_session2(int k);

/*#define PRINT(fmt, arg0, arg1, arg2, arg3, arg4, arg5)	    \
	printed = pj_ansi_snprintf(p, maxlen - (p-buffer),  \
				   fmt, arg0, arg1, arg2, arg3, arg4, arg5); \
	if (printed <= 0) return -PJ_ETOOSMALL; \
	p += printed*/


/* Utility to create a=candidate SDP attribute */
int print_cand(char buffer[], unsigned maxlen,
		      const pj_ice_sess_cand *cand);

/* 
 * Encode ICE information in SDP.
 */
int encode_session(char buffer[], unsigned maxlen);

/* 
 * Encode ICE information in SDP.
 */
int encode_session2(unsigned k, char buffer[], unsigned maxlen);

/*
 * Show information contained in the ICE stream transport. This is
 * invoked from the menu.
 */
void icedemo_show_ice(void);

/*
 * Get local candicates SDP.
 */
pj_status_t icedemo_get_local_sdp (unsigned k, char *buffer, int size);

/*
 * Input and parse SDP from the remote (containing remote's ICE information) 
 * and save it to global variables.
 */
void icedemo_parse_remote(unsigned k, const char *buf, int size);

/*
 * Input and parse SDP from the remote (containing remote's ICE information) 
 * and save it to global variables.
 */
void icedemo_input_remote(void);

/*
 * Start ICE negotiation! This function is invoked from the menu.
 */
pj_status_t icedemo_start_nego(unsigned k);

/*
 * Send application data to remote agent.
 */
pj_status_t icedemo_send_data(unsigned k, unsigned comp_id, const char *data, unsigned data_len);

/*
 * Send application data to server.
 */
pj_status_t icedemo_send_data_to_srv (unsigned k, const char *data, unsigned data_len);

/*
 * Display help for the menu.
 */
void icedemo_help_menu(void);

/*
 * Display console menu
 */
void icedemo_print_menu(void);

/*
 * Main console loop.
 */
#ifndef TSHOME
void icedemo_console(void);
#else
void icedemo_console(pj_bool_t host_mode);
#endif

/*
 * Display program usage.
 */
void icedemo_usage();

#endif
