window.onload = prepareForm;

function print_left_guide(selected) {
	var i = 0;
	var counts = 7;
	var texts = new Array([counts]);
	texts[0] = "当前系统信息";
	texts[1] = "设置向导";
	texts[2] = "基本控制";
	texts[3] = "串口参数";
	texts[4] = "网关通路";
	texts[5] = "串口设备通路";
	texts[6] = "红外信息";
	var links = new Array([counts]);
	links[0] = "curr_state.html";
	links[1] = "base_info.html";
	links[2] = "basic_control.html";
	links[3] = "home_serial_port.html";
	links[4] = "pathway_info.html";
	links[5] = "device_link_access.html";
	links[6] = "infrared_device_pathway.html";
	document.writeln("<div class=\"navigation\">");
	document.writeln("<ul>");

	for (i = 0; i < counts; i++) {
		document.write("<li ");
		if (i == selected) {
			document.write("class=\"selected\"");
		}
		document.writeln("><a href =\"" + links[i] + "\">" + texts[i]
				+ "</a></li>");
		// document
		// .writeln("<li class=\"selected\"><a
		// href=\"base_info.html\">设置向导</a></li>");
	}

	document.writeln("</ul>");
	document.writeln("</div>");
}

function can_use() {
	alert("主机程序未启动，该功能无法实现");
	return 1;
}

function jumpPage(obj_page) {
	$(".next_btn").show();
	window.location.href = "../cgi-bin/" + obj_page;
	return 1;
}

var page_counts = 9;
var guide_pages = new Array([page_counts]);
var GUIDE_NAME = "?guide_type=1"
guide_pages[0] = "base_info.html";
guide_pages[1] = "base_info.html" + GUIDE_NAME;
guide_pages[2] = "base_info.html" + "?guide_type=2";
guide_pages[3] = "base_info.html" + "?guide_type=3";
guide_pages[4] = "guide_info.html";
guide_pages[5] = "home_serial_port.html" + GUIDE_NAME;
guide_pages[6] = "pathway_info.html" + GUIDE_NAME;
guide_pages[7] = "device_link_access.html" + GUIDE_NAME;
guide_pages[8] = "infrared_device_pathway.html" + GUIDE_NAME;
function back_page() {
	var curr_page = "";
	curr_page = window.location.toString();
	var i = 1;
	for (i = 1; i < page_counts; i++) {
		if (curr_page.indexOf(guide_pages[i]) != -1) {
			window.location.href = guide_pages[i - 1];
			break;
		}
	}
	return 1;
}

