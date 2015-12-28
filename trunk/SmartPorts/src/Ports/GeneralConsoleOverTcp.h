/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralConsoleOverTcp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
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

//#include "app.h"

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

    void Update();

private:
    void tprintf(const char *format, ...);
    void DumpGeneralStatus(/*pj_turn_srv *srv*/);
    void DumpUserInfo(std::string home);
    void Kick(std::string userName);
    void DumpIPCWebinfo();
    void TestConfigs(const std::string &strCmd, const std::string &strArg);
    //void Block(std::string userName);

    EnLoginStatus m_enLoginStatus;
    std::string m_strUName;
    std::string m_strUPwd;

    std::string m_strLastCmd;

    time_t m_ttLastUpdate;
    bool m_bAutoDumpStatus;
    bool m_bAutoDumpUser;
};

#endif // _GeneralConsoleOverTcp_H
