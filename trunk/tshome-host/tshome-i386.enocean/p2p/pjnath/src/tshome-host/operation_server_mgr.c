/*
 * operation_server_mgr.c
 *
 *  Created on: Feb 9, 2012
 *      Author: SongShanping
 */

#include "operation_server_mgr.h"
#include "db/ts_db_conn_infos.h"
#include "update_mgr.h"
#include "common/common_define.h"
#include "common/thread.h"

#include <netinet/in.h>
#include "snd_rcv.h"
#include <unistd.h> 
#include <sys/wait.h>

#define MD5SUM_LEN		32
#define MD5SUM_LEN_ENCRYPT	48
#define URL_MAX_LEN             256

// g_aes_key is declared in codec_mgr.c
extern aes_encode *g_aes_key;

// g_rsa_key is declared in codec_mge.c
extern RSA * g_rsa_key;

// to count the relogin times. 
int relogin_count = 0;

// the buff of report.
unsigned char server_buff[REPORT_MAX_LEN];

// to save server session id.
int32_t g_server_session_id = 0;

// to judge whether the host has logout from server.
int g_is_host_logout = FALSE;

// to judge whether the host has stoped net connection.
int g_is_net_connect_stoped = FALSE;

THREAD_MUTEX mux_lock_server_buff = PTHREAD_MUTEX_INITIALIZER;

THREAD_MUTEX mux_lock_network_stop_flag = PTHREAD_MUTEX_INITIALIZER;

THREAD_MUTEX mux_lock_host_logout_flag = PTHREAD_MUTEX_INITIALIZER;

void save_server_session_id(int32_t session_id)
{
    g_server_session_id = session_id;
}

int32_t get_server_session_id()
{
    return g_server_session_id;
}

void distroy_server_session_id()
{
    g_server_session_id = 0;
}

int ask_secret_action()
{
    ask_secret_req_t req;
    memset(&req, 0, sizeof(req));

    init_common_header(&(req.header),
                       INTERNET,
                       sizeof(ask_secret_req_t),
                       ASK_SECRET_KEY,
                       REQUEST,
                       HOST_S);

    int buff_len = sizeof(ask_secret_req_t)+LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_server_buff);

    memset(server_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(server_buff);
    u_char *temp = server_buff + LEN_CMDH_HEADER;
    fill_ask_secretkey_req(temp, &req);

    printf("%s",server_buff);

   	icedemo_send_data_to_srv(PJ_SEVER_THREAD_ID, (const char *)server_buff, buff_len);

    UNLOCK_MUTEX(&mux_lock_server_buff);
    return 0;
}

int ask_secret_handle(char *key, char *big_num)
{
    if (NULL == key || NULL == big_num){
        return -1;
    }

    init_rsa_key(key,big_num);
    
    init_aes_key();

    if (NULL == g_aes_key || NULL == g_rsa_key){
        return -1;
    }

    char aes_key_encode[RSA_TEXT_LEN];
    memset(aes_key_encode, 0, sizeof(aes_key_encode));

    rt_encrypt((const char *)(g_aes_key->key), aes_key_encode, g_rsa_key);

    if (-1 == pass_secret_action(aes_key_encode, sizeof(aes_key_encode))){
        return -1;
    }

    return 0;
}

int pass_secret_action(char* secret_key, int key_len)
{
    printf("****Into pass_secret_action *****\n");

    pass_secret_req_t req;
    memset(&req, 0, sizeof(req));

    init_common_header(&(req.header),
                       INTERNET,
                       sizeof(pass_secret_req_t),
                       PASS_SECRET_KEY,
                       REQUEST,
                       HOST_S);


    memset(req.secret_key, 0, sizeof(req.secret_key));
    memcpy(req.secret_key, secret_key, key_len);

    int len = sizeof(pass_secret_req_t) + LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_server_buff);

    memset(server_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(server_buff);
    u_char *temp = server_buff + LEN_CMDH_HEADER;
    fill_pass_secretkey_req(temp, &req);

    icedemo_send_data_to_srv(PJ_SEVER_THREAD_ID, (const char *)server_buff, len);

    UNLOCK_MUTEX(&mux_lock_server_buff);

    return 0;
}

