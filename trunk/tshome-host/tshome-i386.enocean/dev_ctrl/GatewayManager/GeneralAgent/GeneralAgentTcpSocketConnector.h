/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocketConnector.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocketConnector.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _GENERALAGENTTCPSOCKETCONNECTOR_H
#define _GENERALAGENTTCPSOCKETCONNECTOR_H

#include "GeneralAgentTcpSocket.h"
#include <ISocketHandler.h>

class CGeneralAgentTcpSocketConnector : public CGeneralAgentTcpSocket
{
public:
    CGeneralAgentTcpSocketConnector(ISocketHandler&);
    ~CGeneralAgentTcpSocketConnector();

    void SetCloseAndDelete();
    void OnConnectFailed();
    void OnDelete();

    /////////////////////////////
    // �����������
    virtual void OnConnect();
    virtual void OnReconnect();
    virtual void OnDisconnect();
    virtual void OnConnectTimeout();
    virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);
	// ͨ��ͷ pCmd
	// ��� pData�� ���ȼ� pCmd->u8DataLen
	virtual void OnCommand_WifiAgent(remote_header_t *pCmd, unsigned char *pData);
    /////////////////////////////

private:
    CGeneralAgentTcpSocketConnector(const CGeneralAgentTcpSocketConnector& s) : CGeneralAgentTcpSocket(s)
    {} // copy constructor
    CGeneralAgentTcpSocketConnector& operator=(const CGeneralAgentTcpSocketConnector& )
    {
        return *this;
    } // assignment operator
    /** Create a new instance and reconnect */
    CGeneralAgentTcpSocketConnector *Reconnect();
    bool m_b_connected;
};

#endif // _GENERALAGENTTCPSOCKETCONNECTOR_H
