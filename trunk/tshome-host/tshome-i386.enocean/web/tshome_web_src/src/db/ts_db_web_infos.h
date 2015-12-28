/*
 * ts_db_web_infos.h
 *
 *	apt-get install libxml2
 *  Created on: 2012-3-21
 *      Author: pangt
 */

#ifndef TS_DB_WEB_INFOS_H_
#define TS_DB_WEB_INFOS_H_

#include "ts_db_protocol_infos.h"

#define TS_WEB_VALIDATE_TRUE 0
#define TS_WEB_VALIDATE_FALSE 1

#define TS_WEB_ON_OFF_GATEWAY 1
#define TS_WEB_ON_OFF_SERIAL 2
#define TS_WEB_ON_OFF_INFRARED 3

#define TS_WEB_NULL -1

#define TS_WEB_NET_LINE 1
#define TS_WEB_NET_WIFI 0

#define TS_WEB_DHCP 1
#define TS_WEB_NOT_DHCP 0

#define TS_PAGE_TYPE "guide_type"
#define TS_PAGE_TYPE_ID_NOTYPE -1
#define TS_PAGE_TYPE_ID_GUIDE 1
#define TS_PAGE_GUIDE "guide_type=1"

#define TS_LEFT_GUIDE_CURR_STATE 0
#define TS_LEFT_GUIDE_GUIDE 1
#define TS_LEFT_GUIDE_BASIC_CTRL 2
#define TS_LEFT_GUIDE_SERIAL_INFO 3
#define TS_LEFT_GUIDE_PATHWAY 4
#define TS_LEFT_GUIDE_SERIAL_DEVICE 5
#define TS_LEFT_GUIDE_INFRARED 6

#define TS_IS_FIRST_USE 1
#define TS_IS_FIRST_USE_NOT 0

#define TS_WEB_CAN_DHCP 1
#define TS_WEB_CAN_NOT_DHCP 0
#define TS_WEB_CAN_DNS 1
#define TS_WEB_CAN_NOT_DNS 0

#define TS_WEB_DEFAULT_USER "admin"
#define TS_WEB_DEFAULT_PASSWD "admin"
#define TS_WEB_DEFAULT_LOGIN_USER "tiansu"
#define TS_WEB_DEFAULT_LOGIN_PASSWD "tiansu"
#define TS_WEB_DEFAULT_IP "192.168.1.100"
#define TS_WEB_DEFAULT_GW "192.168.1.1"
#define TS_WEB_DEFAULT_NM "255.255.255.0"
#define TS_WEB_DEFAULT_SERVER_NAME "www.ts-home.cn"
#define TS_WEB_DEFAULT_SERVER_PORT 34500

#define TS_WEB_GET_NO_NET_ADDR "0.0.0.0"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _base_user_info
{
	char user_name[32];
	char passwd[16];
	char ip[32];
	int32_t port;
	char login_user_name[32];
	char login_passwd[32];
	char gw[32];
	char net_mask[32];
	int32_t who_am_i;
	char server_name[256];
	int32_t server_port;
	int32_t is_first_use;
} ts_host_base_user_info;

//login out jump
#define TS_WEB_LOGIN_OUT fprintf(cgiOut, "				<a href=\"login.html\" style=\"position:absolute;top:6px;right:5px;font-size:14px;color:#fff\" >注销</a>\n");\

//login failed
#define TS_LOGIN_FAILED char * login_ip = getenv("REMOTE_ADDR");\
int32_t is_pass = TS_WEB_VALIDATE_FALSE;\
int32_t ret1 = ts_web_login_validate(login_ip, &is_pass);\
if ((ret1!=EXIT_SUCCESS)||(TS_WEB_VALIDATE_FALSE == is_pass)) {\
	fprintf(cgiOut, "<meta http-equiv=\"Refresh\" content=\"0;URL=../cgi-bin/login.html\">\n");\
	return EXIT_FAILURE;\
}

