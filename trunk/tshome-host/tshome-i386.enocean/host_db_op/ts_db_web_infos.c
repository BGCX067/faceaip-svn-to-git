/*
 * ts_db_web_infos.cpp
 *
 *  Created on: 2012-3-21
 *      Author: pangt
 */

#include <sys/un.h>
#include <unistd.h>
#include <asm/types.h>
#include <netinet/ether.h>
#include <sys/ioctl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>

#include "ts_db_web_infos.h"
#include "log/ts_log_tools.h"
#include "common/remote_def.h"

#define TS_WEB_DEFAULT_PORT 8888

int32_t db_get_base_user_info(ts_host_base_user_info * user_info)
{
	int32_t ret = EXIT_SUCCESS;
	const char * get_user_info = "select * from t_host_base_info;";
	int32_t rows = 0; //rows
	int32_t columns = 0; // columns
	char **all_result;
	ret = sc_sqlite3_get_table(get_user_info, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		ret = error_code;
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	//If there is no records ,insert .
	if (rows <= 0) {
		//set default value
		strcpy(user_info->user_name, TS_WEB_DEFAULT_LOGIN_USER);
		strcpy(user_info->passwd, TS_WEB_DEFAULT_LOGIN_PASSWD);
		strcpy(user_info->login_user_name, TS_WEB_DEFAULT_USER);
		strcpy(user_info->login_passwd, TS_WEB_DEFAULT_PASSWD);
		strcpy(user_info->ip, TS_WEB_DEFAULT_IP);
		strcpy(user_info->gw, TS_WEB_DEFAULT_GW);
		strcpy(user_info->net_mask, TS_WEB_DEFAULT_NM);
		strcpy(user_info->server_name, TS_WEB_DEFAULT_SERVER_NAME);
		user_info->who_am_i = TS_PRO_TYPE_HOST;
		user_info->port = TS_WEB_DEFAULT_PORT;
		user_info->server_port = TS_WEB_DEFAULT_SERVER_PORT;
		user_info->is_first_use = TS_IS_FIRST_USE;

		//insert;
		const char * base_insert_sql = "insert into t_host_base_info (user_name,passwd,ip,port,gw,net_mask,server_name,server_port,login_name,login_passwd,is_first_use) values ('%s','%s','%s',%d,'%s','%s','%s',%d,'%s','%s',%d) ;";
//		char insert_sql[strlen(base_insert_sql) + sizeof(ts_host_base_user_info) + TS_DB_INT_ADD]={0};
		char insert_sql[TS_DB_SQL_LENGTH] = { 0 };
		snprintf(insert_sql, sizeof(insert_sql), base_insert_sql, TS_WEB_DEFAULT_USER, TS_WEB_DEFAULT_PASSWD, TS_WEB_DEFAULT_IP, TS_WEB_DEFAULT_PORT, TS_WEB_DEFAULT_GW, TS_WEB_DEFAULT_NM, TS_WEB_DEFAULT_SERVER_NAME, TS_WEB_DEFAULT_SERVER_PORT, TS_WEB_DEFAULT_LOGIN_USER, TS_WEB_DEFAULT_LOGIN_PASSWD, TS_IS_FIRST_USE);

		ret = sc_sqlite3_exec(insert_sql, 0, 0);
		if (ret != 0) {
			ret = ERR_DB_INSERT;
			return ret;
		}
	} else {
		int i = 0;
		int j = 0;
		for (i = 1; i <= rows; i++) {
			for (j = 0; j < columns; j++) {
				if (NULL == all_result[i * columns + j]) {
					continue;
				}
				if (0 == strcmp(all_result[j], "user_name")) {
					strcpy(user_info->user_name, all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "passwd")) {
					strcpy(user_info->passwd, all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "ip")) {
					strcpy(user_info->ip, all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "port")) {
					user_info->port = atoi(all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "gw")) {
					strcpy(user_info->gw, all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "net_mask")) {
					strcpy(user_info->net_mask, all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "server_name")) {
					strcpy(user_info->server_name, all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "server_port")) {
					user_info->server_port = atoi(all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "login_name")) {
					strcpy(user_info->login_user_name, all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "login_passwd")) {
					strcpy(user_info->login_passwd, all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "who_am_i")) {
					user_info->who_am_i = atoi(all_result[i * columns + j]);
				} else if (0 == strcmp(all_result[j], "is_first_use")) {
					user_info->is_first_use = atoi(all_result[i * columns + j]);
					if ((user_info->is_first_use != 0) && (user_info->is_first_use != 1)) {
						user_info->is_first_use = TS_IS_FIRST_USE;
					}
				}
			}
		}
	}

	return ret;
}

int32_t db_web_validate_user(ts_host_base_user_info user_info, int8_t * pass)
{
	int ret = EXIT_SUCCESS;
	ts_host_base_user_info base_info;
	ret = db_get_base_user_info(&base_info);
	if (EXIT_SUCCESS != ret) {
		*pass = TS_WEB_VALIDATE_FALSE;
		return ret;
	}

	if ((0 == strcmp(user_info.user_name, base_info.user_name)) && (0 == strcmp(user_info.passwd, base_info.passwd))) {
		*pass = TS_WEB_VALIDATE_TRUE;
	} else {
		*pass = TS_WEB_VALIDATE_FALSE;
	}
	return ret;
}

int32_t db_set_base_user_info(ts_host_base_user_info user_info)
{
	int32_t ret = EXIT_SUCCESS;
	ts_host_base_user_info tmp_info;

#ifndef TS_CURR_PRO_TYPE_GW
	user_info.who_am_i = TS_PRO_TYPE_HOST;
#else
	user_info.who_am_i = TS_PRO_TYPE_GATEWAY;
#endif
	db_get_base_user_info(&tmp_info);
	char *base_sql_upd = "update t_host_base_info set user_name = \'%s\', passwd = \'%s\', ip = \'%s\' , port = %d ,"
		" gw = '%s' , net_mask = '%s',server_name = '%s' ,server_port=%d,login_name='%s',login_passwd='%s',"
		" who_am_i = %d,is_first_use = %d;";
	char sql_upd[strlen(base_sql_upd) + TS_DB_INT_ADD + sizeof(ts_host_base_user_info) + 1];
	sprintf(sql_upd, base_sql_upd, user_info.user_name, user_info.passwd, user_info.ip, user_info.port, user_info.gw, user_info.net_mask, user_info.server_name, user_info.server_port, user_info.login_user_name, user_info.login_passwd, user_info.who_am_i, user_info.is_first_use);
	ret = sc_sqlite3_exec(sql_upd, 0, 0);
	if (ret != 0) {
		ret = ERR_DB_UPDATE;
		return ret;
	}
	return ret;
}

int32_t db_web_get_infr_device_infos(struct list_head * head_list, ts_infrared_device_list * infos, int32_t * counts)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_get_infos = "select a.device_id id,a.deviceName, (select b.channel from t_host_remotectrl b where a.device_id = b.device_id limit 1) channel "
		" from t_device a,t_remotectrl c where 1 = 1 and a.device_id = c.device;";
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **results;
	ret = sc_sqlite3_get_table(b_get_infos, &results, &rows, &columns);
	if (ret != 0) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(results);
		return ret;
	}

	*counts = rows;
	int32_t i = 0;
	int32_t j = 0;
	for (i = 1; i <= rows; i++) {
		infos = (ts_infrared_device_list *) calloc(sizeof(ts_infrared_device_list), sizeof(char));
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(results[j], "id")) {
				infos->infr_device_info.id = atoi(results[i * columns + j]);
			} else if (0 == strcmp(results[j], "deviceName")) {
				strcpy(infos->infr_device_info.device_name, results[i * columns + j]);
			} else if (0 == strcmp(results[j], "channel")) {
				if (NULL != results[i * columns + j]) {
					infos->infr_device_info.channel = atoi(results[i * columns + j]);
				} else {
					infos->infr_device_info.channel = TS_WEB_NULL;
				}
			}
		}
		list_add_tail(&infos->list, head_list);
	}

	sqlite3_free_table(results);
	return ret;
}