int pass_sceret_handle(int32_t session_id)
{
    printf(">>>>>>>>>>>>>>>> Into pass_sceret_handle \n");

    save_server_session_id(session_id);
    
    printf("server_session = %d;  session_id = %d \n", g_server_session_id, session_id);

    char* name = (char*)malloc(HOST_NAME_LEN);
    if (NULL == name){
        return -1;
    }
    memset(name, 0, HOST_NAME_LEN);
    char* passwd = (char*)malloc(HOST_PWD_LEN);
    if (NULL == passwd){
        return -1;
    }
    memset(passwd, 0, HOST_PWD_LEN);

    
    printf(">>>>>>>>>>>>>>>> Before get user name \n");
    db_get_host_user_info(name, passwd);
    
    
    printf(">>>>>>>>>>>>>>>> db_get_host_user_info: name : %s \t pwd : %s \n", name,passwd);
    
    login_server_action(name, passwd);

    free(name);
    name = NULL;
    free(passwd);
    passwd = NULL;
    
    
    printf(">>>>>>>>>>>>>>>> leave from pass_sceret_handle \n");

    return 0;
}

int login_server_action(char* username, char* password)
{
    if (NULL == username || NULL == password){
        return -1;
    }
    login_req_t req;

    memset(&req, 0, sizeof(req));

    init_common_header(&(req.header),
                       INTERNET,
                       sizeof(login_req_t),
                       LOGIN,
                       REQUEST,
                       HOST_S);

    //get session id
    int32_t session = get_server_session_id();

    //encrypt
    int n_len = at_get_encrypt_len(username);
    char *name_encrypt = (char*)malloc(n_len + 1);
    if (NULL == name_encrypt){
        return -1;
    }
    memset(name_encrypt, 0, n_len + 1);
    int n_base_len = strlen(username);
    at_encrypt(username, name_encrypt, n_base_len, *g_aes_key);

    int p_len = at_get_encrypt_len(password);
    char *pwd_encrypt = (char*)malloc(p_len + 1);
    if (NULL == pwd_encrypt){
        return -1;
    }
    memset(pwd_encrypt, 0, p_len + 1);
    int p_base_len = strlen(password);
    at_encrypt(password, pwd_encrypt, p_base_len, *g_aes_key);

    memcpy(req.user_name, name_encrypt, n_len);
    memcpy(req.password, pwd_encrypt, p_len);
    req.password_len = p_base_len;
    req.username_len = n_base_len;
    req.session_id = htonl(session);


    int len = sizeof(login_req_t) + LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_server_buff);

    memset(server_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(server_buff);
    u_char *temp = server_buff + LEN_CMDH_HEADER;
    fill_login_req(temp, &req);

    icedemo_send_data_to_srv(PJ_SEVER_THREAD_ID, (const char *)server_buff, len);

    UNLOCK_MUTEX(&mux_lock_server_buff);

    free(name_encrypt);
    free(pwd_encrypt);
    
    printf(">>>>>>>>>>>>>>>> leave from login_server_action \n");

    return 0;
}

int login_server_handle(int32_t session_id, char status)
{
    
    printf(">>>>>>>>>>>>>>>> Into login_server_handle \n");
    
    int32_t server_session = get_server_session_id();
    
    printf("server_session = %d;  session_id = %d \n",server_session,session_id);

    if (server_session != session_id){
        return -1; //session no match
    }

    if (status == LOGIN_SUCCESS){

        relogin_count = 0;

        LOCK_MUTEX(&mux_lock_host_logout_flag);
        g_is_host_logout = FALSE;
        UNLOCK_MUTEX(&mux_lock_host_logout_flag);
        
        printf(">>>>>>>>>>>>>>>> In login_server_handle: Login success \n");

        update_query(QUERY_FROM_HOST);
        

    }else if (status == LOGIN_FAILED){//re-login
        if (relogin_count > 3){
            relogin_count = 0;
            
            
            printf(">>>>>>>>>>>>>>>> leave from login_server_handle: Login FAILED \n");
            
            destroy_aes_key();
            destroy_rsa_key();
            
            return -1;
        }
        relogin_count++;
    
        printf(">>>>>>>>>>>>>>>> In login_server_handle: re-login count = %d \n",relogin_count);

        destroy_aes_key();
        destroy_rsa_key();
        
        ask_secret_action();

    }else if (status == LOGIN_RELOG){
        
        printf(">>>>>>>>>>>>>>>> In login_server_handle: LOGIN_RELOG \n");
        relogin_count = 0;
    }

    return 0;
}

