/*
 * operation_gateway_mgr.c
 *
 *  Created on: Feb 9, 2012
 *      Author: SongShanping
 */

#include "operation_gateway_mgr.h"
#include "term_conn_mgr.h"
#include "codec_mgr.h"
#include "db/ts_db_conn_infos.h"
#include "gateway_mgr.h"
#include "common/thread.h"

#include "pjsip_mgr.h"

#include <netinet/in.h>
#include "snd_rcv.h"
#include <dlfcn.h>

THREAD_MUTEX mux_lock_knx = PTHREAD_MUTEX_INITIALIZER;

u_char knx_buff[REPORT_MAX_LEN] = {0};

extern struct list_head terminal_list;

extern THREAD_MUTEX mux_lock_term_list;

void *handle = NULL;

int device_ctrl_handle(unsigned k, unsigned com_id, char *ctrl_msg, int len)
{
    if (NULL == ctrl_msg){
        return -1;
    }

    if (handle == NULL) {
        return -1;
    }
    
    
    printf("\n>>>INTO device_ctrl_handle \n");
    
	INT_FUN_VOIDP_INT send_request = (INT_FUN_VOIDP_INT) dlsym(handle, "protocol_request");
    
    kic_head_t *kic_head = (kic_head_t*)ctrl_msg;

    COMM_SOURCE source = (COMM_SOURCE)kic_head->header.comm_source;

    if (source == SERVER){ //IR study from server
        
        printf("\n>>>INTO device_ctrl_handle: receive from server \n");
    	//send to gateway model
    	int ret = send_request(ctrl_msg, len);
        
        printf("\n>>>> send to gateway ret = %d\n",ret);
        
		return ret;
    }

    //check permission
    terminal_identify_info_t *term = get_terminal(ntohl(kic_head->session_id));
    if (NULL == term){
        
        printf("\n>>>INTO device_ctrl_handle: have no terminal info \n");
        
		remote_header_t	header = kic_head->header;
		
		global_error_action(header.protocol_type,
							DEVICE_OPERATION,
							ERROR,
							ntohl(kic_head->session_id),
							CLIENT,
							GE_NO_TERMINAL_INFO,
							k,
							com_id);
		
        return GE_NO_TERMINAL_INFO;
    }

    int is_control = TS_DEVICE_PERMISSION_FALSE;
    
    int en_len = at_get_encrypt_len_from_strlen(term->len_name)+1;
    char *name_decode = (char *)malloc(en_len);
    if (NULL != name_decode){
        memset(name_decode, 0, en_len);

        aes_encode aes_encode_key;
        at_create_key_by_string((const u_char *)(term->aes_key),
                                &aes_encode_key);

        at_decrypt((char *)term->user_name, name_decode,
                   term->len_name,aes_encode_key);

        int device_id = ntohl(kic_head->home_device_id);
        db_is_device_control_by_name(name_decode, device_id, &is_control);

        free(name_decode);
    }

    if (TS_DEVICE_PERMISSION_FALSE == is_control){
        
        printf("\n>>>INTO device_ctrl_handle: no permission \n");
        
		global_error_action(term->net_type,
							DEVICE_OPERATION,
							ERROR,
							ntohl(term->session_id),
							CLIENT,
							GE_CONTROL_NO_PERMISION,
							term->p_k,
							term->comp_id);
		
        return GE_CONTROL_NO_PERMISION; //no permission
    }

    //send to gateway model
    int ret = send_request(ctrl_msg, len);
    
    printf("\n>>>> send to gateway ret = %d\n",ret);
    
    printf("\n>>>INTO device_ctrl_handle: have send to Gateway model \n");

    return ret;
}

int start_gateway_module()
{
    handle = dlopen(GATEWAY_SO_PATH , RTLD_LAZY);
	if(handle == NULL)
	{
		printf("[%s]\n", dlerror());
		return -1;
	}
	else
	{
		printf(">>>>libGatewayManager.so Opened OK\n");
	}
    
	int nRes = -1;
    
	//set_knx_callbacks
    typedef void (*set_knx_callbacks)(OnSearchResponse, OnReadResponse,
                                      OnWriteResponse, OnKnxConnect, OnKnxDisconnect, OnRWError);
	set_knx_callbacks set_knx_backs = (set_knx_callbacks) dlsym(handle, "protocol_set_knx_callbacks");
	if (NULL != set_knx_backs){
        set_knx_backs(on_search_response,
                      on_read_response,
                      on_write_response,
                      on_knx_connect,
                      on_knx_disconnect,
                      on_rw_error);
        printf(">>>>libGatewayManager.so set_knx_callbacks OK\n");
	}else {
        printf(">>>>libGatewayManager.so set_knx_callbacks failed\n");
    }

    
	//set_ctrl_response
    typedef void (* set_ctrl_response) (OnCtrlResponse ctrl_response);
	set_ctrl_response setCtrlResponse = dlsym(handle, "protocol_set_ctrl_response");
	if (NULL != setCtrlResponse){
        setCtrlResponse(recv_gateway_buff_callback);
	}
    
	//start_manager
	INT_FUN_VOID startManager = (INT_FUN_VOID) dlsym(handle, "protocol_start_manager");
	if (NULL != startManager){
        nRes = startManager();
	}
    
	//set_wifi_callbacks
    typedef void (*set_wifi_callbacks)(OnWifiConnect conn_fun,
                                       OnWifiDisconnect disconn_fun);
    set_wifi_callbacks set_wifi_backs = (set_wifi_callbacks) dlsym(handle, "protocol_set_wifi_callbacks");
	if (NULL != set_wifi_backs){
        set_wifi_backs(on_wifi_connect, on_wifi_disconnect);
	}
    
	//connect_all
	INT_FUN_VOID connectAll = (INT_FUN_VOID) dlsym(handle, "protocol_connect_all");
	if (NULL != connectAll){
        nRes = connectAll();
	}
    
    

    return nRes;
}

