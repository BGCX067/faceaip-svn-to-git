/*
 * report_mgr.c
 *
 *  Created on: Feb 7, 2012
 *      Author: SongShanping
 */

#include "app.h"
#include "../Logs.h"

//long long int a2l(const u_char src[], int len);
//int a2i(const u_char src[], int len);
//int j_a2i(const u_char src[], int len, int is_java_order);
//short a2s(const u_char *src, int len);

void init_common_header(remote_header_t *header,
		int protocol_type,
		int protocol_ver,
		int length,
		int fun_code,
		int comm_type,
		int source_code)
{
	if (NULL == header){
		return;
	}
	header->start = (u_char)0x7f;
	header->protocol_type = (u_char)protocol_type;
	header->length = htons((int16_t)length);
	header->serial_number = 0;
	header->function_code = (u_char)fun_code;
	header->comm_type = (u_char)comm_type;
	header->comm_source = (u_char)source_code;
	header->proto_ver = htons((int16_t)protocol_ver);
	header->space = 0;
//	memset(header->space, 0, sizeof(header->space));
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
	des->length = htons(source->length);
	des->protocol_type = source->protocol_type;
	des->proto_ver = htons(source->proto_ver);
	des->serial_number = htons(source->serial_number);
	des->space = 0;//source->space;
//	memset(des->space, 0, sizeof(des->space));
}

/*
void copy_knx_ctrl_report(knx_control_report_t *des, knx_control_report_t *source)
{
	if (NULL == source || NULL == des){
			return;
	}

	copy_common_header(&(des->header), &(source->header));

	memcpy(des->session_id, source->session_id, sizeof(source->session_id));
	des->device_type = source->device_type;
	memcpy(des->home_device_id, source->home_device_id, sizeof(source->home_device_id));
	des->cmd_type = source->cmd_type;
	des->main_addr = source->main_addr;
	des->mid_addr = source->mid_addr;
	des->logical_addr = source->logical_addr;
	des->result_code = source->result_code;
	des->data_len = source->data_len;
	des->data_type = source->data_type;
	memcpy(des->data, source->data, sizeof(source->data));
}
*/



int pass_common_header(u_char *recv_buff, remote_header_t *res)
{
	if(NULL == recv_buff){
		return -1;
	}

	memcpy(res, recv_buff, sizeof(remote_header_t));
	res->length = ntohs(res->length);
	res->serial_number = ntohs(res->serial_number);
	res->proto_ver = ntohs(res->proto_ver);
	/*int index = 0;
	res->start = recv_buff[index++];
	res->protocol_type = recv_buff[index++];

	u_char sl[2];
	memcpy(sl, recv_buff+index, sizeof(sl));
	int16_t len = btos(sl, sizeof(sl));//j_a2i(sl, sizeof(sl), 0);
	res->length = ntohs(len);
	index += sizeof(sl);

	u_char sb[2];
	memcpy(sb, recv_buff+index, sizeof(sb));
	int16_t searil = btos(sb, sizeof(sb));//j_a2i(sb, sizeof(sb), 0);
	res->serial_number = ntohs(searil);
	index += sizeof(sb);

	res->function_code = recv_buff[index++];
	res->comm_type = recv_buff[index++];
	res->comm_source = recv_buff[index++];

	u_char spv[2];
        memcpy(spv, recv_buff+index, sizeof(spv));
        int16_t pv = btos(spv, sizeof(spv));
        res->proto_ver = ntohs(pv);
        index += sizeof(spv);

	res->space = 0;*/
//	memset(res->space, 0, sizeof(res->space));
//printf("before pass_common_header return\n");
	return 0;
}

int fill_heartbeat_res(u_char *send_buff, heartbeat_res_t *res)
{
	if (NULL == send_buff || NULL == res){
		return -1;
	}
	res->session_id = htonl(res->session_id);
	memcpy(send_buff, res, sizeof(heartbeat_res_t));
	return 0;
}

