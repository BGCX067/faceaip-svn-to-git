#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include "db/ts_db_tools.h"
#include "db/ts_db_web_infos.h"

#include "log/ts_log_tools.h"

//#define PAGE_TYPE "type"
#define PAGE_TYPE_SERVER 0
#define PAGE_TYPE_NET_ETH0 1
#define PAGE_TYPE_NET_ETH1 2
#define PAGE_TYPE_NET_WLAN 3

void set_net_guide(char * net_name, int32_t net_type, int32_t can_dhcp,int32_t can_dns)
{
	char brd_addr[32] = { 0 };
	char mac[32] = { 0 };
	ts_host_net_info net_info;
	bzero(&net_info,sizeof(net_info));
	bzero(brd_addr, sizeof(brd_addr));
	bzero(mac, sizeof(mac));
	strcpy(net_info.name, net_name);
	db_web_get_host_net(net_name, &net_info);
	ts_net_get_gateway(net_info.gw, net_name);
	ts_net_get_net_infos(net_info.ip, net_info.netmask, brd_addr, mac, net_name);

	fprintf(cgiOut, "							<div class=\"second_title\">%s 网卡配置</div>\n", net_name);
	fprintf(cgiOut, "							<form method=\"post\" id=\"set_ip_prot_%s\">\n", net_name);
	fprintf(cgiOut, "							<table width=\"600\" align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");

	if (TS_WEB_CAN_DHCP == can_dhcp) {
		fprintf(cgiOut, "								<tr height=\"45\" valign=\"middle\">\n");
		fprintf(cgiOut, "									<td width=\"70\" align=\"left\" ><span><b>自动获取：</b></span></td>\n");
		if (TS_WEB_DHCP == net_info.is_dhcp) {
			fprintf(cgiOut, "    								<td  width=\"250\" align=\"left\"><input name=\"is_dhcp\" type=\"radio\" checked value=\"%d\" onclick=\"dhcp_yes(this)\" /> 是 <span style=\"padding-left:15px \"></span><input name=\"is_dhcp\" type=\"radio\" value=\"%d\" onclick=\"dhcp_no(this)\"/>否</td>\n", TS_WEB_DHCP, TS_WEB_NOT_DHCP);
			fprintf(cgiOut, "									<script type=\"text/javascript\" >$(function(){dhcp_yes($(\"input[name='is_dhcp']\"));\n");
			fprintf(cgiOut, "									});</script>\n");
		} else {
			fprintf(cgiOut, "    								<td><input name=\"is_dhcp\" type=\"radio\"  value=\"%d\" onclick=\"dhcp_yes(this)\" /> 是 <span style=\"padding-left:15px\"></span><input name=\"is_dhcp\" type=\"radio\" value=\"%d\" checked onclick=\"dhcp_no(this)\" />否</td>\n", TS_WEB_DHCP, TS_WEB_NOT_DHCP);
			fprintf(cgiOut, "									<script type=\"text/javascript\" >$(function(){dhcp_no($(\"input[name='is_dhcp']\"));\n");
			fprintf(cgiOut, "									});</script>\n");
		}
		fprintf(cgiOut, "    								<td><input class=\"is_dhcp_value\" name=\"is_dhcp_value\" style=\"display:none\" type=\"text\" value=\"%d\"/></td>\n", net_info.is_dhcp);
		fprintf(cgiOut, "							  	</tr>\n");
	} else {
		net_info.is_dhcp = TS_WEB_NOT_DHCP;
		fprintf(cgiOut, "    								<td><input class=\"is_dhcp_value\" name=\"is_dhcp_value\" style=\"display:none\" type=\"text\" value=\"%d\"/></td>\n", TS_WEB_NOT_DHCP);
	}

	//set ip
	fprintf(cgiOut, "								<tr height=\"45\" valign=\"middle\">\n");
	fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>IP地址：</b></span></td>\n");
	int32_t i_ip = 0;
	fprintf(cgiOut, "									<td width=\"250\" align=\"left\">\n");
	fprintf(cgiOut, "										<script type=\"text/javascript\" >var ips ;\n");
	fprintf(cgiOut, "										ips = parse_ip('%s');</script>\n", net_info.ip);
	for (i_ip = 0; i_ip < TS_IP_UNIT_COUNT; i_ip++) {
		fprintf(cgiOut, "									<input name=\"ip_%d\" size=\"3\" maxlength=\"3\" value=\"0\" type=\"text\"/>\n", i_ip);
		fprintf(cgiOut, "									<script type=\"text/javascript\" >$(\"input[name='ip_%d']\").val(ips[%d]) </script>\n", i_ip, i_ip);
		if (i_ip != TS_IP_UNIT_COUNT - 1) {
			fprintf(cgiOut, "								<span class=\"home_ip_edit home_ip_point\">.</span>\n");
		}
	}
	fprintf(cgiOut, "									</td>\n");
	fprintf(cgiOut, "							  	</tr>\n");

	//set gw
	fprintf(cgiOut, "							 	<tr height=\"45\" valign=\"middle\">\n");
	fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>网关：</b></span></td>\n");
	fprintf(cgiOut, "									<td width=\"250\" align=\"left\">\n");
	fprintf(cgiOut, "										<script type=\"text/javascript\" >var ips ;\n");
	fprintf(cgiOut, "										ips = parse_ip('%s');</script>\n", net_info.gw);
	for (i_ip = 0; i_ip < TS_IP_UNIT_COUNT; i_ip++) {
		fprintf(cgiOut, "									<input size=\"3\" name=\"gw_%d\" maxlength=\"3\" value=\"0\" type=\"text\">\n", i_ip);
		fprintf(cgiOut, "									<script type=\"text/javascript\" >$(\"input[name='gw_%d']\").val(ips[%d]) </script>\n", i_ip, i_ip);
		if (i_ip != TS_IP_UNIT_COUNT - 1) {
			fprintf(cgiOut, "								<span class=\"home_ip_edit home_gw_point\" >.</span>\n");
		}
	}
	fprintf(cgiOut, "									</td>\n");
	fprintf(cgiOut, "							  	</tr>\n");

	//net mask
	fprintf(cgiOut, "							 	<tr height=\"45\" valign=\"middle\">\n");
	fprintf(cgiOut, "									<td width=\"70\" align=\"left\"><span><b>子网掩码：</b></span></td>\n");
	fprintf(cgiOut, "									<td width=\"250\" align=\"left\">\n");
	fprintf(cgiOut, "										<script type=\"text/javascript\" >var ips ;\n");
	fprintf(cgiOut, "										ips = parse_ip('%s');</script>\n", net_info.netmask);
	for (i_ip = 0; i_ip < TS_IP_UNIT_COUNT; i_ip++) {
		fprintf(cgiOut, "									<input name=\"net_mask_%d\" size=\"3\" maxlength=\"3\" value=\"0\" type=\"text\" />\n", i_ip);
		fprintf(cgiOut, "									<script type=\"text/javascript\" >$(\"input[name='net_mask_%d']\").val(ips[%d]) </script>\n", i_ip, i_ip);
		if (i_ip != TS_IP_UNIT_COUNT - 1) {
			fprintf(cgiOut, "								<span class=\"home_ip_edit home_net_mask_point\">.</span>\n");
		}
	}
	fprintf(cgiOut, "									</td>\n");
	fprintf(cgiOut, "							  	</tr>\n");

	//wifi
	if (TS_WEB_NET_WIFI == net_type) {
		fprintf(cgiOut, "								<tr height=\"45\" valign=\"middle\">\n");
		fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>SSID：</b></span></td>\n");
		fprintf(cgiOut, "									<td width=\"250\" align=\"left\"><input type=\"text\" name=\"ssid\" value=\"%s\" class=\"home_ip_edit wireless_ssid\"/></td>\n", net_info.ssid);
		fprintf(cgiOut, "							  	</tr>\n");
		fprintf(cgiOut, "							 	<tr height=\"45\" valign=\"middle\">\n");
		fprintf(cgiOut, "									<td width=\"70\" align=\"left\"><span><b>密码：</b></span></td>\n");
		fprintf(cgiOut, "									<td width=\"250\" align=\"left\"><input type=\"password\" name=\"login_passwd\" value=\"%s\" class=\"home_ip_edit wireless_password\"/></td>\n", net_info.login_passwd);
		fprintf(cgiOut, "								  </tr>\n");
	}

	//have dns
	if (TS_WEB_CAN_DNS == can_dns) {
		ts_web_get_dns(net_info.dns);
		fprintf(cgiOut, "							 	<tr height=\"45\" valign=\"middle\">\n");
		fprintf(cgiOut, "									<td width=\"70\" align=\"left\"><span><b>DNS：</b></span></td>\n");
		fprintf(cgiOut, "									<td width=\"250\" align=\"left\">\n");
		fprintf(cgiOut, "										<script type=\"text/javascript\" >var ips ;\n");
		fprintf(cgiOut, "										ips = parse_ip('%s');</script>\n", net_info.dns);
		for (i_ip = 0; i_ip < TS_IP_UNIT_COUNT; i_ip++) {
			fprintf(cgiOut, "									<input name=\"dns_%d\" size=\"3\" maxlength=\"3\" value=\"0\" type=\"text\" />\n", i_ip);
			fprintf(cgiOut, "									<script type=\"text/javascript\" >$(\"input[name='dns_%d']\").val(ips[%d]) </script>\n", i_ip, i_ip);
			if (i_ip != TS_IP_UNIT_COUNT - 1) {
				fprintf(cgiOut, "								<span class=\"home_ip_edit home_dns_point\">.</span>\n");
			}
		}
		fprintf(cgiOut, "									</td>\n");
		fprintf(cgiOut, "							  	</tr>\n");
	}

	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "							</form>\n");
}

