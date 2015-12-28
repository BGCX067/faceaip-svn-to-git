/*
 * operation_server_mgr.h
 *
 *  Created on: Feb 9, 2012
 *      Author: SongShanping
 */

#ifndef OPERATION_MGR_H_
#define OPERATION_MGR_H_

#include "report_mgr.h"
#include "codec_mgr.h"
#include "start_up.h"
/*
 * handle actions of server
 */

void save_server_session_id(int32_t session_id);

int32_t get_server_session_id();

void distroy_server_session_id();

/*
 * send the report of ask_rsa_secret
 */
int ask_secret_action();

/*
 * deal with the response of ask_rsa_secret
 */
int ask_secret_handle(char *key, char *big_num);

/*
 * pass the aes code to server
 */
int pass_secret_action(char* secret_key, int len);

/*
 * deal with the response of pass_aes_code
 */
int pass_sceret_handle(int32_t session_id);

/*
 * send login info to server
 */
int login_server_action(char* username, char* password);

/*
 * deal with the report of login
 */
int login_server_handle(int32_t session_id, char status);

/*
 * send the request to update config file.
 */
int update_query(int query_from);

/*
 * deal with the config file url to download file.
 */
int update_handle(update_res_t *res);

/*
 * send the request to update sofeware.
 */
// int software_update_action(int32_t  session_id,
//                           char big_version,
//                           char small_version,
//                           int client_type);
/*
 * deal with the software url to download file.
 */
int update_issue_handle(int32_t session_id);

/*
 * deal with the request to stop host net conn from server. 
 */
int stop_host_network_action(int32_t  session_id,int32_t report_id, char status,
                             void *response, int *len);

/*
 * send the response to server
 */
int stop_host_network_handle(int32_t session_id,int32_t report_id, void *res, int *len);

/*
 * the action that stop host network connection from terminal.
 */
int term_stop_host_net_action();

/*
 * deal with the report of stop host network connection from terminal.
 */
int term_stop_host_net_handle(int32_t session_id, char status);

/*
 * terminal log out from server.
 */
int logout_server_action(int32_t session_id);

/*
 * deal with the response of log out .
 */
int logout_server_handle(int32_t session_id, char status);


/*
 * host log out from server .
 */
void logout_server();

#endif /* OPERATION_MGR_H_ */
