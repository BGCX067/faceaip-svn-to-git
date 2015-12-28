/*
 * sqlite3_ctrl.c
 *
 *  Created on: 2012-1-11
 *      Author: pangt
 */

#include "ts_db_protocol_infos.h"
#include "common/remote_def.h"

static int32_t get_3_knx_group_addr(u_int16_t group_addr, int32_t * main_addr, int32_t * mid, int32_t * logic)
{
	int32_t ret = EXIT_SUCCESS;
	*main_addr = group_addr >> 11;
	*mid = group_addr >> 8 & 7;
	*logic = group_addr & 0xFF;
	return ret;
}

static int32_t get_2_knx_group_addr(u_int16_t group_addr, int32_t * main_addr, int32_t * logic)
{
	int32_t ret = EXIT_SUCCESS;
	*main_addr = group_addr >> 11;
	*logic = group_addr & 0x7FF;
	return ret;
}

int32_t get_table_max_id(const char * table_name, const char * id_name, int * max_id)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_get_max_id = "select max(%s) from %s ;";

//	char *get_max_id = (char *) calloc(strlen(b_get_max_id) + strlen(table_name) + strlen(id_name) + TS_DB_INT_ADD, sizeof(char));
	char get_max_id[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(get_max_id, b_get_max_id, id_name, table_name);
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **results;
	ret = sc_sqlite3_get_table(get_max_id, &results, &rows, &columns);
//	free(get_max_id);

	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (rows > 0) {
		*max_id = atoi(results[columns]);
	} else {
		*max_id = 0;
	}

	sqlite3_free_table(results);
	return ret;
}

static int32_t get_knx_group_info(char * group_info1, u_int16_t * group_addr)
{
	int32_t ret = EXIT_SUCCESS;
	char group_info[16] = { 0 };
	strcpy(group_info, group_info1);
	char * token = strtok(group_info, "/");
	int32_t i = 0;
	int32_t tmp = 0;
	int32_t main_group = 0;
	int32_t mid_group = 0;
	int32_t logic_group = 0;

	int32_t is_3 = TS_FLAG_FALSE;
	for (i = 0; token != NULL; i++) {
		if (0 == i) {
			main_group = atoi(token);
		} else if (1 == i) {
			tmp = atoi(token);
		} else if (2 == i) {
			is_3 = TS_FLAG_TRUE;
			logic_group = atoi(token);
		}
		token = strtok(NULL, "/");
	}

	if (TS_FLAG_TRUE == is_3) {
		mid_group = tmp;
		*group_addr = (main_group << 11) | (mid_group << 8) | (logic_group);
	} else if (TS_FLAG_FALSE == is_3) {
		//	mid_group = TS_KNX_GROUP_ADDR_INVALID;?
		logic_group = tmp;
		*group_addr = (main_group << 11) | (logic_group);
	}

	return ret;
}

int32_t get_device_gateway_type(int32_t device_id, int32_t *gateway_type)
{
	//get gateway Type
	const char * base_get_gatewayType = "select b.gatewayTypeId from t_device a ,t_gateway b where a.device_id = %d and a.gateway = b.id";
//	char * sql_get_gateway_info = (char *) calloc(strlen(base_get_gatewayType) + TS_DB_INT_ADD, sizeof(char));
	char sql_get_gateway_info[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_get_gateway_info, base_get_gatewayType, device_id);
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_gateway_type;
	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(sql_get_gateway_info, &all_gateway_type, &rows, &columns);
//	free(sql_get_gateway_info);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		ret = error_code;
		sqlite3_free_table(all_gateway_type);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(all_gateway_type[j], "gatewayTypeId")) {
				*gateway_type = atoi(all_gateway_type[i * columns + j]);
			}
		}
	}
	sqlite3_free_table(all_gateway_type);
	return ret;
}

//get gateway by interface
static int32_t get_gateway_by_interface(struct list_head * head_list, ts_gateway_list_t * gateways, int32_t interface_id, int32_t * g_rows)
{
	//get gateway in interface
	char base_sql_get_gateways[] = "select b.id,a.interface_id,b.gatewayName,b.gatewayIp,b.gatewayPort,b.description,b.gatewayTypeId from t_host_gateway_interface a,t_gateway b where 1 = 1 and a.gateway_id != %d and a.gateway_id = b.id and a.interface_id = %d;";
//	char * sql_select_gateways = (char *) calloc(strlen(base_sql_get_gateways) + TS_DB_INT_ADD * 2 + 1, sizeof(u_char));
	char sql_select_gateways[TS_DB_SQL_LENGTH] = { 0 };
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_gateway_res; //gateway results
	//	TS_STRING_SET_0(sql_select_gateways);
	sprintf(sql_select_gateways, base_sql_get_gateways, TS_HOST_GATEWAY_TYPE_NO_GATEWAY, interface_id);
	int32_t ret = sc_sqlite3_get_table(sql_select_gateways, &all_gateway_res, &rows, &columns);
//	free(sql_select_gateways);

	*g_rows = rows;
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_gateway_res);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	//get gateways
	int32_t i = 0, j = 0;
	for (i = 0; i <= rows; i++) {
		gateways = (ts_gateway_list_t *) malloc(sizeof(ts_gateway_list_t));
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(all_gateway_res[j], "id")) {
				if (NULL == all_gateway_res[i * columns + j]) {
					continue;
				}
				gateways->gateway.id = atoi(all_gateway_res[i * columns + j]);
			} else if (0 == strcmp(all_gateway_res[j], "gatewayName")) {
				if (NULL != all_gateway_res[i * columns + j]) {
					memcpy(gateways->gateway.name, all_gateway_res[i * columns + j], strlen(all_gateway_res[i * columns + j]));
				}
			} else if (0 == strcmp(all_gateway_res[j], "gatewayIP")) {
				if (NULL != all_gateway_res[i * columns + j]) {
//					memcpy(gateways->gateway.gatewayIP, all_gateway_res[i * columns + j], strlen(all_gateway_res[i * columns + j]));
					strcpy((char *) gateways->gateway.gatewayIP, all_gateway_res[i * columns + j]);
				}
			} else if (0 == strcmp(all_gateway_res[j], "gatewayPort")) {
				if (NULL != all_gateway_res[i * columns + j]) {
					gateways->gateway.gatewayPort = atoi(all_gateway_res[i * columns + j]);
				}
			} else if (0 == strcmp(all_gateway_res[j], "description")) {
				if (NULL != all_gateway_res[i * columns + j]) {
					memcpy(gateways->gateway.description, all_gateway_res[i * columns + j], strlen(all_gateway_res[i * columns + j]));
				}
			} else if (0 == strcmp(all_gateway_res[j], "gatewayTypeId")) {
				if (NULL != all_gateway_res[i * columns + j]) {
					gateways->gateway.getewayType = atoi(all_gateway_res[i * columns + j]);
				}
			}
		}
		list_add_tail(&gateways->list, head_list);
	}
	sqlite3_free_table(all_gateway_res);
	return EXIT_SUCCESS;
}

int32_t base_get_all_lines(struct list_head * head_list, ts_passageway_list_t * passageways, const char *sql_select_interface, int32_t * counts)
{
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_interface_res; //interface results

	//get result
	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(sql_select_interface, &all_interface_res, &rows, &columns);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_interface_res);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}
	int32_t i = 0, j = 0;
	*counts = rows;
	for (i = 1; i <= rows; i++) {
		int32_t g_rows = 0;
		LIST_HEAD(gateway_list_head);
		ts_gateway_list_t * gateways = NULL;
		get_gateway_by_interface(&gateway_list_head, gateways, atoi(all_interface_res[i * columns + 0]), &g_rows);
		passageways = (ts_passageway_list_t *) calloc(sizeof(ts_passageway_list_t) + sizeof(ts_gateway_list_t) * (g_rows), sizeof(char));
		passageways->passageway.gateway_counts = g_rows;
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(all_interface_res[j], "id")) {
				if (NULL == all_interface_res[i * columns + j]) {
					continue;
				}
				passageways->passageway.host_interface.id = atoi(all_interface_res[i * columns + j]);
			} else if (0 == strcmp(all_interface_res[j], "desc")) {
				if (NULL != all_interface_res[i * columns + j]) {
					memcpy(passageways->passageway.host_interface.desc, all_interface_res[i * columns + j], strlen(all_interface_res[i * columns + 1]));
				}
			} else if (0 == strcmp(all_interface_res[j], "type_name")) {
				if (NULL != all_interface_res[i * columns + j]) {
					memcpy(passageways->passageway.host_interface.type_name, all_interface_res[i * columns + j], strlen(all_interface_res[i * columns + 1]));
				}
			} else if (0 == strcmp(all_interface_res[j], "type")) {
				//if gateway type is wifi ,then copy gateway informations
				passageways->passageway.host_interface.type = atoi(all_interface_res[i * columns + j]);
				if (atoi(all_interface_res[i * columns + j]) == TS_GATEWAY_TYPE_WIFI) { //WIFI类
					if (g_rows > 0) {
						//init list_head
						struct list_head t = { &passageways->passageway.gateway_heead, &passageways->passageway.gateway_heead };
						passageways->passageway.gateway_heead = t;
						ts_gateway_list_t * p_tmp = NULL;
						list_for_each_entry_safe(gateways,p_tmp,&gateway_list_head,list) {
							passageways->passageway.gateway_list = (ts_gateway_list_t *) malloc(sizeof(ts_gateway_list_t));
							//set id
							passageways->passageway.gateway_list->gateway.id = gateways->gateway.id;

							//set name
							TS_STRING_SET_0( passageways->passageway.gateway_list->gateway.name);
							if (NULL != gateways->gateway.name) {
								if (NULL != all_interface_res[i * columns + j]) {
									memcpy(passageways->passageway.gateway_list->gateway.name, gateways->gateway.name, sizeof(gateways->gateway.name));
								}
							}

							//set ip
							TS_STRING_SET_0( passageways->passageway.gateway_list->gateway.gatewayIP);
							if (NULL != gateways->gateway.gatewayIP) {
								memcpy(passageways->passageway.gateway_list->gateway.gatewayIP, gateways->gateway.gatewayIP, sizeof(gateways->gateway.gatewayIP));
							}
							//set port
							passageways->passageway.gateway_list->gateway.gatewayPort = gateways->gateway.gatewayPort;

							//set desc
							TS_STRING_SET_0( passageways->passageway.gateway_list->gateway.description);
							if (NULL != gateways->gateway.description) {
								memcpy(passageways->passageway.gateway_list->gateway.description, gateways->gateway.description, sizeof(gateways->gateway.description));
							}

							//set type
							passageways->passageway.gateway_list->gateway.getewayType = gateways->gateway.getewayType;

							//add to list
							list_add_tail(&passageways->passageway.gateway_list->list, &passageways->passageway.gateway_heead);
							// free
							list_del(&gateways->list);
							free(gateways);
						}
					} else {
						passageways->passageway.gateway_list = NULL;
					}
				} else {
					passageways->passageway.gateway_list = NULL;
				}
			} else if (0 == strcmp(all_interface_res[j], "info")) {
				TS_STRING_SET_0(passageways->passageway.host_interface.info);
				memcpy(passageways->passageway.host_interface.info, all_interface_res[i * columns + j], strlen(all_interface_res[i * columns + j]));
			}
		}
		list_add_tail(&passageways->list, head_list);
	}
	sqlite3_free_table(all_interface_res);
	return ret;
}

