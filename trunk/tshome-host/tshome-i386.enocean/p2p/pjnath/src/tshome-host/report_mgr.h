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
#include "common/remote_def.h"
#include "common/common_define.h"
#include <string.h>

/*
 * tools
 */

short btos(const u_char bs[], int len);

int btoi(const u_char bs[], int len);

int64_t btoll(const u_char bs[], int len);

unsigned long long ntoh64(const unsigned long long no);

unsigned long long hton64(const unsigned long long ho);

/*end*/

// copy common header data
void copy_common_header(remote_header_t *des, remote_header_t *source);

// copy knx ctrl report data
void copy_knx_ctrl_report(knx_control_report_t *des, knx_control_report_t *source);

// init common header
int init_common_header(remote_header_t *header,
                        int protocol_type,
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
int pass_common_header(u_char *recv_buff, remote_header_t **res);

//pjsip report header
int fill_pjsip_report_header(u_char *send_buff);

// ask secret key
int fill_ask_secretkey_req(u_char *send_buff, ask_secret_req_t *req);

int parse_ask_secretkey_res(u_char *recv_buff, ask_secret_res_t **res);

int fill_term_ask_secretkey_res(u_char *send_buff, ask_secret_res_t *res);

//pass secret key
int fill_pass_secretkey_req(u_char *send_buff, pass_secret_req_t *req);

int parse_pass_secretkey_res(u_char *recv_buff, pass_secret_res_t **res);

int fill_term_pass_secretkey_res(u_char *send_buff, pass_secret_res_t *res);

int parse_term_pass_secretkey_req(u_char *recv_buff, pass_secret_req_t **req);

//log in server
int fill_login_req(u_char *send_buff, login_req_t *req);

int parse_login_res(u_char *recv_buff, login_res_t **res);

int fill_term_login_res(u_char *send_buff, login_res_t *res);

int parse_term_login_req(u_char *recv_buff, login_req_t **req);

//update configuration and software
int fill_update_req(u_char *send_buff, update_req_t *req);

int parse_update_res(u_char *recv_buff, update_res_t **res);

/*
//update software
int fill_updatesoftware_req(u_char *send_buff, update_software_req_t *req);
*/

int parse_update_issue_req(u_char *recv_buff,
                             update_issue_req_t **req);


//device control (terminal->host)
int fill_knxctrl_res(u_char *send_buff, knx_control_report_t *res);

int parse_knxctrl_req(u_char *recv_buff, knx_control_report_t **req);

int fill_wifictrl_res(u_char *send_buff, wifi_control_res_t *res);

int parse_wifictrl_req(u_char *recv_buff, wifi_control_req_t **req);

int parse_scenectrl_req(u_char *recv_buff, scene_control_report_t **req);

int fill_serial_initiative_res(u_char *send_buff, serial_initiative_res_t *res);

int fill_ir_study_res(u_char *send_buff, ir_study_res_t *res);

int parse_ir_study_req(u_char *recv_buff, ir_study_req_t **req);

//stop host net connection
int fill_stophostnet_res(u_char *send_buff, host_deny_connect_res_t *res);

int parse_stophostnet_req(u_char *recv_buff, host_deny_connect_req_t **req);

int fill_term_stophostnet_req(u_char *send_buff, stop_host_connect_req_t *req);

int parse_term_stophostnet_res(u_char *recv_buff, stop_host_connect_res_t **res);

//log out
int fill_logout_req(u_char *send_buff, logout_req_t *req);

int parse_logout_res(u_char *recv_buff, logout_res_t **res);

int fill_term_logout_res(u_char *send_buff, logout_res_t *res);

int parse_term_logout_req(u_char *recv_buff, logout_req_t **req);

//pass terminal session id to host
int fill_pass_term_session_res(u_char *send_buff, pass_term_session_res_t *res);

int parse_pass_term_session_req(u_char *recv_buff,
                                pass_term_session_req_t **req);

// global error
int parse_global_error(u_char *recv_buff,
        global_error_t **res);

void printKnx(knx_control_report_t *req);

#endif /* REPORT_MGR_H_ */
