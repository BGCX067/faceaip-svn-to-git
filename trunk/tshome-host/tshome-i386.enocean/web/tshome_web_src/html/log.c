#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include "db/ts_db_tools.h"
#include "db/ts_db_web_infos.h"

#include "log/ts_log_tools.h"

int cgiMain()
{

	int32_t ret = EXIT_SUCCESS;
	log_open("log.c");

	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-Transitional.dtd\">\n");
	fprintf(cgiOut, "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");

	//login valiate failed
	TS_LOGIN_FAILED

	fprintf(cgiOut, "<head>\n");
	fprintf(cgiOut, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n");
	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n");
	fprintf(cgiOut, "<title>TSHome主机</title>\n");
	fprintf(cgiOut, "<meta name=\"keywords\" content=\"智能,家居,主机,系统,TSHome\" />\n");
	fprintf(cgiOut, "<link rel=\"stylesheet\" href=\"../css/calendar/jscal2.css\"  type=\"text/css\"/>\n");
	fprintf(cgiOut, "<link rel=\"stylesheet\" href=\"../css/calendar/steel/steel.css\"  type=\"text/css\"/>\n");
	fprintf(cgiOut, "<link rel=\"stylesheet\" href=\"../css/main.css\"  type=\"text/css\"/>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/jquery-1.7.1.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/calendar/jscal2.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/calendar/lang/en.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/log.js\"></script>\n");
	fprintf(cgiOut, "</head>\n");
	fprintf(cgiOut, "<body>\n");
	fprintf(cgiOut, "<!--主体 start-->\n");
	fprintf(cgiOut, "<div class=\"containerbackground\">\n");
	fprintf(cgiOut, "	<div class=\"container_inner_back\">\n");
	fprintf(cgiOut, "		<div class=\"container\">\n");
	fprintf(cgiOut, "			<!--页头 start-->\n");
	fprintf(cgiOut, "			<div class=\"head\">\n");
	fprintf(cgiOut, "				<span><a href=\"#\"><img src=\"../images//logo_index.jpg\" width=\"300\" height=\"90\"/></a></span>\n");
	TS_WEB_LOGIN_OUT
	fprintf(cgiOut, "			</div>\n");
	fprintf(cgiOut, "			<!--页头 end-->\n");
	fprintf(cgiOut, "			<!--主体 start-->\n");
	fprintf(cgiOut, "			<div class=\"center\">\n");
//	TS_WEB_MENUS
	fprintf(cgiOut, "				<!--导航 start-->\n");
	fprintf(cgiOut, "				<div class=\"navigation\">\n");
	fprintf(cgiOut, "					<ul>\n");\
	fprintf(cgiOut, "						<li ><a href=\"basic_control.html\">基本控制</a></li>\n");
	fprintf(cgiOut, "						<li><a href=\"home_serial_port.html\">串口参数</a></li>\n");
	fprintf(cgiOut, "						<li><a href=\"pathway_info.html\">网关通路</a></li>\n");
	fprintf(cgiOut, "						<li><a href=\"device_link_access.html\">串口设备通路</a></li>\n");
	fprintf(cgiOut, "						<li><a href=\"infrared_device_pathway.html\">红外设备通路</a></li>\n");
//	fprintf(cgiOut, "						<li class=\"selected\"><a href=\"log.html\">日志信息</a></li>\n");
	fprintf(cgiOut, "					</ul>\n");
	fprintf(cgiOut, "				</div>\n");
	fprintf(cgiOut, "				<!--导航 end-->\n");
	fprintf(cgiOut, "				<!--内容 start-->\n");
	fprintf(cgiOut, "				<div class=\"content\">\n");
	fprintf(cgiOut, "					<!--查看条件 start-->\n");
	fprintf(cgiOut, "					<div class=\"check_condition\">\n");
	fprintf(cgiOut, "						<!--时间 start-->\n");
	fprintf(cgiOut, "						<div class=\"date_check\">\n");
	fprintf(cgiOut, "							<form>\n");
	fprintf(cgiOut, "							<table width=\"420\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "  								<tr height=\"21\">\n");
	fprintf(cgiOut, "    								<td width=\"55\">开始时间:</td>\n");
	fprintf(cgiOut, "    								<td width=\"130\"><input type=\"text\" class=\"date_second\" id=\"start_date\" value=\"2012-03-23 05:30:00\"/></td>\n");
	fprintf(cgiOut, "    								<td width=\"55\">结束时间:</td>\n");
	fprintf(cgiOut, "    								<td width=\"130\"><input type=\"text\" class=\"date_second\" id=\"stop_date\" value=\"2012-03-23 05:30:00\"/></td>\n");
	fprintf(cgiOut, "    								<td width=\"50\"><input type=\"button\" class=\"date_check_btn\" value=\"查看\" /></td>\n");
	fprintf(cgiOut, "  								</tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "							</form>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--时间 end-->\n");
	fprintf(cgiOut, "						<!--级别 start-->\n");
	fprintf(cgiOut, "						<div class=\"rank_check\">\n");
	fprintf(cgiOut, "							<form>\n");
	fprintf(cgiOut, "							<div class=\"rank_select\" style=\"position:relative\">\n");
	fprintf(cgiOut, "								<input type=\"text\" class=\"rank_name\" value=\"级别选择\"/>\n");
	fprintf(cgiOut, "								<div class=\"rank_second\" style=\"display:none;\">\n");
	fprintf(cgiOut, "								<ul>\n");
	fprintf(cgiOut, "									<li class=\"blue\">错误</li>\n");
	fprintf(cgiOut, "									<li>警告</li>\n");
	fprintf(cgiOut, "									<li>信息</li>\n");
	fprintf(cgiOut, "								</ul>\n");
	fprintf(cgiOut, "								</div>\n");
	fprintf(cgiOut, "							</div>\n");
	fprintf(cgiOut, "							<input type=\"button\" class=\"rank_check_btn\" value=\"查看\" />\n");
	fprintf(cgiOut, "							<div class=\"clear\"></div>\n");
	fprintf(cgiOut, "							</form>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--级别 end-->\n");
	fprintf(cgiOut, "						<div class=\"clear\"></div>\n");
	fprintf(cgiOut, "					</div>\n");
	fprintf(cgiOut, "					<!--查看条件 end-->\n");
	fprintf(cgiOut, "					<!--表格 start-->\n");
	fprintf(cgiOut, "					<div class=\"check_table\">\n");
	fprintf(cgiOut, "						<!--标题 start-->\n");
	fprintf(cgiOut, "						<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\">\n");
	fprintf(cgiOut, "						  <tr>\n");
	fprintf(cgiOut, "							<th width=\"124\">时间</th>\n");
	fprintf(cgiOut, "							<th width=\"160\">级别</th>\n");
	fprintf(cgiOut, "							<th width=\"430\">事件</th>\n");
	fprintf(cgiOut, "						  </tr>\n");
	fprintf(cgiOut, "						</table>\n");
	fprintf(cgiOut, "						<!--标题 end-->\n");
	fprintf(cgiOut, "						<!--列表 start-->\n");
	fprintf(cgiOut, "						<div class=\"check_list\">\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\" style=\"border-top:0px;\">\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--列表 end-->\n");
	fprintf(cgiOut, "					</div>\n");
	fprintf(cgiOut, "					<!--表格 end-->\n");
	fprintf(cgiOut, "				</div>\n");
	fprintf(cgiOut, "				<!--内容 end-->\n");
	fprintf(cgiOut, "			</div>\n");
	fprintf(cgiOut, "			<!--主体 end-->\n");
	fprintf(cgiOut, "		</div>\n");
	fprintf(cgiOut, "	</div>\n");
	fprintf(cgiOut, "</div>\n");
	fprintf(cgiOut, "<!--主体 end-->\n");
	fprintf(cgiOut, "<!--底部 start-->\n");
	fprintf(cgiOut, "<div class=\"footerbackground\">\n");
	fprintf(cgiOut, "	<div class=\"footer\">南京天溯自动化控制系统有限公司-TSHome All Right Reserved!</div>\n");
	fprintf(cgiOut, "</div>\n");
	fprintf(cgiOut, "<!--底部 end-->\n");
	fprintf(cgiOut, "</body>\n");
	fprintf(cgiOut, "</html>\n");

	//close db
	ret = sc_close();
	if (ret != SQLITE_OK) {
		return EXIT_FAILURE;
	} else {
		ret = EXIT_SUCCESS;
	}

	log_close();
	return EXIT_SUCCESS;
}
