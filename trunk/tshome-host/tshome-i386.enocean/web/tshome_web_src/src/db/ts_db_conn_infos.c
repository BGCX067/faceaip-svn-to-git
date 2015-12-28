/*
 * ts_db_conn_infos.cpp
 *
 *  Created on: 2012-2-15
 *      Author: pangt
 */

#include "ts_db_conn_infos.h"

int32_t db_is_device_control_by_name(char * user_name, int32_t device_id, int32_t * is_control)
{
	//get gateway Type
	const char * base_get_permission = "select a.device_id "
		"from t_device a,t_ref_role_device b,t_ref_user_role c,t_user  d "
		"where d.id = c.user and c.role = b.role_id and b.device = a.device_id "
		"and b.permission = %d and d.userName = '%s' and a.device_id = %d";
//	char * sql_permission = (char *) calloc(strlen(base_get_permission) + strlen(user_name) + TS_DB_INT_ADD * 2, sizeof(char));
	char sql_permission[TS_DB_SQL_LENGTH] = {0};
	sprintf(sql_permission, base_get_permission, TS_DEVICE_PERMISSION_FALSE, user_name, device_id);
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_result;
	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(sql_permission, &all_result, &rows, &columns);
//	free(sql_permission);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	}

	if (rows <= 0) {
		*is_control = TS_DEVICE_PERMISSION_TRUE;
	} else {
		*is_control = TS_DEVICE_PERMISSION_FALSE;
	}

	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_is_system_control_by_name(char * user_name, int32_t permission_id, int32_t * is_control)
{
	//get gateway Type
	const char * base_get_permission = "select a.userName,c.permissions from t_user a,t_ref_user_role b ,t_ref_role_system c "
		"where a.id = b.user and b.role = c.role_id and a.userName = '%s'";
//	char * sql_get_permission = (char *) calloc(strlen(base_get_permission) + TS_DB_INT_ADD * 3 + strlen(user_name), sizeof(char));
	char sql_get_permission[TS_DB_SQL_LENGTH] = {0};
	sprintf(sql_get_permission, base_get_permission, user_name);
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_result;
	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(sql_get_permission, &all_result, &rows, &columns);


//	free(sql_get_permission);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		ret = error_code;
		sqlite3_free_table(all_result);
		return error_code;
	}
	if (rows <= 0) {
		*is_control = TS_SYSTEM_PERMISSION_FALSE;
	} else {
		int32_t permissions = 0;
		int32_t i = 0, j = 0;
		for (i = 1; i <= rows; i++) {
			for (j = 0; j < columns; j++) {
				if (0 == strcmp(all_result[j], "permissions")) {
					permissions = atoi(all_result[i * columns + j]);
				}
			}
		}

		if (1 == (permissions >> (permission_id - 1) & 0x01)) {
			*is_control = TS_SYSTEM_PERMISSION_TRUE;
		}
	}
	sqlite3_free_table(all_result);
	return ret;
}

//get whether the user can control this device
int32_t db_is_device_control(int32_t user_id, int32_t device_id, int32_t * is_control)
{
	//get gateway Type
	const char * base_get_permission = "select a.device_id from t_device a,t_ref_role_device b,t_ref_user_role c,t_user  d "
		"where d.id = c.user and c.role = b.role_id and b.device = a.device_id "
		"and b.permission = %d and d.id = %d and a.device_id = %d";
//	char * sql_permission = (char *) calloc(strlen(base_get_permission) + TS_DB_INT_ADD * 3, sizeof(char));
	char sql_permission[TS_DB_SQL_LENGTH] = {0};
	sprintf(sql_permission, base_get_permission, TS_DEVICE_PERMISSION_FALSE, user_id, device_id);
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_result;
	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(sql_permission, &all_result, &rows, &columns);
//	free(sql_permission);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	}

	if (rows <= 0) {
		*is_control = TS_DEVICE_PERMISSION_TRUE;
	} else {
		*is_control = TS_DEVICE_PERMISSION_FALSE;
	}

	sqlite3_free_table(all_result);
	return ret;
}

