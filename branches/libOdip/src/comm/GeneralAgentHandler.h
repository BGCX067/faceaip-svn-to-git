/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentHandler.h - _explain_
 *
 * Copyright (C) 2011 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: GeneralAgentHandler.h 0001 2012-04-06 09:54:05Z WuJunjie $
 *
 *  Explain:
 *     -·´Ó¦¶Ñ-
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
#include "DoubleOutLog.h"

class GeneralAgentHandler : public SocketHandler
{
public:
	GeneralAgentHandler(CDoubleOutLog *pLog=NULL);

	void tprintf(TcpSocket *,const char *format, ...);
	void List(TcpSocket *);
	void Update();
	
	Socket *GetSocketByName(std::string strServerName);

	void SocketSetCloseAndDelete();
};

#endif // _GENERALAGENTHANDLER_H
