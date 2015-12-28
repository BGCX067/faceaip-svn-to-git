/*
 ============================================================================
 Name        : tshome_web.c
 Author      : pandaFly
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

/**
 * get so:
 * gcc -O -fpic -shared -o ts_db_tools.so *.o
 * gcc -Wall -o bt base_db_test.c -I /opt/tshome-host/include/ -L /opt/tshome-host/lib/ -lts_db_tools_web -lsqlite3
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sqlite3.h>
#include <pthread.h>
#include "db/sqlite3_ctrl.h"

//sqlite3 *tmp_sc_db;
//char * sc_z_err_msg;

const char * file_name = "db/tshome.db";

int db_get_host_user_info(char * user_name, char * passwd)
{
	int ret = EXIT_SUCCESS;
	const char * sql_get_host_user_info = "select login_name ,login_passwd from t_host_base_info;";
	int rows = 0; //interface rows
	int columns = 0; // interface columns
	char **all_result;
	ret = sc_sqlite3_get_table(sql_get_host_user_info, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
		int error_code = -1;
//		fprintf(stderr, "Can't query : %s.\tError code : %d\n", sqlite3_errmsg(tmp_sc_db), error_code);
		ret = error_code;
		sqlite3_free_table(all_result);
		return error_code;
	}

	int i = 0, j = 0;
	for (i = 1; i <= rows; i++) {
		for (j = 0; j < columns; j++) {
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

int sel_test(char * message)
{
	int ret = 0;
	const char * sql_sel = "select login_name ,login_passwd from t_host_base_info;";
	int rows = 0; //interface rows
	int columns = 0; // interface columns
	char **all_result;
	ret = sc_sqlite3_get_table(sql_sel, &all_result, &rows, &columns);
	if (ret != SQLITE_OK) {
//		fprintf(stderr, "Can't sel : %s.\tError code : %d\n", sqlite3_errmsg(tmp_sc_db), ret);
		sqlite3_free_table(all_result);
		return ret;
	} else {
		printf("%s : ", message);
		printf("sel success\n");
	}

	printf("rows = %d\n", rows);

	sqlite3_free_table(all_result);
	return ret;
}

int del_test(char * message)
{
	int ret = 0;
	const char * ins_sql1 = "delete from t_host_base_info where id > 2";
	ret = sc_sqlite3_exec(ins_sql1, NULL, NULL);
	if (ret != 0) {
//		fprintf(stderr, "Can't del : %s.\tError code : %d\n", sqlite3_errmsg(tmp_sc_db), ret);
		return ret;
	} else {
		printf("%s : ", message);
		printf("del success\n");
	}
	return ret;
}

int ins_test(char * message)
{
	int ret = 0;
	//1
//	const char * ins_sql1 = "insert into t_host_base_info (who_am_i) values (0);";
	char ins_sql1[512] = "insert into t_camera (id,device,cameraName,c_ip,c_port,loginName,loginPwd,description,channel,ip2,port2,id,device,cameraName,c_ip,c_port,loginName,loginPwd,description,channel,ip2,port2) values ('c_ip','c_port','loginName','loginPwd','description','channel','ip2','port2','4','','0','7','60','camera','192.168.10.22','11111','user','user','','4','','0');";
	ret = sc_sqlite3_exec(ins_sql1, NULL, NULL);
	if (ret != 0) {
		return ret;
	} else {
		printf("%s : ", message);
		printf("ins success\n");
	}
	return ret;
}

//char message[] = "ad";

void upd_who_am_i(char * message)
{
	int ret = 0;
	const char * upd_sql = "update t_host_base_info set who_am_i = 0";
	ret = sc_sqlite3_exec(upd_sql, NULL, NULL);
	if (ret != 0) {
		return;
	} else {
		printf("%s : ", message);
		printf("upd success\n");
	}
}

void upd_who_am_i_thread(char * message)
{

	fprintf(stderr, "0 thread %ld \n\n", pthread_self());
	int32_t i = 0;
	while (i < 3) {
		i++;
		ins_test(message);
		upd_who_am_i(message);
		del_test(message);
		sel_test(message);
		printf("%s over\n\n", message);
	}
}

void upd_who_am_i_thread1(char * message)
{
	int32_t i = 0;
	while (i < 3) {
		i++;
		ins_test(message);
		upd_who_am_i(message);
		del_test(message);
		sel_test(message);
		printf("%s\n\n", message);
	}
}

int main()
{

	int ret = EXIT_SUCCESS;
	pthread_t db_thread, db_thread1;
	void * thread_result;
//
//	ret = pthread_create(&db_thread, NULL, (void *) upd_who_am_i_thread, (void *) "0 success");
//	ret = pthread_create(&db_thread1, NULL, (void *) upd_who_am_i_thread1, (void *) "1 success");
//	if (0 != ret) {
//		printf("create thread over!\n");
//		exit(EXIT_FAILURE);
//	}

//	char name[32] = { 0 };
//	char pwd[32] = { 0 };
//	db_get_host_user_info(name, pwd);
//	printf("%s,%s\n", name, pwd);

	char *message = "main thread";

//	fprintf(stderr, "main thread %ld \n\n", pthread_self());
//	int32_t i = 0;
//	while (i < 3) {
//		i++;
//		usleep(100);
//		del_test(message);
//		ins_test(message);
//		upd_who_am_i(message);
//		sel_test(message);
//		printf("main over !\n\n");
//	}

	ins_test("");

//	ret = pthread_join(db_thread, &thread_result);
//	if (0 != ret) {
//		printf("join thread fail!\n");
//		exit(EXIT_FAILURE);
//	}
//
//	ret = pthread_join(db_thread1, &thread_result);
//	if (0 != ret) {
//		printf("join thread1 fail!\n");
//		exit(EXIT_FAILURE);
//	}

//close db
	ret = sc_close();
	if (ret != SQLITE_OK) {
		ret = EXIT_FAILURE;
		return ret;
	}

	printf("over \n");

	return ret;
}

