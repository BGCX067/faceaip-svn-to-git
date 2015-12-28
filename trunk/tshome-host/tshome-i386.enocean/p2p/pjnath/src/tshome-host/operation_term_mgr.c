/*
 * operation_term_mgr.c
 *
 *  Created on: Feb 9, 2012
 *      Author: SongShanping
 */

#include "operation_term_mgr.h"
#include "common/thread.h"
#include "term_conn_mgr.h"
#include "db/ts_db_conn_infos.h"
#include "encode/md5_tools.h"

#include <netinet/in.h>
#include "snd_rcv.h"
#include <unistd.h> 

THREAD_MUTEX mux_lock_term_session = PTHREAD_MUTEX_INITIALIZER;

THREAD_MUTEX mux_lock_term_buff = PTHREAD_MUTEX_INITIALIZER;

extern RSA * g_rsa_decode_key;
unsigned char term_buff[REPORT_MAX_LEN];
unsigned int g_terminal_base_session_id = 11111;


int get_new_terminal_session_id()
{
    LOCK_MUTEX(&mux_lock_term_session);

    ++g_terminal_base_session_id;

    UNLOCK_MUTEX(&mux_lock_term_session);
    return g_terminal_base_session_id;
}


int term_ask_rsa_secret_handle(unsigned k,
		unsigned comp_id,
		PROTOCOL_TYPE net_type)
{
    printf(">>>>In term_ask_rsa_secret_handle\n");
    //init rsa key
    if (NULL == g_rsa_decode_key){
        init_rsa_decode_key();
    }
    char *key = rt_get_rsa_publicKey(g_rsa_decode_key);
    char *big_num = rt_get_rsa_bigNum(g_rsa_decode_key);

    //transfer rsa key
    ask_secret_res_t res;
    memset(&res, 0, sizeof(res));

    init_common_header(&(res.header),
                       net_type,
                       sizeof(ask_secret_res_t),
                       ASK_SECRET_KEY,
                       RESPONSE,
                       HOST_S);

    memcpy(res.secret_key, key, sizeof(res.secret_key));
    memcpy(res.secret_mode, big_num, sizeof(res.secret_mode));

    int len = sizeof(ask_secret_res_t)+LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_term_buff);

    memset(term_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(term_buff);
    u_char *temp = term_buff + LEN_CMDH_HEADER;
    fill_term_ask_secretkey_res(temp, &res);

    printf(">>>term_ask_rsa_secret_handle:%s\n",term_buff);

    icedemo_send_data(k, comp_id ,term_buff, len);

    UNLOCK_MUTEX(&mux_lock_term_buff);

    return 0;
}

int term_pass_aes_secret_handle(unsigned k,
		unsigned comp_id,
        PROTOCOL_TYPE net_type,
        char *aes_key_buff)
{
    
    printf(">>>>In term_pass_aes_secret_handle\n");
    //init sessionid
    int session_id = get_new_terminal_session_id();

    if (NULL == g_rsa_decode_key){
        init_rsa_decode_key();
    }

    char aes_decode[RSA_TEXT_LEN] = {0};
    rt_decrypt(aes_key_buff, aes_decode, g_rsa_decode_key);

    //save terminal info
    terminal_identify_info_t term_info;
    memset(&term_info, 0, sizeof(term_info));
    term_info.session_id = session_id;
    memcpy(term_info.aes_key, aes_decode, sizeof(aes_decode));
    term_info.comp_id = comp_id;
    term_info.p_k = k;
    term_info.net_type = net_type;

    add_terminal(&term_info);

    //pass session id to terminal
    pass_secret_res_t res;
    memset(&res, 0, sizeof(res));

    init_common_header(&(res.header),
                       net_type,
                       sizeof(pass_secret_res_t),
                       PASS_SECRET_KEY,
                       RESPONSE,
                       HOST_S);

    res.session_id = htonl(session_id);

    int len = sizeof(pass_secret_res_t)+LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_term_buff);

    memset(term_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(term_buff);
    u_char *temp = term_buff + LEN_CMDH_HEADER;
    fill_term_pass_secretkey_res(temp, &res);

    printf("%s",term_buff);

    icedemo_send_data(k, comp_id ,term_buff, len);

    UNLOCK_MUTEX(&mux_lock_term_buff);

    return 0;
}

