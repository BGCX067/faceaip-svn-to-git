/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeHomeIF.h - _explain_
 *
 * Copyright (C) 2012 tiansu-china.com, All Rights Reserved.
 *
 * $Id: TeHomeIF.h 0001 2012-5-8 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     TeHomeIF -- Telecom eHome Internet Protocol
 *
 *  Update:
 *     2012-05-08 14:38:33 WuJunjie 549 Create
 *     2012-05-25 10:21:25 WuJunjie 549 all decode interface ready
 * 
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _TEHOMEIF_H
#define _TEHOMEIF_H

#include <json.h>
#include <algorithm>
#include <stdio.h>

#include "TeHomeMsg.h"

class CTeHomeIF
{
public :
    CTeHomeIF();
    virtual ~CTeHomeIF();

    virtual bool Parse( const std::string &document);
    bool ToString( std::string &document);
    const std::string &ToString();

    virtual const int getError() const;
    virtual const std::string &getErrorMessages() const;

    virtual void Encode(); //example only

    virtual void Encode(const CAckMsgSimple                                &Msg);
    virtual void Encode(const CAckMsgSession                               &Msg);
    virtual void Encode(const CAgentMsg_requestAuth                        &Msg);
    virtual void Encode(const CAgentMsg_certificationActivation            &Msg);
	
    virtual void Encode_FULL(const CAgentMsg_initDeviceConfigParam         &Msg);
    virtual void Encode(const CAgentMsg_initDeviceConfigParam              &Msg);
	
    virtual void Encode(const CAgentMsg_addDeviceConfigParam              &Msg);
	
    virtual void Encode(const CAgentMsg_ReportCtrlDeviceState              &Msg);
    virtual void Encode(const CAgentMsg_reportDeviceStateList              &Msg);
    virtual void Encode(const CAgentMsg_reportAlarm                        &Msg);
    virtual void Encode(const CAgentMsg_reportGatewayState                 &Msg);
    virtual void Encode(const CAgentMsg_reportDeviceAlarm                  &Msg);
    virtual void Encode(const CAgentMsg_requestDeviceConfig                &Msg);
    virtual void Encode(const CAgentMsg_requestDeviceConfig_ACK            &Msg);
    virtual void Encode(const CeHomeMsg_requestReportDeviceConfigParam     &Msg);
    virtual void Encode(const CeHomeMsg_requestReportDeviceConfigParam_ACK &Msg);
    virtual void Encode(const CeHomeMsg_getCtrlDeviceState                 &Msg);
    virtual void Encode(const CeHomeMsg_getCtrlDeviceState_ACK             &Msg);
    virtual void Encode(const CeHomeMsg_register &Msg);
    virtual void Encode(const CeHomeMsg_register_ACK             &Msg);
    virtual void Encode(const CeHomeMsg_getDeviceStateList                 &Msg);
    virtual void Encode(const CeHomeMsg_getDeviceStateList_ACK             &Msg);
    virtual void Encode(const CeHomeMsg_controlDevice                      &Msg);
    virtual void Encode(const CeHomeMsg_controlDevice_ACK                  &Msg);

    // 下列Msg， 调用者自行保证初始化， 接口内部不初始化（vector）
    bool Decode(CAckMsgSimple                                &Msg);
    bool Decode(CAckMsgSession                               &Msg);
    bool Decode(CAgentMsg_requestAuth                        &Msg);
    bool Decode(CAgentMsg_certificationActivation            &Msg);
    bool Decode(CAgentMsg_initDeviceConfigParam              &Msg);
	bool Decode(CAgentMsg_addDeviceConfigParam               &Msg);
    bool Decode(CAgentMsg_ReportCtrlDeviceState              &Msg);
    bool Decode(CAgentMsg_reportDeviceStateList              &Msg);
    bool Decode(CAgentMsg_reportAlarm                        &Msg);
    bool Decode(CAgentMsg_reportGatewayState                 &Msg);
    bool Decode(CAgentMsg_reportDeviceAlarm                  &Msg);
    bool Decode(CAgentMsg_requestDeviceConfig                &Msg);
    bool Decode(CAgentMsg_requestDeviceConfig_ACK            &Msg);
    bool Decode(CeHomeMsg_requestReportDeviceConfigParam     &Msg);
    bool Decode(CeHomeMsg_requestReportDeviceConfigParam_ACK &Msg);
    bool Decode(CeHomeMsg_getCtrlDeviceState                 &Msg);
    bool Decode(CeHomeMsg_getCtrlDeviceState_ACK             &Msg);
    bool Decode(CeHomeMsg_register &Msg);
    bool Decode(CeHomeMsg_register_ACK             &Msg);
    bool Decode(CeHomeMsg_getDeviceStateList                 &Msg);
    bool Decode(CeHomeMsg_getDeviceStateList_ACK             &Msg);
    bool Decode(CeHomeMsg_controlDevice                      &Msg);
    bool Decode(CeHomeMsg_controlDevice_ACK                  &Msg);

    bool Decode(std::vector<CTSHomeHostHttpMsg_Operate>      &Msg);

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
#endif /* _TEHOMEIF_H */