function guide_control(type, obj_page) {
	// server info
	if (0 == type) {
		// 取值
		var login_username = $("input[name='login_username']").val();
		var login_password = $("input[name='login_userpassword']").val();
		var sever_ip = $("input[name='login_server_ip']").val();
		var sever_port = $("input[name='login_server_port']").val();

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
		// return 1;

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
							if (-1 != page_data.indexOf("成功")) {
								window.location.href = "../cgi-bin/" + obj_page;
							}
						} else {
							window.location.href = "../cgi-bin/" + obj_page;
						}
					},
					failed : function(page_data) {
						alert(page_data);
					}
				});
	} else if ((1 == type) || (2 == type) || (3 == type)) {
		var data = "";
		var ret = 1;
		data += "is_dhcp_value=" + $("input[name='is_dhcp_value']").val() + "&";

		var ip = "";
		var i = 0;
		for (i = 0; i < 4; i++) {
			attr_name = "input[name='ip_" + i + "']";
			ip += $(attr_name).val();
			if (i != 3) {
				ip += "."
			}
		}

		data += "ip=" + ip + "&";
		ret = checkIPByName("IP地址", ip);
		if (1 != ret) {
			return 1;
		}

		var gw = "";
		i = 0;
		for (i = 0; i < 4; i++) {
			attr_name = "input[name='gw_" + i + "']";
			gw += $(attr_name).val();
			if (i != 3) {
				gw += "."
			}
		}
		data += "gw=" + gw + "&";
		ret = checkIPByName("网关", gw);
		if (1 != ret) {
			return 1;
		}

		var net_mask = "";
		i = 0;
		for (i = 0; i < 4; i++) {
			attr_name = "input[name='net_mask_" + i + "']";
			net_mask += $(attr_name).val();
			if (i != 3) {
				net_mask += "."
			}
		}
		data += "net_mask=" + net_mask + "&";
		ret = checkIPByName("子网掩码", net_mask);
		if (1 != ret) {
			return 1;
		}

		if ((null != $("input[name='dns_0']").val())
				&& ($("input[name='dns_0']").val().length > 0)) {
			var dns = "";
			i = 0;
			for (i = 0; i < 4; i++) {
				attr_name = "input[name='dns_" + i + "']";
				dns += $(attr_name).val();
				if (i != 3) {
					dns += "."
				}
			}
			data += "dns=" + dns + "&";
			ret = checkIPByName("DNS:", dns);
			if (1 != ret) {
				return 1;
			}
		}

		switch (type) {
			case 1 :// eth0 conf
				data += "net_name=eth0&";
				break;
			case 2 :// eth1 conf
				data += "net_name=eth1&";
				break;
			case 3 :// wlan0 conf
				data += "net_name=wlan0&";
				data += "ssid=" + $("input[name='ssid']").val() + "&";
				data += "login_passwd=" + $("input[name='login_passwd']").val();
				+"&";
				break;
			default :
				break;
		}

		set_ip_port(data);
		var doJump = "window.location.href = " + "'../cgi-bin/" + obj_page
				+ "'";
		setTimeout(doJump, 3000);
		// setTimeout("window.location.href = '../cgi-bin/guide_info.html'",
		// 3000);
		// window.location.href = "../cgi-bin/" + obj_page;

		// $.ajax({
		// url : "../cgi-bin/set_ip_port.html",
		// type : 'Post',
		// contentType : "application/x-www-form-urlencoded; charset=utf-8",
		// async : false,
		// data : data,
		// timeout : 6000,
		// success : function(page_data) {
		// alert(1);
		// alert(page_data);
		// if (page_data.length > 0) {
		// if (-1 != page_data.indexOf("成功")) {
		// window.location.href = "../cgi-bin/" + obj_page;
		// }
		// } else {
		// window.location.href = "../cgi-bin/" + obj_page;
		// }
		// },
		// failed : function(page_data) {
		// alert(page_data);
		// }
		// });
		// alert(data);
	}

	return 1;
}

function isUsed(all, item) {
	var strs = new Array(); // 定义一数组
	strs = all.split(","); // 字符分割
	for (i = 0; i < strs.length; i++) {
		if (item == strs[i]) {
			return 0;
		}
	}
	return 1;
}

var xml_info = "";

var large_num = 1000000;

function BASEisNotInt(name, obj) {
	// alert(name);
	var exp = "^\\d+$";
	var reg = obj.match(exp);
	if (reg == null) {
		alert(name + "必须为非负整数");
		return 0;
	} else {
		if (reg >= large_num) {
			alert(name + "数据过大");
			return 0;
		}
		return 1;
	}
}

function BASEisNotFloat(name, obj) {
	var exp = "^\\d+(\\.\\d+)?$";
	var reg = obj.match(exp);
	if (reg == null) {
		alert(name + "必须为非负整数或者小数");
		return 0;
	} else {
		if (obj >= large_num) {
			alert(name + "数据过大");
			return 0;
		}
		return 1;
	}
}

// serial name can't the same
var serial_names = "";
function validate_serial_name(serial_name) {

	var i = 0;
	for (i = 0; i < serial_name.length; i++) {
		if ((serial_name.charCodeAt(i) > 127)||(serial_name.charCodeAt(i) < 0)) {
			alert("非法字符 : " + serial_name.charAt(i));
			return 0;
		}
	}

	if (isUsed(serial_names, serial_name) == 0) {
		alert("串口名不能重复");
		return 0
	} else {
		serial_names += serial_name + ",";
		return 1;
	}
	return 1;
}

