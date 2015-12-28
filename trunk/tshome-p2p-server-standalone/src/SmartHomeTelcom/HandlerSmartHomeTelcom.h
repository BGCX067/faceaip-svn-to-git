/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CHandlerSmartHomeTelcom.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: CHandlerSmartHomeTelcom.h 0001 2012-04-06 09:54:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerSmartHomeTelcom_H
#define _HandlerSmartHomeTelcom_H
#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>
#include "../common/DoubleOutLog.h"


class CHandlerSmartHomeTelcom : public SocketHandler
{
public:
	CHandlerSmartHomeTelcom(CDoubleOutLog *pLog=NULL);

	void tprintf(TcpSocket *,const char *format, ...);
	void List(TcpSocket *);
	void Update();
	
	Socket *GetSocketByName(std::string strServerName);
};

#endif // _HandlerSmartHomeTelcom_H
