/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeHomeIF.cpp - _explain_
 *
 * Copyright (C) 2012 tiansu-china.com, All Rights Reserved.
 *
 * $Id: TeHomeIF.cpp 0001 2012-5-8 14:38:29Z wu_junjie $
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

#include "CommonInclude.h"
#include <assert.h>
#include "TeHomeIF.h"

CTeHomeIF::CTeHomeIF()
{
    m_bDataValid = false;
    m_iError = 0;
    m_strError = "";

    //assert(m_pJsonValue);
}

CTeHomeIF::~CTeHomeIF()
{
    //    if (m_pJsonValue)
    //    {
    //        delete m_pJsonValue;
    //    }
}

bool CTeHomeIF::Parse( const std::string &document)
{
    Json::Reader reader;
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

bool CTeHomeIF::ToString( std::string &document)
{
    if (!m_bDataValid)
    {
        return false;
    }

    //Json::StyledWriter writer(document);
    Json::FastWriter writer(document);
	
    //writer.write( m_JsonValue );
    writer.writeCompact( m_JsonValue );

    return true;
}

const std::string &CTeHomeIF::ToString()
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

bool CTeHomeIF::isValid()
{
    return m_bDataValid;
}

void CTeHomeIF::setValid(bool bValid)
{
    m_bDataValid = bValid;
}

const int CTeHomeIF::getError() const
{
    return m_iError;
}

const std::string &CTeHomeIF::getErrorMessages() const
{
    return m_strError;
}

const Json::Value &CTeHomeIF::GetValue() const
{
    return m_JsonValue;
}

// just example
void CTeHomeIF::Encode()
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
        piece_ex["email"] = "joy.woo@hotmail.com";
        piece_ex["fax"] = i+9;
        pieces.append(piece_ex);//ok,yes we just have apply One piece ,then push back to the array
    }
    var["joy"] = pieces;//yes,store pieces in var [Value]
    m_JsonValue = var;

    setValid();
}

void CTeHomeIF::Encode(const CAckMsgSimple &Msg)
{
    Json::Value var;

    var["result"] = Msg.result;
    var["reason"] = Msg.reason;

    m_JsonValue = var;
    setValid();
}

bool CTeHomeIF::Decode(CAckMsgSimple &Msg)
{
    if ( m_JsonValue.type() == Json::arrayValue)
    {
        int size = m_JsonValue.size();
        size = size>1?1:size;
        //数组类型，使用数字下标作为索引遍历所有元素
        for ( int index =0; index < size; ++index )
        {
            CTSHomeHostHttpMsg_Operate __op;
            const Json::Value arrayOperate = m_JsonValue[index];

            Msg.result = arrayOperate["result"].asString();
            Msg.reason  = arrayOperate["reason"] .asString(); //
        }
    }
    else
    {
        Msg.result = m_JsonValue["result"].asString();
        Msg.reason = m_JsonValue["reason"].asString();
    }

    return true;
}

void CTeHomeIF::Encode(const CAckMsgSession &Msg)
{
    Json::Value var;

    var["result"] = Msg.result;
    var["reason"] = Msg.reason;
    var["sessionId"] = Msg.SessionId;
    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CAckMsgSession &Msg)
{

    Msg.result    = m_JsonValue["result"].asString();
    Msg.reason    = m_JsonValue["reason"].asString();
    Msg.SessionId = m_JsonValue["sessionId"].asString();

    return true;
}

void CTeHomeIF::Encode(const CAgentMsg_requestAuth                                &Msg)
{
    Json::Value var;

    var["productID"]    = Msg.ProductID;
    var["productName"]  = Msg.ProductName;
    var["authUser"]     = Msg.AuthUser;
    var["authPassword"] = Msg.AuthPassword;

    m_JsonValue = var;

    setValid();
}
bool CTeHomeIF::Decode(CAgentMsg_requestAuth &Msg)
{

    Msg.ProductID    = m_JsonValue["ProductID"]   .asString();
    Msg.ProductName  = m_JsonValue["ProductName"] .asString();
    Msg.AuthUser     = m_JsonValue["AuthUser"]    .asString();
    Msg.AuthPassword = m_JsonValue["AuthPassword"].asString();

    return true;
}

void CTeHomeIF::Encode(const CAgentMsg_certificationActivation                   &Msg)
{
    Json::Value var;

    var["SessionId"]    = Msg.SessionId;
    var["UserId"]       = Msg.UserId   ;
    var["DeviceId"]     = Msg.DeviceId ;

    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CAgentMsg_certificationActivation &Msg)
{
    Msg.SessionId   = m_JsonValue["SessionId"] .asString();
    Msg.UserId      = m_JsonValue["UserId"]    .asString();
    Msg.DeviceId    = m_JsonValue["DeviceId"]  .asString();

    return true;
}

