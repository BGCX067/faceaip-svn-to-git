/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SmsStack.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: SmsStack.h 5884 2013-07-05 11:10:13Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-05 11:10:13 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _SmsStack_H
#define _SmsStack_H

#include <json.h>
#include <algorithm>
#include <stdio.h>

#include "SmsMsg.h"

class CSmsStack
{
public :
    CSmsStack();
    virtual ~CSmsStack();

    virtual bool Parse( const std::string &document);
    bool ToString( std::string &document);
    const std::string &ToString();

    virtual const int getError() const;
    virtual const std::string &getErrorMessages() const;

    virtual void Encode(); //example only

    virtual void Encode(const CAckMsgSimple                                &Msg);
    virtual void Encode(const CAckMsgSession                               &Msg);
    virtual void Encode(const CSmsMsg_RegisterReq                        &Msg);
    virtual void Encode(const CSmsMsg_RegisterAck                        &Msg);
    
    // 下列Msg， 调用者自行保证初始化， 接口内部不初始化（vector）
    bool Decode(CAckMsgSimple                                &Msg);
    bool Decode(CAckMsgSession                               &Msg);
    bool Decode(CSmsMsg_RegisterReq                        &Msg);
    bool Decode(CSmsMsg_RegisterAck                        &Msg);

    const Json::Value &GetValue() const;

    bool isValid();
    void setValid(bool bValid=true);
    void dump();

protected:
    bool m_bDataValid;
    Json::Value m_JsonValue;
    std::string m_strDocument;
    int m_iError;
    std::string m_strError;
};
#endif /* _SmsStack_H */
