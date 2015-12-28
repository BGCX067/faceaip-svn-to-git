/*
 ============================================================================
 Name        : tshome_web.c
 Author      : pandaFly
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

/**
 * get so:
 * gcc -O -fpic -shared -o ts_db_tools.so *.o
 * gcc -o tshome_web tshome_web.c -L .  ./ts_db_tools.so -I /home/pangt/project/TSHome/trunk/TSHome-Host/5.Coding/common_files/ -lsqlite3
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "common/list.h"

//#ifndef TS_FILE_DB
//#define TS_FILE_DB
//#endif

#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include "db/ts_db_protocol_infos.h"
#include "db/ts_db_conn_infos.h"
#include "common/remote_def.h"
#include "netinet/in.h"

int64_t htonll(int64_t v)
{
	union
	{
		int32_t lv[2];
		int64_t llv;
	} u;
	u.lv[0] = htonl(v >> 32);
	u.lv[1] = htonl(v & 0xFFFFFFFFULL);
	return u.llv;
}

int64_t ntohll(int64_t v)
{
	union
	{
		int32_t lv[2];
		int64_t llv;
	} u;
	u.llv = v;
	return ((int64_t) ntohl(u.lv[0]) << 32) | (int64_t) ntohl(u.lv[1]);
}

static unsigned long long swap64(unsigned long long _x)
{
	/* this is __bswap64 from:
	 * $FreeBSD: src/sys/i386/include/endian.h,v 1.41$
	 */
	return ((_x >> 56) | ((_x >> 40) & 0xff00) | ((_x >> 24) & 0xff0000) | ((_x >> 8) & 0xff000000) | ((_x << 8) & ((unsigned long long) 0xff << 32)) | ((_x << 24) & ((unsigned long long) 0xff << 40)) | ((_x << 40) & ((unsigned long long) 0xff << 48)) | ((_x << 56)));
}

unsigned long long hton64(const unsigned long long ho)
{
	if (ntohs(0x1234) == 0x1234)
		return ho;
	else
		return swap64(ho);
}

unsigned long long ntoh64(const unsigned long long no)
{
	return hton64(no);
}

/**
 * get all interface informations
 */
int get_host_interfaces()
{
	int rc = EXIT_SUCCESS;
	//get interface info
	LIST_HEAD(list_head);
	ts_host_interface_list_t * host_interfaces_list = NULL;

	rc = db_get_host_interfaces(&list_head, host_interfaces_list);

	if (rc != SQLITE_OK) {
		return EXIT_FAILURE;
	}
	//Traversal list
	ts_host_interface_list_t * p_tmp = NULL;
	list_for_each_entry_safe(host_interfaces_list,p_tmp,&list_head,list) {
		printf("%d\t", host_interfaces_list->host_interface.id);
		printf("%s\t", host_interfaces_list->host_interface.desc);
		printf("%d\t", host_interfaces_list->host_interface.type);
		printf("%s\t", host_interfaces_list->host_interface.info);
		printf("\n");
	}
	printf("\n");
	//	printf("size = %d\n", sizeof(host_interfaces_list));
	return rc;
}

/**
 * get_all_passways
 */
int get_all_passageway()
{
	int rc = EXIT_SUCCESS;
	//get interface info
	LIST_HEAD(list_head);
	ts_passageway_list_t * passageways = NULL;
	rc = db_get_all_lines(&list_head, passageways);
	if (rc != SQLITE_OK) {
		return EXIT_FAILURE;
	}
	//Traversal list
	ts_passageway_list_t * p_tmp = NULL;
	list_for_each_entry_safe(passageways,p_tmp,&list_head,list) {
		printf("interface:\t");
		printf("%d\t", passageways->passageway.host_interface.id);
		printf("%s\t", passageways->passageway.host_interface.desc);
		printf("%d\t", passageways->passageway.host_interface.type);
		printf("%s\t", passageways->passageway.host_interface.info);
		printf("\n");
//		printf("aaaa\n");
		if ((passageways->passageway.host_interface.type == TS_HOST_INTERFACE_TYPE_IP)) {
			if (NULL != passageways->passageway.gateway_list) {
				ts_gateway_list_t * p_tmp_gateway = NULL;
//				ts_gateway_list_t * p_tmp_gateway_1 = NULL;
				list_for_each_entry_safe(passageways->passageway.gateway_list,p_tmp_gateway,&passageways->passageway.gateway_heead,list) {
//				list_for_each_entry_safe(p_tmp_gateway_1,p_tmp_gateway,&passageways->passageway.gateway_list->list,list) {
					printf("\tGateways:\t");
					printf("%d\t", passageways->passageway.gateway_list->gateway.id);
					printf("%s\t", passageways->passageway.gateway_list->gateway.gatewayIP);
					printf("%d\t", passageways->passageway.gateway_list->gateway.gatewayPort);
					printf("%s\t", passageways->passageway.gateway_list->gateway.description);
					printf("%d\t", passageways->passageway.gateway_list->gateway.getewayType);
					printf("\n");
				}
			}
		}
	}
	printf("\n");
	return rc;
}

int get_serial_info()
{
	int rc = EXIT_SUCCESS;
	char * file_name;
	file_name = (char *) calloc(30, sizeof(char));
	char *comm_port;
	comm_port = (char *) calloc(30, sizeof(char));
	int device_id = 5;
	int interface_id = 0;
	int gateway_id = 0;
	int zone = 0;
	rc = db_get_serial_device_info(device_id, &interface_id, &gateway_id, &zone, file_name, comm_port);
	if (EXIT_SUCCESS == rc) {
		printf("interface_id = %d,gateway_id = %d,zone = %d,file_name : %s,comm_port : %s\n", interface_id, gateway_id, zone, file_name, comm_port);
	} else {
		printf("err %d : ", rc);
	}
	return rc;
}

