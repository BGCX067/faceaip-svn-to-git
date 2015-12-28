/**
 *串口设备连接主机信息维护
 */
//求数组的最大值
Array.prototype.max = function() {  
 var max = this[0];
 var len = this.length; 
 for (var i = 1; i < len; i++){
  if (parseInt(this[i]) > max) { 
   max = this[i];   
  } 
 }   
 return max;
}
var device_link_delete_no=0;//声明设备连接通路删除按钮编号
//修改确定删除操作方法
function device_link_oprate(){
	//下拉列表选择
	$(".device_link_delete_"+device_link_delete_no).parent().parent().find(".device_link_select").mouseover(function(){
		$(this).find(".device_link_second").show();
	});
	$(".device_link_delete_"+device_link_delete_no).parent().parent().find(".device_link_select").mouseout(function(){
		$(this).find(".device_link_second").hide();
	});
	$(".device_link_delete_"+device_link_delete_no).parent().parent().find("select.device_link_second").click(function(){
		var select_value="";
		$(this).find("option").each(function(i,val){
			if($(this).attr("selected")=="selected"){
				select_value=select_value+$(this).html();
			}
		});
		$(this).parent().find("input").val(select_value);
	});
	//修改
	$(".device_link_delete_"+device_link_delete_no).parent().parent().find("input.revise_btn").click(function(){
		//显示隐藏
		$(this).parent().parent().find("span.device_link_homeportnote").hide();
		$(this).hide();
		$(this).parent().parent().find(".device_link_select").show();
		$(this).parent().parent().find("input.ok_btn").show();
		//赋值
		$(this).parent().parent().find(".device_link_select input").val($(this).parent().parent().find("span.device_link_homeportnote").html());
	});
	//确定
	$(".device_link_delete_"+device_link_delete_no).parent().parent().find("input.ok_btn").click(function(){
		//显示隐藏
		$(this).parent().parent().find(".device_link_select").hide();
		$(this).hide();
		$(this).parent().parent().find("span.device_link_equipment").show();
		$(this).parent().parent().find("span.device_link_room").show();
		$(this).parent().parent().find("span.device_link_homeportnote").show();
		$(this).parent().parent().find("input.revise_btn").show();
		//赋值
		$(this).parent().parent().find("span.device_link_homeportnote").html($(this).parent().parent().find(".device_link_select input").val());
		if($(this).parent().parent().find("input.device_link_equipment_revise").css("display")=="none"){
		}else{
			$(this).parent().parent().find("input.device_link_equipment_revise").hide();
			$(this).parent().parent().find("span.device_link_equipment").html($(this).parent().parent().find("input.device_link_equipment_revise").val());
		}
		if($(this).parent().parent().find("input.device_link_room_revise").css("display")=="none"){
		}else{
			$(this).parent().parent().find("input.device_link_room_revise").hide();
			$(this).parent().parent().find("span.device_link_room").html($(this).parent().parent().find("input.device_link_room_revise").val());
		}
	});
	//删除
	$(".device_link_delete_"+device_link_delete_no).parent().parent().find("input.delete_btn").click(function(){
		$(this).parent().parent().remove();
		//id
		//var id=$(this).parent().parent().find("span.device_link_id").html();
	});
}
//生成已有列表 
function device_link_access_list(parameter){
	if(parameter != null){
		for(i=0;i<parameter.length;i++){
			var id=parameter[i].id;
			var equipment=parameter[i].equipment;
			var room=parameter[i].room;
			var homeport_type=parameter[i].homeport_type;
			var homeport_note=parameter[i].homeport_note;
			var homeport_note_select=parameter[i].homeport_note_select;
			$(".device_link_access_list table.table_serial_port").append('<tr><td width="41"><span class="device_link_id">'+id+'</span></td><td width="113" align="center"><span class="device_link_equipment">'+equipment+'</span></td><td width="179" align="center"><span class="device_link_room">'+room+'</span></td><td width="133" align="center"><span class="device_link_homeporttype">'+homeport_type+'</span></td><td width="162" align="center"><span class="device_link_homeportnote">'+homeport_note+'</span><div class="device_link_select" style="display:none"><input type="text" value=""/><select size="5" class="device_link_second" style="display:none;z-index:'+(99999-(++device_link_delete_no))+'"></select></div></td><td width="83" align="center"><input type="button" class="revise_btn"/><input type="button" class="ok_btn" style="display:none"/><input type="button" class="delete_btn device_link_delete_'+device_link_delete_no+'"/></td></tr>');
			//下拉列表生成
			for(j=0;j<homeport_note_select.length;j++){
				$(".device_link_delete_"+device_link_delete_no).parent().parent().find("select.device_link_second").append('<option>'+homeport_note_select[j]+'</option>');
			}
			//修改确定删除操作方法
			device_link_oprate();
		}
	}
}
//从数据库取出数据，列出已有列表
window.onload = function (){
	var parameter=[{'id':'1','equipment':'灯','room':'客厅','homeport_type':'串口','homeport_note':'192.168.2.1:78','homeport_note_select':['192.168.2.1:78','192.168.2.1:8','192.168.2.1:7','192.168.2.1:781','192.168.2.1:786']},{'id':'2','equipment':'灯','room':'客厅','homeport_type':'串口','homeport_note':'192.168.2.1:78','homeport_note_select':['192.168.2.1:78','192.168.2.1:8','192.168.2.1:7','192.168.2.1:781','192.168.2.1:786']}];
	device_link_access_list(parameter);
}
$(function(){
	//新增按钮
	$("input.new_add_btn").click(function(){
		//id为最大值加1
		var device_link_id;
		var num=[];
		$(".device_link_access_list table.table_serial_port span.device_link_id").each(function(i,val){
			num[i]=$(this).html();
		});
		if(num!=null){
			device_link_id=parseInt(num.max())+1;
		}else{
			device_link_id=1;
		}
		$(".device_link_access_list table.table_serial_port").append('<tr><td width="41"><span class="device_link_id">'+device_link_id+'</span></td><td width="113" align="center"><span class="device_link_equipment" style="display:none"></span><input type="text" class="device_link_equipment_revise"/></td><td width="179" align="center"><span class="device_link_room" style="display:none"></span><input type="text" class="device_link_room_revise"/></td><td width="133" align="center"><span class="device_link_homeporttype">串口</span></td><td width="162" align="center"><span class="device_link_homeportnote" style="display:none"></span><div class="device_link_select"><input type="text" value=""/><select size="5" class="device_link_second" style="display:none;z-index:'+(99999-(++device_link_delete_no))+'"></select></div></td><td width="83" align="center"><input type="button" class="revise_btn" style="display:none"/><input type="button" class="ok_btn"/><input type="button" class="delete_btn device_link_delete_'+device_link_delete_no+'"/></td></tr>');
		var parameter=[{'id':'1','equipment':'灯','room':'客厅','homeport_type':'串口','homeport_note':'192.168.2.1:78','homeport_note_select':['192.168.2.1:78','192.168.2.1:8','192.168.2.1:7','192.168.2.1:781','192.168.2.1:786']},{'id':'2','equipment':'灯','room':'客厅','homeport_type':'串口','homeport_note':'192.168.2.1:78','homeport_note_select':['192.168.2.1:78','192.168.2.1:8','192.168.2.1:7','192.168.2.1:781','192.168.2.1:786']}];
		var homeport_note_select=parameter[0].homeport_note_select;
		//下拉列表生成
		for(j=0;j<homeport_note_select.length;j++){
			$(".device_link_delete_"+device_link_delete_no).parent().parent().find("select.device_link_second").append('<option>'+homeport_note_select[j]+'</option>');
		}
		//修改确定删除操作方法
		device_link_oprate();
	});
	//保存按钮
	$("input.save_btn").click(function(){
		//列表的保存
		var device_link_save=[];
		$(".device_link_access_list table.table_serial_port span.device_link_id").each(function(i,val){
			var id=$(this).html();//id
			var equipment=$(this).parent().parent().find("span.device_link_equipment").html();
			var room=$(this).parent().parent().find("span.device_link_room").html();
			var homeporttype=$(this).parent().parent().find("span.device_link_homeporttype").html();
			var homeportnote=$(this).parent().parent().find("span.device_link_homeportnote").html();
			device_link_save[i]={'id':id,'equipment':equipment,'room':room,'homeporttype':homeporttype,'homeportnote':homeportnote};
		});
	});
});
