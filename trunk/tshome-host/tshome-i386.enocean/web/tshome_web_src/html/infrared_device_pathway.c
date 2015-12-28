#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include "db/ts_db_protocol_infos.h"
#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include <dlfcn.h>
#include <unistd.h>
#include "log/ts_log_tools.h"

#include "common/common_define.h"

//typedef int (*protocol_is_ifrared_open_t)(int, int *);
//protocol_is_ifrared_open_t fun_is_ifrared_open;
//void* handle;

int32_t dev_counts = 0;
int32_t all_on_off = TS_WEB_ON;
int32_t is_exits = TS_NO_RUNNING;

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

int cgiMain()
{
	//init database
	int rc = EXIT_SUCCESS;

	log_open("infrared_device_pathway.c");

	int32_t page_type = TS_PAGE_TYPE_ID_NOTYPE;
	char * data;
	data = getenv("QUERY_STRING");
	if (data == NULL) {

	} else {
		cgiFormInteger(TS_PAGE_TYPE, &page_type, TS_PAGE_TYPE_ID_NOTYPE);
	}

	ts_is_running(TS_MONITOR_NAME, &is_exits);

//	log_debug_web("%s\n",TS_LIB_GATEWAY_2_WEB);
//	handle = dlopen(TS_LIB_GATEWAY_2_WEB, RTLD_LAZY);
//	if (NULL == handle) {
//		log_debug_web( "Cannot open library!!!!!!!!!!path = %s\n", TS_LIB_GATEWAY_2_WEB);
//		return EXIT_FAILURE;
//	} else {
////		log_debug_web( " open library success! path = %s\n",TS_LIB_GATEWAY_2_WEB);
//	}
//
//	fun_is_ifrared_open = (protocol_is_ifrared_open_t) dlsym(handle, "protocol_is_ifrared_open");
//	const char *dlsym_error = dlerror();
//	if (NULL != dlsym_error) {
//		log_debug_web( "can't open function.%s\n", dlsym_error);
//		dlclose(handle);
//		return EXIT_FAILURE;
//	}

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
	fprintf(cgiOut, "<!--<script type=\"text/javascript\" language=\"javascript\" src=\"../js/infrared_device_pathway.js\"></script>-->\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/ts_set_base_info.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" >\n");

	//ajax base function
	TS_WEB_AJAX_BASE_FUNCTION

	fprintf(cgiOut, "/**\n");
	fprintf(cgiOut, " *红外设备通路\n");
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
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "var infrared_device_delete_no=0;//声明设备连接通路删除按钮编号\n");
	fprintf(cgiOut, "//修改确定删除操作方法\n");
	fprintf(cgiOut, "function infrared_device_oprate(){\n");
	fprintf(cgiOut, "	//下拉列表选择\n");
	fprintf(cgiOut, "	$(\".infrared_device_delete_\"+infrared_device_delete_no).parent().parent().find(\".infrared_device_pathway_select\").mouseover(function(){\n");
	fprintf(cgiOut, "		$(this).find(\"select.infrared_device_pathway_second\").show();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	$(\".infrared_device_delete_\"+infrared_device_delete_no).parent().parent().find(\".infrared_device_pathway_select\").mouseout(function(){\n");
	fprintf(cgiOut, "		$(this).find(\"select.infrared_device_pathway_second\").hide();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	$(\".infrared_device_delete_\"+infrared_device_delete_no).parent().parent().find(\"select.infrared_device_pathway_second\").click(function(){\n");
	fprintf(cgiOut, "		var select_value=\"\";\n");
	fprintf(cgiOut, "		$(this).find(\"option\").each(function(i,val){\n");
	fprintf(cgiOut, "			if($(this).attr(\"selected\")==\"selected\"){\n");
	fprintf(cgiOut, "				select_value=select_value+$(this).html();\n");
	fprintf(cgiOut, "			}\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		$(this).parent().find(\"input\").val(select_value);\n");
	fprintf(cgiOut, "		$(this).hide();\n");
	fprintf(cgiOut, "	});\n");

	//turn on
	fprintf(cgiOut, "	$(\".infrared_device_delete_\"+infrared_device_delete_no).parent().parent().find(\"input#switch_on\").click(function(){\n");
	fprintf(cgiOut, "		var id = $(this).parent().parent().parent().find('span#host_infr_id').html();\n");
	fprintf(cgiOut, "		on_off('type=%d&value='+id+'&on_off=%d');\n", TS_WEB_ON_OFF_INFRARED, TS_WEB_ON);
	fprintf(cgiOut, "	});\n");

//	//turn off
	fprintf(cgiOut, "	$(\".infrared_device_delete_\"+infrared_device_delete_no).parent().parent().find(\"input#switch_off\").click(function(){\n");
	fprintf(cgiOut, "		var id = $(this).parent().parent().parent().find('span#host_infr_id').html();\n");
	fprintf(cgiOut, "		on_off('type=%d&value='+id+'&on_off=%d');\n", TS_WEB_ON_OFF_INFRARED, TS_WEB_OFF);
	fprintf(cgiOut, "	});\n");

//	//开关
	TS_WEB_ON_OFF

	//
	fprintf(cgiOut, "	//修改\n");
	fprintf(cgiOut, "	$(\".infrared_device_delete_\"+infrared_device_delete_no).parent().parent().find(\"input.revise_btn\").click(function(){\n");
	fprintf(cgiOut, "		document.getElementById(\"save_btn\").style.display=\"none\";\n");
	fprintf(cgiOut, "		//显示隐藏\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.infrared_device_pathway_no\").hide();\n");
	fprintf(cgiOut, "		$(this).hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\".infrared_device_pathway_select\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.ok_btn\").show();\n");
	fprintf(cgiOut, "		//赋值\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\".infrared_device_pathway_select input\").val($(this).parent().parent().find(\"span.infrared_device_pathway_no\").html());\n");
	fprintf(cgiOut, "	});\n");
	//
	fprintf(cgiOut, "	//确定\n");
	fprintf(cgiOut, "	$(\".infrared_device_delete_\"+infrared_device_delete_no).parent().parent().find(\"input.ok_btn\").click(function(){\n");
	fprintf(cgiOut, "		$('#save_btn').show();\n");
	fprintf(cgiOut, "		//显示隐藏\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\".infrared_device_pathway_select\").hide();\n");
	fprintf(cgiOut, "		$(this).hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.infrared_device_name\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.infrared_device_pathway_no\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.revise_btn\").show();\n");
	fprintf(cgiOut, "		//赋值\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.infrared_device_pathway_no\").html($(this).parent().parent().find(\".infrared_device_pathway_select input\").val());\n");
	fprintf(cgiOut, "		if($(this).parent().parent().find(\"input.infrared_device_name_revise\").css(\"display\")==\"none\"){\n");
	fprintf(cgiOut, "		}else{\n");
	fprintf(cgiOut, "			$(this).parent().parent().find(\"input.infrared_device_name_revise\").hide();\n");
	fprintf(cgiOut, "			$(this).parent().parent().find(\"span.infrared_device_name\").html($(this).parent().parent().find(\"input.infrared_device_name_revise\").val());\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//删除\n");
	fprintf(cgiOut, "	$(\".infrared_device_delete_\"+infrared_device_delete_no).parent().parent().find(\"input.delete_btn\").click(function(){\n");
	fprintf(cgiOut, "		$(this).parent().parent().remove();\n");
	fprintf(cgiOut, "		//id\n");
	fprintf(cgiOut, "		//var id=$(this).parent().parent().find(\"span.infrared_device_id\").html();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "}\n");

	fprintf(cgiOut, "//从数据库取出数据，列出已有列表\n");
	fprintf(cgiOut, "window.onload = function (){\n");
	fprintf(cgiOut, "	var parameter;\n");
	get_infrared_device();
//	fprintf(cgiOut, "	var parameter=[{'id':'1','infrared_device_name':'快进','infrared_device_pathway_no':'1','infrared_device_pathway_select':['1','2','3','4','5']},{'id':'2','infrared_device_name':'快进','infrared_device_pathway_no':'1','infrared_device_pathway_select':['1','2','3','4','5']}];\n");
	fprintf(cgiOut, "	infrared_device_list(parameter);\n");
	fprintf(cgiOut, "}\n");

	fprintf(cgiOut, "//生成已有列表 \n");
	fprintf(cgiOut, "function infrared_device_list(parameter){\n");
	fprintf(cgiOut, "	if(parameter != null){\n");
	fprintf(cgiOut, "		for(i=0;i<parameter.length;i++){\n");
	fprintf(cgiOut, "			var id=parameter[i].id;\n");
	fprintf(cgiOut, "			var infrared_device_name=parameter[i].infrared_device_name;\n");
	fprintf(cgiOut, "			var infrared_device_pathway_no=parameter[i].infrared_device_pathway_no;\n");
	fprintf(cgiOut, "			var infrared_device_pathway_select=parameter[i].infrared_device_pathway_select;\n");
//	fprintf(cgiOut, "			$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span class=\"infrared_device_id\">'+id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\">'+infrared_device_name+'</span></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\">'+infrared_device_pathway_no+'</span><div class=\"infrared_device_pathway_select\" style=\"display:none\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"infrared_device_pathway_second\" style=\"display:none;z-index:'+(9999-(++infrared_device_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" class=\"delete_btn infrared_device_delete_'+infrared_device_delete_no+'\" style=\"display:none\"/></td><td width=\"110\" align=\"center\"><input name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\" checked /> 打开 <input name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\"/> 关闭</td></tr>');\n");
	//fprintf(cgiOut, "			$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"host_infr_id\" class=\"infrared_device_id\">'+id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\">'+infrared_device_name+'</span></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\">'+infrared_device_pathway_no+'</span><div class=\"infrared_device_pathway_select\" style=\"display:none\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"infrared_device_pathway_second\" style=\"display:none;z-index:'+(9999-(++infrared_device_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" class=\"delete_btn infrared_device_delete_'+infrared_device_delete_no+'\" style=\"display:none\"/></td><td width=\"110\" align=\"center\"><input id=\"switch_on\" name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\" checked /> 打开 <input id=\"switch_off\" name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\"/> 关闭</td></tr>');\n");
	fprintf(cgiOut, "			var on_off=parameter[i].on_off;\n");

//	fprintf(cgiOut, "			if(%d==on_off) {\n", TS_WEB_ON);
//	fprintf(cgiOut, "				$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"host_infr_id\" class=\"infrared_device_id\">'+id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\">'+infrared_device_name+'</span></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\">'+infrared_device_pathway_no+'</span><div class=\"infrared_device_pathway_select\" style=\"display:none\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"infrared_device_pathway_second\" style=\"display:none;z-index:'+(9999-(++infrared_device_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" class=\"delete_btn infrared_device_delete_'+infrared_device_delete_no+'\" style=\"display:none\"/></td><td width=\"110\" align=\"center\"><input id=\"switch_on\" name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\" checked /> 打开 <input id=\"switch_off\" name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\"/> 关闭</td></tr>');\n");
//	fprintf(cgiOut, "			} else {\n");
//	fprintf(cgiOut, "				$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"host_infr_id\" class=\"infrared_device_id\">'+id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\">'+infrared_device_name+'</span></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\">'+infrared_device_pathway_no+'</span><div class=\"infrared_device_pathway_select\" style=\"display:none\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"infrared_device_pathway_second\" style=\"display:none;z-index:'+(9999-(++infrared_device_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" class=\"delete_btn infrared_device_delete_'+infrared_device_delete_no+'\" style=\"display:none\"/></td><td width=\"110\" align=\"center\"><input id=\"switch_on\" name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\"  /> 打开 <input id=\"switch_off\" name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\" checked/> 关闭</td></tr>');\n");
//	fprintf(cgiOut, "			} \n");

	fprintf(cgiOut, "			if(0==i) {\n");
	fprintf(cgiOut, "				if(%d==%d) {\n", TS_WEB_ON, all_on_off);
	fprintf(cgiOut, "					$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"host_infr_id\" class=\"infrared_device_id\">'+id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\">'+infrared_device_name+'</span></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\">'+infrared_device_pathway_no+'</span><div class=\"infrared_device_pathway_select\" style=\"display:none\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"infrared_device_pathway_second\" style=\"display:none;z-index:'+(9999-(++infrared_device_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" class=\"delete_btn infrared_device_delete_'+infrared_device_delete_no+'\" style=\"display:none\"/></td><td rowspan=%d width=\"110\" align=\"center\"><input id=\"switch_on\" name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\" checked /> 打开 <input id=\"switch_off\" name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\"/> 关闭</td></tr>');\n", dev_counts);
	fprintf(cgiOut, "				} else {\n");
	fprintf(cgiOut, "					$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"host_infr_id\" class=\"infrared_device_id\">'+id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\">'+infrared_device_name+'</span></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\">'+infrared_device_pathway_no+'</span><div class=\"infrared_device_pathway_select\" style=\"display:none\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"infrared_device_pathway_second\" style=\"display:none;z-index:'+(9999-(++infrared_device_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" class=\"delete_btn infrared_device_delete_'+infrared_device_delete_no+'\" style=\"display:none\"/></td><td rowspan=%d width=\"110\" align=\"center\"><input id=\"switch_on\" name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\"  /> 打开 <input id=\"switch_off\" name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\" checked/> 关闭</td></tr>');\n", dev_counts);
	fprintf(cgiOut, "				} \n");
	fprintf(cgiOut, "			}else {\n");
	fprintf(cgiOut, "				$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"host_infr_id\" class=\"infrared_device_id\">'+id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\">'+infrared_device_name+'</span></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\">'+infrared_device_pathway_no+'</span><div class=\"infrared_device_pathway_select\" style=\"display:none\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"infrared_device_pathway_second\" style=\"display:none;z-index:'+(9999-(++infrared_device_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" class=\"delete_btn infrared_device_delete_'+infrared_device_delete_no+'\" style=\"display:none\"/></td></tr>');\n");
	fprintf(cgiOut, "			}\n");

	fprintf(cgiOut, "			//下拉列表生成\n");
	fprintf(cgiOut, "			for(j=0;j<infrared_device_pathway_select.length;j++){\n");
	fprintf(cgiOut, "				$(\".infrared_device_delete_\"+infrared_device_delete_no).parent().parent().find(\"select.infrared_device_pathway_second\").append('<option>'+infrared_device_pathway_select[j]+'</option>');\n");
	fprintf(cgiOut, "			}\n");
	fprintf(cgiOut, "			//修改确定删除操作方法\n");
	fprintf(cgiOut, "			infrared_device_oprate();\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");

	fprintf(cgiOut, "\n");
	fprintf(cgiOut, "$(function(){\n");
	fprintf(cgiOut, "	//新增按钮\n");
	fprintf(cgiOut, "	$(\"input.new_add_btn\").click(function(){\n");
	fprintf(cgiOut, "		//id为最大值加1\n");
	fprintf(cgiOut, "		var infrared_device_id;\n");
	fprintf(cgiOut, "		var num=[];\n");
	fprintf(cgiOut, "		$(\".infrared_device_list table.table_serial_port span.infrared_device_id\").each(function(i,val){\n");
	fprintf(cgiOut, "			num[i]=$(this).html();\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		if(num!=null){\n");
	fprintf(cgiOut, "			infrared_device_id=parseInt(num.max())+1;\n");
	fprintf(cgiOut, "		}else{\n");
	fprintf(cgiOut, "			infrared_device_id=1;\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "		if(num == \"\"){\n");
	fprintf(cgiOut, "			infrared_device_id=1;\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "\n");
	fprintf(cgiOut, "		$(\".infrared_device_list table.table_serial_port\").append('<tr><td width=\"41\"><span class=\"infrared_device_id\">'+infrared_device_id+'</span></td><td width=\"250\" align=\"center\"><span class=\"infrared_device_name\" style=\"display:none\"></span><input type=\"text\" class=\"infrared_device_name_revise\"/></td><td width=\"228\" align=\"center\"><span class=\"infrared_device_pathway_no\" style=\"display:none\"></span><div class=\"infrared_device_pathway_select\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"infrared_device_pathway_second\" style=\"display:none;z-index:'+(9999-(++infrared_device_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\" style=\"display:none\"/><input type=\"button\" class=\"ok_btn\"/><input type=\"button\" class=\"delete_btn infrared_device_delete_'+infrared_device_delete_no+'\" style=\"display:none\"/></td><td width=\"110\" align=\"center\"><input name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\" checked /> 打开 <input name=\"infrared_device_'+infrared_device_delete_no+'\" type=\"radio\" value=\"\"/> 关闭</td></tr>');\n");
	fprintf(cgiOut, "		var parameter=[{'id':'1','infrared_device_name':'快进','infrared_device_pathway_no':'1','infrared_device_pathway_select':['1','2','3','4','5']},{'id':'2','infrared_device_name':'快进','infrared_device_pathway_no':'1','infrared_device_pathway_select':['1','2','3','4','5']}];\n");
	fprintf(cgiOut, "		var infrared_device_pathway_select=parameter[0].infrared_device_pathway_select;\n");
	fprintf(cgiOut, "		//下拉列表生成\n");
	fprintf(cgiOut, "		for(j=0;j<infrared_device_pathway_select.length;j++){\n");
	fprintf(cgiOut, "			$(\".infrared_device_delete_\"+infrared_device_delete_no).parent().parent().find(\"select.infrared_device_pathway_second\").append('<option>'+infrared_device_pathway_select[j]+'</option>');\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "		//修改确定删除操作方法\n");
	fprintf(cgiOut, "		infrared_device_oprate();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//保存按钮\n");
	fprintf(cgiOut, "	$(\"input.save_btn\").click(function(){\n");
	fprintf(cgiOut, "	document.getElementById('save_btn').style.display='none'\n");
	fprintf(cgiOut, "		//列表的保存\n");
	fprintf(cgiOut, "		var infrared_device_infos='';\n");
//	fprintf(cgiOut, "		var infrared_device_save=[];\n");
	fprintf(cgiOut, "		$(\".infrared_device_list table.table_serial_port span.infrared_device_id\").each(function(i,val){\n");
	fprintf(cgiOut, "			var id=$(this).html();//id\n");
	fprintf(cgiOut, "			var infrared_device_name=$(this).parent().parent().find(\"span.infrared_device_name\").html();\n");
	fprintf(cgiOut, "			var infrared_device_pathway_no=$(this).parent().parent().find(\"span.infrared_device_pathway_no\").html();\n");
	fprintf(cgiOut, "			infrared_device_infos+=id+','+infrared_device_pathway_no+';';\n");
//	fprintf(cgiOut, "			infrared_device_save[i]={'id':id,'infrared_device_name':infrared_device_name,'infrared_device_pathway_no':infrared_device_pathway_no};\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		var info_len = infrared_device_infos.length;\n");
	fprintf(cgiOut, "		var send_all = 'info_len='+info_len+'&info='+infrared_device_infos;\n");
	if (TS_PAGE_TYPE_ID_GUIDE == page_type) {
		fprintf(cgiOut, "	send_all = send_all + '&is_first_use='+%d;\n", TS_IS_FIRST_USE_NOT);
	}
	fprintf(cgiOut, "		set_infrared_channel(send_all);\n");
	if (TS_PAGE_TYPE_ID_GUIDE == page_type) {
		fprintf(cgiOut, "		jumpPage('%s');\n", "../cgi-bin/curr_state.html");
	}
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "	});\n");

//	//保存
	TS_WEB_SAVE("set_infrared_channel", "set_channel");

	fprintf(cgiOut, "</script>\n");

	//HTML
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
		fprintf(cgiOut, "<script type=\"text/javascript\">print_left_guide(%d)</script>\n", TS_LEFT_GUIDE_INFRARED);
	}
	fprintf(cgiOut, "				<!--导航 end-->\n");

//	TS_WEB_MENUS
	fprintf(cgiOut, "				<!--内容 start-->\n");
	fprintf(cgiOut, "				<div class=\"content\">\n");
	fprintf(cgiOut, "					<!--表格 start-->\n");
	fprintf(cgiOut, "					<div class=\"infrared_device_pathway_info\">\n");

	//set driver
	fprintf(cgiOut, "				<div class=\"title_text\">红外驱动</div>\n");
	fprintf(cgiOut, "				<br/>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" >\n");
	fprintf(cgiOut, "	function set_driver(flag){\n");
	fprintf(cgiOut, "		var name = $(flag).parent().find(\"#infr_driver_value\").val();\n");
	fprintf(cgiOut, "		var data = 'infr_driver_value=' + name\n");
	fprintf(cgiOut, "		set_infr_driver(data);\n");
	fprintf(cgiOut, "	};\n");
	TS_WEB_SAVE("set_infr_driver", "set_infr_driver");
	fprintf(cgiOut, "</script>\n");
	fprintf(cgiOut, "				<form id=\"set_infr_diver\">\n");
	char ir_name[256] = {0 };
	db_get_infr_driver(ir_name);
	fprintf(cgiOut, "					<b>驱动名称 : </b><input id=\"infr_driver_value\" type=\"text\" value=\"%s\"/>\n", ir_name);
	fprintf(cgiOut, "					<input type=\"button\" id=\"set_infr_driver\" value=\"保存\" onclick=\"set_driver(this)\"/>\n");
	fprintf(cgiOut, "				</form>\n");


	//set protrol
	fprintf(cgiOut, "						<div class=\"title_text\">网关通路</div>\n");
	fprintf(cgiOut, "						<br/>\n");
	fprintf(cgiOut, "						<div class=\"title\">\n");
	//infr passway
	fprintf(cgiOut, "						<!--标题 start-->\n");
	fprintf(cgiOut, "						<div class=\"title\">\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\">\n");
	fprintf(cgiOut, "							  <tr>\n");
	fprintf(cgiOut, "								<th width=\"41\">ID</th>\n");
	fprintf(cgiOut, "								<th width=\"250\">红外设备名称</th>\n");
	fprintf(cgiOut, "								<th width=\"228\">通路号</th>\n");
	fprintf(cgiOut, "								<th width=\"83\">操作</th>\n");
	fprintf(cgiOut, "								<th width=\"110\">开关</th>\n");
	fprintf(cgiOut, "							  </tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--标题 end-->\n");
	fprintf(cgiOut, "						<!--内容 start-->\n");
	fprintf(cgiOut, "						<div class=\"infrared_device_list\">\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\" style=\"border-top:0px\">\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--内容 end-->\n");
	fprintf(cgiOut, "					</div>\n");
	fprintf(cgiOut, "					<!--表格 end-->\n");
	fprintf(cgiOut, "					<!--按钮 start-->\n");
//	fprintf(cgiOut, "					<div class=\"button\"><input id='save_btn' type=\"button\" class=\"save_btn\" style='display:none'/></div>\n");
	if (TS_PAGE_TYPE_ID_GUIDE != page_type) {
		fprintf(cgiOut, "				<div class=\"button\"><input id=\"save_btn\" type=\"button\" style=\"display:none\" class=\"save_btn\"/><input type=\"button\" class=\"new_add_btn\"  style='display:none'/></div>\n");
	} else {
		fprintf(cgiOut, "					<div class=\"button\">");
		fprintf(cgiOut, "						<input  id=\"new_add_btn\" type=\"button\" class=\"new_add_btn\"  style=\"display:none\"/>");
		fprintf(cgiOut, "						<input id=\"pre_btn\" type=\"button\" class=\"pre_btn\" onclick=\"back_page()\"/>\n");
		fprintf(cgiOut, "						<input class=\"save_btn\" id=\"save_btn\" type=\"button\" style=\"background:url(../images/finish_btn.jpg)\"/></div>\n");
	}
	fprintf(cgiOut, "					<!--按钮 end-->\n");
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
	rc = sc_close();
	if (rc != SQLITE_OK) {
//		dlclose(handle);
		return EXIT_FAILURE;
	}

	log_close();

//	dlclose(handle);

	return rc;
}