int get_knx_group()
{
	int rc = EXIT_SUCCESS;
	LIST_HEAD(list_head);
	ts_knx_control_info_list_t * knx_list = NULL;
	int gateway_id = 0;
	int permission = 1;
	rc = db_get_knx_groups(gateway_id, &list_head, knx_list, permission);
	if (EXIT_SUCCESS == rc) {
		ts_knx_control_info_list_t * p_tmp = NULL;
		list_for_each_entry_safe (knx_list,p_tmp,&list_head,list) {
			printf("cmd_type = %d , ", knx_list->knx_info.cmd_type);
			printf("data_type = %d , ", knx_list->knx_info.data_type);
			printf("device_type = %d , ", knx_list->knx_info.device_type);
			printf("home_device_id = %d", knx_list->knx_info.home_device_id);
			printf("group_addr = %d , ", knx_list->knx_info.group_addr);
			printf("time = %s , ", knx_list->knx_info.time);
			printf("data = %s , ", knx_list->knx_info.data);
			printf("data_len = %d , ", knx_list->knx_info.data_len);
			printf("\n");
		}
	} else {
		printf("get error : error code : %d", rc);
	}
	//	rc = db_knx_get(&list_head, knx_list, 1);
	return rc;
}

int del_gateway()
{
	int rc = EXIT_SUCCESS;
	rc = db_protocol_del_gateway_of_line(7);
	if (EXIT_SUCCESS != rc) {
		printf("delete passageway error : %d", rc);
	}
	return rc;
}

int upd_gateway()
{
	int rc = EXIT_SUCCESS;
	rc = db_protocol_modify_gateway_of_line(1, "192.168.1.999", 3600);
	if (EXIT_SUCCESS != rc) {
		printf("delete passageway error : %d", rc);
	}

//	int a = 0;
//	scanf("%d", &a);
//	sqlite3 * tmp_curr_db = sc_get_db_conn(TS_DB_TYPE_FILE);
//	sc_load_or_save_db(tmp_curr_db, "db/tshome.db", TS_DB_MEMORY_2_FILE);

	return rc;
}

int get_host_info()
{
	int rc = EXIT_SUCCESS;
	char *user_name = (char *) calloc(32, sizeof(char));
	char *user_pwd = (char *) calloc(32, sizeof(char));
	rc = db_get_host_user_info(user_name, user_pwd);
	if (EXIT_SUCCESS != rc) {
		printf("get device permission error : %d", rc);
	} else {
		printf("name = %s\tpasswd = %s\n", user_name, user_pwd);
	}
	return rc;
}

int get_software_info()
{
	int rc = EXIT_SUCCESS;
	char *time = (char *) calloc(20, sizeof(char));
	rc = db_get_lastest_version_software(time);
	if (EXIT_SUCCESS != rc) {
		printf("get device permission error : %d", rc);
	} else {
		printf("Time = %s\n", time);
	}
	return rc;
}

int get_conf_info()
{
	int rc = EXIT_SUCCESS;
//	char *time = (char *) calloc(20, sizeof(char));
	unsigned long long time = 0;
	rc = db_get_lastest_version_conf(&time);
//	time = 4294967296;
//	time = 281474976710656;
//	time = 73849001;
	time = 13737010000;
	if (EXIT_SUCCESS != rc) {
		printf("get device permission error : %d", rc);
	} else {
		unsigned long long ihton64 = hton64(time);
		unsigned long long intoh64 = ntoh64(ihton64);
		printf("Time   = %lld,hton64 : %lld,ntoh : %lld\n\n", time, ihton64, intoh64);
		printf("Time   = %lld,hton64 : %lld,ntoh : %lld\n", time, hton64(time), ntoh64(hton64(time)));
		printf("ntohll : %lld,htonll : %lld\n", ntohll(time), htonll(ntohll(time)));
		printf("htonll : %lld,ntohll : %lld\n", htonll(time), ntohll(htonll(time)));
	}

	/*
	 unsigned long long a=0x123456789abcde;
	 unsigned long long b=0xdebc9a78563412;
	 unsigned long long c = ntoh64(b);
	 unsigned long long d = hton64(a);
	 unsigned long long e = hton64(d);


	 //	printf("time = %lld\n ",time);
	 printf ("x : ntoh64=%llx\n", c);
	 printf ("x : hton64=%llx\n", d);
	 printf ("x : e=%llx\n", e);


	 printf ("ntoh64=%lld\n", c);
	 printf ("hton64=%lld\n", d);
	 */
	return rc;
}
////////////////////////////////

int32_t validate_user1()
{
	int32_t ret = EXIT_SUCCESS;
	ts_host_base_user_info user_info;
	strcpy(user_info.user_name, "admin");
	strcpy(user_info.passwd, "admin");
	int8_t pass = TS_WEB_VALIDATE_FALSE;
	db_web_validate_user(user_info, &pass);

	if (TS_WEB_VALIDATE_FALSE == pass) {
		ts_db_tools_log_error("login success");
	} else {
		ts_db_tools_log_error("login failed");
	}

	return ret;
}

int32_t set_user_info()
{
	int32_t ret = EXIT_SUCCESS;
	//设置初始值
	ts_host_base_user_info base_info;
	strcpy(base_info.user_name, "admin");
	strcpy(base_info.passwd, "admin");
	strcpy(base_info.ip, "192.168.10.111");
	base_info.port = 45;
	base_info.is_first_use = TS_IS_FIRST_USE;
	//设置数据库
	db_set_base_user_info(base_info);
	return ret;
}

