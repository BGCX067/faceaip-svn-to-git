/**
 *红外设备通路配置
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
var infrared_device_delete_no=0;//声明设备连接通路删除按钮编号
//修改确定删除操作方法
function infrared_device_oprate(){
	//下拉列表选择
	$(".infrared_device_delete_"+infrared_device_delete_no).parent().parent().find(".infrared_device_pathway_select").mouseover(function(){
		$(this).find("select.infrared_device_pathway_second").show();
	});
	$(".infrared_device_delete_"+infrared_device_delete_no).parent().parent().find(".infrared_device_pathway_select").mouseout(function(){
		$(this).find("select.infrared_device_pathway_second").hide();
	});
	$(".infrared_device_delete_"+infrared_device_delete_no).parent().parent().find("select.infrared_device_pathway_second").click(function(){
		var select_value="";
		$(this).find("option").each(function(i,val){
			if($(this).attr("selected")=="selected"){
				select_value=select_value+$(this).html();
			}
		});
		$(this).parent().find("input").val(select_value);
		$(this).hide();
	});
	//修改
	$(".infrared_device_delete_"+infrared_device_delete_no).parent().parent().find("input.revise_btn").click(function(){
		//显示隐藏
		$(this).parent().parent().find("span.infrared_device_pathway_no").hide();
		$(this).hide();
		$(this).parent().parent().find(".infrared_device_pathway_select").show();
		$(this).parent().parent().find("input.ok_btn").show();
		//赋值
		$(this).parent().parent().find(".infrared_device_pathway_select input").val($(this).parent().parent().find("span.infrared_device_pathway_no").html());
	});
	//确定
	$(".infrared_device_delete_"+infrared_device_delete_no).parent().parent().find("input.ok_btn").click(function(){
		//显示隐藏
		$(this).parent().parent().find(".infrared_device_pathway_select").hide();
		$(this).hide();
		$(this).parent().parent().find("span.infrared_device_name").show();
		$(this).parent().parent().find("span.infrared_device_pathway_no").show();
		$(this).parent().parent().find("input.revise_btn").show();
		//赋值
		$(this).parent().parent().find("span.infrared_device_pathway_no").html($(this).parent().parent().find(".infrared_device_pathway_select input").val());
		if($(this).parent().parent().find("input.infrared_device_name_revise").css("display")=="none"){
		}else{
			$(this).parent().parent().find("input.infrared_device_name_revise").hide();
			$(this).parent().parent().find("span.infrared_device_name").html($(this).parent().parent().find("input.infrared_device_name_revise").val());
		}
	});
	//删除
	$(".infrared_device_delete_"+infrared_device_delete_no).parent().parent().find("input.delete_btn").click(function(){
		$(this).parent().parent().remove();
		//id
		//var id=$(this).parent().parent().find("span.infrared_device_id").html();
	});
}
//生成已有列表 
function infrared_device_list(parameter){
	if(parameter != null){
		for(i=0;i<parameter.length;i++){
			var id=parameter[i].id;
			var infrared_device_name=parameter[i].infrared_device_name;
			var infrared_device_pathway_no=parameter[i].infrared_device_pathway_no;
			var infrared_device_pathway_select=parameter[i].infrared_device_pathway_select;
			$(".infrared_device_list table.table_serial_port").append('<tr><td width="41"><span class="infrared_device_id">'+id+'</span></td><td width="250" align="center"><span class="infrared_device_name">'+infrared_device_name+'</span></td><td width="228" align="center"><span class="infrared_device_pathway_no">'+infrared_device_pathway_no+'</span><div class="infrared_device_pathway_select" style="display:none"><input type="text" value=""/><select size="5" class="infrared_device_pathway_second" style="display:none;z-index:'+(9999-(++infrared_device_delete_no))+'"></select></div></td><td width="83" align="center"><input type="button" class="revise_btn"/><input type="button" class="ok_btn" style="display:none"/><input type="button" class="delete_btn infrared_device_delete_'+infrared_device_delete_no+'"/></td><td width="110" align="center"><input name="infrared_device_'+infrared_device_delete_no+'" type="radio" value="" checked /> 打开 <input name="infrared_device_'+infrared_device_delete_no+'" type="radio" value=""/> 关闭</td></tr>');
			//下拉列表生成
			for(j=0;j<infrared_device_pathway_select.length;j++){
				$(".infrared_device_delete_"+infrared_device_delete_no).parent().parent().find("select.infrared_device_pathway_second").append('<option>'+infrared_device_pathway_select[j]+'</option>');
			}
			//修改确定删除操作方法
			infrared_device_oprate();
		}
	}
}
//从数据库取出数据，列出已有列表
window.onload = function (){
	var parameter=[{'id':'1','infrared_device_name':'快进','infrared_device_pathway_no':'1','infrared_device_pathway_select':['1','2','3','4','5']},{'id':'2','infrared_device_name':'快进','infrared_device_pathway_no':'1','infrared_device_pathway_select':['1','2','3','4','5']}];
	infrared_device_list(parameter);
}

$(function(){
	//新增按钮
	$("input.new_add_btn").click(function(){
		//id为最大值加1
		var infrared_device_id;
		var num=[];
		$(".infrared_device_list table.table_serial_port span.infrared_device_id").each(function(i,val){
			num[i]=$(this).html();
		});
		if(num!=null){
			infrared_device_id=parseInt(num.max())+1;
		}else{
			infrared_device_id=1;
		}
		$(".infrared_device_list table.table_serial_port").append('<tr><td width="41"><span class="infrared_device_id">'+infrared_device_id+'</span></td><td width="250" align="center"><span class="infrared_device_name" style="display:none"></span><input type="text" class="infrared_device_name_revise"/></td><td width="228" align="center"><span class="infrared_device_pathway_no" style="display:none"></span><div class="infrared_device_pathway_select"><input type="text" value=""/><select size="5" class="infrared_device_pathway_second" style="display:none;z-index:'+(9999-(++infrared_device_delete_no))+'"></select></div></td><td width="83" align="center"><input type="button" class="revise_btn" style="display:none"/><input type="button" class="ok_btn"/><input type="button" class="delete_btn infrared_device_delete_'+infrared_device_delete_no+'"/></td><td width="110" align="center"><input name="infrared_device_'+infrared_device_delete_no+'" type="radio" value="" checked /> 打开 <input name="infrared_device_'+infrared_device_delete_no+'" type="radio" value=""/> 关闭</td></tr>');
		var parameter=[{'id':'1','infrared_device_name':'快进','infrared_device_pathway_no':'1','infrared_device_pathway_select':['1','2','3','4','5']},{'id':'2','infrared_device_name':'快进','infrared_device_pathway_no':'1','infrared_device_pathway_select':['1','2','3','4','5']}];
		var infrared_device_pathway_select=parameter[0].infrared_device_pathway_select;
		//下拉列表生成
		for(j=0;j<infrared_device_pathway_select.length;j++){
			$(".infrared_device_delete_"+infrared_device_delete_no).parent().parent().find("select.infrared_device_pathway_second").append('<option>'+infrared_device_pathway_select[j]+'</option>');
		}
		//修改确定删除操作方法
		infrared_device_oprate();
	});
	//保存按钮
	$("input.save_btn").click(function(){
		//列表的保存
		var infrared_device_save=[];
		$(".infrared_device_list table.table_serial_port span.infrared_device_id").each(function(i,val){
			var id=$(this).html();//id
			var infrared_device_name=$(this).parent().parent().find("span.infrared_device_name").html();
			var infrared_device_pathway_no=$(this).parent().parent().find("span.infrared_device_pathway_no").html();
			infrared_device_save[i]={'id':id,'infrared_device_name':infrared_device_name,'infrared_device_pathway_no':infrared_device_pathway_no};
		});
	});
});