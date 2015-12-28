/*
 * report_mgr.c
 *
 *  Created on: Feb 7, 2012
 *      Author: SongShanping
 */

#include "report_mgr.h"
#include <netinet/in.h>



static unsigned long long swap64(unsigned long long _x)  
{
   /* this is __bswap64 from:
    * $FreeBSD: src/sys/i386/include/endian.h,v 1.41$
    */
	return ((_x >> 56) | ((_x >> 40) & 0xff00) | ((_x >> 24) & 0xff0000) |
			((_x >> 8) & 0xff000000) | ((_x << 8) & ((unsigned long long)0xff << 32)) |
			((_x << 24) & ((unsigned long long)0xff << 40)) |
			((_x << 40) & ((unsigned long long)0xff << 48)) | ((_x << 56)));
}


unsigned long long hton64(const unsigned long long ho)  
{
	if (ntohs(0x1234) == 0x1234) {
		return ho;  
	} else {
		return swap64(ho);
	}
}

unsigned long long ntoh64(const unsigned long long no)
{
    return hton64(no);
}


static int64_t base_swap(const u_char bs[], int len)
{
	int64_t s =0; 
	int i;
	int count = len - 1;
	for(i = 0; i < len; i++){
		int64_t t = bs[i]&0xFF;
		t <<= 8*(count - i);
		s |= t;
	}
	return s;
}


int64_t btoll(const u_char bs[], int len)
{
	return base_swap(bs, len);
}

int btoi(const u_char bs[], int len)
{
	return base_swap(bs, len);
}

short btos(const u_char bs[], int len)
{
	return base_swap(bs, len);
}

int init_common_header(remote_header_t *header,
                        int protocol_type,
                        int length,
                        int fun_code,
                        int comm_type,
                        int source_code)
{
    if (NULL == header){
        return -1;
    }
    int body_len = length-HEADER_LEN;
    header->start = (u_char)0x7f;
    header->protocol_type = (u_char)protocol_type;
    header->length = htons((int16_t)body_len);
    header->serial_number = 0;
    header->function_code = (u_char)fun_code;
    header->comm_type = (u_char)comm_type;
    header->comm_source = (u_char)source_code;
	header->proto_ver = htons(PROTOCOL_VERSION);
    header->space = 0;

    return 0;
}

void copy_common_header(remote_header_t *des, remote_header_t *source)
{
    if (NULL == source || NULL == des){
        return;
    }

    des->start = source->start;
    des->comm_source = source->comm_source;
    des->comm_type = source->comm_type;
    des->function_code = source->function_code;

    des->length = source->length;

    des->protocol_type = source->protocol_type;

    des->serial_number = source->serial_number;
	des->proto_ver = source->proto_ver;
    des->space = source->space;
}

void copy_knx_ctrl_report(knx_control_report_t *des, knx_control_report_t *source)
{
    if (NULL == source || NULL == des){
        return;
    }

    kic_head_t des_kic_head = des->kic_head;
    kic_head_t source_kic_head = source->kic_head;

    copy_common_header(&(des_kic_head.header), &(source_kic_head.header));

    des_kic_head.session_id = source_kic_head.session_id;
    des_kic_head.device_type = source_kic_head.device_type;
    des_kic_head.home_device_id = source_kic_head.home_device_id;

    des->cmd_type = source->cmd_type;
    memcpy(des->data, source->data, sizeof(source->data));
    des->address = source->address;
    des->result_code = source->result_code;
    des->data_len = source->data_len;
    des->data_type = source->data_type;
}



int pass_common_header(u_char *recv_buff, remote_header_t **res)
{
    if(NULL == recv_buff){
        return -1;
    }

    *res = (remote_header_t *)recv_buff;

    return 0;
}

int fill_pjsip_report_header(u_char *send_buff)
{
    if (NULL == send_buff){
        return -1;
    }

    memcpy(send_buff, PJSIP_CMDH_HEADER, LEN_CMDH_HEADER);

    return 0;
}

