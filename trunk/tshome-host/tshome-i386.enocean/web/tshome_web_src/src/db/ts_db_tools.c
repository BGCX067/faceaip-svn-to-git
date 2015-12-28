/*
 * tstools.c
 *
 *  Created on: 2012-3-24
 *      Author: root
 */

#include "ts_db_tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <iconv.h>

#include "sqlite3_ctrl.h"
#include "common/ts_err_code.h"
#include "common/common_define.h"
#include "ts_db_web_infos.h"

#include "log/ts_log_tools.h"

#define CONTROL_LEN  512

#define TS_HOST_PRO_DIR "/opt/tshome-host/bin/"

void start_pro()
{
	ts_host_base_user_info user_info;
	char s_name[32] = { 0 };
	char s_passwd[32] = { 0 };
	db_get_host_user_info(s_name, s_passwd);
	char s_control[CONTROL_LEN];

	db_get_base_user_info(&user_info);
	//run in background
//	sprintf(s_control, "%s%s %s%s -t %s:%d -p %s -u %s &", TS_HOST_PRO_DIR, TS_MONITOR_NAME, TS_HOST_PRO_DIR, TS_PROGRAM_NAME, user_info.server_name, user_info.server_port, s_name, s_passwd);
	sprintf(s_control, "%s%s &", TS_HOST_PRO_DIR, TS_MONITOR_NAME);
	log_debug_web("%s\n", s_control);
	system(s_control);

}

void stop_pro()
{
	char s_control[CONTROL_LEN];
//	sprintf(s_control, "killall -15 %s\n", TS_MONITOR_NAME);
	sprintf(s_control, "./kill_monitor.sh");
	log_debug_web( "%s", s_control);
	system(s_control);
}

int32_t ts_db_tools_log_error(const char * log)
{
	int32_t ret = EXIT_SUCCESS;
	FILE *p_file = NULL;
	const char *file_name = TS_WEB_LOG_FILE;
	p_file = fopen(file_name, "w+");

	if (NULL == p_file) {
		log_err("parse xml : no file\n");
		return EXIT_FAILURE;
	}

	//write
	const char * start = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<details>\n";
	const char * s_log = "\t<log>";
	const char * s_log_end = "</log>\n";
	const char * end = "</details>\n";
	int len = strlen(log) + strlen(start) + strlen(end) + strlen(s_log) + strlen(s_log_end) + 1;
	char * s_write = (char *) calloc(len, sizeof(char));
	sprintf(s_write, "%s%s%s%s%s", start, s_log, log, s_log_end, end);
	fwrite(s_write, sizeof(char), len, p_file);
	fclose(p_file);
	free(s_write);
	return ret;
}

int32_t ts_db_tools_upd_software(const char * file_name)
{
	int32_t ret = EXIT_SUCCESS;
	return ret;
}

//filter letters
#define TS_XML_GET_CHILD for (; cur_node->next != NULL; cur_node = cur_node->next) {\
if (!(xmlStrcmp(cur_node->name, (const xmlChar *) "text"))\
	|| (!xmlStrcmp(cur_node->name, (const xmlChar *) "comment"))) {\
	continue;\
}\


//#define TS_XML_BASE_INSERT "insert into %s () values () ;"
#define TS_XML_BASE_INSERT "insert into %s (%s) values (%s) ;"
#define TS_XML_BASE_DROP "drop table %s;"
#define TS_XML_FREE 	if (NULL != sz_key) { \
	xmlFree(sz_key);\
}\
if (NULL != doc) {\
	xmlFreeDoc(doc);\
}\
if (NULL != sz_attr) {\
	xmlFree(sz_attr);\
}\


#define TS_BAK_DB 	s_bak = (char *) calloc(strlen(TS_DB_FILE_NAME) + strlen(TS_DB_FILE_NAME_BAK) + strlen(s_base_bak), sizeof(char));\
sprintf(s_bak,s_base_bak,TS_DB_FILE_NAME_BAK,TS_DB_FILE_NAME);\
system(s_bak);\
free(s_bak);\
s_bak = NULL;

/**
*get columns from create sql.
*/
static int32_t parse_create(char *create_table ,char *result) {
	int32_t ret = EXIT_SUCCESS;
	char *buf = NULL;
	char *infos;
	char s_column_tmp[32] = {0};
	int32_t i = 0;
	char create_table_bak[TS_DB_SQL_LENGTH] = {0};
	strcpy(create_table_bak,create_table);
	
	buf = strchr(create_table_bak,'(') + 1;

	while ((infos = strsep(&buf, ",")) != NULL) {
		char * buf_details;
		char * details;
		buf_details = infos;
		while ((details = strsep(&buf_details, " ")) != NULL) {
			bzero(s_column_tmp,sizeof(s_column_tmp));
			if(('[' == details[0] )||( '"' == details[0])) {
				memcpy(s_column_tmp,details+1,strlen(details)-2);
			} else {
				strcpy(s_column_tmp,details);
			} 
			sprintf(result,"%s%s,|",result,s_column_tmp);
			
			break;
		}
		i++;
	}
	
	return ret;
}

