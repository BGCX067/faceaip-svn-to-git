/*
 * IPCProtocolManager.c
 *
 *  Created on: 2012-2-7
 *      Author: sunzq
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/types.h>
#include <arpa/inet.h>
#include <time.h>
#include "app.h"
#include "../Logs.h"

#define THIS_FILE "IPCProtocolManager.c"

//#define  DEBUG

//static char *method;
//static char *msg;

/********************static function prototype*******************************/


static int check_params(const u_char *recv_buff, void *req);
/*
 *IPC protocol datagram processor
 *@fill a send datagram buffer
 *@parse a receive datagram buffer
 */
// fill a IPC protocol header
static int fill_ipc_comm_header(u_char  *send_buff, const IPC_HEADER  *header);

//parse a IPC protocol header
//static int parse_ipc_comm_header(const  u_char *recv_buff, IPC_HEADER **header);

// parse a IPC host service request datagram buffer from web-subsystem
static int parse_ipc_host_serv_req(aes_encode aes, const u_char *recv_buff, IPC_DG_HOST_SERV **req);

//fill a IPC host service response datagram buffer to web-subsystem
static int fill_ipc_host_serv_res(aes_encode aes, u_char *send_buff, IPC_DG_HOST_SERV *res);

//parse a IPC notify update request datagram buffer from web-subsystem
static int parse_ipc_notify_update_req(aes_encode aes, const u_char *recv_buff, IPC_DG_NOTIFY_UPDATE **req);

//static int fill_ipc_notify_update_res(ts_ipc_server *srv, u_char *send_buff, IPC_DG_NOTIFY_UPDATE_RES *res);

//parse a IPC download configuration response from web-subsystem
static int parse_ipc_check_update_res(aes_encode aes, const u_char *recv_buff, IPC_DG_CHECK_UPDATE_RES **res);

// fill a IPC download configuration request to web-subsystem
static int fill_ipc_check_update_req(aes_encode aes, u_char *send_buff, IPC_DG_CHECK_UPDATE_REQ *req);

//parse a IPC ask udp-subsystem run states request from web-subsystem
static int parse_ipc_ask_udpsystate_req(const u_char *recv_buff, IPC_DG_ASK_UDPSYS_REQ **req);

//fill a IPC ask udp-subsystem run states response to web-subsystem
static int fill_ipc_ask_udpsystate_res(u_char *send_buff, const IPC_DG_ASK_UDPSYS_RES *res);

//parse a IPC control udp-subsystem request from web-subsystem
static int parse_ipc_ctrl_udpsys_req(const u_char *recv_buff, IPC_DG_CTRL_UDPSYS **req);

//parse a IPC infrared learning request from web-subsystem
static int parse_ipc_inf_learn_req(aes_encode aes, const u_char *recv_buff, IPC_DG_INF_LEARN **req);

//fill a IPC infrared learning response to web-subsystem
static int fill_ipc_inf_learn_res(aes_encode aes, u_char *send_buff, IPC_DG_INF_LEARN *res);



/**************************public function*************************************/

