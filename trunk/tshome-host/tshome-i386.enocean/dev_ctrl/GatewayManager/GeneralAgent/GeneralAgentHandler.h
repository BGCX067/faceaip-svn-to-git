/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentHandler.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentHandler.h 0001 2012-04-06 09:54:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _GENERALAGENTHANDLER_H
#define _GENERALAGENTHANDLER_H
#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>


class GeneralAgentHandler : public SocketHandler
{
public:
	GeneralAgentHandler();

	void tprintf(TcpSocket *,const char *format, ...);
	void List(TcpSocket *);
	void SendToAllHost(char * pBuf, int nLen);
	void Update();
};

#endif // _GENERALAGENTHANDLER_H