/**
* upd host table .
*/
static int32_t upd_host_table(char * create_table,char * table_name) {
	int32_t ret = EXIT_SUCCESS;
	const char * S_HOST_TABLE_BAK = "_bak_tmp";

	//parse new sql
	char new_create[128] = {0};
	parse_create(create_table,new_create);	

	//parse old sql
	char old_create[128] = {0};
	int32_t rows = 0;
	int32_t columns = 0;
	char **all_result ;
	const char *b_s_get_old = "select * from %s;";
	char s_get_old[128] = {0};
	char same_items[128] = { 0 };
	
	char is_table_exits[128] = {0};
	const char * b_s_is_exits = "select sql  from sqlite_master where name = '%s'";
	sprintf(is_table_exits,b_s_is_exits,table_name);
	ret =  sc_sqlite3_get_table(is_table_exits, &all_result, &rows, &columns);
	if(ret != SQLITE_OK) {
		sqlite3_free_table(all_result);
		return ret;
	}

	//if the table is exist , get data
	if(rows > 0) {
		sprintf(s_get_old,b_s_get_old,table_name);
		ret =  sc_sqlite3_get_table(s_get_old, &all_result, &rows, &columns);
		if(ret != SQLITE_OK) {
			sqlite3_free_table(all_result);
			return ret;
		}
	}
	
	if(rows > 0 ) {
		char same_item[128] = {0};
		char * buf = new_create;
		char * infos ;
		int j = 0;
		bzero(old_create,sizeof(old_create));
		for(;j < columns;j++ ) {
			sprintf(old_create,"%s%s,|",old_create,all_result[0+j]);	
		}

		while ((infos = strsep(&buf, "|")) != NULL) {
			if (NULL != strstr(old_create,infos)) {
				sprintf(same_item,"%s%s",same_item,infos);
			}
		}
		
		memcpy(same_items,same_item,strlen(same_item) - 1);

		//rename old table
		char rename_talbe[128] = {0};
		sprintf(rename_talbe,"alter table %s rename to %s%s;",table_name,table_name,S_HOST_TABLE_BAK);
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, (const char*)rename_talbe, 0, 0);
		if(ret != SQLITE_OK) {
			sqlite3_free_table(all_result);
			return ret;
		}
			
		sqlite3_free_table(all_result);
	}

	//create new table;
	ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, (const char*) create_table, 0, 0);
	if(ret != SQLITE_OK) {
		return ret;
	}
	

	//if hava old table ,copy old data to new table
	if(rows > 0 ) {
		char cp_data[TS_DB_SQL_LENGTH] = { 0 };
		sprintf(cp_data,"insert into %s (%s) select %s from %s%s;",table_name,same_items,same_items,table_name,S_HOST_TABLE_BAK);
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, (const char*) cp_data, 0, 0);
		if(ret != SQLITE_OK) {
			return ret;
		}

		//drop old table
		char drop_table[TS_DB_SQL_LENGTH] = { 0 };
		sprintf(drop_table,"drop table %s%s",table_name,S_HOST_TABLE_BAK);
		ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, (const char*) drop_table, 0, 0);
		if(ret != SQLITE_OK) {
			return ret;
		}
		
	}

	return ret;
}


