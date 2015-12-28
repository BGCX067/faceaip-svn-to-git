# Copyright (C) 2012  ezlibs.com
#
# platforms:
PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = linux-x86-64-centos5
#PLATFORM = linux-am335x
#android
#PLATFORM = linux-arm-none-gnueabi

VERSION =	1.3
DIFF_VERSION =	1.0

PROJECT_NAME=GeneralAgent

include         Makefile.Defines.$(PLATFORM)

#CFLAGS =	-Wall -g -O2 -MD
#CFLAGS += -D_DEBUG #debugme

##############################################################################
# 代码文件位置
GENERALAGENT_BASEDIR_SRC=../src

##############################################################################
#库文件位置
#使用自带库, 目前自带 android 和 x86-32的库
EZLIBS_BASEDIR_LIBS=../libs

##使用发布库
#EZLIBS_BASEDIR_LIBS=$(HOME)/libs

OBJS_GENERALAGENT = \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentLog.o \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentHandler.o\
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentTcpSocket.o \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgent.o

##############################################################################
## 配置模块
#ezConfig
CFLAGS += -DEXAMPLE_EZCONFIG
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/ezConfig
LIBS +=	-L$(EZLIBS_BASEDIR_LIBS)/lib -lezConfig-$(PLATFORM)
#json#thread
OBJS_CONFIGS	= \
		${GENERALAGENT_BASEDIR_SRC}/Configs/ConfigManager.o \
		${GENERALAGENT_BASEDIR_SRC}/Configs/ConfigGeneral.o \
		${GENERALAGENT_BASEDIR_SRC}/Configs/ConfigTcpSocketServer.o \
		$(GENERALAGENT_BASEDIR_SRC)/Configs/ConfigTcpSocketConnector.o \
		$(GENERALAGENT_BASEDIR_SRC)/Configs/ConfigConsoleOverTcp.o \
		${GENERALAGENT_BASEDIR_SRC}/Configs/ConfigDatabase.o \
		${GENERALAGENT_BASEDIR_SRC}/Configs/ConfigWebService.o
#
###############################################################################
### 日志模块
##log4cplus
#CFLAGS += -DEXAMPLE_LOG4CPP
#CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/log4cplus-${PLATFORM}/include
#LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/log4cplus-${PLATFORM}/lib -llog4cplus -lrt

##############################################################################
#以下功能可以随意组合， 但是至少保留一个功能，否则无意义
#不需要的文件可以删除

CFLAGS += -DEXAMPLE_GENERALCONSOLE_OVERTCP #远程调试接口
OBJS_GENERALAGENT += \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralConsoleOverTcp.o

CFLAGS += -DEXAMPLE_SOCKET_STATUS_AGENT #tcp状态服务器
OBJS_GENERALAGENT += \
		$(GENERALAGENT_BASEDIR_SRC)/SocketStatusAgent.o

#CFLAGS += -DEXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR #http协议的客户端
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentHttpKeepConnector.o

CFLAGS += -DEXAMPLE_DATAPARSE_EXAMPLE #数据解析示例

CFLAGS += -DEXAMPLE_GENERALAGENTTCPSOCKETSERVER #TCP服务端
OBJS_GENERALAGENT += \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentTcpSocketServer.o

#CFLAGS += -DEXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR #tcp客户端
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentTcpSocketConnector.o

CFLAGS += -DEXAMPLE_GENERALAGENTUDPSOCKETSERVER #UDP服务端
OBJS_GENERALAGENT += \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentUdpSocket.o

CFLAGS += -DEXAMPLE_GENERALSOCKETPROCESSOR #使用单独的处理线程
OBJS_GENERALAGENT += \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralSocketProcessor.o	\
		$(GENERALAGENT_BASEDIR_SRC)/SocketMsgQue.o

CFLAGS += -DEXAMPLE_GENERALAGENTHTTPSERVER #HTTP服务端
OBJS_GENERALAGENT += \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentHttpServer.o

CFLAGS += -DEXAMPLE_SIGNAL_ONDATA #使用数据回调机制

#######################################################################################################
##WebService服务器
CFLAGS += -DEXAMPLE_THE_WEBSERVICE
OBJS_GENERALAGENT += \
		${GENERALAGENT_BASEDIR_SRC}/WebService/Session.o \
		${GENERALAGENT_BASEDIR_SRC}/WebService/TSHomeWebServiceIF.o \
		${GENERALAGENT_BASEDIR_SRC}/WebService/HandlerWebService.o \
		${GENERALAGENT_BASEDIR_SRC}/WebService/HttpServerWebService.o \
		${GENERALAGENT_BASEDIR_SRC}/WebService/AgentWebService.o 

##############################################################################
#use ezutil library for http_parse
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)
#ezUtilCPP
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/ezUtilCPP/
LIBS +=	-L$(EZLIBS_BASEDIR_LIBS)/lib -lezUtilCPP-$(PLATFORM)

CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ppSockets
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lppSockets-$(PLATFORM)

## json
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/json
LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -ljson-$(PLATFORM)
#zlib
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/zlib
LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -lz-$(PLATFORM)

#
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezThread
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lezThread-$(PLATFORM)
LIBS +=	-lpthread

#libsigc, do not use it now, use mysignal instead
#CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/libsigc++-$(PLATFORM)/lib/sigc++-2.0/include
#CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/libsigc++-$(PLATFORM)/include/sigc++-2.0
#LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/libsigc++-${PLATFORM}/lib/ -lsigc-2.0

#CFLAGS += -DHAVE_LIBUUID
#LIBS += -luuid

#利用main调用，不用ezthread库
OBJS_MAINTHREAD_P = $(GENERALAGENT_BASEDIR_SRC)/main.o
OBJS_MAINTHREAD_P += $(OBJS_GENERALAGENT)
OBJS_MAINTHREAD_P += $(OBJS_CONFIGS)
DIST_MAINTHREAD = generalAgentMain-$(PLATFORM)

#多线程，用ezthread库
OBJS_MULTITHREAD_P = $(GENERALAGENT_BASEDIR_SRC)/main_multi.o
OBJS_MULTITHREAD_P += $(OBJS_GENERALAGENT)
OBJS_MULTITHREAD_P += $(OBJS_CONFIGS)
DIST_MULTITHREAD = generalAgentMulti-$(PLATFORM)

DIST =	$(DIST_MULTITHREAD)
#DIST +=	$(DIST_MAINTHREAD)

all: $(DIST)

$(DIST_MAINTHREAD):$(OBJS_MAINTHREAD_P)
		$(CPP) -o $@ $^ $(LIBS)

$(DIST_MULTITHREAD):$(OBJS_MULTITHREAD_P)
		$(CPP) -o $@ $^ $(LIBS)

tar:	clean
		tar -czf $(PROJECT_NAME).tgz *.h *.cpp Makefile*

-include *.d

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(DIST)
	find $(GENERALAGENT_BASEDIR_SRC) -name "*.o" |xargs rm -f
	find $(GENERALAGENT_BASEDIR_SRC) -name "*.d" |xargs rm -f
	find $(GENERALAGENT_BASEDIR_SRC) -name "*.cfbk" |xargs rm -f
