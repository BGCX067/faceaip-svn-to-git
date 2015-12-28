/*
 * sqlite3_ctrl.c
 *
 *  Created on: 2012-1-11
 *      Author: pangt
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common/ts_err_code.h"
#include <unistd.h>
#include <pthread.h>
#include <common/list.h>

#include "sqlite3_ctrl.h"

sqlite3 * sc_db_file;

#define TS_DB_WAIT_TIME 100

struct _ts_db_conn_map
{
	pthread_t thread_db;
	sqlite3 * db_conn;
}typedef ts_db_conn_map_t;

struct _ts_db_conn_list_t
{
	struct list_head list;
	ts_db_conn_map_t db_conn_map;
}typedef ts_db_conn_list_t;

//the lock which protect db can't be used by more thread
pthread_mutex_t mutex_file_db = PTHREAD_MUTEX_INITIALIZER; /* protects both work_area and time_to_exit */

LIST_HEAD( list_head_file_conns);

#define TS_DB_CONN_EXIT 1
#define TS_DB_CONN_NOT_EXIT 0

void ts_free(void * name)
{
	if (NULL != name) {
		free(name);
		name = NULL;
	}
}

/**
 * db_tpye : TS_DB_TYPE_FILE ,TS_DB_TYPE_MEMORY
 */
sqlite3 * sc_get_db_conn()
{
	sqlite3 * curr_sc_db = NULL;

	int32_t ret = SQLITE_OK;

	pthread_t curr_thread = pthread_self();
	int8_t is_conn_exits = TS_DB_CONN_NOT_EXIT;

	//Traversal list
	ts_db_conn_list_t * db_file_conns = NULL;
	ts_db_conn_list_t * p_tmp = NULL;
	pthread_t tmp_thread;

	pthread_mutex_lock(&mutex_file_db);
	list_for_each_entry_safe(db_file_conns,p_tmp,&list_head_file_conns,list) {
		tmp_thread = db_file_conns->db_conn_map.thread_db;
		if (tmp_thread == curr_thread) {
			curr_sc_db = db_file_conns->db_conn_map.db_conn;
			if (NULL != curr_sc_db) {
				is_conn_exits = TS_DB_CONN_EXIT;
				break;
			}
		}
	}
	pthread_mutex_unlock(&mutex_file_db);

	//add conn to list
	if (TS_DB_CONN_NOT_EXIT == is_conn_exits) {
		ret = sqlite3_open(TS_DB_FILE_NAME, &curr_sc_db);
		if (ret != SQLITE_OK) {
			ret = ERR_DB_OPEN;
			log_err( "%s.\tError code : %d\n", sqlite3_errmsg(curr_sc_db), ret);
			sqlite3_close(curr_sc_db);
			curr_sc_db = NULL;
			return curr_sc_db;
		}

		//add conn to list
		db_file_conns = (ts_db_conn_list_t *) calloc(sizeof(ts_db_conn_list_t), sizeof(char));
		if (NULL == db_file_conns) {
			curr_sc_db = NULL;
			return curr_sc_db;
		}
		db_file_conns->db_conn_map.thread_db = curr_thread;
		if (NULL != curr_sc_db) {
			//add conn to list
			db_file_conns->db_conn_map.db_conn = curr_sc_db;
			pthread_mutex_lock(&mutex_file_db);
			list_add_tail(&db_file_conns->list, &list_head_file_conns);
			pthread_mutex_unlock(&mutex_file_db);
		}
	}
	return curr_sc_db;
}

