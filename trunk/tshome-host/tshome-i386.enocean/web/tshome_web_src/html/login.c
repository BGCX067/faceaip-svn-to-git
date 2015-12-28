#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include "db/ts_db_web_infos.h"
#include "log/ts_log_tools.h"

char user_name[32] = { 0 };
char passwd[32] = { 0 };
int32_t is_validate = TS_WEB_VALIDATE_FALSE;

int32_t validate_user(const char * user_name, const char * passwd)
{
	int32_t ret = EXIT_SUCCESS;

	if ((NULL == user_name) && (NULL == passwd)) {
		return ret;
	}

	if (0 == strlen(user_name)) {
		fprintf(cgiOut, "<script>is_null(\"用户名\")</script>\n");
		return ret;
	}
	if (0 == strlen(passwd)) {
		fprintf(cgiOut, "<script>is_null(\"密码\")</script>\n");
		return ret;
	}

	ts_host_base_user_info user_info;
	strcpy(user_info.user_name, user_name);
	strcpy(user_info.passwd, passwd);
	int8_t pass = TS_WEB_VALIDATE_FALSE;
	ret = db_web_validate_user(user_info, &pass);

	if ((EXIT_SUCCESS == ret) && (TS_WEB_VALIDATE_TRUE == pass)) {
		//save login info
		char * login_ip = getenv("REMOTE_ADDR");
//		log_debug_web( "%s\n", login_ip);
		ret = ts_web_login_add(login_ip);
		if (ret == EXIT_SUCCESS) {
			ts_host_base_user_info base_info;
			db_get_base_user_info(&base_info);
			if (TS_IS_FIRST_USE_NOT == base_info.is_first_use) {
				fprintf(cgiOut, "<meta http-equiv=\"Refresh\" content=\"0;URL=curr_state.html\">\n");
			} else {
				fprintf(cgiOut, "<meta http-equiv=\"Refresh\" content=\"0;URL=base_info.html\">\n");
			}
		}
		exit(0);
	} else if ((strlen(user_name) > 0) && (strlen(passwd)) > 0) {
		fprintf(cgiOut, "<script>loginFailed()</script>\n");
	}
	return ret;
}

int32_t get_login_info()
{
	char * data;
	data = getenv("QUERY_STRING");
	if ((data == NULL) || (strlen(data) == 0)) {
		is_validate = TS_WEB_VALIDATE_FALSE;
	} else {
		is_validate = TS_WEB_VALIDATE_TRUE;
		cgiFormString("user_name", user_name, sizeof(user_name));
		cgiFormString("passwd", passwd, sizeof(passwd));
	}
	return EXIT_SUCCESS;
}

