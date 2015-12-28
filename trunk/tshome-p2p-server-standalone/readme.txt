tshome-p2p-server-standalone
	bin  执行文件
	build  Makefile
	docs  
	include  需用的头文件， 从tshome-p2p-server 编译后， 用脚本install-ts-p2p-lib.sh 生成
	lib  需用的库文件， 从tshome-p2p-server 编译后， 用脚本install-ts-p2p-lib.sh 生成
	scripts  脚本， 数据库文件等
	src 源代码， based \pjnath\src\pjturn-srv

based
	1.12  libpj
	      libpjlib-util
	
update

2012-8-13 13:12:20
add g_log
2012-9-11 13:28:22
add
2012-9-19 14:21:09
将端口冲突等出错信息打印出来
fd -1 :: bind for port:19023: 98 Address already in use (3)
terminate called after throwing an instance of 'Exception'
Aborted

2012-11-6 15:56:04
移植到64位系统中