int update_query(int query_from)
{
    int session_id = get_server_session_id();
    int64_t latest_time = 0;
    int res = db_get_lastest_version_conf(&latest_time);

    if(res != EXIT_SUCCESS) {
		return -1;
    }

    update_req_t req;
    memset(&req, 0, sizeof(req));

    init_common_header(&(req.header),
                       INTERNET,
                       sizeof(update_req_t),
                       UPDATE_QUERY,
                       REQUEST,
                       HOST_S);

    req.latest_time = hton64(latest_time);
    req.session_id = htonl(session_id);
    req.query_from = (u_char) query_from;
    sprintf((char *) req.product_id, "%s", PRODUCT_ID);

    int len = sizeof(update_req_t) + LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_server_buff);

    memset(server_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(server_buff);
    u_char *temp = server_buff + LEN_CMDH_HEADER;
    fill_update_req(temp, &req);

    icedemo_send_data_to_srv(PJ_SEVER_THREAD_ID, (const char *)server_buff, len);

    UNLOCK_MUTEX(&mux_lock_server_buff);

    return 0;
}

/*
 * Parse updated packages url received from server
 * @param url       the url of packages to be updated from server
 * @sub_str         the sub string of the url
 * @param key_len   the len of the key we want to retrive from url
 * @param buffer    the buffer where we store the parsed key
 * @return          success or failed
 */
int parse_url (char *url, char *sub_str, int key_len, char *buffer)
{
    char *str = NULL;

    if (NULL == url || 
        NULL == buffer ||
        NULL == sub_str) {
        printf ("url or buffer or sub_str is NULL\n");
        return EXIT_FAILURE;
    }

    str = strstr (url, sub_str);
    if (NULL == str) {
        printf ("there is no key in the url:%s\n", url);
        return EXIT_FAILURE;
    }
    str += strlen(sub_str);
    strncpy (buffer, str, key_len);

    return EXIT_SUCCESS;
}


/*
 * This function is to verify whether the server suggested version
 * is in the same format with the local product version.
 * @param local_ver    the current version of this software
 * @param update_ver   the update version the server suggested
 * @return             success or failure
 */
#define TS_CONF_UPD 0
#define TS_SOFTWARE_UPD 1
int version_cmp (char *local_ver, char *update_ver,int upd_type)
{
    int local_ver_len, update_ver_len;
    int local_num[3], update_num[3];
    char chars[2];
    char *str = NULL;
    int i;

    if (NULL == local_ver ||
        NULL == update_ver) {
        printf ("local version or update version is NULL\n");
        return EXIT_FAILURE;
    }

    //compare the length of the two version
    local_ver_len = strlen (local_ver);
    update_ver_len = strlen (update_ver);
    if (local_ver_len != VERSION_LEN ||
        update_ver_len != VERSION_LEN) {
        printf ("the format of local version and update version is different\n");
        return EXIT_FAILURE;
    }

    if (strncmp (local_ver, update_ver, PRODUCT_VENDOR_LEN)) {
        printf ("The product id or vendor is not the same\n");
        return EXIT_FAILURE;
    }

    //convert the local version from string to integer
    for (i = 0; i < 3; i++) {
        str = local_ver + PRODUCT_VENDOR_LEN + 1 + i * 2;
        strncpy (chars, str, 1);
        chars[1] = '\0';
        local_num[i] = atoi (chars);
    }

    //convert the update version from string to integer
    for (i = 0; i < 3; i++) {
        str = update_ver + PRODUCT_VENDOR_LEN + 1 + i * 2;
        strncpy (chars, str, 1);
        chars[1] = '\0';
        update_num[i] = atoi (chars);
    }

    if( TS_SOFTWARE_UPD == upd_type) { 
		if ((local_num[0] > update_num[0]) ||
	        ((local_num[0] == update_num[0]) && (local_num[1] > update_num[1])) ||
	        ((local_num[0] == update_num[0]) && (local_num[1] == update_num[1]) && (local_num[2] >= update_num[2]))) {
	        printf ("the update version is smaller than local version\n");
	        return EXIT_FAILURE;
	    }
	}else if( TS_CONF_UPD == upd_type) { 
		if ((local_num[0] > update_num[0]) ||
	        ((local_num[0] == update_num[0]) && (local_num[1] > update_num[1])) ||
	        ((local_num[0] == update_num[0]) && (local_num[1] == update_num[1]) && (local_num[2] > update_num[2]))) {
	        printf ("the conf version is smaller than local version\n");
	        return EXIT_FAILURE;
	    }
	}
    
    return EXIT_SUCCESS;
}

