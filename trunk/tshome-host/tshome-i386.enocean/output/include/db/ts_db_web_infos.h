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

#define TS_WEB_NULL -1


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _base_user_info
{
	char user_name[31];
	char passwd[15];
	char ip[32];
	int32_t port;
	char login_user_name[32];
	char login_passwd[32];
} ts_host_base_user_info;

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

#ifdef __cplusplus
}
#endif

#endif /* TS_DB_WEB_INFOS_H_ */
