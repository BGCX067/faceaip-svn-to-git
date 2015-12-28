#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include "db/sqlite3_ctrl.h"
#include "db/ts_db_protocol_infos.h"
#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include "common/list.h"
#include "log/ts_log_tools.h"
//#include "dev_ctrl/Conversion.h"

char * all_gateway_ids = NULL;
int32_t is_exits = TS_NO_RUNNING;

#define PATHWAY_ON_OFF fprintf(cgiOut, "	var request = getHTTPObject();\n");\
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
//		else{
//			all_gateway_ids += strlen(s_int) + 1;
//			*all_gateway_ids = ' ';
//		}

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
//	log_debug_web("3------>\n");

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
//				log_debug_web( "%d------>\n", passageways->passageway.gateway_list->gateway.id);
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

//	log_debug_web( "	parameter=[%s]\n", bak_infos);
	fprintf(cgiOut, "	parameter=[%s]\n", bak_infos);
	free(bak_infos);
	free(s_tmp);
	return rc;
}

int cgiMain()
{
	int ret = EXIT_SUCCESS;

	log_open("pathway_info.c");

	int32_t page_type = TS_PAGE_TYPE_ID_NOTYPE;
	char * data;
	data = getenv("QUERY_STRING");
	if (data == NULL) {

	} else {
		cgiFormInteger(TS_PAGE_TYPE, &page_type, TS_PAGE_TYPE_ID_NOTYPE);
	}

	ts_is_running(TS_MONITOR_NAME, &is_exits);

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
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/jquery-1.7.1.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/ts_set_base_info.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\">\n");

	//del ids
	fprintf(cgiOut, "	var del_ids='';\n");

	TS_WEB_AJAX_BASE_FUNCTION
	//开关
	fprintf(cgiOut, "	/**\n");
	fprintf(cgiOut, " *网关通路\n");
	fprintf(cgiOut, " */\n");
	fprintf(cgiOut, "//求数组的最大值\n");
	fprintf(cgiOut, "Array.prototype.max = function() {  \n");
	fprintf(cgiOut, " var max = this[0];\n");
	fprintf(cgiOut, " var len = this.length; \n");
	fprintf(cgiOut, " for (var i = 1; i < len; i++){\n");
	fprintf(cgiOut, "  if (parseInt(this[i]) > max) { \n");
	fprintf(cgiOut, "   max = this[i];   \n");
	fprintf(cgiOut, "  } \n");
	fprintf(cgiOut, " }   \n");
	fprintf(cgiOut, " return max;\n");
	fprintf(cgiOut, "} \n");
	fprintf(cgiOut, "var gateway_no=0;//声明网关开关名编号\n");
	fprintf(cgiOut, "//网关\n");
	fprintf(cgiOut, "function gateway_list(parameter_gateway){\n");
	fprintf(cgiOut, "	if(parameter_gateway != null){\n");

	fprintf(cgiOut, "		for(i=0;i<parameter_gateway.length;i++){\n");

	fprintf(cgiOut, "			var id=parameter_gateway[i].id;\n");
//	fprintf(cgiOut, "			alert(id+\",\"+parameter_gateway.length)\n");
	fprintf(cgiOut, "			var ip=parameter_gateway[i].ip;\n");
	fprintf(cgiOut, "			var port=parameter_gateway[i].port;\n");
	fprintf(cgiOut, "			var type=parameter_gateway[i].type;\n");
	fprintf(cgiOut, "			var on_off=parameter_gateway[i].on_off;\n");

	fprintf(cgiOut, "			if(%d==on_off) {\n", SWITCH_ON);
	fprintf(cgiOut, "				$(\".gateway_info_list table.table_serial_port\").append('<tr align=\"center\"><td width=\"41\"><span  id=\"ts_id\" class=\"gateway_id\">'+id+'</span></td><td width=\"298\">'+ip+'</td><td width=\"153\">'+port+'</td><td width=\"110\"><span class=\"gateway_type\">'+type+'</span></td><td width=\"110\"><input  id=\"switch_on\" name=\"gateway_'+(++gateway_no)+'\" class=\"gateway_'+gateway_no+'\" type=\"radio\" value=\"\" checked /> 连接 <input id=\"switch_off\" name=\"gateway_'+gateway_no+'\" type=\"radio\" value=\"\"/> 断开</td></tr>');\n");
	fprintf(cgiOut, "			} else {\n");
	fprintf(cgiOut, "				$(\".gateway_info_list table.table_serial_port\").append('<tr align=\"center\"><td width=\"41\"><span  id=\"ts_id\" class=\"gateway_id\">'+id+'</span></td><td width=\"298\">'+ip+'</td><td width=\"153\">'+port+'</td><td width=\"110\"><span class=\"gateway_type\">'+type+'</span></td><td width=\"110\"><input  id=\"switch_on\" name=\"gateway_'+(++gateway_no)+'\" class=\"gateway_'+gateway_no+'\" type=\"radio\" value=\"\" /> 连接 <input id=\"switch_off\" name=\"gateway_'+gateway_no+'\" type=\"radio\" value=\"\" checked/> 断开</td></tr>');\n");
	fprintf(cgiOut, "			} \n");

	fprintf(cgiOut, "			//switch_on\n");
	fprintf(cgiOut, "			$(\".gateway_\"+gateway_no).parent().parent().find(\"input#switch_on\").click(function(){				\n");
	fprintf(cgiOut, "				var id=$(this).parent().parent().find(\"span#ts_id\").html();\n");
