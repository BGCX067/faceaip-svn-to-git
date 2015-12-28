/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentDef.h - _explain_
 *
 * Copyright (C) 2012 tiansu-china.com, All Rights Reserved.
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

#define DEFAULT_NEXT_HOST "192.168.10.116"
#define DEFAULT_NEXT_HOST_PORT 60000

#define DEFAULT_NEXT_HTTP_HOST "192.168.10.246"
#define DEFAULT_NEXT_HTTP_PORT 80

#define THE_TCPSVR_AGENT_PORT 60000
#define THE_UDPSVR_AGENT_PORT 60000
#define THE_UDPSVR_AGENT_NAME "TSHomeIpcServer"

#define THE_STATUS_AGENT_PORT 19199

// µ÷ÊÔ¶Ë¿Ú
#define PORT_GENERAL_CONSOLE_OVER_TCP 19023

#define ERR_CODE_SENDTO_NOT_TERMINAL -100
#define ERR_CODE_PARAM_NOT_VALID ERR_CODE_SENDTO_NOT_TERMINAL-1

#endif // _GENERALAGENTDEF_H