// data validate
function is_validate(data) {
	data += "";
	if (data.length > 31) {
		alert("输入有误，字符串过长.");
		return 0;
	}
	if (null == data) {
		alert("输入有误，字符串过短.");
		return 0;
	}
	if (data.length <= 0) {
		alert("输入有误，字符串过短.");
		return 0;
	}
	var i = -1;
	i = data.indexOf("&");
	if (i != -1) {
		alert("输入有误，不能出现以下符号：& | ; ? : = ,");
		return 0;
	}
	i = data.indexOf("|");
	if (i != -1) {
		alert("输入有误，不能出现以下符号：& | ; ? : = ,");
		return 0;
	}
	i = data.indexOf("?");
	if (i != -1) {
		alert("输入有误，不能出现以下符号：& | ; ? : = ,");
		return 0;
	}
	i = data.indexOf(":");
	if (i != -1) {
		alert("输入有误，不能出现以下符号：& | ; ? : = ,");
		return 0;
	}
	i = data.indexOf(";");
	if (i != -1) {
		alert("输入有误，不能出现以下符号：& | ; ? : = ,");
		return 0;
	}
	i = data.indexOf("=");
	if (i != -1) {
		alert("输入有误，不能出现以下符号：& | ; ? : = ,");
		return 0;
	}
	i = data.indexOf(",");
	if (i != -1) {
		alert("输入有误，不能出现以下符号：& | ; ? : = ,");
		return 0;
	}
	return 1;
}

function checkIP(obj) {
	return checkIPByName("IP地址", obj)
}

function checkIPByName(name, obj) {
	var exp = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
	// alert(obj);
	var reg = obj.match(exp);
	if (reg == null) {
		alert(name + "不合法或为空:" + obj);
		return 0;
	} else {
		// alert("IP地址合法！");
		return 1;
	}
}

function checkPort(obj) {
	if (0 == is_validate(obj)) {
		return 0;
	}
	var exp = /^[1-9]|([1-9][0-9])|([1-9][0-9][0-9])|([1-9][0-9][0-9][0-9])|([1-6][0-5][0-5][0-3][0-5])$/
	var reg = obj.match(exp);
	if ((reg == null) || (obj < 0) || (obj > 65536)) {
		alert("端口号不合法！");
		return 0;
	} else {
		return 1;
	}
}
// $(this).parent().parent().parent().parent().find
function set_local_net(net_name) {
	$('body')
			.append('<div class="popupbg"></div><div class="popupbox"><div class="popupbox_bg"><div class="popupbox_btn_ok"><div class="popupbox_btn_cancel"><div class="popupboxbackground"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr height="28"><td></td></tr><tr height="75"><td>web服务器需要重启；</br>修改ip地址后，浏览网页的地址将发生变化；且若主机程序已启动，则需要重启该程序</td></tr><tr height="40"><td><input type="button" class="popup_receive_ok" value="确定"/><input type="button" class="popup_receive_cancel" value="取消"/></td></tr></table></div></div></div></div></div>');
	$("input.popup_receive_ok").click(function() {// 确认

				// 删除对话框
				$(".popupbg").remove();
				$(".popupbox").remove();
				var data = "";
				var is_va = 1;

				var form_name = "";
				var form_set_net = document.getElementById("set_ip_prot_"
						+ net_name);
				// alert(form_set_net.elements.length);
				for (var i = 0; i < form_set_net.elements.length; i++) {
					if (form_set_net.elements[i].name == "ip") {
						is_va = checkIPByName("IP地址",
								form_set_net.elements[i].value);
						if (is_va == 0) {
							break;
						}
					} else if (form_set_net.elements[i].name == "gw") {
						is_va = checkIPByName("网关地址",
								form_set_net.elements[i].value);
						if (is_va == 0) {
							break;
						}
					} else if (form_set_net.elements[i].name == "net_mask") {
						is_va = checkIPByName("子网掩码",
								form_set_net.elements[i].value);
						if (is_va == 0) {
							break;
						}
					} else if (form_set_net.elements[i].name == "net_mask") {
						is_va = checkIPByName("子网掩码",
								form_set_net.elements[i].value);
						if (is_va == 0) {
							break;
						}
					}
					if (form_set_net.elements[i].name == "is_dhcp") {
						var New = document.getElementsByName("is_dhcp");
						var strNew = "";
						for (var j = 0; j < New.length; j++) {
							if (New.item(j).checked) {
								strNew = New.item(j).getAttribute("value");
								data = data + "is_dhcp_value=" + strNew + "&";
								break;
							} else {
								continue;
							}
						}
					} else {
						data += form_set_net.elements[i].name;
						data += "=";
						data += escape(form_set_net.elements[i].value);
						data += "&";
					}
				}

				if (is_va == 1) {
					// alert(data);
					set_ip_port(data);
					setTimeout("location.replace(location);", 3000);

				}

			});
	$("input.popup_receive_cancel").click(function() {// 取消
				// 删除对话框
				$(".popupbg").remove();
				$(".popupbox").remove();
				location.replace(location);
			});
}