int32_t db_web_set_infr_device_info(ts_infrared_device info)
{
	int32_t ret = EXIT_SUCCESS;
	ts_host_base_user_info tmp_info;
	db_get_base_user_info(&tmp_info);
//	char *base_sql_upd = "update t_remotectrl set channel = \'%d\' where id = %d";
	char *base_sql_del = "delete from t_host_remotectrl where id = %d";
//	ret = sc_sqlite3_exec("begin transaction", 0, 0);

	TS_DB_BEGIN_TRANS_FILE

//	char sql_del[strlen(base_sql_del) + TS_DB_INT_ADD * 2 + 1];
	char sql_del[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_del, base_sql_del, info.id);
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, sql_del, 0, 0);
	if (ret != 0) {
		ret = ERR_DB_DELETE;
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
		return ret;
	}

	char *base_sql_ins = "insert into t_host_remotectrl (device_id,channel) values (%d,%d);";
	char sql_ins[strlen(base_sql_ins) + TS_DB_INT_ADD * 2 + 1];
	sprintf(sql_ins, base_sql_ins, info.id, info.channel);
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, sql_ins, 0, 0);
	if (ret != 0) {
		ret = ERR_DB_INSERT;
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
		return ret;
	}

	TS_DB_COMMIT_TRANS_FILE
