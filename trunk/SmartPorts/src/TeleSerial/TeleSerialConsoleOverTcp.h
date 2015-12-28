/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerialConsoleOverTcp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerialConsoleOverTcp.h 0001 2012-04-28 09:58:37Z WuJunjie $
 *
 *  Explain:
 *     -self dbg platform-
 *
 *  Update:
 *     2012-04-28 09:58:37 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _TeleSerialConsoleOverTcp_H
#define _TeleSerialConsoleOverTcp_H

#include <TcpSocket.h>
#include <ISocketHandler.h>

#define LoginStatusNotlogin

enum    EnLoginStatus
{
    LoginStatusNotLogin =  0,
    LoginStatusWaitPwd,
    LoginStatusLoginUser,
    LoginStatusLoginAdmin,
};

class CTeleSerialConsoleOverTcp : public TcpSocket
{
public:
    CTeleSerialConsoleOverTcp(ISocketHandler&, std::string strSocketName="GeneralConsoleOverTcp");
    ~CTeleSerialConsoleOverTcp();

    void OnAccept();
    void OnLine(const std::string& );

    void Update();

private:
    void tprintf(const char *format, ...);
    void DumpPjturnStatus(/*pj_turn_srv *srv*/);
    void DumpUserInfo(std::string home);
    void Kick(std::string userName);
    void DumpIPCWebinfo();
    void TestAPNSPush(std::string msg);
    //void Block(std::string userName);

    EnLoginStatus m_enLoginStatus;
    std::string m_strUName;
    std::string m_strUPwd;

    std::string m_strLastCmd;

    time_t m_ttLastUpdate;
    bool m_bAutoDumpPJ;
    bool m_bAutoDumpUser;
};

#endif // _GeneralConsoleOverTcp_H
