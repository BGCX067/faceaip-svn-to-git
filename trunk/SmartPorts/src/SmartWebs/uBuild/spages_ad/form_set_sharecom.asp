<html>
<!-- Copyright (C) 2011 ezlibs.com, All Rights Reserved. -->
<!-- form_set_sys.asp -->
<head>
<title>SmartPort</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
</head>
<body>

<h1>透明串口设置</h1>
<hr>
<h1>todo 2013-07-13 18:40:32</h1>
<!-- ------------------------------------------------------------ -->
<hr>

<table>
<form action=/ezform/formSetShareCom method=POST">
	<tr>
	<td><h2>串口1:</h2></td><td> </td>
</tr>

<tr>
	<td>串口设备:</td><td><% aspGetSys("sys_name"); %></td>
</tr>
<tr>
	<td>波特率</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>
<tr>
	<td>数据位:</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>
<tr>
	<td>校验位:</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>
<tr>
	<td>停止位:</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>
<tr>
	<td>模式:</td><td>TCPServer</td>
</tr>
<tr>
	<td>端口:</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>
<!-- ------------------------------------------------------------ -->
<tr>
	<td><h2>串口2:</h2></td><td> </td>
</tr>
<tr>
	<td>串口设备:</td><td><% aspGetSys("sys_name"); %></td>
</tr>
<tr>
	<td>波特率</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>
<tr>
	<td>数据位:</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>
<tr>
	<td>校验位:</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>
<tr>
	<td>停止位:</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>
<tr>
	<td>模式:</td><td>TCPServer</td>
</tr>
<tr>
	<td>端口:</td><td><input type=text name=sys_name size=50 value="<% aspGetSys("sys_name"); %>"/></td>
</tr>

<!-- ------------------------------------------------------------ -->

<tr>
    <td></td><td ALIGN="CENTER"> <input type=submit name=ok value="Ok"> <input type=reset name=reset value="Reset"></td>
</tr>
</form>
</table>

<hr>
<p><% aspGetSys("sys_copyright"); %></p>
</body>
</html>