//	ret = sc_sqlite3_exec("commit transaction", 0, 0);

	return ret;
}

int32_t db_web_get_all_gateways(struct list_head * head_list, ts_gateway_list_t * gateways, int32_t * counts)
{
	//get gateway in interface
	char base_sql_get_gateways[] = "SELECT a.id,a.gatewayIP,a.gatewayPort,b.gatewayType type_name "
		"FROM t_gateway a,t_gatewayType b where a.gatewayTypeId = b.id and b.id != %d;";
//	char * sql_get_gateways = (char *) calloc(strlen(base_sql_get_gateways) + TS_DB_INT_ADD + 1, sizeof(char));
	char sql_get_gateways[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_get_gateways, base_sql_get_gateways, TS_GATEWAY_TYPE_HOST);

	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_gateway_res = NULL;
	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(sql_get_gateways, &all_gateway_res, &rows, &columns);
//	free(sql_get_gateways);

	*counts = rows;
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_gateway_res);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	//get gateways
	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		gateways = (ts_gateway_list_t *) calloc(sizeof(ts_gateway_list_t), sizeof(char));
		for (j = 0; j < columns; j++) {
			if (NULL == all_gateway_res[i * columns + j]) {
				continue;
			}
			if (0 == strcmp(all_gateway_res[j], "id")) {
				gateways->gateway.id = atoi(all_gateway_res[i * columns + j]);
			} else if (0 == strcmp(all_gateway_res[j], "gatewayIP")) {
				TS_STRING_SET_0(gateways->gateway.gatewayIP);
				if (NULL != all_gateway_res[i * columns + j]) {
					memcpy(gateways->gateway.gatewayIP, all_gateway_res[i * columns + j], strlen(all_gateway_res[i * columns + j]));
				}
			} else if (0 == strcmp(all_gateway_res[j], "gatewayPort")) {
				gateways->gateway.gatewayPort = atoi(all_gateway_res[i * columns + j]);
			} else if (0 == strcmp(all_gateway_res[j], "type_name")) {
				TS_STRING_SET_0(gateways->gateway.description);
				if (NULL != all_gateway_res[i * columns + j]) {
					memcpy(gateways->gateway.gateway_type_name, all_gateway_res[i * columns + j], strlen(all_gateway_res[i * columns + j]));
				}
			}
		}
		list_add_tail(&gateways->list, head_list);
	}

	sqlite3_free_table(all_gateway_res);
	return EXIT_SUCCESS;
}

int32_t db_web_set_host_interface(ts_host_interface_t host_interface)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_get_sql = "select * from t_host_interface where 1 = 1 and id = %d";
//	char * get_sql = (char *) calloc(strlen(b_get_sql) + TS_DB_INT_ADD, sizeof(char));
	char get_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(get_sql, b_get_sql, host_interface.id);
	int32_t rows = 0;
	int32_t columns = 0;
	char ** all_results;
	ret = sc_sqlite3_get_table(get_sql, &all_results, &rows, &columns);
//	ts_free(get_sql);
	if (ret != 0) {
		ret = ERR_DB_QUERY;
		return ret;
	}

	//whether insert or update
	if (rows <= 0) { //insert
		const char * b_insert = "insert into t_host_interface (desc,type,info) values ('%s',%d,'%s');";
//		char * insert_sql = (char *) calloc(strlen(b_insert) + TS_DB_INT_ADD + sizeof(ts_host_interface_t), sizeof(char));
		char insert_sql[TS_DB_SQL_LENGTH] = { 0 };
		sprintf(insert_sql, b_insert, host_interface.desc, host_interface.type, host_interface.info);
		ret = sc_sqlite3_exec(insert_sql, 0, 0);
//		ts_free(insert_sql);
		if (ret != 0) {
			ret = ERR_DB_INSERT;
			return ret;
		}

	} else { //update
		const char * b_upd = "update t_host_interface set desc = '%s',type = %d,info = '%s' where id = %d;";
//		char * upd_sql = (char *) calloc(strlen(b_upd) + TS_DB_INT_ADD * 2 + sizeof(ts_host_interface_t), sizeof(char));
		char upd_sql[TS_DB_SQL_LENGTH] = { 0 };
		sprintf(upd_sql, b_upd, host_interface.desc, host_interface.type, host_interface.info, host_interface.id);
		ret = sc_sqlite3_exec(upd_sql, 0, 0);
//		ts_free(upd_sql);
		if (ret != 0) {
			ret = ERR_DB_UPDATE;
			return ret;
		}

	}

	return ret;
}

