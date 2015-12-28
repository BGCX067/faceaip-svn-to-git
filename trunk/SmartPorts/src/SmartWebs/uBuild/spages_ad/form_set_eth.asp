<html>
<!-- Copyright (C) 2011 ezlibs.com, All Rights Reserved. -->
<!-- form_set_eth.asp -->
<head>
<title>SmartPort</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
</head>

<body>
<script type="text/javascript">
  function Check(ele, iface) {

    var inputs = document.getElementsByTagName("input");
    for (var i = 0; i < inputs.length; i++) {
      if (inputs[i].type == "text" && iface == 0 && inputs[i].name.indexOf("eth0") >= 0) {
        if (ele.checked) {
          inputs[i].disabled = true;
        } else {
          inputs[i].disabled = false;
        }
      }
      if (inputs[i].type == "text" && iface == 1 && inputs[i].name.indexOf("eth1") >= 0) {
        if (ele.checked) {
          inputs[i].disabled = true;
        } else {
          inputs[i].disabled = false;
        }
      }
    }
  }
  
//  function sb(){
//  		var v = document.getElementById("dd").value;
//  		alert("v="+v +", l="+v.length);
//  		
//  }
</script>
<form action=/ezform/formSetIf method=POST">

<h1>NetWork Setting</h1>

<table>
<!-- ------------------------------------------------------------ -->
<tr>
	<td colspan="2"><h2>General</h2></td>
</tr>
<tr>
	<td>Primary DNS:</td><td><input type=text name=PrimaryDNS size=50 value="<% aspGetIF("general", "PrimaryDNS"); %>"/></td>
</tr>
<tr>
	<td>Secondary DNS:</td><td><input type=text name=SecondaryDNS size=50 value="<% aspGetIF("general", "SecondaryDNS"); %>"/></td>
</tr>

<!-- ------------------------------------------------------------ -->
<hr>

<tr>
	<td colspan="2"><h2>Eth0:(<% aspGetIF("eth0", "mac"); %>)</h2></td>
</tr>
<tr>
	<td>Auto:</td><td><input type="checkbox" id=ch name=eth0_auto <% aspGetIF("eth0", "auto"); %> onclick="Check(this, 0)"/></td>
</tr>
<tr>
	<td>IP:</td><td><input type=text name=eth0_ipv4 id="dd" size=50 value="<% aspGetIF("eth0", "ip"); %>"/></td>
</tr>
<tr>
	<td>Mask:</td><td><input type=text name=eth0_mask size=50 value="<% aspGetIF("eth0", "mask"); %>"/></td>
</tr>
<tr>
	<td>GateWay:</td><td><input type=text name=eth0_gwv4 size=50 value="<% aspGetIF("eth0", "gateway"); %>"/></td>
</tr>
<tr>
	<td>Default:</td><td><input type="radio" <% aspGetIF("eth0", "default"); %> name="default_eth" value="eth0" /></td>
</tr>
<!-- ------------------------------------------------------------ -->
<tr>
	<td colspan="2"><h2>Eth1:(<% aspGetIF("eth1", "mac"); %>)</h2></td>
</tr>
<!--checked="checked", 如果没有，则不要写这个属性-->
<tr>
	<td>Auto:</td><td><input type="checkbox" name=eth1_auto <% aspGetIF("eth1", "auto"); %>  onclick="Check(this, 1)"/></td>
</tr>
<tr>
	<td>IP:</td><td><input type=text name=eth1_ipv4 size=50 value="<% aspGetIF("eth1", "ip"); %>"/></td>
</tr>
<tr>
	<td>Mask:</td><td><input type=text name=eth1_mask size=50 value="<% aspGetIF("eth1", "mask"); %>"/></td>
</tr>
<tr>
	<td>GateWay:</td><td><input type=text name=eth1_gwv4 size=50 value="<% aspGetIF("eth1", "gateway"); %>"/></td>
</tr>
<tr>
	<td>Default:</td><td><input type="radio" <% aspGetIF("eth1", "default"); %> name="default_eth" value="eth1" /></td>
</tr>

<!-- ------------------------------------------------------------ -->

<tr>
    <td></td>
      <td ALIGN="CENTER"> <input type=submit name=ok value="Ok"> <input type=reset name=reset value="Reset"></td>
</tr>
</table>

</form>

<hr>
<!-- ------------------------------------------------------------ -->	
	

<h1>高级</h1>

<table>
<form action=/ezform/formSetFunction method=POST">
<!-- ------------------------------------------------------------ -->
<tr>
	<td>GARP:</td><td><input type="checkbox" id=ch name=eth0_auto <% aspGetIF("eth0", "auto"); %>"/></td>
</tr>
<tr>
	<td>Network Time Protocol:</td><td><input type="checkbox" id=ch name=eth0_auto <% aspGetIF("eth0", "auto"); %>"/></td>
</tr>
<tr>
	<td>Host Search Protocol:</td><td><input type="checkbox" id=ch name=eth0_auto <% aspGetIF("eth0", "auto"); %>"/></td>
</tr>

<!-- ------------------------------------------------------------ -->

<tr>
    <td></td>
      <td ALIGN="CENTER"> <input type=submit name=ok value="Ok"> <input type=reset name=reset value="Reset"></td>
</tr>
</form>
</table>

<script type="text/javascript">
	var checks = document.getElementsByName("eth0_auto");
	Check(checks[0],0);

	checks = document.getElementsByName("eth1_auto");
	Check(checks[0],1);
</script>
<hr>
<p><% aspGetSys("sys_copyright"); %></p>
</body>
</html>
