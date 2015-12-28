/*
 * set_serial_info.c
 *
 *  Created on: 2012-3-28
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <cgic.h>
#include <string.h>

#include "db/sqlite3_ctrl.h"
#include "db/ts_db_web_infos.h"
#include "log/ts_log_tools.h"
#include "db/ts_db_tools.h"

int32_t del_passways(char * ids)
{
	int32_t ret = EXIT_SUCCESS;
	char *buf;
	char *infos;
	buf = ids;
	int32_t id = 0;
	while ((infos = strsep(&buf, ",")) != NULL) {
		id = atoi(infos);
		db_web_del_host_interface(id);
	}
	return ret;
}

int32_t upd_passageway_info(char * passageway_infos)
{
	int32_t ret = EXIT_SUCCESS;
	char *buf;
	char *infos;
	buf = passageway_infos;
//	log_debug_web( "%s\n", passageway_infos);

	while ((infos = strsep(&buf, ";")) != NULL) {
		char * buf_details;
		char * details;
		buf_details = infos;
		int32_t i = 0;
		ts_host_interface_t host_info;

		while ((details = strsep(&buf_details, "|")) != NULL) {
			if (0 == i) {
				host_info.id = atoi(details);
			} else if (1 == i) { //set infos like ip
				strcpy((char *) host_info.info, details);
			} else if (2 == i) { //set desc
				strcpy((char *) host_info.desc, details);
			} else if (3 == i) { //set the gateways which connect with the interface
				db_web_del_host_inter_gateway(host_info.id);
				if ((NULL != details) && (strlen(details) > 0)) {
					char * gateway_details;
					while ((gateway_details = strsep(&details, ",")) != NULL) {
						if ((NULL != gateway_details) && (strlen(gateway_details) > 0)) {
							db_web_insert_host_inter_gateway(host_info.id, atoi(gateway_details));
						}
					}
				}
			}
			i++;
			host_info.type = TS_HOST_INTERFACE_TYPE_IP;
		}
		db_web_set_host_interface(host_info);
	}
	return ret;
}

int32_t cgiMain(void)
{
	int ret = EXIT_SUCCESS;
	log_open("set_passway.c");

	char * data;
	int32_t del_len = 0;
	int32_t info_len = 0;
	char * s_dels = NULL;
	char * s_infos = NULL;

	fprintf(cgiOut, "Content-Type:text/html;charset=utf-8\n\n");
//	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\" />\n");
//	printf("<meta http-equiv=\"Refresh\" content=\"0;URL=count.cgi\">");

	data = getenv("QUERY_STRING");
	if (data == NULL) {

	} else {
//		log_debug_web( "data = %s\n", data);
		//dels
		cgiFormInteger("del_len", &del_len, 0);
//		log_debug_web( "del len = %d\n", del_len);
		if (del_len > 0) {
			s_dels = (char *) calloc(del_len, sizeof(char));
			cgiFormString("del_ids", s_dels, del_len);
			del_passways(s_dels);
		}

		//infos
		cgiFormInteger("info_len", &info_len, 0);
//		log_debug_web( "info_len = %d\n", info_len);
		if (info_len > 0) {
			s_infos = (char *) calloc(info_len, sizeof(char));
			cgiFormString("infos", s_infos, info_len);
			upd_passageway_info(s_infos);
			free(s_infos);
		}
	}

//close db
	ret = sc_close();
	if (ret != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();
	return ret;
}