int32_t db_web_del_host_interface(int32_t id)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_del = "delete from t_host_interface where id = %d;";
//	char * del_sql = (char *) calloc(strlen(b_del) + TS_DB_INT_ADD, sizeof(char));
	char del_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(del_sql, b_del, id);
//	ret = sc_sqlite3_exec("begin transaction", 0, 0);
	ret = sc_sqlite3_exec(del_sql, 0, 0);
//	free(del_sql);
	if (ret != 0) {
		ret = ERR_DB_UPDATE;
//		ret = sc_sqlite3_exec("rollback transaction", 0, 0);
		return ret;
	}
	db_web_del_host_inter_gateway(id);
//	ret = sc_sqlite3_exec("commit transaction", 0, 0);
	return ret;
}

int32_t db_web_del_host_inter_gateway(int32_t interface_id)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_del = "delete from t_host_gateway_interface where interface_id = %d;";
//	char * del_sql = (char *) calloc(strlen(b_del) + TS_DB_INT_ADD, sizeof(char));
	char del_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(del_sql, b_del, interface_id);
	ret = sc_sqlite3_exec(del_sql, 0, 0);
//	ts_free(del_sql);
	if (ret != 0) {
		ret = ERR_DB_UPDATE;
		return ret;
	}

	return ret;
}

int32_t db_web_insert_host_inter_gateway(int32_t interface_id, int32_t gateway_id)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_insert = "insert into t_host_gateway_interface (interface_id,gateway_id) values (%d,%d)";
//	char * insert_sql = (char *) calloc(strlen(b_insert) + TS_DB_INT_ADD, sizeof(char));
	char insert_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(insert_sql, b_insert, interface_id, gateway_id);
	ret = sc_sqlite3_exec(insert_sql, 0, 0);
//	ts_free(insert_sql);
	if (ret != 0) {
		ret = ERR_DB_UPDATE;
		return ret;
	}

	return ret;
}

int32_t db_web_get_host_device_infos(struct list_head * head_list, ts_web_host_device_info_list * infos, int32_t *counts)
{
	//get gateway in interface
	char base_sql_get_info[] = "select a.device_id,a.deviceName,b.roomName,"
		" (select d.comm_port from t_host_serial_info d,t_host_device_serial e "
		" where a.device_id = e.device and e.serial = d.id) comm_port "
		" from t_device a,t_room b,t_gateway c  "
		" where a.room = b.id and a.gateway = c.id and c.gatewayTypeId = %d and a.subType in (%d,%d)";
//	char * sql_get_info = (char *) calloc(strlen(base_sql_get_info) + TS_DB_INT_ADD + 1, sizeof(char));
	char sql_get_info[TS_DB_SQL_LENGTH] = { 0 };
	//	char sql_get_info[strlen(base_sql_get_info) + TS_DB_INT_ADD + 1] = {0};

	sprintf(sql_get_info, base_sql_get_info, TS_GATEWAY_TYPE_HOST, TS_DEV_SUBTYPE_SERIAL, TS_DEV_SUBTYPE_SERIAL_CUSTOM);

	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **results = NULL;
	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(sql_get_info, &results, &rows, &columns);
	*counts = rows;
//	ts_free(sql_get_info);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(results);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	//get gateways
	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		infos = (ts_web_host_device_info_list *) calloc(sizeof(ts_web_host_device_info_list), sizeof(char));
		for (j = 0; j < columns; j++) {
			if (NULL == results[i * columns + j]) {
				continue;
			}
			if (0 == strcmp(results[j], "device_id")) {
				infos->dev_info.id = atoi(results[i * columns + j]);
			} else if (0 == strcmp(results[j], "deviceName")) {
				strcpy(infos->dev_info.device_name, results[i * columns + j]);
			} else if (0 == strcmp(results[j], "roomName")) {
				strcpy(infos->dev_info.room_name, results[i * columns + j]);
			} else if (0 == strcmp(results[j], "comm_port")) {
				strcpy(infos->dev_info.host_info, results[i * columns + j]);
			}
		}
		list_add_tail(&infos->list, head_list);
	}
	sqlite3_free_table(results);
	return EXIT_SUCCESS;
}

