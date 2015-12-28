/*
 * gateway_mgr.c
 *
 *  Created on: Feb 7, 2012
 *      Author: SongShanping
 */

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>
#include <netinet/in.h>

#include "gateway_mgr.h"
#include "common/thread.h"
#include "report_mgr.h"
#include "common/common_define.h"
#include "common/remote_def.h"
#include "operation_gateway_mgr.h"
#include "term_conn_mgr.h"
#include "snd_rcv.h"

THREAD_MUTEX mux_lock_gateway;

THREAD_MUTEX mux_lock_gateway_buff;

TIMER_COND cond_get_gateway;

// the count of node in gatewey report list
int length_gateway = 0;
int stop_flag_gateway = FALSE;

pj_thread_t *gateway_handle = NULL;

extern struct list_head terminal_list;
extern THREAD_MUTEX mux_lock_term_list;

u_char gateway_buff[REPORT_MAX_LEN] = {0};

LIST_HEAD(gateway_report_list);

void init_gateway_thread_environment()
{
    INIT_MUTEX(&mux_lock_gateway);
    INIT_MUTEX(&mux_lock_gateway_buff);
    INIT_COND(&cond_get_gateway);
}

void destroy_gateway_thread_environment()
{
    DESTROY_MUTEX(&mux_lock_gateway);
    DESTROY_MUTEX(&mux_lock_gateway_buff);
    DESTROY_COND(&cond_get_gateway);
}

void destroy_gateway_report_list()
{
    //free all node in list
    LOCK_MUTEX(&mux_lock_gateway);
    gateway_report_t *rlist = NULL;
    gateway_report_t *alist = NULL;
    list_for_each_entry_safe(rlist, alist, &gateway_report_list, list_node){
        gateway_report_t *temp = rlist;
        list_del(&rlist->list_node);
        free(temp->data);
        free(temp);
        temp = NULL;
        rlist = NULL;
    }
    UNLOCK_MUTEX(&mux_lock_gateway);
}

void recv_gateway_buff_callback(int type, void *data, int buff_len)
{
    if (NULL == data){
        return;
    }

    gateway_report_t *rlist = (gateway_report_t *)malloc(sizeof(gateway_report_t));
   // memset(rlist,0, sizeof(gateway_report_t));
    rlist->type = type;
    rlist->buff_len = buff_len;

    rlist->data = malloc(buff_len+1);
    if (NULL == rlist->data){
        free(rlist);
    	return;
    }
    memset(rlist->data, 0, buff_len+1);
    memcpy(rlist->data, data, buff_len);

    LOCK_MUTEX(&mux_lock_gateway);

    list_add_tail(&(rlist->list_node), &gateway_report_list);

    length_gateway++;

    SIGNAL_COND(&cond_get_gateway);

    UNLOCK_MUTEX(&mux_lock_gateway);
}

int gateway_report_handle(void *param)
{
    gateway_report_t *rlist = NULL;
    while(FALSE == stop_flag_gateway){

        LOCK_MUTEX(&mux_lock_gateway);

        if (length_gateway <= 0){
            COND_WAIT(&cond_get_gateway, &mux_lock_gateway);
        }

        if (TRUE == stop_flag_gateway){
            UNLOCK_MUTEX(&mux_lock_gateway);
            break;
        }

        rlist = list_entry(gateway_report_list.next, gateway_report_t, list_node);
        if (NULL != rlist){
            list_del(&rlist->list_node);
            length_gateway--;
        }
        
        UNLOCK_MUTEX(&mux_lock_gateway);

        if (NULL != rlist){
            deal_with_gateway_report(rlist->type, rlist->data, rlist->buff_len);
            free(rlist->data);
            free(rlist);
            rlist = NULL;
        }
    }

    pj_thread_join(gateway_handle);
    pj_thread_destroy(gateway_handle);

    return 0;
}