int fill_heartbeat_req(u_char *send_buff, heartbeat_req_t *req)
{
	if (NULL == send_buff || NULL == req){
		return -1;
	}
	req->session_id = htonl(req->session_id);
	memcpy(send_buff, req, sizeof(heartbeat_res_t));
	return 0;
}

int parse_heartbeat_req(u_char *recv_buff, heartbeat_req_t *req)
{
	if(NULL == recv_buff){
		return -1;
	}
	memcpy(req, recv_buff, sizeof(heartbeat_req_t));
	req->session_id = ntohl(req->session_id);
	return 0;

}

int parse_heartbeat_res(u_char *recv_buff, heartbeat_res_t *res)
{
	if(NULL == recv_buff){
		return -1;
	}
	memcpy(res, recv_buff, sizeof(heartbeat_res_t));
	res->session_id = ntohl(res->session_id);
	return 0;

}


int fill_ask_secretkey_res(u_char *send_buff, ask_secret_res_t *res)
{
	if (NULL == send_buff || NULL == res){
		return -1;
	}

	//remote_header_t * header = (remote_header_t *)send_buff;
	//copy_common_header(header, &(res->header));
	/*memcpy(send_buff, &(res->header), sizeof(remote_header_t));
	int index = sizeof(remote_header_t);
	memcpy(send_buff+index, res->secret_key, sizeof(res->secret_key));
	index += sizeof(res->secret_key);
	memcpy(send_buff+index, res->secret_mode, sizeof(res->secret_mode));*/
	memcpy(send_buff, res, sizeof(ask_secret_res_t));
	return 0;
}

int parse_ask_secretkey_req(u_char *recv_buff, ask_secret_req_t *req)
{
	if(NULL == recv_buff){
			return -1;
	}
//printf("header size = %d\n", sizeof(remote_header_t));
	/*remote_header_t *header = (remote_header_t *)req;
	pass_common_header(recv_buff, header);*/

	memcpy(req, recv_buff, sizeof(ask_secret_req_t));
	return 0;
}

int fill_pass_secretkey_res(u_char *send_buff, pass_secret_res_t *res)
{
	if (NULL == send_buff || NULL == res){
		return -1;
	}

	//remote_header_t * header = (remote_header_t *)send_buff;
	//copy_common_header(header, &(res->header));
//	memcpy(send_buff, &(res->header), sizeof(remote_header_t));
//	int index = sizeof(remote_header_t);
	int32_t sessionid = htonl(res->session_id);
//	memcpy(send_buff+index, &sessionid, sizeof(sessionid));
	res->session_id = sessionid;
	memcpy(send_buff, res, sizeof(pass_secret_res_t));
	return 0;
}

int parse_pass_secretkey_req(u_char *recv_buff, pass_secret_req_t *req)
{
	if(NULL == recv_buff){
			return -1;
	}
	/*remote_header_t *header = (remote_header_t *)req;
	pass_common_header(recv_buff, header);
	int index = sizeof(remote_header_t);
	memcpy(req->secret_key, recv_buff+index, sizeof(req->secret_key));*/
	memcpy(req, recv_buff, sizeof(pass_secret_req_t));
	return 0;
}

int fill_login_res(u_char *send_buff, login_res_t *res)
{
	if (NULL == send_buff || NULL == res){
		return -1;
	}
	//remote_header_t * header = (remote_header_t *)send_buff;
	//copy_common_header(header, &(res->header));
	/*memcpy(send_buff, &(res->header), sizeof(remote_header_t));

	int index = sizeof(remote_header_t);
	int32_t sessionid = htonl(res->session_id);
	memcpy(send_buff+index, &sessionid, sizeof(sessionid));
	index += sizeof(sessionid);

	//int32_t addr = (res->peer_addr);
	int32_t addr = htonl(res->peer_ip);
printf("in fill_login_res: addr=%d\n",addr);
	memcpy(send_buff+index, &addr, sizeof(addr));
	index += sizeof(addr);
	int32_t port = htonl(res->peer_port);
	//int32_t port = (res->peer_port);
printf("in fill_login_res: port=%d\n",port);
	memcpy(send_buff+index, &port, sizeof(port));
	index += sizeof(port);

	send_buff[index++] = res->status;

	memset(send_buff+index, 0,sizeof(res->space));*/

	res->session_id = htonl(res->session_id);
	res->peer_ip = htonl(res->peer_ip);
	res->peer_port = htonl(res->peer_port);
	memcpy(send_buff, res, sizeof(login_res_t));

	return 0;
}

