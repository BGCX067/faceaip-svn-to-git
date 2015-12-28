/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocketConnector.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocketConnector.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -Tcp客户端实例-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _GENERALAGENTTCPSOCKETCONNECTOR_H
#define _GENERALAGENTTCPSOCKETCONNECTOR_H

#include "GeneralAgentTcpSocket.h"
#include <ISocketHandler.h>


class CTeleSerialTcpSocketConnector : public CTeleSerialTcpSocket
{
public:
    CTeleSerialTcpSocketConnector(ISocketHandler&
                                    , std::string strSocketName="GeneralAgentTcpSocketConnector"
                                                                , time_t Timeout= 5
                                                                                  , bool bReconn = false);
    ~CTeleSerialTcpSocketConnector();

    void SetCloseAndDelete();
    void OnConnectFailed();
    void OnDelete();

    /////////////////////////////
    // 加入命令解析
    virtual void OnConnect();
    virtual void OnReconnect();
    virtual void OnDisconnect();
    virtual void OnConnectTimeout();
    virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);
#ifdef TeleSerial_FUNC_DATAPARSE_EXAMPLE
    // 通用头 pCmd
    // 数据 pData， 长度见 pCmd->u8DataLen
    virtual void OnCommand_WifiAgent(WIFI_AGENT_PROTOCOL_HEAD *pCmd, unsigned char *pData);
#endif
    /////////////////////////////

private:
    CTeleSerialTcpSocketConnector(const CTeleSerialTcpSocketConnector& s) : CTeleSerialTcpSocket(s)
    {} // copy constructor
    CTeleSerialTcpSocketConnector& operator=(const CTeleSerialTcpSocketConnector& )
    {
        return *this;
    } // assignment operator
    /** Create a new instance and reconnect */
    CTeleSerialTcpSocketConnector *Reconnect();
    bool m_b_connected;

    ///////////
    std::string m_strSocketName;
    time_t m_ttTimeout;
    bool m_bReconn;
    ///////////
};

#endif // _GENERALAGENTTCPSOCKETCONNECTOR_H