int32_t db_get_host_interfaces(struct list_head * head_list, ts_host_interface_list_t * p_host_interface_list)
{
	const char * sql_select = "select * from t_host_interface where 1 = 1";
	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	int32_t ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		p_host_interface_list = (ts_host_interface_list_t *) malloc(sizeof(ts_host_interface_list_t));
		for (j = 0; j < ncolumn; j++) {
			if (0 == strcmp(all_result[j], "id")) {
				p_host_interface_list->host_interface.id = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "desc")) {
				TS_STRING_SET_0(p_host_interface_list->host_interface.desc);
				memcpy(p_host_interface_list->host_interface.desc, all_result[i * ncolumn + j], strlen(all_result[i * ncolumn + 1]));
			} else if (0 == strcmp(all_result[j], "type")) {
				p_host_interface_list->host_interface.type = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "info")) {
				TS_STRING_SET_0(p_host_interface_list->host_interface.info);
				memcpy(p_host_interface_list->host_interface.info, all_result[i * ncolumn + j], strlen(all_result[i * ncolumn + j]));
			}
		}
		list_add_tail(&p_host_interface_list->list, head_list);
	}

	sqlite3_free_table(all_result);

	return EXIT_SUCCESS;
}

//

/**
 * get wifi and knx passageways
 */
int32_t db_get_all_lines_counts(struct list_head * head_list, ts_passageway_list_t * passageways, int32_t * counts)
{
	int32_t ret = EXIT_SUCCESS;
	const char *base_sql_get_interface = "select a.id id,a.desc desc,a.type,a.info,b.type type_name "
		"from t_host_interface a ,t_host_interface_type b "
		"where 1 = 1 and (a.type = %d or a.type = %d) and a.type = b.id;";
//	char * sql_select_interface = (char *) calloc(strlen(base_sql_get_interface) + TS_DB_INT_ADD * 2 + 1, sizeof(char));
	char sql_select_interface[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select_interface, base_sql_get_interface, TS_HOST_INTERFACE_TYPE_KNX, TS_HOST_INTERFACE_TYPE_IP);
	ret = base_get_all_lines(head_list, passageways, sql_select_interface, counts);
//	free(sql_select_interface);
	return ret;
}

int32_t db_get_all_lines(struct list_head * head_list, ts_passageway_list_t * passageways)
{
	int32_t ret = EXIT_SUCCESS;
	int32_t count = 0;
	ret = db_get_all_lines_counts(head_list, passageways, &count);
	return ret;
}

/**
 * get the wifi device infomation
 */
//int32_t db_get_wifi_info(int32_t device_id, ts_passageway_t * passway)
int32_t db_get_interface_gateway(int32_t device_id, int32_t *gateway_type, int32_t *gateway_id)
{
	//select the device info
	const char * base_get_base_info = "select a.id,a.gatewayTypeId from t_gateway a,t_device b where a.id = b.gateway and b.device_id = %d";
//	char * sql_get_interface_id = (char *) calloc(strlen(base_get_base_info) + TS_DB_INT_ADD + 1, sizeof(char));
	char sql_get_interface_id[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_get_interface_id, base_get_base_info, device_id);
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_res; //interface results

	//get passway info
	int32_t ret = sc_sqlite3_get_table(sql_get_interface_id, &all_res, &rows, &columns);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_res);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (0 >= rows) {
		ret = ERR_DB_DEVICE_INFO;
		return ret;
	} else {
		*gateway_type = atoi(all_res[columns + 1]);
		*gateway_id = atoi(all_res[columns]); //The gateway id of the device;
	}
	ret = EXIT_SUCCESS;
//	free(sql_get_interface_id);
	sqlite3_free_table(all_res);
	return ret;
}

int32_t db_get_knx_device_curr_value(ts_knx_control_info_t * knx_control_info)
{
	int32_t ret = EXIT_SUCCESS;
	const char * base_sql = "select c.knxDataType ,"
		" (select d.currData from t_host_knx_curr_value d where 1 =1 and d.grp_id = c.id ) currData,"
		" (select d.is_first_use from t_host_knx_curr_value d where 1 =1 and d.grp_id = c.id ) is_first_use,"
		"  (select d.time from t_host_knx_curr_value d where 1 =1 and d.grp_id = c.id ) time"
		" from t_device  a,t_ctrlunit b,t_knx_group c"
		" where a.device_id = b.device "
		"	 and c.ctrlUnit = b.id and a.device_id = %d and (groupAddr='%d/%d/%d' or groupAddr = '%d/%d')";

	int32_t main_3 = 0;
	int32_t mid_3 = 0;
	int32_t logic_3 = 0;
	int32_t main_2 = 0;
	int32_t logic_2 = 0;
	get_3_knx_group_addr(knx_control_info->group_addr, &main_3, &mid_3, &logic_3);
	get_2_knx_group_addr(knx_control_info->group_addr, &main_2, &logic_2);

	//select the device info
	char sql_get_knx_info[TS_DB_SQL_LENGTH] = { 0 };
	//get device id
	sprintf(sql_get_knx_info, base_sql, knx_control_info->home_device_id, main_3, mid_3, logic_3, main_2, logic_2);
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_res; //interface results
	ret = sc_sqlite3_get_table(sql_get_knx_info, &all_res, &rows, &columns);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_res);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	int32_t i = 0, j = 0;
	for (i = 0; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(all_res[j], "currData")) {
				TS_STRING_SET_0(knx_control_info->data);
				if (NULL != all_res[i * columns + j]) {
					memcpy(knx_control_info->data, all_res[i * columns + j], strlen(all_res[i * columns + j]));
				}
			} else if (0 == strcmp(all_res[j], "time")) {
				TS_STRING_SET_0(knx_control_info->time);
				if (NULL != all_res[i * columns + j]) {
					memcpy(knx_control_info->time, all_res[i * columns + j], strlen(all_res[i * columns + j]));
				}
			} else if (0 == strcmp(all_res[j], "knxDataType")) {
				if (NULL != all_res[i * columns + j]) {
					knx_control_info->data_type = atoi(all_res[i * columns + j]);
				}
			} else if (0 == strcmp(all_res[j], "is_first_use")) {
				if (NULL != all_res[i * columns + j]) {
					knx_control_info->is_first_use = atoi(all_res[i * columns + j]);
				}
			}
		}
	}

	sqlite3_free_table(all_res);
	return ret;
}

int32_t db_upd_knx_device_curr_value(ts_knx_control_info_t knx_control_info, int32_t gateway_id)
{
	int ret = EXIT_SUCCESS;
//	get grpId
	const char *b_get_grp_id = "select c.id from t_device a,t_ctrlunit b , t_knx_group c "
		"where 1 = 1 and a.device_id = b.device and b.id = c.ctrlUnit"
		" and (groupAddr=\'%d/%d/%d\' or groupAddr=\'%d/%d\') and a.gateway = %d ";
//changet int16 to char
	int32_t main_3 = 0;
	int32_t mid_3 = 0;
	int32_t logic_3 = 0;
	int32_t main_2 = 0;
	int32_t logic_2 = 0;
	get_3_knx_group_addr(knx_control_info.group_addr, &main_3, &mid_3, &logic_3);
	get_2_knx_group_addr(knx_control_info.group_addr, &main_2, &logic_2);

	char ** all_res;
	int32_t rows = 0;
	int32_t columns = 0;
	char get_grp_id[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(get_grp_id, b_get_grp_id, main_3, mid_3, logic_3, main_2, logic_2, gateway_id);
	ret = sc_sqlite3_get_table(get_grp_id, &all_res, &rows, &columns);
	if (SQLITE_OK != ret) {
		sqlite3_free_table(all_res);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
		sqlite3_free_table(all_res);
		return ret;
	}

	int32_t grp_id = atoi(all_res[1 * columns]);

	//begin trans
	TS_DB_BEGIN_TRANS_FILE

	//del curr value;
	const char * b_del_curr_value = "delete from t_host_knx_curr_value where 1 = 1 and grp_id = %d";
//	char * del_curr_value = (char *) calloc(strlen(b_del_curr_value) + TS_DB_INT_ADD + 1, sizeof(char));
	char del_curr_value[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(del_curr_value, b_del_curr_value, grp_id);

//	ret = sc_sqlite3_exec("begin transaction", 0, 0);
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, del_curr_value, 0, 0);
//	//ts_free(del_curr_value);
	if (SQLITE_OK != ret) {
		sc_sqlite3_exec("rollback transaction", 0, 0);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	//ins curr value
	const char * b_ins_curr_value = "insert into t_host_knx_curr_value (currData,time,grp_id,is_first_use) values ('%s','%s',%d,%d);";
	char int_curr_value[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(int_curr_value, b_ins_curr_value, knx_control_info.data, knx_control_info.time, grp_id, knx_control_info.is_first_use);
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, int_curr_value, 0, 0);
	//ts_free(int_curr_value);
	if (SQLITE_OK != ret) {
		sc_sqlite3_exec("rollback transaction", 0, 0);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	TS_DB_COMMIT_TRANS_FILE
//	ret = sc_sqlite3_exec("commit transaction", 0, 0);

	sqlite3_free_table(all_res);
	return ret;
}

//int32_t db_get_infrared_channel(int32_t device_id, int32_t * channel_id)
int32_t db_get_infrared_device_info(int32_t device_id, int32_t *interface_id, int32_t *gateway_id, int32_t *channel_id)
{
	int32_t ret = EXIT_SUCCESS;
	int32_t flag = TS_FLAG_FALSE;
	int32_t gateway_type = 0;
	ret = get_device_gateway_type(device_id, &gateway_type);
	if (EXIT_SUCCESS != ret) {
		return ret;
	}

	//const char * base_sql = "select b.channel  from  t_device a,t_remotectrl b where a.device_id = b.device and a.device_id = %d";
	const char * base_sql = "select b.channel from  t_device a,t_host_remotectrl b where a.device_id = b.device_id and a.device_id = %d";
//	char * sql_get_knx_info = (char *) calloc(strlen(base_sql) + TS_DB_INT_ADD + 1, sizeof(char));
	char sql_get_knx_info[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_get_knx_info, base_sql, device_id);

	//get info
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_res; //interface results
//	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(sql_get_knx_info, &all_res, &rows, &columns);
//	free(sql_get_knx_info);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_res);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(all_res[j], "channel")) {
				*channel_id = atoi(all_res[i * columns + j]);
				flag = TS_FLAG_TRUE;
			}
		}
	}

	//get is get info
	if (TS_FLAG_TRUE != flag) {
		*channel_id = 0;
		ret = ERR_DB_DEVICE_INFO;
		sqlite3_free_table(all_res);
		return ret;
	}

	//get interface id and gateway id
	db_get_interface_gateway(device_id, interface_id, gateway_id);
	sqlite3_free_table(all_res);
	return ret;
}

