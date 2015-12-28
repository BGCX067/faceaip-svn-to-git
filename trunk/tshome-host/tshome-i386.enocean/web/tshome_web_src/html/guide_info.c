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
	log_open("guide_info.c");

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
	fprintf(cgiOut, "<link rel=\"stylesheet\" href=\"../css/main.css\"  type=\"text/css\"/>\n");
	fprintf(cgiOut, "<!--[if IE 6]>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/iepng.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\">\n");
	fprintf(cgiOut, "EvPNG.fix('div, ul, img, li, input, a, span, h3, b');\n");
	fprintf(cgiOut, "</script>\n");
	fprintf(cgiOut, "<![endif]-->\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/jquery-1.7.1.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/basic_control.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/ts_set_base_info.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" src=\"../js/jquery-1.3.2.min.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" src=\"../js/jquery.validate.min.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" src=\"../js/function.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" src=\"../js/jquery.form.js\"></script>\n");

	//
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
	fprintf(cgiOut, "<script type=\"text/javascript\">print_left_guide(%d)</script>\n", TS_LEFT_GUIDE_GUIDE);

	fprintf(cgiOut, "				<!--内容 start-->\n");
	fprintf(cgiOut, "				<div class=\"content\" >\n");

	fprintf(cgiOut, "				<div class=\"title_top\">获取基本配置</div>\n");
	fprintf(cgiOut, "				<div class=\"text_info\">\n");
	fprintf(cgiOut, "					请您获取您在网站上的配置信息。<br/>\n");
	fprintf(cgiOut, "					如果您可以联网，请点击开启程序按钮，这大约需要花费您几分钟时间。<br/>\n");
	fprintf(cgiOut, "					您也可以进行配置导入，您可以从TSHOME上网站获取最新配置。<br/>\n");
	fprintf(cgiOut, "					地址为：http://www.ts-home.cn:9000/TSRemote/<br/>\n");
	fprintf(cgiOut, "				</div>\n");

	fprintf(cgiOut, "				<div class=\"basic_control\">\n");

	//get from server
	fprintf(cgiOut, "						<div class=\"title\">可联网</div>\n");
	fprintf(cgiOut, "						<div class=\"button\"  style=\"text-align:left\"><span style=\"padding-left:25px\"><input type=\"button\" class=\"get_conf_btn\" /></span></div>\n");

	//get from xml
	fprintf(cgiOut, "						<div class=\"title\">不可联网</div>\n");
	fprintf(cgiOut, "\n						<!--配置文件导入 start-->\n");
	fprintf(cgiOut, "						<br/>\n");
//	fprintf(cgiOut, "						<form enctype=\"multipart/form-data\" id=\"form1\">\n");
//	fprintf(cgiOut, " 							<input TYPE=TEXT ID=\"txtImgUrl\" style=\"display:none\">\n");
//	fprintf(cgiOut, " 							<a href=\"javascript:void(0);\"><span style=\"padding-left:25px\"><input type=\"file\" id=\"FileUpload\" name=\"FileUpload\" onchange=\"SingleUpload('txtImgUrl','FileUpload')\" /></span></a>\n");
//	fprintf(cgiOut, "						</form>\n");
	fprintf(cgiOut, "						<form enctype=\"multipart/form-data\" id=\"form1\">\n");
	fprintf(cgiOut, " 							<input TYPE=\"TEXT\" ID=\"txtImgUrl\" style=\"display:none\">\n");
	fprintf(cgiOut, " 							<a href=\"javascript:void(0);\"><span style=\"padding-left:25px\"><input type=\"file\" id=\"file_load\" name=\"fileload\" onchange=\"SingleUpload('txtImgUrl','FileUpload')\" /></span></a>\n");
	fprintf(cgiOut, "						</form>\n");
	fprintf(cgiOut, "						<!--配置文件导入 end-->\n");

	//bottom button
	fprintf(cgiOut, "					<div class=\"button\">\n");
	fprintf(cgiOut, "						<input id=\"pre_btn\" type=\"button\" class=\"pre_btn\" onclick=\"back_page()\"/>\n");
	fprintf(cgiOut, "						<input id=\"next_btn\" type=\"button\" style=\"display:none\" class=\"next_btn\" onclick=\"jumpPage('%s%s')\"/>\n", "../cgi-bin/home_serial_port.html?", TS_PAGE_GUIDE);
	fprintf(cgiOut, "					</div>\n");

	fprintf(cgiOut, "				</div>\n");
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

	//lock the screen
	fprintf(cgiOut, "<input type=\"text\" value=\"正在处理中，请稍后......\" class=\"ts_running\" style='display:none'/>\n");
	fprintf(cgiOut, "<input id=\"ts_wait\" type=\"text\" value=\"正在处理中，请稍后1......\" class=\"ts_wait\" style='display:none'/>\n");
	fprintf(cgiOut, "<div class=\"lock\" style='display:none'>\n");
	fprintf(cgiOut, "</div>\n");

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
