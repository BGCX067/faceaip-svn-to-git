/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerialHttpServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerialHttpServer.h 5884 2012-09-06 01:31:28Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-06 01:31:28  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _TeleSerialHTTPSERVER_H
#define _TeleSerialHTTPSERVER_H

#include <HttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>

class CTeleSerialHttpServer : public HttpdSocket
{
public:
    CTeleSerialHttpServer(ISocketHandler& h);

    void Init();

    void Exec();

    void CreateHeader();

    void GenerateDocument();

    virtual void OnData(const char *,size_t);
    virtual void OnDataComplete();
private:
    	std::string m_strBody;
};

#endif // _TeleSerialHTTPSERVER_H
