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

#include <sqlite3.h>
#include <pthread.h>
#include <unistd.h>
//#include "db/sqlite3_ctrl.h"

sqlite3 *sc_db;
char * sc_z_err_msg;

//const char * file_name = "db/tshome.db";
const char * file_name = "/opt/tshome-host/db/tshome.db";

int ins_test()
{
	int ret = 0;

//	ret = sqlite3_exec(g_sc_db, "begin transaction", 0, 0, &sc_z_err_msg);
	//1
	const char * ins_sql1 = "insert into t_camera (id,device,cameraName,c_ip,c_port,loginName,"
		"loginPwd,description,channel,ip2,port2,id,device,cameraName,c_ip,c_port,loginName,"
		"loginPwd,description,channel,ip2,port2) "
		"values ('6','60','camera','192.168.10.22','11111','user','user','','4','','0','7','60',"
		"'camera','192.168.10.22','11111','user','user','','4','','0');";
	ret = sqlite3_exec(sc_db, ins_sql1, NULL, NULL, &sc_z_err_msg);
	if (ret != 0) {
		fprintf(stderr, "Can't ins : %s.\tError code : %d\n", sqlite3_errmsg(sc_db), ret);
		return ret;
	} else {
		printf("ins success\n");
	}
	return ret;
}

int main(int argc, char *argv[])
{
	int ret = EXIT_SUCCESS;
	pthread_t db_thread, db_thread1;
	void * thread_result;


	ret = sqlite3_open(file_name, &sc_db);
	if (ret != SQLITE_OK) {
		int error_code = -1;
		fprintf(stderr, "%s.\tError code : %d\n", sqlite3_errmsg(sc_db), error_code);
		sqlite3_close(sc_db);
		return error_code;
	}

	ins_test();


//close db
	ret = sqlite3_close(sc_db);
	if (ret != SQLITE_OK) {
		ret = EXIT_FAILURE;
		return ret;
	}

	printf("over \n");
	return ret;
}

