<html>
<head>
<title>SmartPorts</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
</head>

<body>
<H1>运行状态</H1>
<p>SmartPorts is a fully-featured embedded device.</p>
<hr>
<h2>机器信息:</h2>
<UL>
<LI>机器名：<% aspGetSys("sys_name"); %></li>
<LI>编号：<% aspGetSys("sys_product_id"); %></li>
<LI>序列号：<% aspGetSys("sys_serial_no"); %></li>
<LI>版本号：<% aspGetSys("sys_version"); %></li>
</UL>
<p></p>

<hr>
<h2>运行信息:</h2>
<UL>
<LI>开启时间：<% aspGetSys("sys_start_date_time"); %></li>
<LI>运行时长：<% aspGetSys("sys_run_period"); %> (秒)</li>
</UL>
<p></p>
<hr>

<h2>联网信息:</h2>
<UL>
<LI>公网链接：<% aspGetSys("sys_internet"); %></li>
<LI>出口IP：<% aspGetSys("sys_nat_ip"); %></li>
</UL>
<p></p>

<hr>
<p><% aspGetSys("sys_copyright"); %></p>
</body>
</html>