int parse_login_req(u_char *recv_buff, login_req_t *req)
{
	/*if(NULL == recv_buff){
			return -1;
	}
	remote_header_t *header = (remote_header_t *)req;
	pass_common_header(recv_buff, header);
	int index = sizeof(remote_header_t);
	u_char sb[4];
	memcpy(sb, recv_buff+index, sizeof(sb));
	int32_t sessionid = btoi(sb, sizeof(sb));//j_a2i(sb, sizeof(sb), 0);
	req->session_id = sessionid;//ntohl(sessionid);
	index += sizeof(sb);

	req->username_len = recv_buff[index++];
	memcpy(req->user_name, recv_buff+index, sizeof(req->user_name));
	index += sizeof(req->user_name);

	req->password_len = recv_buff[index++];
	memcpy(req->password, recv_buff+index, sizeof(req->password));
	memset(req->space, 0, sizeof(req->space));
	return 0;*/
	memcpy(req, recv_buff, sizeof(login_req_t));
	req->session_id = ntohl(req->session_id);
	return 0;
}

int fill_updateconfig_res(u_char *send_buff, update_res_t *res, aes_encode aes)
{
	if (NULL == send_buff || NULL == res){
		return -1;
	}
	int encrypt_len_c= 0;
	int encrypt_len_f= 0;
	int encrypt_len_m= 0;
	char * s_encrypt_c = NULL;
	char * s_encrypt_f = NULL;
	char * s_encrypt_m = NULL;

	res->session_id = htonl(res->session_id);

	if(res->conf_url_len > 0){
		encrypt_len_c = at_get_encrypt_len_from_strlen(res->conf_url_len);
		s_encrypt_c = (char *) calloc(encrypt_len_c, sizeof(char));
		at_encrypt((const char *)res->conf_url, s_encrypt_c, res->conf_url_len, aes);
	}

	if(res->file_url_len > 0){
		encrypt_len_f = at_get_encrypt_len_from_strlen(res->file_url_len);
	    s_encrypt_f = (char *) calloc(encrypt_len_f, sizeof(char));
	    at_encrypt((const char *)res->file_url, s_encrypt_f, res->file_url_len, aes);
	}

	if(res->file_url_len > 0){
 		//printf("res->md5=%s\n", res->md5);
		encrypt_len_m = at_get_encrypt_len_from_strlen(strlen((char *)res->md5));
		s_encrypt_m = (char *) calloc(encrypt_len_m, sizeof(char));
	    at_encrypt((const char *)res->md5, s_encrypt_m, strlen((char *)res->md5), aes);
	}

	memset(res->conf_url, 0, sizeof(res->conf_url));
	memset(res->file_url, 0, sizeof(res->file_url));
	memset(res->md5, 0, sizeof(res->md5));

	assert(encrypt_len_c <= sizeof(res->conf_url));
	if(s_encrypt_c != NULL){
		memcpy(res->conf_url, s_encrypt_c, encrypt_len_c);
		free(s_encrypt_c);
	}
	
	assert(encrypt_len_f <= sizeof(res->file_url));
	if(s_encrypt_f != NULL){
		memcpy(res->file_url, s_encrypt_f, encrypt_len_f);
		free(s_encrypt_f);
	}
	
	assert(encrypt_len_m <= sizeof(res->md5));
	if(s_encrypt_m != NULL){
		memcpy(res->md5, s_encrypt_m, encrypt_len_m);
		free(s_encrypt_m);
	}

	memcpy(send_buff, res, sizeof(update_res_t));
	//remote_header_t * header = (remote_header_t *)send_buff;
	//copy_common_header(header, &(res->header));
	/*memcpy(send_buff, &(res->header), sizeof(remote_header_t));

	int index = sizeof(remote_header_t);
	int32_t sessionid = htonl(res->session_id);
	memcpy(send_buff+index, &sessionid, sizeof(sessionid));
	index+=sizeof(sessionid);
	send_buff[index++] = res->query_from;
	send_buff[index++] = res->conf_upd_mode;
	send_buff[index++] = res->conf_url_len;
	int encrypt_len = 0;
	char * s_encrypt = NULL;
	//////////////////////////////////////
	encrypt_len = at_get_encrypt_len_from_strlen(res->conf_url_len);
	s_encrypt = (char *) calloc(encrypt_len, sizeof(char));
	at_encrypt((const char *)res->conf_url, s_encrypt, res->conf_url_len, aes);
	memcpy(send_buff + index, s_encrypt, encrypt_len);
	index += encrypt_len;
	free(s_encrypt);
	s_encrypt = NULL;
	///////////////////////////////////////
	send_buff[index++] = res->file_upd_mode;
        send_buff[index++] = res->file_url_len;
	///////////////////////////////////////
	encrypt_len = at_get_encrypt_len_from_strlen(res->file_url_len);
        s_encrypt = (char *) calloc(encrypt_len, sizeof(char));
        at_encrypt((const char *)res->file_url, s_encrypt, res->file_url_len, aes);
        memcpy(send_buff + index, s_encrypt, encrypt_len);
        index += encrypt_len;
        free(s_encrypt);
        s_encrypt = NULL;
	//////////////////////////////////////
	encrypt_len = sizeof(res->md5);
	s_encrypt = (char *) calloc(encrypt_len, sizeof(char));
        at_encrypt((const char *)res->md5, s_encrypt, (encrypt_len-16), aes);
        memcpy(send_buff + index, s_encrypt, encrypt_len);
        index += encrypt_len;
        free(s_encrypt);
        s_encrypt = NULL;	*/

	
	return 0;
}