function set_user_info(net_name) {
	var data = "";
	var is_va = 1;

	var form_name = "";
	var value = "";
	var form_set_net = document.getElementById("set_user_info");
	for (var i = 0; i < form_set_net.elements.length; i++) {
		if ("old_passwd" == form_set_net.elements[i].name) {
			value = form_set_net.elements[i].value + "";
			is_va = is_validate(value);
			if (is_va == 0) {
				break;
			}
		} else if ("new_passwd" == form_set_net.elements[i].name) {
			value = form_set_net.elements[i].value + "";
			is_va = is_validate(value);
			if (is_va == 0) {
				break;
			}
		} else if ("reply_passwd" == form_set_net.elements[i].name) {
			value = form_set_net.elements[i].value + "";
			is_va = is_validate(value);
			if (is_va == 0) {
				break;
			}
		}
		data += form_set_net.elements[i].name;
		data += "=";
		data += escape(value);
		data += "&";
	}

	if (is_va == 1) {

		$.ajax({
					url : "../cgi-bin/set_user_info.html",
					type : 'Post',
					contentType : "application/x-www-form-urlencoded; charset=utf-8",
					async : false,
					data : data,
					timeout : 6000,
					success : function(page_data) {
						if (page_data.length > 0) {
							alert(page_data);
						} else {
							// window.location.href = "../cgi-bin/login.html";
							// location.replace(location);
						}
					},
					failed : function(page_data) {
						alert(page_data);
					}
				});

		// location.replace(location);
	}

}

function prepareForm() {
	if (!document.getElementById) {
		return;
	}
}

// 开关主机,需要加入确认框
function on_off_host(data, reload, on_off) {

	if (1 == on_off) {// 开
		$('body')
				.append('<div class="popupbg"></div><div class="popupbox"><div class="popupbox_bg"><div class="popupbox_btn_ok"><div class="popupbox_btn_cancel"><div class="popupboxbackground"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr height="28"><td></td></tr><tr height="75"><td>主机程序即将打开，是否确定？</td></tr><tr height="40"><td><input type="button" class="popup_renew_ok" value="确定"/><input type="button" class="popup_renew_cancel" value="取消"/></td></tr></table></div></div></div></div></div>');
	} else if (0 == on_off) {// 关
		$('body')
				.append('<div class="popupbg"></div><div class="popupbox"><div class="popupbox_bg"><div class="popupbox_btn_ok"><div class="popupbox_btn_cancel"><div class="popupboxbackground"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr height="28"><td></td></tr><tr height="75"><td>主机程序即将关闭，是否确定？</td></tr><tr height="40"><td><input type="button" class="popup_renew_ok" value="确定"/><input type="button" class="popup_renew_cancel" value="取消"/></td></tr></table></div></div></div></div></div>');
	} else {// 重启
		$('body')
				.append('<div class="popupbg"></div><div class="popupbox"><div class="popupbox_bg"><div class="popupbox_btn_ok"><div class="popupbox_btn_cancel"><div class="popupboxbackground"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr height="28"><td></td></tr><tr height="75"><td>主机程序即将重启，是否确定？</td></tr><tr height="40"><td><input type="button" class="popup_renew_ok" value="确定"/><input type="button" class="popup_renew_cancel" value="取消"/></td></tr></table></div></div></div></div></div>');
	}

	$("input.popup_renew_ok").click(function() {// 确认
				// 删除对话框
				$(".popupbg").remove();
				$(".popupbox").remove();
				control_host(data, reload);
			});
	$("input.popup_renew_cancel").click(function() {// 取消
				// 删除对话框
				$(".popupbg").remove();
				$(".popupbox").remove();
				location.replace(location);
				// control_host(data, reload);
			});
}

function control_host(data, reload) {
	control_host_time(data, reload, 3000);
}

