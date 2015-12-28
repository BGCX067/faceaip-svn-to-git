#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include "db/ts_db_tools.h"
#include "db/ts_db_web_infos.h"

#include "log/ts_log_tools.h"

void set_net(char * net_name, int32_t net_type, char *is_last, int32_t can_dns, int32_t is_dhcp)
{
	char brd_addr[32] = { 0 };
	char mac[32] = { 0 };
	ts_host_net_info net_info;
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
	fprintf(cgiOut, "								<tr height=\"45\" valign=\"middle\">\n");
	fprintf(cgiOut, "									<td width=\"70\" align=\"left\"><span><b>自动获取：</b></span></td>\n");
	if ((TS_WEB_CAN_DHCP == is_dhcp) && (TS_WEB_DHCP == net_info.is_dhcp)) {
		fprintf(cgiOut, "    								<td> 是 </td>\n");
	} else {
		fprintf(cgiOut, "    								<td> 否 </td>\n");
	}
	fprintf(cgiOut, "							  	</tr>\n");
	fprintf(cgiOut, "								<tr height=\"45\" valign=\"middle\">\n");
	fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>IP地址：</b></span></td>\n");
	fprintf(cgiOut, "									<td width=\"220\" align=\"left\"><span class=\"home_ip_show home_note_ip\">%s</span>\n", net_info.ip);
	fprintf(cgiOut, "									</td>\n");
//	fprintf(cgiOut, "							  	</tr>\n");
//	fprintf(cgiOut, "							 	<tr height=\"45\" valign=\"middle\">\n");
	fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>网关：</b></span></td>\n");
	fprintf(cgiOut, "									<td width=\"220\" align=\"left\"><span class=\"home_ip_show home_note_gw\">%s</span>\n", net_info.gw);
	fprintf(cgiOut, "									</td>\n");
	fprintf(cgiOut, "									<td width=\"1\"><input name=\"net_name\" type=\"text\" style=\"display:none\" value=\"%s\"/></td>\n", net_name);
	fprintf(cgiOut, "									<td width=\"1\"><input name=\"type\" type=\"text\" value=\"%d\" style=\"display:none\"/></td>\n", net_info.type);
	fprintf(cgiOut, "									<td width=\"1\"><input name=\"ip\" type=\"text\" value=\"%s\" class=\"home_edit_ip\" style=\"display:none\"/></td>\n", net_info.ip);
	fprintf(cgiOut, "									<td width=\"1\"><input name=\"gw\" type=\"text\" value=\"%s\" class=\"home_edit_gw\" style=\"display:none\"/></td>\n", net_info.gw);
	fprintf(cgiOut, "									<td width=\"1\"><input name=\"net_mask\" type=\"text\" value=\"%s\" class=\"home_edit_net_mask\" style=\"display:none\"/></td>\n", net_info.netmask);
	fprintf(cgiOut, "							  	</tr>\n");
	fprintf(cgiOut, "							 	<tr height=\"45\" valign=\"middle\">\n");
	fprintf(cgiOut, "									<td width=\"70\" align=\"left\"><span><b>子网掩码：</b></span></td>\n");
	fprintf(cgiOut, "									<td width=\"220\" align=\"left\"><span class=\"home_ip_show home_note_net_mask\">%s</span>\n", net_info.netmask);
	fprintf(cgiOut, "									</td>\n");

	//have dns
	if (TS_WEB_CAN_DNS == can_dns) {
		ts_web_get_dns(net_info.dns);
//			fprintf(cgiOut, "							 	<tr height=\"45\" valign=\"middle\">\n");
		fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>DNS：</b></span></td>\n");
		fprintf(cgiOut, "									<td width=\"220\" align=\"left\"><span class=\"home_ip_show home_note_dns\">%s</span>\n", net_info.dns);
		fprintf(cgiOut, "									</td>\n");
		fprintf(cgiOut, "									<td width=\"1\"><input name=\"dns\" type=\"text\" value=\"%s\" class=\"home_edit_dns\" style=\"display:none\"/></td>\n", net_info.dns);
		fprintf(cgiOut, "							  	</tr>\n");
	}

//			fprintf(cgiOut, "							  	</tr>\n");
	if (TS_WEB_NET_WIFI == net_type) {
//		fprintf(cgiOut, "								  <tr height=\"45\" valign=\"middle\">\n");
		fprintf(cgiOut, "									<td width=\"50\" align=\"left\"><span><b>SSID：</b></span></td>\n");
		fprintf(cgiOut, "									<td width=\"140\" align=\"left\"><span class=\"home_ip_show wireless_adapter_ssid\">%s</span><input type=\"text\" name=\"ssid\" class=\"home_ip_edit wireless_ssid\" style=\"display:none\"/></td>\n", net_info.ssid);
		fprintf(cgiOut, "							  	</tr>\n");
		fprintf(cgiOut, "								<tr height=\"45\" valign=\"middle\">\n");
		char login_passwd[64] = { 0 };
		memset(login_passwd, '*', strlen(net_info.login_passwd));
		fprintf(cgiOut, "									<td width=\"70\" align=\"left\"><span><b>密码：</b></span></td>\n");
		fprintf(cgiOut, "									<td width=\"140\" align=\"left\"><span class=\"home_ip_show wireless_adapter_password\">%s</span><input type=\"text\" name=\"login_passwd\" class=\"home_ip_edit wireless_password\" style=\"display:none\"/></td>\n", login_passwd);
		fprintf(cgiOut, "								  </tr>\n");
	}

	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "							</form>\n");
	fprintf(cgiOut, "							</div>\n");
}

#define SET_NET_INFOS do{\
	set_net("eth0", TS_WEB_NET_LINE, "",TS_WEB_CAN_DNS,TS_WEB_CAN_DHCP);\
	set_net("eth1", TS_WEB_NET_LINE, "",TS_WEB_CAN_NOT_DNS,TS_WEB_CAN_NOT_DHCP);\
	set_net("wlan0", TS_WEB_NET_WIFI, "_last",TS_WEB_CAN_NOT_DNS,TS_WEB_CAN_NOT_DHCP);\
}while(0);

int32_t is_exits = TS_NO_RUNNING;
char * all_gateway_ids = NULL;
char * comm_ports = NULL;

int32_t dev_counts = 0;
int32_t all_on_off = TS_WEB_ON;

