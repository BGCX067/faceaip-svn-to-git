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

int32_t upd_serial_device_info(char * serial_infos)
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
		ts_web_host_serial_device_info serial_device_info;
		int8_t flag = TS_FLAG_FALSE;
		while ((details = strsep(&buf_details, ",")) != NULL) {
			if (0 == i) {
				flag = TS_FLAG_TRUE;
				serial_device_info.id = atoi(details);
			} else if (1 == i) {
				strcpy(serial_device_info.host_info, details);
			}
			i++;
		}

		if (TS_FLAG_TRUE == flag) {
			db_web_set_host_device_info(serial_device_info);
		}
	}
	return ret;
}

int32_t cgiMain(void)
{
	int ret = EXIT_SUCCESS;
	log_open("set_comm.c");

	char * data;
	int32_t info_len = 0;
	char * s_infos = NULL;

	fprintf(cgiOut, "Content-Type:text/html;charset=utf-8\n\n");
//	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n");
//	printf("<meta http-equiv=\"Refresh\" content=\"0;URL=count.cgi\">");

	data = getenv("QUERY_STRING");
	if (data == NULL) {
		log_debug_web( "<p>错误！数据没有被输入或者数据传输有问题</p>");
	} else {
		//infos
		cgiFormInteger("info_len", &info_len, 0);
		if (info_len > 0) {
			s_infos = (char *) calloc(info_len, sizeof(char));
			cgiFormString("info", s_infos, info_len);
			upd_serial_device_info(s_infos);
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