//	fprintf(cgiOut, "				on_off('type=%d&value='+id+'&on_off=%d');\n", TS_WEB_ON_OFF_GATEWAY, TS_WEB_ON);
//	fprintf(cgiOut, "				alert('on' + id);\n");
	fprintf(cgiOut, "				var data = 'type=%d&value='+id+'&on_off=%d';\n", TS_WEB_ON_OFF_GATEWAY, TS_WEB_ON);
	PATHWAY_ON_OFF
	fprintf(cgiOut, "			});\n");
	fprintf(cgiOut, "			//switch_close\n");
	fprintf(cgiOut, "			$(\".gateway_\"+gateway_no).parent().parent().find(\"input#switch_off\").click(function(){				\n");
	fprintf(cgiOut, "				var id=$(this).parent().parent().find(\"span#ts_id\").html();\n");
//	fprintf(cgiOut, "				on_off('type=%d&value='+id+'&on_off=%d');\n", TS_WEB_ON_OFF_GATEWAY, TS_WEB_OFF);
//	fprintf(cgiOut, "				alert('close' + id);\n");
	fprintf(cgiOut, "				var data = 'type=%d&value='+id+'&on_off=%d';\n", TS_WEB_ON_OFF_GATEWAY, TS_WEB_OFF);
	PATHWAY_ON_OFF
	fprintf(cgiOut, "			});\n");
//	TS_WEB_ON_OFF
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");
//	fprintf(cgiOut, "			//switch_on\n");
////	fprintf(cgiOut, "			$(\".gateway_\"+gateway_no).parent().parent().find(\"input#switch_on\").click(function(){				\n");
//	fprintf(cgiOut, "			$(\".gateway_info_list table.table_serial_port\").parent().parent().find(\"input#switch_on\").click(function(){				\n");
//	fprintf(cgiOut, "				var id=$(this).parent().parent().find(\"span#ts_id\").html();\n");
////	fprintf(cgiOut, "				on_off('type=%d&value='+id+'&on_off=%d');\n", TS_WEB_ON_OFF_GATEWAY, TS_WEB_ON);
//	fprintf(cgiOut, "				alert('on' + id);\n");
//	fprintf(cgiOut, "			});\n");
//	fprintf(cgiOut, "			//switch_close\n");
//	fprintf(cgiOut, "			$(\".gateway_\"+gateway_no).parent().parent().find(\"input#switch_off\").click(function(){				\n");
//	fprintf(cgiOut, "				var id=$(this).parent().parent().find(\"span#ts_id\").html();\n");
//	fprintf(cgiOut, "				on_off('type=%d&value='+id+'&on_off=%d');\n", TS_WEB_ON_OFF_GATEWAY, TS_WEB_OFF);
//	fprintf(cgiOut, "				alert('close' + id);\n");
//	fprintf(cgiOut, "			});\n");
//	TS_WEB_ON_OFF

	fprintf(cgiOut, "\n");
	fprintf(cgiOut, "$(\".homeport_delete_\"+homeport_delete_no).parent().parent().find(\"input#switch_on\").click(function(){\n");
	fprintf(cgiOut, "$(\"#save_btn\").show();\n");
	fprintf(cgiOut, "//var id=$(this).parent().parent().parent().find(\"span.infrared_device_id\").html();\n");
	fprintf(cgiOut, "	var id=$(this).parent().parent().parent().find(\"span#ts_id\").html();\n");
	fprintf(cgiOut, "	alert('open' + id);\n");
	fprintf(cgiOut, "});\n");
	fprintf(cgiOut, "\n");
	fprintf(cgiOut, "\n");
	fprintf(cgiOut, "//从数据库取出数据，列出已有列表\n");
	fprintf(cgiOut, "window.onload = function (){\n");