int end_gateway_module()
{
    
    printf(">>>>Into fun: end gateway module ....\n");
    
	int nRes = -1;
    
	//disconnect_all
    typedef void (*disconnect_all_t)();
	disconnect_all_t disconnect_all = (disconnect_all_t) dlsym(handle, "protocol_disconnect_all");
    if (NULL != disconnect_all) {
        disconnect_all();
    }

	//stop_manager
	typedef void (*StopManager)();
	StopManager stopManager = (StopManager) dlsym(handle, "protocol_stop_manager");
    if (NULL != stopManager) {
        stopManager();
    }
    
    if(handle != NULL)
	{
		nRes = dlclose(handle);
		if(nRes == 0)
		{
			printf("libGatewayManager.so closed\n");
		}
	}

    printf(">>>> end gateway module ....\n");
    
    return 0;
}

/*
 * callback of knx devices
 */

KNX_DATA_TYPE get_data_type_from_dpt(KNX_DATA_POINT_TYPE dpt)
{
	KNX_DATA_TYPE result = DATA_TYPE_UNKNOWN;
	switch (dpt)
	{
		case DPT_Boolean:
			result = DATA_TYPE_BINARY;
			break;
		case DPT_Date:
			result = DATA_TYPE_DATE;
			break;
		case DPT_Time:
			result = DATA_TYPE_TIME;
			break;
		case DPT_String:
			result = DATA_TYPE_TEXT;
			break;
		case DPT_1BitCtrl:
		case DPT_3BitCtrl:
		case DPT_CharSet:
		case DPT_8BitUnSign:
		case DPT_8BitSign:
		case DPT_2OctUnSign:
		case DPT_2OctSign:
		case DPT_Access:
		case DPT_4OctUnSign:
		case DPT_4OctSign:
			result = DATA_TYPE_INT;
			break;
		case DPT_2OctFloat:
		case DPT_4OctFloat:
			result = DATA_TYPE_ANALOG;
			break;
		default:
			result = DATA_TYPE_UNKNOWN;
			break;
	}
	return result;
}

void knx_response_thread_handle(knx_response_thread_param* param)
{
    if (NULL == param) {
        return;
    }
    
    knx_response_handle(param->device_id, param->grp_data, param->type);
    
    free(param);
    
}

