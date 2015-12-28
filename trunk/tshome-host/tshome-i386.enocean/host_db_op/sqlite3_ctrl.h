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
#include <log/ts_log_tools.h>
#include "common/common_define.h"

#define TS_DB_FILE_NAME DB_FILE_PATH
#define TS_DB_FILE_NAME_BAK DB_FILE_PATH_BAK

#define TS_DB_INT_ADD 11
#define TS_DB_SQL_LENGTH 512

#define TS_FLAG_TRUE 0
#define TS_FLAG_FALSE 1

#define TS_DB_IS_INIT_TRUE 0
#define TS_DB_IS_INIT_FALSE 1

#define TS_STRING_SET_0(name) memset(name, '\0',sizeof(name))

#define TS_DB_EXEC_TYPE_NOW 0
#define TS_DB_EXEC_TYPE_NOW_NO_TRANS 1

#ifdef __cplusplus
extern "C"
{
#endif

extern sqlite3 * sc_db; //unused only for param

//begin trans
#define TS_DB_BEGIN_TRANS_FILE do {\
	do {\
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "begin immediate transaction", 0, 0);\
	} while ((SQLITE_OK != ret) && (SQLITE_BUSY == ret));\
}while(0);

//commit trans
#define TS_DB_COMMIT_TRANS_FILE do { \
	do {\
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "commit transaction", 0, 0);\
	} while (ret == SQLITE_BUSY);\
} while(0);

void ts_free(void * name);

//sqlite3 * sc_get_db_conn(int32_t db_type);

/**
 * close db
 */
int32_t sc_close();

int32_t sc_sqlite3_exec_by_type(int8_t set_type, const char * sql, int (*callback)(void*, int, char**, char**), void * param);

int32_t sc_sqlite3_exec(const char *sql, int (*callback)(void*, int, char**, char**), void * param);

int32_t sc_sqlite3_get_table(const char *zSql, char ***pazResult, int *pnRow, int *pnColumn);

#ifdef __cplusplus
}
#endif

#endif /* SQLITE3_CTRL_H_ */