//int32_t db_get_serial_info(int32_t device_id, char * file_name, int32_t * zone)
//int32_t db_get_serial_device_info(int32_t comm_id, int32_t *gateway_id, int32_t *zone, char *file_name, char *comm_port)
int32_t db_get_devtype_and_protrol(int32_t comm_id, char *file_name, int32_t * device_type)
{
	int32_t ret = EXIT_SUCCESS;

	const char * base_sql = "select d.file_name,d.is_self "
		" from t_host_device_serial b ,t_serial_board d where 1 = 1"
		" and b.serial = %d and d.device = b.device";
//	char * sql_get_serial_info = (char *) calloc(strlen(base_sql) + TS_DB_INT_ADD * 2, sizeof(char));
	char sql_get_serial_info[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_get_serial_info, base_sql, comm_id);

	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_res; //interface results
	ret = sc_sqlite3_get_table(sql_get_serial_info, &all_res, &rows, &columns);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_res);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(all_res[j], "file_name")) {
				if (NULL != all_res[i * columns + j]) {
					memcpy(file_name, all_res[i * columns + j], strlen(all_res[i * columns + j]));
				}
			} else if (0 == strcmp(all_res[j], "is_self")) {
				int32_t t = atoi(all_res[i * columns + j]);
				if (0 == t) {
					*device_type = COM;
				} else {
					*device_type = CUSTOMER_COM;
				}

			}
		}
	}
//	free(sql_get_serial_info);

	sqlite3_free_table(all_res);
	return ret;
}

int32_t db_get_serial_device_info(int32_t device_id, int32_t *comm_id, int32_t *gateway_id, int32_t *zone, char *file_name, char *comm_port)
{
	int32_t ret = EXIT_SUCCESS;

	const char * base_sql = "select a.device_id,a.gateway,a.zone,c.comm_port ,d.file_name,c.id comm_id"
		" from t_device a,t_host_device_serial b ,t_host_serial_info c ,t_serial_board d"
		" where a.device_id  = %d "
		" and b.device = a.device_id "
		" and c.id = b.serial and a.device_id = d.device";
//	char * sql_get_serial_info = (char *) calloc(strlen(base_sql) + TS_DB_INT_ADD * 2, sizeof(char));
	char sql_get_serial_info[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_get_serial_info, base_sql, device_id, TS_HOST_INTERFACE_TYPE_SERIAL);

	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_res; //interface results
	ret = sc_sqlite3_get_table(sql_get_serial_info, &all_res, &rows, &columns);
//	free(sql_get_serial_info);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_res);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == all_res[i * columns + j]) {
				continue;
			}
			if (0 == strcmp(all_res[j], "zone")) {
				*zone = atoi(all_res[i * columns + j]);
			} else if (0 == strcmp(all_res[j], "file_name")) {
				if (NULL != all_res[i * columns + j]) {
					//	memcpy(file_name, all_res[i * columns + j], strlen(all_res[i * columns + j]));
					strcpy(file_name, all_res[i * columns + j]);
				}
			} else if (0 == strcmp(all_res[j], "gateway")) {
				*gateway_id = atoi(all_res[i * columns + j]);
			} else if (0 == strcmp(all_res[j], "comm_port")) {
				if (NULL != all_res[i * columns + j]) {
					memcpy(comm_port, all_res[i * columns + j], strlen(all_res[i * columns + j]));
				}
			} else if (0 == strcmp(all_res[j], "comm_id")) {
				*comm_id = atoi(all_res[i * columns + j]);
			}
		}
	}

	sqlite3_free_table(all_res);
	return ret;
}

int32_t db_get_knx_groups(int32_t gateway_id, struct list_head * head, ts_knx_control_info_list_t * knx_group_list, int32_t permission)
{
	int32_t ret = EXIT_SUCCESS;
	const char * base_sql = "select c.device_id,c.deviceType,a.* from t_knx_group a,t_ctrlunit b,t_device c "
		"where (a.permission = %d or a.permission = %d) "
		"and b.device = c.device_id and a.ctrlUnit = b.id";
//	char * sql_get_knx_info = (char *) calloc(strlen(base_sql) + TS_DB_INT_ADD * 2 + 1, sizeof(char));
	char sql_get_knx_info[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_get_knx_info, base_sql, TS_KNX_GROUP_PERMISSION_RW, permission);

	//get data from db
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_result;
	ret = sc_sqlite3_get_table(sql_get_knx_info, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		knx_group_list = (ts_knx_control_info_list_t *) calloc(sizeof(ts_knx_control_info_list_t), sizeof(char));
		for (j = 0; j < columns; j++) {
			if (NULL == all_result[i * columns + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "device_id")) {
				knx_group_list->knx_info.home_device_id = atoi(all_result[i * columns + j]);
			} else if (0 == strcmp(all_result[j], "groupAddr")) {
				get_knx_group_info(all_result[i * columns + j], &knx_group_list->knx_info.group_addr);
			} else if (0 == strcmp(all_result[j], "deviceType")) {
				knx_group_list->knx_info.device_type = atoi(all_result[i * columns + j]);
			} else if (0 == strcmp(all_result[j], "grpType")) {
				knx_group_list->knx_info.cmd_type = atoi(all_result[i * columns + j]);
			} else if (0 == strcmp(all_result[j], "dataType")) {
				knx_group_list->knx_info.data_type = atoi(all_result[i * columns + j]);
			} else if (0 == strcmp(all_result[j], "curData")) {
				TS_STRING_SET_0(knx_group_list->knx_info.data);
				memcpy(knx_group_list->knx_info.data, all_result[i * columns + j], strlen(all_result[i * columns + j]));
				//get data length
				knx_group_list->knx_info.data_len = strlen(all_result[i * columns + j]);
			} else if (0 == strcmp(all_result[j], "date")) {
				TS_STRING_SET_0(knx_group_list->knx_info.time);
				memcpy(knx_group_list->knx_info.time, all_result[i * columns + j], strlen((const char *) all_result[i * columns + j]));
			}
		}
		list_add_tail(&knx_group_list->list, head);
	}
//	free(sql_get_knx_info);
	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_protocol_add_gateway_of_line(int32_t gateway_id, int32_t interface_id)
{

	int32_t ret = EXIT_SUCCESS;
	int32_t gateway_type = 0;
	int32_t interface_type = 0;
	int32_t rows = 0;
	int32_t columns = 0;
	char **all_result;

	//get is the gateway exist
	const char * base_passageway_exist = "select id from t_host_gateway_interface where gateway_id = %d and interface_id = %d";
//	char *sql_exist = (char *) calloc(strlen(base_passageway_exist) + TS_DB_INT_ADD * 2, sizeof(char));
	char sql_exist[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_exist, base_passageway_exist, gateway_id, interface_id);
	ret = sc_sqlite3_get_table(sql_exist, &all_result, &rows, &columns);
//	free(sql_exist);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	sqlite3_free_table(all_result);
	if (rows >= 1) {
		ret = ERR_DB_PASSAGEWAY_EXIT;
		return ret;
	}

	//	get gateway type
	const char * base_gateway_info = "select gatewayTypeId from t_gateway where id = %d";
//	char *sql_get_gateway_info = (char *) calloc(strlen(base_gateway_info) + TS_DB_INT_ADD, sizeof(char));
	char sql_get_gateway_info[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_get_gateway_info, base_gateway_info, gateway_id);
	ret = sc_sqlite3_get_table(sql_get_gateway_info, &all_result, &rows, &columns);
//	free(sql_get_gateway_info);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (1 == rows) {
		gateway_type = atoi(all_result[1 * columns]);
	} else {
		ret = ERR_DB_GET_GATEWAY_INFO;
		sqlite3_free_table(all_result);
		return ret;
	}
	sqlite3_free_table(all_result);

	//get interface type
	const char * base_interface_type = "select type from t_host_interface where id = %d";
//	char *sql_interface_type = (char *) calloc(strlen(base_interface_type) + TS_DB_INT_ADD, sizeof(char));
	char sql_interface_type[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_interface_type, base_interface_type, interface_id);
	ret = sc_sqlite3_get_table(sql_interface_type, &all_result, &rows, &columns);
//	free(sql_interface_type);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (1 == rows) {
		interface_type = atoi(all_result[1 * columns]);
	} else {
		ret = ERR_DB_GET_INTERFACE_INFO;
		sqlite3_free_table(all_result);
		return ret;
	}

	//get whether gateway type can connect with interface
	int32_t can_conn = TS_FLAG_FALSE;
	if ((TS_GATEWAY_TYPE_WIFI == gateway_type) && (TS_HOST_INTERFACE_TYPE_IP == interface_type)) {
		can_conn = TS_FLAG_TRUE;
	} else if ((TS_GATEWAY_TYPE_HOST == gateway_type) && (TS_HOST_INTERFACE_TYPE_SERIAL == interface_type)) {
		can_conn = TS_FLAG_TRUE;
	} else if ((TS_GATEWAY_TYPE_HOST == gateway_type) && (TS_HOST_INTERFACE_TYPE_INFRARED == interface_type)) {
		can_conn = TS_FLAG_TRUE;
	} else if ((TS_GATEWAY_TYPE_KNX == gateway_type) && (TS_HOST_INTERFACE_TYPE_KNX == interface_type)) {
		can_conn = TS_FLAG_TRUE;
	} else {
		can_conn = TS_FLAG_FALSE;
	}

	if (TS_FLAG_FALSE == can_conn) {
		ret = ERR_DB_GATEWAY_INTERFACE_CONN;
		sqlite3_free_table(all_result);
		return ret;
	}
	sqlite3_free_table(all_result);

	//conn gateway and interface
	char *base_sql_insert = "insert into t_host_gateway_interface (gateway_id,interface_id) values (%d,%d);";