int fill_ask_secretkey_req(u_char *send_buff, ask_secret_req_t *req)
{
    if (NULL == send_buff || NULL == req){
        return -1;
    }

    ask_secret_req_t *temp = (ask_secret_req_t*)send_buff;
    copy_common_header(&(temp->header), &(req->header));

    return sizeof(ask_secret_req_t);
}

int parse_ask_secretkey_res(u_char *recv_buff, ask_secret_res_t **res)
{
    if(NULL == recv_buff){
        return -1;
    }

    *res = (ask_secret_res_t *)recv_buff;

    return 0;
}

int fill_term_ask_secretkey_res(u_char *send_buff, ask_secret_res_t *res)
{
    if (NULL == send_buff || NULL == res){
        return -1;
    }

    ask_secret_res_t *temp = (ask_secret_res_t*)send_buff;
    copy_common_header(&(temp->header), &(res->header));
    memcpy(temp->secret_key, res->secret_key, sizeof(res->secret_key));
    memcpy(temp->secret_mode, res->secret_mode, sizeof(res->secret_mode));

    return sizeof(ask_secret_res_t);
}

int fill_pass_secretkey_req(u_char *send_buff, pass_secret_req_t *req)
{
    if (NULL == send_buff || NULL == req){
        return -1;
    }

    pass_secret_req_t *temp = (pass_secret_req_t*)send_buff;
    copy_common_header(&(temp->header), &(req->header));
    memcpy(temp->secret_key, req->secret_key, sizeof(req->secret_key));

    return sizeof(pass_secret_req_t);
}

int parse_pass_secretkey_res(u_char *recv_buff, pass_secret_res_t **res)
{
    if(NULL == recv_buff){
        return -1;
    }

    *res = (pass_secret_res_t *)recv_buff;

    return 0;
}

int fill_term_pass_secretkey_res(u_char *send_buff, pass_secret_res_t *res)
{
    if (NULL == send_buff || NULL == res){
        return -1;
    }

    pass_secret_res_t *temp = (pass_secret_res_t*)send_buff;
    copy_common_header(&(temp->header), &(res->header));
    temp->session_id = res->session_id;

    return sizeof(pass_secret_res_t);
}

int parse_term_pass_secretkey_req(u_char *recv_buff, pass_secret_req_t **req)
{
    if(NULL == recv_buff){
        return -1;
    }

    *req = (pass_secret_req_t *)recv_buff;

    return 0;
}

int fill_login_req(u_char *send_buff, login_req_t *req)
{
    if (NULL == send_buff || NULL == req){
        return -1;
    }

    login_req_t *temp = (login_req_t*)send_buff;
    copy_common_header(&(temp->header), &(req->header));
    temp->session_id = req->session_id;
    temp->password_len = req->password_len;
    temp->username_len = req->username_len;
    memcpy(temp->password, req->password,sizeof(req->password));
    memcpy(temp->user_name, req->user_name,sizeof(req->user_name));

    return sizeof(login_req_t);
}

int parse_login_res(u_char *recv_buff, login_res_t **res)
{
    if(NULL == recv_buff){
        return -1;
    }

    *res = (login_res_t *)recv_buff;

    return 0;
}

int fill_term_login_res(u_char *send_buff, login_res_t *res)
{
    if (NULL == send_buff || NULL == res){
        return -1;
    }

    login_res_t *temp = (login_res_t*)send_buff;
    copy_common_header(&(temp->header), &(res->header));
    temp->session_id = res->session_id;
    temp->status = res->status;
    memset(temp->space, 0, sizeof(temp->space));

    return sizeof(login_res_t);
}

int parse_term_login_req(u_char *recv_buff, login_req_t **req)
{
    if(NULL == recv_buff){
        return -1;
    }

    *req = (login_req_t *)recv_buff;

    return 0;
}

int fill_update_req(u_char *send_buff, update_req_t *req)
{
    if (NULL == send_buff || NULL == req){
        return -1;
    }

    update_req_t *temp = (update_req_t*)send_buff;
    copy_common_header(&(temp->header), &(req->header));
    temp->session_id = req->session_id;
    temp->latest_time = req->latest_time;
    temp->query_from = req->query_from;
	strcpy((char *) temp->product_id, (char *)req->product_id);

    return 0;
}

