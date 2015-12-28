/**
 * 主机串口信息
 */
// 求数组的最大值
Array.prototype.max = function() {
	var max = this[0];
	var len = this.length;
	for (var i = 1; i < len; i++) {
		if (parseInt(this[i]) > max) {
			max = this[i];
		}
	}
	return max;
}
// 修改确定删除操作
function serial_port_operate() {
	// 修改
	$(".serial_port_info table.table_serial_port input.serial_port_delete_"
			+ serial_port_delete_no).parent().find("input.revise_btn").click(
			function() {
				// 显示隐藏
				$(this).parent().parent().find("span.serial_description")
						.hide();
				$(this).parent().parent().find("span.baud_rate").hide();
				$(this).parent().parent().find("span.data_bit").hide();
				$(this).parent().parent().find("span.stop_bit").hide();
				$(this).parent().parent().find("span.verify").hide();
				$(this).parent().parent().find("span.serial_name").hide();
				$(this).hide();
				$(this).parent().parent()
						.find("input.serial_description_revise").show();
				$(this).parent().parent().find("input.baud_rate_revise").show();
				$(this).parent().parent().find("input.data_bit_revise").show();
				$(this).parent().parent().find("input.stop_bit_revise").show();
				$(this).parent().parent().find("input.verify_revise").show();
				$(this).parent().parent().find("input.serial_name_revise")
						.show();
				$(this).parent().parent().find("input.ok_btn").show();
				// 赋值
				$(this).parent().parent()
						.find("input.serial_description_revise").val($(this)
								.parent().parent()
								.find("span.serial_description").html());
				$(this).parent().parent().find("input.baud_rate_revise")
						.val($(this).parent().parent().find("span.baud_rate")
								.html());
				$(this).parent().parent().find("input.data_bit_revise")
						.val($(this).parent().parent().find("span.data_bit")
								.html());
				$(this).parent().parent().find("input.stop_bit_revise")
						.val($(this).parent().parent().find("span.stop_bit")
								.html());
				$(this).parent().parent().find("input.verify_revise")
						.val($(this).parent().parent().find("span.verify")
								.html());
				$(this).parent().parent().find("input.serial_name_revise")
						.val($(this).parent().parent().find("span.serial_name")
								.html());
			});
	// 确定
	$(".serial_port_info table.table_serial_port input.serial_port_delete_"
			+ serial_port_delete_no).parent().find("input.ok_btn").click(
			function() {
				// 显示隐藏
				$(this).parent().parent()
						.find("input.serial_description_revise").hide();
				$(this).parent().parent().find("input.baud_rate_revise").hide();
				$(this).parent().parent().find("input.data_bit_revise").hide();
				$(this).parent().parent().find("input.stop_bit_revise").hide();
				$(this).parent().parent().find("input.verify_revise").hide();
				$(this).parent().parent().find("input.serial_name_revise")
						.hide();
				$(this).hide();
				$(this).parent().parent().find("span.serial_description")
						.show();
				$(this).parent().parent().find("span.baud_rate").show();
				$(this).parent().parent().find("span.data_bit").show();
				$(this).parent().parent().find("span.stop_bit").show();
				$(this).parent().parent().find("span.verify").show();
				$(this).parent().parent().find("span.serial_name").show();
				$(this).parent().parent().find("input.revise_btn").show();
				// 赋值
				$(this).parent().parent().find("span.serial_description")
						.html($(this).parent().parent()
								.find("input.serial_description_revise").val());
				$(this).parent().parent().find("span.baud_rate")
						.html($(this).parent().parent()
								.find("input.baud_rate_revise").val());
				$(this).parent().parent().find("span.data_bit").html($(this)
						.parent().parent().find("input.data_bit_revise").val());
				$(this).parent().parent().find("span.stop_bit").html($(this)
						.parent().parent().find("input.stop_bit_revise").val());
				$(this).parent().parent().find("span.verify").html($(this)
						.parent().parent().find("input.verify_revise").val());
				$(this).parent().parent().find("span.serial_name").html($(this)
						.parent().parent().find("input.serial_name_revise")
						.val());
			});
	// 删除
	$(".serial_port_info table.table_serial_port input.serial_port_delete_"
			+ serial_port_delete_no).click(function() {
		$(this).parent().parent().remove();
			// id
			// var
			// id=$(this).parent().parent().find("span.serial_port_id").html();
		});
}
var serial_port_delete_no = 0;// 声明主机串口信息列表删除按钮编号
function serial_port_list(parameter) {
	if (parameter != null) {
		for (i = 0; i < parameter.length; i++) {
			var id = parameter[i].id;// id
			var description = parameter[i].description;// 描述
			var baud_rate = parameter[i].baud_rate;// 波特率
			var data_bit = parameter[i].data_bit;// 数据位
			var stop_bit = parameter[i].stop_bit;// 停止位
			var verify = parameter[i].verify;// 校验
			var serial_name = parameter[i].serial_name;// 串口名
			$(".serial_port_info table.table_serial_port")
					.append('<tr><td width="41"><span class="serial_port_id">'
							+ id
							+ '</span></td><td width="149" align="center"><span class="serial_description">'
							+ description
							+ '</span><input type="text" class="serial_description_revise" style="display:none"/></td><td width="70" align="center"><span class="baud_rate">'
							+ baud_rate
							+ '</span><input type="text" class="baud_rate_revise" style="display:none"/></td><td width="70" align="center"><span class="data_bit">'
							+ data_bit
							+ '</span><input type="text" class="data_bit_revise" style="display:none"/></td><td width="70" align="center"><span class="stop_bit">'
							+ stop_bit
							+ '</span><input type="text" class="stop_bit_revise" style="display:none"/></td><td width="70" align="center"><span class="verify">'
							+ verify
							+ '</span><input type="text" class="verify_revise" style="display:none"/></td><td width="89" align="center"><span class="serial_name">'
							+ serial_name
							+ '</span><input type="text" class="serial_name_revise" style="display:none"/></td><td width="83" align="center"><input type="button" class="revise_btn"/><input type="button" class="ok_btn" style="display:none"/><input type="button" class="delete_btn serial_port_delete_'
							+ (++serial_port_delete_no)
							+ '"/></td><td width="66" align="center" valign="middle"><span class="serial_port_switch"><ul><li><input name="serial_port_'
							+ serial_port_delete_no
							+ '" type="radio" value="" checked /> <em>打开</em></li><li><input name="serial_port_'
							+ serial_port_delete_no
							+ '" type="radio" value=""/> <em>关闭</em></li></ul></span></td></tr>');
			// 修改确定删除方法调用
			serial_port_operate()
		}
	}
}
// 从数据库取出数据，列出已有列表
var parameter = [{
			'id' : '1',
			'description' : '描述1',
			'baud_rate' : '12',
			'data_bit' : '1',
			'stop_bit' : '1',
			'verify' : '1',
			'serial_name' : '2',
			'serial_port_switch' : '打开'
		}, {
			'id' : '2',
			'description' : '描述2',
			'baud_rate' : '12',
			'data_bit' : '1',
			'stop_bit' : '1',
			'verify' : '1',
			'serial_name' : '2',
			'serial_port_switch' : '打开'
		}];
