/**
 *通路信息维护
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
var gateway_no=0;//声明网关开关名编号
//网关
function gateway_list(parameter){
	if(parameter != null){
		for(i=0;i<parameter.length;i++){
			var id=parameter[i].id;
			var ip=parameter[i].ip;
			var port=parameter[i].port;
			var type=parameter[i].type;
			$(".gateway_info_list table.table_serial_port").append('<tr align="center"><td width="41"><span class="gateway_id">'+id+'</span></td><td width="298">'+ip+'</td><td width="153">'+port+'</td><td width="110"><span class="gateway_type">'+type+'</span></td><td width="110"><input name="gateway_'+(++gateway_no)+'" type="radio" value="" checked /> 打开 <input name="gateway_'+gateway_no+'" type="radio" value=""/> 关闭</td></tr>');
		}
	}
}
//从数据库取出数据，列出已有列表
window.onload = function (){
	var parameter=[{'id':'1','ip':'192.168.0.1','port':'12','type':'KNX'},{'id':'2','ip':'192.168.0.1','port':'12','type':'KNX'}];
	gateway_list(parameter);//网关
	var parameter=[{'id':'1','note':'192.168.10.10:68','description':'手势操作让用户的操作空间不再局限于界','gateway':'1,2,','gateway_select':['1','2','3','4','5']},{'id':'2','note':'192.168.10.10:68','description':'手势操作让用户的操作空间不再局限于界','gateway':'3,','gateway_select':['1','2','3','4','5']}];
	homeport_list(parameter);//主机口	
}
//主机口
var homeport_delete_no=0;//声明主机口删除按钮编号
//主机口修改确定删除操作方法
function homeport_option(){
	//下拉多选
	$(".homeport_delete_"+homeport_delete_no).parent().parent().find("span.homeport_gateway_select").mouseover(function(){
		$(this).find(".homeport_gateway_second").show();
	});
	$(".homeport_delete_"+homeport_delete_no).parent().parent().find("span.homeport_gateway_select").mouseout(function(){
		$(this).find(".homeport_gateway_second").hide();
	});
	$(".homeport_delete_"+homeport_delete_no).parent().parent().find("select.homeport_gateway_second").click(function(){
		var selected_value="";				
		var select_value="";
		$("span.home_port_gateway").each(function(i,val){
			if($(this).css("display")=="block"){
				selected_value=selected_value+$(this).html();
			}
		});
		$(this).find("option").each(function(i,val){
				if($(this).attr("selected")=="selected"){
					if(selected_value.indexOf($(this).html())>=0){
						$(this).removeAttr("selected");
					}else{
						select_value=select_value+$(this).html()+",";
					}
				}
		});
		$(this).parent().find("input").val(select_value);
	});
	//修改
	$(".homeport_delete_"+homeport_delete_no).parent().parent().find("input.revise_btn").click(function(){
		//显示隐藏
		$(this).parent().parent().find("span.home_port_note").hide();
		$(this).parent().parent().find("span.home_port_description").hide();
		$(this).parent().parent().find("span.home_port_gateway").hide();
		$(this).hide();
		$(this).parent().parent().find("input.home_port_note_revise").show();
		$(this).parent().parent().find("input.home_port_description_revise").show();
		$(this).parent().parent().find("span.homeport_gateway_select").show();
		$(this).parent().parent().find("input.ok_btn").show();
		//赋值
		$(this).parent().parent().find("input.home_port_note_revise").val($(this).parent().parent().find("span.home_port_note").html());
		$(this).parent().parent().find("input.home_port_description_revise").val($(this).parent().parent().find("span.home_port_description").html());
		$(this).parent().parent().find("span.homeport_gateway_select input").val($(this).parent().parent().find("span.home_port_gateway").html());
	});
	//确定
	$(".homeport_delete_"+homeport_delete_no).parent().parent().find("input.ok_btn").click(function(){
		//显示隐藏
		$(this).parent().parent().find("input.home_port_note_revise").hide();
		$(this).parent().parent().find("input.home_port_description_revise").hide();
		$(this).parent().parent().find("span.homeport_gateway_select").hide();
		$(this).hide();
		$(this).parent().parent().find("span.home_port_note").show();
		$(this).parent().parent().find("span.home_port_description").show();
		$(this).parent().parent().find("span.home_port_gateway").show();
		$(this).parent().parent().find("input.revise_btn").show();
		//赋值
		$(this).parent().parent().find("span.home_port_note").html($(this).parent().parent().find("input.home_port_note_revise").val());
		$(this).parent().parent().find("span.home_port_description").html($(this).parent().parent().find("input.home_port_description_revise").val());
		$(this).parent().parent().find("span.home_port_gateway").html($(this).parent().parent().find("span.homeport_gateway_select input").val());
	});
	//删除
	$(".homeport_delete_"+homeport_delete_no).click(function(){
		$(this).parent().parent().remove();
		//id
		//var id=$(this).parent().parent().find("span.home_port_id").html();
	});
}
//主机口生成已有列表
function homeport_list(parameter){
	if(parameter != null){
		for(i=0;i<parameter.length;i++){
			var id=parameter[i].id;
			var note=parameter[i].note;
			var description=parameter[i].description;
			var gateway=parameter[i].gateway;
			var gateway_select=parameter[i].gateway_select;
			$(".home_port_list table.table_serial_port").append('<tr align="center"><td width="41"><span class="home_port_id">'+id+'</span></td><td width="186"><span class="home_port_note">'+note+'</span><input type="text" class="home_port_note_revise" style="display:none"/></td><td width="292"><span class="home_port_description">'+description+'</span><input type="text" class="home_port_description_revise" style="display:none"/></td><td width="110"><span class="home_port_gateway">'+gateway+'</span><span class="homeport_gateway_select" style="display:none"><input type="text" value=""/><select class="homeport_gateway_second" size="5" multiple="multiple" style="display:none;z-index:'+(9999-(++homeport_delete_no))+'"></select></span></td><td width="83"><input type="button" class="revise_btn"/><input type="button" class="ok_btn" style="display:none"/><input type="button" class="delete_btn homeport_delete_'+homeport_delete_no+'"/></td></tr>');
			//下拉列表生成
			for(j=0;j<gateway_select.length;j++){
				$(".homeport_delete_"+homeport_delete_no).parent().parent().find("select.homeport_gateway_second").append('<option>'+gateway_select[j]+'</option>');
			}
			//调用操作方法
			homeport_option();
		}
	}
}
$(function(){
	//新增按钮
	$("input.new_add_btn").click(function(){
		//id号为最大值加1
		var home_port_id;
		var num=[];
		$(".home_port_list table.table_serial_port span.home_port_id").each(function(i,val){
			num[i]=$(this).html();
		});
		if(num!=null){
			home_port_id=parseInt(num.max())+1;
		}else{
			home_port_id=1;
		}
		$(".home_port_list table.table_serial_port").append('<tr align="center"><td width="41"><span class="home_port_id">'+home_port_id+'</span></td><td width="186"><span class="home_port_note" style="display:none"></span><input type="text" class="home_port_note_revise"/></td><td width="292"><span class="home_port_description" style="display:none"></span><input type="text" class="home_port_description_revise"/></td><td width="110"><span class="home_port_gateway" style="display:none"></span><span class="homeport_gateway_select"><input type="text" value=""/><select class="homeport_gateway_second" size="5" multiple="multiple" style="display:none;z-index:'+(9999-(++homeport_delete_no))+'"></select></span></td><td width="83"><input type="button" class="revise_btn" style="display:none"/><input type="button" class="ok_btn"/><input type="button" class="delete_btn homeport_delete_'+homeport_delete_no+'"/></td></tr>');
		var parameter=[{'id':'1','note':'192.168.10.10:68','description':'手势操作让用户的操作空间不再局限于界','gateway':'1,2,','gateway_select':['1','2','3','4','5']},{'id':'2','note':'192.168.10.10:68','description':'手势操作让用户的操作空间不再局限于界','gateway':'3,','gateway_select':['1','2','3','4','5']}];
		var gateway_select=parameter[0].gateway_select;
		//下拉列表生成
		for(j=0;j<gateway_select.length;j++){
			$(".homeport_delete_"+homeport_delete_no).parent().parent().find("select.homeport_gateway_second").append('<option>'+gateway_select[j]+'</option>');
		}
		//调用操作方法
		homeport_option();
	});
	//保存按钮
	$("input.save_btn").click(function(){
		//列表的保存
		var home_port_save=[];
		$(".home_port_list table.table_serial_port span.home_port_id").each(function(i,val){
			var id=$(this).html();//id
			var note=$(this).parent().parent().find("span.home_port_note").html();
			var description=$(this).parent().parent().find("span.home_port_description").html();
			var gateway=$(this).parent().parent().find("span.home_port_gateway").html();
			home_port_save[i]={'id':id,'note':note,'description':description,'gateway':gateway};
		});
	});
});