#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>
#include <signal.h>

#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include "db/ts_db_conn_infos.h"
#include "common/remote_def.h"
#include "common/common_define.h"
#include "log/ts_log_tools.h"

#define PAGE_COUNT 9
#define PAGE_TYPE_HOST_CONTROL 0
#define PAGE_TYPE_UPDATE 1
#define PAGE_TYPE_CONF_IMP 2
#define PAGE_TYPE_STOP_NET 3
#define PAGE_TYPE_IS_LOCAL 4
#define PAGE_TYPE_NET_INFO 5
#define PAGE_TYPE_EXPORT_LOG 6
#define PAGE_TYPE_USER_INFO 7
#define PAGE_TYPE_SERVER_INFO 8

void set_net(char *net_name, int32_t net_type, char * is_last, int32_t can_dhcp, int32_t can_dns)
{
	ts_host_net_info net_info;
	char brd_addr[32] = { 0 };
	char mac[32] = { 0 };
	fprintf(cgiOut, "							<div class=\"second_title\">%s 网卡配置</div>\n", net_name);
	fprintf(cgiOut, "							<div class=\"second_content%s\">\n", is_last);
	bzero(brd_addr, sizeof(brd_addr));
	bzero(mac, sizeof(mac));
	strcpy(net_info.name, net_name);
	db_web_get_host_net(net_name, &net_info);
	ts_net_get_gateway(net_info.gw, net_name);
	ts_net_get_net_infos(net_info.ip, net_info.netmask, brd_addr, mac, net_name);
	fprintf(cgiOut, "							<form method=\"post\" id=\"set_ip_prot_%s\">\n", net_name);
	fprintf(cgiOut, "							<table width=\"600\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
//	fprintf(cgiOut, "								<tr height=\"45\" valign=\"left\">\n");

//can dhcp
	if (TS_WEB_CAN_DHCP == can_dhcp) {
		fprintf(cgiOut, "								<tr height=\"45\" valign=\"left\">\n");
		fprintf(cgiOut, "									<td width=\"70\" align=\"left\" ><span><b>自动获取：</b></span></td>\n");
		if (TS_WEB_DHCP == net_info.is_dhcp) {
			fprintf(cgiOut, "    								<td align=\"left\"><input name=\"is_dhcp\" id=\"dhcp_y\" type=\"radio\" checked value=\"%d\" onclick=\"dhcp_yes(this)\" /> 是 <span style=\"padding-left:15px \"></span><input name=\"is_dhcp\" type=\"radio\" value=\"%d\" onclick=\"dhcp_no(this)\"/>否</td>\n", TS_WEB_DHCP, TS_WEB_NOT_DHCP);
			fprintf(cgiOut, "									<script type=\"text/javascript\" >$(function(){dhcp_yes(\"input#dhcp_y\");\n");
			fprintf(cgiOut, "									});</script>\n");
		} else {
			fprintf(cgiOut, "    								<td><input name=\"is_dhcp\" type=\"radio\"  value=\"%d\" onclick=\"dhcp_yes(this)\" /> 是 <span style=\"padding-left:15px\"></span><input name=\"is_dhcp\" type=\"radio\" value=\"%d\" checked onclick=\"dhcp_no(this)\" />否</td>\n", TS_WEB_DHCP, TS_WEB_NOT_DHCP);
		}
		fprintf(cgiOut, "							  	</tr>\n");
	} else {
		net_info.is_dhcp = TS_WEB_NOT_DHCP;
	}

	fprintf(cgiOut, "								<tr height=\"45\" valign=\"left\">\n");
	//ip
	fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>IP地址：</b></span></td>\n");
	fprintf(cgiOut, "									<td width=\"220\" align=\"left\"><span class=\"home_ip_show home_note_ip\">%s</span>\n", net_info.ip);
	int32_t i_ip = 0;
	for (i_ip = 0; i_ip < TS_IP_UNIT_COUNT; i_ip++) {
		fprintf(cgiOut, "									<input size=\"2\" maxlength=\"3\" value=\"0\" type=\"text\" class=\"home_ip_edit home_ip_%d\" style=\"display:none\"/>\n", i_ip);
		if (i_ip != TS_IP_UNIT_COUNT - 1) {
			fprintf(cgiOut, "								<span class=\"home_ip_edit home_ip_point\" style=\"display:none\">.</span>\n");
		}
	}
	fprintf(cgiOut, "									</td>\n");

//	fprintf(cgiOut, "							  	</tr>\n");
//	fprintf(cgiOut, "							 	<tr height=\"45\" valign=\"middle\">\n");
	fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>网关：</b></span></td>\n");
	fprintf(cgiOut, "									<td width=\"220\" align=\"left\"><span class=\"home_ip_show home_note_gw\">%s</span>\n", net_info.gw);
	for (i_ip = 0; i_ip < TS_IP_UNIT_COUNT; i_ip++) {
		fprintf(cgiOut, "									<input size=\"2\" maxlength=\"3\" value=\"0\" type=\"text\" class=\"home_ip_edit home_gw_%d\" style=\"display:none\"/>\n", i_ip);
		if (i_ip != TS_IP_UNIT_COUNT - 1) {
			fprintf(cgiOut, "								<span class=\"home_ip_edit home_gw_point\" style=\"display:none\">.</span>\n");
		}
	}
	fprintf(cgiOut, "									</td>\n");
	fprintf(cgiOut, "									<td width=\"1\"><input name=\"net_name\" type=\"text\" style=\"display:none\" value=\"%s\"/></td>\n", net_name);
	fprintf(cgiOut, "									<td width=\"1\"><input name=\"type\" type=\"text\" value=\"%d\" style=\"display:none\"/></td>\n", net_info.type);
	fprintf(cgiOut, "									<td width=\"1\"><input name=\"ip\" type=\"text\" value=\"%s\" class=\"home_edit_ip\" style=\"display:none\"/></td>\n", net_info.ip);
	fprintf(cgiOut, "									<td width=\"1\"><input name=\"gw\" type=\"text\" value=\"%s\" class=\"home_edit_gw\" style=\"display:none\"/></td>\n", net_info.gw);
	fprintf(cgiOut, "									<td width=\"1\"><input name=\"net_mask\" type=\"text\" value=\"%s\" class=\"home_edit_net_mask\" style=\"display:none\"/></td>\n", net_info.netmask);
//	fprintf(cgiOut, "									<td width=\"1\"><input name=\"dns\" type=\"text\" value=\"%s\" class=\"home_edit_dns\" style=\"display:none\"/></td>\n", net_info.dns);
	fprintf(cgiOut, "							  	</tr>\n");
	fprintf(cgiOut, "							 	<tr height=\"45\" valign=\"middle\">\n");
	fprintf(cgiOut, "									<td width=\"70\" align=\"left\"><span><b>子网掩码：</b></span></td>\n");
	fprintf(cgiOut, "									<td width=\"220\" align=\"left\"><span class=\"home_ip_show home_note_net_mask\">%s</span>\n", net_info.netmask);
	for (i_ip = 0; i_ip < TS_IP_UNIT_COUNT; i_ip++) {
		fprintf(cgiOut, "									<input size=\"2\" maxlength=\"3\" value=\"0\" type=\"text\" class=\"home_ip_edit home_net_mask_%d\" style=\"display:none\"/>\n", i_ip);
		if (i_ip != TS_IP_UNIT_COUNT - 1) {
			fprintf(cgiOut, "								<span class=\"home_ip_edit home_net_mask_point\" style=\"display:none\">.</span>\n");
		}
	}
	fprintf(cgiOut, "									</td>\n");
	//fprintf(cgiOut, "							  	</tr>\n");


//have dns
		if (TS_WEB_CAN_DNS == can_dns) {
			ts_web_get_dns(net_info.dns);
//			fprintf(cgiOut, "							 	<tr height=\"45\" valign=\"middle\">\n");
			fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>DNS：</b></span></td>\n");
			fprintf(cgiOut, "									<td width=\"220\" align=\"left\"><span class=\"home_ip_show home_note_dns\">%s</span>\n", net_info.dns);
			for (i_ip = 0; i_ip < TS_IP_UNIT_COUNT; i_ip++) {
				fprintf(cgiOut, "									<input size=\"2\" maxlength=\"3\" value=\"0\" type=\"text\" class=\"home_ip_edit home_dns_%d\" style=\"display:none\"/>\n", i_ip);
				if (i_ip != TS_IP_UNIT_COUNT - 1) {
					fprintf(cgiOut, "								<span class=\"home_ip_edit home_dns_point\" style=\"display:none\">.</span>\n");
				}
			}
			fprintf(cgiOut, "									</td>\n");
			fprintf(cgiOut, "									<td width=\"1\"><input name=\"dns\" type=\"text\" value=\"%s\" class=\"home_edit_dns\" style=\"display:none\"/></td>\n", net_info.dns);
//			fprintf(cgiOut, "							  	</tr>\n");
		}

		if (TS_WEB_NET_WIFI == net_type) {
	//		fprintf(cgiOut, "								  <tr height=\"45\" valign=\"middle\">\n");
			fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>SSID：</b></span></td>\n");
			fprintf(cgiOut, "									<td width=\"140\" align=\"left\"><span class=\"home_ip_show wireless_adapter_ssid\">%s</span><input type=\"text\" name=\"ssid\" class=\"home_ip_edit wireless_ssid\" style=\"display:none\"/></td>\n", net_info.ssid);
		}

		if (TS_WEB_NET_WIFI == net_type) {
			fprintf(cgiOut, "								  <tr height=\"45\" valign=\"middle\">\n");
	//		fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>SSID：</b></span></td>\n");
	//		fprintf(cgiOut, "									<td width=\"140\" align=\"left\"><span class=\"home_ip_show wireless_adapter_ssid\">%s</span><input type=\"text\" name=\"ssid\" class=\"home_ip_edit wireless_ssid\" style=\"display:none\"/></td>\n", net_info.ssid);
			fprintf(cgiOut, "									<td width=\"70\" align=\"left\"><span><b>密码：</b></span></td>\n");
			fprintf(cgiOut, "									<td width=\"140\" align=\"left\"><span class=\"home_ip_show wireless_adapter_password\">%s</span><input type=\"password\" name=\"login_passwd\" class=\"home_ip_edit wireless_password\" style=\"display:none\"/></td>\n", net_info.login_passwd);
//			fprintf(cgiOut, "								  </tr>\n");
		}
		fprintf(cgiOut, "								  </tr>\n");

		fprintf(cgiOut, "							 	<tr height=\"40\">\n");
		fprintf(cgiOut, "									<td width=\"460\" colspan=\"4\"><span><input type=\"button\" class=\"home_revise_btn\"/><input type=\"button\" class=\"home_save_btn\" style=\"display:none\" onclick=\"set_local_net('%s')\"/></span></td>\n", net_name);
		fprintf(cgiOut, "							  	</tr>\n");
		fprintf(cgiOut, "							</table>\n");
		fprintf(cgiOut, "							</form>\n");
		fprintf(cgiOut, "							</div>\n");
	}

#define SET_NET_INFOS do{\
	set_net("eth0", TS_WEB_NET_LINE, "",TS_WEB_CAN_DHCP,TS_WEB_CAN_DNS);\
	set_net("eth1", TS_WEB_NET_LINE, "",TS_WEB_CAN_NOT_DHCP,TS_WEB_CAN_NOT_DNS);\
	set_net("wlan0", TS_WEB_NET_WIFI, "_last",TS_WEB_CAN_NOT_DHCP,TS_WEB_CAN_NOT_DNS);\
}while(0);

	int32_t cgiMain()
	{
		//init database
		int32_t ret = EXIT_SUCCESS;

		log_open("basic_control.c");
		int32_t type = PAGE_TYPE_HOST_CONTROL;
		char pages[PAGE_COUNT][64] = { { 0 } };
//	int32_t to_type = PAGE_TYPE_NET_ETH0;
		char * data;
		data = getenv("QUERY_STRING");
		if (data == NULL) {

		} else {
			cgiFormInteger(TS_PAGE_TYPE, &type, PAGE_TYPE_HOST_CONTROL);
		}
//	set_pages(pages);
		const char *b_page = "basic_control.html?";
		int32_t i_page = 0;
		for (i_page = 0; i_page < PAGE_COUNT; i_page++) {
			bzero(pages[i_page], 64);
			sprintf(pages[i_page], "%s%s=%d", b_page, TS_PAGE_TYPE, i_page);
		}

//	ts_host_net_info net_info;

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
//	fprintf(cgiOut, "			<div class=\"center\" style='height:1050px'>\n");
		fprintf(cgiOut, "			<div class=\"center\">\n");

//	TS_WEB_MENUS

		fprintf(cgiOut, "				<!--导航 start-->\n");
		fprintf(cgiOut, "<script type=\"text/javascript\">print_left_guide(%d)</script>\n", TS_LEFT_GUIDE_BASIC_CTRL);
		fprintf(cgiOut, "				<!--导航 end-->\n");

		fprintf(cgiOut, "				<!--内容 start-->\n");
		fprintf(cgiOut, "				<div class=\"content\" >\n");
		fprintf(cgiOut, "					<!--标题 start-->\n");
		fprintf(cgiOut, "					<div class=\"title\">");
		fprintf(cgiOut, "<a href=\"%s\">主机控制</a>|", pages[PAGE_TYPE_HOST_CONTROL]);
		fprintf(cgiOut, "<a href=\"%s\">更新</a>|", pages[PAGE_TYPE_UPDATE]);
		fprintf(cgiOut, "<a href=\"%s\">配置文件导入</a>|", pages[PAGE_TYPE_CONF_IMP]);
		fprintf(cgiOut, "<a href=\"%s\">外网服务</a>|", pages[PAGE_TYPE_STOP_NET]);
		fprintf(cgiOut, "<a href=\"%s\">内外网切换</a>|", pages[PAGE_TYPE_IS_LOCAL]);
		fprintf(cgiOut, "<a href=\"%s\">网卡信息</a>|", pages[PAGE_TYPE_NET_INFO]);
		fprintf(cgiOut, "<a href=\"%s\">日志导出</a>|", pages[PAGE_TYPE_EXPORT_LOG]);
		fprintf(cgiOut, "<a href=\"%s\">用户信息</a>|", pages[PAGE_TYPE_USER_INFO]);
		fprintf(cgiOut, "<a href=\"%s\">SERVER信息</a>", pages[PAGE_TYPE_SERVER_INFO]);
		fprintf(cgiOut, "</div>\n");
		fprintf(cgiOut, "					<!--标题 end-->\n");
		fprintf(cgiOut, "					<!--内容 start-->\n");
		fprintf(cgiOut, "					<div class=\"basic_control\">\n");
//

		int32_t is_exits = TS_NO_RUNNING;
		ts_is_running(TS_MONITOR_NAME, &is_exits);
		if (PAGE_TYPE_HOST_CONTROL == type) {
			fprintf(cgiOut, "\n						<!--主机控制 start-->\n");
			fprintf(cgiOut, "						<div class=\"title\" id=\"basic_control\">程序控制开关</div>\n");
			fprintf(cgiOut, "						<div class=\"switch\">\n");
			fprintf(cgiOut, "							<table width=\"200\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
			fprintf(cgiOut, "  								<tr align=\"left\" valign=\"middle\" height=\"35\">\n");
			if (TS_RUNNING == is_exits) {
				fprintf(cgiOut, "    								<td><input name=\"control_switch\" type=\"radio\" value=\"\"  onclick=\"on_off_host('info=%d&ampon_off=%d&amp',1,2)\"/> 重启<span style=\"padding-left:15px\"></span><input name=\"control_switch\" type=\"radio\" value=\"\"  checked /> 开 <span style=\"padding-left:15px\"></span><input name=\"control_switch\" type=\"radio\" value=\"\"  onclick=\"on_off_host('info=%d&ampon_off=%d&amp',1,0)\"/> 关</td>\n", RESTART_HOST, FALSE, ON_OFF_HOST, FALSE);
			} else {
				fprintf(cgiOut, "    								<td><input name=\"control_switch\" type=\"radio\" value=\"\"  onclick=\"on_off_host('info=%d&ampon_off=%d&amp',1,2)\"/> 重启<span style=\"padding-left:15px\"></span><input name=\"control_switch\" type=\"radio\" value=\"\" onclick=\"on_off_host('info=%d&on_off=%d&',1,1)\" /> 开 <span style=\"padding-left:15px\"></span><input name=\"control_switch\" type=\"radio\" value=\"\"  onclick=\"on_off_host('info=%d&ampon_off=%d&amp',1,0)\"  checked /> 关</td>\n", RESTART_HOST, FALSE, ON_OFF_HOST, TRUE, ON_OFF_HOST, FALSE);
			}
			fprintf(cgiOut, "  								</tr>\n");
			fprintf(cgiOut, "							</table>\n");
			fprintf(cgiOut, "						</div>\n");
			fprintf(cgiOut, "						<!--主机控制 end-->\n");
		}

//
		if (PAGE_TYPE_UPDATE == type) {
			fprintf(cgiOut, "\n						<!--更新 start-->\n");
			fprintf(cgiOut, "						<div class=\"title\" id=\"configure_receive\">更新</div>\n");
			fprintf(cgiOut, "						<div class=\"operate\">\n");
			if (TS_RUNNING == is_exits) {
				fprintf(cgiOut, "						<div class=\"button\"><input type=\"button\" class=\"receive_btn\" /></div>\n");
			} else {
				fprintf(cgiOut, "						<div class=\"button\"><input type=\"button\" class=\"receive_btn_no_run\" /></div>\n");
			}
			fprintf(cgiOut, "						</div>\n");
			fprintf(cgiOut, "						<!--更新 end-->\n");
		}

//
		if (PAGE_TYPE_CONF_IMP == type) {
			fprintf(cgiOut, "\n						<!--配置文件导入 start-->\n");
			fprintf(cgiOut, "						<div class=\"title\" id=\"home_import\">配置文件导入</div>\n");
			fprintf(cgiOut, "						<div class=\"operate\">\n");
//		fprintf(cgiOut, "						<form enctype=\"multipart/form-data\" id=\"form1\">\n");
			fprintf(cgiOut, "						<form id=\"form1\">\n");
//		fprintf(cgiOut, " 							<input type=\"text\" id=\"txt\" name=\"txt\" value=\"panda\" style=\"display:none\">\n");
//		fprintf(cgiOut, " 							<input type=\"text\" id=\"txt_img_url\" name=\"txt_img_url\" style=\"display:none\">\n");
			fprintf(cgiOut, " 							<a href=\"javascript:void(0);\"><input type=\"file\" id=\"file_load\" name=\"fileload\" onchange=\"SingleUpload('txt_img_url','FileUpload')\" /></a>\n");
			fprintf(cgiOut, "						</form>\n");
//		fprintf(cgiOut, "						<form action=\"../cgi-bin/imp_xml.html\" method=\"post\" enctype=\"multipart/form-data\" target=\"_blank\">\n");
//		fprintf(cgiOut, "						   <input type=\"file\" name=\"FileUpload\" value=\"\" />\n");
//		fprintf(cgiOut, "						    <input type=\"submit\" name=\"submit\" value=\"OK\">\n");
//		fprintf(cgiOut, "						</form>\n");
			fprintf(cgiOut, "						<!--配置文件导入 end-->\n");
		}

//
		if (PAGE_TYPE_STOP_NET == type) {
			fprintf(cgiOut, "\n						<!--外网服务 start-->\n");
			fprintf(cgiOut, "						<div class=\"title\" id=\"stop_outer\">外网服务</div>\n");
			fprintf(cgiOut, "						<div class=\"switch\">\n");
			fprintf(cgiOut, "							<table width=\"200\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
			fprintf(cgiOut, "  								<tr align=\"left\" valign=\"middle\" height=\"35\">\n");
			int32_t is_stop = TS_STOP_NET;
			db_get_is_stop_net(&is_stop);
			if (TS_RUNNING == is_exits) {
				if (TS_STOP_NET == is_stop) {
					fprintf(cgiOut, "    								<td><input name=\"outer_switch\" type=\"radio\" value=\"\" onclick=\"stop_host('info=%d&on_off=%d&',%d)\"/> 开 <span style=\"padding-left:15px\"></span><input name=\"outer_switch\" type=\"radio\" value=\"\" onclick=\"stop_host('info=%d&on_off=%d&',%d)\"  checked  /> 关</td>\n", STOP_HOST_NETWORK, TRUE, TRUE, STOP_HOST_NETWORK, FALSE, FALSE);
				} else {
					fprintf(cgiOut, "    								<td><input name=\"outer_switch\" type=\"radio\" value=\"\"  checked  onclick=\"stop_host('info=%d&on_off=%d&',%d)\"/> 开 <span style=\"padding-left:15px\"></span><input name=\"outer_switch\" type=\"radio\" value=\"\" onclick=\"stop_host('info=%d&on_off=%d&',%d)\" /> 关</td>\n", STOP_HOST_NETWORK, TRUE, TRUE, STOP_HOST_NETWORK, FALSE, FALSE);
				}
			} else {
				fprintf(cgiOut, "    								<td><input readonly=\"readonly\" name=\"outer_switch\" type=\"radio\" value=\"\" onclick=\"can_use()\" /> 开 <span style=\"padding-left:15px\"></span><input  readonly=\"readonly\"  name=\"outer_switch\" type=\"radio\" value=\"\"  checked  onclick=\"can_use()\" /> 关</td>\n");
			}
			fprintf(cgiOut, "  								</tr>\n");
			fprintf(cgiOut, "							</table>\n");
			fprintf(cgiOut, "						</div>\n");
			fprintf(cgiOut, "						<!--外网服务 end-->\n");
		}

//is_local
		if (PAGE_TYPE_IS_LOCAL == type) {
			fprintf(cgiOut, "\n						<!--是否内网-->\n");
			fprintf(cgiOut, "						<div class=\"title\" id=\"is_local\">内外网切换</div>\n");
			fprintf(cgiOut, "						<div class=\"switch\">\n");
			fprintf(cgiOut, "							<table width=\"200\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
			fprintf(cgiOut, "  								<tr align=\"left\" valign=\"middle\" height=\"35\">\n");
			int32_t is_local = TS_IS_LOCAL;
			db_get_is_local(&is_local);
			if (TS_RUNNING == is_exits) {
				if (TS_IS_LOCAL != is_local) {
					fprintf(cgiOut, "    								<td><input name=\"is_local\" type=\"radio\" value=\"\" onclick=\"set_host_local('info=%d&on_off=%d&',%d)\"/> 内网 <span style=\"padding-left:15px\"></span><input name=\"is_local\" type=\"radio\" value=\"\" onclick=\"set_host_local('info=%d&on_off=%d&',%d)\"  checked  /> 外网</td>\n", SET_LOACL, TS_IS_LOCAL, TRUE, SET_LOACL, TS_IS_REMOTE, FALSE);
				} else {
					fprintf(cgiOut, "    								<td><input name=\"is_local\" type=\"radio\" value=\"\"  checked  onclick=\"set_host_local('info=%d&on_off=%d&',%d)\"/> 内网 <span style=\"padding-left:15px\"></span><input name=\"is_local\" type=\"radio\" value=\"\" onclick=\"set_host_local('info=%d&on_off=%d&',%d)\" /> 外网</td>\n", SET_LOACL, TS_IS_LOCAL, TRUE, SET_LOACL, TS_IS_REMOTE, FALSE);
				}
			} else {
				fprintf(cgiOut, "    								<td><input readonly=\"readonly\" name=\"is_local\" type=\"radio\" value=\"\"  onclick=\"can_use()\" /> 内网 <span style=\"padding-left:15px\"></span><input  readonly=\"readonly\"  name=\"is_local\" type=\"radio\" value=\"\"  checked  onclick=\"can_use()\"  /> 外网</td>\n");
			}
			fprintf(cgiOut, "  								</tr>\n");
			fprintf(cgiOut, "							</table>\n");
			fprintf(cgiOut, "						</div>\n");
			fprintf(cgiOut, "						<!--是否内网 end-->\n");
		}

		//IP
		if (PAGE_TYPE_NET_INFO == type) {
			fprintf(cgiOut, "						<!--网卡信息 start-->\n");
			fprintf(cgiOut, "						<div class=\"title\" id=\"network_info\">网卡信息</div>\n");
			fprintf(cgiOut, "						<div class=\"newwork_info\">\n");
			SET_NET_INFOS
			fprintf(cgiOut, "						</div>						\n");
			fprintf(cgiOut, "						<!--网卡信息 end-->\n");
		}

		//log
		if (PAGE_TYPE_EXPORT_LOG == type) {
			fprintf(cgiOut, "\n						<!--log start-->\n");
			fprintf(cgiOut, "						<div class=\"title\" id=\"export_log\">日志导出</div>\n");
			fprintf(cgiOut, "						<div class=\"operate\">\n");
			fprintf(cgiOut, "						<div class=\"button\"><input type=\"button\" class=\"receive_log_btn\" /></div>\n");
			fprintf(cgiOut, "						</div>\n");
			fprintf(cgiOut, "						<!--log end-->\n");
		}

		if (PAGE_TYPE_USER_INFO == type) {
			fprintf(cgiOut, "						<!--用户信息 start-->\n");
			fprintf(cgiOut, "						<div class=\"title\" id=\"user_info\">用户信息</div>\n");
			fprintf(cgiOut, "						<div class=\"net_number\">\n");
			fprintf(cgiOut, "							<form method=\"post\" id=\"set_user_info\">\n");
			fprintf(cgiOut, "							<table width=\"420\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
			fprintf(cgiOut, "							  <tr height=\"40\" valign=\"middle\">\n");
			fprintf(cgiOut, "								<td width=\"55\" align=\"right\"><span><b>用户名：</b></span></td>\n");
			ts_host_base_user_info host_info;
			db_get_base_user_info(&host_info);
			fprintf(cgiOut, "								<td width=\"135\"><span class=\"user_info_name\">%s</span></td>\n", host_info.user_name);
			fprintf(cgiOut, "								<td width=\"100\" align=\"right\"><span><b>原密码：</b></span></td>\n");
			fprintf(cgiOut, "								<td width=\"130\"><input name=\"old_passwd\" value=\"\" type=\"password\" class=\"user_info_input user_old_password\"/></td>\n");
			fprintf(cgiOut, "							  </tr>\n");
			fprintf(cgiOut, "							  <tr height=\"40\" valign=\"middle\">\n");
			fprintf(cgiOut, "								<td width=\"55\" align=\"right\"><span><b>新密码：</b></span></td>\n");
			fprintf(cgiOut, "								<td width=\"135\"><input type=\"password\" name=\"new_passwd\"  class=\"user_info_input user_new_password\"/></td>\n");
			fprintf(cgiOut, "								<td width=\"100\"  align=\"right\"><span><b>再次输入密码：</b></span></td>\n");
			fprintf(cgiOut, "								<td width=\"130\"><input name=\"reply_passwd\" type=\"password\" class=\"user_info_input user_new_password_second\"/></td>\n");
			fprintf(cgiOut, "							  </tr>\n");
			fprintf(cgiOut, "							  <tr height=\"40\">\n");
			fprintf(cgiOut, "								<td width=\"420\" colspan=\"4\"><span><input type=\"button\" class=\"user_info_ok_btn\"  onclick=\"set_user_info()\"/></span></td>\n");
			fprintf(cgiOut, "							  </tr>\n");
			fprintf(cgiOut, "							</table>\n");
			fprintf(cgiOut, "							</form>\n");
			fprintf(cgiOut, "						</div>						\n");
			fprintf(cgiOut, "						<!--用户信息 end-->						\n");
		}

		//server info
		if (PAGE_TYPE_SERVER_INFO == type) {
			fprintf(cgiOut, "				<!--SEVER信息 start-->\n");
			ts_host_base_user_info base_info;
			db_get_base_user_info(&base_info);
			fprintf(cgiOut, "				<div class=\"title\" id=\"server_info\">服务器信息</div>\n");
			fprintf(cgiOut, "				<div class=\"net_number\">\n");
			fprintf(cgiOut, "				<table width=\"420\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
			fprintf(cgiOut, "					<tr height=\"40\" valign=\"middle\">\n");
			fprintf(cgiOut, "						<td width=\"80\" align=\"left\"><span><b>用户名：</b></span></td>\n");
			fprintf(cgiOut, "						<td width=\"160\"><input type=\"text\" name=\"login_username\" class=\"login_user_input login_username\" value=\"%s\" /></td>\n", base_info.login_user_name);
			fprintf(cgiOut, "						<td width=\"100\" align=\"left\"><span><b>密码：</b></span></td>\n");
			fprintf(cgiOut, "						<td width=\"160\"><input type=\"password\" name=\"login_userpassword\" class=\"login_user_input login_userpassword\" value=\"%s\" /></td>\n", base_info.login_passwd);
			fprintf(cgiOut, "					</tr>\n");
			fprintf(cgiOut, "					<tr height=\"40\" valign=\"middle\">\n");
			fprintf(cgiOut, "						<td width=\"80\" align=\"left\"><span><b>服务器IP：</b></span></td>\n");
			fprintf(cgiOut, "						<td width=\"160\"><input type=\"text\" name=\"login_server_ip\" class=\"login_user_input login_server_ip\"  value=\"%s\" /></td>\n", base_info.server_name);
			fprintf(cgiOut, "						<td width=\"100\" align=\"left\"><span><b>服务器端口：</b></span></td>\n");
			fprintf(cgiOut, "						<td width=\"160\"><input type=\"text\" name=\"login_server_port\" class=\"login_user_input login_server_port\" value=\"%d\" /></td>\n", base_info.server_port);
			fprintf(cgiOut, "					</tr>\n");
			fprintf(cgiOut, "					<tr height=\"40\">\n");
			fprintf(cgiOut, "						<td width=\"420\" colspan=\"4\"><span><input type=\"button\" class=\"login_submit_btn\"/></span></td>\n");
			fprintf(cgiOut, "					</tr>\n");
			fprintf(cgiOut, "				</table>\n");
			fprintf(cgiOut, "				</div>\n");
			fprintf(cgiOut, "				<!--SEVER信息 end-->\n");
		}

		fprintf(cgiOut, "					</div>\n");
		fprintf(cgiOut, "					<!--内容 end-->\n");
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