//fprintf(cgiOut, "									<td width=\"135\"><span class=\"home_ip_show home_note_ip\">%s</span><input name=\"ip\" type=\"text\" class=\"home_ip_edit home_ip\" style=\"display:none\"/></td>\n", net_info.ip);

#define TS_IP_UNIT_COUNT 4

/**
 *desc: get host base info
 *@param : user_info : user information
 *return : EXIT_SUCCESS : run success ,EXIT_FAILED : run failed
 * */
int32_t db_get_base_user_info(ts_host_base_user_info * user_info);

/**
 *desc: set host base info
 *@param : user_info : user information
 *return : EXIT_SUCCESS : run success ,EXIT_FAILED : run failed
 * */
int32_t db_set_base_user_info(ts_host_base_user_info user_info);

/**
 *desc: get host base info
 *@param : user_info : user name
 *@out param : pass : whether pass
 *return : EXIT_SUCCESS : run success ,EXIT_FAILED : run failed
 * */
int32_t db_web_validate_user(ts_host_base_user_info user_info, int8_t * pass);

typedef struct _ts_infrared_device
{
	int32_t id;
	char device_name[50];
	int32_t channel;
} ts_infrared_device;

typedef struct _ts_infrared_device_list
{
	struct list_head list;
	ts_infrared_device infr_device_info;
} ts_infrared_device_list;

int32_t db_web_get_infr_device_infos(struct list_head * head_list, ts_infrared_device_list * infos, int32_t *counts);
int32_t db_web_set_infr_device_info(ts_infrared_device info);

/**
 * get all gateways
 */
int32_t db_web_get_all_gateways(struct list_head * head_list, ts_gateway_list_t * gateways, int32_t * counts);

/**
 * set host interface
 */
int32_t db_web_set_host_interface(ts_host_interface_t host_interface);

/**
 * del host interface
 */
int32_t db_web_del_host_interface(int32_t id);

/**
 * delete passageways by interface_id
 */
int32_t db_web_del_host_inter_gateway(int32_t interface_id);

int32_t db_web_insert_host_inter_gateway(int32_t interface_id, int32_t gateway_id);

typedef struct _ts_web_host_serial_device_info
{
	int32_t id;
	char device_name[64];
	char room_name[64];
	char host_info[32];
} ts_web_host_serial_device_info;

typedef struct _ts_web_host_device_info_list
{
	struct list_head list;
	ts_web_host_serial_device_info dev_info;
} ts_web_host_device_info_list;

/**
 * query all serial info which connect with host instead of gateway
 */
int32_t db_web_get_host_device_infos(struct list_head * head_list, ts_web_host_device_info_list * infos, int32_t *counts);

int32_t db_web_set_host_device_info(ts_web_host_serial_device_info info);

typedef struct _t_host_net_info
{
	int32_t type;
	char ip[32];
	int32_t port;
	char netmask[32];
	char gw[32];
	int32_t is_dhcp;
	char name[32];
	char ssid[32];
	char login_passwd[32];
	char dns[32];
} ts_host_net_info;
/**
 *get host net info
 */
int32_t db_web_get_host_net(char *name, ts_host_net_info *net_info);

int32_t db_web_set_host_net(ts_host_net_info net_info);

/**
 * get net infos
 */
int ts_net_get_net_infos(char *ip, char *zwym, char*brdaddr, char *mac, char * net_name);

/**
 * get dns
 */
int32_t ts_web_get_dns(char *ip);

/**
 * get net infos;
 */
int ts_net_get_gateway(char *ip, char * net_name);

/**
 * insert login info
 */
int32_t ts_web_login_add(char * ip);

/**
 * del login info
 */
int32_t ts_web_login_del(char * ip);

/**
 * validate login info
 */
int32_t ts_web_login_validate(char * ip, int32_t * is_login);

int32_t ts_send_data(web_action_report_t * act_ctrl);

#define TS_ENV_TYPE_X86 0
#define TS_ENV_TYPE_ARM 1

int32_t ts_is_running(char * name, int32_t * is_exits);

#ifdef __cplusplus
}
#endif

#endif /* TS_DB_WEB_INFOS_H_ */
