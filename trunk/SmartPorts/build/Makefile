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
PLATFORM = linux-am335x

VERSION =	0.9.0
DIFF_VERSION =	0.9.0

PROJECT_NAME=SmartPorts

include         Makefile.Defines.$(PLATFORM)

CFLAGS =	-Wall -O2 -MD
#CFLAGS += -D_DEBUG -g#debugme

#pass the ../src/Solar.h:54: warning: ??CSolar::m_TimerMaintain?ˉ will be initialized after
#CPPFLAGS += -Wno-reorder
#ignore warning: deprecated conversion from
CFLAGS += -Wno-write-strings

##############################################################################
# 代码文件位置
SMARTPORTS_BASEDIR_SRC=../src
GENERALAGENT_BASEDIR_SRC=../src/Ports

CONFIGS_BASEDIR_SRC=../src/Configs

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
		${CONFIGS_BASEDIR_SRC}/ConfigNetworkInterface.o \
		${CONFIGS_BASEDIR_SRC}/ConfigNet.o \
		${CONFIGS_BASEDIR_SRC}/ConfigSmartWebs.o \
		${CONFIGS_BASEDIR_SRC}/ConfigNTPClient.o \
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
CFLAGS += -DUSE_LOG4CPP
CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/log4cplus-${PLATFORM}/include
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/log4cplus-${PLATFORM}/lib -llog4cplus -lrt
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

#CFLAGS += -DUSE_GENERALSOCKETPROCESSOR #使用单独的处理线程
#OBJS_GENERALAGENT += \
#		$(GENERALAGENT_BASEDIR_SRC)/GeneralSocketProcessor.o	\
#		$(GENERALAGENT_BASEDIR_SRC)/SocketMsgQue.o

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
##WebServer
CFLAGS += -DUSE_SMARTWEBS
SMARTWEBS_BASEDIR_SRC=../src/SmartWebs
OBJS_DEP_SMARTWEBS = \
		$(SMARTWEBS_BASEDIR_SRC)/WebsSmartPorts.o \
		$(SMARTWEBS_BASEDIR_SRC)/smart_webs.o \
		$(SMARTWEBS_BASEDIR_SRC)/websFunction.o \
		$(SMARTWEBS_BASEDIR_SRC)/enocean/enocean_mgr.o \
		$(SMARTWEBS_BASEDIR_SRC)/enocean/esp.o

# User Management switch
CFLAGS	+= -DUSER_MANAGEMENT_SUPPORT
# Digest Access switch
CFLAGS	+= -DDIGEST_ACCESS_SUPPORT
CFLAGS  += -DUSE_USB_INTERFACE -D__UNIT_TEST -DDEV_ENOCEAN_SUPPORT
#CFLAGS  += -DUSE_PROCESS_THREAD
CFLAGS  += -DCHECK_STATUS_THREAD
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezserial
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezserial-$(PLATFORM)
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)
CFLAGS += -I$(HOME)/libs/include/ezsocket
LIBS += -L$(HOME)/libs/lib -lezsocket-$(PLATFORM)

CFLAGS	+= -DWEBS -DOS="LINUX" -DLINUX
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/aspwebs
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -laspwebs-$(PLATFORM) -lpthread
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezplatform
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezplatform-$(PLATFORM)
##############################################################################
#ntp
CFLAGS  += -DFUNCTION_DATETIME_AND_NTP


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
#todo
#LIBS += -luuid

#多线程，用ezthread库
OBJS_MULTITHREAD_P = \
	${SMARTPORTS_BASEDIR_SRC}/Stack/SmsStack.o \
	$(SMARTPORTS_BASEDIR_SRC)/SysMan/ezSms.o \
	$(SMARTPORTS_BASEDIR_SRC)/main.o \
	$(SMARTPORTS_BASEDIR_SRC)/Solar.o
##############################################################################
#自动维护功能
OBJS_MULTITHREAD_P += \
	$(SMARTPORTS_BASEDIR_SRC)/SysMan/Maintenance.o
##############################################################################
	
OBJS_MULTITHREAD_P += $(OBJS_GENERALAGENT)
OBJS_MULTITHREAD_P += $(OBJS_CONFIGS)
OBJS_MULTITHREAD_P += $(OBJS_DEP_SMARTWEBS)
DIST_MULTITHREAD = smartports-$(PLATFORM)

DIST =	$(DIST_MULTITHREAD)

CPPFLAGS  = $(CFLAGS)
CPPFLAGS += -Wno-reorder

all: $(DIST)

$(DIST_MULTITHREAD):$(OBJS_MULTITHREAD_P)
		$(CPP) -O2 -o $@ $^ $(LIBS)
		$(STRIP) $(DIST_MULTITHREAD)

tar:	clean
		tar -czf $(PROJECT_NAME).tgz *.h *.cpp Makefile*

-include *.d

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPP) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f $(DIST)
	find $(SMARTPORTS_BASEDIR_SRC) -name "*.o" |xargs rm -f
	find $(SMARTPORTS_BASEDIR_SRC) -name "*.d" |xargs rm -f
	find $(SMARTPORTS_BASEDIR_SRC) -name "*.cfbk" |xargs rm -f

dbg:$(DIST)
	mv $(DIST) /home/share
	rm -rf /home/share/spages
	cp ../src/SmartWebs/uBuild/spages /home/share/spages/ -rf