//	char *sql_insert = (char *) calloc(strlen(base_sql_insert) + TS_DB_INT_ADD * 2, sizeof(char));
	char sql_insert[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_insert, base_sql_insert, gateway_id, interface_id);
	ret = sc_sqlite3_exec(sql_insert, 0, 0);
//	free(sql_insert);
	if (ret != 0) {
		ret = ERR_DB_INSERT;
		return ret;
	}

	return ret;
}

int32_t db_protocol_del_gateway_of_line(int32_t gateway_id)
{
	int32_t ret = EXIT_SUCCESS;
	char *base_sql_del = "delete from t_host_gateway_interface where gateway_id = %d;";
//	char *sql_del = (char *) calloc(strlen(base_sql_del) + 4, sizeof(char));
	char sql_del[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_del, base_sql_del, gateway_id);
	ret = sc_sqlite3_exec(sql_del, 0, 0);
	if (ret != 0) {
		ret = ERR_DB_INSERT;
		return ret;
	}
//	free(sql_del);
	return ret;
}

int32_t db_protocol_modify_gateway_of_line(int32_t gateway_id, char *ip, int32_t port)
{
	int32_t ret = EXIT_SUCCESS;
	char *base_sql_upd = "update t_gateway set gatewayIp = \'%s\' , gatewayPort = %d where id = %d;";
//	char *sql_upd = (char *) calloc(strlen(base_sql_upd) + TS_DB_INT_ADD + 1, sizeof(char));
	char sql_upd[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_upd, base_sql_upd, ip, port, gateway_id);
	ret = sc_sqlite3_exec(sql_upd, 0, 0);
	if (ret != 0) {
		ret = ERR_DB_INSERT;
		return ret;
	}
//	free(sql_upd);
	return ret;
}

int32_t db_get_all_serial_details(struct list_head * head_list, ts_host_interface_serial_info_list * serial_infos, int32_t * counts)
{
	const char * base_get_serial_infos = "SELECT * FROM t_host_serial_info";
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **s_serial_info;
	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(base_get_serial_infos, &s_serial_info, &rows, &columns);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_serial_info);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	*counts = rows;

	//get counts
	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		serial_infos = (ts_host_interface_serial_info_list *) calloc(sizeof(ts_host_interface_serial_info_list), sizeof(char));
		for (j = 0; j < columns; j++) {
			if (NULL == s_serial_info[i * columns + j]) {
				continue;
			}
			if (0 == strcmp(s_serial_info[j], "id")) {
				serial_infos->host_serial.id = atoi(s_serial_info[i * columns + j]);
			}
			if (0 == strcmp(s_serial_info[j], "interface_id")) {
				serial_infos->host_serial.interface_id = atoi(s_serial_info[i * columns + j]);
			}
			if (0 == strcmp(s_serial_info[j], "baud_rate")) {
				serial_infos->host_serial.baud_rate = atoi(s_serial_info[i * columns + j]);
			}
			if (0 == strcmp(s_serial_info[j], "data_bit")) {
				serial_infos->host_serial.data_bit = atoi(s_serial_info[i * columns + j]);
			}
			if (0 == strcmp(s_serial_info[j], "stop_bit")) {
				serial_infos->host_serial.stop_bit = atof(s_serial_info[i * columns + j]);
			}
			if (0 == strcmp(s_serial_info[j], "parity_check")) {
				serial_infos->host_serial.parity_check = atoi(s_serial_info[i * columns + j]);
			}
			if (0 == strcmp(s_serial_info[j], "comm_id")) {
				serial_infos->host_serial.comm_id = atoi(s_serial_info[i * columns + j]);
			}
			if (0 == strcmp(s_serial_info[j], "comm_port")) {
				memset(serial_infos->host_serial.comm_port, 0, sizeof(serial_infos->host_serial.comm_port));
				strcpy(serial_infos->host_serial.comm_port, s_serial_info[i * columns + j]);
			}
		}
		list_add_tail(&serial_infos->list, head_list);
	}

	sqlite3_free_table(s_serial_info);
	return ret;
}

int32_t db_get_serial_details(ts_host_interface_serial_info * serial_info)
{
	int32_t ret = EXIT_SUCCESS;
	if (NULL == serial_info) {
		ret = EXIT_FAILURE;
		return ret;
	}

	const char * base_get_serial_infos = "SELECT * FROM t_host_serial_info where id = %d;";
//	char * get_serial_info = (char *) calloc(strlen(base_get_serial_infos) + TS_DB_INT_ADD, sizeof(char));
	char get_serial_info[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(get_serial_info, base_get_serial_infos, serial_info->id);

	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **s_serial_info;
	ret = sc_sqlite3_get_table(get_serial_info, &s_serial_info, &rows, &columns);
//	free(get_serial_info);
	//query error
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_serial_info);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	//no records
	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
//			sqlite3_errmsg(sc_db), ret);
		return ret;
	}

	//get counts
	int32_t j = 0;
	for (j = 0; j < columns; j++) {
		if (NULL == s_serial_info[columns + j]) {
			continue;
		}
		if (0 == strcmp(s_serial_info[j], "id")) {
			serial_info->id = atoi(s_serial_info[columns + j]);
		}
		if (0 == strcmp(s_serial_info[j], "interface_id")) {
			serial_info->interface_id = atoi(s_serial_info[columns + j]);
		}
		if (0 == strcmp(s_serial_info[j], "baud_rate")) {
			serial_info->baud_rate = atoi(s_serial_info[columns + j]);
		}
		if (0 == strcmp(s_serial_info[j], "data_bit")) {
			serial_info->data_bit = atoi(s_serial_info[columns + j]);
		}
		if (0 == strcmp(s_serial_info[j], "stop_bit")) {
			serial_info->stop_bit = atof(s_serial_info[columns + j]);
		}
		if (0 == strcmp(s_serial_info[j], "parity_check")) {
			serial_info->parity_check = atoi(s_serial_info[columns + j]);
		}
		if (0 == strcmp(s_serial_info[j], "comm_id")) {
			serial_info->comm_id = atoi(s_serial_info[columns + j]);
		}
		if (0 == strcmp(s_serial_info[j], "comm_port")) {
			memset(serial_info->comm_port, 0, sizeof(serial_info->comm_port));
			strcpy(serial_info->comm_port, s_serial_info[columns + j]);
		}
	}

	sqlite3_free_table(s_serial_info);
	return ret;
}

int32_t db_set_serial_details(ts_host_interface_serial_info serial_info)
{
	int32_t ret = EXIT_SUCCESS;
	//whether the recode is in db
	ts_host_interface_serial_info tmp;
	tmp.id = serial_info.id;
	ret = db_get_serial_details(&tmp);
	if (ERR_DB_NO_RECORDS == ret) {
		ret = EXIT_SUCCESS;

		//insert host_interface
		const char * base_insert_inter = "insert into t_host_interface (desc,type,info) values (\"serial\",%d,'');";
//		char * insert_inter = (char *) calloc(strlen(base_insert_inter) + TS_DB_INT_ADD, sizeof(char));
		char insert_inter[TS_DB_SQL_LENGTH] = { 0 };
		sprintf(insert_inter, base_insert_inter, TS_HOST_INTERFACE_TYPE_SERIAL);
		ret = sc_sqlite3_exec(insert_inter, 0, 0);
//		free(insert_inter);
		if (SQLITE_OK != ret) {
			int32_t error_code = ERR_DB_UPDATE;
			ret = error_code;
		}

		//get max id
		int max_id = 0;
		ret = get_table_max_id("t_host_interface", "id", &max_id);
		if ((EXIT_SUCCESS != ret) || (SQLITE_OK != ret)) {
			return ret;
		}

		//insert host_serial_info
		const char * base_insert_serial_info = "insert into t_host_serial_info (interface_id,baud_rate,data_bit,stop_bit,parity_check,"
			"comm_id,comm_port) values (%d,%d,%d,%f,%d,%d,'%s');";
//		char * insert_ser_info = (char *) calloc(strlen(base_insert_serial_info) + TS_DB_INT_ADD * 6 + sizeof(ts_host_interface_serial_info), sizeof(char));
		char insert_ser_info[TS_DB_SQL_LENGTH] = { 0 };
		sprintf(insert_ser_info, base_insert_serial_info, max_id, serial_info.baud_rate, serial_info.data_bit, serial_info.stop_bit, serial_info.parity_check, serial_info.comm_id, serial_info.comm_port);
		ret = sc_sqlite3_exec(insert_ser_info, 0, 0);
//		free(insert_ser_info);
		if (SQLITE_OK != ret) {
			int32_t error_code = ERR_DB_UPDATE;
			ret = error_code;
		}

		return ret;
	} else {
		//update
		const char * base_upd_serial = "update t_host_serial_info set "
			" baud_rate = %d ,data_bit = %d ,stop_bit = %f ,parity_check = %d, comm_id = %d ,"
			" comm_port = \'%s\' where id = %d;";
//		char * upd_serial = (char *) calloc(strlen(base_upd_serial) + TS_DB_INT_ADD * 7 + sizeof(ts_host_interface_serial_info), sizeof(char));
		char upd_serial[TS_DB_SQL_LENGTH] = { 0 };
		sprintf(upd_serial, base_upd_serial, serial_info.baud_rate, serial_info.data_bit, serial_info.stop_bit, serial_info.parity_check, serial_info.comm_id, serial_info.comm_port, serial_info.id);

		//upd db
		ret = sc_sqlite3_exec(upd_serial, 0, 0);
//		free(upd_serial);
		if (SQLITE_OK != ret) {
			int32_t error_code = ERR_DB_UPDATE;
			ret = error_code;
		}
	}

	return ret;
}