int cgiMain()
{

	int rc = EXIT_SUCCESS;
	log_open("login.c");

	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-Transitional.dtd\">\n");
	fprintf(cgiOut, "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");
	fprintf(cgiOut, "<head>\n");
	fprintf(cgiOut, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n");
	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n");
	fprintf(cgiOut, "<title>TSHome主机</title>\n");
	fprintf(cgiOut, "<meta name=\"keywords\" content=\"智能,家居,主机,系统,TSHome\" />\n");
	fprintf(cgiOut, "<link rel=\"stylesheet\" href=\"../css/main.css\"  type=\"text/css\"/>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/jquery-1.7.1.js\"></script>	\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/ts_login.js\"></script>	\n");

	//web page
	fprintf(cgiOut, "</head>\n");
	fprintf(cgiOut, "<body>\n");

	//del last login info
	char * login_ip = getenv("REMOTE_ADDR");
	ts_web_login_del(login_ip);

	get_login_info();

	fprintf(cgiOut, "<!--主体 start-->\n");
	fprintf(cgiOut, "<div class=\"containerbackground_second\">\n");
	fprintf(cgiOut, "	<div class=\"container_back\">\n");
	fprintf(cgiOut, "		<div class=\"container\">\n");
	fprintf(cgiOut, "			<!--页头 start-->\n");
	fprintf(cgiOut, "			<div class=\"header\">\n");
	fprintf(cgiOut, "				<span><a href=\"#\"><img src=\"../images//logo.jpg\" width=\"300\" height=\"90\"/></a></span>\n");
	fprintf(cgiOut, "			</div>\n");
	fprintf(cgiOut, "			<!--页头 end-->\n");
	fprintf(cgiOut, "			<!--用户登录 start-->\n");
	fprintf(cgiOut, "			<div class=\"loginbox\">\n");
	fprintf(cgiOut, "				<!--标题 start-->\n");
	fprintf(cgiOut, "				<div class=\"title\">用户登录</div>\n");
	fprintf(cgiOut, "				<!--标题 end-->\n");
	fprintf(cgiOut, "				<!--登录 start-->\n");
//	fprintf(cgiOut,"				<form action=\"../cgi-bin/login_validate.html\">\n");
	fprintf(cgiOut, "				<form action=\"../cgi-bin/login.html\">\n");
	fprintf(cgiOut, "				<table width=\"471\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_login\">\n");
//	fprintf(cgiOut, "				  <tr align=\"left\" valign=\"middle\">\n");
//	fprintf(cgiOut, "					<td width=\"211\"><span class=\"title\">用户名:</span></td>\n");
//	fprintf(cgiOut, "					<td width=\"260\"><span><input name=\"user_name\" type=\"text\" value=\"%s\" class=\"input_login\"/></span></td>\n", user_name);
//	fprintf(cgiOut, "					<td width=\"260\"><span><input name=\"user_name\" type=\"button\" value=\"dd\" class=\"input_login\"/></span></td>\n");
//	fprintf(cgiOut, "				  </tr>\n");
	fprintf(cgiOut, "				  <tr align=\"left\" valign=\"middle\">\n");
	fprintf(cgiOut, "					<td width=\"211\"><span class=\"title\">用户名:</span></td>\n");
	fprintf(cgiOut, "					<td width=\"260\"><span><input name=\"user_name\" id=\"user_name\" type=\"text\" value=\"%s\" class=\"input_login\"/></span></td>\n", user_name);
	fprintf(cgiOut, "				  <tr align=\"left\" valign=\"middle\">\n");
	fprintf(cgiOut, "					<td width=\"211\"><span class=\"title\">密码:</span></td>\n");
	fprintf(cgiOut, "					<td width=\"260\"><span><input name=\"passwd\" type=\"password\" value=\"%s\" class=\"input_login\"/></span></td>\n", passwd);
	fprintf(cgiOut, "				  </tr>\n");
	fprintf(cgiOut, "				  <tr align=\"left\" valign=\"middle\">\n");
	fprintf(cgiOut, "					<td width=\"211\"><span></span></td>\n");
	fprintf(cgiOut, "					<td width=\"260\"><span><input type=\"submit\" class=\"submit_login\" value=\"登录\" /></span></td>\n");
	fprintf(cgiOut, "				  </tr>\n");
	fprintf(cgiOut, "				</table>\n");
	fprintf(cgiOut, "				</form>\n");
	fprintf(cgiOut, "				<!--登录 end-->\n");
	fprintf(cgiOut, "			</div>\n");
	fprintf(cgiOut, "			<!--用户登录 end-->\n");
	fprintf(cgiOut, "		</div>\n");
	fprintf(cgiOut, "	</div>\n");
	fprintf(cgiOut, "</div>\n");
	fprintf(cgiOut, "<!--主体 end-->\n");
	fprintf(cgiOut, "<!--底部 start-->\n");
	fprintf(cgiOut, "<div class=\"footerbackground\">\n");
	fprintf(cgiOut, "	<div class=\"footer\">南京天溯自动化控制系统有限公司-TSHome All Right Reserved!</div>\n");
	fprintf(cgiOut, "</div>\n");
	fprintf(cgiOut, "<!--底部 end-->\n");

	//login validate
	if (TS_WEB_VALIDATE_TRUE == is_validate) {
		validate_user(user_name, passwd);
	}

	fprintf(cgiOut, "</body>\n");
	fprintf(cgiOut, "</html>\n");

	//close db
	rc = sc_close();
	if (rc != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();
	return EXIT_SUCCESS;
}
