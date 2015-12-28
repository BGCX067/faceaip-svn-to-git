/*
 * set_ip_port.c
 *
 *  Created on: 2012-3-26
 *      Author: root
 */

/*
 * user_info_show.c
 * apt-get install libcgic-dev
 *  Created on: 2012-3-21
 *      Author: pangt
 */

#include <stdio.h>
#include <stdlib.h>
#include <cgic.h>

#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include "log/ts_log_tools.h"
#include "common/common_define.h"

int cgiMain(void)
{
	int rc = EXIT_SUCCESS;
	log_open("set_user_info.c");
	char * data;

	printf("Content-Type:text/html;charset=utf-8\n\n");

	TS_LOGIN_FAILED
	data = getenv("QUERY_STRING");
	if (data == NULL) {

	} else {
		//get value
		ts_host_base_user_info host_info;
		db_get_base_user_info(&host_info);

#define TS_PASSWD_LEN 32
		char old_passwd[TS_PASSWD_LEN] = { 0 };
		char new_passwd[TS_PASSWD_LEN] = { 0 };
		char reply_passwd[TS_PASSWD_LEN] = { 0 };

		cgiFormString("old_passwd", old_passwd, sizeof(old_passwd));
		//validate old password
		if (0 != strcmp(host_info.passwd, old_passwd)) {
			fprintf(cgiOut, "初始密码错误，请核实！\n");
			return EXIT_FAILURE;
		}

		//validate password is the same
		cgiFormString("new_passwd", new_passwd, sizeof(new_passwd));
		cgiFormString("reply_passwd", reply_passwd, sizeof(reply_passwd));

		if (0 != strcmp(new_passwd, reply_passwd)) {
			fprintf(cgiOut, "两次输入的密码不相同，请重试\n");
			return EXIT_FAILURE;
		}

		bzero(host_info.passwd, sizeof(host_info.passwd));
		strcpy(host_info.passwd, new_passwd);
		rc = db_set_base_user_info(host_info);
		if (EXIT_SUCCESS == rc) {
			fprintf(cgiOut, "修改成功\n");
		}
	}

	//close db
	rc = sc_close();
	if (rc != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();
	return rc;
}