void deal_with_gateway_report(int type, void *data, int len)
{
    if (NULL == data){
        return;
    }

    int buff_len = 0;
    int32_t session_id = 0;
    int ctrl_result = FAILED;
    KNX_COMMAND_TYPE cmmd_type = -1;

    kic_head_t *kic_head = (kic_head_t*)data;

    COMM_SOURCE source = kic_head->header.comm_source;

    DEVICE_TYPE d_type = kic_head->device_type;
    session_id = ntohl(kic_head->session_id);

    if (d_type == KNX){
        
        printf("\n>>> In deal_with_gateway_report, reset KNX report \n");
        
        knx_control_report_t *knx_data = (knx_control_report_t *)data;

        knx_data->kic_head.header.comm_source = (u_char)HOST_S;
        knx_data->kic_head.header.comm_type = (u_char)RESPONSE;

        ctrl_result = knx_data->result_code;
        cmmd_type = knx_data->cmd_type;

        buff_len = sizeof(knx_control_report_t)+LEN_CMDH_HEADER;

    }else if (d_type == COM){

    	com_control_res_t *com_data = (com_control_res_t *)data;

    	com_data->kic_head.header.comm_source = (u_char)HOST_S;
    	com_data->kic_head.header.comm_type = (u_char)RESPONSE;

		ctrl_result = com_data->res_code;

		buff_len = len + LEN_CMDH_HEADER;

    }else if (d_type == WIFI
    		|| d_type == INFRARED
    		|| d_type == SERIAL_RES){

    	kic_head_t *kic_head = (kic_head_t*)data;

    	kic_head->header.comm_source = (u_char)HOST_S;
    	kic_head->header.comm_type = (u_char)RESPONSE;

    	buff_len = len + LEN_CMDH_HEADER;
        
    }else if (d_type == IR_STUDY){
        
        remote_header_t *header = (remote_header_t *)data;
        header->comm_source = (u_char)HOST_S;
        header->comm_type = (u_char)RESPONSE;
        
    	buff_len = len + LEN_CMDH_HEADER;
    }


    LOCK_MUTEX(&mux_lock_gateway_buff);

    memset(gateway_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(gateway_buff);
    u_char *temp = gateway_buff + LEN_CMDH_HEADER;

    memcpy(temp, data, len);

    if (source == SERVER){ //IR study

    	icedemo_send_data_to_srv(PJ_SEVER_THREAD_ID, gateway_buff, buff_len);

    }else if (source == CLIENT){

    	if (d_type == KNX){ //knx control result response, no need to broadcast;

				terminal_identify_info_t *term = get_terminal(session_id);
				if (NULL != term){
					icedemo_send_data(term->p_k,
										term->comp_id ,
										gateway_buff,
										buff_len);
										
                    
//                    printf("\n>>>In deal_with_gateway_report: have sent to client. Len = %d \n",buff_len);
//                    knx_control_report_t *req = (knx_control_report_t *)gateway_buff;
//                    printKnx(req);
				}

		}else if (d_type == SERIAL_RES){

			terminal_info_t *rlist = NULL;

			LOCK_MUTEX(&mux_lock_term_list);
			list_for_each_entry(rlist, &terminal_list, list_node){

				terminal_identify_info_t *term = &(rlist->terminal);

				icedemo_send_data(term->p_k,
									term->comp_id ,
									gateway_buff,
									buff_len);
									
			}
			UNLOCK_MUTEX(&mux_lock_term_list);

		}else if (d_type == COM){

			if (ctrl_result == FAILED){//if control failed, no need to broadcast;

				terminal_identify_info_t *term = get_terminal(session_id);
				if (NULL != term){
					icedemo_send_data(term->p_k,
										term->comp_id ,
										gateway_buff,
										buff_len);
										
				}

			    UNLOCK_MUTEX(&mux_lock_gateway_buff);
				return;
			}

			//if control success, broadcast;
			LOCK_MUTEX(&mux_lock_term_list);
			terminal_info_t *rlist = NULL;
			list_for_each_entry(rlist, &terminal_list, list_node){

				terminal_identify_info_t *term = &(rlist->terminal);

					icedemo_send_data(term->p_k,
										term->comp_id ,
										gateway_buff,
										buff_len);
										
			}
			UNLOCK_MUTEX(&mux_lock_term_list);

		}else if (d_type == WIFI
	    		|| d_type == INFRARED){
			//no need to broadcast;
			terminal_identify_info_t *term = get_terminal(session_id);
			if (NULL != term){
				icedemo_send_data(term->p_k,
									term->comp_id ,
									gateway_buff,
									buff_len);
									
			}
		}
    }

    UNLOCK_MUTEX(&mux_lock_gateway_buff);
}

void start_gateway_report_handle_thread(pj_pool_t *param)
{
	init_gateway_thread_environment();

	LOCK_MUTEX(&mux_lock_gateway);
	stop_flag_gateway = FALSE;
	length_gateway = 0;
	UNLOCK_MUTEX(&mux_lock_gateway);

    pj_pool_t *pool = param;

    pj_status_t status = pj_thread_create(pool,
									"gateway_report_thread",
    								gateway_report_handle,
									NULL,
									PJ_GATEWAY_THREAD_MEMORY_SIZE,
									0,
									&gateway_handle);

}

void stop_gateway_report_handle_thread()
{
    
    printf(">>>> Into stop_gateway_report_handle_thread... \n");
    
    LOCK_MUTEX(&mux_lock_gateway);
    stop_flag_gateway = TRUE;

    SIGNAL_COND(&cond_get_gateway);
    UNLOCK_MUTEX(&mux_lock_gateway);

    sleep(0.5);
    
    destroy_gateway_report_list();
    destroy_gateway_thread_environment();
    
    
    printf(">>>> Leave from stop_gateway_report_handle_thread... \n");
}