int parse_update_res(u_char *recv_buff, update_res_t **res)
{
    if(NULL == recv_buff){
        return -1;
    }

    *res = (update_res_t *)recv_buff;

	int i =0;
	printf(" ----------------------> md5 is [\n");
	for( ; i < 48; i++)
	{
		printf("%d, \n", (*res)->md5[i]);
	}
	printf("\n");

    return 0;
}

/*
int fill_updatesoftware_req(u_char *send_buff, update_software_req_t *req)
{
    if (NULL == send_buff || NULL == req){
        return -1;
    }

    update_software_req_t *temp = (update_software_req_t*)send_buff;
    copy_common_header(&(temp->header), &(req->header));
    temp->session_id = req->session_id;
    temp->big_version = req->big_version;
    temp->small_version = req->small_version;
    temp->client_type = req->client_type;
    temp->space = req->space;

    return 0;
}
*/

int parse_update_issue_req(u_char *recv_buff,
                             update_issue_req_t **req)
{
    if(NULL == recv_buff){
        return -1;
    }

    *req = (update_issue_req_t *)recv_buff;

    return 0;
}

int fill_knxctrl_res(u_char *send_buff, knx_control_report_t *res)
{

    if (NULL == send_buff || NULL == res){
        return -1;
    }

    knx_control_report_t *temp = (knx_control_report_t*)send_buff;

    copy_knx_ctrl_report(temp, res);

    return 0;
}

int parse_knxctrl_req(u_char *recv_buff, knx_control_report_t **req)
{
    if(NULL == recv_buff){
        return -1;
    }

    *req = (knx_control_report_t *)recv_buff;

    return 0;
}

int fill_wifictrl_res(u_char *send_buff, wifi_control_res_t *res)
{
    if (NULL == send_buff || NULL == res){
        return -1;
    }

    wifi_control_res_t *temp = (wifi_control_res_t*)send_buff;

    kic_head_t temp_kic_head = temp->kic_head;
    kic_head_t res_kic_head = res->kic_head;

    copy_common_header(&(temp_kic_head.header), &(res_kic_head.header));
    temp_kic_head.session_id = res_kic_head.session_id;
    temp_kic_head.device_type = res_kic_head.device_type;
    temp_kic_head.home_device_id = res_kic_head.home_device_id;
    temp->function_code = res->function_code;
    temp->res_code = res->res_code;

    return 0;
}

int parse_wifictrl_req(u_char *recv_buff, wifi_control_req_t **req)
{
    if(NULL == recv_buff){
        return -1;
    }

    *req = (wifi_control_req_t *)recv_buff;

    return 0;
}

int parse_scenectrl_req(u_char *recv_buff, scene_control_report_t **req)
{
	if (NULL == recv_buff){
		return -1;
	}

	*req = (scene_control_report_t *)recv_buff;

	return 0;
}

int fill_serial_initiative_res(u_char *send_buff, serial_initiative_res_t *res)
{
 //do nothing

	return 0;
}

int fill_ir_study_res(u_char *send_buff, ir_study_res_t *res)
{
    return 0;
}

int parse_ir_study_req(u_char *recv_buff, ir_study_req_t **req)
{
    return 0;
}

int fill_stophostnet_res(u_char *send_buff, host_deny_connect_res_t *res)
{
    if (NULL == send_buff || NULL == res){
        return -1;
    }

    host_deny_connect_res_t *temp = (host_deny_connect_res_t*)send_buff;
    copy_common_header(&(temp->header), &(res->header));
    temp->session_id = res->session_id;
    temp->report_id = res->report_id;
    temp->res_code = res->res_code;
    memset(temp->space, 0 ,sizeof(temp->space));

    return 0;
}

int parse_stophostnet_req(u_char *recv_buff, host_deny_connect_req_t **req)
{
    if(NULL == recv_buff){
        return -1;
    }

    *req = (host_deny_connect_req_t *)recv_buff;

    return 0;
}

