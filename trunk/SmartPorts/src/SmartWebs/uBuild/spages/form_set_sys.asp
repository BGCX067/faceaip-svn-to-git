<html>
<!-- Copyright (C) 2011 ezlibs.com, All Rights Reserved. -->
<!-- form_set_sys.asp -->
<head>
<title>SmartPort</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
</head>
<body>

<h1>常规设置</h1>
<table>
<form action=/ezform/formSetSys method=POST">

<tr>
	<td>机器名:</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>
<tr>
	<td>编号:</td><td><input type=text name=sys_product_id size=50 value="<% aspGetSys("sys_product_id"); %>"/></td>
</tr>
<tr>
	<td>序列号:</td><td><% aspGetSys("sys_serial_no"); %></td>
</tr>
<tr>
	<td>版本号:</td><td><% aspGetSys("sys_version"); %></td>
</tr>

<tr>
    <td></td>
      <td ALIGN="CENTER"> <input type=submit name=ok value="Ok"> <input type=reset name=reset value="Reset"></td>
</tr>
</form>
</table>

<hr>	
<!-- ------------------------------------------------------------ -->
<h1>日期和时间</h1>
<table>
<form action=/ezform/formSetDateTime method=POST">
<tr>
	<td>自动确定日期和时间:</td><td><input type="checkbox" id=ch name=ntp_enable <% aspGetDateTime("ntp_enable"); %>"/>不勾选则使用输入的日期和时间</td>
</tr>
<tr>
	<td><h3>高级:</h3></td><td></td>
</tr>
<tr>
	<td>__时间服务器:</td><td><input type=text name=ntp_server size=40 value="<% aspGetDateTime("ntp_server"); %>"/></td>
</tr>
<tr>
	<td>__检查周期:</td><td><input type=text name=ntp_checkperiod size=40 value="<% aspGetDateTime("ntp_checkperiod"); %>"/>秒（>=5）</td>
</tr>

<!--
<tr>
	<td>自动确定时区:</td><td><input type="checkbox" id=ch name=autotimezone <% aspGetDateTime("autotimezone"); %>"/></td>
</tr>
-->

<tr>
	<td>设置日期时间:</td><td><input type=text name=sys_date_time size=40 value="<% aspGetDateTime("sys_date_time"); %>"/></td>
</tr>
<tr>
	<td>选择时区:</td><td><input type=text name=sys_timezone size=40 value="<% aspGetDateTime("sys_timezone"); %>"/></td>
</tr>
<!--
<tr>
	<td>选择日期格式:</td><td><input type=text name=sys_date_format size=50 value="<% aspGetDateTime("sys_date_format"); %>"/></td>
</tr>
-->
<tr>
    <td></td>
      <td ALIGN="CENTER"> <input type=submit name=ok size=20 value="Ok"> <input type=reset name=reset size=20 value="Reset"></td>
</tr>
</form>
</table>

<hr>
<h1>系统维护</h1>

<table>
<form action=/ezform/formSetSys method=POST>
<tr>
    <td><input type="hidden" name="sys_reboot" value="now" />重启系统：</td>
    <td ALIGN="LEFT"> <input type=submit name=ok value="重启"></td>
</tr>
</form>

<form action=/ezform/formSetSys method=POST>
<tr>
    <td><input type="hidden" name="sys_restore" value="factory" />恢复出厂状态：</td>
    <td ALIGN="LEFT"> <input type=submit name=ok value="恢复"></td>
</tr>
</form>

<form action=/ezform/formSetSys method=POST>
<tr>
    <td><input type="hidden" name="sys_net_console" value="factory" />远程维护功能：</td>
    <td ALIGN="LEFT"> <input type=submit name=ok value="运行中，点击关闭"></td>
</tr>
</form>

<form action=/ezform/formSetSys method=POST>
<tr>
    <td><input type="hidden" name="sys_watch_dog" value="factory" />看门狗：</td>
    <td ALIGN="LEFT"> <input type=submit name=ok value="运行中，点击关闭"></td>
</tr>
</form>

</table>

<hr>
<p><% aspGetSys("sys_copyright"); %></p>
</body>
</html>
