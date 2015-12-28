/*
 * pjsip_mgr.h
 *
 *  Created on: Feb 7, 2012
 *      Author: SongShanping
 */

#ifndef PJSIP_MGR_H_
#define PJSIP_MGR_H_

#include <stdio.h>
#include <stdlib.h>
#include "common/remote_def.h"
#include "common/list.h"
#include "common/common_define.h"

#include <pjlib.h>

#include "db/ts_db_conn_infos.h"
#include "db/ts_db_tools.h"

/*
 *data struct for pjsip report queue
 */
typedef struct _pjsip_report
{
    struct list_head list_node; //list node
    unsigned comp_id;           //comp id
    unsigned p_k;               //thread id
    char ip_addr[32];           //ip address
    int addr_len;               //len of ip address
    char *msg;                  //response report message
    int buff_len;               //len of response report message
}pjsip_report_t;

/*
 * release memory of list
 */
void destroy_pjsip_report_list();

//to do test
void test_list();


/*
 * PJSIP call back method
 * add receive message buff to list
 */
void recv_pjsip_buff_callback(unsigned k,
							unsigned comp_id,
							char *buff,  int buff_len,
							char *ip_addr, int addr_len,
							void *response, int *res_len);

/*
 * PJSIP call back method
 * be called when host disconnected.
 */
void re_connect_server_callback();

/*
 * PJSIP call back method
 * be called when has exception.
 */
void pjsip_exception_callback();

/*
 * PJSIP report handle method
 * get report from list and to deal with
 */
int pjsip_report_handle(void *param);

/*
 * get common header from report buff.
 */
int get_common_header(unsigned k,
					  unsigned comp_id,
					  char *report, 
					  int len, 
					  remote_header_t **lheader);

/*
 * judge the report whether is to stop the internet connection of host
 */
int is_server_stop_host_conn_req(remote_header_t *header);

/*
 * judge the report whether is to pass terminal session id from server
 */
int is_pass_terminal_session_req(remote_header_t *header);

/*
 * judge the report whether is global error msg from server
 */
int is_global_error(remote_header_t *header);

/*
 * judge the report whether is exeption error msg from server
 */
int is_exception_error(remote_header_t *header);

/*
 * send global error to terminal or server.
 */
int global_error_action(PROTOCOL_TYPE net_type,
						FUNCTION_CODE func_code,
						COMM_TYPE comm_type,
						int session_id,
						COMM_SOURCE destinate,
						GLOBAL_ERROR_TYPE err_code,
						unsigned k,
						unsigned comp_id);

/*
 * to deal with the global error from server
 */
int global_error_handle(global_error_t *res);

/*
 * parse the report to deal with .
 */
void deal_with_pjsip_report(char *preport,
							int len,
							unsigned com_id,
							unsigned k);

/*
 * init the enviroment of pjsip thread .
 */
void init_pjsip_thread_environment();

/*
 * destroy the enviroment of pjsip thread .
 */
void destroy_pjsip_thread_environment();

/*
 * start the thread to handle pjsip report .
 */
void start_pjsip_report_handle_thread(pj_pool_t *param);

/*
 * stop the thread of handle pjsip report .
 */
void stop_pjsip_report_handle_thread();

#endif /* PJSIP_MGR_H_ */