//static int32_t get_user_id_by_session_id(char *session_id, int32_t *user_id)
//{
//	int32_t ret = EXIT_SUCCESS;
//	//get user id
//	const char * base_get_user_id = "select a.id from t_user a,t_session_info b where a.userName = b.user_name and b.session_id = \'%s\'";
//	char * sql_permission = (char *) calloc(strlen(base_get_user_id) + TS_DB_INT_ADD, sizeof(char));
//	sprintf(sql_permission, base_get_user_id, session_id);
//	int32_t rows = 0; //interface rows
//	int32_t columns = 0; // interface columns
//	char **all_result;
//	ret = sc_sqlite3_get_table(sc_db, sql_permission, &all_result, &rows, &columns);
//	if (ret != SQLITE_OK) {
//		ret = ERR_DB_QUERY;
//		sqlite3_free_table(all_result);
//		return ret;
//	}
//
//	//get permission
//	if (rows <= 0) {
//		ret = ERR_DB_USER_NOT_EXIT;
//		return ret;
//	} else {
//		*user_id = atoi(all_result[1]);
//		printf("user_id = %d\n", *user_id);
//	}
//
//	free(sql_permission);
//	sqlite3_free_table(all_result);
//	return ret;
//}

//int32_t db_is_device_control_4_session(char *session_id, int32_t device_id, int32_t * is_control)
//{
//	int32_t ret = EXIT_SUCCESS;
//	int32_t user_id = 0;
//	ret = get_user_id_by_session_id(session_id, &user_id);
//	if (EXIT_SUCCESS != ret) {
//		return ret;
//	} else {
//		ret = db_is_device_control(user_id, device_id, is_control);
//	}
//
//	return ret;
//}

//get whether the user can has the system permission
int32_t db_is_system_control(int32_t user_id, int32_t permission_id, int32_t * is_control)
{
	//get gateway Type
	const char * base_get_permission = "select a.userName,c.permissions from t_user a,t_ref_user_role b ,t_ref_role_system c "
		"where a.id = b.user and b.role = c.role_id and a.id = %d";
//	char * sql_get_permission = (char *) calloc(strlen(base_get_permission) + TS_DB_INT_ADD * 3 + TS_DB_INT_ADD, sizeof(char));
	char sql_get_permission[TS_DB_SQL_LENGTH] = {0};
	sprintf(sql_get_permission, base_get_permission, user_id);
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_result;
	int32_t ret = EXIT_SUCCESS;
	ret = sc_sqlite3_get_table(sql_get_permission, &all_result, &rows, &columns);
//	free(sql_get_permission);

	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		ret = error_code;
		sqlite3_free_table(all_result);
		return error_code;
	}
	if (rows <= 0) {
		*is_control = TS_SYSTEM_PERMISSION_FALSE;
	} else {
		int32_t permissions = 0;
		int32_t i = 0, j = 0;
		for (i = 1; i <= rows; i++) {
			for (j = 0; j < columns; j++) {
				if (0 == strcmp(all_result[j], "permissions")) {
					permissions = atoi(all_result[i * columns + j]);
				}
			}
		}

		if (1 == (permissions >> (permission_id - 1) & 0x01)) {
			*is_control = TS_SYSTEM_PERMISSION_TRUE;
		}
	}
	sqlite3_free_table(all_result);
	return ret;
}

//int32_t db_is_system_control_4_session(char *session_id, int32_t permission_id, int32_t * is_control)
//{
//	int32_t ret = EXIT_SUCCESS;
//	int32_t user_id = 0;
//	ret = get_user_id_by_session_id(session_id, &user_id);
//	if (EXIT_SUCCESS != ret) {
//		return ret;
//	} else {
//		ret = db_is_system_control(user_id, permission_id, is_control);
//	}
//	return ret;
//}

int32_t db_set_session_user(char *session_id, char *user_name)
{
	int32_t ret = EXIT_SUCCESS;

	//insert into db
	const char *base_sql = "insert into t_session_info (session_id,user_name) values (\'%s\','\%s\'); ";
//	char *insert_sql = (char *) calloc(strlen(base_sql) + TS_DB_INT_ADD + 31, sizeof(char));
	char insert_sql[TS_DB_SQL_LENGTH] = {0};
	sprintf(insert_sql, base_sql, session_id, user_name);
	ret = sc_sqlite3_exec(insert_sql, 0, 0);
//	free(insert_sql);
	if (ret != 0) {
		int32_t error_code = ERR_DB_INSERT;
		return error_code;
	}

	return ret;
}

