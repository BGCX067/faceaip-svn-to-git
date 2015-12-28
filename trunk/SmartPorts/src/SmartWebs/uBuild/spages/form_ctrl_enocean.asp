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
<hr>
<p><% aspGetSys("sys_copyright"); %></p>

</body>
</html>
