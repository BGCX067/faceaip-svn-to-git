# Copyright (C) 2012  tiansu-china.com
# 
# platforms:
PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = linux-arm-none-gnueabi

VERSION =	1.3
DIFF_VERSION =	1.0

SRC_PATH=../src/comm

CFLAGS =	-Wall -g -O2 -MD
#CFLAGS += -D_DEBUG #debugme

OBJS_GENERALAGENT = \
		$(SRC_PATH)/DoubleOutLog.o \
		$(SRC_PATH)/GeneralAgentHandler.o\
		$(SRC_PATH)/GeneralAgentTcpSocket.o \
		$(SRC_PATH)/GeneralAgent.o

OBJS_GENERALAGENT += \
		$(SRC_PATH)/../nvrnetapi.o

##############################################################################
#以下功能可以随意组合， 但是至少保留一个功能，否则无意义
#不需要的文件可以删除

#CFLAGS += -DUSE_GENERALCONSOLE_OVERTCP #远程调试接口
#OBJS_GENERALAGENT += \
#		./GeneralConsoleOverTcp.o
#
#CFLAGS += -DUSE_SOCKET_STATUS_AGENT #tcp状态服务器
#OBJS_GENERALAGENT += \
#		./SocketStatusAgent.o
#
#CFLAGS += -DUSE_GENERALAGENTHTTPKEEPCONNECTOR #http协议的客户端
#OBJS_GENERALAGENT += \
#		./GeneralAgentHttpKeepConnector.o

CFLAGS += -DUSE_DATAPARSE_EXAMPLE #数据解析示例

#CFLAGS += -DUSE_GENERALAGENTTCPSOCKETSERVER #TCP服务端
#OBJS_GENERALAGENT += \
#		./GeneralAgentTcpSocketServer.o

CFLAGS += -DUSE_GENERALAGENTTCPSOCKETCONNECTOR #客户端
OBJS_GENERALAGENT += \
		$(SRC_PATH)/GeneralAgentTcpSocketConnector.o
		
#CFLAGS += -DUSE_GENERALAGENTUDPSOCKETSERVER #UDP服务端
#OBJS_GENERALAGENT += \
#		./GeneralAgentUdpSocket.o

CFLAGS += -DUSE_GENERALSOCKETPROCESSOR #使用单独的处理线程
OBJS_GENERALAGENT += \
		$(SRC_PATH)/GeneralSocketProcessor.o	\
		$(SRC_PATH)/SocketMsgQue.o
		
#CFLAGS += -DUSE_GENERALAGENTHTTPSERVER #HTTP服务端
#OBJS_GENERALAGENT += \
#		./GeneralAgentHttpServer.o
		
##############################################################################

LIBS +=	-L$(HOME)/libs/lib -lppSockets-$(PLATFORM)

#利用main调用，不用ezthread库
OBJS_MAINTHREAD_P = main.o
OBJS_MAINTHREAD_P += $(OBJS_GENERALAGENT)
DIST_MAINTHREAD = mainOdipTest-$(PLATFORM)

#多线程，用ezthread库
OBJS_MULTITHREAD_P = main_multi.o
OBJS_MULTITHREAD_P += $(OBJS_GENERALAGENT)
DIST_MULTITHREAD = mainOdipTestMultiThread

DIST =	$(DIST_MAINTHREAD)
# $(DIST_MULTITHREAD)

include         Makefile.version
include         Makefile.Defines.$(PLATFORM)

CFLAGS += -I$(SRC_PATH)
CFLAGS += -I$(HOME)/libs/include/ppSockets
#
CFLAGS += -I$(HOME)/libs/include/ezThread



CPPFLAGS =	$(CFLAGS)

CFLAGS += -I$(HOME)/libs/include/ezUtilCPP
LIBS +=	-L$(HOME)/libs/lib -lezUtilCPP-$(PLATFORM)
#use ezutil library for http_parse
CFLAGS += -I$(HOME)/libs/include/ezutil
LIBS +=	-L$(HOME)/libs/lib -lezutil-$(PLATFORM)

LIBS +=	-L$(HOME)/libs/lib -lezThread-$(PLATFORM)
LIBS +=	-lpthread

$(DIST_MAINTHREAD):$(OBJS_MAINTHREAD_P)
		$(CPP) $(PLATFORM_LINK_FLAGS) -o $@ $^ $(LIBS)

$(DIST_MULTITHREAD):$(OBJS_MULTITHREAD_P)
		$(CPP) -o $@ $^ $(LIBS)

clean:
	rm -f $(DIST) $(OBJS_MULTITHREAD_P) $(OBJS_MAINTHREAD_P) *.o *.gdb *.d *.cfbk *.tgz
	
FILES =	*.h *.cpp Makefile*

tar:	clean
		tar -czf $(DIST).tgz $(FILES)

-include *.d