//	fprintf(cgiOut,"	var parameter_gateway=[{'id':'1','ip':'192.168.0.1','port':'12','type':'KNX'},{'id':'2','ip':'192.168.0.1','port':'12','type':'KNX'}];\n");
	fprintf(cgiOut, "	var parameter_gateway;");
	get_gateways();
	fprintf(cgiOut, "	gateway_list(parameter_gateway);//网关\n");

//interface
	fprintf(cgiOut, "	var parameter;");
//fprintf(cgiOut, "	parameter=[{'id':'1','note':'192.168.10.10:68','description':'手势操作让用户的操作空间不再局限于界','gateway':'1,2,','gateway_select':['1','2','3','4','5']},{'id':'2','note':'192.168.10.10:68','description':'手势操作让用户的操作空间不再局限于界','gateway':'3,','gateway_select':['1','2','3','4','5']}];\n");
	get_all_passways();
	fprintf(cgiOut, "	homeport_list(parameter);//网卡	\n");
	fprintf(cgiOut, "}\n");

	fprintf(cgiOut, "//网卡\n");
	fprintf(cgiOut, "var homeport_delete_no=0;//声明网卡删除按钮编号\n");
	fprintf(cgiOut, "//网卡修改确定删除操作方法\n");
	fprintf(cgiOut, "function homeport_option(){\n");
	fprintf(cgiOut, "	//下拉多选\n");
	fprintf(cgiOut, "	$(\".homeport_delete_\"+homeport_delete_no).parent().parent().find(\"span.homeport_gateway_select\").mouseover(function(){\n");
	fprintf(cgiOut, "		$(this).find(\".homeport_gateway_second\").show();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	$(\".homeport_delete_\"+homeport_delete_no).parent().parent().find(\"span.homeport_gateway_select\").mouseout(function(){\n");
	fprintf(cgiOut, "		$(this).find(\".homeport_gateway_second\").hide();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	$(\".homeport_delete_\"+homeport_delete_no).parent().parent().find(\"select.homeport_gateway_second\").click(function(){\n");
	fprintf(cgiOut, "		var selected_value=\"\";				\n");
	fprintf(cgiOut, "		var select_value=\"\";\n");
	fprintf(cgiOut, "		$(\"span.home_port_gateway\").each(function(i,val){\n");
	fprintf(cgiOut, "			if($(this).css(\"display\")==\"block\"){\n");
	fprintf(cgiOut, "				selected_value=selected_value+$(this).html();\n");
	fprintf(cgiOut, "			}\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		$(this).find(\"option\").each(function(i,val){\n");
	fprintf(cgiOut, "				if($(this).attr(\"selected\")==\"selected\"){\n");
//	fprintf(cgiOut, "					if(selected_value.indexOf($(this).html()+\",\")>=0){\n");
	fprintf(cgiOut, "					if(isUsed(selected_value,$(this).html()) == 0){\n");
