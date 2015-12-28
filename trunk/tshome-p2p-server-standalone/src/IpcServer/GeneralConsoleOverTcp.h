/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralConsoleOverTcp.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralConsoleOverTcp.h 0001 2012-04-28 09:58:37Z WuJunjie $
 *
 *  Explain:
 *     -self dbg platform-
 *
 *  Update:
 *     2012-04-28 09:58:37 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _GeneralConsoleOverTcp_H
#define _GeneralConsoleOverTcp_H

#include <TcpSocket.h>
#include <ISocketHandler.h>

#include "app.h"

#define LoginStatusNotlogin

enum    EnLoginStatus
{
    LoginStatusNotLogin =  0,
    LoginStatusWaitPwd,
    LoginStatusLoginUser,
    LoginStatusLoginAdmin,
};

class CGeneralConsoleOverTcp : public TcpSocket
{
public:
    CGeneralConsoleOverTcp(ISocketHandler&, std::string strSocketName="GeneralConsoleOverTcp");
    ~CGeneralConsoleOverTcp();

    void OnAccept();
    void OnLine(const std::string& );
private:
    void tprintf(const char *format, ...);
    void DumpPjturnStatus(pj_turn_srv *srv);
	void DumpUserInfo(std::string home);
	void Kick(std::string userName);
	void DumpIPCWebinfo();
	void TestAPNSPush(std::string msg); 
	//void Block(std::string userName);
	
    EnLoginStatus m_enLoginStatus;
    std::string m_strUName;
    std::string m_strUPwd;

    std::string m_strLastCmd;
};

#endif // _GeneralConsoleOverTcp_H
