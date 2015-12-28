=== Project ===

== Description ==

== Frequently Asked Questions ==

	= install =
		在用户$(HOME)目录中解压本工程和libs
		
		makefile中使用的库位置采用$(HOME)变量，因此 libs 请解压到$(HOME)中， 如改到其他位置，请自行修改makefile
	= build =
		全功能版 Server
			Makefile
		全功能版 Server，去除对日志、配置、uuid 模块的依赖， 使用自带library
			Makefile.Standalone
		HTTP长连接客户端， 连接Webservice
			Makefile.HttpConn
		Tcp长连接客户端， 连接tcpServer
			Makefile.TcpConn

= What about? =

	Answer to foo bar dilemma.

= How about? =

	Answer to foo bar dilemma.

== Changelog ==

	= 1.0 =
	* A change since the previous version.
	* Another change.
	
	= 1.01 =
	* 2012-12-02 14:53:26
	* 增加start的参数传入并生效.
	* Another change.

== FileList ==