//	fprintf(cgiOut, "						alert(selected_value,\"----\",$(this).html(),\"=====\",selected_value.indexOf($(this).html()));\n");
	fprintf(cgiOut, "						$(this).removeAttr(\"selected\");\n");
	fprintf(cgiOut, "					}else{\n");
	fprintf(cgiOut, "						select_value=select_value+$(this).html()+\",\";\n");
	fprintf(cgiOut, "					}\n");
	fprintf(cgiOut, "				}\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		$(this).parent().find(\"input\").val(select_value);\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//修改\n");
	fprintf(cgiOut, "	$(\".homeport_delete_\"+homeport_delete_no).parent().parent().find(\"input.revise_btn\").click(function(){\n");
	fprintf(cgiOut, "		document.getElementById(\"save_btn\").style.display=\"none\";\n");
	fprintf(cgiOut, "		//显示隐藏\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.home_port_note\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.home_port_description\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.home_port_gateway\").hide();\n");
	fprintf(cgiOut, "		$(this).hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.home_port_note_revise\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.home_port_description_revise\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.homeport_gateway_select\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.ok_btn\").show();\n");
	fprintf(cgiOut, "		//赋值\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.home_port_note_revise\").val($(this).parent().parent().find(\"span.home_port_note\").html());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.home_port_description_revise\").val($(this).parent().parent().find(\"span.home_port_description\").html());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.homeport_gateway_select input\").val($(this).parent().parent().find(\"span.home_port_gateway\").html());\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//确定\n");
	fprintf(cgiOut, "	$(\".homeport_delete_\"+homeport_delete_no).parent().parent().find(\"input.ok_btn\").click(function(){\n");
	fprintf(cgiOut, "		$(\"#save_btn\").show();\n");
	fprintf(cgiOut, "		$(\"#new_add_btn\").show();\n");
	fprintf(cgiOut, "		//显示隐藏\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.home_port_note_revise\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.home_port_description_revise\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.homeport_gateway_select\").hide();\n");
	fprintf(cgiOut, "		$(this).hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.home_port_note\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.home_port_description\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.home_port_gateway\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.revise_btn\").show();\n");
	fprintf(cgiOut, "		//赋值\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.home_port_note\").html($(this).parent().parent().find(\"input.home_port_note_revise\").val());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.home_port_description\").html($(this).parent().parent().find(\"input.home_port_description_revise\").val());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.home_port_gateway\").html($(this).parent().parent().find(\"span.homeport_gateway_select input\").val());\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//删除\n");
	fprintf(cgiOut, "	$(\".homeport_delete_\"+homeport_delete_no).click(function(){\n");
	fprintf(cgiOut, "		$(\"#save_btn\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().remove();\n");
	fprintf(cgiOut, "		//id\n");
	fprintf(cgiOut, "		var id=$(this).parent().parent().find(\"span.home_port_id\").html();\n");
	fprintf(cgiOut, "		del_ids += id+','\n");
	fprintf(cgiOut, "	});\n");
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
	fprintf(cgiOut, "			$(\".home_port_list table.table_serial_port\").append('<tr align=\"center\"><td width=\"41\"><span class=\"home_port_id\">'+id+'</span></td><td width=\"186\"><span class=\"home_port_note\">'+note+'</span><input type=\"text\" class=\"home_port_note_revise\" style=\"display:none\"/></td><td width=\"292\"><span class=\"home_port_description\">'+description+'</span><input type=\"text\" class=\"home_port_description_revise\" style=\"display:none\"/></td><td width=\"110\"><span class=\"home_port_gateway\">'+gateway+'</span><span class=\"homeport_gateway_select\" style=\"display:none\"><input type=\"button\" value=\"\"/><select class=\"homeport_gateway_second\" size=\"5\" multiple=\"multiple\" style=\"display:none;z-index:'+(9999-(++homeport_delete_no))+'\"></select></span></td><td width=\"83\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input id=\"ts_del\" type=\"button\" class=\"delete_btn homeport_delete_'+homeport_delete_no+'\"/></td></tr>');\n");
	fprintf(cgiOut, "			//下拉列表生成\n");
	fprintf(cgiOut, "			for(j=0;j<gateway_select.length;j++){\n");
	fprintf(cgiOut, "				$(\".homeport_delete_\"+homeport_delete_no).parent().parent().find(\"select.homeport_gateway_second\").append('<option>'+gateway_select[j]+'</option>');\n");
	fprintf(cgiOut, "			}\n");
	fprintf(cgiOut, "			//调用操作方法\n");
	fprintf(cgiOut, "			homeport_option();\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "$(function(){\n");
	fprintf(cgiOut, "	//新增按钮\n");
	fprintf(cgiOut, "	$(\"input.new_add_btn\").click(function(){\n");
	fprintf(cgiOut, "		document.getElementById(\"new_add_btn\").style.display=\"none\";\n");
	fprintf(cgiOut, "		//id号为最大值加1\n");
	fprintf(cgiOut, "		var home_port_id;\n");
	fprintf(cgiOut, "		var num=[];\n");
	fprintf(cgiOut, "		$(\".home_port_list table.table_serial_port span.home_port_id\").each(function(i,val){\n");
	fprintf(cgiOut, "			num[i]=$(this).html();\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		if(num!=null){\n");
	fprintf(cgiOut, "			home_port_id=parseInt(num.max())+1;\n");
	fprintf(cgiOut, "		}else{\n");
	fprintf(cgiOut, "			home_port_id=1;\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "		if(num == \"\"){\n");
	fprintf(cgiOut, "			home_port_id=1;\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "\n");
	fprintf(cgiOut, "		$(\".home_port_list table.table_serial_port\").append('<tr align=\"center\"><td width=\"41\"><span class=\"home_port_id\">'+home_port_id+'</span></td><td width=\"186\"><span class=\"home_port_note\" style=\"display:none\"></span><input type=\"text\" class=\"home_port_note_revise\"/></td><td width=\"292\"><span class=\"home_port_description\" style=\"display:none\"></span><input type=\"text\" class=\"home_port_description_revise\"/></td><td width=\"110\"><span class=\"home_port_gateway\" style=\"display:none\"></span><span class=\"homeport_gateway_select\"><input type=\"text\" value=\"\"/><select class=\"homeport_gateway_second\" size=\"5\" multiple=\"multiple\" style=\"display:none;z-index:'+(9999-(++homeport_delete_no))+'\"></select></span></td><td width=\"83\"><input type=\"button\" class=\"revise_btn\" style=\"display:none\"/><input type=\"button\" class=\"ok_btn\"/><input type=\"button\" class=\"delete_btn homeport_delete_'+homeport_delete_no+'\"/></td></tr>');\n");
	fprintf(cgiOut, "		var parameter ;\n");
	fprintf(cgiOut, "		parameter = [{'id':'1','note':'192.168.10.10:68','description':'手势操作让用户的操作空间不再局限于界','gateway':'1,2,','gateway_select':[%s]},];\n", all_gateway_ids);

	fprintf(cgiOut, "		var gateway_select=parameter[0].gateway_select;\n");
	fprintf(cgiOut, "		//下拉列表生成\n");
	fprintf(cgiOut, "		for(j=0;j<gateway_select.length;j++){\n");
	fprintf(cgiOut, "			$(\".homeport_delete_\"+homeport_delete_no).parent().parent().find(\"select.homeport_gateway_second\").append('<option>'+gateway_select[j]+'</option>');\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "		//调用操作方法\n");
	fprintf(cgiOut, "		homeport_option();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//保存按钮\n");
	fprintf(cgiOut, "	$(\"input.save_btn\").click(function(){\n");
	fprintf(cgiOut, "		document.getElementById(\"save_btn\").style.display=\"none\";\n");
	fprintf(cgiOut, "		//列表的保存\n");

	fprintf(cgiOut, "		var is_can_save = 1;\n");
	fprintf(cgiOut, "		var home_port_save=[];\n");
	fprintf(cgiOut, "		var infos = '';\n");
	fprintf(cgiOut, "		$(\".home_port_list table.table_serial_port span.home_port_id\").each(function(i,val){\n");
	fprintf(cgiOut, "			var id=$(this).html();//id\n");
	fprintf(cgiOut, "			var note=$(this).parent().parent().find(\"span.home_port_note\").html();\n");
	//validate ip
	fprintf(cgiOut, "			if(is_can_save == 1) {");
	fprintf(cgiOut, "				is_can_save = checkIP(note);\n");
	fprintf(cgiOut, "			}\n");

	fprintf(cgiOut, "			var description=$(this).parent().parent().find(\"span.home_port_description\").html();\n");
	fprintf(cgiOut, "			var gateway=$(this).parent().parent().find(\"span.home_port_gateway\").html();\n");
	//because gateway spilt with "," ,others spilt with "|"

	fprintf(cgiOut, "			if(is_can_save == 1) {");
	fprintf(cgiOut, "				is_can_save =  is_validate(description);\n");
	fprintf(cgiOut, "			}\n");
	fprintf(cgiOut, "			infos+=id+'|'+note+'|'+description+'|'+gateway+';';\n");
	//fprintf(cgiOut, "			home_port_save[i]={'id':id,'note':note,'description':description,'gateway':gateway};\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		if(is_can_save == 1) {\n");
	fprintf(cgiOut, "			var del_len = del_ids.length;\n");
	fprintf(cgiOut, "			var info_len = infos.length;\n");
	fprintf(cgiOut, "			var sendAll = 'del_len='+del_len+'&info_len='+info_len+'&del_ids='+del_ids+'&infos='+infos;\n");
	fprintf(cgiOut, "			set_passageway(sendAll);\n");
	if (TS_PAGE_TYPE_ID_GUIDE == page_type) {
		fprintf(cgiOut, "		jumpPage('%s%s')\n", "../cgi-bin/device_link_access.html?", TS_PAGE_GUIDE);
	}
	fprintf(cgiOut, "		} \n");
//	fprintf(cgiOut, "		else {\n");
//	fprintf(cgiOut, "			alert('error') \n");
//	fprintf(cgiOut, "		} \n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "});\n");

	//保存
	TS_WEB_SAVE("set_passageway", "set_passageway");

	fprintf(cgiOut, "</script>\n");
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

	//guide
	fprintf(cgiOut, "				<!--导航 start-->\n");
	if (TS_PAGE_TYPE_ID_GUIDE == page_type) {
		fprintf(cgiOut, "<script type=\"text/javascript\">print_left_guide(%d)</script>\n", TS_LEFT_GUIDE_GUIDE);
	} else {
		fprintf(cgiOut, "<script type=\"text/javascript\">print_left_guide(%d)</script>\n", TS_LEFT_GUIDE_PATHWAY);
	}
	fprintf(cgiOut, "				<!--导航 end-->\n");

//	TS_WEB_MENUS
	fprintf(cgiOut, "				<!--内容 start-->\n");
	fprintf(cgiOut, "				<div class=\"content\">\n");

	fprintf(cgiOut, "					<!--网关 start-->\n");
	fprintf(cgiOut, "						<!--标题 start-->\n");
	fprintf(cgiOut, "						<div class=\"pathway_info_title\">网关</div>\n");
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
	fprintf(cgiOut, "						<div class=\"pathway_info_title\">网卡</div>\n");
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
	fprintf(cgiOut, "								<th width=\"83\">操作</th>\n");
	fprintf(cgiOut, "							  </tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "							<!--标题 end-->\n");
	fprintf(cgiOut, "							<!--列表 start-->\n");
	fprintf(cgiOut, "							<div class=\"home_port_list\" id=\"box_scroll\" style=\"height:250px\">\n");
	fprintf(cgiOut, "								<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\" style=\"border-top:0px;\">\n");
	fprintf(cgiOut, "								</table>\n");
	fprintf(cgiOut, "							</div>\n");
	fprintf(cgiOut, "							<!--列表 end-->\n");
	fprintf(cgiOut, "							<!--按钮 start-->\n");
//	fprintf(cgiOut, "							<div class=\"button\"><input id=\"save_btn\" type=\"button\" class=\"save_btn\" style=\"display:none\"/><input type=\"button\" id=\"new_add_btn\" class=\"new_add_btn\"/></div>\n");
	if (TS_PAGE_TYPE_ID_GUIDE != page_type) {
		fprintf(cgiOut, "							<div class=\"button\"><input id=\"save_btn\" type=\"button\" style=\"display:none\" class=\"save_btn\"/><input type=\"button\" id=\"new_add_btn\" class=\"new_add_btn\"/></div>\n");
	} else {
		fprintf(cgiOut, "					<div class=\"button\">");
		fprintf(cgiOut, "<input  id=\"new_add_btn\" type=\"button\" class=\"new_add_btn\"/>");
		fprintf(cgiOut, "<input id=\"pre_btn\" type=\"button\" class=\"pre_btn\" onclick=\"back_page()\"/>\n");
		fprintf(cgiOut, "<input class=\"save_btn\" id=\"save_btn\" type=\"button\" style=\"background:url(../images/next_btn.jpg)\"/></div>\n");
	}
	fprintf(cgiOut, "							<!--按钮 end-->\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--表格 end-->\n");
	fprintf(cgiOut, "					<!--网卡 end-->\n");
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

	ts_free(all_gateway_ids);

	//close db
	ret = sc_close();
	if (ret != SQLITE_OK) {
		return EXIT_FAILURE;
	} else {
		ret = EXIT_SUCCESS;
	}

	log_close();
	return ret;
}
