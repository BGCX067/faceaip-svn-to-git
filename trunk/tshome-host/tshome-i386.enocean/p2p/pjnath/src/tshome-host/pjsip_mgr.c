/*
 * pjsip_mgr.c
 *
 *  Created on: Feb 7, 2012
 *      Author: SongShanping
 */

#include "pjsip_mgr.h"
#include "common/thread.h"
#include "report_mgr.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>
#include "operation_server_mgr.h"
#include "operation_term_mgr.h"
#include "operation_gateway_mgr.h"

#include "common/ts_err_code.h"
#include "start_up.h"
#include <netinet/in.h>
#include "snd_rcv.h"
#include "CommonInclude.h"

// g_is_net_connect_stoped is declared in start_up.c
extern int g_is_net_connect_stoped;

extern THREAD_MUTEX mux_lock_network_stop_flag;

// g_is_host_logout is declared in start_up.c
extern int g_is_host_logout;

extern THREAD_MUTEX mux_lock_host_logout_flag;

// global error buff
unsigned char global_error_buff[REPORT_MAX_LEN];

THREAD_MUTEX mux_lock_global_error_buff = PTHREAD_MUTEX_INITIALIZER;

// lock of pjsip report list
THREAD_MUTEX mux_lock_pjsip;

// condiction to get pjsip report
TIMER_COND cond_get_pjsip;

// the count of pjsip report in list
int length_pjsip = 0;

// to judge whether the pjsip repord received thread is stopd. 
int stop_flag_pjsip = FALSE;

// pj thread handle.
pj_thread_t *pj_handle = NULL;

// init the pjsip report list.
LIST_HEAD(pjsip_report_list);

void init_pjsip_thread_environment()
{
    INIT_MUTEX(&mux_lock_pjsip);
    INIT_COND(&cond_get_pjsip);
}

void destroy_pjsip_thread_environment()
{
    DESTROY_MUTEX(&mux_lock_pjsip);
    DESTROY_COND(&cond_get_pjsip);
}

void destroy_pjsip_report_list()
{
    //free all node in list
    LOCK_MUTEX(&mux_lock_pjsip);
    pjsip_report_t *rlist = NULL;
    pjsip_report_t *alist = NULL;
    list_for_each_entry_safe(rlist, alist, &pjsip_report_list, list_node){
        pjsip_report_t *temp = rlist;
        list_del(&rlist->list_node);
        if (NULL != temp->msg){
            free(temp->msg);
            temp->msg = NULL;
        }
        free(temp);
        temp = NULL;
        rlist = NULL;
    }
    UNLOCK_MUTEX(&mux_lock_pjsip);
}

void test_list()
{
    pjsip_report_t *rlist = NULL;
    list_for_each_entry(rlist,&pjsip_report_list,list_node){
        if (NULL != rlist->msg){
            printf("%s",rlist->msg);
        }
    }
}