int32_t db_web_set_host_device_info(ts_web_host_serial_device_info info)
{
	int32_t ret = EXIT_SUCCESS;
	//delete
	const char * b_del_sql = "delete from t_host_device_serial where device = %d";
//	char * del_sql = (char *) calloc(strlen(b_del_sql) + TS_DB_INT_ADD, sizeof(char));
	char del_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(del_sql, b_del_sql, info.id);
//	ret = sc_sqlite3_exec("begin transaction", 0, 0);

	TS_DB_BEGIN_TRANS_FILE

	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, del_sql, 0, 0);
//	free(del_sql);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_DELETE;
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
		return ret;
	}

	//insert
	const char * b_ins_sql = "insert into t_host_device_serial ( serial, device) values ( "
		"(select id from t_host_serial_info where comm_port = '%s'  limit 1) , %d);";
	char * ins_sql = (char *) calloc(strlen(b_ins_sql) + TS_DB_INT_ADD, sizeof(char));
	sprintf(ins_sql, b_ins_sql, info.host_info, info.id);
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, ins_sql, 0, 0);
//	free(ins_sql);
	if (ret != 0) {
		ret = ERR_DB_INSERT;
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
		return ret;
	}

	TS_DB_COMMIT_TRANS_FILE

//	ret = sc_sqlite3_exec("commit transaction", 0, 0);
	return ret;
}

/**
 * insert login info
 */
int32_t ts_web_login_add(char * ip)
{
	int32_t ret = EXIT_SUCCESS;
	//insert
	const char * b_ins_sql = "insert into t_host_login_info ( ip ) values ( '%s' );";
//	char * ins_sql = (char *) calloc(strlen(b_ins_sql) + strlen(ip) + 1, sizeof(char));
	char ins_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(ins_sql, b_ins_sql, ip);
	ret = sc_sqlite3_exec(ins_sql, 0, 0);
//	free(ins_sql);
	if (ret != 0) {
		ret = ERR_DB_INSERT;
		return ret;
	}

	return ret;
}

int32_t ts_web_login_del(char * ip)
{
	int32_t ret = EXIT_SUCCESS;
	//insert
	const char * b_ins_sql = "delete from  t_host_login_info where ip = '%s';";
//	char * ins_sql = (char *) calloc(strlen(b_ins_sql) + strlen(ip) + 1, sizeof(char));
	char ins_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(ins_sql, b_ins_sql, ip);
	ret = sc_sqlite3_exec(ins_sql, 0, 0);
//	ts_free(ins_sql);
	if (ret != 0) {
		ret = ERR_DB_DELETE;
		return ret;
	}

	return ret;
}

int32_t ts_web_login_validate(char * ip, int32_t * is_login)
{
	//get gateway in interface
	char base_sql_get_info[] = "select * from t_host_login_info where ip = '%s'";
//	char * sql_get_info = (char *) calloc(strlen(base_sql_get_info) + strlen(ip) + 1, sizeof(char));
	char sql_get_info[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_get_info, base_sql_get_info, ip);
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **results = NULL;
	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(sql_get_info, &results, &rows, &columns);
//	ts_free(sql_get_info);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(results);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (rows > 0) {
		*is_login = TS_WEB_VALIDATE_TRUE;
	} else {
		*is_login = TS_WEB_VALIDATE_FALSE;
	}

	sqlite3_free_table(results);
	return EXIT_SUCCESS;
}

