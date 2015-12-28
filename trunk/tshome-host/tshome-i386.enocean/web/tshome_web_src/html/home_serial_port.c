#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include "log/ts_log_tools.h"
//#include "dev_ctrl/Conversion.h"

//#ifdef TSHOME_WEB

int32_t is_exits = TS_NO_RUNNING;

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

int cgiMain()
{

	//init database
	int rc = EXIT_SUCCESS;

	log_open("home_serial_port.c");

	int32_t page_type = TS_PAGE_TYPE_ID_NOTYPE;
	char * data;
	data = getenv("QUERY_STRING");
	if (data == NULL) {

	} else {
		cgiFormInteger(TS_PAGE_TYPE, &page_type, TS_PAGE_TYPE_ID_NOTYPE);
	}

	ts_is_running(TS_MONITOR_NAME, &is_exits);

	cgiHeaderContentType("text/html\n");
	fprintf(cgiOut, " <!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-Transitional.dtd\">\n");
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
	fprintf(cgiOut, "<!--<script type=\"text/javascript\" language=\"javascript\" src=\"../js/home_serial_port.js\"></script>-->\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" >\n");

//	//base functions
	TS_WEB_AJAX_BASE_FUNCTION

	fprintf(cgiOut, "	/**\n");
	fprintf(cgiOut, " *串口参数\n");
	fprintf(cgiOut, " */\n");
	fprintf(cgiOut, " var del_ids = '';\n");
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
	fprintf(cgiOut, "//修改确定删除操作\n");
	fprintf(cgiOut, "function serial_port_operate(){\n");
	fprintf(cgiOut, "	//修改\n");
	fprintf(cgiOut, "	$(\".serial_port_info table.table_serial_port input.serial_port_delete_\"+serial_port_delete_no).parent().find(\"input.revise_btn\").click(function(){\n");
	fprintf(cgiOut, "		document.getElementById(\"save_btn\").style.display=\"none\";\n");
	fprintf(cgiOut, "		//显示隐藏\n");
	fprintf(cgiOut, "		//$(this).parent().parent().find(\"span.serial_description\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.baud_rate\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.data_bit\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.stop_bit\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.verify\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.serial_name\").hide();\n");
	fprintf(cgiOut, "		$(this).hide();\n");
	fprintf(cgiOut, "		//$(this).parent().parent().find(\"input.serial_description_revise\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.baud_rate_revise\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.data_bit_revise\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.stop_bit_revise\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.verify_revise\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.serial_name_revise\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.ok_btn\").show();	\n");
	fprintf(cgiOut, "		//赋值\n");
	fprintf(cgiOut, "		//$(this).parent().parent().find(\"input.serial_description_revise\").val($(this).parent().parent().find(\"span.serial_description\").html());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.baud_rate_revise\").val($(this).parent().parent().find(\"span.baud_rate\").html());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.data_bit_revise\").val($(this).parent().parent().find(\"span.data_bit\").html());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.stop_bit_revise\").val($(this).parent().parent().find(\"span.stop_bit\").html());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.verify_revise\").val($(this).parent().parent().find(\"span.verify\").html());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.serial_name_revise\").val($(this).parent().parent().find(\"span.serial_name\").html());\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//switch_on\n");
	fprintf(cgiOut, "	$(\".serial_port_info table.table_serial_port input.serial_port_delete_\"+serial_port_delete_no).parent().parent().find(\"input#switch_on\").click(function(){\n");
	fprintf(cgiOut, "		var id=$(this).parent().parent().find(\"span#ts_id\").html();\n");
	fprintf(cgiOut, "		on_off('type=%d&value='+id+'&on_off=%d');\n", TS_WEB_ON_OFF_SERIAL, TS_WEB_ON);
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//switch_off\n");
	fprintf(cgiOut, "	$(\".serial_port_info table.table_serial_port input.serial_port_delete_\"+serial_port_delete_no).parent().parent().find(\"input#switch_off\").click(function(){;\n");
	fprintf(cgiOut, "		var id=$(this).parent().parent().find(\"span#ts_id\").html();\n");
	fprintf(cgiOut, "		on_off('type=%d&value='+id+'&on_off=%d');\n", TS_WEB_ON_OFF_SERIAL, TS_WEB_OFF);
	fprintf(cgiOut, "	});\n");

	//on off
	TS_WEB_ON_OFF

	fprintf(cgiOut, "	//delete\n");
	fprintf(cgiOut, "	$(\".serial_port_info table.table_serial_port input.serial_port_delete_\"+serial_port_delete_no).parent().parent().find(\"input#ts_del\").click(function(){;\n");
	fprintf(cgiOut, "		$(\"#save_btn\").show();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//确定\n");
	fprintf(cgiOut, "	$(\".serial_port_info table.table_serial_port input.serial_port_delete_\"+serial_port_delete_no).parent().find(\"input.ok_btn\").click(function(){\n");
	fprintf(cgiOut, "		$(\"#save_btn\").show();\n");
	fprintf(cgiOut, "		//显示隐藏\n");
	fprintf(cgiOut, "		//$(this).parent().parent().find(\"input.serial_description_revise\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.baud_rate_revise\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.data_bit_revise\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.stop_bit_revise\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.verify_revise\").hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.serial_name_revise\").hide();\n");
	fprintf(cgiOut, "		$(this).hide();\n");
	fprintf(cgiOut, "		//$(this).parent().parent().find(\"span.serial_description\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.baud_rate\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.data_bit\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.stop_bit\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.verify\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.serial_name\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.revise_btn\").show();	\n");
	fprintf(cgiOut, "		//赋值\n");
	fprintf(cgiOut, "		//$(this).parent().parent().find(\"span.serial_description\").html($(this).parent().parent().find(\"input.serial_description_revise\").val());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.baud_rate\").html($(this).parent().parent().find(\"input.baud_rate_revise\").val());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.data_bit\").html($(this).parent().parent().find(\"input.data_bit_revise\").val());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.stop_bit\").html($(this).parent().parent().find(\"input.stop_bit_revise\").val());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.verify\").html($(this).parent().parent().find(\"input.verify_revise\").val());\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.serial_name\").html($(this).parent().parent().find(\"input.serial_name_revise\").val());\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//删除\n");
	fprintf(cgiOut, "	$(\".serial_port_info table.table_serial_port input.serial_port_delete_\"+serial_port_delete_no).click(function(){\n");
	fprintf(cgiOut, "		$(\"#save_btn\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().remove();\n");
	fprintf(cgiOut, "		//id\n");
	fprintf(cgiOut, "		var id=$(this).parent().parent().find(\"span.serial_port_id\").html();\n");
	fprintf(cgiOut, "		del_ids+=id+','");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "var serial_port_delete_no=0;//声明串口参数列表删除按钮编号\n");
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
	fprintf(cgiOut, "			if(%d==serial_port_switch) {\n", TS_WEB_ON);
	fprintf(cgiOut, "				$(\".serial_port_info table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"ts_id\" class=\"serial_port_id\">'+id+'</span></td><td width=\"91\" align=\"center\"><span class=\"baud_rate\">'+baud_rate+'</span><input type=\"text\" class=\"baud_rate_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"data_bit\">'+data_bit+'</span><input type=\"text\" class=\"data_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"stop_bit\">'+stop_bit+'</span><input type=\"text\" class=\"stop_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"verify\">'+verify+'</span><input type=\"text\" class=\"verify_revise\" style=\"display:none\"/></td><td width=\"111\" align=\"center\"><span class=\"serial_name\">'+serial_name+'</span><input type=\"text\" class=\"serial_name_revise\" style=\"display:none\"/></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" id=\"ts_del\" class=\"delete_btn serial_port_delete_'+(++serial_port_delete_no)+'\"/></td><td width=\"110\" align=\"center\" valign=\"middle\"><input name=\"serial_port_'+serial_port_delete_no+'\"  id=\"switch_on\" type=\"radio\" value=\"\" checked /> 打开 <input id=\"switch_off\" name=\"serial_port_'+serial_port_delete_no+'\" type=\"radio\" value=\"\"/> 关闭</td></tr>');\n");
	fprintf(cgiOut, "			} else {\n");
	fprintf(cgiOut, "				$(\".serial_port_info table.table_serial_port\").append('<tr><td width=\"41\"><span  id=\"ts_id\" class=\"serial_port_id\">'+id+'</span></td><td width=\"91\" align=\"center\"><span class=\"baud_rate\">'+baud_rate+'</span><input type=\"text\" class=\"baud_rate_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"data_bit\">'+data_bit+'</span><input type=\"text\" class=\"data_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"stop_bit\">'+stop_bit+'</span><input type=\"text\" class=\"stop_bit_revise\" style=\"display:none\"/></td><td width=\"91\" align=\"center\"><span class=\"verify\">'+verify+'</span><input type=\"text\" class=\"verify_revise\" style=\"display:none\"/></td><td width=\"111\" align=\"center\"><span class=\"serial_name\">'+serial_name+'</span><input type=\"text\" class=\"serial_name_revise\" style=\"display:none\"/></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" id=\"ts_del\" class=\"delete_btn serial_port_delete_'+(++serial_port_delete_no)+'\"/></td><td width=\"110\" align=\"center\" valign=\"middle\"><input name=\"serial_port_'+serial_port_delete_no+'\"  id=\"switch_on\" type=\"radio\" value=\"\"  /> 打开 <input id=\"switch_off\" name=\"serial_port_'+serial_port_delete_no+'\" type=\"radio\" value=\"\" checked/> 关闭</td></tr>');\n");
	fprintf(cgiOut, "			} \n");

	fprintf(cgiOut, "			//修改确定删除方法调用\n");
	fprintf(cgiOut, "			serial_port_operate()\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "//从数据库取出数据，列出已有列表\n");
	//get all serials
	fprintf(cgiOut, "	var parameter;\n");
	get_all_serial();

//	fprintf(cgiOut,"var parameter=[{'id':'1','baud_rate':'12','data_bit':'1','stop_bit':'1','verify':'1','serial_name':'2','serial_port_switch':'打开'},{'id':'2','baud_rate':'12','data_bit':'1','stop_bit':'1','verify':'1','serial_name':'2','serial_port_switch':'打开'}];\n");
	fprintf(cgiOut, "window.onload = function (){\n");
	fprintf(cgiOut, "	serial_port_list(parameter);	\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "$(function(){\n");
	fprintf(cgiOut, "	//新增按钮\n");
	fprintf(cgiOut, "	$(\"input.new_add_btn\").click(function(){\n");
	fprintf(cgiOut, "		var serial_port_id;//新增ID号\n");
	fprintf(cgiOut, "		//id值最大值加1\n");
	fprintf(cgiOut, "		var num=[];\n");
	fprintf(cgiOut, "		$(\".serial_port_info table.table_serial_port span.serial_port_id\").each(function(i,val){\n");
	fprintf(cgiOut, "			num[i]=$(this).html();\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		if(num!=null){\n");
	fprintf(cgiOut, "			serial_port_id=parseInt(num.max())+1;\n");
	fprintf(cgiOut, "		}else{\n");
	fprintf(cgiOut, "			serial_port_id=1;\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "		if (num == '') {");
	fprintf(cgiOut, "			serial_port_id = 1;");
	fprintf(cgiOut, "		}");
//	fprintf(cgiOut, "		alert(1);\n");
	fprintf(cgiOut, "		$(\".serial_port_info table.table_serial_port\").append('<tr><td width=\"41\"><span class=\"serial_port_id\">'+serial_port_id+'</span></td><td width=\"91\" align=\"center\"><span class=\"baud_rate\" style=\"display:none\"></span><input type=\"text\" class=\"baud_rate_revise\"/></td><td width=\"91\" align=\"center\"><span class=\"data_bit\" style=\"display:none\"></span><input type=\"text\" class=\"data_bit_revise\"/></td><td width=\"91\" align=\"center\"><span class=\"stop_bit\" style=\"display:none\"></span><input type=\"text\" class=\"stop_bit_revise\"/></td><td width=\"91\" align=\"center\"><span class=\"verify\" style=\"display:none\"></span><input type=\"text\" class=\"verify_revise\"/></td><td width=\"111\" align=\"center\"><span class=\"serial_name\" style=\"display:none\"></span><input type=\"text\" class=\"serial_name_revise\"/></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\" style=\"display:none\"/><input type=\"button\" class=\"ok_btn\"/><input type=\"button\" class=\"delete_btn serial_port_delete_'+(++serial_port_delete_no)+'\"/></td><td width=\"110\" align=\"center\" valign=\"middle\"><input name=\"serial_port_'+serial_port_delete_no+'\" type=\"radio\" value=\"\"  /> 打开 <input name=\"serial_port_'+serial_port_delete_no+'\" type=\"radio\" value=\"\"/ checked> 关闭</td></tr>');\n");
	//add a column which can be choose

	fprintf(cgiOut, "		//修改确定删除方法调用\n");
	fprintf(cgiOut, "		serial_port_operate();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//保存按钮\n");
	fprintf(cgiOut, "	$(\"input.save_btn\").click(function(){\n");
	fprintf(cgiOut, "		//列表的保存\n");
	fprintf(cgiOut, "		document.getElementById(\"save_btn\").style.display=\"none\";\n");
	fprintf(cgiOut, "		var serial_infos='';\n");
	fprintf(cgiOut, "		var is_can_save = 1;\n");
	fprintf(cgiOut, "		serial_names = \"\";\n");
//	fprintf(cgiOut, "		var serial_port_save=[];\n");
	fprintf(cgiOut, "		$(\".serial_port_info table.table_serial_port span.serial_port_id\").each(function(i,val){\n");
	fprintf(cgiOut, "			var id=$(this).html();//id\n");
	fprintf(cgiOut, "			//var description=$(this).parent().parent().find(\"span.serial_description\").html();\n");
	fprintf(cgiOut, "			var baud_rate=$(this).parent().parent().find(\"span.baud_rate\").html();\n");
	fprintf(cgiOut, "			var data_bit=$(this).parent().parent().find(\"span.data_bit\").html();\n");
	fprintf(cgiOut, "			var stop_bit=$(this).parent().parent().find(\"span.stop_bit\").html();\n");
	fprintf(cgiOut, "			var verify=$(this).parent().parent().find(\"span.verify\").html();\n");
	fprintf(cgiOut, "			var serial_name=$(this).parent().parent().find(\"span.serial_name\").html();\n");
	//validate
	//波特率
//	fprintf(cgiOut, "			alert(baud_rate);\n");
	fprintf(cgiOut, "			if(is_can_save == 1) {");
	fprintf(cgiOut, "				is_can_save =  BASEisNotInt(\"波特率\",baud_rate);\n");
	fprintf(cgiOut, "			}\n");
	//数据位
	fprintf(cgiOut, "			if(is_can_save == 1) {");
	fprintf(cgiOut, "				is_can_save =  BASEisNotInt(\"数据位\",data_bit);\n");
	fprintf(cgiOut, "			}\n");
	//停止位
	fprintf(cgiOut, "			if(is_can_save == 1) {");
	fprintf(cgiOut, "				is_can_save =  BASEisNotFloat(\"停止位\",stop_bit);\n");
	fprintf(cgiOut, "			}\n");
	//校验
	fprintf(cgiOut, "			if(is_can_save == 1) {");
	fprintf(cgiOut, "				is_can_save =  BASEisNotInt(\"校验\",verify);\n");
	fprintf(cgiOut, "			}\n");

	fprintf(cgiOut, "			if(is_can_save == 1) {");
	fprintf(cgiOut, "				is_can_save =  is_validate(serial_name);\n");
	fprintf(cgiOut, "			}\n");

	fprintf(cgiOut, "			if(is_can_save == 1) {");
	fprintf(cgiOut, "				is_can_save =  validate_serial_name(serial_name);\n");
	fprintf(cgiOut, "			}\n");

	fprintf(cgiOut, "			serial_infos+=id+','+baud_rate+','+data_bit+','+stop_bit+','+verify+','+serial_name+',;'");
	fprintf(cgiOut, "		});\n");

	fprintf(cgiOut, "		if(is_can_save == 1) {\n");
	fprintf(cgiOut, "			var del_len = del_ids.length;\n");
	fprintf(cgiOut, "			var info_len = serial_infos.length;\n");
	fprintf(cgiOut, "			var sendAll = 'del_len='+del_len+'&info_len='+info_len+'&del_ids='+del_ids+'&serial_infos='+serial_infos;\n");
//	fprintf(cgiOut, "			alert(sendAll);\n");
	fprintf(cgiOut, "			set_serial_info(sendAll);\n");
	if (TS_PAGE_TYPE_ID_GUIDE == page_type) {
		fprintf(cgiOut, "			jumpPage('%s%s')\n", "../cgi-bin/pathway_info.html?", TS_PAGE_GUIDE);
	}
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "});\n");

	//保存
	TS_WEB_SAVE("set_serial_info", "set_serial_info");

	fprintf(cgiOut, "</script>\n");

	//html
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
		fprintf(cgiOut, "<script type=\"text/javascript\">print_left_guide(%d)</script>\n", TS_LEFT_GUIDE_SERIAL_INFO);
	}
	fprintf(cgiOut, "				<!--导航 end-->\n");

//	TS_WEB_MENUS
	fprintf(cgiOut, "				<!--内容 start-->\n");
	fprintf(cgiOut, "				<div class=\"content\">\n");
	fprintf(cgiOut, "					<!--表格 start-->\n");
	fprintf(cgiOut, "					<div class=\"home_serial_port_info\">\n");
	fprintf(cgiOut, "						<!--标题 start-->\n");
	fprintf(cgiOut, "						<div class=\"title\">\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\">\n");
	fprintf(cgiOut, "							  <tr>\n");
	fprintf(cgiOut, "								<th width=\"41\">ID</th>\n");
	fprintf(cgiOut, "								<th width=\"91\">波特率</th>\n");
	fprintf(cgiOut, "								<th width=\"91\">数据位</th>\n");
	fprintf(cgiOut, "								<th width=\"91\">停止位</th>\n");
	fprintf(cgiOut, "								<th width=\"91\">校验</th>\n");
	fprintf(cgiOut, "								<th width=\"111\">串口名</th>\n");
	fprintf(cgiOut, "								<th width=\"83\">操作</th>\n");
	fprintf(cgiOut, "								<th width=\"110\">开关</th>\n");
	fprintf(cgiOut, "							  </tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--标题 end-->\n");
	fprintf(cgiOut, "						<!--内容 start-->\n");
	fprintf(cgiOut, "						<div class=\"serial_port_info\" id=\"box_scroll\" style=\"height:250px\">\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\" style=\"border-top:0px\">\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--内容 end-->\n");
	fprintf(cgiOut, "					</div>\n");
	fprintf(cgiOut, "					<!--表格 end-->\n");
	fprintf(cgiOut, "					<!--按钮 start-->\n");
	if (TS_PAGE_TYPE_ID_GUIDE != page_type) {
		fprintf(cgiOut, "					<div class=\"button\"><input  id=\"save_btn\" type=\"button\" style=\"display:none\" class=\"save_btn\"/><input type=\"button\" class=\"new_add_btn\"/></div>\n");
	} else {
		fprintf(cgiOut, "					<div class=\"button\">");
		fprintf(cgiOut, "<input type=\"button\" class=\"new_add_btn\"/>");
		fprintf(cgiOut, "<input id=\"pre_btn\" type=\"button\" class=\"pre_btn\" onclick=\"back_page()\"/>\n");
		fprintf(cgiOut, "<input class=\"save_btn\" id=\"save_btn\" type=\"button\" style=\"background:url(../images/next_btn.jpg)\"/></div>\n");
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
		return EXIT_FAILURE;
	}

	log_close();

	return EXIT_SUCCESS;
}