window.onload = function() {
	serial_port_list(parameter);
}
$(function() {
	// 新增按钮
	$("input.new_add_btn").click(function() {
		var serial_port_id;// 新增ID号
		// id值最大值加1
		var num = [];
		$(".serial_port_info table.table_serial_port span.serial_port_id")
				.each(function(i, val) {
							num[i] = $(this).html();
						});
		if (num != null) {
			serial_port_id = parseInt(num.max()) + 1;
		} else {
			serial_port_id = 1;
		}
		if (num == "") {
			serial_port_id = 1;
		}
		$(".serial_port_info table.table_serial_port")
				.append('<tr><td width="41"><span class="serial_port_id">'
						+ serial_port_id
						+ '</span></td><td width="149" align="center"><span class="serial_description" style="display:none"></span><input type="text" class="serial_description_revise"/></td><td width="70" align="center"><span class="baud_rate" style="display:none"></span><input type="text" class="baud_rate_revise"/></td><td width="70" align="center"><span class="data_bit" style="display:none"></span><input type="text" class="data_bit_revise"/></td><td width="70" align="center"><span class="stop_bit" style="display:none"></span><input type="text" class="stop_bit_revise"/></td><td width="70" align="center"><span class="verify" style="display:none"></span><input type="text" class="verify_revise"/></td><td width="89" align="center"><span class="serial_name" style="display:none"></span><input type="text" class="serial_name_revise"/></td><td width="83" align="center"><input type="button" class="revise_btn" style="display:none"/><input type="button" class="ok_btn"/><input type="button" class="delete_btn serial_port_delete_'
						+ (++serial_port_delete_no)
						+ '"/></td><td width="66" align="center" valign="middle"><span class="serial_port_switch"><ul><li><input name="serial_port_'
						+ serial_port_delete_no
						+ '" type="radio" value="" checked /> <em>打开</em></li><li><input name="serial_port_'
						+ serial_port_delete_no
						+ '" type="radio" value=""/> <em>关闭</em></li></ul></span></td></tr>');
		// 修改确定删除方法调用
		serial_port_operate()
	});
	// 保存按钮
	$("input.save_btn").click(function() {
		// 列表的保存
		var serial_port_save = [];
		$(".serial_port_info table.table_serial_port span.serial_port_id")
				.each(function(i, val) {
					var id = $(this).html();// id
					var description = $(this).parent().parent()
							.find("span.serial_description").html();
					var baud_rate = $(this).parent().parent()
							.find("span.baud_rate").html();
					var data_bit = $(this).parent().parent()
							.find("span.data_bit").html();
					var stop_bit = $(this).parent().parent()
							.find("span.stop_bit").html();
					var verify = $(this).parent().parent().find("span.verify")
							.html();
					var serial_name = $(this).parent().parent()
							.find("span.serial_name").html();
					serial_port_save[i] = {
						'id' : id,
						'description' : description,
						'baud_rate' : baud_rate,
						'data_bit' : data_bit,
						'stop_bit' : stop_bit,
						'verify' : verify,
						'serial_name' : serial_name
					};
				});
	});
});