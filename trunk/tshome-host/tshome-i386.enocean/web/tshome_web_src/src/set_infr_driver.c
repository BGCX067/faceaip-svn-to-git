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

int32_t cgiMain(void)
{
	int ret = EXIT_SUCCESS;
	log_open("set_infr_driver.c");

	char * data;
	char s_infos[128] = { 0 } ;

	fprintf(cgiOut, "Content-Type:text/html;charset=utf-8\n\n");

	data = getenv("QUERY_STRING");
	if (data == NULL) {
		log_debug_web("<p>错误！数据没有被输入或者数据传输有问题</p>");
	} else {
		//infos
		cgiFormString("infr_driver_value", s_infos, sizeof(s_infos));
		fprintf(stderr,"IR = %s\n",s_infos);
		db_set_infr_driver(s_infos);
	}

//close db
	ret = sc_close();
	if (ret != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();
	return ret;
}