u_char ipc_protocol_recv_handler(aes_encode aes, const u_char *recv_buff, void **structs)
{
	u_char type = UNKNOWN; // return value

	int re = check_params(recv_buff, structs);
	if(re != 0){
		//return re;
		return UNKNOWN;
	}
	//must be a IPC_HEADER which is  a IPC protocol structs`s field type point
	//IPC_DG_ASK_UDPSYS_REQ *p_header = (IPC_HEADER*) structs;
	//IPC_HEADER **p = (IPC_HEADER **)structs;
	//parse_ipc_comm_header(recv_buff, p);

	memcpy(*structs, recv_buff, sizeof(IPC_HEADER));
	IPC_HEADER *pHeader = (IPC_HEADER *)recv_buff;
	
	type = pHeader->function;//(*p)->function;
	switch(type){
	case IPC_HEARTBEAT:
		//do nothing
		break;
	case HOST_SERV_REQ:
		parse_ipc_host_serv_req(aes, recv_buff, (IPC_DG_HOST_SERV **)structs);
		break;
	case NOTIFY_UPDATE:
		parse_ipc_notify_update_req(aes, recv_buff, (IPC_DG_NOTIFY_UPDATE **)structs);
		break;
	case CHECK_UPDATE:
		//parse_ipc_down_cfg_res(srv, recv_buff, (IPC_DG_DOWN_CFG **)structs);
		parse_ipc_check_update_res(aes, recv_buff, (IPC_DG_CHECK_UPDATE_RES **)structs);
		break;
	case ASK_UDPSYS_STATES:
		parse_ipc_ask_udpsystate_req(recv_buff, (IPC_DG_ASK_UDPSYS_REQ **)structs);
		break;
	case CTRL_UDPSYS:
		parse_ipc_ctrl_udpsys_req(recv_buff, (IPC_DG_CTRL_UDPSYS **)structs);
		break;
	case INFRARED_LEARN:
		parse_ipc_inf_learn_req(aes, recv_buff, (IPC_DG_INF_LEARN **)structs);
		break;
	default:
			break;
	}

	return type;
} // end of ipc_protocol_recv_handler()

int ipc_protocol_send_handler(aes_encode aes, u_char *send_buff, const void *structs,
		const IPC_FUN_CODE fun)
{
#ifdef DEBUG
	method = "ipc_protocol_send_handler";
	msg = "ipc_protocol_send_handler called";
	MYLOG(THIS_FILE, method, LOG_DEBUG, msg)
#endif
	int re = check_params((const u_char *)send_buff, (void *)structs);
	if(re != 0){
		return re;
	}
	if(fun == UNKNOWN){
		return -1;
	}
	fill_ipc_comm_header(send_buff, (IPC_HEADER *)structs);
	switch(fun){
	case HOST_SERV_REQ:
		fill_ipc_host_serv_res(aes, send_buff, (IPC_DG_HOST_SERV *)structs);
		break;
	case NOTIFY_UPDATE:
	//	fill_ipc_notify_update_res(srv, send_buff, (IPC_DG_NOTIFY_UPDATE_RES *)structs);
		break;
	case CHECK_UPDATE:
		//fill_ipc_down_cfg_req(srv, send_buff, (IPC_DG_DOWN_CFG *)structs);
		fill_ipc_check_update_req(aes, send_buff, (IPC_DG_CHECK_UPDATE_REQ *)structs);
		break;
	case ASK_UDPSYS_STATES:
		fill_ipc_comm_header(send_buff, (IPC_HEADER *)structs);
		fill_ipc_ask_udpsystate_res(send_buff, (IPC_DG_ASK_UDPSYS_RES *)structs);
		break;
	case CTRL_UDPSYS:
		//do nothing
		break;
	case INFRARED_LEARN:
		fill_ipc_inf_learn_res(aes, send_buff, (IPC_DG_INF_LEARN *)structs);
		break;
	default:
		break;
	}
	return 1;
}

/*****************************private function*******************************/


/*
 *IPC protocol datagram processor
 *@fill a send datagram buffer
 *@parse a receive datagram buffer
 */
// fill a IPC protocol header
static int fill_ipc_comm_header(u_char  *send_buff, const IPC_HEADER  *header)
{
#ifdef DEBUG
	method = "fill_ipc_comm_header";
	msg = "fill_ipc_comm_header called";
	MYLOG(THIS_FILE, method, LOG_DEBUG, msg)
#endif
	memcpy(send_buff, header, sizeof(IPC_HEADER));
	return 1;
}

//parse a IPC protocol header
#if 0
static int parse_ipc_comm_header(const  u_char *recv_buff, IPC_HEADER **header)
{
	*header = (IPC_HEADER *)recv_buff;
	return 1;
}
#endif