int32_t ts_db_tools_parse_xml(const char * szDocName)
{
	char * s_base_bak = "cp %s %s";
	char * s_bak = NULL;
	s_bak = (char *) calloc(strlen(TS_DB_FILE_NAME) + strlen(TS_DB_FILE_NAME_BAK) + strlen(s_base_bak), sizeof(char));
	sprintf(s_bak, s_base_bak, TS_DB_FILE_NAME, TS_DB_FILE_NAME_BAK);
	system(s_bak);
	free(s_bak);
	s_bak = NULL;

	int32_t ret = EXIT_SUCCESS;

	xmlDocPtr doc = NULL;
	xmlNodePtr cur_node = NULL;
	xmlChar *sz_key = NULL;

	char * table_name = NULL;

	xmlAttrPtr attr = NULL;
	xmlChar* sz_attr = NULL;

//	char * drop_sql = NULL;

	char s_insert_sql[TS_DB_SQL_LENGTH] = { 0 };
	char s_inserts[TS_DB_SQL_LENGTH] = { 0 };
	char s_values[TS_DB_SQL_LENGTH] = { 0 };
	char drop_sql[TS_DB_SQL_LENGTH] = { 0 };
	const char * S_HOST_TALBE_START = "t_host_";

	doc = xmlReadFile(szDocName, "UTF-8", XML_PARSE_RECOVER);
	if (NULL == doc) {
		log_err( "Document not parsed sucessfully!\n");
		ret = ERR_DB_XML_PARSE;
		TS_BAK_DB
		TS_XML_FREE
		return ret;
	}

	cur_node = xmlDocGetRootElement(doc);
	if (NULL == cur_node) {
		log_err( "Empty document!\n");
		ret = ERR_DB_XML_PARSE;
		TS_BAK_DB
		TS_XML_FREE
		return ret;
	}

	if (xmlStrcmp(cur_node->name, BAD_CAST "home")) {
		log_err( "Document of the wrong type,root node != \"home\"");
		ret = ERR_DB_XML_PARSE;
		TS_BAK_DB
		TS_XML_FREE
		return ret;
	}

	cur_node = cur_node->xmlChildrenNode;
	//	xmlNodePtr propNodePtr = curNode;

	int32_t i = 0;
//	int32_t max = 0;
	int32_t rows = 0;

	TS_DB_BEGIN_TRANS_FILE
//	ret = EXIT_SUCCESS;
	TS_XML_GET_CHILD

		table_name = (char*) cur_node->name;

		//get properties
		attr = cur_node->properties;
		while (NULL != attr) {

			//before create table ,I'll drop it
			if (!xmlStrcmp(attr->name, (const xmlChar *) "sql")) {
				//if the table isn't start with "t_host_" which means it's server's table ,I'll drop it.
				//drop sql
				bzero(drop_sql, TS_DB_SQL_LENGTH);
				
				//create sql
				sz_attr = xmlGetProp(cur_node, BAD_CAST attr->name);

				if(0 != strncmp(table_name,S_HOST_TALBE_START,strlen(S_HOST_TALBE_START))) {
					sprintf(drop_sql, TS_XML_BASE_DROP, table_name);
					ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, (const char*) drop_sql, 0, 0);
					if (ret != SQLITE_OK) {
						ret = ERR_DB_DROP_TABLE;
					} else {
						ret = EXIT_SUCCESS;
					}
					ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, (const char*) sz_attr, 0, 0);
					if (ret != SQLITE_OK) {
						ret = ERR_DB_CREATE_TABLE;
						TS_BAK_DB
						sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
						TS_XML_FREE
						return ret;
					}
				} else {
					//It is host table.
					ret = upd_host_table((char *)sz_attr,table_name);
					if(ret != SQLITE_OK) {
						log_err("create host table %s error\n",table_name);
						TS_BAK_DB
						sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
						TS_XML_FREE
						return ret;
					}
				}

				//get how much column in create table.
				rows = 1;
				int32_t j = 0;
				for (j = 0; j < strlen((const char *) sz_attr); j++) {
					if (sz_attr[j] == ',') {
						rows++;
					}
				}

			}
			attr = attr->next;
		}



		//get children
		if (NULL != cur_node->xmlChildrenNode) {
			cur_node = cur_node->xmlChildrenNode;

			TS_XML_GET_CHILD
				cur_node = cur_node->xmlChildrenNode;
				i = 0;

				bzero(s_values, sizeof(s_values));
				bzero(s_inserts, sizeof(s_inserts));

				//get values
				TS_XML_GET_CHILD
					if (0 != i) {
						sprintf(s_inserts, "%s,'%s'", s_inserts, cur_node->name);
						sprintf(s_values, "%s,'%s'", s_values, xmlNodeGetContent(cur_node));
					} else {
						sprintf(s_inserts, "'%s'", cur_node->name);
						sprintf(s_values, "'%s'", xmlNodeGetContent(cur_node));
					}
					i++;
				}
				if (i > rows) {
					ret = ERR_DB_INSERT;
					TS_BAK_DB
					TS_XML_FREE
					sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
					return ret;
				} else {
					bzero(s_insert_sql, sizeof(s_insert_sql));
					sprintf(s_insert_sql, TS_XML_BASE_INSERT, table_name, s_inserts, s_values);
				}
				ret = sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, (const char*) s_insert_sql, 0, 0);

				if (ret != SQLITE_OK) {
					ret = ERR_DB_INSERT;
					TS_BAK_DB
					TS_XML_FREE
					sc_sqlite3_exec_by_type(TS_DB_EXEC_TYPE_NOW_NO_TRANS, "rollback transaction", 0, 0);
					return ret;
				}
				cur_node = cur_node->parent;
			}
			cur_node = cur_node->parent;
		}		
	}

	TS_DB_COMMIT_TRANS_FILE

	log_debug_web( "imp success!!!!!\n");

//	char s_cmd[64] = { 0 };
//	sprintf(s_cmd, "killall -15 %s", TS_PROGRAM_NAME);
//	system(s_cmd);

	TS_XML_FREE
	return ret;
}