int32_t db_del_serial_details(ts_host_interface_serial_info serial_info)
{
	int32_t ret = EXIT_SUCCESS;

	//del
	const char * base_del_serial = "delete from t_host_serial_info where id = %d;";
	const char * base_del_inter = "delete from t_host_interface where id = "
		"(select interface_id from t_host_serial_info where id = %d);";

//	char * del_inter = (char *) calloc(strlen(base_del_inter) + TS_DB_INT_ADD, sizeof(char));
	char del_inter[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(del_inter, base_del_inter, serial_info.id);

//	ret = sc_sqlite3_exec("begin transaction", 0, 0);
	TS_DB_BEGIN_TRANS_FILE
	//del interface
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, del_inter, 0, 0);
	if (SQLITE_OK != ret) {
		int32_t error_code = ERR_DB_UPDATE;
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
		ret = error_code;
	}

//	char * del_serial = (char *) calloc(strlen(base_del_serial) + TS_DB_INT_ADD, sizeof(char));
	char del_serial[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(del_serial, base_del_serial, serial_info.id);

	//del serial
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, del_serial, 0, 0);
	if (SQLITE_OK != ret) {
		int32_t error_code = ERR_DB_UPDATE;
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
		ret = error_code;
	}
	TS_DB_COMMIT_TRANS_FILE
//	ret = sc_sqlite3_exec("commit transaction", 0, 0);
	return ret;
}

//get knx group data type by addr and gateway id
int32_t db_get_knx_group_type(int32_t gateway_id, ts_knx_control_info_t * knx_control)
{
	int32_t ret = EXIT_SUCCESS;
	const char * base_query_sql = "select a.knxDataType,c.device_id "
		" from t_knx_group a,t_ctrlunit b ,t_device c "
		" where a.ctrlUnit = b.id  and b.device = c.device_id and c.gateway = %d "
		" and (a.groupAddr='%d/%d/%d' or a.groupAddr = '%d/%d');";
	int32_t main_3 = 0;
	int32_t mid_3 = 0;
	int32_t logic_3 = 0;
	int32_t main_2 = 0;
	int32_t logic_2 = 0;
	get_3_knx_group_addr(knx_control->group_addr, &main_3, &mid_3, &logic_3);
	get_2_knx_group_addr(knx_control->group_addr, &main_2, &logic_2);

//	char * query_sql = (char *) calloc(strlen(base_query_sql) + TS_DB_INT_ADD * 6 + 1, sizeof(char));

	char query_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(query_sql, base_query_sql, gateway_id, main_3, mid_3, logic_3, main_2, logic_2);

	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **s_results;
	ret = sc_sqlite3_get_table(query_sql, &s_results, &rows, &columns);
//	free(query_sql);
	//query error
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	//no records
	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	//get counts
	int32_t j = 0;
	int32_t i = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == s_results[i * columns + j]) {
				continue;
			} else {
				if (0 == strcmp(s_results[j], "knxDataType")) {
					knx_control->data_type = atoi(s_results[i * columns + j]);
				} else if (0 == strcmp(s_results[j], "device_id")) {
					knx_control->home_device_id = atoi(s_results[i * columns + j]);
				}
			}
		}
	}

	sqlite3_free_table(s_results);
	return ret;
}

//get gateway type by id
int32_t db_get_gateway_by_id(int32_t geteway_id, int32_t * gateway_type)
{
	int32_t ret = EXIT_SUCCESS;

	if (0 == geteway_id) {
		*gateway_type = TS_GATEWAY_TYPE_HOST;
		return ret;
	}

	const char * base_query_sql = "select gatewayTypeId from t_gateway where id = %d";
//	char * query_sql = (char *) calloc(strlen(base_query_sql) + TS_DB_INT_ADD, sizeof(char));
	char query_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(query_sql, base_query_sql, geteway_id);

	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **s_results;
	ret = sc_sqlite3_get_table(query_sql, &s_results, &rows, &columns);
//	free(query_sql);
	//query error
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	//no records
	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
		sqlite3_free_table(s_results);
		return ret;
	}

	//get counts
	int32_t j = 0;
	int32_t i = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == s_results[i * columns + j]) {
				continue;
			} else {
				if (0 == strcmp(s_results[j], "gatewayTypeId")) {
					*gateway_type = atoi(s_results[i * columns + j]);
				}
			}
		}
	}

	sqlite3_free_table(s_results);
	return ret;
}

static int32_t get_next_readable_group_by_sql(int32_t * n_group_id, int32_t * gateway_id, int16_t * gateway_addr, const char * query_sql)
{

	int32_t ret = EXIT_SUCCESS;
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **s_results;
	ret = sc_sqlite3_get_table(query_sql, &s_results, &rows, &columns);

	//query error
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	//no records
	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	//get counts
	int32_t j = 0;
	int32_t i = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == s_results[i * columns + j]) {
				continue;
			} else {
				if (0 == strcmp(s_results[j], "n_group_id")) {
					*n_group_id = atoi(s_results[i * columns + j]);
				}
				if (0 == strcmp(s_results[j], "gatewayId")) {
					*gateway_id = atoi(s_results[i * columns + j]);
				}
				if (0 == strcmp(s_results[j], "grp_addr")) {
					//only 3 parts
					get_knx_group_info(s_results[i * columns + j], (u_int16_t *) gateway_addr);
//					strcpy(gateway_addr, s_results[i * columns + j]);
//					*gateway_addr = (int16_t) atoi(s_results[i * columns + j]);
				}
			}
		}
	}

	sqlite3_free_table(s_results);
	return ret;

}

//get next readable group
int32_t db_get_next_readable_group(int32_t * n_group_id, int32_t * gateway_id, int16_t * grp_addr)
{
	int32_t ret = EXIT_SUCCESS;
	const char * base_get_sql1 = "select a.id n_group_id,d.id gatewayId,a.groupAddr grp_addr "
		" from t_knx_group a,t_ctrlunit b ,t_device c, t_gateway d "
		" where a.ctrlUnit = b.id  and b.device = c.device_id and c.gateway = d.id "
		" and (a.permission = %d or a.permission = %d) and a.id > %d order by a.id limit 1;";
//	char* get_sql1 = (char *) calloc(strlen(base_get_sql1) + TS_DB_INT_ADD * 3, sizeof(char));
	char get_sql1[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(get_sql1, base_get_sql1, TS_KNX_GROUP_PERMISSION_READ, TS_KNX_GROUP_PERMISSION_RW, *n_group_id);

	ret = get_next_readable_group_by_sql(n_group_id, gateway_id, grp_addr, get_sql1);
//	free(get_sql1);

	if (ret == ERR_DB_NO_RECORDS) {
		const char * base_get_sql2 = "select a.id n_group_id,d.id gatewayId,a.groupAddr grp_addr "
			" from t_knx_group a,t_ctrlunit b ,t_device c, t_gateway d "
			" where a.ctrlUnit = b.id  and b.device = c.device_id and c.gateway = d.id "
			" and (a.permission = %d or a.permission = %d) order by a.id limit 1;";
//		char* get_sql2 = (char *) calloc(strlen(base_get_sql2) + TS_DB_INT_ADD * 2, sizeof(char));
		char get_sql2[TS_DB_SQL_LENGTH] = { 0 };
		sprintf(get_sql2, base_get_sql2, TS_KNX_GROUP_PERMISSION_READ, TS_KNX_GROUP_PERMISSION_RW);

		ret = get_next_readable_group_by_sql(n_group_id, gateway_id, grp_addr, get_sql2);
//		free(get_sql2);
		if (EXIT_SUCCESS != ret) {
			return ret;
		}

	} else if (EXIT_SUCCESS != ret) {
		return ret;
	}
	return ret;
}

//db_get_who_am i
int32_t db_get_who_am_i(int32_t * who)
{
	int32_t ret = EXIT_SUCCESS;
	const char * query_sql = "select who_am_i from t_host_base_info where 1 = 1 limit 1";
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **s_results;
	ret = sc_sqlite3_get_table(query_sql, &s_results, &rows, &columns);

	//query error
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	//no records
	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
		sqlite3_free_table(s_results);
		return ret;
	}

	//get counts
	int32_t j = 0;
	int32_t i = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == s_results[i * columns + j]) {
				continue;
			} else {
				if (0 == strcmp(s_results[j], "who_am_i")) {
					*who = atoi(s_results[i * columns + j]);
				}
			}
		}
	}
	sqlite3_free_table(s_results);

	return ret;
}