//配置上报接口
// 这个接口实现的都是组合的情况， 实现好备用
// 一次上传多个 平台不支持2012-9-5 16:05:01
void CTeHomeIF::Encode_FULL(const CAgentMsg_initDeviceConfigParam                     &Msg)
{
    Json::Value var;
    unsigned int ii;
    unsigned int jj;


    Json::Value seat;//store all pieces
    seat["seat_name"] = Msg.m_Seat.seat_name;

    //__fline;
    //printf("Msg.m_Seat.m_deploy.size():%d\n", Msg.m_Seat.m_deploy.size());

    Json::Value deploy_all;//store all pieces
    for (ii=0; ii<Msg.m_Seat.m_deploy.size(); ii++)
    {
        Json::Value deploy;//store all pieces

        deploy["deploy_name"] = Msg.m_Seat.m_deploy[ii].deploy_name ;
        deploy["deploy_type"] = Msg.m_Seat.m_deploy[ii].deploy_type ;
        deploy["deviceType"]  = Msg.m_Seat.m_deploy[ii].deviceType  ;
        deploy["device_model"]= Msg.m_Seat.m_deploy[ii].device_model;

        Json::Value operate;//store all pieces
        for (jj=0;jj < Msg.m_Seat.m_deploy[ii].m_Operate.size(); jj++)
        {
            Json::Value pieces;//here it store just one piece

            pieces["operate_id"     ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_id     ;
            pieces["operate_ranage" ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_ranage ;
            pieces["operate_type"   ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_type   ;
            pieces["operate_value"  ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_value  ;
            pieces["operate_explain"] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_explain;
            operate.append(pieces);
        }

        deploy["operate"] = operate;//ok,yes we just have apply One piece ,then push back to the array

        deploy_all.append(deploy);
    }
    seat["deploy"] = deploy_all;

    var["sessionId"] = Msg.sessionId;
    var["productID"] = Msg.productID;
    var["userID"]    = Msg.userID   ;
    var["deviceID"]  = Msg.deviceID ;
    var["seat"]      = seat;

    m_JsonValue = var;

    setValid();
}

/*
	调试时间：2012-9-1 15:33:57
		结果：{"result":"1","reason":"认证失败，请重新认证."}
*/
void CTeHomeIF::Encode(const CAgentMsg_initDeviceConfigParam                     &Msg)
{
    setValid(false);

    // 根据协议文档来看，如此， 否则用Encode_FULL来编码
    if (Msg.m_Seat.m_deploy.size()!=1)
    {
        return;
    }

    if (Msg.m_Seat.m_deploy[0].m_Operate.size()!=1)
    {
        return;
    }

    Json::Value var;
    unsigned int ii;
    unsigned int jj;

    Json::Value seat;//store all pieces
    seat["seat_name"] = Msg.m_Seat.seat_name;

    //Json::Value deploy_all;//store all pieces
    for (ii=0; ii<Msg.m_Seat.m_deploy.size(); ii++)
    {
        Json::Value deploy;//store all pieces

        deploy["deploy_name"] = Msg.m_Seat.m_deploy[ii].deploy_name ;
        deploy["deploy_type"] = Msg.m_Seat.m_deploy[ii].deploy_type ;
        deploy["deviceType"]  = Msg.m_Seat.m_deploy[ii].deviceType  ;
// 这里没有 ， 在请求上报中有
		//deploy["device_model"]= Msg.m_Seat.m_deploy[ii].device_model;

        //Json::Value operate;//store all pieces
        for (jj=0;jj < Msg.m_Seat.m_deploy[ii].m_Operate.size(); jj++)
        {
            Json::Value pieces;//here it store just one piece

            pieces["operate_id"     ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_id     ;
            pieces["operate_ranage" ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_ranage ;
            pieces["operate_type"   ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_type   ;
            pieces["operate_value"  ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_value  ;
            pieces["operate_explain"] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_explain;
            //operate.append(pieces);
            deploy["operate"] = pieces;//ok,yes we just have apply One piece ,then push back to the array

        }


        //deploy_all.append(deploy);
        seat["deploy"] = deploy;
    }

    var["requestMark"] = Msg.requestMark; // 不在消息体中。。。。
    var["sessionId"] = Msg.sessionId ;
    var["productID"] = Msg.productID ;
    var["userID"]    = Msg.userID    ;
    var["deviceID"]  = Msg.deviceID  ;
    var["seat"] = seat;

    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CAgentMsg_initDeviceConfigParam &Msg)
{
    //CAgentMsg_initDeviceConfigParam Msg;
    //CAgentMsg_initDeviceConfigParam_Seat __seat;
    CAgentMsg_initDeviceConfigParam_Seat_deploy __deploy;
    CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate __operate;

    const Json::Value arraySeat = m_JsonValue["seat"];
    const Json::Value arrayDeploy = arraySeat["deploy"];
    const Json::Value arrayOperate = arrayDeploy["operate"];

    __operate.operate_id      = arrayOperate["operate_id"].asString();
    __operate.operate_ranage  = arrayOperate["operate_ranage"].asString();
    __operate.operate_type    = arrayOperate["operate_type"].asString();
    __operate.operate_value   = arrayOperate["operate_value"].asString();
    __operate.operate_explain = arrayOperate["operate_explain"].asString();

    __deploy.deploy_name = arrayDeploy["deploy_name"] .asString();
    __deploy.deploy_type = arrayDeploy["deploy_type"] .asString();
    __deploy.deviceType  = arrayDeploy["deviceType"]  .asString();
    __deploy.device_model= arrayDeploy["device_model"].asString();
    __deploy.m_Operate.push_back(__operate);

    Msg.sessionId = m_JsonValue["SessionId"].asString();
    Msg.productID = m_JsonValue["ProductId"].asString();
    Msg.userID    = m_JsonValue["UserID"]   .asString();
    Msg.deviceID  = m_JsonValue["DeviceID"] .asString();
    Msg.m_Seat.seat_name = arraySeat["seat_name"].asString();
    Msg.m_Seat.m_deploy.push_back(__deploy);

    return true;
}

void CTeHomeIF::Encode(const CAgentMsg_addDeviceConfigParam &Msg)
{
    Json::Value var;

    var["deviceName"]     	= Msg.deviceName ;
    var["description"]     	= Msg.description ;
    var["room"]      		= Msg.room  ;
    var["permission"]       = Msg.permission     ;
    var["deviceType"]     	= Msg.deviceType ;
    var["subType"]     		= Msg.subType;
	var["gateway"]     		= Msg.gateway ;
    var["zone"]     		= Msg.zone ;
    var["startRow"]         = Msg.startRow     ;
    var["startColumn"]     	= Msg.startColumn ;
    var["rowCount"]     	= Msg.rowCount;
	var["columnCount"]     	= Msg.columnCount ;
    var["deviceId"]     	= Msg.deviceId ;
    var["ctrlType"]      	= Msg.ctrlType  ;
    var["datatype"]         = Msg.datatype     ;
    var["grouptype"]     	= Msg.grouptype ;
    var["value"]     		= Msg.value;
	var["dataPoint"]     	= Msg.dataPoint ;
    var["enoceanId"]     	= Msg.enoceanId ;
    var["choiceType"]      	= Msg.choiceType  ;
    var["funcType"]         = Msg.funcType     ;
    var["appType"]     		= Msg.appType ;
    var["rocker"]     		= Msg.rocker;
	var["autoLearned"]      = Msg.autoLearned     ;
    var["totalTime"]     	= Msg.totalTime ;
    var["totalSteps"]     	= Msg.totalSteps;

    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CAgentMsg_addDeviceConfigParam &Msg)
{
	Msg.deviceName 		= m_JsonValue["deviceName"].asString() ;
    Msg.description 	= m_JsonValue["description"].asString() ;
    Msg.room  			= m_JsonValue["room"] .asString() ;
    Msg.permission     	= m_JsonValue["permission"]    .asString() ;
    Msg.deviceType 		= m_JsonValue["deviceType"].asString() ;
    Msg.subType			= m_JsonValue["subType"].asString() ;
	Msg.gateway 		= m_JsonValue["gateway"].asString() ;
    Msg.zone 			= m_JsonValue["zone"].asString() ;
    Msg.startRow     	= m_JsonValue["startRow"].asString() ;
    Msg.startColumn 	= m_JsonValue["startColumn"].asString() ;
    Msg.rowCount		= m_JsonValue["rowCount"].asString() ;
	Msg.columnCount 	= m_JsonValue["columnCount"].asString() ;
    Msg.deviceId 		= m_JsonValue["deviceId"].asString() ;
    Msg.ctrlType  		= m_JsonValue["ctrlType"].asString() ;
    Msg.datatype     	= m_JsonValue["datatype"].asString() ;
    Msg.grouptype 		= m_JsonValue["grouptype"].asString() ;
    Msg.value			= m_JsonValue["value"].asString() ;
	Msg.dataPoint 		= m_JsonValue["dataPoint"].asString() ;
    Msg.enoceanId 		= m_JsonValue["enoceanId"].asString() ;
    Msg.choiceType  	= m_JsonValue["choiceType"].asString() ;
    Msg.funcType     	= m_JsonValue["funcType"].asString() ;
    Msg.appType 		= m_JsonValue["appType"].asString() ;
    Msg.rocker			= m_JsonValue["rocker"].asString() ;
	Msg.autoLearned     = m_JsonValue["autoLearned"].asString() ;
    Msg.totalTime 		= m_JsonValue["totalTime"].asString() ;
    Msg.totalSteps		= m_JsonValue["totalSteps"].asString() ;

    return true;
}


void CTeHomeIF::Encode(const CAgentMsg_ReportCtrlDeviceState                     &Msg)
{
    Json::Value var;

    var["sessionId"]     = Msg.SessionId ;
    var["productID"]     = Msg.ProductID ;
    var["deviceID"]      = Msg.DeviceID  ;
    var["state"]         = Msg.State     ;
    var["stateTime"]     = Msg.StateTime ;
    var["ip"]     = Msg.strIP;

    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CAgentMsg_ReportCtrlDeviceState &Msg)
{
    Msg.SessionId = m_JsonValue["SessionId"].asString() ;
    Msg.ProductID = m_JsonValue["ProductID"].asString() ;
    Msg.DeviceID  = m_JsonValue["DeviceID"] .asString() ;
    Msg.State     = m_JsonValue["State"]    .asString() ;
    Msg.StateTime = m_JsonValue["StateTime"].asString() ;

    return true;
}

void CTeHomeIF::Encode(const CAgentMsg_reportDeviceStateList                     &Msg)
{
    Json::Value var;
    unsigned int ii;

    var["sessionId"] = Msg.sessionId;
    var["productID"] = Msg.productID;
    var["deviceID"]  = Msg.deviceID ;

    Json::Value operate_all;//store all pieces
    for (ii=0; ii<Msg.m_operate.size(); ii++)
    {
        Json::Value operate;//store all pieces

        operate["operate_id"]     = Msg.m_operate[ii].operate_id      ;
        operate["operate_value"]  = Msg.m_operate[ii].operate_value   ;
        operate["operate_ranage"] = Msg.m_operate[ii].operate_ranage  ;
        operate["operate_type"]   = Msg.m_operate[ii].operate_type    ;
        operate["operate_explain"]= Msg.m_operate[ii].operate_explain ;
        operate["value_time"]     = Msg.m_operate[ii].value_time      ;

        operate_all.append(operate);
    }
    var["operate"] = operate_all;

    m_JsonValue = var;
    setValid();
}
bool CTeHomeIF::Decode(CAgentMsg_reportDeviceStateList &Msg)
{
    Msg.sessionId = m_JsonValue["SessionId"].asString();
    Msg.productID = m_JsonValue["ProductId"].asString();
    Msg.deviceID  = m_JsonValue["DeviceID"] .asString();
    const Json::Value arrayOperate = m_JsonValue["operate"];

    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CAgentMsg_reportDeviceStateList_operate _operate;
        _operate.operate_id       = arrayOperate[i]["operate_id"]     .asString();
        _operate.operate_value    = arrayOperate[i]["operate_value"]  .asString();
        _operate.operate_ranage   = arrayOperate[i]["operate_ranage"] .asString();
        _operate.operate_type     = arrayOperate[i]["operate_type"]   .asString();
        _operate.operate_explain  = arrayOperate[i]["operate_explain"].asString();
        _operate.value_time       = arrayOperate[i]["value_time"]     .asString();

        Msg.m_operate.push_back(_operate);
    }
    return true;
}

void CTeHomeIF::Encode(const CAgentMsg_reportAlarm                               &Msg)
{
    Json::Value var;
    unsigned int ii=0;

    var["deviceID"]  = Msg.deviceID;
    var["sessionId"] = Msg.sessionId;

    //    Json::Value operate_all;//store all pieces
    //    for (ii=0; ii<Msg.m_operate.size(); ii++)
    //    {
    Json::Value operate;//store all pieces

    operate["operate_id"]     = Msg.m_operate[ii].operate_id       ;
    operate["operate_value"]  = Msg.m_operate[ii].operate_value    ;
    operate["operate_ranage"] = Msg.m_operate[ii].operate_ranage   ;
    operate["operate_explain"]= Msg.m_operate[ii].operate_explain  ;
    operate["value_time"]     = Msg.m_operate[ii].value_time       ;

    //        operate_all.append(operate);
    //    }
    //    var["operate"] = operate_all;
    var["operate"] = operate;

    m_JsonValue = var;
    setValid();
}

bool CTeHomeIF::Decode(CAgentMsg_reportAlarm &Msg)
{
    Msg.sessionId = m_JsonValue["SessionId"].asString();
    Msg.deviceID  = m_JsonValue["DeviceID"] .asString();

    const Json::Value arrayOperate = m_JsonValue["operate"];

    //for (unsigned int i = 0; i < arrayOperate.size(); i++)
    //{
    CAgentMsg_reportDeviceStateList_operate _operate;
    _operate.operate_id       = arrayOperate["operate_id"]     .asString();
    _operate.operate_value    = arrayOperate["operate_value"]  .asString();
    _operate.operate_ranage   = arrayOperate["operate_ranage"] .asString();
    _operate.operate_type     = arrayOperate["operate_type"]   .asString();
    _operate.operate_explain  = arrayOperate["operate_explain"].asString();
    _operate.value_time       = arrayOperate["value_time"]     .asString();

    Msg.m_operate.push_back(_operate);
    //}
    return true;
}

void CTeHomeIF::Encode(const CAgentMsg_reportGatewayState                        &Msg)
{
    Json::Value var;

    var["SessionId"] = Msg.SessionId ;
    var["ProductID"] = Msg.ProductID ;
    var["DeviceID"]  = Msg.DeviceID  ;
    var["State"]     = Msg.State     ;
    var["StateTime"] = Msg.StateTime ;

    m_JsonValue = var;

    setValid();
}
bool CTeHomeIF::Decode(CAgentMsg_reportGatewayState &Msg)
{
    Msg.SessionId = m_JsonValue["SessionId"].asString();
    Msg.ProductID = m_JsonValue["ProductID"].asString();
    Msg.DeviceID  = m_JsonValue["DeviceID"] .asString();
    Msg.State     = m_JsonValue["State"]    .asString();
    Msg.StateTime = m_JsonValue["StateTime"].asString();

    return true;
}

void CTeHomeIF::Encode(const CAgentMsg_reportDeviceAlarm                         &Msg)
{
    Json::Value var;

    var["SessionId"] 	 = Msg.SessionId  ;
    var["ProductID"] 	 = Msg.ProductID  ;
    var["DeviceID"]      = Msg.DeviceID   ;
    var["Operate_id"]    = Msg.Operate_id ;
    var["Explain"] 	     = Msg.Explain 	  ;
    var["StateTime"] 	 = Msg.StateTime  ;

    m_JsonValue = var;

    setValid();
}
bool CTeHomeIF::Decode(CAgentMsg_reportDeviceAlarm &Msg)
{
    Msg.SessionId  = m_JsonValue["SessionId"]  .asString();
    Msg.ProductID  = m_JsonValue["ProductID"]  .asString();
    Msg.DeviceID   = m_JsonValue["DeviceID"]   .asString();
    Msg.Operate_id = m_JsonValue["Operate_id"] .asString();
    Msg.Explain    = m_JsonValue["Explain"]    .asString();
    Msg.StateTime  = m_JsonValue["StateTime"]  .asString();

    return true;
}
void CTeHomeIF::Encode(const CAgentMsg_requestDeviceConfig                       &Msg)
{
    Json::Value var;

    var["SessionId"] = Msg.SessionId;
    var["ProductID"] = Msg.ProductID;
    m_JsonValue = var;

    setValid();
}
bool CTeHomeIF::Decode(CAgentMsg_requestDeviceConfig &Msg)
{
    Msg.SessionId  = m_JsonValue["SessionId"]  .asString();
    Msg.ProductID  = m_JsonValue["ProductID"]  .asString();

    return true;
}
void CTeHomeIF::Encode(const CeHomeMsg_requestReportDeviceConfigParam            &Msg)
{
    Json::Value var;

    var["UserId"]   = Msg.UserId  ;
    var["DeviceId"] = Msg.DeviceId;
    m_JsonValue = var;

    setValid();
}
bool CTeHomeIF::Decode(CeHomeMsg_requestReportDeviceConfigParam &Msg)
{
    Msg.UserId    = m_JsonValue["UserId"]    .asString();
    Msg.DeviceId  = m_JsonValue["deviceId"]  .asString();

    return true;
}

void CTeHomeIF::Encode(const CAgentMsg_requestDeviceConfig_ACK                   &Msg)
{
    Json::Value var;
    unsigned int ii;

    var["result"]   = Msg.Result  ;
    var["reason"]   = Msg.Reason  ;
    var["pointnum"] = Msg.Pointnum;

    Json::Value operate_all;//store all pieces
    for (ii=0; ii<Msg.m_config.size(); ii++)
    {
        Json::Value operate;//store all pieces

        operate["Room"]       = Msg.m_config[ii].Room       ;
        operate["DeviceType"] = Msg.m_config[ii].DeviceType ;
        operate["number"]     = Msg.m_config[ii].number     ;

        operate_all.append(operate);
    }
    var["config"] = operate_all;

    m_JsonValue = var;
    setValid();
}
bool CTeHomeIF::Decode(CAgentMsg_requestDeviceConfig_ACK &Msg)
{
    Msg.Result = m_JsonValue["result"].asString();
    Msg.Reason = m_JsonValue["reason"].asString();
    Msg.Pointnum  = m_JsonValue["pointnum"] .asString();
    const Json::Value arrayOperate = m_JsonValue["config"];

    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CAgentMsg_requestDeviceConfig_ACK_Config _config;
        _config.Room        = arrayOperate[i]["Room"]     .asString();
        _config.DeviceType  = arrayOperate[i]["DeviceType"]  .asString();
        _config.number      = arrayOperate[i]["number"] .asString();

        Msg.m_config.push_back(_config);
    }
    return true;
}
void CTeHomeIF::Encode(const CeHomeMsg_requestReportDeviceConfigParam_ACK        &Msg)
{
    setValid(false);

    // 根据协议文档来看，如此， 否则用Encode_FULL来编码
    if (Msg.m_Seat.m_deploy.size()!=1)
    {
        return;
    }

    if (Msg.m_Seat.m_deploy[0].m_Operate.size()!=1)
    {
        return;
    }

    Json::Value var;
    unsigned int ii;
    unsigned int jj;


    Json::Value seat;//store all pieces
    seat["seat_name"] = Msg.m_Seat.seat_name;

    //Json::Value deploy_all;//store all pieces
    for (ii=0; ii<Msg.m_Seat.m_deploy.size(); ii++)
    {
        Json::Value deploy;//store all pieces

        deploy["deploy_name"] = Msg.m_Seat.m_deploy[ii].deploy_name ;
        deploy["deploy_type"] = Msg.m_Seat.m_deploy[ii].deploy_type ;
        deploy["deviceType"]  = Msg.m_Seat.m_deploy[ii].deviceType ;
        deploy["device_model"]= Msg.m_Seat.m_deploy[ii].device_model;

        //Json::Value operate;//store all pieces
        for (jj=0;jj < Msg.m_Seat.m_deploy[ii].m_Operate.size(); jj++)
        {
            Json::Value pieces;//here it store just one piece

            pieces["operate_id"      ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_id     ;
            pieces["operate_ranage"  ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_ranage ;
            pieces["operate_type"    ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_type   ;
            pieces["operate_value"   ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_value  ;
            pieces["operate_explain" ] = Msg.m_Seat.m_deploy[ii].m_Operate[jj].operate_explain;
            //operate.append(pieces);
            deploy["operate"] = pieces;//ok,yes we just have apply One piece ,then push back to the array

        }


        //deploy_all.append(deploy);
        seat["deploy"] = deploy;
    }

    var["Result"]    = Msg.Result;
    var["Reason"]    = Msg.Reason;
    var["ProductId"] = Msg.productID;
    var["UserID"]    = Msg.userID;
    var["DeviceID"]  = Msg.deviceID;
    var["seat"] = seat;

    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CeHomeMsg_requestReportDeviceConfigParam_ACK &Msg)
{
    //CAgentMsg_initDeviceConfigParam Msg;
    //CAgentMsg_initDeviceConfigParam_Seat __seat;
    CAgentMsg_initDeviceConfigParam_Seat_deploy __deploy;
    CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate __operate;

    const Json::Value arraySeat = m_JsonValue["seat"];
    const Json::Value arrayDeploy = arraySeat["deploy"];
    const Json::Value arrayOperate = arrayDeploy["operate"];

    __operate.operate_id      = arrayOperate["operate_id"].asString();
    __operate.operate_ranage  = arrayOperate["operate_ranage"].asString();
    __operate.operate_type    = arrayOperate["operate_type"].asString();
    __operate.operate_value   = arrayOperate["operate_value"].asString();
    __operate.operate_explain = arrayOperate["operate_explain"].asString();

    __deploy.deploy_name = arrayDeploy["deploy_name"] .asString();
    __deploy.deploy_type = arrayDeploy["deploy_type"] .asString();
    __deploy.deviceType  = arrayDeploy["deviceType"]  .asString();
    __deploy.device_model= arrayDeploy["device_model"].asString();
    __deploy.m_Operate.push_back(__operate);

    Msg.Result    = m_JsonValue["Result"]   .asString();
    Msg.Reason    = m_JsonValue["Reason"]   .asString();
    Msg.productID = m_JsonValue["ProductId"].asString();
    Msg.userID    = m_JsonValue["userID"]   .asString();
    Msg.deviceID  = m_JsonValue["deviceId"] .asString();

    Msg.m_Seat.seat_name = arraySeat["seat_name"].asString();
    Msg.m_Seat.m_deploy.push_back(__deploy);

    return true;
}

void CTeHomeIF::Encode(const CeHomeMsg_getCtrlDeviceState                        &Msg)
{
    Json::Value var;

    var["DeviceID"] = Msg.DeviceID;
    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CeHomeMsg_getCtrlDeviceState &Msg)
{
    // 2012-10-1之前的版本
    //Msg.DeviceID  = m_JsonValue["deviceId"]  .asString();
    // after 2012-10-1
    Msg.DeviceID  = m_JsonValue["deviceID"]  .asString();

    return true;
}

void CTeHomeIF::Encode(const CeHomeMsg_getCtrlDeviceState_ACK                    &Msg)
{
    Json::Value var;

    var["result"] = Msg.Result;
    var["reason"] = Msg.Reason;
    var["state"]  = Msg.State;

    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CeHomeMsg_getCtrlDeviceState_ACK &Msg)
{
    Msg.Result =  m_JsonValue["result"].asString();
    Msg.Reason =  m_JsonValue["reason"].asString();
    Msg.State  =  m_JsonValue["State"] .asString();

    return true;
}
	void CTeHomeIF::Encode(const CeHomeMsg_register                        &Msg)
{
    Json::Value var;

    var["deviceId"] = Msg.DeviceID;
    var["userId"] = Msg.UserID;
    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CeHomeMsg_register &Msg)
{
    Msg.DeviceID  = m_JsonValue["deviceId"].asString();
    Msg.UserID= m_JsonValue["userId"].asString();

    return true;
}

void CTeHomeIF::Encode(const CeHomeMsg_register_ACK                    &Msg)
{
    Json::Value var;

    var["result"] = Msg.Result;
    var["reason"] = Msg.Reason;

    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CeHomeMsg_register_ACK &Msg)
{
    Msg.Result =  m_JsonValue["result"].asString();
    Msg.Reason =  m_JsonValue["reason"].asString();

    return true;
}

void CTeHomeIF::Encode(const CeHomeMsg_getDeviceStateList                        &Msg)
{
    Json::Value var;

    var["DeviceID"] = Msg.DeviceID;
    m_JsonValue = var;

    setValid();
}

bool CTeHomeIF::Decode(CeHomeMsg_getDeviceStateList &Msg)
{
    Msg.DeviceID =  m_JsonValue["DeviceID"].asString();

    return true;
}

void CTeHomeIF::Encode(const CeHomeMsg_getDeviceStateList_ACK                        &Msg)
{
    Json::Value var;
    unsigned int ii;

    var["result"] = Msg.result;
    var["reason"] = Msg.reason;

    Json::Value operate_all;//store all pieces
    for (ii=0; ii<Msg.m_operate.size(); ii++)
    {
        Json::Value operate;//store all pieces

        operate["operate_id"]     = Msg.m_operate[ii].operate_id       ;
        operate["operate_value"]  = Msg.m_operate[ii].operate_value    ;
        operate["operate_ranage"] = Msg.m_operate[ii].operate_ranage   ;
        operate["operate_type"]   = Msg.m_operate[ii].operate_type     ;
        operate["operate_explain"]= Msg.m_operate[ii].operate_explain  ;
        operate["value_time"]     = Msg.m_operate[ii].value_time       ;

        operate_all.append(operate);
    }
    var["operate"] = operate_all;

    m_JsonValue = var;
    setValid();
}

bool CTeHomeIF::Decode(CeHomeMsg_getDeviceStateList_ACK &Msg)
{
    Msg.result = m_JsonValue["result"].asString();
    Msg.reason = m_JsonValue["reason"].asString();
    const Json::Value arrayOperate = m_JsonValue["operate"];

    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CeHomeMsg_getDeviceStateList_ACK_operate _operate;
        _operate.operate_id       = arrayOperate[i]["operate_id"]     .asString();
        _operate.operate_value    = arrayOperate[i]["operate_value"]  .asString();
        _operate.operate_ranage   = arrayOperate[i]["operate_ranage"] .asString();
        _operate.operate_type     = arrayOperate[i]["operate_type"]   .asString();
        _operate.operate_explain  = arrayOperate[i]["operate_explain"].asString();
        _operate.value_time       = arrayOperate[i]["value_time"]     .asString();

        Msg.m_operate.push_back(_operate);
    }
    return true;
}
// 协议文档里面发一个operate， 相应有多个？ how？
void CTeHomeIF::Encode(const CeHomeMsg_controlDevice                             &Msg)
{
    Json::Value var;
    unsigned int ii=0;

    var["deviceID"] = Msg.DeviceID;

    //    Json::Value operate_all;//store all pieces
    //    for (ii=0; ii<Msg.m_operate.size(); ii++)
    //    {
    Json::Value operate;//store all pieces

    operate["operate_id"]     = Msg.m_operate[ii].operate_id    ;
    operate["operate_ranage"] = Msg.m_operate[ii].operate_ranage;
    operate["operate_type"]   = Msg.m_operate[ii].operate_type  ;
    operate["orderId"]        = Msg.m_operate[ii].orderId       ;
    operate["extendpara"]     = Msg.m_operate[ii].extendpara    ;

    //        operate_all.append(operate);
    //    }
    var["operate"] = operate;

    m_JsonValue = var;
    setValid();
}

bool CTeHomeIF::Decode(CeHomeMsg_controlDevice &Msg)
{
    Msg.DeviceID  = m_JsonValue["deviceID"] .asString();

    const Json::Value arrayOperate = m_JsonValue["operate"];

    //for (unsigned int i = 0; i < arrayOperate.size(); i++)
    //{
    CeHomeMsg_controlDevice_operate _operate;
    _operate.operate_id       = arrayOperate["operate_id"]     .asString();
    _operate.operate_ranage   = arrayOperate["operate_ranage"] .asString();
    _operate.operate_type     = arrayOperate["operate_type"]   .asString();
    _operate.orderId          = arrayOperate["orderId"]        .asString();
    _operate.extendpara       = arrayOperate["extendpara"]     .asString();

    Msg.m_operate.push_back(_operate);
    //}
    return true;
}


void CTeHomeIF::Encode(const CeHomeMsg_controlDevice_ACK                         &Msg)
{
    Json::Value var;
    unsigned int ii=0;

    var["result"]   =        Msg.Result  ;
    var["reason"]   =        Msg.Reason  ;
    var["deviceID"] =        Msg.deviceID;

//    Json::Value operate_all;//store all pieces
//    for (ii=0; ii<Msg.m_operate.size(); ii++)
//    {
        Json::Value operate;//store all pieces

        operate["operate_id"]   = Msg.m_operate[ii].operate_id  ;
        operate["resultState"]  = Msg.m_operate[ii].resultState ;
        operate["stateTime"]    = Msg.m_operate[ii].stateTime   ;

//        operate_all.append(operate);
//    }
    var["operate"] = operate;//operate_all;

    m_JsonValue = var;
    setValid();
}

bool CTeHomeIF::Decode(CeHomeMsg_controlDevice_ACK &Msg)
{
    Msg.Result = m_JsonValue["Result"].asString();
    Msg.Reason = m_JsonValue["Reason"].asString();
    Msg.deviceID = m_JsonValue["deviceID"].asString();
    const Json::Value arrayOperate = m_JsonValue["operate"];

    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CeHomeMsg_controlDevice_ACK_Operate _operate;
        _operate.operate_id       = arrayOperate[i]["operate_id"]     .asString();
        _operate.resultState  = arrayOperate[i]["resultState"].asString();
        _operate.stateTime       = arrayOperate[i]["stateTime"]     .asString();

        Msg.m_operate.push_back(_operate);
    }
    return true;
}

bool CTeHomeIF::Decode(std::vector<CTSHomeHostHttpMsg_Operate> &Msg)
{
    if ( m_JsonValue.type() == Json::arrayValue)
    {
        int size = m_JsonValue.size();
        //数组类型，使用数字下标作为索引遍历所有元素
        for ( int index =0; index < size; ++index )
        {
            CTSHomeHostHttpMsg_Operate __op;
            const Json::Value arrayOperate = m_JsonValue[index];

            __op.deviceid = arrayOperate["deviceid"].asString();
            __op.operate  = arrayOperate["operate"] .asString(); //
            __op.value    = arrayOperate["value"]   .asString();
            __op.devicetype    = arrayOperate["devicetype"]   .asString();

            __op.m_ideviceid = atoll(__op.deviceid.c_str());
            __op.m_ioperate 	= atol(__op.operate.c_str());
            __op.m_ivalue 	= atol(__op.value.c_str());
            __op.m_idevicetype 	= atol(__op.devicetype.c_str());

            Msg.push_back(__op);
        }
    }
    return true;
}