int update_handle(update_res_t *res)
{
    int32_t server_session = get_server_session_id();
    int session_id = ntohl(res->session_id);

    if (server_session != session_id){
        return -1; //session no match
    }

    int conf_upd_mode = 0;
    char conf_url[CONF_URL_MAX_LEN] = {0};
    int file_upd_mode = 0;
    char file_url[FILE_URL_MAX_LEN] = {0};
    char md5[MD5SUM_LEN_ENCRYPT] = {0};
    char result[BLOCK_SIZE * 2 + 1] = {0};
    char cmd[1024] = {0};
	
    conf_upd_mode = res->conf_upd_mode;
    file_upd_mode = res->file_upd_mode;

    if (conf_upd_mode == NO_UPDATE){        
        log_debug(">>>>>>>>>>>>>>>> Into update_handle: CONF NO NEED UPDATE. \n");
    }else if(conf_upd_mode == RECOMMAND_UPDATE){      
        log_debug(">>>>>>>>> Into update_handle: CONF RECOMMAND TO UPDATE. \n");
    }else{
        log_debug(">>>>>>>>> Into update_handle: CONF MUST TO UPDATE. \n");
    }

    if (file_upd_mode == NO_UPDATE){        
        log_debug(">>>>>>>>>>>>>>>> Into update_handle: FILE NO NEED UPDATE. \n");
    }else if(file_upd_mode == RECOMMAND_UPDATE){      
        log_debug(">>>>>>>>> Into update_handle: FILE RECOMMAND TO UPDATE. \n");
    }else{
        log_debug(">>>>>>>>> Into update_handle: FILE MUST TO UPDATE. \n");
    }
	
    if(conf_upd_mode == NO_UPDATE && 
       file_upd_mode == NO_UPDATE) {
        return 0;
    }

    int query_from = res->query_from;
    // if it is web update request, do update if it has anything new
    if(query_from == QUERY_FROM_WEB_UPD){
        if(conf_upd_mode != NO_UPDATE){
            conf_upd_mode = MUST_UPDATE;
        }
        if(file_upd_mode != NO_UPDATE){
            file_upd_mode = MUST_UPDATE;
        }
    }

    if(conf_upd_mode == MUST_UPDATE &&
       res->conf_url_len > 0){
        //conf_url_len = at_get_encrypt_len_from_strlen(res->conf_url_len);
        at_decrypt((char *) res->conf_url, (char *)conf_url, res->conf_url_len, *g_aes_key);
        //snprintf((char *) conf_url, URL_MAX_LEN - 2, "\"%s\"", (char *) conf_url);
    } else {
        //conf_url_len = 0;
        strcpy((char *) conf_url, "null");
    }
		
    if(file_upd_mode == MUST_UPDATE &&
       res->file_url_len > 0) {
    	//file_url_len = at_get_encrypt_len_from_strlen(res->file_url_len);
        at_decrypt((char *) res->file_url, file_url, res->file_url_len, *g_aes_key);
    //    snprintf((char *) file_url, URL_MAX_LEN, "\"%s\"", (char *) file_url);
        at_decrypt((char *) res->md5, md5, MD5SUM_LEN, *g_aes_key);
		
    } else {
        //file_url_len = 0;
        strcpy((char *) file_url, "null");
        strcpy((char *) md5, "null");
    }

    if (!strcmp (file_url, "null") &&
        !strcmp (conf_url, "null")) {
        return EXIT_FAILURE;
    }
    //To verify whether the version of the new update is valid
	
    if (strcmp (file_url, "null")) {
        char buffer[VERSION_LEN+1] = {0};
        int ret = parse_url (file_url, "pro=", VERSION_LEN, buffer);
        if (ret != EXIT_SUCCESS) {
            log_debug ("Parse file url failed\n", file_url);
            return EXIT_FAILURE;
        }
        buffer[VERSION_LEN] = '\0';
        ret =  version_cmp (PRODUCT_ID, buffer,TS_SOFTWARE_UPD);
        if (ret != EXIT_SUCCESS) {
            log_debug ("Version cmp failed:local version:%s, update version:%s\n", PRODUCT_ID, buffer);
            return EXIT_FAILURE;
        }
    }
    
	if (strcmp (conf_url, "null")) {
        char buffer[VERSION_LEN+1] = {0};
        int ret = parse_url (conf_url, "pro=", VERSION_LEN, buffer);
        if (ret != EXIT_SUCCESS) {
            log_debug ("Parse config url failed\n", conf_url);
            return EXIT_FAILURE;
        }
        buffer[VERSION_LEN] = '\0';
        ret =  version_cmp (CONFIG_ID, buffer,TS_CONF_UPD);
        if (ret != EXIT_SUCCESS) {
            log_debug ("Version cmp failed:local version:%s, update version:%s\n", CONFIG_ID, buffer);
            return EXIT_FAILURE;
        }
    }

    hex_2_string((char *)g_aes_key->key, BLOCK_SIZE, (char *)result);
	char session_from[128] = {0};
	sprintf(session_from,"&sessionId=%d&from=%s",g_server_session_id,"c");
	const char * NULL_STRING = "null";
	if(0 != strcmp(conf_url,NULL_STRING)) {
		sprintf(conf_url,"%s%s",conf_url,session_from);
	}

	if(0 != strcmp(file_url,NULL_STRING)) {
		sprintf(file_url,"%s%s",file_url,session_from);
	}
    // parameters: conf_url, file_url, md5 and g_aes_key->key in string formate
    sprintf(cmd, "%s%s \"%s\" \"%s\" %s %s", 
            UPDATE_BIN_PATH, UPDATE_BIN_FILE, conf_url, file_url, md5, result);

    log_debug(">>>>>>>>> In update_handle: cmd is [%s]. \n", cmd);

    int status = system(cmd);

    if(status == -1){
        log_debug(">>>>>>>>> In update_handle:[status = -1] when system(cmd)\n");
        return EXIT_FAILURE;
    }else{
        if(WIFEXITED(status)){
        	if( 0 == WEXITSTATUS(status)){
            	log_debug(">>>>>>>>> In update_handle:[run successfully, status = 0] when system(cmd)\n");
			}else{
                log_debug(">>>>>>>>> In update_handle:[Shell statement failed, status = %d] when system(cmd)\n", WEXITSTATUS(status));
            }
        }else{
                log_debug(">>>>>>>>> In update_handle:[There's an exception, status = %d] when system(cmd)\n", WEXITSTATUS(status));
        }
    }
	
    log_debug(">>>>>>>>> Leave from config_update_handle \n");

    return 0;
}
/*
int software_update_action(int32_t session_id,
                           char big_version,
                           char small_version,
                           int client_type)
{
    
    update_software_req_t req;

    memset(&req, 0, sizeof(req));

    init_common_header(&(req.header),
                       INTERNET,
                       sizeof(update_software_req_t),
                       CHECK_SOFTWARE,
                       REQUEST,
                       HOST_S);

    req.session_id = htonl(session_id);
    req.big_version = big_version;
    req.small_version = small_version;
    req.client_type = (char)client_type;

    int len = sizeof(update_software_req_t) + LEN_CMDH_HEADER;

    memset(server_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(server_buff);
    u_char *temp = server_buff + LEN_CMDH_HEADER;
    fill_updatesoftware_req(temp, &req);

    icedemo_send_data_to_srv(PJ_SEVER_THREAD_ID, (const char *)server_buff, len);

    return 0;
}
*/