int parse_updateconfig_req(u_char *recv_buff, update_req_t *req)
{
	if(NULL == recv_buff){
			return -1;
	}
	/*remote_header_t *header = (remote_header_t *)req;
	pass_common_header(recv_buff, header);
	int index = sizeof(remote_header_t);

	u_char sb[4];
	memcpy(sb, recv_buff+index, sizeof(sb));
	int32_t sessionid = btoi(sb, sizeof(sb));//j_a2i(sb, sizeof(sb), 0);
	req->session_id = sessionid;//ntohl(sessionid);
	index += sizeof(sb);

	u_char st[8];
	memcpy(st, recv_buff+index, sizeof(st));
	int64_t time = btoll(st, sizeof(st));//a2l(st, sizeof(st));
	req->latest_time = time;
	index += sizeof(st);

	req->query_from = recv_buff[index++];

	memcpy(req->product_id, recv_buff+index, sizeof(req->product_id));*/
#ifdef LINUX_X86_64
	update_req_t_64 __host_req;
	LOG4CPLUS_DEBUG(LOG_TURN, "sizeof(__host_req)(" << sizeof(__host_req) << ")");

	memcpy(&__host_req, recv_buff, sizeof(__host_req));

	req->header = __host_req.header;

	req->session_id = __host_req.session_id;
	req->query_from= __host_req.query_from;

	memcpy(&req->latest_time, __host_req.latest_time, 8);
	memcpy(&req->product_id, __host_req.product_id, sizeof(__host_req.product_id));
	
#else
	memcpy(req, recv_buff, sizeof(update_req_t));
#endif

	req->session_id = ntohl(req->session_id);
	req->latest_time = ntoh64(req->latest_time);

	LOG4CPLUS_DEBUG(LOG_TURN, "req->session_id(" << req->session_id << ") req->latest_time(" << req->latest_time << ")");

	return 0;
}