void knx_response_handle(int device_id,
						KNX_GroupData grp_data,
						KNX_COMMAND_TYPE type)
{
    printf("\n>>> Into knx_response_handle... \n");
    
	knx_control_report_t knx_data;

	memset(&knx_data, 0 , sizeof(knx_control_report_t));

	kic_head_t *kic_head = &(knx_data.kic_head);

	init_common_header(&(kic_head->header),
					   INTERNET,
					   sizeof(knx_control_report_t),
					   DEVICE_OPERATION,
					   RESPONSE,
					   HOST_S);

	kic_head->device_type = (u_char)KNX;
	kic_head->home_device_id = htonl(device_id);

	knx_data.cmd_type = (u_char)type;

	short addr = (short)(grp_data.nGroupAddr);
	knx_data.address = htons(addr);

	knx_data.result_code = (u_char)SUCCESS;

	KNX_DATA_TYPE knx_data_type = get_data_type_from_dpt(grp_data.nGrpDPT);

	int data_len = 0;
	switch (knx_data_type) {
		case DATA_TYPE_BINARY:{
			data_len = sizeof(int);
            int b = htonl(grp_data.Data.BinData);
            memcpy(knx_data.data, &b, data_len);
		}
			break;
        case DATA_TYPE_INT:{
			data_len = sizeof(int);
            int d = htonl((int)grp_data.Data.AnaData);
            memcpy(knx_data.data, &d, data_len);
		}
			break;
		case DATA_TYPE_ANALOG:{
			data_len = sizeof(grp_data.Data.AnaData);
            float d = grp_data.Data.AnaData;
            memcpy(knx_data.data, &d, data_len);
		}
			break;
		case DATA_TYPE_TEXT:{
			data_len = strlen(grp_data.Data.TxtData);
            
            memcpy(knx_data.data, grp_data.Data.TxtData, data_len);
		}
			break;
		case DATA_TYPE_DATE:
		case DATA_TYPE_TIME:{
            
            KNX_TIMEDATA d = grp_data.Data.TimeData;
            time_data_t sent_d;
            sent_d.year = htons((short)d.year);
            sent_d.month = (u_char)d.month;
            sent_d.day = (u_char)d.day;
            sent_d.hour = (u_char)d.hour;
            sent_d.minute = (u_char)d.minute;
            sent_d.second = (u_char)d.second;
            sent_d.weekday = (u_char)d.weekday;
            
			data_len = sizeof(time_data_t);
            memcpy(knx_data.data, &sent_d, data_len);
		}
			break;
		default:{
			data_len = 0;
		}
			break;
	}
	knx_data.data_len = (u_char)data_len;
	knx_data.data_type = (u_char)grp_data.nGrpDPT;

	int buff_len = sizeof(knx_control_report_t)+LEN_CMDH_HEADER;

	terminal_info_t *rlist = NULL;

	LOCK_MUTEX(&mux_lock_term_list);

	list_for_each_entry(rlist, &terminal_list, list_node){


		terminal_identify_info_t *term = &(rlist->terminal);

		knx_data.kic_head.header.protocol_type = (u_char)term->net_type;

		knx_data.kic_head.session_id = htonl(term->session_id);
        
		LOCK_MUTEX(&mux_lock_knx);

		memset(knx_buff, 0, REPORT_MAX_LEN);

		fill_pjsip_report_header(knx_buff);

		u_char *tmp_buff = knx_buff + LEN_CMDH_HEADER;
		memcpy(tmp_buff, &knx_data, sizeof(knx_control_report_t));

        icedemo_send_data(term->p_k,
							term->comp_id ,
							(const char *)knx_buff,
							buff_len);
        
//        printf(">> on_write_response: have sent. len = %d\n",buff_len);
//        printKnx((knx_control_report_t*)knx_buff);
							
		UNLOCK_MUTEX(&mux_lock_knx);
	}

	UNLOCK_MUTEX(&mux_lock_term_list);
}

void on_read_response(int device_id, KNX_GroupData grp_data)
{
    printf("\n>>>INTO on_read_response \n");
    
	//knx_response_handle(device_id, grp_data, READ);
    
    knx_response_thread_param * param = (knx_response_thread_param *)malloc(sizeof(knx_response_thread_param)); // free in knx_response_thread_handle
    
    if (NULL == param || NULL == pool_instance) {
        return;
    }
    
    memset(param,0, sizeof(knx_response_thread_param));
    
    param->device_id = device_id;
    
    memcpy(&(param->grp_data), &grp_data, sizeof(KNX_GroupData));
    
    param->type = READ;
    
    
    pj_thread_t *response_handle = NULL;
    
    pj_thread_create(pool_instance,
                     "on_read_response_thread",
                     knx_response_thread_handle,
                     param,
                     PJ_GATEWAY_THREAD_MEMORY_SIZE,
                     0,
                     &response_handle);
    // destroy the thread
    pj_thread_join(response_handle);

}

void on_write_response(int device_id, KNX_GroupData grp_data)
{
    printf("\n>>>INTO on_write_response \n");
    
    knx_response_thread_param * param = (knx_response_thread_param *)malloc(sizeof(knx_response_thread_param)); // free in knx_response_thread_handle
    
    if (NULL == param || NULL == pool_instance) {
        return;
    }
    
    memset(param,0, sizeof(knx_response_thread_param));
    
    param->device_id = device_id;
    
    memcpy(&(param->grp_data), &grp_data, sizeof(KNX_GroupData));
    
    param->type = WRITE;
    
    pj_thread_t *response_handle = NULL;
    
    pj_thread_create(pool_instance,
                     "on_write_response_thread",
                     knx_response_thread_handle,
                     param,
                     PJ_GATEWAY_THREAD_MEMORY_SIZE,
                     0,
                     &response_handle);
    
    // destroy the thread
    pj_thread_join(response_handle);
}

void on_knx_connect(int gw_index, int success, int expId, char *strErr)
{
	// do nothing.
}

void on_knx_disconnect(int gw_index)
{
	// do nothing.
}

void on_rw_error(int gw_index,
					int exception_id,
					char *exceiption_dis,
					int success)
{
	// do nothing.
}

void on_search_response(int gw_index, char *ip, int port, int status)
{
	// do nothing.
}

/*
 * callback of wifi devices
 */
void on_wifi_connect(int gw_index, int success)
{
	// do nothing.
}

void on_wifi_disconnect(int gw_index)
{
	// do nothing.
}
