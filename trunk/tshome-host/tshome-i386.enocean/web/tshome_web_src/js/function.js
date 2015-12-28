// ==========================页面加载时JS函数开始===============================
// 输入框显示提示效果，配合CSS运用
$(function() {
	$(".input,.login_input,.textarea").focus(function() {
				$(this).addClass("focus");
			}).blur(function() {
				$(this).removeClass("focus");
			});

	// 输入框提示,获取拥有HintTitle,HintInfo属性的对象
	$("[HintTitle],[HintInfo]").focus(function(event) {
		$("*").stop(); // 停止所有正在运行的动画
		$("#HintMsg").remove(); // 先清除，防止重复出错
		var HintHtml = "<ul id=\"HintMsg\"><li class=\"HintTop\"></li><li class=\"HintInfo\"><b>"
				+ $(this).attr("HintTitle")
				+ "</b>"
				+ $(this).attr("HintInfo")
				+ "</li><li class=\"HintFooter\"></li></ul>"; // 设置显示的内容
		var offset = $(this).offset(); // 取得事件对象的位置
		$("body").append(HintHtml); // 添加节点
		$("#HintMsg").fadeTo(0, 0.85); // 对象的透明度
		var HintHeight = $("#HintMsg").height(); // 取得容器高度
		$("#HintMsg").css({
					"top" : offset.top - HintHeight + "px",
					"left" : offset.left + "px"
				}).fadeIn(500);
	}).blur(function(event) {
				$("#HintMsg").remove(); // 删除UL
			});
});

// 主框架切换及显示首次快捷菜单
$(function() {
			// 关闭打开左栏目
			$("#sysBar").toggle(function() {
						$("#mainLeft").hide();
						$("#barImg").attr("src", "images/butOpen.gif");
					}, function() {
						$("#mainLeft").show();
						$("#barImg").attr("src", "images/butClose.gif");
					});
			// 页面加载完毕，显示第一个子菜单
			$(".left_menu").hide();
			$(".left_menu:eq(0)").show();
		});
// ==========================页面加载时JS函数结束===============================

// ===========================系统管理JS函数开始================================

// 后台主菜单控制函数
function tabs(tabNum) {
	// 设置点击后的切换样式
	$("#tabs ul li").removeClass("hover");
	$("#tabs ul li").eq(tabNum - 1).addClass("hover");
	// 根据参数决定显示子菜单
	$(".left_menu").hide();
	$(".left_menu").eq(tabNum).show();
}

// 全选取消按钮函数，调用样式如：
function checkAll(chkobj) {
	if ($(chkobj).text() == "全选") {
		$(chkobj).text("取消");
		$(".checkall input").attr("checked", true);
	} else {
		$(chkobj).text("全选");
		$(".checkall input").attr("checked", false);
	}
}

// 遮罩提示窗口
function jsmsg(w, h, msgtitle, msgbox, url, msgcss) {
	$("#msgdialog").remove();
	var cssname = "";
	switch (msgcss) {
		case "Success" :
			cssname = "icon-01";
			break;
		case "Error" :
			cssname = "icon-02";
			break;
		default :
			cssname = "icon-03";
			break;
	}
	var str = "<div id='msgdialog' title='" + msgtitle + "'><p class='"
			+ cssname + "'>" + msgbox + "</p></div>";
	$("body").append(str);
	$("#msgdialog").dialog({
				// title: null,
				// show: null,
				bgiframe : true,
				autoOpen : false,
				width : w,
				// height: h,
				resizable : false,
				closeOnEscape : true,
				buttons : {
					"确定" : function() {
						$(this).dialog("close");
					}
				},
				modal : true
			});
	$("#msgdialog").dialog("open");
	if (url == "back") {
		sysMain.history.back(-1);
	} else if (url != "") {
		sysMain.location.href = url;
	}
}

// 可以自动关闭的提示
function jsprint(msgtitle, url, msgcss) {
	$("#msgprint").remove();
	var cssname = "";
	switch (msgcss) {
		case "Success" :
			cssname = "pcent correct";
			break;
		case "Error" :
			cssname = "pcent disable";
			break;
		default :
			cssname = "pcent warning";
			break;
	}
	var str = "<div id=\"msgprint\" class=\"" + cssname + "\">" + msgtitle
			+ "</div>";
	$("body").append(str);
	$("#msgprint").show();
	if (url == "back") {
		sysMain.history.back(-1);
	} else if (url != "") {
		sysMain.location.href = url;
	}
	// 3秒后清除提示
	setTimeout(function() {
				$("#msgprint").fadeOut(500);
				// 如果动画结束则删除节点
				if (!$("#msgprint").is(":animated")) {
					$("#msgprint").remove();
				}
			}, 3000);
}
// ===========================系统管理JS函数结束================================

// ================上传文件JS函数开始，需和jquery.form.js一起使用===============
// 单个文件上传
function SingleUpload(repath, uppath) {

	var submitUrl = "../cgi-bin/imp_xml.html";

	$('body')
			.append('<div class="popupbg"></div><div class="popupbox"><div class="popupbox_bg"><div class="popupbox_btn_ok"><div class="popupbox_btn_cancel"><div class="popupboxbackground"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr height="28"><td></td></tr><tr height="75"><td>导入后需要重启主机程序，是否确定?</td></tr><tr height="40"><td><input type="button" class="popup_renew_ok" value="确定"/><input type="button" class="popup_renew_cancel" value="取消"/></td></tr></table></div></div></div></div></div>');
	$("input.popup_renew_ok").click(function() {// 确认
				// SingleUpload(txtImgUrl, FileUpload);

				// 删除对话框
				$(".popupbg").remove();
				$(".popupbox").remove();

				$(".lock").show();
				$(".ts_running").show();
				// 开始提交
				$("#form1").ajaxSubmit({
							url : submitUrl,
							type : "post",
							dataType : "text",
							timeout : 60000,
							success : function(page_data, textStatus) {
								// if (data.msg == 1) {
								// $("#" + repath).val(data.msbox);
								// } else {
								// alert(data.msbox);
								// }
								// $("#" +
								// repath).nextAll(".files").eq(0).show();
								// $("#" + repath).nextAll(".uploading").eq(0)
								// .hide();
								if ((page_data.length > 0)
										&& (page_data.length < 15)) {
									alert(page_data);
									page_data = page_data + "";
									if (page_data.indexOf("成功") != -1) {
										$(".next_btn").show();
									}
								} else {
//									alert("p : " + page_data);
									alert("导入有误，请检查配置文件编码格式是否正确");
								}
							},
							error : function(page_data, status, e) {
								alert("导入有误，请检查配置文件");
								// $("#" +
								// repath).nextAll(".files").eq(0).show();
								// $("#" + repath).nextAll(".uploading").eq(0)
								// .hide();
							}
						});

				tt = window.setTimeout("unshow()", 1);
				// location.replace(location);
			});
	$("input.popup_renew_cancel").click(function() {// 取消
				// 删除对话框
				$(".popupbg").remove();
				$(".popupbox").remove();
			});

	return 1;

};
// ===========================上传文件JS函数结束================================
