function parse_ip(ip) {
	var ips = "";
	if (null != ip) {
		ips = ip.split(".", 4);
	}
	return ips;
}

$(function() {
	$(".center .content .title a").click(function() {
				$(".center .content .title a").removeClass("selected");
				$(this).addClass("selected");
			});

	// net info
	$("input.home_revise_btn").click(function() {
		$(this).parent().parent().parent().parent().find(".home_ip_show").attr(
				"style", "display:none");
		$(this).parent().parent().parent().parent().find(".home_ip_edit").attr(
				"style", "display");
		$(this).hide();
		$(this).parent().parent().parent().parent().find("input.home_save_btn")
				.show();

		var ips = "";

		// 传值
		// gw
		ips = parse_ip($(this).parent().parent().parent().parent()
				.find(".home_note_gw").html());
		for (var i = 0; i < 4; i++) {
			$(this).parent().parent().parent().parent().find("input.home_gw_"
					+ i).attr("value", ips[i]);
		}

		// ip
		ips = parse_ip($(this).parent().parent().parent().parent()
				.find(".home_note_ip").html());
		for (var i = 0; i < 4; i++) {
			$(this).parent().parent().parent().parent().find("input.home_ip_"
					+ i).attr("value", ips[i]);
		}

		// net mask
		ips = parse_ip($(this).parent().parent().parent().parent()
				.find(".home_note_net_mask").html());
		for (var i = 0; i < 4; i++) {
			$(this).parent().parent().parent().parent()
					.find("input.home_net_mask_" + i).attr("value", ips[i]);
		}

		// dns
		if (null != $(this).parent().parent().parent().parent()
				.find(".home_note_dns")) {
			ips = parse_ip($(this).parent().parent().parent().parent()
					.find(".home_note_dns").html());
			for (var i = 0; i < 4; i++) {
				$(this).parent().parent().parent().parent()
						.find("input.home_dns_" + i).attr("value", ips[i]);
			}
		}

		$(this).parent().parent().parent().parent().find("input.wireless_ssid")
				.val($(this).parent().parent().parent().parent()
						.find(".wireless_adapter_ssid").html());
		$(this).parent().parent().parent().parent()
				.find("input.wireless_password").val($(this).parent().parent()
						.parent().parent().find(".wireless_adapter_password")
						.html());
	});

	$("input.home_save_btn").click(function() {
		// 显示隐藏
		$(this).parent().parent().parent().parent().find(".home_ip_show").attr(
				"style", "display");
		$(this).parent().parent().parent().parent().find(".home_ip_edit")
				.hide();
		// 传值
		// ip
		var ip = "";
		for (var i = 0; i < 4; i++) {
			ip += $(this).parent().parent().parent().parent()
					.find("input.home_ip_" + i).attr("value");
			if (i != 3) {
				ip += ".";
			}
		}
		$(this).parent().parent().parent().parent().find(".home_edit_ip").attr(
				"value", ip);
		$(this).parent().parent().parent().parent().find(".home_note_ip")
				.html(ip);

		// gw
		ip = "";
		for (var i = 0; i < 4; i++) {
			ip += $(this).parent().parent().parent().parent()
					.find("input.home_gw_" + i).attr("value");
			if (i != 3) {
				ip += ".";
			}
		}
		$(this).parent().parent().parent().parent().find(".home_edit_gw").attr(
				"value", ip);
		$(this).parent().parent().parent().parent().find(".home_note_gw")
				.html(ip);

		// net_mask
		ip = "";
		for (var i = 0; i < 4; i++) {
			ip += $(this).parent().parent().parent().parent()
					.find("input.home_net_mask_" + i).attr("value");
			if (i != 3) {
				ip += ".";
			}
		}
		$(this).parent().parent().parent().parent().find(".home_edit_net_mask")
				.attr("value", ip);
		$(this).parent().parent().parent().parent().find(".home_note_net_mask")
				.html(ip);

		// dns
		if (null != $(this).parent().parent().parent().parent()
				.find(".home_note_dns")) {
			ip = "";
			for (var i = 0; i < 4; i++) {
				ip += $(this).parent().parent().parent().parent()
						.find("input.home_dns_" + i).attr("value");
				if (i != 3) {
					ip += ".";
				}
			}
			$(this).parent().parent().parent().parent().find(".home_edit_dns")
					.attr("value", ip);
			$(this).parent().parent().parent().parent().find(".home_note_dns")
					.html(ip);
		}
	});

	// });

	// 支持内外网控制人数//3-27
	$("input.net_revise_btn").click(function() {
				// 显示隐藏
				$(".net_number_inner").hide();
				$(".net_number_outer").hide();
				$("input.net_number_input").show();
				$(this).hide();
				$("input.net_save_btn").show();
				// 传值
				$("input.net_inner").val($(".net_number_inner").html());
				$("input.net_outer").val($(".net_number_outer").html());
			});

	$("input.net_save_btn").click(function() {
				// 显示隐藏
				$(".net_number_inner").show();
				$(".net_number_outer").show();
				$("input.net_number_input").hide();
				$(this).hide();
				$("input.net_revise_btn").show();
				// 传值
				$(".net_number_inner").html($("input.net_inner").val());
				$(".net_number_outer").html($("input.net_outer").val());
			});

	// update
	$("input.receive_btn").click(function() {
		$('body')
				.append('<div class="popupbg"></div><div class="popupbox"><div class="popupbox_bg"><div class="popupbox_btn_ok"><div class="popupbox_btn_cancel"><div class="popupboxbackground"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr height="28"><td></td></tr><tr height="75"><td>更新需要大约2分钟。</br>需要您手动刷新，是否确定?</td></tr><tr height="40"><td><input type="button" class="popup_receive_ok" value="确定"/><input type="button" class="popup_receive_cancel" value="取消"/></td></tr></table></div></div></div></div></div>');
		$("input.popup_receive_ok").click(function() {// 确认
					// 更新
					$(".popupbg").remove();
					$(".popupbox").remove();
					control_host_time('info=12&', 0, 120000);
				});
		$("input.popup_receive_cancel").click(function() {// 取消
					// 删除对话框
					$(".popupbg").remove();
					$(".popupbox").remove();
				});
	});

	// guide_update
	$("input.get_conf_btn").click(function() {
		$('body')
				.append('<div class="popupbg"></div><div class="popupbox"><div class="popupbox_bg"><div class="popupbox_btn_ok"><div class="popupbox_btn_cancel"><div class="popupboxbackground"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr height="28"><td></td></tr><tr height="75"><td>更新需要大约3分钟,是否确定?</td></tr><tr height="40"><td><input type="button" class="popup_receive_ok" value="确定"/><input type="button" class="popup_receive_cancel" value="取消"/></td></tr></table></div></div></div></div></div>');
		$("input.popup_receive_ok").click(function() {// 确认
					// 更新
					$(".popupbg").remove();
					$(".popupbox").remove();
//					control_host_time('info=15&', 0, 3000);
					control_host('info=15&', 0);
					// $(".next_btn").show();
				});
		$("input.popup_receive_cancel").click(function() {// 取消
					// 删除对话框
					$(".popupbg").remove();
					$(".popupbox").remove();
				});
	});

	// download log
	$("input.receive_log_btn").click(function() {
				control_host_time('info=13&', 0, 3000);
			});

	// update
	$("input.receive_btn_no_run").click(function() {
				alert("主机程序未启动，该功能无法实现");
			});

	// 用户信息
	$("input.user_info_ok_btn").click(function() {
		// 取值
		var old_mima = $(".user_old_password").val();
		var new_mima = $(".user_new_password").val();
		var new_mima_second = $(".user_new_password_second").val();
		// 如果提交成功，提示信息
		// if(){
		// 提示信息
		// alert("您的密码修改成功！");
		// 清空文本框
		$(".user_old_password").val("");
		$(".user_new_password").val("");
		$(".user_new_password_second").val("");
			// }
		});

	// SEVER信息
	$("input.login_submit_btn").click(function() {
		// 取值
		var login_username = $(this).parent().parent().parent().parent()
				.find(".login_username").val();
		var login_password = $(this).parent().parent().parent().parent()
				.find(".login_userpassword").val();
		var sever_ip = $(this).parent().parent().parent().parent()
				.find(".login_server_ip").val();
		var sever_port = $(this).parent().parent().parent().parent()
				.find(".login_server_port").val();

		// validate
		var is_va = 1;
		is_va = is_validate(login_username);
		if (0 == is_va) {
			return 1;
		}
		is_va = is_validate(login_password);
		if (0 == is_va) {
			return 1;
		}
		is_va = is_validate(sever_ip);
		if (0 == is_va) {
			return 1;
		}
		is_va = checkPort(sever_port);
		if (0 == is_va) {
			return 1;
		}

		var data = "";
		data = "login_username=" + login_username + "&login_userpassword="
				+ login_password + "&login_server_ip=" + sever_ip
				+ "&login_server_port=" + sever_port;

		// alert(data);

		$.ajax({
					url : "../cgi-bin/set_server_info.html",
					type : 'Post',
					contentType : "application/x-www-form-urlencoded; charset=utf-8",
					async : false,
					data : data,
					timeout : 6000,
					success : function(page_data) {
						if (page_data.length > 0) {
							alert(page_data);
						} else {
							alert("修改成功!");
						}
					},
					failed : function(page_data) {
						alert(page_data);
					}
				});

			// //如果提交成功
			// //清空文本框
			// $(this).parent().parent().parent().parent().find(".login_username").val("");
			// $(this).parent().parent().parent().parent().find(".login_userpassword").val("");
			// $(this).parent().parent().parent().parent().find(".login_server_ip").val("");
			// $(this).parent().parent().parent().parent().find(".login_server_port").val("");
	});
});

function dhcp_no(flag) {

	$(flag).parent().parent().parent().find("input[name='is_dhcp_value']")
			.attr("value", "0");
	$(flag).parent().parent().parent().find("input[type='text']")
			.removeAttr("disabled");

	// $(flag).parent().parent().parent().find("input[type='text']").removeAttr("disabled");
	// alert(0);
	return 1;
}

function dhcp_yes(flag) {
	$(flag).parent().parent().parent().parent()
			.find("input[name='is_dhcp_value']").attr("value", "1");
	$(flag).parent().parent().parent().find("input[type='text']").attr(
			"disabled", "disabled");
	$(flag).parent().parent().parent().find("input[name='ssid']")
			.removeAttr("disabled");
	$(flag).parent().parent().parent().find("input[name='login_passwd']")
			.removeAttr("disabled");

	return 1;
}