int update_issue_handle(int32_t session_id)
{
    int32_t server_session = get_server_session_id();

    if (server_session != session_id){
        return -1; //session no match
    }

	update_query(QUERY_FROM_HOST);

    return 0;
}

int stop_host_network_handle(int32_t session_id,
                             int32_t report_id, void *res, int *len)
{

    int32_t server_session = get_server_session_id();

    if (server_session != session_id){
        return -1; //session not match
    }

	//check permission
    terminal_identify_info_t *term = get_terminal(session_id);
    if (NULL == term){
        
        printf("\n>>>INTO stop_host_network_handle: have no terminal info \n");
		
        return -1;
    }
	
    int is_control = TS_SYSTEM_PERMISSION_FALSE;
    
    int en_len = at_get_encrypt_len_from_strlen(term->len_name)+1;
    char *name_decode = (char *)malloc(en_len);
    if (NULL != name_decode){
        memset(name_decode, 0, en_len);
		
        aes_encode aes_encode_key;
        at_create_key_by_string((const u_char *)(term->aes_key),
                                &aes_encode_key);
		
        at_decrypt((char *)term->user_name, name_decode,
                   term->len_name,aes_encode_key);
		
        db_is_system_control_by_name(name_decode, TS_SYSTEM_PERMISSION_REMOTE_CONTROL, &is_control);
		
        free(name_decode);
    }
	
    if (TS_SYSTEM_PERMISSION_FALSE == is_control){
		
		stop_host_network_action(session_id,report_id, NO_PERMISSION, res, len);
		
	} else {
		LOCK_MUTEX(&mux_lock_network_stop_flag);
		g_is_net_connect_stoped = TRUE;
		db_set_is_stop_net(TS_STOP_NET);
		UNLOCK_MUTEX(&mux_lock_network_stop_flag);
		
		stop_host_network_action(session_id,report_id, DENY_SUCCESS, res, len);
	}

    return 0;
}