static char char2dec(char c)
{
	char n = 0;
	char lA = 'A';
	char lZ = 'Z';
	char l_a = 'a';
	char l_z = 'z';
	char l0 = '0';
	char l9 = '9';
	if ((c >= lA) && (c <= lZ)) {
		n = c - lA + 10;
	} else if ((c >= l_a) && (c <= l_z)) {
		n = c - l_a + 10;
	} else if ((c >= l0) && (c <= l9)) {
		n = c - l0;
	}
	return n;
}

void string_2_hex(char * text, char * result)
{
	int32_t len = strlen(text) / 2;
	int32_t i = 0;
	for (i = 0; i < len; i++) {
		result[i] = (char) char2dec(text[2 * i]) * 16 + char2dec(text[2 * i + 1]);
	}
}

void hex_2_string(char * text, int32_t len, char *result)
{
	int i = 0;
	char tmp[3] = { 0 };
	for (i = 0; i < len; i++) {
		int32_t a = (u_char) text[i] / 16;
		int32_t b = (u_char) text[i] % 16;
		sprintf(tmp, "%X%X", a, b);
		result[2 * i] = tmp[0];
		result[2 * i + 1] = tmp[1];
	}
}

/**
 * record infrared study code
 * @key code:the key code of the gateway
 */
int32_t db_set_infrared_code(int32_t gateway_id, int32_t key_code, char * text, int32_t type, int32_t len)
{
	int32_t ret = EXIT_SUCCESS;

	const char * b_sql_get_code = "select * from t_host_remote_control_key where gateway_id = %d and key_code = %d;";
	char * s_get_sql = (char *) calloc(strlen(b_sql_get_code) + TS_DB_INT_ADD * 2, sizeof(char));
//	char s_get_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(s_get_sql, b_sql_get_code, gateway_id, key_code);
	int32_t rows = 0;
	int32_t columns = 0;
	char ** s_results;

	ret = sc_sqlite3_get_table(s_get_sql, &s_results, &rows, &columns);
	ts_free(s_get_sql);
	if (SQLITE_OK != ret) {
		ret = ERR_DB_QUERY;
		return ret;
	}

	//the column name of db
	char text_name[20] = { 0 };
	char text_len[10] = { 0 };

	if (TS_INFRCODE_TYPE_0 == type) {
		strcpy(text_name, "text");
		strcpy(text_len, "len");
	} else {
		strcpy(text_name, "text1");
		strcpy(text_len, "len1");
	}

	//update
	if (rows > 0) {
		const char * b_upd_sql = "update t_host_remote_control_key set %s = '%s',%s = %d  "
			"where gateway_id = %d and key_code = %d;";
		int32_t upd_len = strlen(b_upd_sql) + TS_DB_INT_ADD * 3 + strlen(text_name) + strlen(text_len) + len * 2 + 1;
		char * upd_sql = NULL;
		do {
			upd_sql = (char*) calloc(upd_len, sizeof(char));
		} while (NULL == upd_sql);
		char * text2save = (char *) calloc(len * 2, sizeof(char));
		hex_2_string(text, len, text2save);
		sprintf(upd_sql, b_upd_sql, text_name, text2save, text_len, len, gateway_id, key_code);

		ret = sc_sqlite3_exec(upd_sql, 0, 0);
		ts_free(text2save);
		ts_free(upd_sql);
		if (SQLITE_OK != ret) {
			ret = ERR_DB_UPDATE;
			return ret;
		}
	} else { //insert
		const char * b_sql = "insert into t_host_remote_control_key (gateway_id,key_code,%s,%s) values (%d,%d,'%s',%d);";
		//char * s_sql = (char *) calloc(len * 2 + strlen(b_sql) + strlen(text_name) + strlen(text_len) + TS_DB_INT_ADD * 3 + 1, sizeof(char));
		int32_t ins_len = len * 2 + strlen(b_sql) + strlen(text_name) + strlen(text_len) + TS_DB_INT_ADD * 3 + 1;
		char * s_sql = NULL;
		do {
			s_sql = (char*) calloc(ins_len, sizeof(char));
		} while (NULL == s_sql);
//		char s_sql[TS_DB_SQL_LENGTH] = { 0 };
		char * text2save = (char *) calloc(len * 2 + 1, sizeof(char));
		hex_2_string(text, len, text2save);
		sprintf(s_sql, b_sql, text_name, text_len, gateway_id, key_code, text2save, len);

		ret = sc_sqlite3_exec(s_sql, 0, 0);
		ts_free(text2save);
		ts_free(s_sql);
		if (SQLITE_OK != ret) {
			ret = ERR_DB_INSERT;
			return ret;
		}
	}
	return ret;
}

/**
 * get infrared study code
 * @key code:the key code of the gateway
 */
int32_t db_get_infrared_code(int32_t gateway_id, int32_t key_code, char * text, int32_t type, int32_t * len)
{
	int32_t ret = EXIT_SUCCESS;

	//the column name of db
	char text_name[20] = { 0 };
	char text_len[10] = { 0 };

	if (TS_INFRCODE_TYPE_0 == type) {
		strcpy(text_name, "text");
		strcpy(text_len, "len");
	} else {
		strcpy(text_name, "text1");
		strcpy(text_len, "len1");
	}

	const char * b_query_sql = "select %s,%s from t_host_remote_control_key where gateway_id = %d and key_code = %d;";
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
//	char * query_sql = (char *) calloc(strlen(b_query_sql) + TS_DB_INT_ADD * 2, sizeof(char));
	char query_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(query_sql, b_query_sql, text_name, text_len, gateway_id, key_code);

	char **s_results;
	ret = sc_sqlite3_get_table(query_sql, &s_results, &rows, &columns);
//	free(query_sql);
	//query error
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	//no records
	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	//get counts
	int32_t j = 0;
	int32_t i = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == s_results[i * columns + j]) {
				continue;
			} else {
				if (0 == strcmp(s_results[j], text_name)) {
					string_2_hex(s_results[i * columns + j], text);
				}
				if (0 == strcmp(s_results[j], text_len)) {
					*len = atoi(s_results[i * columns + j]);
				}
			}
		}
	}
	sqlite3_free_table(s_results);

	return ret;
}

int32_t db_get_serial_datagram(int32_t deviceid, int32_t key_id, char * text, int32_t * len)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_query_sql = "select functionCode from t_serial_key where id = %d;";
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
//	char * query_sql = (char *) calloc(strlen(b_query_sql) + TS_DB_INT_ADD * 2, sizeof(char));
	char query_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(query_sql, b_query_sql, key_id);

	char **s_results;
	ret = sc_sqlite3_get_table(query_sql, &s_results, &rows, &columns);
//	free(query_sql);
	//query error
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	//no records
	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	//get counts
	int32_t j = 0;
	int32_t i = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == s_results[i * columns + j]) {
				continue;
			} else {
				if (0 == strcmp(s_results[j], "functionCode")) {
					//*len = atoi(s_results[i * columns + j]);
					string_2_hex(s_results[i * columns + j], text);
					*len = strlen(s_results[i * columns + j]) / 2;
				}
			}
		}
	}
	sqlite3_free_table(s_results);

	return ret;
}

int32_t db_get_serial_device_curr(int32_t device_id, int32_t *type_id, int32_t *zone, char * curr_state)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_query_sql = "select * from t_host_serial_curr_state where device_id = %d;";
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char query_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(query_sql, b_query_sql, device_id);

	char **s_results;
	ret = sc_sqlite3_get_table(query_sql, &s_results, &rows, &columns);
	//query error
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	//no records
	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	//get counts
	int32_t j = 0;
	int32_t i = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == s_results[i * columns + j]) {
				continue;
			} else {
				if (0 == strcmp(s_results[j], "value")) {
					strcpy(curr_state, s_results[i * columns + j]);
				} else if (0 == strcmp(s_results[j], "type")) {
					*type_id = atoi(s_results[i * columns + j]);
				} else if (0 == strcmp(s_results[j], "zone")) {
					*zone = atoi(s_results[i * columns + j]);
				}
			}
		}
	}
	sqlite3_free_table(s_results);

	return ret;
}

int32_t db_set_serial_device_curr(int32_t device_id, int32_t type_id, int32_t zone, const char * curr_state)
{
	int32_t ret = EXIT_SUCCESS;
	//delete
	const char * b_del_sql = "delete from t_host_serial_curr_state where device_id = %d;";
//	char * s_del_sql = (char *) calloc(strlen(b_del_sql) + TS_DB_INT_ADD, sizeof(char));
	char s_del_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(s_del_sql, b_del_sql, device_id);
//	ret = sc_sqlite3_exec("begin transaction", 0, 0);

	TS_DB_BEGIN_TRANS_FILE

	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, s_del_sql, 0, 0);
//	free(s_del_sql);
	if (SQLITE_OK != ret) {
		int32_t error_code = ERR_DB_DELETE;
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
		ret = error_code;
	}

	//insert
	const char * b_ins_sql = "insert into t_host_serial_curr_state (device_id ,value,type,zone ) values (%d,'%s',%d,%d);";
//	char * s_ins_sql = (char *) calloc(strlen(b_ins_sql) + TS_DB_INT_ADD + strlen(curr_state), sizeof(char));
	char s_ins_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(s_ins_sql, b_ins_sql, device_id, curr_state, type_id, zone);
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, s_ins_sql, 0, 0);
//	free(s_ins_sql);
	if (SQLITE_OK != ret) {
		int32_t error_code = ERR_DB_INSERT;
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
		ret = error_code;
	}
	TS_DB_COMMIT_TRANS_FILE
//	ret = sc_sqlite3_exec("commit transaction", 0, 0);

	return ret;

}

//get device type by device id
int32_t db_get_serial_device_type_by_id(int32_t * device_type, int32_t device_id)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_query_sql = "select is_self from t_serial_board where device = %d;";
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
//	char * query_sql = (char *) calloc(strlen(b_query_sql) + TS_DB_INT_ADD * 2, sizeof(char));
	char query_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(query_sql, b_query_sql, device_id);

	char **s_results;
	ret = sc_sqlite3_get_table(query_sql, &s_results, &rows, &columns);
