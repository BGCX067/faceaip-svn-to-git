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

ts_host_net_info net_info;
int32_t upd_net_info(const char * net_name, int32_t is_running)
{
	int32_t ret = EXIT_SUCCESS;

	//kill host
	if (TS_RUNNING == is_running) {
		stop_pro();
	}

	if (0 == strcmp(TS_WEB_GET_NO_NET_ADDR, net_info.ip)) {
		strcpy(net_info.ip, TS_WEB_DEFAULT_IP);
	}

	if (0 == strcmp(TS_WEB_GET_NO_NET_ADDR, net_info.gw)) {
		strcpy(net_info.gw, TS_WEB_DEFAULT_GW);
	}

	if (0 == strcmp(TS_WEB_GET_NO_NET_ADDR, net_info.netmask)) {
		strcpy(net_info.netmask, TS_WEB_DEFAULT_NM);
	}

	char record[256] = { 0 };
	sprintf(record, "echo ./set_net_infos.sh \"%s\" %d \"%s\" \"%s\" \"%s\" > %sset_net_%s;chmod 755 %sset_net_%s", net_name, net_info.is_dhcp, net_info.ip, net_info.gw, net_info.netmask, TS_PRO_PATH, net_name, TS_PRO_PATH, net_name);
	system(record);

	//if it can dns ,add dns info
	if((NULL != net_info.dns) && (strlen(net_info.dns) > 0)) {
		char set_dns[256] = { 0 };
		sprintf(set_dns, "echo \"echo nameserver %s > /etc/resolv.conf \" >> %sset_net_%s",net_info.dns, TS_PRO_PATH, net_name);
		system(set_dns);
		bzero(set_dns,sizeof(set_dns));
		sprintf(set_dns, "echo \"echo nameserver 8.8.8.8 >> /etc/resolv.conf \" >> %sset_net_%s",TS_PRO_PATH, net_name);
		system(set_dns);
	}

	//if type is wifi , add wifi info
	if (strstr(net_name, "wlan") != NULL) {
		char ap[128] = { 0 };
		sprintf(ap, "echo \"iwconfig %s ap auto;iwconfig wlan0 essid '%s' key %s;\" >> %sset_net_%s", net_name, net_info.ssid, net_info.login_passwd, TS_PRO_PATH, net_name);
		system(ap);
	}

	char cmd[128] = { 0 };
	sprintf(cmd, "cd %s;./set_net_%s", TS_PRO_PATH, net_name);
	system(cmd);

	//set ip ,gw,net mask
	db_web_set_host_net(net_info);

//start host
	if (TS_RUNNING == is_running) {
		start_pro();
	}

	//kill web server
	char cmd_kill_webs[128] = { 0 };
	sprintf(cmd_kill_webs, "killall %s;", TS_WEB_SERVER_NAME);
	system(cmd_kill_webs);

	return ret;
}

int cgiMain(void)
{
	int rc = EXIT_SUCCESS;
	log_open("set_ip_port.c");

	char * data;
	int32_t is_running = TS_NO_RUNNING;
//	char net_name[32] = { 0 };

	printf("Content-Type:text/html;charset=utf-8\n\n");
//	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n");
	TS_LOGIN_FAILED
	memset(&net_info, 0, sizeof(net_info));

	data = getenv("QUERY_STRING");
	if (data == NULL) {
	} else {
		//get value
		cgiFormString("net_name", net_info.name, sizeof(net_info.name));
		if ((NULL == net_info.name) || (strlen(net_info.name) <= 0)) {
			fprintf(cgiOut, "Check code!");
			return EXIT_FAILURE;
		}
		db_web_get_host_net(net_info.name, &net_info);

		//127.0.0.1/cgi-bin/set_ip_port.html?ip=192.168.10.247&isdhcp=1&name=eth0&type=1&gw=192.168.10.1&net_mask=255.255.255.0&ssid=&login_passwd=panda
		//get value
		cgiFormString("ip", net_info.ip, sizeof(net_info.ip));
		int32_t is_dhcp = -1;
		cgiFormInteger("is_dhcp_value", &is_dhcp, -1);
		net_info.is_dhcp = is_dhcp;

		ts_is_running(TS_MONITOR_NAME, &is_running);

		cgiFormInteger("type", &net_info.type, TS_WEB_NET_LINE);
		cgiFormString("gw", net_info.gw, sizeof(net_info.gw));

		cgiFormString("net_mask", net_info.netmask, sizeof(net_info.netmask));
		cgiFormString("ssid", net_info.ssid, sizeof(net_info.ssid));
		cgiFormString("login_passwd", net_info.login_passwd, sizeof(net_info.login_passwd));
		cgiFormString("dns", net_info.dns, sizeof(net_info.dns));
	}

	upd_net_info(net_info.name, is_running);

	//close db
	rc = sc_close();
	if (rc != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();
	return rc;
}