int stop_host_network_action(int32_t session_id,
                             int32_t report_id,
							char status,
							void *response,
							int *len)
{
    host_deny_connect_res_t res;

    memset(&res, 0, sizeof(res));

    init_common_header(&(res.header),
                       INTERNET,
                       sizeof(host_deny_connect_res_t),
                       STOP_CONNECTION,
                       RESPONSE,
                       HOST_S);

    res.session_id = htonl(session_id);
    
    res.report_id = htonl(report_id);

    res.res_code = status;

    *len = sizeof(host_deny_connect_res_t) + LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_server_buff);

    memset(server_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(server_buff);

    u_char *temp = server_buff + LEN_CMDH_HEADER;
    fill_stophostnet_res(temp, &res);
    
    
    memcpy(response, server_buff, *len);


    UNLOCK_MUTEX(&mux_lock_server_buff);

    return 0;
}

int term_stop_host_net_action()
{
    //get session id
    int32_t session_id = get_server_session_id();

    //encrypt
    int n_len = at_get_encrypt_len(CHECK_CODE);
    char *code_encrypt = (char*)malloc(n_len + 1);
    if (NULL == code_encrypt){
        return -1;
    }
    memset(code_encrypt, 0, n_len + 1);
    int n_base_len = strlen(CHECK_CODE);
    at_encrypt(CHECK_CODE, code_encrypt, n_base_len, *g_aes_key);

    stop_host_connect_req_t req;

    memset(&req, 0, sizeof(req));

    init_common_header(&(req.header),
                       INTERNET,
                       sizeof(stop_host_connect_req_t),
                       STOP_CONNECTION,
                       REQUEST,
                       HOST_S);

    req.session_id = htonl(session_id);
    memcpy(req.check_code, code_encrypt, n_len);

    int len = sizeof(stop_host_connect_req_t) + LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_server_buff);

    memset(server_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(server_buff);
    u_char *temp = server_buff + LEN_CMDH_HEADER;
    fill_term_stophostnet_req(temp, &req);

    icedemo_send_data_to_srv(PJ_SEVER_THREAD_ID, (const char *)server_buff, len);

    UNLOCK_MUTEX(&mux_lock_server_buff);

    free(code_encrypt);

    return 0;
}

int term_stop_host_net_handle(int32_t session_id, char status)
{
    // client function , do nothing here.
    return 0;
}

void logout_server()
{
    logout_server_action(get_server_session_id());
    sleep(3);
}

int logout_server_action(int32_t session_id)
{
    logout_req_t req;

    memset(&req, 0, sizeof(req));

    init_common_header(&(req.header),
                       INTERNET,
                       sizeof(logout_req_t),
                       LOGOUT,
                       REQUEST,
                       HOST_S);

    req.session_id = htonl(session_id);

    int len = sizeof(logout_req_t) + LEN_CMDH_HEADER;

    LOCK_MUTEX(&mux_lock_server_buff);
    memset(server_buff, 0, REPORT_MAX_LEN);

    fill_pjsip_report_header(server_buff);
    u_char *temp = server_buff + LEN_CMDH_HEADER;
    fill_logout_req(temp, &req);
    icedemo_send_data_to_srv(PJ_SEVER_THREAD_ID, (const char *)server_buff, len);

    UNLOCK_MUTEX(&mux_lock_server_buff);

    return 0;
}

int logout_server_handle(int32_t session_id, char status)
{
    int32_t server_session = get_server_session_id();

    if (server_session != session_id){
        return -1; //session no match
    }

    if ((int)status == LOGOUT_SUCCESS){//log out success
        LOCK_MUTEX(&mux_lock_host_logout_flag);
        g_is_host_logout = TRUE;
        UNLOCK_MUTEX(&mux_lock_host_logout_flag);
        
        destroy_aes_key();
        destroy_rsa_key();
        distroy_server_session_id();

    }else{
        // do nothing.
    }

    return 0;
}