//	free(query_sql);
	//query error
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	//no records
	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	//get counts
	int32_t j = 0;
	int32_t i = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == s_results[i * columns + j]) {
				continue;
			} else {
				if (0 == strcmp(s_results[j], "is_self")) {
					int32_t t = atoi(s_results[i * columns + j]);
					if (0 == t) {
						*device_type = COM;
					} else {
						*device_type = CUSTOMER_COM;
					}
				}
			}
		}
	}
	sqlite3_free_table(s_results);
	return ret;
}

int32_t db_get_device_type_by_id(int32_t * device_type, int32_t device_id)
{
	int32_t ret = EXIT_SUCCESS;

	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **s_results;

	const char * b_get_dev_type = "select subType from t_device where device_id = %d;";
//	char * get_dev_type = (char *) calloc(strlen(b_get_dev_type) + TS_DB_INT_ADD + 1, sizeof(char));
	char get_dev_type[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(get_dev_type, b_get_dev_type, device_id);
	ret = sc_sqlite3_get_table(get_dev_type, &s_results, &rows, &columns);
//	free(get_dev_type);
	//query error
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (rows > 0) {
		*device_type = atoi(s_results[rows * columns]);
	} else {
		*device_type = 0;
		ret = ERR_DB_NO_RECORDS;
	}

	sqlite3_free_table(s_results);
	return ret;
}

int32_t db_get_grp_id_by_addr(int32_t addr, int32_t dev_id, int32_t * grp_id)
{
	int ret = EXIT_SUCCESS;

	const char * b_get_grp_id = "select c.id from t_device a,t_ctrlunit b ,t_knx_group c where a.device_id = b.device and b.id = c.ctrlUnit and a.device_id = %d and (c.groupAddr = '%d/%d/%d' or c.groupAddr = '%d/%d');";

	int32_t main_3 = 0;
	int32_t mid_3 = 0;
	int32_t logic_3 = 0;
	int32_t main_2 = 0;
	int32_t logic_2 = 0;
	get_3_knx_group_addr(addr, &main_3, &mid_3, &logic_3);
	get_2_knx_group_addr(addr, &main_2, &logic_2);
	char get_sql[TS_DB_SQL_LENGTH] = { 0 };

	sprintf(get_sql, b_get_grp_id, dev_id, main_3, mid_3, logic_3, main_2, logic_2);
	int32_t rows = 0;
	int32_t columns = 0;
	char ** s_results;

	ret = sc_sqlite3_get_table(get_sql, &s_results, &rows, &columns);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(s_results);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (rows > 0) {
		*grp_id = atoi(s_results[rows * columns]);
	} else {
		ret = ERR_DB_NO_RECORDS;
	}

	sqlite3_free_table(s_results);

	return ret;
}

int32_t db_get_scene_id(ts_scene_io_info_t *scene_in_info, int32_t * count, ts_scene_info_t **scene_info, int8_t is_use)
{
	int32_t ret = EXIT_SUCCESS;

	//If the device is knx,get the group id
#define TS_IS_NOT_KNX -1
	int32_t grp_id = TS_IS_NOT_KNX;
	db_get_grp_id_by_addr(scene_in_info->param, scene_in_info->dev_id, &grp_id);

	//get scene id
	const char *b_get_scene = "select a.id,a.and_or,b.threshold from t_scene a ,t_scene_input b where a.id = b.scene_id "
		" and b.device_id = %d and b.param = %d and b.value = '%s' and a.is_use = %d";
	char s_get_scene[TS_DB_SQL_LENGTH] = { 0 };
	if (TS_IS_NOT_KNX == grp_id) {
		sprintf(s_get_scene, b_get_scene, scene_in_info->dev_id, scene_in_info->param, scene_in_info->value, is_use);
	} else {
		sprintf(s_get_scene, b_get_scene, scene_in_info->dev_id, grp_id, scene_in_info->value, is_use);
	}

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(s_get_scene, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
	} else {
		*count = nrow;
	}

	*scene_info = (ts_scene_info_t *) calloc(nrow * sizeof(ts_scene_info_t) + 1, sizeof(char));

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		(*scene_info + i - 1)->is_use = is_use;
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "id")) {
				(*scene_info + i - 1)->id = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "and_or")) {
				(*scene_info + i - 1)->and_or = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "threshold")) {
				scene_in_info->threshold = atof(all_result[i * ncolumn + j]);
			}
		}
	}

	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_get_scene_infos(struct list_head * list, ts_scene_io_info_list_t * scene_infos, int32_t scene_id)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_sql_select = "select * from t_scene_output where 1 = 1 and scene_id = %d order by id";
//	char * sql_select = (char *) calloc(strlen(b_sql_select) + TS_DB_INT_ADD + 1, sizeof(char));
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_sql_select, scene_id);
	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		scene_infos = (ts_scene_io_info_list_t *) calloc(sizeof(ts_host_interface_list_t), sizeof(char));
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "device_id")) {
				scene_infos->scene_info.dev_id = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "value")) {
				strcpy(scene_infos->scene_info.value, all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "param")) {
				scene_infos->scene_info.param = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "delay")) {
				scene_infos->scene_info.delay = atoi(all_result[i * ncolumn + j]);
			}
		}
		list_add_tail(&scene_infos->list, list);
	}

	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_get_key_func_name(int key_func_id, char * key_fun_name)
{
	int32_t ret = EXIT_SUCCESS;

	const char * b_sql_select = "select funcKey from t_keyFunction where 1 = 1 and id = %d";
//	char * sql_select = (char *) calloc(strlen(b_sql_select) + sizeof(ts_scene_io_info_t) + 1, sizeof(char));
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_sql_select, key_func_id);

	int32_t rows = 0, columns = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &rows, &columns);
//	free(sql_select);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == all_result[i * columns + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "funcKey")) {
				strcpy(key_fun_name, all_result[i * columns + j]);
			}

		}
	}

	sqlite3_free_table(all_result);

	return ret;
}

int32_t db_get_dev_by_serial(int32_t serial, int32_t *dev_id)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_sql_select = "select device from t_host_device_serial where 1 = 1 and serial = %d";
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_sql_select, serial);

	int32_t rows = 0, columns = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (rows <= 0) {
		ret = ERR_DB_NO_RECORDS;
	}
	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == all_result[i * columns + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "device")) {
				*dev_id = atoi(all_result[i * columns + j]);
			}
		}
	}
	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_reset_knx_curr()
{
	int32_t ret = EXIT_SUCCESS;

	const char * b_upd_sql = "update t_host_knx_curr_value set is_first_use = %d;";
	char s_del_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(s_del_sql, b_upd_sql, TS_KNX_VALUE_FIRST_USE);

	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, s_del_sql, 0, 0);
	if (SQLITE_OK != ret) {
		int32_t error_code = ERR_DB_DELETE;
		ret = error_code;
	}

	return ret;
}

static int32_t get_grp_addr(int32_t dev_id, int32_t grp_id, u_int16_t *addr)
{
	int32_t ret = EXIT_SUCCESS;

	const char * b_sql_select = "select c.groupAddr from t_ctrlunit b, t_knx_group c"
		" where 1 = 1 and c.id = %d and c.ctrlUnit = b.id and b.device = %d";
	//	char * sql_select = (char *) calloc(strlen(b_sql_select) + TS_DB_INT_ADD + 1, sizeof(char));
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_sql_select, grp_id, dev_id);
	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		sqlite3_free_table(all_result);
		return ret;
	} else {
		get_knx_group_info(all_result[nrow], addr);
		sqlite3_free_table(all_result);
	}

	return ret;
}

//int32_t db_get_all_scene_inputs(int scene_id, int * count, ts_scene_io_info_t *** in_infos)
int32_t db_get_all_scene_inputs(int scene_id, int * count, ts_scene_io_info_t ** in_infos)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_sql_select = "select device_id,* from t_scene_input where 1 = 1 and scene_id = %d order by id";
	//	char * sql_select = (char *) calloc(strlen(b_sql_select) + TS_DB_INT_ADD + 1, sizeof(char));
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_sql_select, scene_id);

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
	} else {
		*count = nrow;
	}

	//*in_infos = (ts_scene_io_info_t **) calloc(nrow, sizeof(ts_scene_io_info_t *));
	*in_infos = (ts_scene_io_info_t *) calloc(nrow, sizeof(ts_scene_io_info_t));
	if (NULL == in_infos) {
		return ERR_DB_NO_RECORDS;
	}

	int32_t i = 0, j = 0;
	int32_t dev_id = 0;
	for (i = 1; i <= nrow; i++) {
		dev_id = 0;
//		(*in_infos)[i - 1] = (ts_scene_io_info_t *) calloc(sizeof(ts_scene_io_info_t) , sizeof(char));
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "device_id")) {
				dev_id = atoi(all_result[i * ncolumn + j]);
//				(*in_infos)[i - 1]->dev_id = dev_id;
				(*in_infos + i - 1)->dev_id = dev_id;
			} else if (0 == strcmp(all_result[j], "value")) {
				strcpy((*in_infos + i - 1)->value, all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "param")) {
				u_int16_t addr = 0;
				int32_t grpId = atoi(all_result[i * ncolumn + j]);
				get_grp_addr(dev_id, grpId, &addr);
				if (0 == addr) {
					(*in_infos + i - 1)->param = atoi(all_result[i * ncolumn + j]);
				} else {
					(*in_infos + i - 1)->param = addr;
				}
			} else if (0 == strcmp(all_result[j], "threshold")) {
				float threshold = (float) atof(all_result[i * ncolumn + j]);
				(*in_infos + i - 1)->threshold = threshold;
			}
		}
	}

	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_reset_serial_dev_curr()
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_del_sql = "delete from  t_host_serial_curr_state where 1 = 1";

	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, b_del_sql, 0, 0);
	if (SQLITE_OK != ret) {
		int32_t error_code = ERR_DB_DELETE;
		ret = error_code;
	}

	return ret;
}