int fill_push_update(u_char *send_buff, update_issue_req_t *req)
{
	if (NULL == send_buff || NULL == req){
                return -1;
        }

        //remote_header_t * header = (remote_header_t *)send_buff;
        //copy_common_header(header, &(req->header));
	/*memcpy(send_buff, &(req->header), sizeof(remote_header_t));

        int index = sizeof(remote_header_t);
        int32_t sessionid = htonl(req->session_id);
        memcpy(send_buff+index, &sessionid, sizeof(sessionid));*/
    req->session_id = htonl(req->session_id);
	memcpy(send_buff, req, sizeof(update_issue_req_t));
	return 0;
}

/*
int fill_knxctrl_req(u_char **send_buff, knx_control_report_t *req)
{

	if (NULL == *send_buff || NULL == req){
		return -1;
	}

	knx_control_report_t *temp = (knx_control_report_t*)send_buff;

	copy_knx_ctrl_report(temp, req);

	return 0;
}

int parse_knxctrl_res(u_char *recv_buff, knx_control_report_t **res)
{
	if(NULL == recv_buff){
			return -1;
	}

	*res = (knx_control_report_t *)recv_buff;

	return 0;
}
*/
int fill_wifictrl_req(u_char *send_buff, ir_study_req_t *req)
{
	if (NULL == send_buff || NULL == req){
		return -1;
	}
	//remote_header_t * header = (remote_header_t *)send_buff;
	//copy_common_header(header, &(req->header));
	/*memcpy(send_buff, &(req->header), sizeof(remote_header_t));
	int index = sizeof(remote_header_t);

	int32_t sessionid = htonl(req->session_id);
	memcpy(send_buff+index, &sessionid, sizeof(sessionid));
	index+=sizeof(sessionid);

	send_buff[index++] = req->device_type;

	int32_t reportid = htonl(req->report_id);
        memcpy(send_buff+index, &reportid, sizeof(reportid));
        index+=sizeof(reportid);

	int32_t gatewayid = htonl(req->gateway_id);
        memcpy(send_buff+index, &gatewayid, sizeof(gatewayid));
        index+=sizeof(gatewayid);

	send_buff[index++] = req->channel;

	int16_t fcode = htons(req->function_code);
	memcpy(send_buff+index, &fcode, sizeof(fcode));
	index+=sizeof(fcode);
	send_buff[index] = req->space;*/

	printf("req->gateway_id = %d\n", req->gateway_id);
	printf("req->function_code = %d\n", req->function_code);
	printf("req->channel = %d\n", req->channel);
	printf("req->kic_head.home_device_id = %d\n", req->kic_head.home_device_id);

	

	req->kic_head.session_id = htonl(req->kic_head.session_id);
	req->kic_head.home_device_id= htonl(req->kic_head.home_device_id);
	req->gateway_id = htonl(req->gateway_id);
	req->function_code = htons(req->function_code);

	memcpy(send_buff, req, sizeof(ir_study_req_t));
	//printf("ir learn buff:");
	int i = 0;
	for(; i < sizeof(ir_study_req_t); i++){
		printf("%d,", send_buff[i]);
	}
	printf("\n");
	return 0;
}