void recv_pjsip_buff_callback(unsigned k,
							unsigned comp_id,
							char *buff,  int buff_len,
							char *ip_addr, int addr_len,
							void *response, int *res_len)
{
    if (NULL == buff){
        return;
    }

    printf("**********Into recv_pjsip_buff_callback ******\n");

    char *report = buff + LEN_CMDH_HEADER;

    remote_header_t * header = NULL;

    if (-1 == get_common_header(k, comp_id ,report, buff_len, &header) ||
    		NULL == header){

        printf("**********NULL == header recv_pjsip_buff_callback ******\n");
        return;
    }

    //if it is internet conn and the conn is deny, return.
    if (INTERNET == (int)header->protocol_type){
        
    	int flag = g_is_net_connect_stoped;
        
        printf("**********In recv_pjsip_buff_callback: g_is_net_connect_stoped = %d ******\n",g_is_net_connect_stoped);
        if (TRUE == flag){

            printf("**********Host's network has stopped ******\n");
            return;
        }
    }

    if(TRUE == is_global_error(header) || TRUE == is_exception_error(header)){

    	printf("**********Into Global Error handle ******\n");

    	global_error_t *res = NULL;
    	parse_global_error((unsigned char*)report, &res);

    	if (NULL == res){
    		return;
    	}

    	global_error_handle(res);

	}else if (TRUE == is_server_stop_host_conn_req(header)){

        host_deny_connect_req_t *req = NULL;
        parse_stophostnet_req((unsigned char*)report, &req);

        if (NULL == req){
            return;
        }

        stop_host_network_handle(ntohl(req->session_id),
                                 ntohl(req->report_id),
        						response,
        						res_len);
        
        printf(">>>> stop_host_network_handle: res buff=%s; len=%d\n",(char*)response,*res_len);

    }else{

        pjsip_report_t *rlist = (pjsip_report_t *)malloc(sizeof(pjsip_report_t));
        if (NULL == rlist) {
            return;
        }
        memset(rlist, 0, sizeof(pjsip_report_t));
        rlist->msg = (char *)malloc(buff_len+1);
        if (NULL == rlist->msg) {
			free(rlist);
            return;
        }
        memset(rlist->msg, 0, buff_len+1);
        memcpy(rlist->msg, buff, buff_len);
        rlist->buff_len = buff_len;
        rlist->comp_id = comp_id;
        rlist->p_k = k;

        LOCK_MUTEX(&mux_lock_pjsip);

        list_add_tail(&(rlist->list_node), &pjsip_report_list);

        length_pjsip++;

        SIGNAL_COND(&cond_get_pjsip);

        UNLOCK_MUTEX(&mux_lock_pjsip);
    }
}

void re_connect_server_callback()
{
    //re-login
    ask_secret_action();
}

void pjsip_exception_callback()
{
    // do nothing here.
}

int pjsip_report_handle(void *param)
{
    pjsip_report_t *rlist = NULL;
    while(FALSE == stop_flag_pjsip){

        LOCK_MUTEX(&mux_lock_pjsip);

        if (length_pjsip <= 0){
            COND_WAIT(&cond_get_pjsip, &mux_lock_pjsip);
        }

        if (TRUE == stop_flag_pjsip){
            UNLOCK_MUTEX(&mux_lock_pjsip);
            break;
        }

        rlist = list_entry(pjsip_report_list.next, pjsip_report_t, list_node);
        if (NULL != rlist){
            list_del(&rlist->list_node);
            length_pjsip--;
        }

        UNLOCK_MUTEX(&mux_lock_pjsip);

        if (NULL != rlist && NULL != rlist->msg){
            deal_with_pjsip_report(rlist->msg,
            						rlist->buff_len,
            						rlist->comp_id,
            						rlist->p_k);

            if (NULL != rlist->msg){
                free(rlist->msg);
                rlist->msg = NULL;
            }
            free(rlist);
            rlist = NULL;
        }
    }

    // destroy the thread
    pj_thread_join(pj_handle);
    pj_thread_destroy(pj_handle);

    return 0;
}

int get_common_header(unsigned k,
					  unsigned comp_id,
					  char *report, 
					  int len, 
					  remote_header_t **lheader)
{
    pass_common_header((unsigned char*)report, lheader);

    if (NULL == *lheader){
        printf("**********NULL == *lheader -  In get_common_header ******\n");
        return -1;
    }

    remote_header_t * header = *lheader;
    
    short l_len = ntohs(header->length);
    
    
    printf("l_len = %d\n", l_len);

    
    //whether the report is valid
    if (HEAD_START != (int)header->start || l_len + HEADER_LEN != len){
        *lheader = NULL;
		
		global_error_action(header->protocol_type,
							GLOBAL_ERROR,
							RESPONSE,
							0,
							header->comm_source,
							GE_PARSE_ERROR,
							k,
							comp_id);
		
        return -1;
    }

    return 0;
}

int is_server_stop_host_conn_req(remote_header_t *header)
{
    if ( SERVER == (int)header->comm_source &&
            REQUEST == (int)header->comm_type &&
            STOP_CONNECTION == (int)header->function_code){
        return TRUE;
    }
    return FALSE;
}