//delete the relation of session and user
int32_t db_del_session_user(char *session_id)
{
	int32_t ret = EXIT_SUCCESS;

	//insert into db
	const char *base_sql = "delete from t_session_info where session_id = \'%s\'";
//	char *del_sql = (char *) calloc(strlen(base_sql) + TS_DB_INT_ADD, sizeof(char));
	char del_sql[TS_DB_SQL_LENGTH] = {0};
	sprintf(del_sql, base_sql, session_id);
	ret = sc_sqlite3_exec(del_sql, 0, 0);
//	free(del_sql);
	if (ret != 0) {
		int32_t error_code = ERR_DB_DELETE;
		return error_code;
	}
	return ret;
}

int32_t db_get_host_user_info(char * user_name, char * passwd)
{
	int32_t ret = EXIT_SUCCESS;
	//const char * sql_get_host_user_info = "select b.userName as userName,b.pwd as pwd from t_home a,t_user b where a. homeName = b.userName;";
	const char * sql_get_host_user_info = "select login_name ,login_passwd from t_host_base_info;";
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_result;
	ret = sc_sqlite3_get_table(sql_get_host_user_info, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		ret = error_code;
		sqlite3_free_table(all_result);
		return error_code;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == all_result[i * columns + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "login_name")) {
				memcpy(user_name, all_result[i * columns + j], strlen(all_result[i * columns + j]));
			} else if (0 == strcmp(all_result[j], "login_passwd")) {
				memcpy(passwd, all_result[i * columns + j], strlen(all_result[i * columns + j]));
			}
		}
	}

	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_get_lastest_version_software(char *version)
{
	int32_t ret = EXIT_SUCCESS;
	const char * sql_get_host_user_info = "select lastUpdateTime from t_home;";
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_result;
	ret = sc_sqlite3_get_table(sql_get_host_user_info, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		ret = error_code;
		sqlite3_free_table(all_result);
		return error_code;
	}

	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(all_result[j], "lastUpdateTime")) {
				memcpy(version, all_result[i * columns + j], strlen(all_result[i * columns + j]));
			}
		}
	}
	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_get_lastest_version_conf(unsigned long long *version)
{
	int32_t ret = EXIT_SUCCESS;
	const char * sql_get_host_user_info = "select lastUpdateTime conf from t_home;";
	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_result;
	ret = sc_sqlite3_get_table(sql_get_host_user_info, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		ret = error_code;
		sqlite3_free_table(all_result);
		return error_code;
	}

	int32_t index = 0;
	int32_t i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (NULL == all_result[i * columns + j]) {
				continue;
			}
			if (0 == strcmp(all_result[j], "conf")) {
//				memcpy(version, all_result[i * columns + j], strlen(all_result[i * columns + j]));
				unsigned long long v = strtoll(all_result[i * columns + j], NULL, 10);
//				*((long long int32_t *) version) = v;
//	memcpy(version, &v, sizeof(long long));

				*version = v;
			}
			index++;
		}
	}
	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_user_validate(char * user_name, char * passwd, int32_t * is_pass)
{
	int32_t ret = EXIT_SUCCESS;

	const char * b_sql_get_host_user_info = "select * from t_user where userName = '%s' and pwd = '%s'";
//	char * sql_get_host_user_info = (char *) calloc(strlen(b_sql_get_host_user_info) + 64, sizeof(char));
	char sql_get_host_user_info[TS_DB_SQL_LENGTH] = {0};
	sprintf(sql_get_host_user_info, b_sql_get_host_user_info, user_name, passwd);

	int32_t rows = 0; //interface rows
	int32_t columns = 0; // interface columns
	char **all_result;

	ret = sc_sqlite3_get_table(sql_get_host_user_info, &all_result, &rows, &columns);
//	free(sql_get_host_user_info);
	if (ret != SQLITE_OK) {
		int32_t error_code = ERR_DB_QUERY;
		ret = error_code;
		sqlite3_free_table(all_result);
		return error_code;
	}

	if (rows > 0) {
		*is_pass = TS_IS_USER_PASS_TRUE;
	} else {
		*is_pass = TS_IS_USER_PASS_FALSE;
	}

	sqlite3_free_table(all_result);

	return ret;
}