static int32_t get_host_net(char *name, ts_host_net_info *net_info)
{
	int32_t ret = EXIT_SUCCESS;

	const char *b_sql = "select type,* from t_host_net_info where name = '%s' limit 1";
	char get_sql[TS_DB_SQL_LENGTH] = { 0 };
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **results = NULL;

	sprintf(get_sql, b_sql, name);
	ret = sc_sqlite3_get_table(get_sql, &results, &rows, &columns);
	if (ret != SQLITE_OK) {
		sqlite3_free_table(results);
		return ERR_DB_QUERY;
	}

	if (rows <= 0) {
		return ERR_DB_NO_RECORDS;
	} else {
		ret = EXIT_SUCCESS;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == results[i * columns + j]) {
				continue;
			} else {
				if (0 == strcmp(results[j], "type")) {
					net_info->type = atoi(results[i * columns + j]);
				} else if (0 == strcmp(results[j], "ip")) {
					strcpy((char *) net_info->ip, results[i * columns + j]);
				} else if (0 == strcmp(results[j], "port")) {
					net_info->port = atoi(results[i * columns + j]);
				} else if (0 == strcmp(results[j], "netmask")) {
					strcpy((char *) net_info->netmask, results[i * columns + j]);
				} else if (0 == strcmp(results[j], "gw")) {
					strcpy((char *) net_info->gw, results[i * columns + j]);
				} else if (0 == strcmp(results[j], "is_dhcp")) {
					net_info->is_dhcp = atoi(results[i * columns + j]);
				} else if (0 == strcmp(results[j], "ssid")) {
					strcpy((char *) net_info->ssid, results[i * columns + j]);
				} else if (0 == strcmp(results[j], "name")) {
					strcpy((char *) net_info->name, results[i * columns + j]);
				} else if (0 == strcmp(results[j], "login_passwd")) {
					strcpy((char *) net_info->login_passwd, results[i * columns + j]);
				}
			}
		}
	}
	return ret;
}

int32_t db_web_set_host_net(ts_host_net_info net_info)
{
	int32_t ret = EXIT_SUCCESS;

	//whether db has this net
	ts_host_net_info tmp_net_info;
	int32_t res = get_host_net(net_info.name, &tmp_net_info);
	if (ERR_DB_NO_RECORDS == res) {
		//insert
		const char *b_ins_sql = "insert into t_host_net_info (type,ip,port,netmask,gw,is_dhcp,name,ssid,login_passwd) "
			" values (%d,'%s',%d,'%s','%s',%d,'%s','%s','%s');";
		char ins_sql[TS_DB_SQL_LENGTH] = { 0 };
		sprintf(ins_sql, b_ins_sql, net_info.type, net_info.ip, net_info.port, net_info.netmask, net_info.gw, net_info.is_dhcp, net_info.name, net_info.ssid, net_info.login_passwd);
//		printf("sql = %s\n",ins_sql);
		ret = sc_sqlite3_exec(ins_sql, 0, 0);
		if (ret != 0) {
			ret = ERR_DB_INSERT;
			return ret;
		}
	} else if (EXIT_SUCCESS == res) {
		//update
		const char *b_upd_sql = "update t_host_net_info set type = %d,ip = '%s',port = %d,netmask = '%s' ,"
			" gw = '%s',is_dhcp = %d,name = '%s' ,ssid = '%s' ,login_passwd='%s' where name = '%s'";
		char upd_sql[TS_DB_SQL_LENGTH] = { 0 };
		sprintf(upd_sql, b_upd_sql, net_info.type, net_info.ip, net_info.port, net_info.netmask, net_info.gw, net_info.is_dhcp, net_info.name, net_info.ssid, net_info.login_passwd, net_info.name);
		ret = sc_sqlite3_exec(upd_sql, 0, 0);
		if (ret != 0) {
			ret = ERR_DB_INSERT;
			return ret;
		}
	} else {
		ret = res;
	}

	return ret;
}

int32_t db_web_get_host_net(char *name, ts_host_net_info *net_info)
{
	int32_t ret = EXIT_SUCCESS;

	int32_t res = get_host_net(name, net_info);
	if (ERR_DB_NO_RECORDS == res) {
		//insert
		strcpy(net_info->gw, "192.168.1.1");
		strcpy(net_info->ip, "192.168.1.100");
		net_info->is_dhcp = 0;
		net_info->port = 8888;
		net_info->is_dhcp = TS_WEB_DHCP;
		net_info->type = TS_WEB_NET_WIFI;
		strcpy(net_info->login_passwd, "passwd");
		strcpy(net_info->netmask, TS_WEB_DEFAULT_NM);
		strcpy(net_info->ssid, "ssid");
		strcpy(net_info->name, name);
		db_web_set_host_net(*net_info);
		ret = get_host_net(name, net_info);
	} else {
		ret = res;
	}

	return ret;
}

//net///////////////////////////////////////////////
//#define ETH0 "eth0"
#define MAXINTERFACES   16

