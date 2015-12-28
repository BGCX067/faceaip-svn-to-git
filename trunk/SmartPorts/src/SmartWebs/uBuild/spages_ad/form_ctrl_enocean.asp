<html>
<!-- Copyright (C) 2011 ezlibs.com, All Rights Reserved. -->
<!-- form_set_eth.asp -->
<head>
<title>SmartPort</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
</head>

<body>
	
<form action=/ezform/formCtrlEnocean method=POST>

<h1>Switch Actuator over EnOcean  </h1>
<pre>
基于<a href="http://www.enocean.com/">EnOcean</a>技术，对系统内设备进行控制、温湿度、各类报警信号的采集。
如下演示控制EnOcean开关驱动器：
</pre>
<table>
<!-- ------------------------------------------------------------ -->
<tr>
	<td colspan="2"><h1>Dev0</h1></td>
</tr>
<tr>
	<td><input type=text name=devid_enocean0 size=50 value="0x10189e5"></td>
	<td><input type=submit name=ok value="<% aspGetEnoceanStatus("0x10189e5", "button_value"); %>" style="width:80px;<% aspGetEnoceanStatus("0x10189e5", "button_style"); %>"/></td>
</tr>
<!--
<tr>
	<td>Switch:</td><td><input type="checkbox" name="onoff_enocean0" value="1"/></td>
</tr>
-->

<!-- ------------------------------------------------------------ -->

<tr>
  <td></td>
  <td ALIGN="CENTER"> 
  	
  	<!-- <input type=submit name=ok value="Cancel"/> -->
  	</td>
</tr>
</table>
</form>
<pre>
更多控制功能请关注：
<a href="http://www.enocean.com/">EzLibs云控中心</a>
</pre>

<hr>
<p><% aspGetSys("sys_copyright"); %></p>

</body>
</html>
