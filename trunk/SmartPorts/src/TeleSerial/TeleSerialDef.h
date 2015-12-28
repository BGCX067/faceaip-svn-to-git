/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerialDef.h - _explain_
 *
 * Copyright (C) 2012 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerialDef.h 0001 2012-4-11 9:32:40Z WuJunjie $
 *
 *  Explain:
 *     -Common define-
 *
 *  Update:
 *     2012-4-11 9:32:49 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _TeleSerialDef_H
#define _TeleSerialDef_H

//#define TeleSerial_FUNC_DEFAULT_NEXT_HOST "192.168.10.24"
#define TeleSerial_FUNC_DEFAULT_NEXT_HOST "192.168.10.157"
#define TeleSerial_FUNC_DEFAULT_NEXT_HOST_PORT 60000

#define TeleSerial_FUNC_DEFAULT_NEXT_HTTP_HOST "192.168.10.160"
#define TeleSerial_FUNC_DEFAULT_NEXT_HTTP_PORT 80

#define TeleSerial_FUNC_THE_TCPSVR_AGENT_PORT 60000

#define TeleSerial_FUNC_THE_HTTP_SVR_AGENT_PORT 60080

#define TeleSerial_FUNC_THE_UDPSVR_AGENT_PORT 60000

#define TeleSerial_FUNC_THE_STATUS_AGENT_PORT 19199

// 调试端口
#define TeleSerial_FUNC_PORT_GENERAL_CONSOLE_OVER_TCP 19023

//// 他用时修改
//#define TeleSerial_FUNC_CONSOLE_OVERTCP          // #远程调试接口
//#define TeleSerial_FUNC_STATUS_AGENT             // #tcp状态服务器
//#define TeleSerial_FUNC_HTTPKEEPCONNECTOR   // #http协议的客户端
//#define TeleSerial_FUNC_DATAPARSE_EXAMPLE               // #数据解析示例
//#define TeleSerial_FUNC_TCPSOCKETSERVER     // #TCP服务端
//#define TeleSerial_FUNC_TCPSOCKETCONNECTOR  // #tcp客户端
//#define TeleSerial_FUNC_UDPSOCKETSERVER     // #UDP服务端
//#define TeleSerial_FUNC_SOCKETPROCESSOR          // #使用单独的处理线程
//#define TeleSerial_FUNC_HTTPSERVER          // #HTTP服务端
//#define TeleSerial_FUNC_SIGNAL_ONDATA                   // #使用数据回调机制
//#define TeleSerial_FUNC_WEBSERVICE                   // #webservice
//#define TeleSerial_FUNC_USE_LOG4CPP							// #

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#endif // _TeleSerialDef_H