int32_t db_get_home_id(int32_t * home_id)
{
	int32_t ret = EXIT_SUCCESS;
	const char * sql_get_home_id = "select *  from t_home where 1 = 1";
	int32_t i = 0, j = 0;
	int32_t rows = 0, columns = 0;
	char ** all_result = NULL;
	ret = sc_sqlite3_get_table(sql_get_home_id, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	for (i = 0; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(all_result[j], "id")) {
				*home_id = atoi(all_result[i * columns + j]);
			}
		}
	}
	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_get_is_stop_net(int32_t * is_stop)
{
	int32_t ret = EXIT_SUCCESS;
	const char * sql_get_home_id = "select is_stop_net from t_host_base_info where 1 = 1";
	int32_t i = 0, j = 0;
	int32_t rows = 0, columns = 0;
	char ** all_result = NULL;
	ret = sc_sqlite3_get_table(sql_get_home_id, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	for (i = 0; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(all_result[j], "is_stop_net")) {
				*is_stop = atoi(all_result[i * columns + j]);
			}
		}
	}
	sqlite3_free_table(all_result);
	return ret;
}

int32_t db_set_is_stop_net(int32_t is_stop)
{
	int32_t ret = EXIT_SUCCESS;

	const char *base_sql = "update t_host_base_info set is_stop_net = %d";
//	char *upd_sql = (char *) calloc(strlen(base_sql) + TS_DB_INT_ADD, sizeof(char));
	char upd_sql[TS_DB_SQL_LENGTH] = {0};
	sprintf(upd_sql, base_sql, is_stop);
	ret = sc_sqlite3_exec(upd_sql, 0, 0);
//	free(upd_sql);
	if (ret != 0) {
		int32_t error_code = ERR_DB_DELETE;
		return error_code;
	}

	return ret;
}

int32_t db_get_passwd_by_name(char * user_name, char * passwd)
{

	int32_t ret = EXIT_SUCCESS;
	const char * sql_get_home_id = "select pwd from t_user where userName = '%s';";
	int32_t i = 0, j = 0;
	int32_t rows = 0, columns = 0;
	char ** all_result = NULL;
//	char * s_get_pwd = (char *) calloc(strlen(sql_get_home_id) + strlen(user_name) + 1, sizeof(char));
	char s_get_pwd[TS_DB_SQL_LENGTH] = {0};
	sprintf(s_get_pwd, sql_get_home_id, user_name);

	ret = sc_sqlite3_get_table(s_get_pwd, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	for (i = 0; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
			if (0 == strcmp(all_result[j], "pwd")) {
				strcpy(passwd, all_result[i * columns + j]);
			}
		}
	}
	sqlite3_free_table(all_result);
	return ret;
}


int32_t db_get_max_users(int32_t *users) 
{
	int32_t ret = EXIT_SUCCESS;
	const char * sql_get_max_user = "select maxUsers from t_home limit 1";
	int32_t rows = 0, columns = 0;
	char ** all_result = NULL;

	*users = 10;
	ret = sc_sqlite3_get_table(sql_get_max_user, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	if(NULL != all_result[columns]) {
		*users = atoi(all_result[columns]);
	}


	sqlite3_free_table(all_result);
	return ret ;
}

int32_t db_get_is_local(int32_t *is_local) {
	int32_t ret = EXIT_SUCCESS;
	const char * sql_get_max_user = "select is_local from t_host_base_info limit 1";
	int32_t rows = 0, columns = 0;
	char ** all_result = NULL;

	ret = sc_sqlite3_get_table(sql_get_max_user, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		ret = ERR_DB_QUERY;
		sqlite3_free_table(all_result);
		return ret;
	} else {
		ret = EXIT_SUCCESS;
	}

	if(NULL != all_result[columns]) {
		*is_local = atoi(all_result[columns]);
	}

	sqlite3_free_table(all_result);
	return ret ;
}

int32_t db_set_is_local(int32_t is_local) {
	int32_t ret = EXIT_SUCCESS;
	const char *base_sql = "update t_host_base_info set is_local = %d";
	char upd_sql[TS_DB_SQL_LENGTH] = {0};
	sprintf(upd_sql, base_sql, is_local);
	ret = sc_sqlite3_exec(upd_sql, 0, 0);
	if (ret != 0) {
		int32_t error_code = ERR_DB_DELETE;
		return error_code;
	}

	return ret;
}

