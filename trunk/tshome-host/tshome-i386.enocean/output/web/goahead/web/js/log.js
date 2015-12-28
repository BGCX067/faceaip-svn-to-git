/**
 *日志信息
 */
//生成已有列表
function check_list(parameter){
	if(parameter != null){
		for(i=0;i<parameter.length;i++){
			var check_time=parameter[i].check_time;
			var check_level=parameter[i].check_level;
			var check_event=parameter[i].check_event;
			$(".check_list table.table_serial_port").append('<tr align="center"><td width="124"><span class="check_time">'+check_time+'</span></td><td width="160"><span class="check_level">'+check_level+'</span></td><td width="430"><span class="check_event">'+check_event+'</span></td></tr>');
		}
	}
}
//从数据库取出数据，列出已有列表
window.onload = function (){
	var parameter=[{'check_time':'2013-10-23 10:20:30','check_level':'级别','check_event':'事件'},{'check_time':'2013-10-23 10:20:30','check_level':'级别','check_event':'事件'}];
	check_list(parameter);
}
$(function(){
	//级别选择下拉
	$(".rank_select").mouseover(function(){
		$(".rank_second").show();
	});
	$(".rank_select").mouseout(function(){
		$(".rank_second").hide();
	});
	$(".rank_second ul li").click(function(){//下拉点击
		$(".rank_second ul li").removeClass("blue");
		$(this).addClass("blue");
		$(".rank_select input").val($(this).html());
		$(".rank_second").hide();
	});
	//时间日历
	function updateFields(cal) {
		  var date = cal.selection.get();
		  if (date) {
				  date = Calendar.intToDate(date);
				  document.getElementById("start_date").value = Calendar.printDate(date, "%Y-%m-%d "+cal.getHours()+":"+cal.getMinutes()+":00");
				  cal.hide();
		  }
	};
	function updateField(ca2) {
		  var date = ca2.selection.get();
		  if (date) {
				  date = Calendar.intToDate(date);
				  document.getElementById("stop_date").value = Calendar.printDate(date, "%Y-%m-%d "+ca2.getHours()+":"+ca2.getMinutes()+":00");
				  ca2.hide();
		  }
	};
	Calendar.setup({
	showTime: 24,
	inputField: "start_date",
	trigger: "start_date",
	onSelect:updateFields
	});
	Calendar.setup({
	showTime: 24,
	inputField: "stop_date",
	trigger: "stop_date",
	onSelect:updateField
	});
});