/*
 * sqlite3_ctrl.h
 *
 *  Created on: 2012-1-11
 *      Author: pangt
 */

/**
 * apt-get install sqlite3
 * apt-get install libsqlite3-dev
 * gcc -g -o ** ***.c -lsqlite3
 */

#ifndef SQLITE3_CTRL_H_
#define SQLITE3_CTRL_H_

#include <sqlite3.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TS_DB_FILE_NAME "../../../../db/tshome.db"
//#define TS_DB_FILE_NAME "/home/pangt/web/tshome_web_src/src/db/tshome.db"
#define TS_DB_FILE_NAME_BAK "../../../../db/tshome.db_bak"

#define TS_DB_FILE_2_MEMORY 0
#define TS_DB_MEMORY_2_FILE 1

#define TS_DB_MAX_ADD 20
#define TS_DB_INT_ADD 8

#define TS_FLAG_TRUE 0
#define TS_FLAG_FALSE 1

#define TS_DB_IS_INIT_TRUE 0
#define TS_DB_IS_INIT_FALSE 1

#define TS_STRING_SET_0(name) memset(name, '\0',sizeof(name))

#define TS_SET_TIME(name) \
	time_t cur_time;\
	struct tm * time_info;\
	time(&cur_time);\
	time_info = localtime(&cur_time);\
	sprintf(name, "%04d-%02d-%02d %02d:%02d:%02d", (time_info->tm_year\
		+ 1900), (time_info->tm_mon + 1), time_info->tm_mday,\
		time_info->tm_hour, time_info->tm_min, time_info->tm_sec)\


#ifdef __cplusplus
extern "C"
{
#endif

extern sqlite3 *sc_db;

extern char * sc_z_err_msg;

/**
 * memory db
 */
int sc_load_or_save_db(sqlite3 *p_in_memeory, const char *z_file_name,
	int is_save);

/**
 * sync memory and file
 *isSave  0: from file 2 memory 1：from memory 2 file
 @return 成功 ： SUCCESS
 */
int sc_synchronize_db(int isSave,const char * file_nam);

/**
 * clear maps for session
 */
int sc_delete_sessions();

/**
 * init database
 */
int sc_db_init(const char * file_name);

/**
 * close db
 */
int sc_close(const char * file_name);
int sc_close_db(const char* file_name, sqlite3 * db, char * z_err_msg);

#ifdef __cplusplus
}
#endif

#endif /* SQLITE3_CTRL_H_ */
