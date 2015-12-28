2012-12-19 14:36:42
-------------------------------------------------------
include           : third party head file
lib               : third party lib file
Makefile          : project makefile , 
readme.txt        : me
tutorial_libOdip  : tutorial


Usage(exa in centos):
-----------------------
1. Set dynamic lib path
	[wujj@joy-dev lib]$ export LD_LIBRARY_PATH=/home/wujj/Dropbox/stock/trunk/code/library/libOdip/release_libOdip/lib
2. Make
	[wujj@joy-dev tutorial_libOdip]$ make
3. Run
	[wujj@joy-dev tutorial_libOdip]$ ./getStream 
	Connector[NVRKeepConnector]:0 faceaip.3322.org:9056 Open Succeeded.
	CThreadManager::CThreadManager()>>>>>>>>>
	getStream.cpp(62)--Login Succeeded
	getStream.cpp(26)-- g_RealDataCallBack_V30 Get data at :1354548220, the size is:4096.

Usage(in android):
-----------------------
	static only
	to Makefile for detail

Support:
-----------------------
MP:13512543175 | QQ:11409235
Skype:joy.woo@hotmail.com
joy.woo@qq.com 