int32_t get_all_serial_infos()
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
	//Traversal list
	ts_host_interface_serial_info_list * p_tmp = NULL;
	list_for_each_entry_safe(serial_infos,p_tmp,&list_head,list) {
		printf("%d\t", serial_infos->host_serial.id);
		printf("%d\t", serial_infos->host_serial.interface_id);
		printf("%d\t", serial_infos->host_serial.baud_rate);
		printf("%d\t", serial_infos->host_serial.data_bit);
		printf("%f\t", serial_infos->host_serial.stop_bit);
		printf("%d\t", serial_infos->host_serial.parity_check);
		printf("%d\t", serial_infos->host_serial.comm_id);
		printf("%s\t", serial_infos->host_serial.comm_port);
		printf("\n");
		free(serial_infos);
	}
	printf("\n");
	//	printf("size = %d\n", sizeof(host_interfaces_list));
	return ret;
}

int32_t get_host_serial_by_id()
{
	int32_t ret = EXIT_SUCCESS;
	ts_host_interface_serial_info host_serial_info;

	host_serial_info.id = 1;
	ret = db_get_serial_details(&host_serial_info);
	if (ret != EXIT_SUCCESS) {
		return ret;
	}

	printf("%d\t", host_serial_info.id);
	printf("%d\t", host_serial_info.interface_id);
	printf("%d\t", host_serial_info.baud_rate);
	printf("%d\t", host_serial_info.data_bit);
	printf("%f\t", host_serial_info.stop_bit);
	printf("%d\t", host_serial_info.parity_check);
	printf("%d\t", host_serial_info.comm_id);
	printf("%s\t", host_serial_info.comm_port);
	printf("\n");

	return ret;
}

int32_t set_host_serial_info()
{
	int32_t ret = EXIT_SUCCESS;
	ts_host_interface_serial_info host_serial_info;

	host_serial_info.id = 3;
	host_serial_info.baud_rate = 55;
	host_serial_info.comm_id = 552;
	strcpy(host_serial_info.comm_port, "ccc");
	host_serial_info.data_bit = 553;
	host_serial_info.interface_id = 6;
	host_serial_info.parity_check = 555;
	host_serial_info.stop_bit = 9.6;

	ret = db_set_serial_details(host_serial_info);
	if (ret != EXIT_SUCCESS) {
		return ret;
	}
	return ret;
}

int32_t del_host_serial_info()
{
	int32_t ret = EXIT_SUCCESS;
	ts_host_interface_serial_info host_serial_info;

	host_serial_info.id = 3;
	host_serial_info.interface_id = 8;

	ret = db_del_serial_details(host_serial_info);
	if (ret != EXIT_SUCCESS) {
		return ret;
	}
	return ret;
}