int is_global_error(remote_header_t *header)
{
	if (GLOBAL_ERROR == (int)header->function_code){
		return TRUE;
	}
	return FALSE;
}

int is_exception_error(remote_header_t *header)
{
    if (ERROR == (int)header->comm_type) {
        return TRUE;
    }
    return FALSE;
}

int is_pass_terminal_session_req(remote_header_t *header)
{
    if ( SERVER == (int)header->comm_source &&
            REQUEST == (int)header->comm_type &&
            PASS_TERM_SESSION == (int)header->function_code){
        return TRUE;
    }
    return FALSE;
}

int global_error_action(PROTOCOL_TYPE net_type,
						FUNCTION_CODE func_code,
						COMM_TYPE comm_type,
						int session_id,
						COMM_SOURCE destinate,
						GLOBAL_ERROR_TYPE err_code,
						unsigned k,
						unsigned comp_id)
{
	int len = 0;
	
	global_error_t req;
	memset(&req, 0, sizeof(req));
	
	init_common_header(&(req.header),
                       net_type,
                       sizeof(global_error_t),
                       func_code,
                       comm_type,
                       HOST_S);
	
	req.session_id = htonl(session_id);
	req.error_code = htons(err_code);
	
	len = sizeof(global_error_t) + LEN_CMDH_HEADER;
	
	LOCK_MUTEX(&mux_lock_global_error_buff);
	
	memset(global_error_buff, 0, REPORT_MAX_LEN);
	
	fill_pjsip_report_header(global_error_buff);
	
	u_char *temp = global_error_buff + LEN_CMDH_HEADER;
    memcpy(temp, &req, sizeof(req));
	
	if (destinate == CLIENT) {
		icedemo_send_data(k, comp_id ,global_error_buff, len);
	} else if (destinate == SERVER) {
		icedemo_send_data_to_srv(PJ_SEVER_THREAD_ID, (const char *)global_error_buff, len);
	}
	
	UNLOCK_MUTEX(&mux_lock_global_error_buff);
	
	return 0;
}

//different operations in different function codes
int global_error_handle(global_error_t *res)
{
	short err_code = ntohs(res->error_code);
	
	switch (err_code) {
		case GE_NO_LOGIN:{
			LOCK_MUTEX(&mux_lock_host_logout_flag);
			g_is_host_logout = TRUE;
			UNLOCK_MUTEX(&mux_lock_host_logout_flag);
			
			destroy_aes_key();
			destroy_rsa_key();
			distroy_server_session_id();
			
			ask_secret_action();
		}
			break;
		case GE_NO_USER:{
			// stop pjsip
			pjsip_stop_flag = 1;
			
		}
			break;
		case GE_RE_LOGIN:{
			
		}
			break;
		case GE_PARSE_ERROR:{
			
			// stop pjsip 
			pjsip_stop_flag = 1;
			
		}
			break;
		case GE_PROTOCOL_UNMATCH:{
			
			// stop pjsip 
			pjsip_stop_flag = 1;
			
		}
			break;
		default:
			break;
	}

	return 0;
}

