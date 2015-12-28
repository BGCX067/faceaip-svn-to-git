/* $Id: icedemo.c 3603 2011-07-07 01:53:35Z bennylp $ */
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
#include "snd_rcv.h"
#include "start_up.h"
#include "keep_dog.h"
#include "db/ts_db_conn_infos.h"

#define THIS_FILE   "icedemo.c"

struct app_t  *icedemo;
unsigned clients;

/*
 * And here's the main()
 */
int main(int argc, char *argv[])
{
    struct pj_getopt_option long_options[] = {
	{ "comp-cnt",           1, 0, 'c'},
	{ "nameserver",		1, 0, 'n'},
	{ "max-host",		1, 0, 'H'},
	{ "help",		0, 0, 'h'},
	{ "stun-srv",		1, 0, 's'},
	{ "turn-srv",		1, 0, 't'},
#ifndef TSHOME
	{ "turn-tcp",		0, 0, 'T'},
#else
	{ "client-number",      1, 0, 'N'},
#endif
	{ "turn-username",	1, 0, 'u'},
	{ "turn-password",	1, 0, 'p'},
	{ "turn-fingerprint",	0, 0, 'F'},
	{ "regular",		0, 0, 'R'},
	{ "log-file",		1, 0, 'L'},
    };
    int c, opt_id;
    pj_status_t status;
    unsigned comp_cnt = 1;
    unsigned max_host = -1;
    pj_bool_t turn_fingerprint = PJ_FALSE;
    pj_bool_t regular = PJ_FALSE;
    pj_bool_t name_server = PJ_FALSE;
    pj_bool_t stun_srv_flag = PJ_FALSE;
    pj_bool_t turn_srv_flag = PJ_FALSE;
    pj_bool_t turn_usr_flag = PJ_FALSE;
    pj_bool_t turn_pass_flag = PJ_FALSE;
    pj_str_t ns, stun_srv, turn_srv, turn_usr, turn_pass;
    char * log_file = NULL;
    int k = 0;
    pj_bool_t host_inner_mode = 0;

    while((c=pj_getopt_long(argc,argv, "c:n:N:s:t:u:p:H:L:hTFR", long_options, &opt_id))!=-1) {
        switch (c) {
	case 'c':
            comp_cnt = atoi(pj_optarg);
	    if (comp_cnt < 1 || comp_cnt >= PJ_ICE_MAX_COMP) {
	        puts("Invalid component count value");
	        return 1;
	    }
	    break;
#ifdef TSHOME
	case 'N':
            clients = atoi(pj_optarg);
	    break;
#endif
	case 'n':
            name_server = PJ_TRUE;
	    ns = pj_str(pj_optarg);
	    break;
	case 'H':
	    max_host = atoi(pj_optarg);
	    break;
	case 'h':
	    icedemo_usage();
	    return 0;
	case 's':
            stun_srv_flag = PJ_TRUE;
	    stun_srv = pj_str(pj_optarg);
	    break;
	case 't':
            turn_srv_flag = PJ_TRUE;
	    turn_srv = pj_str(pj_optarg);
	    break;
#ifndef TSHOME
        case 'T':
	    icedemo[k].opt.turn_tcp = PJ_TRUE;
	    break;
#else
#endif
	case 'u':
            turn_usr_flag = PJ_TRUE;
	    turn_usr = pj_str(pj_optarg);
	    break;
	case 'p':
            turn_pass_flag = PJ_TRUE; 
	    turn_pass = pj_str(pj_optarg);
	    break;
	case 'F':
	    turn_fingerprint = PJ_TRUE;
	    break;
	case 'R':
	    regular = PJ_TRUE;
	    break;
	case 'L':
	    log_file = pj_optarg;
	    break;
	default:
	    printf("Argument \"%s\" is not valid. Use -h to see help",
	       argv[pj_optind]);
	    return 1;
	}
    }

    /* Start keep dog thread */
    //keep_dog_thread();
    /* Get MAX support users from DB
    */
    status = db_get_max_users (&clients);
    if (status != 0) {
        printf ("Get Max support users from DB failed!\n");
        return 1;
    }
    if(clients <= 0 ||
       clients % 2 != 0) {
        printf ("client number must be even number\n");
        return 1;
    }

    /* 
     * Use the appt[0] to communicate with server, the others to do P2P communication.
     */
    clients = 4;
    clients += 1;
    icedemo = (struct app_t*)malloc(sizeof(struct app_t) * clients);
    if(NULL == icedemo) {
        printf ("alloc memory failed!\n");
        return 1;
    }
    pj_bzero (icedemo, sizeof(struct app_t) * clients);

    /* Init Outer-Mode configure for STUN or TURN server */
    for (k=0; k<(clients/2+1); k++) {
        icedemo[k].opt.comp_cnt = comp_cnt;
        icedemo[k].opt.max_host = max_host;
        if (PJ_TRUE == name_server)
            icedemo[k].opt.ns = ns;

        if (PJ_TRUE == stun_srv_flag)
	    icedemo[k].opt.stun_srv = stun_srv;

        if (PJ_TRUE == turn_srv_flag)
            icedemo[k].opt.turn_srv = turn_srv;

        if (PJ_TRUE == turn_usr_flag) {
	    icedemo[k].opt.turn_username = turn_usr;
	    icedemo[k].opt.turn_password = turn_usr;
        }

        //if (PJ_TRUE == turn_pass_flag)
	//    icedemo[k].opt.turn_password = turn_pass;

        icedemo[k].opt.turn_fingerprint = turn_fingerprint;
	icedemo[k].opt.regular = regular;
	icedemo[k].opt.log_file = log_file;
    }

    /* Init Inner-Mode configure for STUN or TURN server */
    for (k=(clients/2+1); k<clients; k++) {
        icedemo[k].opt.comp_cnt = comp_cnt;
        icedemo[k].opt.max_host = max_host;
        icedemo[k].opt.turn_fingerprint = turn_fingerprint;
	icedemo[k].opt.regular = regular;
	icedemo[k].opt.log_file = log_file;
    }

    status = icedemo_init();
    if (status != PJ_SUCCESS)
	return 1;

	pjsip_stop_flag = 0;
    start(icedemo[0].pool);

    /* If host is in inner mode ,set 1; else set 0 */
    if (PJ_FALSE == stun_srv_flag &&
        PJ_FALSE == turn_srv_flag)
        host_inner_mode = 1;

    icedemo_console(host_inner_mode);

    end();
    err_exit("Quitting..", PJ_SUCCESS);
    return 0;
}