int ts_net_get_net_infos(char *ip, char *zwym, char*brdaddr, char *mac, char * net_name)
{

	/*set socker prama*/
	int sock;
	struct sockaddr_in sin;
	struct ifreq ifr;

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock == -1) {
		perror("socket");
		return -1;
	}

	strncpy(ifr.ifr_name, net_name, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;

	/*ip*/
	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0) {
		char err_info[100] = { 0 };
		snprintf(err_info, sizeof(err_info), "%s : ", net_name);
		perror(err_info);
		const char * no_dev_net = TS_WEB_GET_NO_NET_ADDR;
		sprintf(ip, "%s", no_dev_net);
		sprintf(zwym, "%s", no_dev_net);
		sprintf(brdaddr, "%s", no_dev_net);
		sprintf(mac, "%s", no_dev_net);
		return -1;
	}

	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	sprintf(ip, "%s", inet_ntoa(sin.sin_addr));

	/*net mask*/
	if (ioctl(sock, SIOCGIFNETMASK, &ifr) < 0) {
		perror("ioctl");
		return -1;
	}

	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	sprintf(zwym, "%s", inet_ntoa(sin.sin_addr));

	/*broad address*/
	ioctl(sock, SIOCGIFBRDADDR, &ifr);
	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	sprintf(brdaddr, "%s", inet_ntoa(sin.sin_addr));

	/*mac*/
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
		perror("ioctl");
		return -1;
	}

	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	sprintf(mac, "%s", inet_ntoa(sin.sin_addr));
	return 0;

}

int32_t ts_web_get_dns(char *dns)
{
	FILE *pstr;
	strcpy(dns, TS_WEB_GET_NO_NET_ADDR);

//	system("cat /etc/resolv.conf > resolv");
	//	pstr = fopen("/proc/net/route", "r");

	pstr = fopen("/etc/resolv.conf", "r");
	if (pstr == NULL) {
		log_err("open file /etc/resolv.conf failed!\n");
		strcpy(dns, TS_WEB_GET_NO_NET_ADDR);
		return -1;
	}

	int32_t flen = 0;
//	char * buffer = "";
	char buffer0[256] = { 0 };

	if (NULL != pstr) {
		fseek(pstr, 0, SEEK_END);
		flen = ftell(pstr);
		fseek(pstr, 0, SEEK_SET);

//		buffer = (char*) calloc(flen + 1, sizeof(char));
		fread(buffer0, sizeof(char), flen, pstr);
	}
	char * buffer;
	buffer = strstr(buffer0, "nameserver");
//		fprintf(stderr,"buffer = %s\n",buffer);

	char * infos;
	char * info1;
	while ((infos = strsep(&buffer, "\n")) != NULL) {
		int i = 0;
		while ((info1 = strsep(&infos, " ")) != NULL) {
			if (1 == i) {
				bzero(dns, strlen(dns));
				strcpy(dns, info1);
				fclose(pstr);
				pstr = NULL;
				return EXIT_SUCCESS;
			}
			i++;
		}
	}
	if (NULL != pstr) {
		fclose(pstr);
	}

	return EXIT_SUCCESS;
}

