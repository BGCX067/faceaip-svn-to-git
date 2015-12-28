/*
 * tstools.h
 *
 *  Created on: 2012-3-24
 *      Author: root
 */

#ifndef TSTOOLS_H_
#define TSTOOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sqlite3.h>

#include "common/remote_def.h"
#include "db/ts_db_conn_infos.h"

#define TS_WEB_LOG_FILE "log_xml/web_log.xml"

#define TS_WEB_ON SWITCH_ON
#define TS_WEB_OFF SWITCH_OFF

#define TS_WEB_STRING_ON "OPEN"
#define TS_WEB_STRING_OFF "CLOSE"

#define TS_WEB_ON_OFF_NO 0

#define TS_WEB_AJAX_BASE_FUNCTION fprintf(cgiOut, "function parseResponse(request) {\n");\
	fprintf(cgiOut, "	if (request.readyState == 4) {\n");\
	fprintf(cgiOut, "		if (request.status == 200 || request.status == 304) {\n");\
	fprintf(cgiOut, "		}\n");\
	fprintf(cgiOut, "	}\n");\
	fprintf(cgiOut, "}\n");\
	fprintf(cgiOut, "\n");\
	fprintf(cgiOut, "function getHTTPObject() {\n");\
	fprintf(cgiOut, "	var xhr = false;\n");\
	fprintf(cgiOut, "	if (window.XMLHttpRequest) {\n");\
	fprintf(cgiOut, "		xhr = new XMLHttpRequest();\n");\
	fprintf(cgiOut, "	} else if (window.ActiveXObject) {\n");\
	fprintf(cgiOut, "		try {\n");\
	fprintf(cgiOut, "			xhr = new ActiveXObject(\"Msxml2.XMLHTTP\");\n");\
	fprintf(cgiOut, "		} catch (e) {\n");\
	fprintf(cgiOut, "			try {\n");\
	fprintf(cgiOut, "				xhr = new ActiveXObject(\"Microsoft.XMLHTTP\");\n");\
	fprintf(cgiOut, "			} catch (e) {\n");\
	fprintf(cgiOut, "				xhr = false;\n");\
	fprintf(cgiOut, "			}\n");\
	fprintf(cgiOut, "		}\n");\
	fprintf(cgiOut, "	}\n");\
	fprintf(cgiOut, "	return xhr;\n");\
	fprintf(cgiOut, "}\n");\


//on off
#define TS_WEB_ON_OFF fprintf(cgiOut, "function on_off(data) {\n");\
	fprintf(cgiOut, "	var request = getHTTPObject();\n");\
	fprintf(cgiOut, "	if (request) {\n");\
	fprintf(cgiOut, "		request.onreadystatechange = function() {\n");\
	fprintf(cgiOut, "			parseResponse(request);\n");\
	fprintf(cgiOut, "		};\n");\
	fprintf(cgiOut, "		$.ajax({\n");\
	fprintf(cgiOut, "			url : \"../cgi-bin/on_off.html\",\n");\
	fprintf(cgiOut, "			type : 'Post',\n");\
	fprintf(cgiOut, "			contentType : \"application/x-www-form-urlencoded; charset=utf-8\",\n");\
	fprintf(cgiOut, "			async : false,\n");\
	fprintf(cgiOut, "			data : data,\n");\
	fprintf(cgiOut, "			timeout : 6000,\n");\
	fprintf(cgiOut, "			success : function(page_data) {\n");\
	fprintf(cgiOut, "				if( (page_data.length > 0)&&(page_data.length < 25)) {\n");\
	fprintf(cgiOut, "					alert(page_data);\n");\
	fprintf(cgiOut, "				}else {;\n");\
	fprintf(cgiOut, "				}\n");\
	fprintf(cgiOut, "			},\n");\
	fprintf(cgiOut, "			failed : function(page_data) {\n");\
	fprintf(cgiOut, "				alert(page_data);\n");\
	fprintf(cgiOut, "			}\n");\
	fprintf(cgiOut, "		});\n");\
	fprintf(cgiOut, "		location.replace(location);\n");\
	fprintf(cgiOut, "		return true;\n");\
	fprintf(cgiOut, "	} else {\n");\
	fprintf(cgiOut, "		return false;\n");\
	fprintf(cgiOut, "	}\n");\
	fprintf(cgiOut, "}\n");\
	fprintf(cgiOut, "\n");\