int32_t get_infr_devices()
{
	int32_t ret = EXIT_SUCCESS;

	//get interface info
	LIST_HEAD(list_head);
	ts_infrared_device_list * infr_devices = NULL;
	int counts = 0;
	ret = db_web_get_infr_device_infos(&list_head, infr_devices, &counts);
	if (ret != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	//Traversal list
	ts_infrared_device_list * p_tmp = NULL;
	list_for_each_entry_safe(infr_devices,p_tmp,&list_head,list) {
		printf("%d\t", infr_devices->infr_device_info.id);
		printf("%s\t", infr_devices->infr_device_info.device_name);
		printf("%d\t", infr_devices->infr_device_info.channel);
		printf("\n");
		free(infr_devices);
	}
	return ret;
}

int32_t set_infr_devices()
{
	int32_t ret = EXIT_SUCCESS;
	ts_infrared_device infr_info;

	infr_info.id = 1;
	infr_info.channel = 4;

	ret = db_web_set_infr_device_info(infr_info);
	if (ret != EXIT_SUCCESS) {
		return ret;
	}
	return ret;
}

int32_t get_all_gateways()
{
	int32_t rc = EXIT_SUCCESS;
	//get interface info
	LIST_HEAD(list_head);

	//Traversal list
	ts_gateway_list_t * gateways = NULL;
	int32_t counts = 0;
	rc = db_web_get_all_gateways(&list_head, gateways, &counts);
	if (rc != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	ts_gateway_list_t * p_tmp = NULL;
	list_for_each_entry_safe(gateways,p_tmp,&list_head,list) {
		printf("\tGateways:\t");
		printf("%d\t", gateways->gateway.id);
		printf("%s\t", gateways->gateway.gatewayIP);
		printf("%d\t", gateways->gateway.gatewayPort);
		printf("%s\t", gateways->gateway.gateway_type_name);
		printf("\n");
	}
	printf("\n");
	return rc;
}

int32_t get_all_passways()
{
	int32_t rc = EXIT_SUCCESS;
	//get interface info
	LIST_HEAD(list_head);
	ts_passageway_list_t * passageways = NULL;
	int32_t counts = 0;
	rc = db_get_all_lines_counts(&list_head, passageways, &counts);
//	rc = db_get_all_lines(&list_head, passageways);
	if (rc != SQLITE_OK) {
		return EXIT_FAILURE;
	}
	//Traversal list
	ts_passageway_list_t * p_tmp = NULL;
	list_for_each_entry_safe(passageways,p_tmp,&list_head,list) {
		printf("interface:\t");
		printf("%d\t", passageways->passageway.host_interface.id);
		printf("%s\t", passageways->passageway.host_interface.desc);
		printf("%d\t", passageways->passageway.host_interface.type);
		printf("%s\t", passageways->passageway.host_interface.info);
		printf("%s\t", passageways->passageway.host_interface.type_name);
		printf("%d\t", passageways->passageway.gateway_counts);
		printf("\n");
		//		printf("aaaa\n");
		if ((passageways->passageway.host_interface.type == TS_HOST_INTERFACE_TYPE_IP)) {
			if (NULL != passageways->passageway.gateway_list) {
				ts_gateway_list_t * p_tmp_gateway = NULL;
				//				ts_gateway_list_t * p_tmp_gateway_1 = NULL;
				list_for_each_entry_safe(passageways->passageway.gateway_list,p_tmp_gateway,&passageways->passageway.gateway_heead,list) {
					//				list_for_each_entry_safe(p_tmp_gateway_1,p_tmp_gateway,&passageways->passageway.gateway_list->list,list) {
					printf("\tGateways:\t");
					printf("%d\t", passageways->passageway.gateway_list->gateway.id);
					printf("%s\t", passageways->passageway.gateway_list->gateway.gatewayIP);
					printf("%d\t", passageways->passageway.gateway_list->gateway.gatewayPort);
					printf("%s\t", passageways->passageway.gateway_list->gateway.description);
					printf("%d\t", passageways->passageway.gateway_list->gateway.getewayType);
					printf("\n");
				}
			}
		}
	}
	printf("\n");
	return rc;
}

int32_t set_host_interface()
{
	int32_t ret = EXIT_SUCCESS;

	ts_host_interface_t host_interface;
	const char* desc = "panda";
	strcpy((char *) host_interface.desc, desc);
	host_interface.id = 36;
	const char* info = "192.168.10.222";
	strcpy((char *) host_interface.info, info);
	host_interface.type = TS_HOST_INTERFACE_TYPE_IP;

	ret = db_web_set_host_interface(host_interface);
	if (ret != EXIT_SUCCESS) {
		return ret;
	}
	return ret;
}

int32_t del_host_interface()
{
	int32_t ret = EXIT_SUCCESS;

	ret = db_web_del_host_interface(8);
	if (ret != EXIT_SUCCESS) {
		return ret;
	}
	return ret;
}

int32_t insert_gateway_interface()
{
	int32_t ret = EXIT_SUCCESS;

	ret = db_web_insert_host_inter_gateway(8, 1);
	ret = db_web_insert_host_inter_gateway(8, 2);
	if (ret != EXIT_SUCCESS) {
		return ret;
	}
	return ret;
}

int32_t del_gateway_interface()
{
	int32_t ret = EXIT_SUCCESS;

	ret = db_web_del_host_inter_gateway(8);
	if (ret != EXIT_SUCCESS) {
		return ret;
	}
	return ret;
}

int32_t get_serial_device_infos()
{
	int32_t ret = EXIT_SUCCESS;

	//get interface info
	LIST_HEAD(list_head);

	//Traversal list
	ts_web_host_device_info_list * devices = NULL;
	int32_t counts = 0;
	ret = db_web_get_host_device_infos(&list_head, devices, &counts);
	if (ret != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	ts_web_host_device_info_list * p_tmp = NULL;
	list_for_each_entry_safe(devices,p_tmp,&list_head,list) {
		printf("%d\t", devices->dev_info.id);
		printf("%s\t", devices->dev_info.device_name);
		printf("%s\t", devices->dev_info.room_name);
		printf("%s\t", devices->dev_info.host_info);
		printf("\n");
	}
	if (ret != EXIT_SUCCESS) {
		return ret;
	}
	return ret;
}

int32_t db_web_set_serial_device()
{
	int32_t ret = EXIT_SUCCESS;

	ts_web_host_serial_device_info info;
	strcpy(info.host_info, "comm1");
	info.id = 5;
	ret = db_web_set_host_device_info(info);
	if (ret != EXIT_SUCCESS) {
		return ret;
	}
	return ret;
}

int32_t xml_parse()
{
	int32_t ret = EXIT_SUCCESS;

	//xml操作
	const char * file_name = "/opt/a.xml";
	ret = ts_db_tools_parse_xml(file_name);
	fprintf(stderr, "4--->\n");

	return ret;
}

int32_t get_home_id()
{
	int32_t ret = EXIT_SUCCESS;

	int32_t home_id = 0;
	db_get_home_id(&home_id);
	fprintf(stderr, "home id = %d\n", home_id);

	return ret;
}

int32_t get_host_user_info()
{
	int32_t ret = EXIT_SUCCESS;
	char * name = (char*) calloc(31, sizeof(char));
	char * passwd = (char*) calloc(31, sizeof(char));
	db_get_host_user_info(name, passwd);
	fprintf(stderr, "name : %s\tpasswd  : %s\n", name, passwd);
	free(name);
	free(passwd);
	return ret;
}

int get_conf()
{
	int32_t ret = EXIT_SUCCESS;

	int len = TS_TIME_LEN;
//	char *version = (char *) calloc(len, sizeof(char));
	unsigned long long version = 0;
//	printf("--------------->\n");
	db_get_lastest_version_conf(&version);
	int i = 0;
	for (i = 0; i < len; i++) {
		printf("%lld,", version);
	}
	printf("\n");

	return ret;
}

int get_is_stop()
{
	int32_t ret = EXIT_SUCCESS;

	int32_t is_stop = TS_STOP_NET;
	db_get_is_stop_net(&is_stop);
	printf("is stop = %d\n", is_stop);

	return ret;
}

int32_t get_knx_data_type()
{
	int32_t ret = EXIT_SUCCESS;
	ts_knx_control_info_t knx_info;
	knx_info.group_addr = 2563;
	int32_t gateway_id = 4;
	db_get_knx_group_type(gateway_id, &knx_info);
	printf("data type = %d,dev_id = %d\n", knx_info.data_type, knx_info.home_device_id);

	return ret;
}

int32_t get_gateway_type()
{
	int32_t ret = EXIT_SUCCESS;
	int32_t gatewayType = TS_GATEWAY_TYPE_WIFI;
	db_get_gateway_by_id(18, &gatewayType);
	printf("data type = %d\n", gatewayType);

	return ret;
}

int32_t get_next_group()
{
	int32_t ret = EXIT_SUCCESS;
	int32_t n_group_id = 2;
	int32_t gateway_id = 0;
	int16_t gateway_addr = 0;
//	char gateway_addr[32];
	db_get_next_readable_group(&n_group_id, &gateway_id, &gateway_addr);
	printf("%d\t%d\t%d\n", n_group_id, gateway_id, gateway_addr);

	return ret;
}

int32_t set_stop_net()
{
	int32_t ret = EXIT_SUCCESS;

	int32_t is_stop = TS_NOT_STOP_NET;
	db_set_is_stop_net(is_stop);

	return ret;
}

int32_t validate_user()
{
	int32_t ret = EXIT_SUCCESS;

	int32_t is_pass = TS_IS_USER_PASS_FALSE;
	char* user_name = "myhome";
	char * pwd = "00491373785F8469E5B9B28355FFF73E21E751A7CC990F30E473ED3F";
	db_user_validate(user_name, pwd, &is_pass);
	printf("is pass = %d\n", is_pass);

	return ret;
}

int32_t get_pwd_by_name()
{
	int32_t ret = EXIT_SUCCESS;
	char* user_name = "tslite";
	char pwd[57] = { 0 };
	db_get_passwd_by_name(user_name, pwd);
	printf("pwd = %s\n", pwd);

	return ret;
}

int32_t get_serial_text()
{
	int32_t ret = EXIT_SUCCESS;

	return ret;
}

int32_t get_myself()
{
	int32_t ret = EXIT_SUCCESS;
	int32_t who_am_i = TS_PRO_TYPE_GATEWAY;
	db_get_who_am_i(&who_am_i);
	printf("who am i = %d\n", who_am_i);

	return ret;
}

int32_t infr_study()
{
	int32_t ret = EXIT_SUCCESS;

	int32_t gateway_id = 2;
//	int32_t len = 288;
	int32_t len = 5;
	char text[] = { 1, 11, 111, 0, 12 };
	//char text[] = { 1, 0, 191, 10, 0, 0, 92, 3, 1, 0, 219, 1, 0, 0, 97, 3, 1, 0, 215, 1, 0, 0, 161, 1, 1, 0, 215, 1, 0, 0, 157, 1, 1, 0, 95, 5, 0, 0, 35, 5, 1, 0, 154, 3, 0, 0, 96, 3, 1, 0, 214, 1, 0, 0, 161, 1, 1, 0, 214, 1, 0, 0, 157, 1, 1, 0, 155, 3, 0, 0, 161, 1, 1, 0, 214, 1, 0, 0, 97, 3, 1, 0, 218, 1, 0, 0, 157, 1, 1, 0, 214, 1, 0, 0, 161, 1, 1, 0, 155, 3, 0, 0, 95, 3, 1, 0, 154, 3, 0, 0, 161, 1, 1, 0, 215, 1, 0, 0, 97, 3, 1, 0, 219, 1, 0, 0, 156, 1, 1, 0, 215, 1, 0, 0, 80, 70, 0, 0, 80, 70, 0, 0, 80, 70, 0, 0, 80, 70, 0, 0, 76, 48, 1, 0, 192, 10, 0, 0, 94, 3, 1, 0, 214, 1, 0, 0, 97, 3, 1, 0, 214, 1, 0, 0, 161, 1, 1, 0, 214, 1, 0, 0, 161, 1, 1, 0, 33, 5, 0, 0, 33, 5, 1, 0, 155, 3, 0, 0, 97, 3, 1, 0, 219, 1, 0, 0, 156, 1, 1, 0, 216, 1, 0, 0, 159, 1, 1, 0, 155, 3, 0, 0, 157, 1, 1, 0, 218, 1, 0, 0, 93, 3, 1, 0, 219, 1, 0, 0, 157, 1, 1, 0, 218, 1, 0, 0, 157, 1, 1, 0, 155, 3, 0, 0, 97, 3, 1, 0, 155, 3, 0, 0, 157, 1, 1, 0, 214, 1, 0, 0, 97, 3, 1, 0, 220, 1, 0, 0, 156, 1, 1, 0, 218, 1, 0, 0, 0, 0 };
	int32_t key_code = 1;
	int32_t type = TS_INFRCODE_TYPE_1;

	db_set_infrared_code(gateway_id, key_code, text, type, len);

	return ret;
}

int32_t get_infr_study()
{
	int32_t ret = EXIT_SUCCESS;
	int32_t gateway_id = 2;
	int32_t len = 0;
	char text[1536] = { 0 };
	int32_t type = TS_INFRCODE_TYPE_1;
	int32_t key_code = 1;

	ret = db_get_infrared_code(gateway_id, key_code, text, type, &len);
//	printf("ret = %d\n", ret);

	int i = 0;
	printf("len = %d,\ntext = ", len);
	for (i = 0; i < len; i++) {
		printf("%d,", text[i]);
	}
	printf("\n");

	return ret;
}

int32_t get_serial_key_text()
{
	int32_t ret = EXIT_SUCCESS;
	int32_t key_id = 4;
	int32_t len = 0;
	char text[255] = { 0 };

	db_get_serial_datagram(0, key_id, text, &len);

	int i = 0;
	printf("len = %d\ntext = ", len);
	for (i = 0; i < len; i++) {
		printf("%d,", text[i]);
	}
	printf("\n");

	return ret;
}

int32_t get_set_serial_device_curr()
{
	int32_t ret = EXIT_SUCCESS;

	int device_id = 2;
	db_set_serial_device_curr(device_id, 2, 5, "123456");

	int device_id_get = 2;
	char s_curr[255] = { 0 };
	int state = 0;
	int zone = TS_ZONE_UNUSED;

	db_get_serial_device_curr(device_id_get, &state, &zone, s_curr);
	fprintf(stderr, "%s,state = %d,zone = %d\n", s_curr, state, zone);

	return ret;
}

int32_t get_serial_device_type()
{
	int32_t ret = EXIT_SUCCESS;

	int device_id = 4;
	int32_t type = 1;
	db_get_serial_device_type_by_id(&type, device_id);
	fprintf(stderr, "type = %d\n", type);

	return ret;
}

int32_t get_device_type()
{
	int32_t ret = EXIT_SUCCESS;

	int device_id = 8;
	int32_t type = 1;
	db_get_device_type_by_id(&type, device_id);
	fprintf(stderr, "type = %d\n", type);

	return ret;
}

int32_t get_serial_device_info()
{
	int32_t ret = EXIT_SUCCESS;

	int comm_id = 1;
	int dev_type = 0;
	char file_name[63] = { 0 };
	db_get_devtype_and_protrol(comm_id, file_name, &dev_type);
	fprintf(stderr, "file = %s,dev_type = %d\n", file_name, dev_type);

	return ret;
}

int32_t get_serial_file_name()
{
	int32_t ret = EXIT_SUCCESS;

	int device_id = 1;
	int comm_id = 0;
	int gateway_id = 0;
	int zone = 0;
	char file_name[64] = { 0 };
	char comm_port[64] = { 0 };

	db_get_serial_device_info(device_id, &comm_id, &gateway_id, &zone, file_name, comm_port);
	fprintf(stderr, "file = %s,comm_id = %d,gateway_id = %d,zone = %d,comm = %s\n", file_name, comm_id, gateway_id, zone, comm_port);

	return ret;
}

int32_t get_dev_gateway()
{
	int32_t ret = EXIT_SUCCESS;
	int32_t device_id = 56;
	int32_t type = 0;
	int32_t gateway_id = 0;
	db_get_interface_gateway(device_id, &type, &gateway_id);

	printf("type = %d\t gateway = %d\n", type, gateway_id);
	return ret;
}

int get_infrared_channel()
{
	int rc = EXIT_SUCCESS;
	int channel_id = 0;
	int gateway_id = 0;
	int interface_id = 0;
	int device_id = 1;
	rc = db_get_infrared_device_info(device_id, &interface_id, &gateway_id, &channel_id);
	if (EXIT_SUCCESS == rc) {
		printf("channel id = %d\tgateway_id = %d,interface_id = %d\n", channel_id, gateway_id, interface_id);
	}
	return rc;
}

int32_t ins_login()
{
	int32_t ret = EXIT_SUCCESS;

	char ip[] = "192.168.1.1";
//	ret = ts_web_login_add(ip);

//	ret = ts_web_login_del(ip);

	int is_pass = TS_WEB_VALIDATE_FALSE;
	ret = ts_web_login_validate(ip, &is_pass);
	fprintf(stderr, "%d\n", is_pass);

	return ret;
}

int get_all_scene_inputs()
{
	int32_t ret = EXIT_SUCCESS;
//
//	int16_t a = 0;
//	printf("size : %d\n",sizeof(ret));

	ts_scene_io_info_t *s_io = NULL;
	int count = 0;
	db_get_all_scene_inputs(1, &count, &s_io);
	int i = 0;
	for (i = 0; i < count; i++) {
		printf("i = %d,p = %p,dev_id = %d,param = %d,value = %s,th = %f\n", i, &s_io[i], s_io[i].dev_id, s_io[i].param, s_io[i].value, s_io[i].threshold);
	}
	free(s_io);
	return ret;
}

int32_t get_all_scene_infos()
{
	int32_t ret = EXIT_SUCCESS;

	//get interface info
	LIST_HEAD(list_head);
	ts_scene_io_info_list_t * scene_infos = NULL;
	int scene_id = 1;
	ret = db_get_scene_infos(&list_head, scene_infos, scene_id);
	if (ret != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	//Traversal list
	ts_scene_io_info_list_t * p_tmp = NULL;
	list_for_each_entry_safe(scene_infos,p_tmp,&list_head,list) {
		printf("%d,%d,%s,%d\n", scene_infos->scene_info.dev_id, scene_infos->scene_info.param, scene_infos->scene_info.value, scene_infos->scene_info.delay);
	}
	printf("\n");
	//	printf("size = %d\n", sizeof(host_interfaces_list));
	return ret;
}

int32_t get_scene_ids()
{
	int32_t ret = EXIT_SUCCESS;
	ts_scene_io_info_t scene_io_info;
	scene_io_info.dev_id = 2;
	scene_io_info.param = 2305;
	strcpy(scene_io_info.value, "1");

	ts_scene_info_t *scene_info;
	int counts = 0;
	db_get_scene_id(&scene_io_info, &counts, &scene_info, TS_SCENE_IS_USE);
	int i = 0;
//	printf("counts = %d\n",counts);
	for (i = 0; i < counts; i++) {
		printf("id = %d,is_use = %d,and_or = %d,th = %f\n", scene_info[i].id, scene_info[i].is_use, scene_info[i].and_or, scene_io_info.threshold);
	}

	return ret;
}

int32_t set_knx_curr_value()
{
	int32_t ret = EXIT_SUCCESS;
	ts_knx_control_info_t knx_control_info;
	int32_t gateway_id = 1;
	knx_control_info.group_addr = 2581;
	strcpy((char *) knx_control_info.time, "1");
	strcpy((char *) knx_control_info.data, "333");
	ret = db_upd_knx_device_curr_value(knx_control_info, gateway_id);

	return ret;
}

int32_t get_knx_curr_value()
{
	int32_t ret = EXIT_SUCCESS;
	ts_knx_control_info_t knx_control_info;
	knx_control_info.group_addr = 2581;
	knx_control_info.home_device_id = 5;
	ret = db_get_knx_device_curr_value(&knx_control_info);

	printf("time = %s,currdData = %s,dataType = %d\n", knx_control_info.time, knx_control_info.data, knx_control_info.data_type);

	return ret;
}

int add_gateway_of_line()
{
	int rc = EXIT_SUCCESS;
	rc = db_protocol_add_gateway_of_line(4, 2);
	if (EXIT_SUCCESS != rc) {
		printf("insert passageway error : %d\n", rc);
	}
	return rc;
}

int32_t whether_device_control()
{
	int32_t ret = EXIT_SUCCESS;

	int32_t dev_id = 1;
//	int32_t user_id = 1;
	const char * user_name = "2";
	int32_t is_control = TS_DEVICE_PERMISSION_FALSE;

	db_is_device_control_by_name((char *) user_name, dev_id, &is_control);
	printf("%d\n", is_control);

	return ret;
}

int32_t whether_system_control()
{
	int32_t ret = EXIT_SUCCESS;

	int32_t system_id = 3;
//	int32_t user_id = 1;
	const char * user_name = "2";
	int32_t is_control = TS_SYSTEM_PERMISSION_FALSE;

	db_is_system_control_by_name((char *) user_name, system_id, &is_control);
	printf("%d\n", is_control);

	return ret;
}

int32_t get_key_func_name()
{
	int32_t ret = EXIT_SUCCESS;

	int32_t id = 32;
	char name[33] = { 0 };
	db_get_key_func_name(id, name);
	printf("%s\n", name);

	return ret;
}

int32_t get_base_info()
{
	int32_t ret = EXIT_SUCCESS;
	ts_host_base_user_info host_base_info;
	ret = db_get_base_user_info(&host_base_info);
	fprintf(stderr, "%s,%d\n", host_base_info.ip, host_base_info.is_first_use);

	return ret;

}

int32_t get_max_user()
{
	int32_t ret = EXIT_SUCCESS;
	int users = 0;
	ret = db_get_max_users(&users);
	fprintf(stderr, "%d\n", users);

	return ret;

}

int32_t hex_string()
{
	int ret = EXIT_SUCCESS;

	char base[16] = { 0 };
	strcpy(base, "panda");
	char s[33] = { 0 };
	hex_2_string(base, 16, s);
	printf("string = %s\n", s);

	char hex[16] = { 0 };
//	string_2_hex(s,hex);
	string_2_hex("70616E64610000000000000000000000", hex);
	printf("hex2 = %s\n", hex);

	return ret;
}

int get_set_is_local()
{
	int ret = EXIT_SUCCESS;

	int is_local = 0;
	db_get_is_local(&is_local);
	printf("is local : %d\n", is_local);

	is_local = 1;
	db_set_is_local(is_local);

	return ret;
}

int reset_serial_curr()
{
	int ret = EXIT_SUCCESS;

	ret = db_reset_serial_dev_curr();
	return ret;
}

int get_host_net()
{
	int ret = EXIT_SUCCESS;
	ts_host_net_info net_info;
	strcpy(net_info.gw, "192.168.10.1");
	strcpy(net_info.ip, "1.1.1.1");
	net_info.is_dhcp = 0;
	net_info.port = 123;
	net_info.is_dhcp = 0;
	net_info.type = 1;
	strcpy(net_info.login_passwd, "as");
	strcpy(net_info.netmask, "255.255.255.0");
	strcpy(net_info.ssid, "nts-1");
	strcpy(net_info.name, "eth1");
	db_web_set_host_net(net_info);

	db_web_get_host_net("eth6", &net_info);
//	printf("ip = %s\n,port = %d\n,netmask = %s\n,gw = %s\n,is_dhcp = %d\n,name = %s\n,ssid= %s\n,passwd = %s\n", net_info.ip, net_info.port, net_info.netmask, net_info.gw, net_info.is_dhcp, net_info.name, net_info.ssid, net_info.login_passwd);

	return ret;
}

int get_devs_by_dp()
{
	int ret = EXIT_SUCCESS;

	int gateway_id = 0;
	int dp = 2;
	int gatewayType = 0;
	int subType = 0;
	int dev_id = 0;

	db_get_device_infos_by_dp(dp, &dev_id, &subType, &gateway_id, &gatewayType);
	printf("gatewayId : %d,devid %d,subType = %d,gatewayType = %d\n", gateway_id, dev_id, subType, gatewayType);

	return ret;
}

int get_ct_by_dp()
{
	int ret = EXIT_SUCCESS;

	int ct = 0;
	int dp = 2;

	db_get_ctrltype_by_dp(dp, &ct);
	printf("dp = %d,ct = %d\n", dp, ct);

	return ret;
}

int get_e_id_by_dp()
{
	int ret = EXIT_SUCCESS;

	unsigned int e_id = 0;
	int rocker = 0;
	int dp = 1;

	db_get_enocean_id_by_dp(dp, &e_id, &rocker);
	printf("e_id = %d,rocker = %d\n", e_id, rocker);

	return ret;
}

int get_dp_by_e_id()
{
	int ret = EXIT_SUCCESS;
	int e_id = 16877725;

	LIST_HEAD(list_head);
	ts_dps_list_t * dps = NULL;

	db_get_dp_by_enocean_id(e_id, &list_head, dps);

	//Traversal list
	ts_dps_list_t * p_tmp = NULL;
	list_for_each_entry_safe(dps,p_tmp,&list_head,list) {
		printf("%d\t", dps->dp_info.dp_id);
		printf("%d\t", dps->dp_info.ctrl_type);
		printf("\n");
	}
	printf("\n");
	return ret;
}

int get_step_times_by_dp()
{
	int ret = EXIT_SUCCESS;

	int dp = 1;
	int max_times = 0;
	int max_steps = 0;

	ret = db_get_dp_infos_by_dp(dp, &max_steps, &max_times);
	printf("ret = %d,max_times = %d,max_steps = %d\n", ret, max_times, max_times);

	return ret;
}

int get_dp_curr()
{
	int ret = EXIT_SUCCESS;
	int dp = 2;
	char curr[64] = { 0 };
	ret = db_get_dp_curr_value(dp, curr);
	printf("curr = %s\n", curr);
	return ret;
}

int set_dp_curr()
{
	int ret = EXIT_SUCCESS;
	int dp = 2;
	char curr[64] = "panda";
	ret = db_set_dp_curr_value(dp, curr);
	return ret;
}

int get_same_dps_by_dp()
{
	int ret = EXIT_SUCCESS;

	int dp = 1;

	LIST_HEAD(list_head);
	ts_dps_list_t * dps = NULL;

	db_get_dps_by_same_dev(dp, &list_head, dps);

	//Traversal list
	ts_dps_list_t * p_tmp = NULL;
	list_for_each_entry_safe(dps,p_tmp,&list_head,list) {
		printf("%d\t", dps->dp_info.dp_id);
		printf("%d\t", dps->dp_info.ctrl_type);
		printf("\n");
	}

	return ret;
}

int get_comm_by_enocean()
{
	int ret = EXIT_SUCCESS;
	int e_id = 418;
	ts_host_interface_serial_info host_serial_info;
	bzero(&host_serial_info, sizeof(host_serial_info));
	ret = db_get_comm_by_enocean_id(e_id, &host_serial_info);
	printf("%d\t", host_serial_info.id);
	printf("%d\t", host_serial_info.interface_id);
	printf("%d\t", host_serial_info.baud_rate);
	printf("%d\t", host_serial_info.data_bit);
	printf("%f\t", host_serial_info.stop_bit);
	printf("%d\t", host_serial_info.parity_check);
	printf("%d\t", host_serial_info.comm_id);
	printf("%s\t", host_serial_info.comm_port);
	printf("\n");

	return ret;
}

int get_set_ir_driver()
{
	int32_t ret = EXIT_SUCCESS;

	char driver_name[128] = { 0 };
	db_get_infr_driver(driver_name);
	printf("IR : %s\n",driver_name);

	db_set_infr_driver("dd");

	return ret;
}

int main(void)
{
//		puts("panda"); /* prints panda */
	int ret = EXIT_SUCCESS;
//	ret = sc_db_init(TS_DB_FILE_NAME);
//	const char * file_name = "../../../output/db/tshome.db";

//	const char * file_name = "db/tshome.db";

//	const char * file_name = "/opt/tshome-host/db/tshome.db";
//	const char * file_name = "/opt/tshome-host/tshome.db";
//	ret = sc_db_init(file_name);
//	ret = sc_db_init_by_type(file_name,TS_DB_TYPE_MEMORY);
	if (ret != SQLITE_OK) {
		ret = EXIT_FAILURE;
		return ret;
	}

//	get_host_interfaces();
//	get_all_passageway();
//	get_infrared_channel();
//	get_serial_info();
//	get_knx_group();
//	upd_gateway();
//	del_gateway();
//	get_dev_gateway();

//	conn/////////////////////////
//		get host info
//	get_host_info();
//	get_software_info();
//	get_conf_info();
//	whether_device_control();
//	whether_system_control();
//	get_pwd_by_name();

//new//////////////////////////////
//	get_dev_gateway();
//	validate_user1();
//	set_user_info();
//	get_all_serial_infos();
//	get_host_serial_by_id();
//	set_host_serial_info();
//	del_host_serial_info();
//	set_infr_devices();
//	get_infr_devices();
//	get_infrared_channel();
//	get_all_gateways();
//	get_all_passways();
//	set_host_interface();
//	del_host_interface();
//	insert_gateway_interface();
//	del_gateway_interface();
//	get_serial_device_info();
//	db_web_set_serial_device();
//	xml_parse();
//	get_home_id();
//	get_host_user_info();
//	get_conf();
//	set_stop_net();
//	get_is_stop();
//	get_knx_data_type();
//	get_gateway_type();
//	get_next_group();
//	validate_user();
//	get_serial_device_infos();

//串口报文
//	get_serial_text();
//	while (true) {
//		get_myself();
//		sleep(1);
//	}

//	infr_study();
//	get_infr_study();
//	get_serial_key_text();
//	get_set_serial_device_curr();
//	get_serial_device_type();
//	get_device_type();
//	get_serial_device_info();
//	get_serial_file_name();
//	ins_login();

//	get_all_scene_infos();
//	get_scene_ids();
//	set_knx_curr_value();
//	get_knx_curr_value();
//	add_gateway_of_line();
//	get_key_func_name();

//	get_base_info();
//	set_user_info();
//	get_max_user();
//	hex_string();
//	get_set_is_local();

//get_all_scene_inputs();
//	reset_serial_curr();

//	get_host_net();

//	get_devs_by_dp();

//	get_ct_by_dp();

//	get_e_id_by_dp();

//	get_dp_by_e_id();

//	get_step_times_by_dp();

//	set_dp_curr();
//	get_dp_curr();

//	get_same_dps_by_dp();

//	get_comm_by_enocean();

	get_set_ir_driver();

//close db
	ret = sc_close();
	if (ret != SQLITE_OK) {
		ret = EXIT_FAILURE;
		return ret;
	}
//	fprintf(stderr,"999->\n");

	return ret;
}

