
2012-7-20 14:24:11

编译方法：
====================
[wujj@emb ~]$ cd 
[wujj@emb ~]$ tar -xzvf GeneralAgentTSHomeIPCServer.tgz 
[wujj@emb ~]$ ls
GeneralAgentTSHomeIPCServer libs ...
[wujj@emb ~]$ cd GeneralAgentTSHomeIPCServer
[wujj@emb ~]$ make 
[wujj@emb ~]$ ls
generalAgentMulti ... 
 
简要说明：
====================
main：main_multi.cpp 
端口等定义：
--------------------
GeneralAgentDef.h
THE_UDPSVR_AGENT_PORT 
 
udpserver： 
GeneralAgentUdpSocket.cpp  
数据收发接口    OnRawData()
 
消息处理：
--------------------
GeneralSocketProcessor 
处理接口 ThreadProc() ，具体参见代码
