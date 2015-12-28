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
//#ifdef TSHOME_WEB
//#include "tr1/stdbool.h"
//#endif
//#include "dev_ctrl/Conversion.h"
#include <dlfcn.h>
#include <unistd.h>
#include "common/common_define.h"
#include "log/ts_log_tools.h"

int32_t cgiMain(void)
{
	int ret = EXIT_SUCCESS;
	log_open("on_off.c");

	char * data;
	int32_t id = 0;
	int32_t type = TS_WEB_ON_OFF_NO;
	int32_t on_off = TS_WEB_OFF;

	fprintf(cgiOut, "Content-Type:text/html;charset=utf-8\n\n");
//	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n");

	data = getenv("QUERY_STRING");
	if (data == NULL) {
		log_debug_web( "<p>错误！数据没有被输入或者数据传输有问题</p>");
	} else {
		//infos
		cgiFormInteger("type", &type, 0);
		cgiFormInteger("value", &id, 0);
		cgiFormInteger("on_off", &on_off, 0);
	}

	web_action_report_t act_ctrl;
	act_ctrl.data = on_off;
	act_ctrl.start_flag = BUFF_START;
	act_ctrl.source = HOST_WEB_PROGRAM;
	act_ctrl.obj = id;

	if (TS_WEB_ON_OFF_INFRARED == type) {
		act_ctrl.fun_code = SET_ON_OFF_INFR;
	} else if (TS_WEB_ON_OFF_GATEWAY == type) {
		act_ctrl.fun_code = SET_ON_OFF_GW;
	} else if (TS_WEB_ON_OFF_SERIAL == type) {
		act_ctrl.fun_code = SET_ON_OFF_SERIAL;
	}

	ret = ts_send_data(&act_ctrl);
	if (EXIT_SUCCESS != ret) {
		fprintf(cgiOut, "开关失败,请检测主机程序是否启动\n");
		return ret;
	}

	ret = act_ctrl.data;
	if (EXIT_SUCCESS != ret) {
		fprintf(cgiOut, "开关失败\n");
		return ret;
	}

//	if (TS_WEB_ON_OFF_INFRARED == type) {
//		protocol_ifrared_open(id, on_off);
//	} else if (TS_WEB_ON_OFF_GATEWAY == type) {
//		protocol_gateway_open(id, on_off);
//	} else if (TS_WEB_ON_OFF_SERIAL == type) {
//		protocol_serial_open(id, on_off);
//	}

//		printf("<meta http-equiv=\"Refresh\" content=\"0;URL=count.cgi\">");

//close db
	ret = sc_close();
	if (ret != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();

	return ret;
}

