/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentDef.h - _explain_
 *
 * Copyright (C) 2012 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentDef.h 0001 2012-4-11 9:32:40Z WuJunjie $
 *
 *  Explain:
 *     -Common define-
 *
 *  Update:
 *     2012-4-11 9:32:49 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _GENERALAGENTDEF_H
#define _GENERALAGENTDEF_H

//#define DEFAULT_NEXT_HOST "192.168.10.24"
#define DEFAULT_NEXT_HOST "192.168.10.216"
#define DEFAULT_NEXT_HOST_PORT 60000

//#define DEFAULT_NEXT_HTTP_HOST "192.168.10.160"
//#define DEFAULT_NEXT_HTTP_PORT 80
#define DEFAULT_NEXT_HTTP_HOST "sms.ezlibs.com"
#define DEFAULT_NEXT_HTTP_PORT 60088

#define THE_TCPSVR_AGENT_PORT 60000

#define THE_HTTP_SVR_AGENT_PORT 60080

#define THE_UDPSVR_AGENT_PORT 60000

#define THE_STATUS_AGENT_PORT 19199

// 调试端口
#define PORT_GENERAL_CONSOLE_OVER_TCP 19023

//// 他用时修改
//#define EXAMPLE_GENERALCONSOLE_OVERTCP          // #远程调试接口
//#define EXAMPLE_SOCKET_STATUS_AGENT             // #tcp状态服务器
//#define EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR   // #http协议的客户端
//#define EXAMPLE_DATAPARSE_EXAMPLE               // #数据解析示例
//#define EXAMPLE_GENERALAGENTTCPSOCKETSERVER     // #TCP服务端
//#define EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR  // #tcp客户端
//#define EXAMPLE_GENERALAGENTUDPSOCKETSERVER     // #UDP服务端
//#define EXAMPLE_GENERALSOCKETPROCESSOR          // #使用单独的处理线程
//#define EXAMPLE_GENERALAGENTHTTPSERVER          // #HTTP服务端
//#define EXAMPLE_SIGNAL_ONDATA                   // #使用数据回调机制
//#define EXAMPLE_THE_WEBSERVICE                   // #webservice
//#define EXAMPLE_LOG4CPP							// #

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#endif // _GENERALAGENTDEF_H
