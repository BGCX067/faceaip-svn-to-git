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
		char login_server_ip[TS_PASSWD_LEN] = { 0 };
		char login_userpassword[TS_PASSWD_LEN] = { 0 };
		char login_username[TS_PASSWD_LEN] = { 0 };
		int32_t port = -1;

		cgiFormString("login_server_ip", login_server_ip, sizeof(login_server_ip));
		cgiFormString("login_userpassword", login_userpassword, sizeof(login_userpassword));
		cgiFormString("login_username", login_username, sizeof(login_username));
		cgiFormInteger("login_server_port",&port,-1);


		strcpy(host_info.login_passwd,login_userpassword);
		strcpy(host_info.login_user_name,login_username);
		strcpy(host_info.server_name,login_server_ip);
		host_info.server_port = port;
		rc = db_set_base_user_info(host_info);
		if (EXIT_SUCCESS == rc) {
			log_debug_web("update success");
//			fprintf(cgiOut, "修改成功\n");
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