int32_t db_get_serial_devs_by_protrol(struct list_head * list_head, ts_serial_protrol_dev_list_t * devs, char * pro_name)
{

	if (pro_name == 0) {
		return ERR_DB_NO_RECORDS;
	}

	int32_t ret = EXIT_SUCCESS;
	const char * b_sql_select = "select b.device_id, b.zone, c.serial from t_serial_board a, t_device b, t_host_device_serial c where 1 = 1 and a.file_name = '%s' and a.device = b.device_id and b.device_id = c.device order by a.device";
	//	char * sql_select = (char *) calloc(strlen(b_sql_select) + TS_DB_INT_ADD + 1, sizeof(char));
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_sql_select, pro_name);

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		devs = (ts_serial_protrol_dev_list_t *) calloc(sizeof(ts_serial_protrol_dev_list_t), sizeof(char));
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "device_id")) {
				devs->devs.device_id = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "zone")) {
				devs->devs.area = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "serial")) {
				devs->devs.com_id = atoi(all_result[i * ncolumn + j]);
			}
		}
		list_add_tail(&devs->list, list_head);
	}

	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_get_device_infos_by_dp(int32_t dp_id, int32_t *dev_id, int32_t *subtype, int32_t *gateway_id, int32_t *gateway_type)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_sql_select = "select b.device_id,c.id,c.gatewayTypeId,b.subType "
		" from t_data_point a,t_device b,t_gateway c where a.id = %d and a.device=b.device_id and b.gateway = c.id;";
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_sql_select, dp_id);
//	printf("sql = %s",sql_select);

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "device_id")) {
				*dev_id = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "gatewayTypeId")) {
				*gateway_type = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "id")) {
				*gateway_id = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "subType")) {
				*subtype = atoi(all_result[i * ncolumn + j]);
			}
		}
	}
	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_get_ctrltype_by_dp(int32_t dp_id, int32_t *ctrl_type)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_sql_select = "select a.ctrlType ct from t_data_point a where a.id = %d";
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_sql_select, dp_id);
//	printf("sql = %s\n",sql_select);

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "ct")) {
				*ctrl_type = atoi(all_result[i * ncolumn + j]);
			}
		}
	}
	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_get_enocean_id_by_dp(int32_t dp_id, u_int32_t *enocean_id, int32_t *rocker)
{
	int32_t ret = EXIT_SUCCESS;

	const char * b_sql_select = "select a.enoceanId,a.rocker from t_enocean a where a.dataPoint = %d";
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_sql_select, dp_id);

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "enoceanId")) {
				if (strlen(all_result[i * ncolumn + j]) > 2) {
					sscanf(all_result[i * ncolumn + j] + 2, "%x", enocean_id);
				}
			} else if (0 == strcmp(all_result[j], "rocker")) {
				*rocker = atoi(all_result[i * ncolumn + j]);
			}
		}
	}
	sqlite3_free_table(all_result);

	return ret;
}

int32_t db_get_dp_by_enocean_id(int32_t e_id, struct list_head * list, ts_dps_list_t * dps)
{
	int32_t ret = EXIT_SUCCESS;
	const char * b_sql_select = "select a.dataPoint dp,b.ctrltype from t_enocean a,t_data_point b where a.dataPoint = b.id "
		" and a.enoceanId = %d;";
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_sql_select, e_id);
//		printf("%s\n",sql_select);

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		dps = (ts_dps_list_t *) calloc(sizeof(ts_dps_list_t), sizeof(char));
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "dp")) {
				dps->dp_info.dp_id = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "ctrltype")) {
				dps->dp_info.ctrl_type = atoi(all_result[i * ncolumn + j]);
			}
		}
		list_add_tail(&dps->list, list);
	}
	sqlite3_free_table(all_result);

	return ret;
}

int32_t db_get_dp_infos_by_dp(int32_t dp, int32_t *max_steps, int32_t *max_times)
{
	int32_t ret = EXIT_SUCCESS;
	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	const char * b_sql_select = "select a.totalTime ,a.totalSteps from t_enocean a where a.dataPoint = %d ;";
	sprintf(sql_select, b_sql_select, dp);

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "totalTime")) {
				*max_steps = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "totalSteps")) {
				*max_times = atoi(all_result[i * ncolumn + j]);
			}
		}
	}
	sqlite3_free_table(all_result);

	return ret;
}

int32_t db_get_dp_curr_value(int32_t dp, char * curr_value)
{
	int32_t ret = EXIT_SUCCESS;

	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	const char * b_sql_select = "select a.curr_value from t_host_dp_curr a where a.data_point = %d ;";
	sprintf(sql_select, b_sql_select, dp);

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "curr_value")) {
				strcpy(curr_value, all_result[i * ncolumn + j]);
			}
		}
	}
	sqlite3_free_table(all_result);

	return ret;
}

int32_t db_set_dp_curr_value(int32_t dp, char * curr_value)
{
	int32_t ret = EXIT_SUCCESS;

	const char * b_del_sql = "delete from t_host_dp_curr where data_point = %d;";
	char s_del_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(s_del_sql, b_del_sql, dp);

	TS_DB_BEGIN_TRANS_FILE

	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, s_del_sql, 0, 0);
	if (SQLITE_OK != ret) {
		int32_t error_code = ERR_DB_DELETE;
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
		ret = error_code;
	}

//insert
	const char * b_ins_sql = "insert into t_host_dp_curr (data_point ,curr_value ) values (%d,'%s');";
	char s_ins_sql[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(s_ins_sql, b_ins_sql, dp, curr_value);
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, s_ins_sql, 0, 0);
	if (SQLITE_OK != ret) {
		int32_t error_code = ERR_DB_INSERT;
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
		ret = error_code;
	}
	TS_DB_COMMIT_TRANS_FILE

	return ret;
}

int32_t db_get_dps_by_same_dev(int32_t dp, struct list_head *list, ts_dps_list_t * dps)
{
	int32_t ret = EXIT_SUCCESS;

	const char * b_get_sql = "select  a.id dp,a.ctrltype from t_data_point a,t_device b  "
		" where a.device = b.device_id "
		" and b.device_id = (select c.device from t_data_point c where c.id = %d limit 1);";

	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	sprintf(sql_select, b_get_sql, dp);
//		printf("%s\n",sql_select);

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		dps = (ts_dps_list_t *) calloc(sizeof(ts_dps_list_t), sizeof(char));
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "dp")) {
				dps->dp_info.dp_id = atoi(all_result[i * ncolumn + j]);
			} else if (0 == strcmp(all_result[j], "ctrltype")) {
				dps->dp_info.ctrl_type = atoi(all_result[i * ncolumn + j]);
			}
		}
		list_add_tail(&dps->list, list);
	}
	sqlite3_free_table(all_result);

	return ret;
}

int32_t db_get_comm_by_enocean_id(u_int32_t e_id, ts_host_interface_serial_info * serial_info)
{
	int32_t ret = EXIT_SUCCESS;

	char sql_select[TS_DB_SQL_LENGTH] = { 0 };
	const char * b_sql_select = "select c.enoceanId,a.*  "
		" from t_host_serial_info a ,t_host_enocean_serial b,t_enocean c  "
		" where a.id = b.serial and b.enocean=  c.id and c.enoceanId =  '%s' limit 1;";
	char s_e_id[32] = { 0 };
	sprintf(s_e_id, "0x%x", e_id);

	sprintf(sql_select, b_sql_select, s_e_id);

	int32_t nrow = 0, ncolumn = 0;
	char **all_result;
	ret = sc_sqlite3_get_table(sql_select, &all_result, &nrow, &ncolumn);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return error_code;
	} else {
		ret = EXIT_SUCCESS;
	}

	if (nrow <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= nrow; i++) {
		for (j = 0; j < ncolumn; j++) {
			if (NULL == all_result[i * ncolumn + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "id")) {
				serial_info->id = atoi(all_result[i * ncolumn + j]);
			}
			if (0 == strcmp(all_result[j], "interface_id")) {
				serial_info->interface_id = atoi(all_result[i * ncolumn + j]);
			}
			if (0 == strcmp(all_result[j], "baud_rate")) {
				serial_info->baud_rate = atoi(all_result[i * ncolumn + j]);
			}
			if (0 == strcmp(all_result[j], "data_bit")) {
				serial_info->data_bit = atoi(all_result[i * ncolumn + j]);
			}
			if (0 == strcmp(all_result[j], "stop_bit")) {
				serial_info->stop_bit = atof(all_result[i * ncolumn + j]);
			}
			if (0 == strcmp(all_result[j], "parity_check")) {
				serial_info->parity_check = atoi(all_result[i * ncolumn + j]);
			}
			if (0 == strcmp(all_result[j], "comm_id")) {
				serial_info->comm_id = atoi(all_result[i * ncolumn + j]);
			}
			if (0 == strcmp(all_result[j], "comm_port")) {
				memset(serial_info->comm_port, 0, sizeof(serial_info->comm_port));
				strcpy(serial_info->comm_port, all_result[i * ncolumn + j]);
			}
		}
	}
	sqlite3_free_table(all_result);

	return ret;

}

#define IR_DRIVER_PATH  "/opt/tshome-host/conf/infr_driver"
int32_t db_get_infr_driver(char * driver_name)
{
	int32_t ret = EXIT_SUCCESS;
	FILE *pstr;
	const char * base_driver = "/dev/IR_driver0";

	pstr = fopen(IR_DRIVER_PATH, "r");
	if (pstr == NULL) {
		log_debug_web("%s failed!\n",IR_DRIVER_PATH);
		strcpy(driver_name, base_driver);
		return -1;
	}

	int32_t flen = 0;
	char buffer[256] = { 0 };

	if (NULL != pstr) {
		fseek(pstr, 0, SEEK_END);
		flen = ftell(pstr);
		fseek(pstr, 0, SEEK_SET);
		fread(buffer, sizeof(char), flen, pstr);
	}

	if (strlen(buffer) > 0) {
		strcpy(driver_name, buffer);
	} else {
		strcpy(driver_name, base_driver);
	}

	if (NULL != pstr) {
		fclose(pstr);
	}

	return ret;
}

int32_t db_set_infr_driver(char * driver_name)
{
	int32_t ret = EXIT_SUCCESS;

	char cmd[256] = {0};
	sprintf(cmd,"echo %s > %s",driver_name,IR_DRIVER_PATH);
	system(cmd);

	return ret;
}

