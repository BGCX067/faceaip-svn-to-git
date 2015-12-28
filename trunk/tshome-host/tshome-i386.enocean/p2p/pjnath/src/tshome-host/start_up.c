/*
 * start_up.c
 *
 *  Created on: Feb 17, 2012
 *      Author: SongShanping
 */

#include "start_up.h"

#include "log/ts_log_tools.h"
#include "codec_mgr.h"
#include "gateway_mgr.h"
#include "pjsip_mgr.h"
#include "operation_gateway_mgr.h"
#include "operation_server_mgr.h"
#include "operation_term_mgr.h"
#include "db/ts_db_conn_infos.h"
#include "term_conn_mgr.h"
#include "web_mgr.h"

#include "db/sqlite3_ctrl.h"
#include "common/common_define.h"

#include <dlfcn.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "snd_rcv.h"

pj_pool_t * pool_instance = NULL;

void start_db()
{
//   int result = sc_db_init(DB_FILE_PATH);
//    if (result != EXIT_SUCCESS) {
//        printf("ERROR: database init failed!\n");
//    }
}

void end_db()
{
//   fprintf(stderr,"\nclose db----------->\n"); 
//    sc_close(DB_FILE_PATH);
    sc_close();
}

void start_cm(pj_pool_t *param)
{
	init_rsa_decode_key();
    
    start_gateway_module();

    start_pjsip_report_handle_thread(param);
 
	start_gateway_report_handle_thread(param);

}

void end_cm()
{
    printf(">>>> Into end_cm... \n");
    
    //logout server
    logout_server();
    
    stop_gateway_report_handle_thread();
    
    stop_pjsip_report_handle_thread();
   
    end_gateway_module();
    
    //release memory
    destroy_aes_key();
    destroy_rsa_decode_key();
    destroy_rsa_key();
    distroy_server_session_id();

    destroy_terminal_list();
    
    printf(">>>> Leave from end_cm... \n");
}

/*
 * Interface with Host Web
 */

//start host progress
void start(pj_pool_t *param)
{
    pool_instance = param;
    
	struct sigaction act;
	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGTERM, &act, 0);

	//start_db();
    
	start_cm(param);

    start_web_action_thread(param);
}

//end host progress
void end()
{
    stop_web_action_thread();
    
	end_cm();
    
	end_db();
}

void sig_handler(int sig)
{
    printf(">>>> capture a signal = %d\n", sig);

    end();
    
    printf(">>>> source have releaseed \n");
	
	log_close();
	
	err_exit("Quitting..", PJ_SUCCESS);

    exit(0);
}