function control_host_time(data, reload, time) {
	var request = getHTTPObject();

	if (request) {
		request.onreadystatechange = function() {
			parseResponse(request);
		};

		// lock the page
		$(".lock").show();
		$(".ts_running").show();

		// alert(1);

		$.ajax({
					url : "../cgi-bin/control_host.html",
					type : 'Post',
					contentType : "application/x-www-form-urlencoded; charset=utf-8",
					async : false,
					data : data,
					timeout : 30000000,
					success : function(page_data) {
						tt = window.setTimeout("unshow()", time);
						// // var can_unshow = 1;
						// if (page_data.length > 0) {
						// if (-1 == data.indexOf("info=15&")) {
						// alert(page_data);
						// } else {
						// // get last len
						// if (-1 != page_data
						// .indexOf("&TS_GUIDE_UPDATE_RESULT="
						// + "更新成功")) {
						// alert("更新成功");
						// $(".next_btn").show();
						// } else if (-1 != page_data
						// .indexOf("&TS_GUIDE_UPDATE_RESULT="
						// + "更新失败")) {
						// alert("更新失败");
						// $(".next_btn").hide();
						// } else if (-1 != page_data
						// .indexOf("&TS_GUIDE_UPDATE_RESULT="
						// + "必须开启外网服务")) {
						// alert("必须开启外网服务");
						// } else {
						// alert(page_data);
						// }
						// }
						// } else {
						// // download log
						// if ("info=13&" == data) {
						// window.location.href = "../ts_logs.tar.gz";
						// }
						// if (reload == 1) {
						// location.replace(location);
						// }
						// }
						// // unshow();
					},
					error : function(page_data, status, e) {
						unshow();
					}
				});

		// tt = window.setTimeout("unshow()", time);

		if (reload == 1) {
			location.replace(location);
		}
		return true;
	} else {
		return false;
	}
}

function time_minus(time) {
	// alert(time);
	document.getElementById('ts_wait').value = "请等待：" + time + "秒";
	if (time <= 0) {
		// unshow();
	}
}

function unshow() {
	$(".lock").hide();
	$(".ts_wait").hide();
	$(".ts_running").hide();
	window.clearInterval(tt);
}

function stop_host(data, value) {
	var text = "";
	if (1 == value) {
		text = "外网服务将被开启，是否确定?";
	} else {
		text = "外网服务将被关闭，是否确定?";
	}
	var show_text = '<div class="popupbg"></div><div class="popupbox"><div class="popupbox_bg"><div class="popupbox_btn_ok"><div class="popupbox_btn_cancel"><div class="popupboxbackground"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr height="28"><td></td></tr><tr height="75"><td>'
			+ text
			+ '</td></tr><tr height="40"><td><input type="button" class="popup_renew_ok" value="确定"/><input type="button" class="popup_renew_cancel" value="取消"/></td></tr></table></div></div></div></div></div>'

	$('body').append(show_text);
	$("input.popup_renew_ok").click(function() {// 确认
				// 删除对话框
				$(".popupbg").remove();
				$(".popupbox").remove();
				control_host(data, 1);
			});
	$("input.popup_renew_cancel").click(function() {// 取消
				// 删除对话框
				$(".popupbg").remove();
				$(".popupbox").remove();
			});
}

function set_host_local(data, value) {
	var text = "";
	if (1 == value) {
		text = "将切换到内网模式，需要重启程序，是否确定?";
	} else {
		text = "将切换到外网模式，需要重启程序，是否确定?";
	}
	var show_text = '<div class="popupbg"></div><div class="popupbox"><div class="popupbox_bg"><div class="popupbox_btn_ok"><div class="popupbox_btn_cancel"><div class="popupboxbackground"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr height="28"><td></td></tr><tr height="75"><td>'
			+ text
			+ '</td></tr><tr height="40"><td><input type="button" class="popup_renew_ok" value="确定"/><input type="button" class="popup_renew_cancel" value="取消"/></td></tr></table></div></div></div></div></div>'

	$('body').append(show_text);
	$("input.popup_renew_ok").click(function() {// 确认
				// 删除对话框
				$(".popupbg").remove();
				$(".popupbox").remove();
				control_host(data, 1);
			});
	$("input.popup_renew_cancel").click(function() {// 取消
				// 删除对话框
				$(".popupbg").remove();
				$(".popupbox").remove();
			});
}

function set_ip_port(data) {
	var request = getHTTPObject();
	if (request) {
		request.onreadystatechange = function() {
			parseResponse(request);
		};
		// alert(data);
		request.open("POST", "../cgi-bin/set_ip_port.html", true);
		request.setRequestHeader("Content-Type",
				"application/x-www-form-urlencoded");
		request.send(data);
		return true;
	} else {
		return false;
	}
}

