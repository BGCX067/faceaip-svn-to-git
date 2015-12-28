/*
 * set_serial_info.c
 *
 *  Created on: 2012-3-28
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <cgic.h>
#include <string.h>

#include "db/sqlite3_ctrl.h"
#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include "log/ts_log_tools.h"

int32_t del_serial_infos(char * s_dels)
{
	int32_t ret = EXIT_SUCCESS;
	char *buf;
	char *infos;
	buf = s_dels;
	while ((infos = strsep(&buf, ",")) != NULL) {
//		log_debug_web( "%s\n",infos);
		ts_host_interface_serial_info ts_info;
		ts_info.id = atoi(infos);
		db_del_serial_details(ts_info);
	}

	return ret;
}

int32_t upd_serial_infos(char * serial_infos)
{
	int32_t ret = EXIT_SUCCESS;
	char *buf;
	char *infos;
	buf = serial_infos;
	while ((infos = strsep(&buf, ";")) != NULL) {
		char * buf_details;
		char * details;
		buf_details = infos;
		int32_t i = 0;
		ts_host_interface_serial_info ts_serial_info;
		int8_t flag = TS_FLAG_FALSE;
		while ((details = strsep(&buf_details, ",")) != NULL) {
			if (0 == i) {
				flag = TS_FLAG_TRUE;
				ts_serial_info.id = atoi(details);
			} else if (1 == i) {
				ts_serial_info.baud_rate = atoi(details);
			}
			if (2 == i) {
				ts_serial_info.data_bit = atoi(details);
			}
			if (3 == i) {
				ts_serial_info.stop_bit = atof(details);
			}
			if (4 == i) {
				ts_serial_info.parity_check = atoi(details);
			}
			if (5 == i) {
				strcpy(ts_serial_info.comm_port, details);
			}
			i++;
		}

		if (TS_FLAG_TRUE == flag) {
			db_set_serial_details(ts_serial_info);
		}
	}
	log_debug_web("upd over\n");
	return ret;
}

int32_t cgiMain(void)
{
	int ret = EXIT_SUCCESS;
	log_open("set_serial.c");

	char * data;
	int32_t del_len = 0;
	int32_t info_len = 0;
	char * s_dels = NULL;
	char * s_infos = NULL;

	fprintf(cgiOut, "Content-Type:text/html;charset=utf-8\n\n");
//	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n");
//	printf("<meta http-equiv=\"Refresh\" content=\"0;URL=count.cgi\">");

	data = getenv("QUERY_STRING");
	if (data == NULL) {
		log_debug_web( "<p>错误！数据没有被输入或者数据传输有问题</p>");
	} else {
		//dels
		cgiFormInteger("del_len", &del_len, 0);
		if (del_len > 0) {
			s_dels = (char *) calloc(del_len, sizeof(char));
			cgiFormString("del_ids", s_dels, del_len);
//			log_debug_web("%s\n",s_dels);
			del_serial_infos(s_dels);
		}
		//infos
		cgiFormInteger("info_len", &info_len, 0);
		if (info_len > 0) {
			s_infos = (char *) calloc(info_len, sizeof(char));
			cgiFormString("serial_infos", s_infos, info_len);
			upd_serial_infos(s_infos);
			ts_free(s_infos);
		}
	}

//close db
	ret = sc_close();
	if (ret != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();
	return ret;
}