int parse_wifictrl_res(u_char *recv_buff, ir_study_res_t *res)
{
	if(NULL == recv_buff){
			return -1;
	}

	/*remote_header_t *header = (remote_header_t *)res;
	pass_common_header(recv_buff, header);
	int index = sizeof(remote_header_t);

	u_char sb[4];
	memcpy(sb, recv_buff+index, sizeof(sb));
	int32_t sessionid = btoi(sb, sizeof(sb));//j_a2i(sb, sizeof(sb), 0);
	res->session_id = sessionid;//ntohl(sessionid);
	index += sizeof(sb);

	res->device_type = recv_buff[index++];

	u_char rb[4];
	memcpy(rb, recv_buff+index, sizeof(rb));
	int32_t reportid = btoi(rb, sizeof(rb));//j_a2i(rb, sizeof(rb), 0);
	res->report_id = reportid;
	index += sizeof(rb);

	u_char gb[4];
	memcpy(gb, recv_buff+index, sizeof(gb));
	int32_t gatewayid = btoi(gb, sizeof(gb));//j_a2i(gb, sizeof(gb), 0);
	res->gateway_id = gatewayid;//ntohl(gatewayid);
	index += sizeof(gb);

	res->channel = recv_buff[index++];

	u_char fb[2];
	memcpy(fb, recv_buff+index, sizeof(fb));
	int16_t fcode = btos(fb, sizeof(fb));
	res->function_code = fcode;
	index += sizeof(fb);

	res->res_code = recv_buff[index];*/

	memcpy(res, recv_buff, sizeof(ir_study_res_t));
	res->kic_head.session_id = ntohl(res->kic_head.session_id);
	res->kic_head.home_device_id = ntohl(res->kic_head.home_device_id);
	res->gateway_id = ntohl(res->gateway_id);
	res->function_code = ntohs(res->function_code);

	return 0;
}


int fill_stophostnet_res(u_char *send_buff, stop_host_connect_res_t *res)
{
	if (NULL == send_buff || NULL == res){
		return -1;
	}

	//remote_header_t * header = (remote_header_t *)send_buff;
	//copy_common_header(header, &(res->header));
	/*memcpy(send_buff, &(res->header), sizeof(remote_header_t));

	int index = sizeof(remote_header_t);

	int32_t sessionid = htonl(res->session_id);
	memcpy(send_buff+index, &sessionid, sizeof(sessionid));
	index+=sizeof(sessionid);

//	int32_t reportid = htonl(res->report_id);
//	memcpy(send_buff+index, &reportid, sizeof(reportid));
//	index+=sizeof(reportid);

	send_buff[index++] = res->res_code;

	memset(send_buff+index, 0, sizeof(res->space));*/

	res->session_id = htonl(res->session_id);
	memcpy(send_buff, res, sizeof(stop_host_connect_res_t));

	return 0;
}

int parse_stophostnet_req(u_char *recv_buff, stop_host_connect_req_t *req)
{
	if(NULL == recv_buff){
			return -1;
	}

	/*remote_header_t *header = (remote_header_t *)req;
	pass_common_header(recv_buff, header);
	int index = sizeof(remote_header_t);

	u_char sb[4];
	memcpy(sb, recv_buff+index, sizeof(sb));
	int32_t sessionid = btoi(sb, sizeof(sb));//j_a2i(sb, sizeof(sb), 0);
	req->session_id = sessionid;//ntohl(sessionid);
	index += sizeof(sb);

	memcpy(req->check_code, recv_buff+index, sizeof(req->check_code));*/

	memcpy(req, recv_buff, sizeof(stop_host_connect_req_t));
	req->session_id = ntohl(req->session_id);

	return 0;
}

int fill_stophostnet_req_to_host(u_char *send_buff, host_deny_connect_req_t *req)
{
        if (NULL == send_buff || NULL == req){
                return -1;
        }

	//remote_header_t * header = (remote_header_t *)send_buff;
	//copy_common_header(header, &(req->header));
	/*memcpy(send_buff, &(req->header), sizeof(remote_header_t));
	int index = sizeof(remote_header_t);

	int32_t sessionid = htonl(req->session_id);
	memcpy(send_buff+index, &sessionid, sizeof(sessionid));
	index+=sizeof(sessionid);

	int32_t reportid = htonl(req->report_id);
	memcpy(send_buff+index, &reportid, sizeof(reportid));*/

	req->session_id = htonl(req->session_id);
	req->report_id = htonl(req->report_id);
	memcpy(send_buff, req, sizeof(host_deny_connect_req_t));
        return 0;
}