void deal_with_pjsip_report(char *preport,
							int len,
							unsigned com_id,
							unsigned k)
{

    printf("**********Into Msg List -  In deal_with_pjsip_report ******\n");

    if (NULL == preport){
        return;
    }

    printf("%s",preport);

    char *report = preport + LEN_CMDH_HEADER;

    remote_header_t * header = NULL;

    if (-1 == get_common_header(k, com_id, report, len, &header) ||
    		NULL == header){
    	return;
    }

    //if it is internet conn and the conn is deny, return.
    if (INTERNET == (int)header->protocol_type){
        
		int flag = g_is_net_connect_stoped;
        
        if (TRUE == flag){
            printf("********** HOST NET WORK HAS STOPED! ******\n");
			
			global_error_action(header->protocol_type,
								GLOBAL_ERROR,
								RESPONSE,
								0,
								header->comm_source,
								GE_NET_HAS_STOPED,
								k,
								com_id);
			
            return;
        }
    }
	
    int fn_code = (int)header->function_code;

    switch (fn_code) {
    case HEARTBEAT:{
            //do nothing.
            break;
        }
    case DEVICE_CONTROL:{
		__fline;
		printf("DEVICE_CONTROL");
		// check sessionid sample
		terminal_identify_info_t * term_info = get_terminal(ntohl(888));
		if (NULL == term_info){
		    printf("NULL == term_info\n");
		}

            //wujj 2012-9-19 21:14:58.
            device_ctrl_handle(k, com_id, report, len);
            break;
        }
    case DEVICE_STATE:{
		__fline;		
            printf("DEVICE_CONTROL_ACK");

            //todo.
            break;
        }
    case ASK_SECRET_KEY:{

            switch ((int)header->comm_source){
            case CLIENT:{
                    if (REQUEST == (int)header->comm_type){
                        term_ask_rsa_secret_handle(k,
                        							com_id,
                                                   (int)header->protocol_type);
                    }
                    break;
                }
            case SERVER:{
                    if (RESPONSE == (int)header->comm_type){
                        ask_secret_res_t *res = NULL;
                        parse_ask_secretkey_res((unsigned char*)report, &res);
                        if (NULL == res){
                            return;
                        }
                        
                        char *l_secret_key = (char*)malloc(sizeof(res->secret_key)+1);
                        memset(l_secret_key, 0, sizeof(res->secret_key)+1);
                        memcpy(l_secret_key,res->secret_key,sizeof(res->secret_key));
                        
                        char *l_secret_mode = (char*)malloc(sizeof(res->secret_mode)+1);
                        memset(l_secret_mode, 0, sizeof(res->secret_mode)+1);
                        memcpy(l_secret_mode,res->secret_mode,sizeof(res->secret_mode));
                        
                        ask_secret_handle(l_secret_key, l_secret_mode);
                        
                        free(l_secret_key);
                        l_secret_key = NULL;
                        free(l_secret_mode);
                        l_secret_mode = NULL;
                    }
                    break;
                }
            case ETS_CLIENT:{
                    //do nothing.
                    break;
                }
            default:{

                    break;
                }
            }

            break;
        }
    case PASS_SECRET_KEY:{

            switch ((int)header->comm_source){
            case CLIENT:{
                    if (REQUEST == (int)header->comm_type){
                        pass_secret_req_t *req = NULL;
                        parse_term_pass_secretkey_req((unsigned char*)report, &req);
                        if (NULL == req){
                            return;
                        }
                        term_pass_aes_secret_handle(k,
                        							com_id,
                                                    (int)header->protocol_type,
                                                    (char *)req->secret_key);
                    }
                    break;
                }
            case SERVER:{
                    if (RESPONSE == (int)header->comm_type){
                        
                        printf("**********In deal_with_pjsip_report : PASS_SECRET_KEY ******\n");
                        pass_secret_res_t *res = NULL;
                        parse_pass_secretkey_res((unsigned char*)report, &res);
                        if (NULL == res){
                            return;
                        }
						
						printf(">>>>>%d\n",res->session_id);
						printf(">>>>>%d\n",sizeof(pass_secret_res_t));
                        pass_sceret_handle(ntohl(res->session_id));
                    }
                    break;
                }
            case ETS_CLIENT:{
                    //do nothing.
                    break;
                }
            default:{

                    break;
                }
            }

            break;
        }
    case LOGIN:{

            switch ((int)header->comm_source){
            case CLIENT:{
                    if (REQUEST == (int)header->comm_type){
                        login_req_t *req = NULL;
                        parse_term_login_req((unsigned char*)report, &req);
                        if (NULL == req){
                            return;
                        }
                        term_login_handle(k,
                        				  com_id,
                                          (int)header->protocol_type,
                                          req);
                    }
                    break;
                }
            case SERVER:{
                    if (RESPONSE == (int)header->comm_type){
                        login_res_t *res = NULL;
                        parse_login_res((unsigned char*)report, &res);
                        if (NULL == res){
                            return;
                        }
                        int ret = login_server_handle(ntohl(res->session_id), (char)res->status);
						if (-1 == ret) {
							//login failed, stop host
							// stop pjsip 
							pjsip_stop_flag = 1;
							
						}
                    }
                    break;
                }
            case ETS_CLIENT:{
                    //do nothing.
                    break;
                }
            default:{

                    break;
                }
            }
            break;
        }
    case UPDATE_QUERY:{

            if (SERVER == (int)header->comm_source){
                update_res_t *res = NULL;
                parse_update_res((unsigned char*)report, &res);
                if (NULL == res){
                    return;
                }

                update_handle(res);
            }
            break;
        }
    case UPDATE_ISSUE:{

            if (SERVER == (int)header->comm_source){
                update_issue_req_t *req = NULL;
                parse_update_issue_req((unsigned char*)report, &req);

                update_issue_handle(ntohl(req->session_id));
            }
            break;
        }
    case P2P:{
            //do nothing.
            break;
        }
    case DEVICE_OPERATION:{
            printf("\n>>>IN CASE: DEVICE_OPERATION \n");
                if (REQUEST == (int)header->comm_type){
                    device_ctrl_handle(k, com_id, report, len);
                }
            break;
        }
    case STOP_CONNECTION:{

            if (SERVER == (int)header->comm_source){
                if (RESPONSE == (int)header->comm_type){
                    stop_host_connect_res_t *res = NULL;
                    parse_term_stophostnet_res((u_char*)report, &res);
                    if (NULL == res){
                        return;
                    }
                    term_stop_host_net_handle(ntohl(res->session_id), (char)res->res_code);
                }
            }
            break;
        }
    case LOGOUT:{

            switch ((int)header->comm_source){
            case CLIENT:{
                    if (REQUEST == (int)header->comm_type){
                        logout_req_t *req = NULL;
                        parse_term_logout_req((unsigned char*)report, &req);
                        if (NULL == req){
                            return;
                        }
                        term_logout_handle(k,
                        				   com_id,
                                           (int)header->protocol_type,
                                           ntohl(req->session_id));
                    }
                    break;
                }
            case SERVER:{
                    if (RESPONSE == (int)header->comm_type){
                        logout_res_t *res = NULL;
                        parse_logout_res((unsigned char*)report, &res);
                        if (NULL == res){
                            return;
                        }
                        logout_server_handle(ntohl(res->session_id), (char)res->res_code);
                    }
                    break;
                }
            case ETS_CLIENT:{
                    //do nothing.
                    break;
                }
            default:{

                    break;
                }
            }
            break;
        }
    default:{

            break;
        }
    }
}

