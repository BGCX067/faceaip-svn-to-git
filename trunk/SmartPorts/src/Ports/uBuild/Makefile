#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# Makefile.Client - Z:\Dropbox\stock\trunk\code\appmodule\GeneralAgent\build\
#
# Copyright (C) 2013 ezlibs.com, All Rights Reserved.
#
# $Id: Makefile.Client 1 2013-06-09 04:44:29 WuJunjie Exp $
#
# explain
#      for tcp client
#
# Update:
#    2013-06-09 04:44 WuJunjie Create
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# platforms:
PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = linux-x86-64-centos5

VERSION =	1.3
DIFF_VERSION =	1.0

PROJECT_NAME=SmartPorts

include         Makefile.Defines.$(PLATFORM)

#CFLAGS =	-Wall -g -O2 -MD
#CFLAGS += -D_DEBUG #debugme
#单独编译此模块
CFLAGS += -DU_BUILD_PORTS

##############################################################################
# 代码文件位置
GENERALAGENT_BASEDIR_SRC=..

CONFIGS_BASEDIR_SRC=../../Configs

##############################################################################
#库文件位置
#使用自带库
#EZLIBS_BASEDIR_LIBS=./libs

##使用发布库
EZLIBS_BASEDIR_LIBS=$(HOME)/libs

OBJS_GENERALAGENT = \
		$(GENERALAGENT_BASEDIR_SRC)/DoubleOutLog.o \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentHandler.o\
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentTcpSocket.o \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgent.o

##############################################################################
## 配置模块
#ezConfig
CFLAGS += -DUSE_EZCONFIG
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/ezConfig
LIBS +=	-L$(EZLIBS_BASEDIR_LIBS)/lib -lezConfig-$(PLATFORM)
#json#thread
OBJS_CONFIGS	= \
		${CONFIGS_BASEDIR_SRC}/ConfigManager.o \
		${CONFIGS_BASEDIR_SRC}/ConfigGeneral.o \
		${CONFIGS_BASEDIR_SRC}/ConfigTcpSocketServer.o \
		$(CONFIGS_BASEDIR_SRC)/ConfigTcpSocketConnector.o \
		$(CONFIGS_BASEDIR_SRC)/ConfigConsoleOverTcp.o \
		${CONFIGS_BASEDIR_SRC}/ConfigDatabase.o \
		${CONFIGS_BASEDIR_SRC}/ConfigWebService.o
CFLAGS += -I$(CONFIGS_BASEDIR_SRC)
		
## json
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/json
LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -ljson-$(PLATFORM)
#zlib
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/zlib
LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -lz-$(PLATFORM)

##############################################################################
## 日志模块
#log4cplus
CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/log4cplus-${PLATFORM}/include
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/log4cplus-${PLATFORM}/lib -llog4cplus -lrt

CFLAGS += -DUSE_LOG4CPP


##############################################################################
#以下功能可以随意组合， 但是至少保留一个功能，否则无意义
#不需要的文件可以删除

CFLAGS += -DUSE_GENERALCONSOLE_OVERTCP #远程调试接口
OBJS_GENERALAGENT += \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralConsoleOverTcp.o

#CFLAGS += -DUSE_SOCKET_STATUS_AGENT #tcp状态服务器
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/SocketStatusAgent.o

#CFLAGS += -DUSE_GENERALAGENTHTTPKEEPCONNECTOR #http协议的客户端
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentHttpKeepConnector.o

#CFLAGS += -DUSE_DATAPARSE_EXAMPLE #数据解析示例
#
#CFLAGS += -DUSE_GENERALAGENTTCPSOCKETSERVER #TCP服务端
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentTcpSocketServer.o

CFLAGS += -DUSE_GENERALAGENTTCPSOCKETCONNECTOR #tcp客户端
OBJS_GENERALAGENT += \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentTcpSocketConnector.o

#CFLAGS += -DUSE_GENERALAGENTUDPSOCKETSERVER #UDP服务端
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentUdpSocket.o

CFLAGS += -DUSE_GENERALSOCKETPROCESSOR #使用单独的处理线程
OBJS_GENERALAGENT += \
		$(GENERALAGENT_BASEDIR_SRC)/GeneralSocketProcessor.o	\
		$(GENERALAGENT_BASEDIR_SRC)/SocketMsgQue.o

#CFLAGS += -DUSE_GENERALAGENTHTTPSERVER #HTTP服务端
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/GeneralAgentHttpServer.o

#######################################################################################################
##WebService服务器
#CFLAGS += -DUSE_MY_WEBSERVICE
#
#OBJS_GENERALAGENT += \
#		${GENERALAGENT_BASEDIR_SRC}/WebService/Session.o \
#		${GENERALAGENT_BASEDIR_SRC}/WebService/TSHomeWebServiceIF.o \
#		${GENERALAGENT_BASEDIR_SRC}/WebService/HandlerWebService.o \
#		${GENERALAGENT_BASEDIR_SRC}/WebService/HttpServerWebService.o \
#		${GENERALAGENT_BASEDIR_SRC}/WebService/AgentWebService.o 

##############################################################################
#use ezutil library for http_parse
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)
#ezUtilCPP
CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/ezUtilCPP/
LIBS +=	-L$(EZLIBS_BASEDIR_LIBS)/lib -lezUtilCPP-$(PLATFORM)

CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ppSockets
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lppSockets-$(PLATFORM)
#
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezThread
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lezThread-$(PLATFORM)
LIBS +=	-lpthread

#libsigc
CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/libsigc++-$(PLATFORM)/lib/sigc++-2.0/include
CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/libsigc++-$(PLATFORM)/include/sigc++-2.0
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/libsigc++-${PLATFORM}/lib/ -lsigc-2.0
LIBS += -luuid

#多线程，用ezthread库
OBJS_MULTITHREAD_P = $(GENERALAGENT_BASEDIR_SRC)/main.o
OBJS_MULTITHREAD_P += $(OBJS_GENERALAGENT)
OBJS_MULTITHREAD_P += $(OBJS_CONFIGS)
DIST_MULTITHREAD = smartports-$(PLATFORM)

DIST =	$(DIST_MULTITHREAD)

all: $(DIST)

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