int32_t sc_sqlite3_exec_by_type(int8_t set_type, const char * sql, int32_t (*callback)(void*, int32_t, char**, char**), void * param)
{
	char * errmsg;
	int32_t ret = SQLITE_OK;
	sqlite3 * curr_sc_db_file = NULL;
	//get db

	curr_sc_db_file = sc_get_db_conn();
	if (NULL == curr_sc_db_file) {
		ret = ERR_DB_OPEN;
		log_err( "file get db conn error.\n");
		return ret;
	}

//	begin///////////////////////////////////////////
	if (TS_DB_EXEC_TYPE_NOW == set_type) {
		do {
			ret = sqlite3_exec(curr_sc_db_file, "begin immediate transaction", 0, 0, &errmsg);
		} while ((SQLITE_OK != ret) && (SQLITE_BUSY == ret));
		if (SQLITE_OK != ret) {
			log_err( "Can't exec : %s.\terror : %s\tError code : %d.\n", sql, sqlite3_errmsg(curr_sc_db_file), ret);
			return ret;
		}
	}

	//exec///////////////////////////////////////////////////////////////////////
	do {
		ret = sqlite3_exec(curr_sc_db_file, sql, callback, param, &errmsg);
	} while ((SQLITE_OK != ret) && (SQLITE_BUSY == ret));
	if (SQLITE_OK != ret) {
		log_err( "Can't exec : %s.\terror : %s\tError code : %d.\n", sql, sqlite3_errmsg(curr_sc_db_file), ret);
		if (TS_DB_EXEC_TYPE_NOW == set_type) {
			sqlite3_exec(curr_sc_db_file, "rollback transaction", 0, 0, &errmsg);
		}
		return ret;
	}

	//commit///////////////////////////////////
	if (TS_DB_EXEC_TYPE_NOW == set_type) {
		do {
			ret = sqlite3_exec(curr_sc_db_file, "commit transaction", 0, 0, &errmsg);
		} while (ret == SQLITE_BUSY);
		if (SQLITE_OK != ret) {
			log_err( "Can't exec : %s.\terror : %s\tError code : %d.\n", sql, sqlite3_errmsg(curr_sc_db_file), ret);
			return ret;
		}
	}

	return ret;
}

int32_t sc_sqlite3_exec(const char * sql, int32_t (*callback)(void*, int32_t, char**, char**), void * param)
{
	int32_t ret = SQLITE_OK;
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW, sql, callback, param);
	return ret;
}

int32_t sc_sqlite3_get_table(const char *zSql, char ***pazResult, int32_t *pnRow, int32_t *pnColumn)
{
	int32_t ret = SQLITE_OK;

	char * errmsg;
	sqlite3 *curr_sc_db = NULL;
	curr_sc_db = sc_get_db_conn();
	if (NULL == curr_sc_db) {
		log_err( "get db conn error.\n");
		ret = ERR_DB_OPEN;
		return ret;
	}

	do {
		ret = sqlite3_get_table(curr_sc_db, zSql, pazResult, pnRow, pnColumn, &errmsg);
		usleep(TS_DB_WAIT_TIME);
	} while ((ret == SQLITE_BUSY) || (ret == SQLITE_NOMEM));
	if (ret != 0) {
		log_err( "Can't sel : %s.\tError code : %d\n", sqlite3_errmsg(curr_sc_db), ret);
		return ret;
	}
	return ret;
}

int32_t sc_close()
{
	int32_t ret = EXIT_SUCCESS;
	sqlite3 * curr_sc_db;
	ts_db_conn_list_t * p_tmp = NULL;
	p_tmp = NULL;
	ts_db_conn_list_t * db_file_conns = NULL;

	pthread_mutex_lock(&mutex_file_db);
	list_for_each_entry_safe(db_file_conns,p_tmp,&list_head_file_conns,list) {
		curr_sc_db = db_file_conns->db_conn_map.db_conn;
//		log_err( "file kill thread : %ld\n", db_file_conns->db_conn_map.thread_db);
		if (NULL != curr_sc_db) {
			sqlite3_close(curr_sc_db);
			curr_sc_db = NULL;
		}
		list_del(&db_file_conns->list);
		ts_free(db_file_conns);
	}
	pthread_mutex_unlock(&mutex_file_db);

	return ret;
}
