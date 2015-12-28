/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SmsStack.cpp - _explain_
 *
 * Copyright (C) 2012 ezlibs.com, All Rights Reserved.
 *
 * $Id: SmsStack.cpp 0001 2012-5-8 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-5-8 14:38:33 WuJunjie 549 Create
 *     2012-05-25 10:21:25 WuJunjie 549 all decode interface ready
 * 
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

//#include "CommonInclude.h"
#include <assert.h>
#include "SmsStack.h"

CSmsStack::CSmsStack()
{
    m_bDataValid = false;
    m_iError = 0;
    m_strError = "";

    //assert(m_pJsonValue);
}

CSmsStack::~CSmsStack()
{
    //    if (m_pJsonValue)
    //    {
    //        delete m_pJsonValue;
    //    }
}

bool CSmsStack::Parse( const std::string &document)
{
#if 0
    // 启用严格模式，让非法的json解析时直接返回false，不自动容错。
    // 编译失败， 库？
    Json::Features f = Json::Features::strictMode();
    Json::Reader reader(f);
#else

    Json::Reader reader;
#endif

    bool parsingSuccessful = reader.parse( document, m_JsonValue );

    if ( !parsingSuccessful )
    {
        printf( "Failed to parse %s file: \n%s\n",
                document.c_str(),
                reader.getFormatedErrorMessages().c_str() );

        m_strError = reader.getFormatedErrorMessages();
        m_iError = -1;
    }
    else
    {
        m_strError = "";
        m_iError = 0;
    }

    setValid(parsingSuccessful);
    return parsingSuccessful;
}

bool CSmsStack::ToString( std::string &document)
{
    if (!m_bDataValid)
    {
        return false;
    }

    //Json::StyledWriter writer(document);
    Json::FastWriter writer(document);
    writer.write( m_JsonValue );

    return true;
}

const std::string &CSmsStack::ToString()
{
    if (!m_bDataValid)
    {
        m_strDocument = "";
    }
    else
    {
        ToString(m_strDocument);
    }

    return m_strDocument;//generate json string:),here all is done
}

bool CSmsStack::isValid()
{
    return m_bDataValid;
}

void CSmsStack::setValid(bool bValid)
{
    m_bDataValid = bValid;
}

const int CSmsStack::getError() const
{
    return m_iError;
}

const std::string &CSmsStack::getErrorMessages() const
{
    return m_strError;
}

const Json::Value &CSmsStack::GetValue() const
{
    return m_JsonValue;
}

// just example
void CSmsStack::Encode()
{

    Json::Value var;

    //apply name and “game_id” value to json struct
    var["name"] = "json";
    var["age"] = 123;

    Json::Value pieces;//store all pieces
    for (int i=1351254;i < 1351254+3;i++)
    {
        Json::Value piece_ex;//here it store just one piece
        piece_ex["mobile"] = i;
        piece_ex["email"] = "ezlibs.com";
        piece_ex["fax"] = i+9;
        pieces.append(piece_ex);//ok,yes we just have apply One piece ,then push back to the array
    }
    var["joy"] = pieces;//yes,store pieces in var [Value]
    m_JsonValue = var;

    setValid();
}

void CSmsStack::Encode(const CAckMsgSimple &Msg)
{
    Json::Value var;

    var["result"] = Msg.result;
    var["reason"] = Msg.reason;

    m_JsonValue = var;
    setValid();
}

bool CSmsStack::Decode(CAckMsgSimple &Msg)
{
	// 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.result = m_JsonValue["result"].asString();
    Msg.reason = m_JsonValue["reason"].asString();

    return true;
}

void CSmsStack::Encode(const CAckMsgSession &Msg)
{
    Json::Value var;

    var["result"] = Msg.result;
    var["reason"] = Msg.reason;
    var["SessionId"] = Msg.SessionId;
    m_JsonValue = var;

    setValid();
}

bool CSmsStack::Decode(CAckMsgSession &Msg)
{
	// 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.result    = m_JsonValue["result"].asString();
    Msg.reason    = m_JsonValue["reason"].asString();
    Msg.SessionId = m_JsonValue["SessionId"].asString();

    return true;
}

void CSmsStack::Encode(const CSmsMsg_RegisterReq                                &Msg)
{
    Json::Value var;

    var["ProductID"]    = Msg.ProductID;
    var["AuthCode"]  = Msg.AuthCode;
    var["AuthName"]     = Msg.AuthName;
    var["Password"] = Msg.Password;

    var["LocalIp"] = Msg.m_LocalStatus.LocalIp;
    var["LocalDateTime"] = Msg.m_LocalStatus.LocalDateTime;

    m_JsonValue = var;

    setValid();
}

bool CSmsStack::Decode(CSmsMsg_RegisterReq &Msg)
{
	// 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.ProductID    = m_JsonValue["ProductID"]   .asString();
    Msg.AuthCode  = m_JsonValue["AuthCode"] .asString();
    Msg.AuthName     = m_JsonValue["AuthName"]    .asString();
    Msg.Password = m_JsonValue["Password"].asString();

    Msg.m_LocalStatus.LocalIp = m_JsonValue["LocalIp"].asString();
    Msg.m_LocalStatus.LocalDateTime = m_JsonValue["LocalDateTime"].asString();
    return true;
}

void CSmsStack::Encode(const CSmsMsg_RegisterAck                        &Msg)
{
    Json::Value var;
    unsigned int ii;

    var["session"] = Msg.m_Session;
    var["result"] = Msg.m_ackInfo.result;
    var["reason"] = Msg.m_ackInfo.reason;

    Json::Value operate_all;//store all pieces
    for (ii=0; ii<Msg.m_Service.size(); ii++)
    {
        Json::Value operate;//store all pieces

        operate["ServiceName"]     = Msg.m_Service[ii].ServiceName       ;
        operate["ServiceLocation"]  = Msg.m_Service[ii].ServiceLocation    ;

        operate_all.append(operate);
    }
    var["Service"] = operate_all;

    m_JsonValue = var;
    setValid();
}

bool CSmsStack::Decode(CSmsMsg_RegisterAck &Msg)
{
	// 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_ackInfo.result = m_JsonValue["result"].asString();
    Msg.m_ackInfo.reason = m_JsonValue["reason"].asString();
    Msg.m_Session = m_JsonValue["session"].asString();

    const Json::Value arrayOperate = m_JsonValue["Service"];

    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CSmsMsg_ServiceAgent _operate;
        _operate.ServiceName       = arrayOperate[i]["ServiceName"]     .asString();
        _operate.ServiceLocation    = arrayOperate[i]["ServiceLocation"]  .asString();

        Msg.m_Service.push_back(_operate);
    }

    return true;
}