int parse_stophostnet_res_from_host(u_char *recv_buff, host_deny_connect_res_t *res)
{
        if(NULL == recv_buff){
        	return -1;
        }

	/*remote_header_t *header = (remote_header_t *)res;
	pass_common_header(recv_buff, header);
	int index = sizeof(remote_header_t);

	u_char sb[4];
	memcpy(sb, recv_buff+index, sizeof(sb));
	int32_t sessionid = btoi(sb, sizeof(sb));//j_a2i(sb, sizeof(sb), 0);
	res->session_id = sessionid;//ntohl(sessionid);
	index += sizeof(sb);

	u_char rb[4];
	memcpy(rb, recv_buff+index, sizeof(rb));
	int32_t reportid = btoi(rb, sizeof(rb));////j_a2i(rb, sizeof(rb), 0);
	res->report_id = reportid;//ntohl(reportid);
	index += sizeof(rb);

	res->res_code = recv_buff[index++];*/
	memcpy(res, recv_buff, sizeof(host_deny_connect_res_t));
	res->session_id = ntohl(res->session_id);
	res->report_id = ntohl(res->report_id);
        return 0;
}

int fill_logout_res(u_char *send_buff, logout_res_t *res)
{
	if (NULL == send_buff || NULL == res){
		return -1;
	}

	//remote_header_t * header = (remote_header_t *)send_buff;
	//copy_common_header(header, &(res->header));
	/*memcpy(send_buff, &(res->header), sizeof(remote_header_t));

	int index = sizeof(remote_header_t);
	int32_t sessionid = htonl(res->session_id);
	memcpy(send_buff+index, &sessionid, sizeof(sessionid));
	index+=sizeof(sessionid);
	send_buff[index++] = res->res_code;
	memset(send_buff+index, 0, sizeof(res->space));*/

	res->session_id = htonl(res->session_id);
	memcpy(send_buff, res, sizeof(logout_res_t));
	return 0;
}

int parse_logout_req(u_char *recv_buff, logout_req_t *req)
{
	if(NULL == recv_buff){
			return -1;
	}

	/*remote_header_t *header = (remote_header_t *)req;
	pass_common_header(recv_buff, header);
	int index = sizeof(remote_header_t);
	u_char sb[4];
	memcpy(sb, recv_buff+index, sizeof(sb));
	int32_t sessionid = btoi(sb, sizeof(sb));//j_a2i(sb, sizeof(sb), 0);
	req->session_id = sessionid;//ntohl(sessionid);*/

	memcpy(req, recv_buff, sizeof(logout_req_t));
	req->session_id = ntohl(req->session_id);
	return 0;
}


int parse_device_ctrl_res(u_char *recv_buff, device_control_res *res, char *data)
{
	memcpy(res, recv_buff, sizeof(device_control_res));
	res->datapoint_id = ntohl(res->datapoint_id);
	res->session_id= ntohl(res->session_id);
	memcpy(data, res->data, (int)res->data_len);
	return 0;
}

int fill_global_error(u_char *send_buff, global_error_t *err)
{
	if (NULL == send_buff || NULL == err){
		return -1;
	}
	//remote_header_t * header = (remote_header_t *)send_buff;
	//copy_common_header(header, &(err->header));
	/*memcpy(send_buff, &(err->header), sizeof(remote_header_t));

	int index = sizeof(remote_header_t);

	int32_t sessionid = htonl(err->session_id);
	memcpy(send_buff+index, &sessionid, sizeof(sessionid));
	index+=sizeof(sessionid);

	int16_t code = htons(err->error_code);
	memcpy(send_buff+index, &code, sizeof(code));
	index+=sizeof(code);
	memset(send_buff+index, 0, sizeof(err->space));*/
	err->session_id = htonl(err->session_id);
	err->error_code = htons(err->error_code);
	memcpy(send_buff, err, sizeof(global_error_t));
	return 0;
}
