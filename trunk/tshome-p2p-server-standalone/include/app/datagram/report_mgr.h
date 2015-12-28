/*
 * report_mgr.h
 *
 *  Created on: Feb 7, 2012
 *      Author: SongShanping
 */

#ifndef REPORT_MGR_H_
#define REPORT_MGR_H_

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <app.h>
#include <string.h>

//void icedemo_send_data_to_srv (const unsigned char *data);

/*
 * tools
 */
#ifdef __cplusplus
extern "C" {
#endif

void copy_common_header(remote_header_t *des, remote_header_t *source);

//void copy_knx_ctrl_report(knx_control_report_t *des, knx_control_report_t *source);

void init_common_header(remote_header_t *header,
						int protocol_type,
						int protocol_ver,
						int length,
						int fun_code,
						int comm_type,
						int source_code);


/*
 * Report process
 * pass receive message buff
 * fill send message buff
 */
//common header
int pass_common_header(u_char *recv_buff, remote_header_t *res);

// heartbeat
int fill_heartbeat_res(u_char *send_buff, heartbeat_res_t *res);

int fill_heartbeat_req(u_char *send_buff, heartbeat_req_t *req);

int parse_heartbeat_req(u_char *recv_buff, heartbeat_req_t *req);

int parse_heartbeat_res(u_char *recv_buff, heartbeat_res_t *res);


// ask secret key
int fill_ask_secretkey_res(u_char *send_buff, ask_secret_res_t *res);

int parse_ask_secretkey_req(u_char *recv_buff, ask_secret_req_t *req);

//pass secret key
int fill_pass_secretkey_res(u_char *send_buff, pass_secret_res_t *res);

int parse_pass_secretkey_req(u_char *recv_buff, pass_secret_req_t *req);

//log in server
int fill_login_res(u_char *send_buff, login_res_t *res);

int parse_login_req(u_char *recv_buff, login_req_t *req);

//push update
int fill_push_update(u_char *send_buff, update_issue_req_t *req);

//update configuration
int fill_updateconfig_res(u_char *send_buff, update_res_t *res, aes_encode aes);

int parse_updateconfig_req(u_char *recv_buff, update_req_t *req);


/*//device control
int fill_knxctrl_req(u_char **send_buff, knx_control_report_t *req);

int parse_knxctrl_res(u_char *recv_buff, knx_control_report_t **res);*/

int fill_wifictrl_req(u_char *send_buff, ir_study_req_t *req);

int parse_wifictrl_res(u_char *recv_buff, ir_study_res_t *res);

//stop host net connection
int fill_stophostnet_res(u_char *send_buff, stop_host_connect_res_t *res);

int parse_stophostnet_req(u_char *recv_buff, stop_host_connect_req_t *req);

int fill_stophostnet_req_to_host(u_char *send_buff, host_deny_connect_req_t *req);
int parse_stophostnet_res_from_host(u_char *recv_buff, host_deny_connect_res_t *res);
//log out
int fill_logout_res(u_char *send_buff, logout_res_t *res);

int parse_logout_req(u_char *recv_buff, logout_req_t *req);

int parse_device_ctrl_res(u_char *recv_buff, device_control_res *res, char *data);

int fill_global_error(u_char *buff, global_error_t *err);
#ifdef __cplusplus
}
#endif

#endif /* REPORT_MGR_H_ */
