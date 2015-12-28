/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerSmartHomeTelcomCtrlServerCtrlServer.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: HandlerSmartHomeTelcomCtrlServerCtrlServer.h 5884 2012-09-06 03:22:35Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-06 03:22:35  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _HandlerSmartHomeTelcomCtrlServer_H
#define _HandlerSmartHomeTelcomCtrlServer_H
#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>

#include "../../common/DoubleOutLog.h"

class CHandlerSmartHomeTelcomCtrlServer : public SocketHandler
{
public:
	CHandlerSmartHomeTelcomCtrlServer(CDoubleOutLog *pLog=NULL);

	void tprintf(TcpSocket *,const char *format, ...);
	void List(TcpSocket *);
	void Update();
	
	Socket *GetSocketByName(std::string strServerName);
};

#endif // _HandlerSmartHomeTelcomCtrlServer_H