int fill_term_stophostnet_req(u_char *send_buff, stop_host_connect_req_t *req)
{
    if (NULL == send_buff || NULL == req){
        return -1;
    }

    stop_host_connect_req_t *temp = (stop_host_connect_req_t*)send_buff;
    copy_common_header(&(temp->header), &(req->header));
    temp->session_id = req->session_id;
    memcpy(temp->check_code, req->check_code, sizeof(req->check_code));

    return 0;
}

int parse_term_stophostnet_res(u_char *recv_buff, stop_host_connect_res_t **res)
{
    if(NULL == recv_buff){
        return -1;
    }

    *res = (stop_host_connect_res_t *)recv_buff;
    return 0;
}

int fill_logout_req(u_char *send_buff, logout_req_t *req)
{
    if (NULL == send_buff || NULL == req){
        return -1;
    }

    logout_req_t *temp = (logout_req_t*)send_buff;
    copy_common_header(&(temp->header), &(req->header));
    temp->session_id = req->session_id;

    return 0;
}

int parse_logout_res(u_char *recv_buff, logout_res_t **res)
{
    if(NULL == recv_buff){
        return -1;
    }

    *res = (logout_res_t *)recv_buff;

    return 0;
}

int fill_term_logout_res(u_char *send_buff, logout_res_t *res)
{
    if (NULL == send_buff || NULL == res){
        return -1;
    }

    logout_res_t *temp = (logout_res_t*)send_buff;
    copy_common_header(&(temp->header), &(res->header));
    temp->session_id = res->session_id;
    temp->res_code = res->res_code;
    memset(temp->space, 0, sizeof(temp->space));

    return 0;
}

int parse_term_logout_req(u_char *recv_buff, logout_req_t **req)
{
    if(NULL == recv_buff){
        return -1;
    }

    *req = (logout_req_t *)recv_buff;

    return 0;
}


int fill_pass_term_session_res(u_char *send_buff, pass_term_session_res_t *res)
{
    if (NULL == send_buff || NULL == res){
        return -1;
    }

    pass_term_session_res_t *temp = (pass_term_session_res_t*)send_buff;
    copy_common_header(&(temp->header), &(res->header));
    temp->session_id = res->session_id;
    temp->term_session_id = res->term_session_id;

    return sizeof(pass_term_session_res_t);
}

int parse_pass_term_session_req(u_char *recv_buff,
                                pass_term_session_req_t **req)
{
    if(NULL == recv_buff){
        return -1;
    }

    *req = (pass_term_session_req_t *)recv_buff;

    return 0;
}


int parse_global_error(u_char *recv_buff,
        global_error_t **res)
{
	if(NULL == recv_buff){
	        return -1;
	    }

	*res = (global_error_t *)recv_buff;

	return 0;
}



void printKnx(knx_control_report_t *req)
{
    printf("----> req->start = %d\n",req->kic_head.header.start);
    printf("----> req->protocal = %d\n",req->kic_head.header.protocol_type);
    printf("----> req->funcode = %d\n",req->kic_head.header.function_code);
    printf("----> req->restypr = %d\n",req->kic_head.header.comm_type);
    printf("----> req->source = %d\n",req->kic_head.header.comm_source);
    
    printf("----> req->buff_len = %d\n",req->kic_head.header.length);
    printf("----> req->buff_len = %d\n",ntohs(req->kic_head.header.length));
    
    printf("----> req->session_id = %d\n",req->kic_head.session_id);
    printf("----> req->device_type = %d\n",req->kic_head.device_type);
    printf("----> req->homedevice_id = %d\n",req->kic_head.home_device_id);
    printf("----> req->cmd_type = %d\n",req->cmd_type);
    printf("----> req->address = %d\n",req->address);
    printf("----> req->data = %d\n",*((int*)req->data));
    printf("----> req->result_code = %d\n",req->result_code);
    printf("----> req->data_len = %d\n",req->data_len);
    printf("----> req->data_type = %d\n",req->data_type);
}