#define BUFSIZE 8192
struct route_info
{
	u_int dstAddr;
	u_int srcAddr;
	u_int gateWay;
	char ifName[IF_NAMESIZE];
};
int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{

	struct nlmsghdr *nlHdr;

	int readLen = 0, msgLen = 0;

	do {
		//get response from kernel
		if ((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0) {
			perror("SOCK READ: ");
			return -1;
		}
		nlHdr = (struct nlmsghdr *) bufPtr;

		//chech whether header avlidate
		if ((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR)) {
			perror("Error in recieved packet");
			return -1;
		} /* Check if the its the last message */
		if (nlHdr->nlmsg_type == NLMSG_DONE) {
			break;
		} else { /* Else move the pointer to buffer appropriately */
			bufPtr += readLen;
			msgLen += readLen;
		}
		/* Check if its a multi part message */
		if ((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0) {
			/* return if its not */
			break;
		}
	} while ((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
	return msgLen;
}

//parse routes infos
void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo, char *gateway)
{
	struct rtmsg *rtMsg;
	struct rtattr *rtAttr;
	int rtLen;
//	char *tempBuf = NULL; //2007-12-10
	struct in_addr dst;
	struct in_addr gate;
//	tempBuf = (char *) malloc(100);
//	char tempBuf[100] = { 0 };
	rtMsg = (struct rtmsg *) NLMSG_DATA(nlHdr);
// If the route is not for AF_INET or does not belong to main routing table then return.
	if ((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
		return;
	/* get the rtattr field */
	rtAttr = (struct rtattr *) RTM_RTA(rtMsg);
	rtLen = RTM_PAYLOAD(nlHdr);
	for (; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr, rtLen)) {

		switch (rtAttr->rta_type) {
			case RTA_OIF:
				if_indextoname(*(int *) RTA_DATA(rtAttr), rtInfo->ifName);
				break;
			case RTA_GATEWAY:
				rtInfo->gateWay = *(u_int *) RTA_DATA(rtAttr);
				break;
			case RTA_PREFSRC:
				rtInfo->srcAddr = *(u_int *) RTA_DATA(rtAttr);
				break;
			case RTA_DST:
				rtInfo->dstAddr = *(u_int *) RTA_DATA(rtAttr);
				break;
		}
	} //2007-12-10
	dst.s_addr = rtInfo->dstAddr;
	if (strstr((char *) inet_ntoa(dst), "0.0.0.0")) {
		gate.s_addr = rtInfo->gateWay;
		sprintf(gateway, "%s", (char *) inet_ntoa(gate));
	}
//	free(tempBuf);
	return;
}

int ts_net_get_gateway(char *ip, char * net_name)
{
	FILE *pstr;
	strcpy(ip, "0.0.0.0");
//		system("cp /proc/net/route .");
	system("cat /proc/net/route > route");
//	pstr = fopen("/proc/net/route", "r");

	pstr = fopen("route", "r");
	if (pstr == NULL) {
		log_err("open file /proc/net/route failed!");
		strcpy(ip, TS_WEB_GET_NO_NET_ADDR);
		return -1;
	}

	int32_t flen = 0;
	char * buffer = "";

	if (NULL != pstr) {
		fseek(pstr, 0, SEEK_END);
		flen = ftell(pstr);
		fseek(pstr, 0, SEEK_SET);

		buffer = (char*) calloc(flen + 1, sizeof(char));
		fread(buffer, sizeof(char), flen, pstr);
	}

	char * infos;
	char * info1;
	int gn = -1;
	while ((infos = strsep(&buffer, "\n")) != NULL) {
		int i = 0;
		char * infos_bak = infos;
		while ((info1 = strsep(&infos, "\t")) != NULL) {
			if (NULL != strstr(info1, "Gateway")) {
				gn = i;
			}
			if ((gn == i) && (0 != strcmp(info1, "00000000"))) {
				if (NULL != strstr(infos_bak, net_name)) {
					struct in_addr addr;
					addr.s_addr = strtol(info1, NULL, 16);
					strcpy(ip, inet_ntoa(addr));
				}
			}
			i++;
		}
	}
	fclose(pstr);

	return EXIT_SUCCESS;
}

//others ////////////////////
int32_t ts_send_data(web_action_report_t * act_ctrl)
{
	int32_t ret = EXIT_SUCCESS;
	int32_t sockfd;
	int32_t len;
	struct sockaddr_un address;

	log_debug_web("data :%d,fun : %d, source : %d,start : %d\n", act_ctrl->data, act_ctrl->fun_code, act_ctrl->source, act_ctrl->start_flag);

	/*  Create a socket for the client.  */
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	/*  Name the socket, as agreed with the server.  */
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, UNIX_DOMAIN);
	len = sizeof(address);

	/*  Now connect our socket to the server's socket.  */
	ret = connect(sockfd, (struct sockaddr *) &address, len);
	if (ret == -1) {
		log_debug_web("can't conn: client : %d\n", ret);
		return EXIT_FAILURE;
	}

	/*  We can now read/write via sockfd.  */
	log_debug_web("start send \n");
	int send_len = write(sockfd, act_ctrl, sizeof(web_action_report_t));
	if (send_len == sizeof(web_action_report_t)) {
		int rev = read(sockfd, act_ctrl, sizeof(web_action_report_t));
		if (rev < 0) {
			close(sockfd);
		} else {
			log_debug_web("rev data : %d\n", act_ctrl->data);
		}
	} else {
		log_debug_web("send error\n");
		close(sockfd);
		ret = EXIT_FAILURE;
		return ret;
	}
	close(sockfd);

	log_debug_web("socket closed\n");
	return ret;
}

int32_t ts_is_running(char * name, int32_t * is_exits)
{
	int ret = EXIT_SUCCESS;
	*is_exits = TS_NO_RUNNING;
	char cmd[128] = { 0 };
	sprintf(cmd, "killall -0 %s", name);
	int32_t run = 1;
	run = system(cmd);
	if (0 == run) {
		*is_exits = TS_RUNNING;
	} else {
		*is_exits = TS_NO_RUNNING;
	}

	return ret;
}