void start_pjsip_report_handle_thread(pj_pool_t *param)
{
	init_pjsip_thread_environment();

	LOCK_MUTEX(&mux_lock_pjsip);
	stop_flag_pjsip = FALSE;
	length_pjsip = 0;
    
	UNLOCK_MUTEX(&mux_lock_pjsip);
    
    int result = TS_NOT_STOP_NET;
    db_get_is_stop_net(&result);
    if (result == TS_NOT_STOP_NET) {
        g_is_net_connect_stoped = FALSE;
    }else if (result == TS_STOP_NET) {
        g_is_net_connect_stoped = TRUE;
    }

    pj_pool_t *pool = param;

    pj_status_t status = pj_thread_create(pool,
						"pjsip_report_thread",
						pjsip_report_handle,
						NULL,
						PJ_SERVER_THREAD_MEMORY_SIZE,
						0,
						&pj_handle);
}

void stop_pjsip_report_handle_thread()
{
    printf(">>>> Into stop_pjsip_report_handle_thread... \n");
    LOCK_MUTEX(&mux_lock_pjsip);

    stop_flag_pjsip = TRUE;
    SIGNAL_COND(&cond_get_pjsip);

    UNLOCK_MUTEX(&mux_lock_pjsip);

    sleep(0.5);
    
    destroy_pjsip_report_list();
    destroy_pjsip_thread_environment();
    printf(">>>> Leave from stop_pjsip_report_handle_thread... \n");
}
