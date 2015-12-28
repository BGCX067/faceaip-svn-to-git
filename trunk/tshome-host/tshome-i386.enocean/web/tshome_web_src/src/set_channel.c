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

int32_t upd_infr_device_channel(char * serial_infos)
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
		ts_infrared_device infr_device_info;
		int8_t flag = TS_FLAG_FALSE;
		while ((details = strsep(&buf_details, ",")) != NULL) {
			if (0 == i) {
				flag = TS_FLAG_TRUE;
				infr_device_info.id = atoi(details);
			} else if (1 == i) {
				if ((NULL == details) || (0 == strlen(details))) {
					flag = TS_FLAG_FALSE;
				} else {
					infr_device_info.channel = atoi(details);
				}
			}
			i++;
		}

		if (TS_FLAG_TRUE == flag) {
//			db_set_serial_details(ts_serial_info);
			db_web_set_infr_device_info(infr_device_info);
		}
	}
	return ret;
}

int32_t cgiMain(void)
{
	int ret = EXIT_SUCCESS;
	log_open("set_channel.c");

	char * data;
	int32_t info_len = 0;
	char * s_infos = NULL;

	fprintf(cgiOut, "Content-Type:text/html;charset=utf-8\n\n");
//	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n");
//	printf("<meta http-equiv=\"Refresh\" content=\"0;URL=count.cgi\">");

	data = getenv("QUERY_STRING");
	if (data == NULL) {
		log_debug_web("<p>错误！数据没有被输入或者数据传输有问题</p>");
	} else {
		//infos
		cgiFormInteger("info_len", &info_len, 0);
		if (info_len > 0) {
			s_infos = (char *) calloc(info_len, sizeof(char));
			cgiFormString("info", s_infos, info_len);
//			log_debug_web("%s\n",s_infos);
			upd_infr_device_channel(s_infos);
			free(s_infos);
		}
	}

	//set is_first_use no
	ts_host_base_user_info base_info;
	cgiFormInteger("is_first_use", &base_info.is_first_use, -1);
	if (TS_IS_FIRST_USE_NOT == base_info.is_first_use) {
		db_get_base_user_info(&base_info);
		base_info.is_first_use = TS_IS_FIRST_USE_NOT;
		db_set_base_user_info(base_info);
		char cmd[128] = { 0 };
		//sprintf(cmd, "cp -f %sbak_start.sh %sstart.sh; echo ./%s >> %sstart.sh &", TS_PROGRAM_BIN_PATH, TS_PROGRAM_BIN_PATH, TS_MONITOR_NAME, TS_PROGRAM_BIN_PATH);
		const char * start_monitor = "start_monitor.sh";
		sprintf(cmd, "echo %s/%s > %s/%s;chmod 755 %s/%s", TS_PROGRAM_BIN_PATH, TS_MONITOR_NAME, TS_PROGRAM_BIN_PATH, start_monitor, TS_PROGRAM_BIN_PATH, start_monitor);
		system(cmd);
	}

//close db
	ret = sc_close();
	if (ret != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();
	return ret;
}