int term_login_handle(unsigned k,
		unsigned comp_id,
		PROTOCOL_TYPE net_type,
		login_req_t * req)
{
    if (NULL == req){
        return -1;
    }
    
    
    printf(">>>>In term_login_handle\n");

    LOGIN_STATUS status;

    //search terminal by session id
    terminal_identify_info_t * term_info = get_terminal(ntohl(req->session_id));
    if (NULL == term_info){
        status = LOGIN_FAILED;
    }else{
        
        char *l_user_name = (char*)malloc(sizeof(req->user_name)+1);
        memset(l_user_name, 0, sizeof(req->user_name)+1);
        memcpy(l_user_name, req->user_name, sizeof(req->user_name));
        
        char *l_password = (char*)malloc(sizeof(req->password)+1);
        memset(l_password, 0, sizeof(req->password)+1);
        memcpy(l_password, req->password, sizeof(req->password));
        
        //check log in
		//check user name & password
		int success = FAILED;

		int name_en_len = at_get_encrypt_len_from_strlen((int)req->username_len)+1;
		int pwd_en_len = at_get_encrypt_len_from_strlen((int)req->password_len)+1;
		
		char *name_decode = (char *)malloc(name_en_len);
		char *pwd_decode = (char *)malloc(pwd_en_len);
		if (NULL != name_decode && NULL != pwd_decode){
			memset(pwd_decode, 0, pwd_en_len);
			memset(name_decode, 0, name_en_len);

			aes_encode aes_encode_key;
			at_create_key_by_string((const u_char *)(term_info->aes_key),
									&aes_encode_key);

			at_decrypt(l_user_name, name_decode,
					   req->username_len,aes_encode_key);
			at_decrypt(l_password, pwd_decode,
					   req->password_len,aes_encode_key);

			char *md5_pwd = (char*)malloc(MD5_LEN+1);
			if (NULL != md5_pwd) {
				
				memset(md5_pwd, 0 , MD5_LEN+1);
				
				db_get_passwd_by_name(name_decode, md5_pwd);
				
				int is_validate = MD5_VALI_FALSE;
				
				mt_valid(md5_pwd, pwd_decode, &is_validate);
				
				success = is_validate==MD5_VALI_TRUE?SUCCESS:FAILED;
				
				free(md5_pwd);
				md5_pwd = NULL;
			}
		}

		free(name_decode);
		free(pwd_decode);

		//deal with log_in result
		if (SUCCESS == success){
			status = LOGIN_SUCCESS;
			
			// delete old login info by the same username
			terminal_identify_info_t * select_term = get_terminal_by_name_pwd(req->user_name,req->password);
			
			if (NULL != select_term && select_term->session_id != ntohl(req->session_id)) {
				remove_terminal(select_term->session_id);
			}
			
			printf(">>>>In term_login_handle: LOGIN_SUCCESS\n");

			term_info->len_name = (int)req->username_len;
			term_info->len_pwd = (int)req->password_len;
			memcpy(term_info->user_name, req->user_name,
				   sizeof(req->user_name));
			memcpy(term_info->passwd, req->password,
				   sizeof(req->password));

		}else{
			status = LOGIN_FAILED;
			
			printf(">>>>In term_login_handle: LOGIN_FAILED\n");

			remove_terminal(ntohl(req->session_id));
		}
	
        
        free(l_user_name);
        free(l_password);
    }
    
    //response to terminal
    login_res_t res;
    memset(&res, 0, sizeof(res));

    init_common_header(&(res.header),
                       net_type,
                       sizeof(login_res_t),
                       LOGIN,
                       RESPONSE,
                       HOST_S);

    res.session_id = req->session_id;
    res.status = (u_char)status;

    int len = sizeof(login_res_t)+LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_term_buff);

    memset(term_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(term_buff);
    u_char *temp = term_buff + LEN_CMDH_HEADER;
    fill_term_login_res(temp, &res);
    
    icedemo_send_data(k, comp_id ,term_buff, len);

    UNLOCK_MUTEX(&mux_lock_term_buff);

    return 0;
}

int term_logout_handle(unsigned k,
		unsigned comp_id,
		PROTOCOL_TYPE net_type,
		int32_t session_id)
{
    LOGOUT_STATUS status = LOGOUT_SUCCESS;

    //delete the terminal searched by session id
    terminal_identify_info_t * term_info = get_terminal(session_id);
    if (NULL != term_info){
        remove_terminal(session_id);
    }

    logout_res_t res;
    memset(&res, 0, sizeof(res));

    init_common_header(&(res.header),
                       net_type,
                       sizeof(logout_res_t),
                       LOGOUT,
                       RESPONSE,
                       HOST_S);

    res.session_id = htonl(session_id);
    res.res_code = (u_char)status;

    int len = sizeof(logout_res_t)+LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_term_buff);

    memset(term_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(term_buff);
    u_char *temp = term_buff + LEN_CMDH_HEADER;
    fill_term_logout_res(temp, &res);

    printf("%s",term_buff);

    icedemo_send_data(k, comp_id ,term_buff, len);

    UNLOCK_MUTEX(&mux_lock_term_buff);
	
	printf(">>>>In term_logout_handle: LOGOUT_SUCCESS");

    return 0;
}