void get_next(int32_t curr_page, char *next_page)
{
	bzero(next_page, sizeof(next_page));
	switch (curr_page) {
		case PAGE_TYPE_SERVER:
			sprintf(next_page, "base_info.html?%s=%d", TS_PAGE_TYPE, PAGE_TYPE_NET_ETH0);
			break;
		case PAGE_TYPE_NET_ETH0:
			sprintf(next_page, "base_info.html?%s=%d", TS_PAGE_TYPE, PAGE_TYPE_NET_ETH1);
			break;
		case PAGE_TYPE_NET_ETH1:
			sprintf(next_page, "base_info.html?%s=%d", TS_PAGE_TYPE, PAGE_TYPE_NET_WLAN);
			break;
		case PAGE_TYPE_NET_WLAN:
			sprintf(next_page, "%s", "guide_info.html");
			break;
		default:
			break;
	}
}

int cgiMain()
{
	int32_t ret = EXIT_SUCCESS;
	log_open("base_info.c");

	char * data;
	int32_t type = PAGE_TYPE_SERVER;
	char next_page[64] = { 0 };
//	int32_t to_type = PAGE_TYPE_NET_ETH0;

	data = getenv("QUERY_STRING");
	if (data == NULL) {

	} else {
		cgiFormInteger(TS_PAGE_TYPE, &type, 0);
	}

	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-Transitional.dtd\">\n");
	fprintf(cgiOut, "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");

	//login valiate failed
	TS_LOGIN_FAILED

	get_next(type, next_page);

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
	fprintf(cgiOut, "			<div class=\"center\" style='height:900px'>\n");