// parse a IPC host service request datagram buffer from web-subsystem
static int parse_ipc_host_serv_req(aes_encode aes, const u_char *recv_buff, IPC_DG_HOST_SERV **req)
{
	//body begin index
	int index  = sizeof(IPC_HEADER);

	//raw length
	u_char homeIdLen=recv_buff[index++];
	//begin  decrypt home id
	u_char * s_decrypt = NULL;
	//encrypt length
	int e_homeIdLen = at_get_encrypt_len_from_strlen(homeIdLen);
	s_decrypt = (u_char *)calloc(e_homeIdLen+1, sizeof(u_char));
	if(NULL == s_decrypt){
		return 0;
	}
	u_char *s_encrypt = (u_char *)(recv_buff + index);
	at_decrypt((char*) s_encrypt, (char *)s_decrypt, homeIdLen, aes);
	int homeId = atoi((const char *)s_decrypt);//homeid
	(*req)->homeId = homeId;
	free(s_decrypt);
	//end decrypt
	index += e_homeIdLen;
	memcpy((*req)->userName, recv_buff+index, sizeof((*req)->userName));
	index += sizeof((*req)->userName);
	(*req)->cmd_state = recv_buff[index];
	return 1;
}

//fill a IPC host service response datagram buffer to web-subsystem
static int fill_ipc_host_serv_res(aes_encode aes, u_char *send_buff, IPC_DG_HOST_SERV *res)
{
	int index = sizeof(IPC_HEADER);
	char base_text[32];
	sprintf(base_text, "%d",res->homeId);
	int encrypt_len = 0;
	encrypt_len = at_get_encrypt_len_from_strlen(strlen(base_text));
	send_buff[index++] = (u_char)strlen(base_text);
	int unLen = sizeof(res->userName);

	//recompute body length
	res->header.length = htons(encrypt_len+1+unLen+4);
        fill_ipc_comm_header(send_buff, (IPC_HEADER *)res);
	//end recompute
	//begin encode
	u_char * s_encrypt = NULL;
	s_encrypt = (u_char *) calloc(encrypt_len, sizeof(u_char));
	if(NULL == s_encrypt){
		return 0;
	}
	at_encrypt((const char *)base_text, (char *)s_encrypt, strlen(base_text), aes);
	memcpy(send_buff + index, s_encrypt, encrypt_len);
	free(s_encrypt);
	//end encode
	index += encrypt_len;
	memcpy(send_buff+index, res->userName, unLen);
	index += unLen;
	send_buff[index++] = res->cmd_state;
	memset(send_buff+index, 0, sizeof(res->space));
	return 1;
}

