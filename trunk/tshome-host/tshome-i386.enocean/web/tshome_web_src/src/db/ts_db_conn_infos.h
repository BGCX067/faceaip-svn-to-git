/*
 * ts_db_conn_infos.h
 *
 *  Created on: 2012-2-15
 *      Author: pangt
 */

#ifndef TS_DB_CONN_INFOS_H_
#define TS_DB_CONN_INFOS_H_


#include "ts_db_protocol_infos.h"

#define TS_IS_CONTROL_TRUE 0
#define TS_IS_CONTROL_FALSE 1

#define TS_IS_USER_PASS_TRUE 0
#define TS_IS_USER_PASS_FALSE 1

#define TS_DEVICE_PERMISSION_FALSE 1
#define TS_DEVICE_PERMISSION_TRUE 0

#define TS_SYSTEM_PERMISSION_FALSE 0
#define TS_SYSTEM_PERMISSION_TRUE 1

#define TS_SERVICE_STOP_WEB 1
#define TS_SERVICE_CONF 2
#define TS_SERVICE_WEB 3

#define TS_TIME_LEN 9

#define TS_STOP_NET 0
#define TS_NOT_STOP_NET 1

#define TS_IS_LOCAL 1
#define TS_IS_REMOTE 0

#define TS_SYSTEM_PERMISSION_CONFIG_GATEWAY  0x1
#define TS_SYSTEM_PERMISSION_CONFIG_DEVICE 0x2
#define TS_SYSTEM_PERMISSION_CONFIG_SCENE  0x4
#define TS_SYSTEM_PERMISSION_CONFIG_ALARM  0x8
#define TS_SYSTEM_PERMISSION_CONFIG_USER  0x10
#define TS_SYSTEM_PERMISSION_CONFIG_ADMIN  0x10
// start or stop remote control service permission
#define TS_SYSTEM_PERMISSION_REMOTE_CONTROL 0x20
// remote ETS programming permission
#define TS_SYSTEM_PERMISSION_REMOTE_ETS 0x40
// ALL PERMISSIONS
#define TS_SYSTEM_PERMISSION_ALL 0x7F

#ifdef __cplusplus
extern "C"
{
#endif

int32_t db_is_device_control_by_name(char * user_name, int32_t device_id, int32_t * is_control);

int32_t db_is_system_control_by_name(char * user_name, int32_t permission_id, int32_t * is_control);


int32_t db_is_device_control(int32_t user_id, int32_t device_id, int32_t * is_control);

int32_t db_is_system_control(int32_t user_id, int32_t permission_id, int32_t * is_control);

//int32_t  db_is_device_control_4_session(char *session_id, int32_t  device_id,
//	int32_t  * is_control);

//int32_t  db_is_system_control_4_session(char *session_id, int32_t  permission_id,
//	int32_t  * is_control);
//t_permisson的id后左移……|系统配置|主机停止外网服务|

//set the relation of session and user
int32_t  db_set_session_user(char *session_id, char *user_name);

//delete the relation of session and user
int32_t  db_del_session_user(char *session_id);

//get the host user name and user password
int32_t  db_get_host_user_info(char * user_name, char * passwd);

//get the lastest config time
int32_t  db_get_lastest_version_conf(unsigned long long *version);

//get the lastest software time
int32_t  db_get_lastest_version_software(char *version);

//whether the username and the password right
int32_t  db_user_validate(char * user_name, char * passwd ,int32_t  * is_pass);

int32_t  db_get_passwd_by_name(char * user_name, char * passwd);

int32_t  db_get_home_id(int32_t * home_id);

int32_t db_get_is_stop_net(int32_t * is_stop);

int32_t db_set_is_stop_net(int32_t is_stop);

int32_t db_get_max_users(int32_t *users);

int32_t db_get_is_local(int32_t *is_local);

int32_t db_set_is_local(int32_t is_local);

#ifdef __cplusplus
}
#endif


#endif /* TS_DB_CONN_INFOS_H_ */

