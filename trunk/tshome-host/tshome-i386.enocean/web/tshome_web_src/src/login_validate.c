/*
 * login_validate.c
 *
 *  Created on: 2012-3-24
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <cgic.h>

#include "db/sqlite3_ctrl.h"
#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include "log/ts_log_tools.h"

int32_t validate_user(const char * user_name, const char * passwd)
{
	int32_t ret = EXIT_SUCCESS;
	ts_host_base_user_info user_info;
	strcpy(user_info.user_name, user_name);
	strcpy(user_info.passwd, passwd);
	int8_t pass = TS_WEB_VALIDATE_FALSE;
	ret = db_web_validate_user(user_info, &pass);

	if ((EXIT_SUCCESS == ret) && (TS_WEB_VALIDATE_TRUE == pass)) {
		fprintf(cgiOut, "<p>SUCCESS</p>\n");
		fprintf(cgiOut, "<meta http-equiv=\"Refresh\" content=\"0;URL=basic_control.html\">\n");
	} else {
		fprintf(cgiOut, "<p>User name or password error!</p>\n");
	}
	return ret;
}

int32_t cgiMain(void)
{
	char * data;
	char user_name[32];
	char passwd[32];

	//初始化数据库
	int rc = EXIT_SUCCESS;
	log_open("login_validate.c");

//	log_debug_web( "over\n");
	fprintf(cgiOut, "Content-Type:text/html;charset=utf-8\n\n");
	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n");

	data = getenv("QUERY_STRING");
	if (data == NULL) {
//		log_debug_web("null");
		fprintf(cgiOut, "<p>错误！数据没有被输入或者数据传输有问题</p>");
	} else {
		log_debug_web("not null\n");
		cgiFormString("user_name", user_name, sizeof(user_name));
		cgiFormString("passwd", passwd, sizeof(passwd));
		validate_user(user_name, passwd);
	}

	//close db
	rc = sc_close();
	if (rc != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();
	return EXIT_SUCCESS;
}