//parse a IPC notify update request datagram buffer from web-subsystem
static int parse_ipc_notify_update_req(aes_encode aes, const u_char *o_recv_buff, IPC_DG_NOTIFY_UPDATE **req)
{
	u_char recv_buff[512];
	memcpy(recv_buff, o_recv_buff, 512);


	int index = sizeof(IPC_HEADER);
	u_char homeIdLen = recv_buff[index++];
	int e_len = at_get_encrypt_len_from_strlen(homeIdLen);
	//begin decrypt homeid
	u_char * s_decrypt = NULL;
	s_decrypt = (u_char *)calloc(e_len+1, sizeof(u_char));
	if(NULL == s_decrypt){
		return 0;
	}
	u_char *s_encrypt = (u_char *)(recv_buff + index);
	at_decrypt((char*) s_encrypt, (char *)s_decrypt, homeIdLen, aes);
	(*req)->homeId = atoi((const char *)s_decrypt);
	free(s_decrypt);
	//end decrypt homeid
	return 1;
}
/*
static int fill_ipc_notify_update_res(ts_ipc_server *srv, u_char *send_buff, IPC_DG_NOTIFY_UPDATE_RES *res)
{
	int index = sizeof(IPC_HEADER);
        char base_text[32];
        sprintf(base_text, "%d",res->homeId);
        int encrypt_len = 0;
        encrypt_len = at_get_encrypt_len_from_strlen(strlen(base_text));
        send_buff[index++] = (u_char)strlen(base_text);
	//recompute body length
        res->header.length = htons(encrypt_len+1+4+3);
        fill_ipc_comm_header(send_buff, (IPC_HEADER *)res);
	//end recompute
	//begin encode url
        u_char * s_encrypt = NULL;
        s_encrypt = (u_char *) calloc(encrypt_len, sizeof(u_char));
	if(NULL == s_encrypt){
		return 0;
	}
	assert(srv != NULL);
        at_encrypt((const char *)base_text, (char *)s_encrypt, strlen(base_text), srv->aes_ins);
        memcpy(send_buff + index, s_encrypt, encrypt_len);
        free(s_encrypt);
	//end encode url
        index += encrypt_len;
        int session_id = htonl(res->sessionId);
        memcpy(send_buff + index, (const char*)&session_id, sizeof(res->sessionId));
        index += sizeof(res->sessionId);
	memset(res->space, 0, sizeof(res->space));
	return 1;
}
*/
//parse a IPC download configuration response from web-subsystem
static int parse_ipc_check_update_res(aes_encode aes, const u_char *o_recv_buff, IPC_DG_CHECK_UPDATE_RES **res)
{

	u_char recv_buff[512];
	memcpy(recv_buff, o_recv_buff, 512);

	int index = sizeof(IPC_HEADER);
	u_char homeIdLen = recv_buff[index++];


	int e_homeIdLen = at_get_encrypt_len_from_strlen(homeIdLen);
	u_char * s_decrypt = NULL;
	s_decrypt = (u_char *)calloc(e_homeIdLen+1, sizeof(u_char));
	if(NULL == s_decrypt){
		return 0;
	}
	u_char *s_encrypt = (u_char *)(recv_buff + index);
	at_decrypt((char*) s_encrypt, (char *)s_decrypt, homeIdLen, aes);
	int32_t homeId = atoi((const char *)s_decrypt);
	(*res)->homeId = homeId;
	free(s_decrypt);


	index+=e_homeIdLen;
	u_char sbs[4];
	memcpy(sbs, recv_buff+index, sizeof(sbs));
	index+=4;
	int32_t sessionId = btoi(sbs, 4);
	(*res)->sessionId = sessionId;//ntohl(sessionId);
	///////////////////////////////////////
	(*res)->query_from = recv_buff[index++];
	(*res)->cfg_force = recv_buff[index++];
	int urlLen = recv_buff[index++];
	(*res)->cfg_url_len = urlLen;//*(recv_buff + (index++));
	if(urlLen > 0){
		int e_cfg_urlLen = at_get_encrypt_len_from_strlen(urlLen);

		u_char * s_decrypt_cfg_url = NULL;
		s_decrypt_cfg_url = (u_char*) calloc(e_cfg_urlLen+1, sizeof(u_char));
		if(NULL == s_decrypt_cfg_url){
			return 0;
		}
		s_encrypt = (u_char *)(recv_buff + index);
		
		at_decrypt((char*) s_encrypt, (char *)s_decrypt_cfg_url, urlLen, aes);
		memset((*res)->cfg_url,0, sizeof((*res)->cfg_url));
		memcpy((*res)->cfg_url, s_decrypt_cfg_url, e_cfg_urlLen);
		index += e_cfg_urlLen;
		free(s_decrypt_cfg_url);
		s_decrypt_cfg_url = NULL;
	}
	/////////////////////////////////////////
	(*res)->soft_force = recv_buff[index++];
	urlLen = recv_buff[index++];
	(*res)->soft_url_len = urlLen;
	if(urlLen > 0){
		int e_soft_urlLen = at_get_encrypt_len_from_strlen(urlLen);

		u_char *s_decrypt_soft_url = NULL;
		s_decrypt_soft_url = (u_char*) calloc(e_soft_urlLen+1, sizeof(u_char));
		if(NULL == s_decrypt_soft_url){
	                return 0;
	        }

		s_encrypt = (u_char *)(recv_buff + index);

		at_decrypt((char*) s_encrypt, (char *)s_decrypt_soft_url, urlLen, aes);
		memset((*res)->soft_url, 0, sizeof((*res)->soft_url));
        memcpy((*res)->soft_url, s_decrypt_soft_url, e_soft_urlLen);
        index += e_soft_urlLen;
        free(s_decrypt_soft_url);
        s_decrypt_soft_url = NULL;

		/////////////////////////////////////////
		u_char *s_decrypt_soft_md5 = (u_char*) calloc(49, sizeof(u_char));
		if(NULL == s_decrypt_soft_md5){
	                return 0;
	        }

		s_encrypt = (u_char *)(recv_buff + index);
		at_decrypt((char*) s_encrypt, (char *)s_decrypt_soft_md5, 32, aes);
		memset((*res)->soft_md5, 0, sizeof((*res)->soft_md5));
        memcpy((*res)->soft_md5, s_decrypt_soft_md5, 32);
        index += 48;
        free(s_decrypt_soft_md5);
		 s_decrypt_soft_md5 = NULL;
	}
	
	return 1;
}


