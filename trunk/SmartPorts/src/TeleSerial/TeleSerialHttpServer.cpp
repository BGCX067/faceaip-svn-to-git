/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerialHttpServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerialHttpServer.cpp 5884 2012-09-06 01:31:44Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-06 01:31:44  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <string.h>
#include <stdio.h>

#include "TeleSerialHttpServer.h"

CTeleSerialHttpServer::CTeleSerialHttpServer(ISocketHandler& h) : HttpdSocket(h)
{m_strBody = "";}

void CTeleSerialHttpServer::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }
}

void CTeleSerialHttpServer::Exec()
{
    CreateHeader();
    GenerateDocument();
}

void CTeleSerialHttpServer::CreateHeader()
{
    SetStatus("200");
    SetStatusText("OK");
    fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    {
        size_t x = 0;
        for (size_t i = 0; i < GetUri().size(); i++)
            if (GetUri()[i] == '.')
                x = i;
        std::string ext = GetUri().substr(x + 1);
        if (ext == "gif" || ext == "jpg" || ext == "png")
            AddResponseHeader("Content-type", "image/" + ext);
        else
            AddResponseHeader("Content-type", "text/" + ext);
    }
    AddResponseHeader("Connection", "close");
    SendResponse();
}

void CTeleSerialHttpServer::GenerateDocument()
{
    std::string strSendBuf = "CTeleSerialHttpServer.\n\r";

    strSendBuf += GetHttpDate()+"\r\n";
    strSendBuf += "Your Body:["+m_strBody+"]\r\n";

    Send(strSendBuf);

    SetCloseAndDelete();
}



void CTeleSerialHttpServer::OnData(const char *p,size_t l)
{
	m_strBody += p;
	printf("m_strBody:%s\n", m_strBody.c_str());
}

void CTeleSerialHttpServer::OnDataComplete()
{
    Exec();
    Reset(); // prepare for next request
    m_strBody = "";
}

