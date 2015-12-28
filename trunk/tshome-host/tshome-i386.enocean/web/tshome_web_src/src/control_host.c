/*
 * user_info_show.c
 * apt-get install libcgic-dev
 *  Created on: 2012-3-21
 *      Author: pangt
 */

#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cgic.h>
#include <sys/stat.h>

#include "db/sqlite3_ctrl.h"
#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include "db/ts_db_conn_infos.h"
#include "common/common_define.h"
#include "log/ts_log_tools.h"

int32_t guide_conf()
{

	int32_t ret = EXIT_SUCCESS;

//	stop_pro()
//	;
	log_debug_web("Start run upd script!\n");
	ret = system("./guide_down_conf.sh");
	log_debug_web("End run upd script!\n");
	ret = ret >> 8;

	return ret;
}

int32_t export_log()
{
	int32_t ret = EXIT_SUCCESS;

	ret = system("tar czf ../ts_logs.tar.gz /var/log/message*");
	if (EXIT_SUCCESS != ret) {
		fprintf(cgiOut, "生成文件有误！\n");
		return ret;
	}
	return ret;
}

int32_t set_local(int32_t is_local)
{
	int32_t ret = EXIT_SUCCESS;

	ret = db_set_is_local(is_local);
	if (EXIT_SUCCESS != ret) {
		return ret;
	} else {
		char cmd[128] = { 0 };
		sprintf(cmd, "killall -15 %s", TS_PROGRAM_NAME);
		system(cmd);
	}

	return ret;
}

int cgiMain(void)
{
	int rc = EXIT_SUCCESS;

	log_open("control_host.c");

	char * data;
	int32_t control_id = -1;
	int32_t on_off = -1;

	fprintf(cgiOut, "Content-Type:text/html;charset=utf-8\n\n");
//	fprintf(cgiOut, "<meta http-equiv=\"Refresh\" content=\"0;URL=basic_control.html\">\n");
//	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n");

	data = getenv("QUERY_STRING");
	if (data == NULL) {
		fprintf(cgiOut, "<p>错误！数据没有被输入或者数据传输有问题</p>");
	} else {
		cgiFormInteger("info", &control_id, -1);
		cgiFormInteger("on_off", &on_off, -1);
	}

//send info to data
	if (ON_OFF_HOST == control_id) {
		if (TRUE == on_off) {
			start_pro();
		} else {
			stop_pro();
		}
//		log_debug_web("start host\n");
	} else if (RESTART_HOST == control_id) {
		stop_pro();
		start_pro();
		log_debug_web("RESTART_HOST\n");
	} else if (EXPORT_LOG == control_id) {
		export_log();
	} else if (SET_LOACL == control_id) { //set is local
		set_local(on_off);
	} else if (WEB_UPD_GUIDE == control_id) { //get conf when guide
		//must local
		const char *GUIDE_UPD_RES = "&TS_GUIDE_UPDATE_RESULT=";
		int32_t is_local = TS_IS_LOCAL;
		db_get_is_local(&is_local);
		if (TS_IS_REMOTE == is_local) {
			int32_t upd_res = 0;
			upd_res = guide_conf();
			log_debug_web("upd_res = %d---------->\n", upd_res);
			if (EXIT_SUCCESS == upd_res) {
				fprintf(cgiOut, "\n%s%s\n", GUIDE_UPD_RES, "更新成功");
				log_debug_web("\n%s%s\n", GUIDE_UPD_RES, "update success");
			} else {
				fprintf(cgiOut, "\n%s%s\n", GUIDE_UPD_RES, "更新失败");
				log_debug_web("\n%s%s\n", GUIDE_UPD_RES, "update failed");
			}
		} else {
			fprintf(cgiOut, "\n%s%s\n", GUIDE_UPD_RES, "必须开启外网服务!");
		}
	} else {
		web_action_report_t act_ctrl;
		act_ctrl.data = on_off;
		act_ctrl.fun_code = control_id;
		act_ctrl.start_flag = BUFF_START;
		act_ctrl.source = HOST_WEB_PROGRAM;
		log_debug_web("%d,%d,%d,%d", act_ctrl.data, act_ctrl.fun_code, act_ctrl.start_flag, act_ctrl.source);
		rc = ts_send_data(&act_ctrl);
		if (EXIT_SUCCESS != rc) {
			fprintf(cgiOut, "操作失败，请重试。\n");
		}
	}
//close db
	rc = sc_close();
	if (rc != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();
//	fprintf(cgiOut, "<meta http-equiv=\"Refresh\" content=\"0;URL=../cgi-bin/basic_control.html\">\n");

	return rc;
}