// fill a IPC download configuration request to web-subsystem
static int fill_ipc_check_update_req(aes_encode aes, u_char *send_buff, IPC_DG_CHECK_UPDATE_REQ *req)
{
	int index = sizeof(IPC_HEADER);
	char base_text[32];
	sprintf(base_text, "%d",req->homeId);
	int encrypt_len = 0;
	encrypt_len = at_get_encrypt_len_from_strlen(strlen(base_text));
	send_buff[index++] = (u_char)strlen(base_text);
 	req->header.length = htons(encrypt_len + 1 + 4 + 8 + at_get_encrypt_len_from_strlen(sizeof(req->client_aes)) +
		1 + 1 + sizeof(req->product_id));
        fill_ipc_comm_header(send_buff, (IPC_HEADER *)req);
	
	//encode
	u_char * s_encrypt = NULL;
	s_encrypt = (u_char *) calloc(encrypt_len, sizeof(u_char));
	at_encrypt((const char *)base_text, (char *)s_encrypt, strlen(base_text), aes);
	memcpy(send_buff + index, s_encrypt, encrypt_len);
	free(s_encrypt);
	s_encrypt = NULL;
	index += encrypt_len;

	int session_id = htonl(req->sessionId);
	memcpy(send_buff + index, (const char*)&session_id, sizeof(req->sessionId));
	index += sizeof(req->sessionId);

	int64_t last = hton64(req->last_time);
	memcpy(send_buff+index,(const char*)&last, sizeof(req->last_time));
	index += sizeof(req->last_time);
	
	//encode client aes key
	encrypt_len = at_get_encrypt_len_from_strlen(sizeof(req->client_aes));
	s_encrypt = (u_char *) calloc(encrypt_len, sizeof(u_char));
        at_encrypt((const char *)req->client_aes, (char *)s_encrypt, sizeof(req->client_aes), aes);
        memcpy(send_buff + index, s_encrypt, encrypt_len);
        free(s_encrypt);
        index += encrypt_len;

	send_buff[index++] = req->query_from;

	//memcpy(req->product_id, "TSHome-H1-100_1.1.1", strlen("TSHome-H1-100_1.1.1"));
	send_buff[index++] = req->pid_len;
	memcpy(send_buff+index, req->product_id,  strlen((char *)req->product_id));
        LOG4CPLUS_DEBUG(LOG_TURN, "product_id("<< req->product_id << ") len("<<  strlen((char *)req->product_id) << ")"<<send);



	return 1;
}

//parse a IPC ask udp-subsystem run states request from web-subsystem
static int parse_ipc_ask_udpsystate_req(const u_char *recv_buff, IPC_DG_ASK_UDPSYS_REQ **req)
{
	return 1;
}