//	TS_WEB_MENUS
	fprintf(cgiOut, "				<!--导航 start-->\n");
	fprintf(cgiOut, "<script type=\"text/javascript\">print_left_guide(%d)</script>\n", TS_LEFT_GUIDE_GUIDE);
	fprintf(cgiOut, "				<!--导航 end-->\n");

	fprintf(cgiOut, "				<!--内容 start-->\n");
	fprintf(cgiOut, "				<div class=\"content\" >\n");
	fprintf(cgiOut, "				<div class=\"basic_control\">\n");

	//server info
	if (PAGE_TYPE_SERVER == type) {
		ts_host_base_user_info base_info;
		db_get_base_user_info(&base_info);

		fprintf(cgiOut, "				<!--SEVER信息 start-->\n");
		fprintf(cgiOut, "				<div class=\"title\" id=\"server_info\">服务器信息</div>\n");
		fprintf(cgiOut, "				<div class=\"net_number\">\n");
		fprintf(cgiOut, "				<table width=\"420\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
		fprintf(cgiOut, "					<tr height=\"40\" valign=\"middle\">\n");
		fprintf(cgiOut, "						<td width=\"80\" align=\"left\"><span><b>用户名：</b></span></td>\n");
		fprintf(cgiOut, "						<td width=\"120\"><input type=\"text\" name=\"login_username\" class=\"login_user_input login_username\" value=\"%s\" /></td>\n", base_info.login_user_name);
		fprintf(cgiOut, "						<td width=\"100\" align=\"left\"><span><b>密码：</b></span></td>\n");
		fprintf(cgiOut, "						<td width=\"120\"><input type=\"password\" name=\"login_userpassword\" class=\"login_user_input login_userpassword\" value=\"%s\" /></td>\n", base_info.login_passwd);
		fprintf(cgiOut, "					</tr>\n");
		fprintf(cgiOut, "					<tr height=\"40\" valign=\"middle\">\n");
		fprintf(cgiOut, "						<td width=\"80\" align=\"left\"><span><b>服务器IP：</b></span></td>\n");
		fprintf(cgiOut, "						<td width=\"120\"><input type=\"text\" name=\"login_server_ip\" class=\"login_user_input login_server_ip\"  value=\"%s\" /></td>\n", base_info.server_name);
		fprintf(cgiOut, "						<td width=\"100\" align=\"left\"><span><b>服务器端口：</b></span></td>\n");
		fprintf(cgiOut, "						<td width=\"120\"><input type=\"text\" name=\"login_server_port\" class=\"login_user_input login_server_port\" value=\"%d\" /></td>\n", base_info.server_port);
		fprintf(cgiOut, "					</tr>\n");
		fprintf(cgiOut, "				</table>\n");
		fprintf(cgiOut, "				</div>\n");
		fprintf(cgiOut, "				<!--SEVER信息 end-->\n");
		fprintf(cgiOut, "\n");
	} else if ((PAGE_TYPE_NET_ETH0 == type) || (PAGE_TYPE_NET_ETH1 == type) || (PAGE_TYPE_NET_WLAN == type)) {
		//IP
		fprintf(cgiOut, "						<!--网络信息 start-->\n");
		fprintf(cgiOut, "						<div class=\"title\" id=\"network_info\">网络信息</div>\n");
		fprintf(cgiOut, "						<div class=\"newwork_info\">\n");
		switch (type) {
			case PAGE_TYPE_NET_ETH0:
				set_net_guide("eth0", TS_WEB_NET_LINE, TS_WEB_CAN_DHCP,TS_WEB_CAN_DNS);
				break;
			case PAGE_TYPE_NET_ETH1:
				set_net_guide("eth1", TS_WEB_NET_LINE, TS_WEB_CAN_NOT_DHCP,TS_WEB_CAN_NOT_DNS);
				break;
			case PAGE_TYPE_NET_WLAN:
				set_net_guide("wlan0", TS_WEB_NET_WIFI, TS_WEB_CAN_NOT_DHCP,TS_WEB_CAN_NOT_DNS);
				break;
		}
		fprintf(cgiOut, "						</div>						\n");
		fprintf(cgiOut, "						<!--网络信息 end-->\n");
	}

	fprintf(cgiOut, "					<div class=\"button\">\n");
	if (PAGE_TYPE_SERVER != type) {
		fprintf(cgiOut, "						<input id=\"pre_btn\" type=\"button\" class=\"pre_btn\" onclick=\"back_page()\"/>\n");
	}
	fprintf(cgiOut, "						<input id=\"next_btn\" type=\"button\" class=\"next_btn\" onclick=\"guide_control(%d,'%s')\"/>\n", type, next_page);
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
