#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include "log/ts_log_tools.h"

char * comm_ports = NULL;

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

	int ret = EXIT_SUCCESS;

	log_open("device_link.c");

	int32_t page_type = TS_PAGE_TYPE_ID_NOTYPE;
	char * data;
	data = getenv("QUERY_STRING");
	if (data == NULL) {

	} else {
		cgiFormInteger(TS_PAGE_TYPE, &page_type, TS_PAGE_TYPE_ID_NOTYPE);
	}

	//get all comm ports
	get_comm_ports();

	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-Transitional.dtd\">\n");
	fprintf(cgiOut, "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");

	//login valiate failed
	TS_LOGIN_FAILED

	fprintf(cgiOut, "<head>\n");
	fprintf(cgiOut, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n");
	fprintf(cgiOut, "<title>TSHome主机</title>\n");
	fprintf(cgiOut, "<meta name=\"keywords\" content=\"智能,家居,主机,系统,TSHome\" />\n");
	fprintf(cgiOut, "<link rel=\"stylesheet\" href=\"../css/main.css\"  type=\"text/css\"/>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/jquery-1.7.1.js\"></script>\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\" src=\"../js/ts_set_base_info.js\"></script>\n");
	fprintf(cgiOut, "<!--<script type=\"text/javascript\" language=\"javascript\" src=\"../js/device_link_access.js\"></script>-->\n");
	fprintf(cgiOut, "<script type=\"text/javascript\" language=\"javascript\">\n");

	//ajax base function
	TS_WEB_AJAX_BASE_FUNCTION

	fprintf(cgiOut, "	/**\n");
	fprintf(cgiOut, " *串口设备通路\n");
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
	fprintf(cgiOut, "var device_link_delete_no=0;//声明设备连接通路删除按钮编号\n");
	fprintf(cgiOut, "//修改确定删除操作方法\n");
	fprintf(cgiOut, "function device_link_oprate(){\n");
	fprintf(cgiOut, "	//下拉列表选择\n");
	fprintf(cgiOut, "	$(\".device_link_delete_\"+device_link_delete_no).parent().parent().find(\".device_link_select\").mouseover(function(){\n");
	fprintf(cgiOut, "		$(this).find(\".device_link_second\").show();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	$(\".device_link_delete_\"+device_link_delete_no).parent().parent().find(\".device_link_select\").mouseout(function(){\n");
	fprintf(cgiOut, "		$(this).find(\".device_link_second\").hide();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	$(\".device_link_delete_\"+device_link_delete_no).parent().parent().find(\"select.device_link_second\").click(function(){\n");
	fprintf(cgiOut, "		var select_value=\"\";\n");
	fprintf(cgiOut, "		$(this).find(\"option\").each(function(i,val){\n");
	fprintf(cgiOut, "			if($(this).attr(\"selected\")==\"selected\"){\n");
	fprintf(cgiOut, "				select_value=select_value+$(this).html();\n");
	fprintf(cgiOut, "			}\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		$(this).parent().find(\"input\").val(select_value);\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//修改\n");
	fprintf(cgiOut, "	$(\".device_link_delete_\"+device_link_delete_no).parent().parent().find(\"input.revise_btn\").click(function(){\n");
	fprintf(cgiOut, "		document.getElementById(\"save_btn\").style.display=\"none\";\n");
	fprintf(cgiOut, "		//显示隐藏\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.device_link_homeportnote\").hide();\n");
	fprintf(cgiOut, "		$(this).hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\".device_link_select\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.ok_btn\").show();\n");
	fprintf(cgiOut, "		//赋值\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\".device_link_select input\").val($(this).parent().parent().find(\"span.device_link_homeportnote\").html());\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//确定\n");
	fprintf(cgiOut, "	$(\".device_link_delete_\"+device_link_delete_no).parent().parent().find(\"input.ok_btn\").click(function(){\n");
	fprintf(cgiOut, "		$(\"#save_btn\").show();\n");
	fprintf(cgiOut, "		//显示隐藏\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\".device_link_select\").hide();\n");
	fprintf(cgiOut, "		$(this).hide();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.device_link_equipment\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.device_link_room\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.device_link_homeportnote\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"input.revise_btn\").show();\n");
	fprintf(cgiOut, "		//赋值\n");
	fprintf(cgiOut, "		$(this).parent().parent().find(\"span.device_link_homeportnote\").html($(this).parent().parent().find(\".device_link_select input\").val());\n");
	fprintf(cgiOut, "		if($(this).parent().parent().find(\"input.device_link_equipment_revise\").css(\"display\")==\"none\"){\n");
	fprintf(cgiOut, "		}else{\n");
	fprintf(cgiOut, "			$(this).parent().parent().find(\"input.device_link_equipment_revise\").hide();\n");
	fprintf(cgiOut, "			$(this).parent().parent().find(\"span.device_link_equipment\").html($(this).parent().parent().find(\"input.device_link_equipment_revise\").val());\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "		if($(this).parent().parent().find(\"input.device_link_room_revise\").css(\"display\")==\"none\"){\n");
	fprintf(cgiOut, "		}else{\n");
	fprintf(cgiOut, "			$(this).parent().parent().find(\"input.device_link_room_revise\").hide();\n");
	fprintf(cgiOut, "			$(this).parent().parent().find(\"span.device_link_room\").html($(this).parent().parent().find(\"input.device_link_room_revise\").val());\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//删除\n");
	fprintf(cgiOut, "	$(\".device_link_delete_\"+device_link_delete_no).parent().parent().find(\"input.delete_btn\").click(function(){\n");
	fprintf(cgiOut, "		$(\"#save_btn\").show();\n");
	fprintf(cgiOut, "		$(this).parent().parent().remove();\n");
	fprintf(cgiOut, "		//id\n");
	fprintf(cgiOut, "		//var id=$(this).parent().parent().find(\"span.device_link_id\").html();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "}\n");

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
//	fprintf(cgiOut, "			$(\".device_link_access_list table.table_serial_port\").append('<tr><td width=\"41\"><span class=\"device_link_id\">'+id+'</span></td><td width=\"113\" align=\"center\"><span class=\"device_link_equipment\">'+equipment+'</span></td><td width=\"179\" align=\"center\"><span class=\"device_link_room\">'+room+'</span></td><td width=\"133\" align=\"center\"><span class=\"device_link_homeporttype\">'+homeport_type+'</span></td><td width=\"162\" align=\"center\"><span class=\"device_link_homeportnote\">'+homeport_note+'</span><div class=\"device_link_select\" style=\"display:none\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"device_link_second\" style=\"display:none;z-index:'+(99999-(++device_link_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" class=\"delete_btn device_link_delete_'+device_link_delete_no+'\"/></td></tr>');\n");
	fprintf(cgiOut, "			$(\".device_link_access_list table.table_serial_port\").append('<tr><td width=\"41\"><span class=\"device_link_id\">'+id+'</span></td><td width=\"113\" align=\"center\"><span class=\"device_link_equipment\">'+equipment+'</span></td><td width=\"179\" align=\"center\"><span class=\"device_link_room\">'+room+'</span></td><td width=\"133\" align=\"center\"><span class=\"device_link_homeporttype\">'+homeport_type+'</span></td><td width=\"162\" align=\"center\"><span class=\"device_link_homeportnote\">'+homeport_note+'</span><div class=\"device_link_select\" style=\"display:none\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"device_link_second\" style=\"display:none;z-index:'+(99999-(++device_link_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\"/><input type=\"button\" class=\"ok_btn\" style=\"display:none\"/><input type=\"button\" class=\"delete_btn device_link_delete_'+device_link_delete_no+'\" style = \"display:none\"></td></tr>');\n");
	fprintf(cgiOut, "			//下拉列表生成\n");
	fprintf(cgiOut, "			for(j=0;j<homeport_note_select.length;j++){\n");
	fprintf(cgiOut, "				$(\".device_link_delete_\"+device_link_delete_no).parent().parent().find(\"select.device_link_second\").append('<option>'+homeport_note_select[j]+'</option>');\n");
	fprintf(cgiOut, "			}\n");
	fprintf(cgiOut, "			//修改确定删除操作方法\n");
	fprintf(cgiOut, "			device_link_oprate();\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "//从数据库取出数据，列出已有列表\n");
	fprintf(cgiOut, "window.onload = function (){\n");

//	fprintf(cgiOut, "	var parameter=[{'id':'1','equipment':'灯','room':'客厅','homeport_type':'串口','homeport_note':'192.168.2.1:78','homeport_note_select':['192.168.2.1:78','192.168.2.1:8','192.168.2.1:7','192.168.2.1:781','192.168.2.1:786']},{'id':'2','equipment':'灯','room':'客厅','homeport_type':'串口','homeport_note':'192.168.2.1:78','homeport_note_select':['192.168.2.1:78','192.168.2.1:8','192.168.2.1:7','192.168.2.1:781','192.168.2.1:786']}];\n");
	fprintf(cgiOut, "	var parameter;\n");
	get_host_serial_device_infos();

	fprintf(cgiOut, "	device_link_access_list(parameter);\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "$(function(){\n");
	fprintf(cgiOut, "	//新增按钮\n");
	fprintf(cgiOut, "	$(\"input.new_add_btn\").click(function(){\n");
	fprintf(cgiOut, "		//id为最大值加1\n");
	fprintf(cgiOut, "		var device_link_id;\n");
	fprintf(cgiOut, "		var num=[];\n");
	fprintf(cgiOut, "		$(\".device_link_access_list table.table_serial_port span.device_link_id\").each(function(i,val){\n");
	fprintf(cgiOut, "			num[i]=$(this).html();\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		if(num!=null){\n");
	fprintf(cgiOut, "			device_link_id=parseInt(num.max())+1;\n");
	fprintf(cgiOut, "		}else{\n");
	fprintf(cgiOut, "			device_link_id=1;\n");
	fprintf(cgiOut, "		}if(num == \"\"){\n");
	fprintf(cgiOut, "			device_link_id=1;\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "\n");
	fprintf(cgiOut, "		$(\".device_link_access_list table.table_serial_port\").append('<tr><td width=\"41\"><span class=\"device_link_id\">'+device_link_id+'</span></td><td width=\"113\" align=\"center\"><span class=\"device_link_equipment\" style=\"display:none\"></span><input type=\"text\" class=\"device_link_equipment_revise\"/></td><td width=\"179\" align=\"center\"><span class=\"device_link_room\" style=\"display:none\"></span><input type=\"text\" class=\"device_link_room_revise\"/></td><td width=\"133\" align=\"center\"><span class=\"device_link_homeporttype\">串口</span></td><td width=\"162\" align=\"center\"><span class=\"device_link_homeportnote\" style=\"display:none\"></span><div class=\"device_link_select\"><input type=\"text\" value=\"\"/><select size=\"5\" class=\"device_link_second\" style=\"display:none;z-index:'+(99999-(++device_link_delete_no))+'\"></select></div></td><td width=\"83\" align=\"center\"><input type=\"button\" class=\"revise_btn\" style=\"display:none\"/><input type=\"button\" class=\"ok_btn\"/><input type=\"button\" class=\"delete_btn device_link_delete_'+device_link_delete_no+'\"/></td></tr>');\n");
	fprintf(cgiOut, "		var parameter=[{'id':'1','equipment':'灯','room':'客厅','homeport_type':'串口','homeport_note':'192.168.2.1:78','homeport_note_select':['192.168.2.1:78','192.168.2.1:8','192.168.2.1:7','192.168.2.1:781','192.168.2.1:786']},{'id':'2','equipment':'灯','room':'客厅','homeport_type':'串口','homeport_note':'192.168.2.1:78','homeport_note_select':['192.168.2.1:78','192.168.2.1:8','192.168.2.1:7','192.168.2.1:781','192.168.2.1:786']}];\n");
	fprintf(cgiOut, "		var homeport_note_select=parameter[0].homeport_note_select;\n");
	fprintf(cgiOut, "		//下拉列表生成\n");
	fprintf(cgiOut, "		for(j=0;j<homeport_note_select.length;j++){\n");
	fprintf(cgiOut, "			$(\".device_link_delete_\"+device_link_delete_no).parent().parent().find(\"select.device_link_second\").append('<option>'+homeport_note_select[j]+'</option>');\n");
	fprintf(cgiOut, "		}\n");
	fprintf(cgiOut, "		//修改确定删除操作方法\n");
	fprintf(cgiOut, "		device_link_oprate();\n");
	fprintf(cgiOut, "	});\n");
	fprintf(cgiOut, "	//保存按钮\n");
	fprintf(cgiOut, "	$(\"input.save_btn\").click(function(){\n");
	fprintf(cgiOut, "		document.getElementById(\"save_btn\").style.display=\"none\";\n");
	fprintf(cgiOut, "		//列表的保存\n");
//	fprintf(cgiOut, "		var device_link_save=[];\n");
	fprintf(cgiOut, "		var infos='';\n");
	fprintf(cgiOut, "		$(\".device_link_access_list table.table_serial_port span.device_link_id\").each(function(i,val){\n");
	fprintf(cgiOut, "			var id=$(this).html();//id\n");
	fprintf(cgiOut, "			var equipment=$(this).parent().parent().find(\"span.device_link_equipment\").html();\n");
	fprintf(cgiOut, "			var room=$(this).parent().parent().find(\"span.device_link_room\").html();\n");
	fprintf(cgiOut, "			var homeporttype=$(this).parent().parent().find(\"span.device_link_homeporttype\").html();\n");
	fprintf(cgiOut, "			var homeportnote=$(this).parent().parent().find(\"span.device_link_homeportnote\").html();\n");
//	fprintf(cgiOut, "			device_link_save[i]={'id':id,'equipment':equipment,'room':room,'homeporttype':homeporttype,'homeportnote':homeportnote};\n");
	fprintf(cgiOut, "			infos+=id+','+homeportnote+';'\n");
	fprintf(cgiOut, "		});\n");
	fprintf(cgiOut, "		var info_len=infos.length;\n");
	fprintf(cgiOut, "		var sendAll = 'info_len='+info_len+'&info='+infos;\n");
	fprintf(cgiOut, "		set_comm(sendAll);\n");
	if (TS_PAGE_TYPE_ID_GUIDE == page_type) {
		fprintf(cgiOut, "		jumpPage('%s%s');\n", "../cgi-bin/infrared_device_pathway.html?", TS_PAGE_GUIDE);
	}
	fprintf(cgiOut, "	});\n");

	//保存
	TS_WEB_SAVE("set_comm", "set_comm");

	fprintf(cgiOut, "});\n");
	fprintf(cgiOut, "\n");
	fprintf(cgiOut, "</script>\n");
	fprintf(cgiOut, "\n");
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
		fprintf(cgiOut, "<script type=\"text/javascript\">print_left_guide(%d)</script>\n", TS_LEFT_GUIDE_SERIAL_DEVICE);
	}
	fprintf(cgiOut, "				<!--导航 end-->\n");

//	TS_WEB_MENUS
	fprintf(cgiOut, "				<!--内容 start-->\n");
	fprintf(cgiOut, "				<div class=\"content\">\n");
	fprintf(cgiOut, "					<!--表格 start-->\n");
	fprintf(cgiOut, "					<div class=\"device_link_access_info\">\n");
	fprintf(cgiOut, "						<!--标题 start-->\n");
	fprintf(cgiOut, "						<div class=\"title\">\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\">\n");
	fprintf(cgiOut, "							  <tr>\n");
	fprintf(cgiOut, "								<th width=\"41\">ID</th>\n");
	fprintf(cgiOut, "								<th width=\"113\">设备名称</th>\n");
	fprintf(cgiOut, "								<th width=\"179\">设备所在房间</th>\n");
	fprintf(cgiOut, "								<th width=\"133\">主机口类型</th>\n");
	fprintf(cgiOut, "								<th width=\"162\">串口名</th>\n");
	fprintf(cgiOut, "								<th width=\"83\">操作</th>\n");
	fprintf(cgiOut, "							  </tr>\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--标题 end-->\n");
	fprintf(cgiOut, "						<!--内容 start-->\n");
	fprintf(cgiOut, "						<div class=\"device_link_access_list\" >\n");
	fprintf(cgiOut, "							<table width=\"718\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"table_serial_port\" style=\"border-top:0px\">\n");
	fprintf(cgiOut, "							</table>\n");
	fprintf(cgiOut, "						</div>\n");
	fprintf(cgiOut, "						<!--内容 end-->\n");
	fprintf(cgiOut, "					</div>\n");
	fprintf(cgiOut, "					<!--表格 end-->\n");
	fprintf(cgiOut, "					<!--按钮 start-->\n");
//	fprintf(cgiOut, "					<div class=\"button\"><input id=\"save_btn\" type=\"button\" style=\"display:none\" class=\"save_btn\"/><input type=\"button\" class=\"new_add_btn\" style='display:none'/></div>\n");
	if (TS_PAGE_TYPE_ID_GUIDE != page_type) {
		fprintf(cgiOut, "				<div class=\"button\"><input id=\"save_btn\" type=\"button\" style=\"display:none\" class=\"save_btn\"/><input type=\"button\" class=\"new_add_btn\"  style='display:none'/></div>\n");
	} else {
		fprintf(cgiOut, "					<div class=\"button\">");
		fprintf(cgiOut, "<input  id=\"new_add_btn\" type=\"button\" class=\"new_add_btn\"  style=\"display:none\"/>");
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

	free(comm_ports);

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