//fill a IPC ask udp-subsystem run states response to web-subsystem
static int fill_ipc_ask_udpsystate_res(u_char *send_buff, const IPC_DG_ASK_UDPSYS_RES *res)
{
#ifdef DEBUG
	method = "fill_ipc_ask_udpsystate_res";
	msg = "fill_ipc_ask_udpsystate_res called";
	MYLOG(THIS_FILE, method, LOG_DEBUG, msg)
#endif
	int index = sizeof(IPC_HEADER);
	send_buff[index++] = res->service;
	int64_t time =htonl(res->runnedTime);
	memcpy(send_buff+index, &time, sizeof(int64_t));
	index += sizeof(int64_t);
	send_buff[index++] = res->heart;
	int32_t m = htonl(res->m_num);
	memcpy(send_buff+index, &m, sizeof(int32_t));
	index += sizeof(int32_t);
	int32_t p = htonl(res->port_num);
	memcpy(send_buff+index, &p, sizeof(int32_t));
	index += sizeof(int32_t);
	send_buff[index++] = res->thread_num;
	send_buff[index] = 0;
	return 1;
}

//parse a IPC control udp-subsystem request from web-subsystem
static int parse_ipc_ctrl_udpsys_req(const u_char *recv_buff, IPC_DG_CTRL_UDPSYS **req)
{
	(*req)->cmd = recv_buff[sizeof(IPC_HEADER)];
	return 1;
}

//parse a IPC infrared learning request from web-subsystem
int parse_ipc_inf_learn_req(aes_encode aes, const u_char *recv_buff, IPC_DG_INF_LEARN **req)
{
	int index = sizeof(IPC_HEADER);
	u_char homeIdLen = recv_buff[index++];
	

	int e_homeIdLen = at_get_encrypt_len_from_strlen(homeIdLen);
	u_char * s_decrypt = NULL;
	s_decrypt = (u_char *)calloc(e_homeIdLen, sizeof(u_char));
	u_char *s_encrypt = (u_char *)(recv_buff + index);
	at_decrypt((char*) s_encrypt, (char *)s_decrypt, homeIdLen, aes);
	int32_t homeId = atoi((const char *)s_decrypt);
	(*req)->homeId = homeId;
	index += e_homeIdLen;
	free(s_decrypt);
	

	u_char gateways[4];
	memcpy(gateways, recv_buff+index, sizeof(gateways));
	int32_t gatewayId = btoi(gateways, sizeof(gateways));
	(*req)->gateway_id = gatewayId;
	index += sizeof(gateways);

	(*req)->channel = recv_buff[index++];

	u_char fb[2];
        memcpy(fb, recv_buff+index, sizeof(fb));
        int16_t fcode = btos(fb, sizeof(fb));
        (*req)->key = fcode;
        index += sizeof(fb);

	return 1;
}

//fill a IPC infrared learning response to web-subsystem
static int fill_ipc_inf_learn_res(aes_encode aes, u_char *send_buff, IPC_DG_INF_LEARN *res)
{
	int index = sizeof(IPC_HEADER);
	char base_text[32];
	sprintf(base_text,"%d",res->homeId);
	send_buff[index++] = strlen(base_text);
	int encrypt_len = 0;
	encrypt_len = at_get_encrypt_len_from_strlen(strlen(base_text));
	res->header.length = htons(encrypt_len+1+4+3+1);
    fill_ipc_comm_header(send_buff, (IPC_HEADER *)res);
	

	//encode
	u_char * s_encrypt = NULL;
	s_encrypt = (u_char *) calloc(encrypt_len, sizeof(u_char));
	at_encrypt((const char *)base_text, (char *)s_encrypt, strlen(base_text), aes);
	memcpy(send_buff + index, s_encrypt, encrypt_len);
	index+=encrypt_len;
	free(s_encrypt);
	

	int gatewayid = htonl(res->gateway_id);
	memcpy(send_buff+index, (const char*)&gatewayid, sizeof(res->gateway_id));
	index += sizeof(res->gateway_id);

	send_buff[index++] = res->channel;

	int16_t key = htons(res->key);
	memcpy(send_buff+index, (const char*)&key, sizeof(res->key));
	index += sizeof(res->key);

	send_buff[index] = res->result;
	return 1;
}

int check_params(const u_char *recv_buff, void *req)
{
	if(NULL == recv_buff){
			return -1;
		}
		if(NULL == req){
			return -1;
		}
	return 0;
}