//	fprintf(cgiOut, "		request.open(\"POST\", \"../cgi-bin/on_off.html\", true);\n");
//	fprintf(cgiOut, "		request.setRequestHeader(\"Content-Type\",\n");
//	fprintf(cgiOut, "				\"application/x-www-form-urlencoded\");\n");
//	fprintf(cgiOut, "		request.send(data);\n");

//web save
#define TS_WEB_SAVE(function_name,page_name) fprintf(cgiOut, "function %s(data) {\n",function_name);\
	fprintf(cgiOut, "	var request = getHTTPObject();\n");\
	fprintf(cgiOut, "		if (window.ActiveXObject) {\n");\
	fprintf(cgiOut, "		request = new ActiveXObject('Microsoft.XMLHTTP');\n");\
	fprintf(cgiOut, "	} else if (window.XMLHttpRequest) {\n");\
	fprintf(cgiOut, "		request = new XMLHttpRequest();\n");\
	fprintf(cgiOut, "	}\n");\
	fprintf(cgiOut, "	if (request) {\n");\
	fprintf(cgiOut, "		request.onreadystatechange = function() {\n");\
	fprintf(cgiOut, "			parseResponse(request);\n");\
	fprintf(cgiOut, "		};\n");\
	fprintf(cgiOut, "	 	$.ajax({\n");\
	fprintf(cgiOut, "			 url : \"../cgi-bin/%s.html\",\n",page_name);\
	fprintf(cgiOut, "			 type : 'Post',\n");\
	fprintf(cgiOut, "			 contentType : \"application/x-www-form-urlencoded; charset=utf-8\",\n");\
	fprintf(cgiOut, "			 async : false,\n");\
	fprintf(cgiOut, "			 data : data,\n");\
	fprintf(cgiOut, "			 timeout : 6,\n");\
	fprintf(cgiOut, "			 success : function(data) {\n");\
	fprintf(cgiOut, "				 location.replace(location);\n");\
	fprintf(cgiOut, "			 },\n");\
	fprintf(cgiOut, "			 failed : function(data) {\n");\
	fprintf(cgiOut, "		 		return true;\n");\
	fprintf(cgiOut, "			 }\n");\
	fprintf(cgiOut, "		 });\n");\
	fprintf(cgiOut, "		return true;\n");\
	fprintf(cgiOut, "	} else {\n");\
	fprintf(cgiOut, "		return false;\n");\
	fprintf(cgiOut, "	}\n");\
	fprintf(cgiOut, "}\n");\
	fprintf(cgiOut, "\n");\

#define TS_WEB_MENUS fprintf(cgiOut, "				<!--导航 start-->\n");\
	fprintf(cgiOut, "				<div class=\"navigation\">\n");\
	fprintf(cgiOut, "					<ul>\n");\
	fprintf(cgiOut, "						<li class=\"selected\"><a href=\"basic_control.html\">基本控制</a></li>\n");\
	fprintf(cgiOut, "						<li><a href=\"home_serial_port.html\">串口参数</a></li>\n");\
	fprintf(cgiOut, "						<li><a href=\"pathway_info.html\">网关通路</a></li>\n");\
	fprintf(cgiOut, "						<li><a href=\"device_link_access.html\">串口设备通路</a></li>\n");\
	fprintf(cgiOut, "						<li><a href=\"infrared_device_pathway.html\">红外设备通路</a></li>\n");\
	fprintf(cgiOut, "						<li><a href=\"log.html\">日志信息</a></li>\n");\
	fprintf(cgiOut, "					</ul>\n");\
	fprintf(cgiOut, "				</div>\n");\
	fprintf(cgiOut, "				<!--导航 end-->\n");\

#ifdef __cplusplus
extern "C"
{
#endif

int32_t ts_db_tools_log_error(const char * log);

int32_t ts_db_tools_upd_software(const char * file_name);

int32_t ts_db_tools_parse_xml(const char * DIR);

void start_pro();

void stop_pro();

#ifdef __cplusplus
}
#endif

#endif /* TSTOOLS_H_ */