function parseResponse(request) {
	if (request.readyState == 4) {
		if (request.status == 200 || request.status == 304) {
			var container = document.getElementById("change_set_ip");
			container.innerHTML = request.responseText;
			prepareForm();
		}
	}
}

function getHTTPObject() {
	var xhr = false;
	if (window.XMLHttpRequest) {
		xhr = new XMLHttpRequest();
	} else if (window.ActiveXObject) {
		try {
			xhr = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (e) {
			try {
				xhr = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (e) {
				xhr = false;
			}
		}
	}
	return xhr;
}

// 读取xml信息
function readFile(fileBrowser) {
	if (navigator.userAgent.indexOf("MSIE") != -1) {
		readFileIE(fileBrowser);
	} else if (navigator.userAgent.indexOf("Firefox") != -1
			|| navigator.userAgent.indexOf("Mozilla") != -1) {
		readFileFirefox(fileBrowser);
	} else {
		alert("Not IE or Firefox (userAgent=" + navigator.userAgent + ")");
	}
}

function readFileFirefox(fileBrowser) {
	try {
		netscape.security.PrivilegeManager
				.enablePrivilege("UniversalXPConnect");
	} catch (e) {
		// alert('Unable to access local files due to browser security settings.
		// To overcome this, follow these steps: (1) Enter "about:config" in the
		// URL field; (2) Right click and select New->Boolean; (3) Enter
		// "signed.applets.codebase_principal_support" (without the quotes) as a
		// new preference name; (4) Click OK and try loading the file again.');
		alert('您无法远程打开本地文件，处理方法\n：1.浏览器地址栏中输入：about:config；\n 2 。将signed.applets.codebase_principal_support属性的值改为true');
		return;
	}

	var fileName = fileBrowser.value;
	var file = Components.classes["@mozilla.org/file/local;1"]
			.createInstance(Components.interfaces.nsILocalFile);
	try {
		// Back slashes for windows
		file.initWithPath(fileName.replace(/\//g, "\\\\"));
	} catch (e) {
		if (e.result != Components.results.NS_ERROR_FILE_UNRECOGNIZED_PATH)
			throw e;
		alert("File '"
				+ fileName
				+ "' cannot be loaded: relative paths are not allowed. Please provide an absolute path to this file.");
		return;
	}

	if (file.exists() == false) {
		alert("File '" + fileName + "' not found.");
		return;
	}
	// alert(file.path); // I test to get the local file's path.
	var is = Components.classes["@mozilla.org/network/file-input-stream;1"]
			.createInstance(Components.interfaces.nsIFileInputStream);
	try {
		is.init(file, 0x01, 00004, null);
	} catch (e) {
		if (e.result != Components.results.NS_ERROR_FILE_ACCESS_DENIED)
			throw e;
		alert("Unable to access local file '"
				+ fileName
				+ "' because of file permissions. Make sure the file and/or parent directories are readable.");
		return;
	}
	var sis = Components.classes["@mozilla.org/scriptableinputstream;1"]
			.createInstance(Components.interfaces.nsIScriptableInputStream);
	sis.init(is);
	var data = sis.read(sis.available());
	xml_info = data;
	// alert("Data from file: " + data); // I test to get the local file's data.
}

function readFileIE(fileBrowser) {
	var data;
	try {
		var fso = new ActiveXObject("Scripting.FileSystemObject");

		var fileName = fso.GetAbsolutePathName(fileBrowser.value);
		if (!fso.FileExists(fileName)) {
			alert("File '" + fileName + "' not found.");
			return;
		}

		var file = fso.OpenTextFile(fileName, 1);

		data = file.ReadAll();
		xml_info = data;
		// alert("Data from file: " + data);
		file.Close();
	} catch (e) {
		if (e.number == -2146827859) {
			// This is what we get if the browser's security settings forbid
			// the use of the FileSystemObject ActiveX control
			alert('Unable to access local files due to browser security settings. To overcome this, go to Tools->Internet Options->Security->Custom Level. Find the setting for "Initialize and script ActiveX controls not marked as safe" and change it to "Enable" or "Prompt"');
		} else if (e.number == -2146828218) {
			// This is what we get if the browser can't access the file
			// because of file permissions
			// alert("Unable to access local file '" + fileName + "' because of
			// file permissions. Make sure the file and/or parent directories
			// are readable.");
			alert(fileName);
		} else
			throw e;
	}
}