int32_t get_infrared_device()
{
	int32_t ret = EXIT_SUCCESS;
//get interface info
	LIST_HEAD(list_head);
	ts_infrared_device_list * infr_infos = NULL;
	int32_t counts = 0;
	ret = db_web_get_infr_device_infos(&list_head, infr_infos, &counts);
	dev_counts = counts;

	if (ret != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	if (counts <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

//	const char * base_info = "{'id':'%d','infrared_device_name':'%s','infrared_device_pathway_no':'%s','infrared_device_pathway_select':['1','2','3','4','5','6','7','8','9','10','11','12'],'on_off':'%d'},";
	const char * base_info = "{'id':'%d','infrared_device_name':'%s','infrared_device_pathway_no':'%s','infrared_device_pathway_select':['1','2','3','4','5','6','7','8','9','10','11','12']},";
	int len = (strlen(base_info) + TS_DB_INT_ADD * 2 + sizeof(ts_host_interface_serial_info_list));
	char * infos = (char *) calloc(len * counts + 1, sizeof(char));
	char * bak_infos = infos;
	char * s_tmp = (char *) calloc(len, sizeof(char));
	int i = 0;

//Traversal list
	ts_infrared_device_list * p_tmp = NULL;
	list_for_each_entry_safe(infr_infos, p_tmp, &list_head, list) {
		memset(s_tmp, 0, len);
		char str[3] = { 0 };
		if (TS_WEB_NULL == infr_infos->infr_device_info.channel) {
			sprintf(str, "%s", "");
		} else {
			sprintf(str, "%d", infr_infos->infr_device_info.channel);

		}
		sprintf(s_tmp, base_info, infr_infos->infr_device_info.id, infr_infos->infr_device_info.device_name, str);
		memcpy(infos, s_tmp, strlen(s_tmp));
		if (i < counts - 1) {
			infos += strlen(s_tmp);
		}

		list_del(&infr_infos->list);
		ts_free(infr_infos);
	}

// get on off
	all_on_off = TS_WEB_OFF;
	web_action_report_t act_ctrl;
//	fun_is_ifrared_open(1, &all_on_off);
	if (TS_RUNNING == is_exits) {
		act_ctrl.data = all_on_off;
		act_ctrl.start_flag = BUFF_START;
		act_ctrl.source = HOST_WEB_PROGRAM;
		act_ctrl.obj = 0; //infrate is only one on_off
		act_ctrl.fun_code = GET_ON_OFF_INFR;
		ts_send_data(&act_ctrl);
		all_on_off = act_ctrl.data;
	} else {
		all_on_off = TS_WEB_OFF;
	}

//del last common,for ie6
	int32_t infos_len = strlen(bak_infos);
	*(bak_infos + infos_len - 1) = '\0';

	fprintf(cgiOut, "	parameter=[%s]\n", bak_infos);
	ts_free(bak_infos);
	ts_free(s_tmp);
//	fprintf(cgiOut, "	var parameter=[{'id':'1','infrared_device_name':'快进','infrared_device_pathway_no':'1','infrared_device_pathway_select':['1','2','3','4','5']},{'id':'2','infrared_device_name':'快进','infrared_device_pathway_no':'1','infrared_device_pathway_select':['1','2','3','4','5']}];\n");
	return ret;
}

int32_t get_all_serial()
{
	int32_t ret = EXIT_SUCCESS;

//get interface info
	LIST_HEAD(list_head);
	ts_host_interface_serial_info_list * serial_infos = NULL;
	int32_t counts = 0;
	ret = db_get_all_serial_details(&list_head, serial_infos, &counts);
	if (ret != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	if (counts <= 0) {
		ret = ERR_DB_NO_RECORDS;
		fprintf(cgiOut, "	parameter=[]\n");
		return ret;
	}

//Traversal list
	ts_host_interface_serial_info_list * p_tmp = NULL;

	const char * base_info = "{'id':'%d','description':'%d','baud_rate':'%d','data_bit':'%d',"
		"'stop_bit':'%2.2f','verify':'%d','serial_name':'%s','serial_port_switch':'%d'},";
	int32_t len = (strlen(base_info) + TS_DB_INT_ADD * 7 + strlen(TS_WEB_STRING_ON) + sizeof(ts_host_interface_serial_info_list));
	char * infos = (char *) calloc(len * counts, sizeof(char));
	char * bak_infos = infos;
	char * s_tmp = (char *) calloc(len, sizeof(char));
	int32_t i = 0;

	web_action_report_t act_ctrl;
	list_for_each_entry_safe(serial_infos,p_tmp,&list_head,list) {
		int32_t on_off = TS_WEB_OFF;
		memset(s_tmp, 0, len);

		if (TS_RUNNING == is_exits) {
			act_ctrl.data = on_off;
			act_ctrl.start_flag = BUFF_START;
			act_ctrl.source = HOST_WEB_PROGRAM;
			act_ctrl.obj = serial_infos->host_serial.id;
			act_ctrl.fun_code = GET_ON_OFF_SERIAL;
			ts_send_data(&act_ctrl);
			on_off = act_ctrl.data;
		} else {
			on_off = TS_WEB_OFF;
		}

//get on off
//		protocol_is_serial_open(serial_infos->host_serial.id, &on_off);
		sprintf(s_tmp, base_info, serial_infos->host_serial.id, serial_infos->host_serial.interface_id, serial_infos->host_serial.baud_rate, serial_infos->host_serial.data_bit, serial_infos->host_serial.stop_bit, serial_infos->host_serial.parity_check, serial_infos->host_serial.comm_port, on_off);

		memcpy(infos, s_tmp, strlen(s_tmp));
		if (i < counts - 1) {
			infos += strlen(s_tmp);
		}
		list_del(&serial_infos->list);
		free(serial_infos);
	}

//del last common,for ie6
	int32_t infos_len = strlen(bak_infos);
	*(bak_infos + infos_len - 1) = '\0';

	fprintf(cgiOut, "	var parameter=[%s]\n", bak_infos);
	free(bak_infos);
	free(s_tmp);

//	printf("size = %d\n", sizeof(host_interfaces_list));
	return ret;
}

static int32_t get_gateways()
{
	int32_t ret = EXIT_SUCCESS;
//get interface info
	LIST_HEAD(list_head);

//Traversal list
	ts_gateway_list_t * gateways = NULL;
	int32_t counts = 0;
	ret = db_web_get_all_gateways(&list_head, gateways, &counts);
	if (ret != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	const char * base_format = "{'id':'%d','ip':'%s','port':'%d','type':'%s','on_off':'%d'},";
	int len = (strlen(base_format) + TS_DB_INT_ADD * 2 + sizeof(ts_gateway_t));
	char * infos = (char *) calloc(len * counts, sizeof(char));
	char * bak_infos = infos;
	char * s_tmp = (char *) calloc(len, sizeof(char));

	all_gateway_ids = (char *) calloc(counts * TS_DB_INT_ADD + counts + 1 + 1, sizeof(char));
	char * gateway_ids_bak = all_gateway_ids;

	const char * gateway_id_format = "'%d',";
	ts_gateway_list_t * p_tmp = NULL;
	int32_t GATEWAY_UNIT_LEN = TS_DB_INT_ADD + strlen(gateway_id_format);
//	char s_int[GATEWAY_UNIT_LEN] = { 0 };
	char * s_int = (char*) calloc(GATEWAY_UNIT_LEN, sizeof(char));

	int i = 0;

	int32_t on_off = SWITCH_OFF;
	web_action_report_t act_ctrl;
	list_for_each_entry_safe(gateways,p_tmp,&list_head,list) {

		memset(s_tmp, 0, len);

//get on off
		if (TS_RUNNING == is_exits) {
			act_ctrl.data = on_off;
			act_ctrl.start_flag = BUFF_START;
			act_ctrl.source = HOST_WEB_PROGRAM;
			act_ctrl.obj = gateways->gateway.id;
			act_ctrl.fun_code = GET_ON_OFF_GW;
			ts_send_data(&act_ctrl);
			on_off = act_ctrl.data;
		} else {
			on_off = TS_WEB_OFF;
		}

//		protocol_is_gateway_open(gateways->gateway.id, &on_off);
		sprintf(s_tmp, base_format, gateways->gateway.id, gateways->gateway.gatewayIP, gateways->gateway.gatewayPort, gateways->gateway.gateway_type_name, on_off);
		memcpy(infos, s_tmp, strlen(s_tmp));
		if (i < counts - 1) {
			infos += strlen(s_tmp);
		}

//set gateway ids;
		memset(s_int, 0, TS_DB_INT_ADD);
		sprintf(s_int, gateway_id_format, gateways->gateway.id);
		memcpy(all_gateway_ids, s_int, GATEWAY_UNIT_LEN);
		if (i < counts - 1) {
			all_gateway_ids += strlen(s_int);
		}

		list_del(&gateways->list);
		ts_free(gateways);
	}

	all_gateway_ids = gateway_ids_bak;
	int32_t all_gw_len = strlen(all_gateway_ids);
	*(all_gateway_ids + all_gw_len - 1) = '\0';

//del last common,for ie6
	int32_t gw_infos_len = strlen(bak_infos);
	*(bak_infos + gw_infos_len - 1) = '\0';

	fprintf(cgiOut, "	parameter_gateway=[%s]\n", bak_infos);

	free(bak_infos);
	free(s_tmp);
	free(s_int);

	return ret;
}

//get all passways
int32_t get_all_passways()
{
	int32_t rc = EXIT_SUCCESS;
	const char * base_format = "{'id':'%d','note':'%s','description':'%s','gateway':'%s','gateway_select':[%s]},";

//get interface info
	LIST_HEAD(list_head);
	ts_passageway_list_t * passageways = NULL;
	int32_t counts = 0;
	rc = db_get_all_lines_counts(&list_head, passageways, &counts);
	if (rc != SQLITE_OK) {
		return EXIT_FAILURE;
	}

//set show list
	int len = (strlen(base_format) + strlen(all_gateway_ids) + TS_DB_INT_ADD * 2 + sizeof(ts_passageway_t));
	char * infos = (char *) calloc(len * counts, sizeof(char));
	char * bak_infos = infos;
	char * s_tmp = (char *) calloc(len, sizeof(char));

//Traversal list
	ts_passageway_list_t * p_tmp = NULL;
	int32_t i = 0;
	list_for_each_entry_safe(passageways,p_tmp,&list_head,list) {
		memset(s_tmp, 0, len);
//if interface type is IP,select gateways
		if ((passageways->passageway.host_interface.type == TS_HOST_INTERFACE_TYPE_IP) && (NULL != passageways->passageway.gateway_list)) {
			//set show gateway list
			int gateway_len = (passageways->passageway.gateway_counts * (TS_DB_INT_ADD + 1));
			char * gateway_infos = (char *) calloc(gateway_len, sizeof(char));
			ts_gateway_list_t * p_tmp_gateway = NULL;
			list_for_each_entry_safe(passageways->passageway.gateway_list,p_tmp_gateway,&passageways->passageway.gateway_heead,list) {
				if (passageways->passageway.gateway_list->gateway.id > 0) {
					sprintf(gateway_infos, "%s%d,", gateway_infos, passageways->passageway.gateway_list->gateway.id);
				}
			}
			sprintf(s_tmp, base_format, passageways->passageway.host_interface.id, passageways->passageway.host_interface.info, passageways->passageway.host_interface.desc, gateway_infos, all_gateway_ids);
			free(gateway_infos);
		} else {
			sprintf(s_tmp, base_format, passageways->passageway.host_interface.id, passageways->passageway.host_interface.info, passageways->passageway.host_interface.desc, "", all_gateway_ids);
		}
		memcpy(infos, s_tmp, strlen(s_tmp));
		if (i < counts - 1) {
			infos += strlen(s_tmp);
		}

		list_del(&passageways->list);
		free(passageways);
	}

//del last common,for ie6
	int32_t infos_len = strlen(bak_infos);
	*(bak_infos + infos_len - 1) = '\0';

	fprintf(cgiOut, "	parameter=[%s]\n", bak_infos);
	free(bak_infos);
	free(s_tmp);
	return rc;
}

int32_t get_comm_ports()
{
	int32_t ret = EXIT_SUCCESS;

//get interface info
	LIST_HEAD(list_head);
	ts_host_interface_serial_info_list * serial_infos = NULL;
	int counts = 0;
	ret = db_get_all_serial_details(&list_head, serial_infos, &counts);
	if (ret != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	const char * format = "'%s',";
	ts_host_interface_serial_info ser_info;
	int32_t len = strlen(format) + sizeof(ser_info.comm_port);
	comm_ports = (char *) calloc(len * counts, sizeof(char));
	char * comm_ports_bak = comm_ports;
	char * s_int = (char*) calloc(len, sizeof(char));

	int32_t i = 0;
//Traversal list
	ts_host_interface_serial_info_list * p_tmp = NULL;
	list_for_each_entry_safe(serial_infos,p_tmp,&list_head,list) {
//set gateway ids;
		memset(s_int, 0, len);
		sprintf(s_int, format, serial_infos->host_serial.comm_port);
		memcpy(comm_ports, s_int, len);
		if (i < counts - 1) {
			comm_ports += strlen(s_int);
		}
		list_del(&serial_infos->list);
		ts_free(serial_infos);
	}

	comm_ports = comm_ports_bak;
	int32_t infos_len = strlen(comm_ports_bak);
	*(comm_ports_bak + infos_len - 1) = '\0';

//	log_debug_web( "%s\n", comm_ports);
	return ret;
}

int32_t get_host_serial_device_infos()
{
	int32_t ret = EXIT_SUCCESS;
//get interface info
	LIST_HEAD(list_head);
	ts_web_host_device_info_list * serial_dev = NULL;
	int32_t counts = 0;
	ret = db_web_get_host_device_infos(&list_head, serial_dev, &counts);
	if (ret != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	if (counts <= 0) {
		ret = ERR_DB_NO_RECORDS;
		return ret;
	}

	const char * format = "{'id':'%d','equipment':'%s','room':'%s','homeport_type':'串口','homeport_note':'%s','homeport_note_select':[%s]},";
	int len = (strlen(format) + TS_DB_INT_ADD * 2 + sizeof(ts_host_interface_serial_info_list) + strlen(comm_ports));
	char * infos = (char *) calloc(len * counts, sizeof(char));
	char * bak_infos = infos;
	char * s_tmp = (char *) calloc(len, sizeof(char));
	int i = 0;
//Traversal list
	ts_web_host_device_info_list * p_tmp = NULL;
	list_for_each_entry_safe(serial_dev, p_tmp, &list_head, list) {
		memset(s_tmp, 0, len);
		sprintf(s_tmp, format, serial_dev->dev_info.id, serial_dev->dev_info.device_name, serial_dev->dev_info.room_name, serial_dev->dev_info.host_info, comm_ports);
		memcpy(infos, s_tmp, strlen(s_tmp));
		if (i < counts - 1) {
			infos += strlen(s_tmp);
		}
		list_del(&serial_dev->list);
		ts_free(serial_dev);
	}
//	log_debug_web( "	parameter=[%s]\n", bak_infos);
//del last common,for ie6
	int32_t infos_len = strlen(bak_infos);
	*(bak_infos + infos_len - 1) = '\0';

	fprintf(cgiOut, "	parameter=[%s]\n", bak_infos);
	free(bak_infos);
	free(s_tmp);
	return ret;
}

int cgiMain()
{

	int32_t ret = EXIT_SUCCESS;
	log_open("curr_state.c");

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
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/ts_set_base_info.js\"></script>\n");
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
	fprintf(cgiOut, "			<div class=\"center\" style='height:2000px'>\n");

//	TS_WEB_MENUS
	fprintf(cgiOut, "				<!--导航 start-->\n");
	fprintf(cgiOut, "<script type=\"text/javascript\">print_left_guide(%d)</script>\n", TS_LEFT_GUIDE_CURR_STATE);
	fprintf(cgiOut, "				<!--导航 end-->\n");

	fprintf(cgiOut, "				<!--内容 start-->\n");
	fprintf(cgiOut, "				<div class=\"content\">\n");
	fprintf(cgiOut, "				<div class=\"curr_state\" >\n");

//程序开关
	ts_is_running(TS_MONITOR_NAME, &is_exits);
	fprintf(cgiOut, "\n						<!--host is open. start-->\n");
	fprintf(cgiOut, "						<div class=\"title\" id=\"basic_control\">程序开关</div>\n");
	fprintf(cgiOut, "						<div class=\"switch\">\n");
	fprintf(cgiOut, "							<table width=\"200\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "  								<tr align=\"left\" valign=\"middle\" height=\"35\">\n");
	if (TS_RUNNING == is_exits) {
		fprintf(cgiOut, "    								<td><span class=\"info\">程序开关 : 开</span></td>\n");
	} else {
		fprintf(cgiOut, "    								<td><span class=\"info\">程序开关 : 关</span></td>\n");
	}
	fprintf(cgiOut, "  								</tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--host is open. end-->\n");

//服务器信息
	{
		fprintf(cgiOut, "				<!--SEVER信息 start-->\n");
		ts_host_base_user_info base_info;
		db_get_base_user_info(&base_info);
		fprintf(cgiOut, "				<div class=\"title\" id=\"server_info\">服务器信息</div>\n");
		fprintf(cgiOut, "				<div class=\"net_number\">\n");
		fprintf(cgiOut, "				<table width=\"600\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
		fprintf(cgiOut, "					<tr height=\"40\" valign=\"middle\">\n");
		fprintf(cgiOut, "						<td width=\"80\" align=\"left\"><span><b>用户名：</b></span></td>\n");
		fprintf(cgiOut, "						<td width=\"140\">%s</td>\n", base_info.login_user_name);
		fprintf(cgiOut, "						<td width=\"100\" align=\"left\"><span><b>密码：</b></span></td>\n");
		int32_t i = 0;
		char show_passwd[32] = { 0 };
		for (i = 0; i < strlen(base_info.login_passwd); i++) {
			if ((0 == i) || (i == strlen(base_info.login_passwd) - 1)) {
				sprintf(show_passwd, "%s%c", show_passwd, base_info.login_passwd[i]);
			} else {
				sprintf(show_passwd, "%s%c", show_passwd, '*');
			}
		}
		fprintf(cgiOut, "						<td width=\"200\">%s</td>\n", show_passwd);
		fprintf(cgiOut, "					</tr>\n");
		fprintf(cgiOut, "					<tr height=\"40\" valign=\"middle\">\n");
		fprintf(cgiOut, "						<td width=\"80\" align=\"left\"><span><b>服务器IP：</b></span></td>\n");
		fprintf(cgiOut, "						<td width=\"200\">%s</td>\n", base_info.server_name);
		fprintf(cgiOut, "						<td width=\"100\" align=\"left\"><span><b>服务器端口：</b></span></td>\n");
		fprintf(cgiOut, "						<td width=\"200\">%d </td>\n", base_info.server_port);
		fprintf(cgiOut, "					</tr>\n");
		fprintf(cgiOut, "				</table>\n");
		fprintf(cgiOut, "				</div>\n");
		fprintf(cgiOut, "				<!--SEVER信息 end-->\n");
	}

//IP
	{
		fprintf(cgiOut, "						<!--网络信息 start-->\n");
		fprintf(cgiOut, "						<div class=\"title\" id=\"network_info\">网络信息</div>\n");
		fprintf(cgiOut, "						<div class=\"newwork_info\">\n");
		SET_NET_INFOS
		fprintf(cgiOut, "						</div>						\n");
		fprintf(cgiOut, "						<!--网络信息 end-->\n");
	}

//串口参数
	fprintf(cgiOut, "\n						<!--serial info . start-->\n");
	fprintf(cgiOut, "						<div class=\"title\" id=\"basic_control\">串口参数</div>\n");
	fprintf(cgiOut, "					<!--表格 start-->\n");
	fprintf(cgiOut, "					<div class=\"home_serial_port_info\">\n");
	fprintf(cgiOut, "						<!--标题 start-->\n");
	fprintf(cgiOut, "						<div class=\"title_table\">\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\">\n");
	fprintf(cgiOut, "							  <tr>\n");
	fprintf(cgiOut, "								<th width=\"41\">ID</th>\n");
	fprintf(cgiOut, "								<th width=\"91\">波特率</th>\n");
	fprintf(cgiOut, "								<th width=\"91\">数据位</th>\n");
	fprintf(cgiOut, "								<th width=\"91\">停止位</th>\n");
	fprintf(cgiOut, "								<th width=\"91\">校验</th>\n");
	fprintf(cgiOut, "								<th width=\"111\">串口名</th>\n");
//	fprintf(cgiOut, "								<th width=\"83\">操作</th>\n");
	fprintf(cgiOut, "								<th width=\"110\">开关</th>\n");
	fprintf(cgiOut, "							  </tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");

	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" >\n");
	fprintf(cgiOut, "function serial_port_list(parameter){\n");
	fprintf(cgiOut, "	if(parameter!=null){\n");
	fprintf(cgiOut, "		for(i=0;i<parameter.length;i++){\n");
	fprintf(cgiOut, "			var id=parameter[i].id;//id\n");
	fprintf(cgiOut, "			//var description=parameter[i].description;//描述\n");
	fprintf(cgiOut, "			var baud_rate=parameter[i].baud_rate;//波特率\n");
	fprintf(cgiOut, "			var data_bit=parameter[i].data_bit;//数据位\n");
	fprintf(cgiOut, "			var stop_bit=parameter[i].stop_bit;//停止位\n");
	fprintf(cgiOut, "			var verify=parameter[i].verify;//校验\n");
	fprintf(cgiOut, "			var serial_name=parameter[i].serial_name;//串口名\n");
	fprintf(cgiOut, "			var serial_port_switch=parameter[i].serial_port_switch;//on off\n");
//	fprintf(cgiOut, "				$(\".serial_port_info table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"ts_id\" class=\"serial_port_id\">'+id+'</span></td><td width=\"91\" align=\"center\"><span class=\"baud_rate\">'+baud_rate+'</span><input type=\"text\" class=\"baud_rate_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"data_bit\">'+data_bit+'</span><input type=\"text\" class=\"data_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"stop_bit\">'+stop_bit+'</span><input type=\"text\" class=\"stop_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"verify\">'+verify+'</span><input type=\"text\" class=\"verify_revise\" style=\"display:none\"/></td><td width=\"111\" align=\"center\"><span class=\"serial_name\">'+serial_name+'</span><input type=\"text\" class=\"serial_name_revise\" style=\"display:none\"/></td></tr>');\n");
	fprintf(cgiOut, "			if(%d==serial_port_switch) {\n", TS_WEB_ON);
	fprintf(cgiOut, "				$(\".serial_port_info table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"ts_id\" class=\"serial_port_id\">'+id+'</span></td><td width=\"91\" align=\"center\"><span class=\"baud_rate\">'+baud_rate+'</span><input type=\"text\" class=\"baud_rate_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"data_bit\">'+data_bit+'</span><input type=\"text\" class=\"data_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"stop_bit\">'+stop_bit+'</span><input type=\"text\" class=\"stop_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"verify\">'+verify+'</span><input type=\"text\" class=\"verify_revise\" style=\"display:none\"/></td><td width=\"111\" align=\"center\"><span class=\"serial_name\">'+serial_name+'</span><input type=\"text\" class=\"serial_name_revise\" style=\"display:none\"/></td><td width=\"110\" align=\"center\" valign=\"middle\">打开</td></tr>');\n");
	fprintf(cgiOut, "			} else {\n");
	fprintf(cgiOut, "				$(\".serial_port_info table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"ts_id\" class=\"serial_port_id\">'+id+'</span></td><td width=\"91\" align=\"center\"><span class=\"baud_rate\">'+baud_rate+'</span><input type=\"text\" class=\"baud_rate_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"data_bit\">'+data_bit+'</span><input type=\"text\" class=\"data_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"stop_bit\">'+stop_bit+'</span><input type=\"text\" class=\"stop_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"verify\">'+verify+'</span><input type=\"text\" class=\"verify_revise\" style=\"display:none\"/></td><td width=\"111\" align=\"center\"><span class=\"serial_name\">'+serial_name+'</span><input type=\"text\" class=\"serial_name_revise\" style=\"display:none\"/></td><td width=\"110\" align=\"center\" valign=\"middle\">关闭</td></tr>');\n");
//	fprintf(cgiOut, "				$(\".serial_port_info table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"ts_id\" class=\"serial_port_id\">'+id+'</span></td><td width=\"91\" align=\"center\"><span class=\"baud_rate\">'+baud_rate+'</span><input type=\"text\" class=\"baud_rate_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"data_bit\">'+data_bit+'</span><input type=\"text\" class=\"data_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"stop_bit\">'+stop_bit+'</span><input type=\"text\" class=\"stop_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"verify\">'+verify+'</span><input type=\"text\" class=\"verify_revise\" style=\"display:none\"/></td><td width=\"111\" align=\"center\"><span class=\"serial_name\">'+serial_name+'</span><input type=\"text\" class=\"serial_name_revise\" style=\"display:none\"/></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" id=\"ts_del\" class=\"delete_btn serial_port_delete_'+(++serial_port_delete_no)+'\"/></td><td width=\"110\" align=\"center\" valign=\"middle\"><input name=\"serial_port_'+serial_port_delete_no+'\"  id=\"switch_on\" type=\"radio\" value=\"\"  /> 打开 <input id=\"switch_off\" name=\"serial_port_'+serial_port_delete_no+'\" type=\"radio\" value=\"\" checked/> 关闭</td></tr>');\n");
	fprintf(cgiOut, "			} \n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "//从数据库取出数据，列出已有列表\n");
	fprintf(cgiOut, "</script>\n");
	fprintf(cgiOut, "						<!--标题 end-->\n");
	fprintf(cgiOut, "						<!--内容 start-->\n");
	fprintf(cgiOut, "						<div class=\"serial_port_info\" id=\"box_scroll\"  style='height:150px'>\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\" style=\"border-top:0px\">\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--内容 end-->\n");
	fprintf(cgiOut, "					</div>\n");
	fprintf(cgiOut, "					<!--表格 end-->\n");
	fprintf(cgiOut, "					<!--serial info . end-->\n");

//网关通路
	fprintf(cgiOut, "\n						<!--网关通路 . start-->\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" >\n");
	fprintf(cgiOut, "//网关\n");
	fprintf(cgiOut, "var homeport_delete_no=0;//声明网卡删除按钮编号\n");
	fprintf(cgiOut, "function gateway_list(parameter_gateway){\n");
	fprintf(cgiOut, "	if(parameter_gateway != null){\n");
	fprintf(cgiOut, "		for(i=0;i<parameter_gateway.length;i++){\n");
	fprintf(cgiOut, "			var id=parameter_gateway[i].id;\n");
	fprintf(cgiOut, "			var ip=parameter_gateway[i].ip;\n");
	fprintf(cgiOut, "			var port=parameter_gateway[i].port;\n");
	fprintf(cgiOut, "			var type=parameter_gateway[i].type;\n");
	fprintf(cgiOut, "			var on_off=parameter_gateway[i].on_off;\n");

	fprintf(cgiOut, "			if(%d==on_off) {\n", SWITCH_ON);
	fprintf(cgiOut, "				$(\".gateway_info_list table.table_serial_port\").append('<tr align=\"center\"><td width=\"41\"><span  id=\"ts_id\" class=\"gateway_id\">'+id+'</span></td><td width=\"298\">'+ip+'</td><td width=\"153\">'+port+'</td><td width=\"110\"><span class=\"gateway_type\">'+type+'</span></td><td width=\"110\">连接</td></tr>');\n");
	fprintf(cgiOut, "			} else {\n");
	fprintf(cgiOut, "				$(\".gateway_info_list table.table_serial_port\").append('<tr align=\"center\"><td width=\"41\"><span  id=\"ts_id\" class=\"gateway_id\">'+id+'</span></td><td width=\"298\">'+ip+'</td><td width=\"153\">'+port+'</td><td width=\"110\"><span class=\"gateway_type\">'+type+'</span></td><td width=\"110\">断开</td></tr>');\n");
//	fprintf(cgiOut, "				$(\".gateway_info_list table.table_serial_port\").append('<tr align=\"center\"><td width=\"41\"><span  id=\"ts_id\" class=\"gateway_id\">'+id+'</span></td><td width=\"298\">'+ip+'</td><td width=\"153\">'+port+'</td><td width=\"110\"><span class=\"gateway_type\">'+type+'</span></td><td width=\"110\"><input  id=\"switch_on\" name=\"gateway_'+(++gateway_no)+'\" class=\"gateway_'+gateway_no+'\" type=\"radio\" value=\"\" /> 连接 <input id=\"switch_off\" name=\"gateway_'+gateway_no+'\" type=\"radio\" value=\"\" checked/> 断开</td></tr>');\n");
	fprintf(cgiOut, "			} \n");

//	fprintf(cgiOut, "			$(\".gateway_info_list table.table_serial_port\").append('<tr align=\"center\"><td width=\"41\"><span  id=\"ts_id\" class=\"gateway_id\">'+id+'</span></td><td width=\"298\">'+ip+'</td><td width=\"153\">'+port+'</td><td width=\"110\"><span class=\"gateway_type\">'+type+'</span></td></tr>');\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "//网卡生成已有列表\n");
	fprintf(cgiOut, "function homeport_list(parameter){\n");
	fprintf(cgiOut, "	if(parameter != null){\n");
	fprintf(cgiOut, "		for(i=0;i<parameter.length;i++){\n");
	fprintf(cgiOut, "			var id=parameter[i].id;\n");
	fprintf(cgiOut, "			var note=parameter[i].note;\n");
	fprintf(cgiOut, "			var description=parameter[i].description;\n");
	fprintf(cgiOut, "			var gateway=parameter[i].gateway;\n");
	fprintf(cgiOut, "			var gateway_select=parameter[i].gateway_select;\n");
//fprintf(cgiOut, "			$(\".home_port_list table.table_serial_port\").append('<tr align=\"center\"><td width=\"41\"><span class=\"home_port_id\">'+id+'</span></td><td width=\"186\"><span class=\"home_port_note\">'+note+'</span><input type=\"text\" class=\"home_port_note_revise\" style=\"display:none\"/></td><td width=\"292\"><span class=\"home_port_description\">'+description+'</span><input type=\"text\" class=\"home_port_description_revise\" style=\"display:none\"/></td><td width=\"110\"><span class=\"home_port_gateway\">'+gateway+'</span><span class=\"homeport_gateway_select\" style=\"display:none\"><input type=\"text\" value=\"\"/><select class=\"homeport_gateway_second\" size=\"5\" multiple=\"multiple\" style=\"display:none;z-index:'+(9999-(++homeport_delete_no))+'\"></select></span></td><td width=\"83\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input id=\"ts_del\" type=\"button\" class=\"delete_btn homeport_delete_'+homeport_delete_no+'\"/></td></tr>');\n");
	fprintf(cgiOut, "			$(\".home_port_list table.table_serial_port\").append('<tr align=\"center\"><td width=\"41\"><span class=\"home_port_id\">'+id+'</span></td><td width=\"186\"><span class=\"home_port_note\">'+note+'</span><input type=\"text\" class=\"home_port_note_revise\" style=\"display:none\"/></td><td width=\"292\"><span class=\"home_port_description\">'+description+'</span><input type=\"text\" class=\"home_port_description_revise\" style=\"display:none\"/></td><td width=\"110\"><span class=\"home_port_gateway\">'+gateway+'</span></tr>');\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "//从数据库取出数据，列出已有列表\n");
//	fprintf(cgiOut, "window.onload = function (){\n");
//	fprintf(cgiOut, "	serial_port_list(parameter);	\n");
//	fprintf(cgiOut, "	var parameter_gateway=\"\";");
//	get_gateways();
//	fprintf(cgiOut, "	gateway_list(parameter_gateway);//网关\n");
//
//////interface
//	fprintf(cgiOut, "	parameter=\"\";\n");
//	get_all_passways();
//	fprintf(cgiOut, "	homeport_list(parameter);//网卡	\n");
//	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "</script>\n");

	fprintf(cgiOut, "					<div class=\"title\" id=\"basic_control\">网关通路</div>\n");
	fprintf(cgiOut, "					<!--网关 start-->\n");
	fprintf(cgiOut, "						<!--标题 start-->\n");
	fprintf(cgiOut, "						<div class=\"pathway_info_title_guide\">网关</div>\n");
	fprintf(cgiOut, "						<!--标题 end-->\n");
	fprintf(cgiOut, "						<!--表格 start-->\n");
	fprintf(cgiOut, "						<div class=\"pathway_info_table\">\n");
	fprintf(cgiOut, "							<!--标题 start-->\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\">\n");
	fprintf(cgiOut, "							  <tr>\n");
	fprintf(cgiOut, "								<th width=\"41\">ID</th>\n");
	fprintf(cgiOut, "								<th width=\"298\">IP地址</th>\n");
	fprintf(cgiOut, "								<th width=\"153\">端口</th>\n");
	fprintf(cgiOut, "								<th width=\"110\">类型</th>\n");
	fprintf(cgiOut, "								<th width=\"110\">开关</th>\n");
	fprintf(cgiOut, "							  </tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "							<!--标题 end-->\n");
	fprintf(cgiOut, "							<!--列表 start-->\n");
	fprintf(cgiOut, "							<div class=\"gateway_info_list\"  id=\"box_scroll\" style=\"height:150px\">\n");
	fprintf(cgiOut, "								<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\" style=\"border-top:0px\">\n");
	fprintf(cgiOut, "								</table>\n");
	fprintf(cgiOut, "							</div>\n");
	fprintf(cgiOut, "							<!--列表 end-->\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--表格 end-->\n");
	fprintf(cgiOut, "					<!--网关 end-->\n");

	fprintf(cgiOut, "					<!--网卡 start-->\n");
	fprintf(cgiOut, "						<!--标题 start-->\n");
	fprintf(cgiOut, "						<div class=\"pathway_info_title_guide\">网卡</div>\n");
	fprintf(cgiOut, "						<!--标题 end-->\n");
	fprintf(cgiOut, "						<!--表格 start-->\n");
	fprintf(cgiOut, "						<div class=\"pathway_info_table\">\n");
	fprintf(cgiOut, "							<!--标题 start-->\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\">\n");
	fprintf(cgiOut, "							  <tr>\n");
	fprintf(cgiOut, "								<th width=\"41\">ID</th>\n");
	fprintf(cgiOut, "								<th width=\"186\">IP地址</th>\n");
	fprintf(cgiOut, "								<th width=\"292\">描述</th>\n");
	fprintf(cgiOut, "								<th width=\"110\">对应网关</th>\n");
//	fprintf(cgiOut, "								<th width=\"83\">操作</th>\n");
	fprintf(cgiOut, "							  </tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "							<!--标题 end-->\n");
	fprintf(cgiOut, "							<!--列表 start-->\n");
	fprintf(cgiOut, "							<div class=\"home_port_list\" id=\"box_scroll\" style=\"height:150px\">\n");
	fprintf(cgiOut, "								<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\" style=\"border-top:0px;\">\n");
	fprintf(cgiOut, "								</table>\n");
	fprintf(cgiOut, "							</div>\n");
	fprintf(cgiOut, "							<!--列表 end-->\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--表格 end-->\n");
	fprintf(cgiOut, "					<!--网卡 end-->\n");
	fprintf(cgiOut, "					<!--网关通路 . end-->\n");

//串口设备通路
	fprintf(cgiOut, "\n						<!--串口设备通路 . start-->\n");
	fprintf(cgiOut, "						<div class=\"title\" id=\"basic_control\">串口设备通路</div>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" >\n");
	fprintf(cgiOut, "//生成已有列表 \n");
	fprintf(cgiOut, "function device_link_access_list(parameter){\n");
	fprintf(cgiOut, "	if(parameter != null){\n");
	fprintf(cgiOut, "		for(i=0;i<parameter.length;i++){\n");
	fprintf(cgiOut, "			var id=parameter[i].id;\n");
	fprintf(cgiOut, "			var equipment=parameter[i].equipment;\n");
	fprintf(cgiOut, "			var room=parameter[i].room;\n");
	fprintf(cgiOut, "			var homeport_type=parameter[i].homeport_type;\n");
	fprintf(cgiOut, "			var homeport_note=parameter[i].homeport_note;\n");
	fprintf(cgiOut, "			var homeport_note_select=parameter[i].homeport_note_select;\n");
	fprintf(cgiOut, "			$(\".device_link_access_list table.table_serial_port\").append('<tr><td width=\"41\"><span class=\"device_link_id\">'+id+'</span></td><td width=\"113\" align=\"center\"><span class=\"device_link_equipment\">'+equipment+'</span></td><td width=\"179\" align=\"center\"><span class=\"device_link_room\">'+room+'</span></td><td width=\"133\" align=\"center\"><span class=\"device_link_homeporttype\">'+homeport_type+'</span></td><td width=\"162\" align=\"center\"><span class=\"device_link_homeportnote\">'+homeport_note+'</span></td></tr>');\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "</script>\n");
	fprintf(cgiOut, "					<!--表格 start-->\n");
	fprintf(cgiOut, "					<div class=\"home_serial_port_info\">\n");
	fprintf(cgiOut, "						<!--标题 start-->\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\">\n");
	fprintf(cgiOut, "							  <tr>\n");
	fprintf(cgiOut, "								<th width=\"41\">ID</th>\n");
	fprintf(cgiOut, "								<th width=\"113\">设备名称</th>\n");
	fprintf(cgiOut, "								<th width=\"179\">设备所在房间</th>\n");
	fprintf(cgiOut, "								<th width=\"133\">主机口类型</th>\n");
	fprintf(cgiOut, "								<th width=\"162\">串口名</th>\n");
//	fprintf(cgiOut, "								<th width=\"83\">操作</th>\n");
	fprintf(cgiOut, "							  </tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						<!--标题 end-->\n");
	fprintf(cgiOut, "						<!--内容 start-->\n");
	fprintf(cgiOut, "						<div class=\"device_link_access_list\"  id=\"box_scroll\" style=\"height:150px\">\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\" style=\"border-top:0px\">\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--内容 end-->\n");
	fprintf(cgiOut, "					</div>\n");
	fprintf(cgiOut, "					<!--表格 end-->\n");

	fprintf(cgiOut, "						<!--串口设备通路 . end-->\n");

//红外设备通路
	fprintf(cgiOut, "\n						<!--红外设备通路 . start-->\n");
	fprintf(cgiOut, "						<div class=\"title\" id=\"basic_control\">红外设备通路</div>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" >\n");
	fprintf(cgiOut, "//从数据库取出数据，列出已有列表\n");
	fprintf(cgiOut, "window.onload = function (){\n");
//serial comms
	fprintf(cgiOut, "var parameter = \"\"\n");
	get_all_serial();
	fprintf(cgiOut, "	serial_port_list(parameter);	\n");

//gateway
	fprintf(cgiOut, "	var parameter_gateway=\"\";");
	get_gateways();
	fprintf(cgiOut, "	gateway_list(parameter_gateway);//网关\n");

//interface
	fprintf(cgiOut, "	parameter=\"\";\n");
	get_all_passways();
	fprintf(cgiOut, "	homeport_list(parameter);//网卡	\n");

//device serial
	fprintf(cgiOut, "	parameter=\"\";\n");
	get_comm_ports();
	get_host_serial_device_infos();
	fprintf(cgiOut, "	device_link_access_list(parameter);\n");

//infrared
	fprintf(cgiOut, "	parameter;\n");
	get_infrared_device();
	fprintf(cgiOut, "	infrared_device_list(parameter);\n");

	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "//生成已有列表 \n");
	fprintf(cgiOut, "function infrared_device_list(parameter){\n");
	fprintf(cgiOut, "	if(parameter != null){\n");
	fprintf(cgiOut, "		for(i=0;i<parameter.length;i++){\n");
	fprintf(cgiOut, "			var id=parameter[i].id;\n");
	fprintf(cgiOut, "			var infrared_device_name=parameter[i].infrared_device_name;\n");
//	fprintf(cgiOut, "			alert(infrared_device_name)\n");
	fprintf(cgiOut, "			var infrared_device_pathway_no=parameter[i].infrared_device_pathway_no;\n");
	fprintf(cgiOut, "			var infrared_device_pathway_select=parameter[i].infrared_device_pathway_select;\n");
	fprintf(cgiOut, "			var on_off=parameter[i].on_off;\n");
	fprintf(cgiOut, "			if(0==i) {\n");
	fprintf(cgiOut, "				if(%d==%d) {\n", TS_WEB_ON, all_on_off);
	fprintf(cgiOut, "					$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"host_infr_id\" class=\"infrared_device_id\">'+id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\">'+infrared_device_name+'</span></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\">'+infrared_device_pathway_no+'</span></td><td rowspan=%d width=\"110\" align=\"center\">打开</td></tr>');\n", dev_counts);
	fprintf(cgiOut, "				} else {\n");
	fprintf(cgiOut, "					$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"host_infr_id\" class=\"infrared_device_id\">'+id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\">'+infrared_device_name+'</span></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\">'+infrared_device_pathway_no+'</span></td><td rowspan=%d width=\"110\" align=\"center\">关闭</td></tr>');\n", dev_counts);
	fprintf(cgiOut, "				} \n");
	fprintf(cgiOut, "			}else {\n");
	fprintf(cgiOut, "					$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"host_infr_id\" class=\"infrared_device_id\">'+id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\">'+infrared_device_name+'</span></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\">'+infrared_device_pathway_no+'</span></td></tr>');\n");
	fprintf(cgiOut, "			}\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");

	fprintf(cgiOut, "</script>\n");
	fprintf(cgiOut, "<!--表格 start-->\n");
	fprintf(cgiOut, "					<div class=\"infrared_device_pathway_info\">\n");
	fprintf(cgiOut, "						<!--标题 start-->\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\">\n");
	fprintf(cgiOut, "							  <tr>\n");
	fprintf(cgiOut, "								<th width=\"41\">ID</th>\n");
	fprintf(cgiOut, "								<th width=\"250\">红外设备名称</th>\n");
	fprintf(cgiOut, "								<th width=\"228\">通路号</th>\n");
//	fprintf(cgiOut, "								<th width=\"83\">操作</th>\n");
	fprintf(cgiOut, "								<th width=\"110\">开关</th>\n");
	fprintf(cgiOut, "							  </tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						<!--内容 start-->\n");
	fprintf(cgiOut, "						<div class=\"infrared_device_list\" id=\"box_scroll\" style=\"height:150px\">\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\" style=\"border-top:0px\">\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--内容 end-->\n");
	fprintf(cgiOut, "					</div>\n");
	fprintf(cgiOut, "					<!--表格 end-->\n");
	fprintf(cgiOut, "						<!--红外设备通路 . end-->\n");

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

	ts_free(comm_ports);
	ts_free(all_gateway_ids);

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
