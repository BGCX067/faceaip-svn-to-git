/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeHomeMsg.h - _explain_
 *
 * Copyright (C) 2012 tiansu-china.com, All Rights Reserved.
 *
 * $Id: TeHomeMsg.h 0001 2012-5-8 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     TeHomeMsg -- Telecom eHome Msg 2.0
 *
 *  Update:
 *     2012-5-8 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _TEHOMEMSG_H
#define _TEHOMEMSG_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

//
// 简单返回消息
//
class CAckMsgSimple
{
public:
    std::string result;
    std::string reason;

    CAckMsgSimple()
    {
        result = "";
        reason = "";
    }

    void dump()
    {
        std::cout << "result:" << result << std::endl;
        std::cout << "reason:" << reason << std::endl;
    }
};

//
// 返回带有session的消息
//
class CAckMsgSession : public CAckMsgSimple
{
public:
    std::string SessionId;
    CAckMsgSession()
    {
        SessionId = "";
    }
    void dump()
    {
        std::cout << "SessionId:" << result << std::endl;
        CAckMsgSimple::dump();
    }
};

//
// 厂商引擎认证接口
//
class CAgentMsg_requestAuth
{
public:
    // 需配置
    std::string ProductID 		;//String 厂家编号，由智慧E 家平台给厂商分配
    // 需配置
    std::string ProductName 	;//String 厂家名，如tiansu
    // 需配置
    std::string AuthUser 		;//string 授权用户名
    // 需配置
    std::string AuthPassword 	;//string 授权用户密码

    CAgentMsg_requestAuth()
    {
        ProductID 	 = "";
        ProductName  = "";
        AuthUser 	 = "";
        AuthPassword = "";
    }
    void dump()
    {
        std::cout << "ProductID   :" << ProductID  << std::endl;
        std::cout << "ProductName :" << ProductName  << std::endl;
        std::cout << "AuthUser    :" << AuthUser   << std::endl;
        std::cout << "AuthPassword:" << AuthPassword << std::endl;
    }
};


//
// 客户智能设备网关激活认证接口
//
class CAgentMsg_certificationActivation
{
public:

    // 获取而来， 不需配置
    std::string SessionId 	;//string
    // 需配置
    std::string UserId 		;//String 用户编号      CAgentMsg_certificationActivation()
    // 需配置
    std::string DeviceId 	;//String 设备编号    {

    CAgentMsg_certificationActivation()
    {
        SessionId 	 = "";
        UserId       = "";
        DeviceId 	 = "";
    }
    void dump()
    {
        cout << "SessionId:"<< SessionId<< endl;
        cout << "UserId   :"<< UserId   << endl;
        cout << "DeviceId :"<< DeviceId << endl;
    }
};

////////////////////////////////
//
// 配置上报接口
//
class CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate
{
public:
    // 需要从模块 获取
    std::string operate_id 		;//String 设备操作序号，与设备的开关号匹配；
    // 需要从模块 获取
    std::string operate_ranage 	;//String 设备能控制最小值、最大值；例	如：1-33
    // 需要从模块 获取
    std::string operate_type 	;//String 设备操作类型，0: 开关标志；1：
    //幅度标志：范围为0-999； 2：
    //固定值方式：如温度为16,26 等；
    // 需要从模块 获取
    std::string operate_value 	;//String 设备操作类型的结果值， 与
    //operate-type 结合使用。当
    //operate-type=0 时，
    //operate-value=1 表示开，
    //operate-value=2 表示关
    //当operate-type=1 时，
    //operate-value 表示具体的范围
    //之间的值，如operate-value=325
    //当operate-type=2 时，
    //operate-value 表示具体的值，
    //如operate-value=16
    // 需要从模块 获取
    std::string operate_explain ;//string 操作说明内容

    CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate()
    {
        operate_id 		= "";//String 设备操作序号，与设备的开关号匹配；
        operate_ranage 	= "";//String 设备能控制最小值、最大值；例	如：1-33
        operate_type 	= "";//String 设备操作类型，0: 开关标志；1：
        operate_value 	= "";//String 设备操作类型的结果值， 与
        operate_explain = "";//string 操作说明内容
    }
    void dump()
    {
        cout << "operate_id     :" << operate_id     << endl;
        cout << "operate_ranage :" << operate_ranage << endl;
        cout << "operate_type   :" << operate_type   << endl;
        cout << "operate_value  :" << operate_value  << endl;
        cout << "operate_explain:" << operate_explain<< endl;
    }
};

//
// 配置上报接口
//
class CAgentMsg_initDeviceConfigParam_Seat_deploy
{
public:
    // 需要从模块 获取
    std::string deploy_name 	;//String 名称
    // 需要从模块 获取
    std::string deploy_type 	;//String 设备的类型，1：安防设备；2：家居设备；
    // 需要从模块 获取
    std::string deviceType 		;//String 传递参数代码
    //1：照明
    //2：空调
    //3：电视
    //4：音响
    //5：窗帘
    //6：排气
    //7：喷水
    //0：其它
    // 需要从模块 获取
    std::string device_model	;//String 设备型号

    // 需要从模块 获取， CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate 定义
    std::vector<CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate> m_Operate;
    CAgentMsg_initDeviceConfigParam_Seat_deploy()
    {
        deploy_name 	= "";//String 名称
        deploy_type 	= "";//String 设备的类型，1：安防设备；2：家居设备；
        deviceType 		= "";//String 传递参数代码
        device_model	= "";//String 设备型号
    }
    void dump()
    {
        cout << "deploy_name :" <<  deploy_name  << endl;
        cout << "deploy_type :" <<  deploy_type  << endl;
        cout << "deviceType  :" <<  deviceType   << endl;
        cout << "device_model:" <<  device_model << endl;
        for (vector<CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate>::iterator iter=m_Operate.begin()
                ; iter!=m_Operate.end()
             ; ++iter)
        {
            iter->dump();
        }
    }
};

//
// 配置上报接口
//
class CAgentMsg_initDeviceConfigParam_Seat
{
public:
    // 需要从模块 获取
    std::string seat_name 		;//String 布局大环境的名称，如客厅
    // 需要从模块 获取， 具体内容见CAgentMsg_initDeviceConfigParam_Seat_deploy 定义
    // 2012-9-5 17:22:35 根据同电信的沟通结果， 只能一个个上传，这里目前只能为 1
    std::vector<CAgentMsg_initDeviceConfigParam_Seat_deploy> m_deploy;
    CAgentMsg_initDeviceConfigParam_Seat()
    {
        seat_name = "";
    }
    void dump()
    {
        cout << "seat_name:" <<  seat_name << endl;
        for (vector<CAgentMsg_initDeviceConfigParam_Seat_deploy>::iterator iter=m_deploy.begin()
                ; iter!=m_deploy.end()
             ; ++iter)
        {
            iter->dump();
        }
    }
}
;
////////////////////////////////
//
// 配置上报接口
//
class CAgentMsg_initDeviceConfigParam
{
public:
    std::string requestMark 	;//String 请求标示，可用值为：
    //add、update、delete
    //(add、update、delete 触发动作
    //以deviceID 与operate_id 为判断条件)

    // 已定义
    std::string sessionId 		;//String
    // 已定义
    std::string productID 		;//String 厂家编号ID
    // 已定义
    std::string userID 			;//String 用户编号
    // 已定义
    std::string deviceID 		;//string 设备编号

    // 需要从模块 获取， 具体见 CAgentMsg_initDeviceConfigParam_Seat 中的定义
    CAgentMsg_initDeviceConfigParam_Seat m_Seat;

    CAgentMsg_initDeviceConfigParam()
    {
        requestMark 	= "";//String
        sessionId 		= "";//String
        productID 		= "";//String 厂家编号ID
        userID 			= "";//String 用户编号
        deviceID 		= "";//string 设备编号
    }
    void dump()
    {
            cout << "requestMark :" <<  requestMark  << endl;
            cout << "sessionId :" <<  sessionId  << endl;
            cout << "productID :" <<  productID  << endl;
            cout << "userID :" <<  userID  << endl;
            cout << "deviceID :" <<  deviceID  << endl;

        m_Seat.dump();
    }
}
;

//
// 添加设备接口
//
class CAgentMsg_addDeviceConfigParam
{
public:
 	//deviceName, description, room, permission, deviceType, subType, gateway, zone, startRow, startColumn, rowCount, columnCount
 	//device, ctrlType, datatype, grouptype, t_data_point.value
 	//dataPoint, enoceanId, choiceType, funcType, appType, rocker, autoLearned, totalTime, totalSteps
    std::string deviceName 	;

    std::string description 	;//String
    
    std::string room 		;//room ID
    
    std::string permission 		;//permission value (int)
    
    std::string deviceType 		;//string 设备编号
    
    std::string subType 	;
    
    std::string gateway 		;//room ID
    
    std::string zone 		;//permission value (int)
    
    std::string startRow 		;//string 设备编号

	std::string startColumn 	;

    std::string rowCount 	;//String
    
    std::string columnCount 		;//room ID
    
    std::string deviceId 		;
    
    std::string ctrlType 		;

	std::string datatype 	;

    std::string grouptype 	;//String
    
    std::string value 		;//room ID
    
    std::string dataPoint 		;//permission value (int)
    
    std::string enoceanId 		;//string 设备编号
    
    std::string choiceType 	;

    std::string funcType 	;//String
    
    std::string appType 		;//room ID
    
    std::string rocker 		;//permission value (int)
    
    std::string autoLearned 		;//string 设备编号

	std::string totalTime 	;

    std::string totalSteps 	;


    CAgentMsg_addDeviceConfigParam()
    {
        
    }
    void dump()
    {
    	cout << endl;
        cout << "deviceName:" << deviceName << endl;

	    cout << " description:" << description << endl 	;//String
	    
	    cout << " room:" << room << endl 		;//room ID
	    
	    cout << " permission:" << permission << endl 		;//permission value (int)
	    
	    cout << " deviceType:" << deviceType << endl 		;//string 设备编号
	    
	    cout << " subType:" << subType << endl 	;
	    
	    cout << " gateway:" << gateway << endl 		;//room ID
	    
	    cout << " zone :" << zone << endl		;//permission value (int)
	    
	    cout << " startRow :" << startRow << endl		;//string 设备编号

		cout << " startColumn:" << startColumn << endl 	;

	    cout << " rowCount:" << rowCount << endl 	;//String
	    
	    cout << " columnCount:" << columnCount << endl 		;//room ID
	    
	    cout << " deviceId:" << deviceId << endl 		;
	    
	    cout << " ctrlType:" << ctrlType << endl 		;

		cout << " datatype:" << datatype << endl 	;

	    cout << " grouptype:" << grouptype << endl 	;//String
	    
	    cout << " value:" << value << endl 		;//room ID
	    
	    cout << " dataPoint:" << dataPoint << endl 		;//permission value (int)
	    
	    cout << " enoceanId:" << enoceanId << endl 		;//string 设备编号
	    
	    cout << " choiceType:" << choiceType << endl 	;

	    cout << " funcType:" << funcType << endl 	;//String
	    
	    cout << " appType:" << appType << endl 		;//room ID
	    
	    cout << " rocker:" << rocker << endl 		;//permission value (int)
	    
	    cout << " autoLearned:" << autoLearned << endl 		;//string 设备编号

		cout << " totalTime :" << totalTime << endl	;

	    cout << " totalSteps:" << totalSteps << endl 	;
    }
}
;


//
// 上报网关状态接口
//
class CAgentMsg_ReportCtrlDeviceState
{
public:
    std::string SessionId 	;//String
    std::string ProductID 	;//String 厂商编号(ID)
    std::string DeviceID 	;//string 设备编号
    std::string State 		;//string 在线/不在线
    std::string StateTime 	;//string 状态时间，格式为YYYY-MM-DD
    std::string strIP 	;//string 状态时间，格式为YYYY-MM-DD
    //		HH:NN:SS（年-月-日时：分：秒）

    CAgentMsg_ReportCtrlDeviceState()
    {
        SessionId 	= "";
        ProductID 	= "";
        DeviceID 	= "";
        State       = "";
        StateTime 	= "";

        strIP = "";
    }
    void dump()
    {
        cout << endl;
        cout << "SessionId:" << SessionId << endl;
        cout << "ProductID:" << ProductID << endl;
        cout << "DeviceID :" << DeviceID  << endl;
        cout << "State    :" << State     << endl;
        cout << "StateTime:" << StateTime << endl;
        cout << "strIP:" << strIP << endl;
    }
}
;

//
//
//
class CAgentMsg_reportDeviceStateList_operate
{
public:
    std::string operate_id 		;//string 设备操作序号，与设备的开关号
    //			相匹配
    std::string operate_value 	;//string operate-value：设备操作类型的
    //		结果值，与operate-type 结合使
    //		用。当operate-type=0 时，
    //		operate-value=1 表示开，
    //		operate-value=2 表示关
    //		当operate-type=1 时，
    //		operate-value 表示具体的范围
    //	之间的值，如operate-value=325
    //	当operate-type=2 时，
    //	operate-value 表示具体的值，
    //	如operate-value=16
    std::string operate_ranage 	;//String 设备能控制最小值、最大值；例
    //			如：1-33
    std::string operate_type 	;//string operate-type：设备操作类型，0:
    //	开关标志；1：幅度标志：范围为
    //	0-999； 2：固定值方式：如温度
    //	为16,26 等；
    std::string operate_explain ;//string 说明
    std::string value_time 		;//string 状态时间，格式为YYYY-MM-DD
    //	HH:NN:SS（年-月-日时：分：秒）

    CAgentMsg_reportDeviceStateList_operate()
    {
        operate_id      = "";//string 设备操作序号，与设备的开关号
        operate_value   = "";//string operate-value：设备操作类型的
        operate_ranage  = "";//String 设备能控制最小值、最大值；例
        operate_type    = "";//string operate-type：设备操作类型，0:
        operate_explain = "";//string 说明
        value_time      = "";//string 状态时间，格式为YYYY-MM-DD
    }
    void dump()
    {}
}
;

//
// 上报设备状态接口
//
class CAgentMsg_reportDeviceStateList
{
public:
    std::string sessionId;//String
    std::string productID;//string 厂家ID
    std::string deviceID ;//string 设备编号
    //operate 操作设备组
    std::vector<CAgentMsg_reportDeviceStateList_operate> m_operate;

    CAgentMsg_reportDeviceStateList()
    {

        sessionId 	= "";
        productID 	= "";
        deviceID 	= "";
    }
    void dump()
    {
        cout << "sessionId:" << sessionId << endl;
        cout << "productID:" << productID << endl;
        cout << "deviceID :" << deviceID  << endl;
        cout << "m_operate.size():" << m_operate.size() << endl;
    }
}
;

//
// 告警上报接口
//
class CAgentMsg_reportAlarm
{
public:
    std::string sessionId;//String
    std::string deviceID ;//string 设备编号
    //operate 操作设备组
    std::vector<CAgentMsg_reportDeviceStateList_operate> m_operate;

    CAgentMsg_reportAlarm()
    {

        sessionId 	= "";
        deviceID 	= "";
    }
    void dump()
    {
        cout << "sessionId:" << sessionId << endl;
        cout << "deviceID :" << deviceID  << endl;
    }
}
;

// 同 CAgentMsg_ReportCtrlDeviceState， 但是再定义， 防止平台有变化
//
// 网关接入状态接口
//
class CAgentMsg_reportGatewayState
{
public:
    std::string SessionId 	;//String
    std::string ProductID 	;//String 厂商编号(ID)
    std::string DeviceID 	;//string 设备编号
    std::string State 		;//string 在线/不在线
    std::string StateTime 	;//string 状态时间，格式为YYYY-MM-DD
    //		HH:NN:SS（年-月-日时：分：秒）

    CAgentMsg_reportGatewayState()
    {
        SessionId   = "";
        ProductID   = "";
        DeviceID    = "";
        State       = "";
        StateTime 	= "";
    }
    void dump()
    {}
}
;
//
// 设备告警信息接口
//
class CAgentMsg_reportDeviceAlarm
{
public:
    std::string SessionId 	;//String
    std::string ProductID 	;//String 厂商编号(ID)
    std::string DeviceID 	;//string 设备编号
    std::string Operate_id 		;//string 在线/不在线
    std::string Explain 		;//string 在线/不在线
    std::string StateTime 	;//string 状态时间，格式为YYYY-MM-DD
    //		HH:NN:SS（年-月-日时：分：秒）

    CAgentMsg_reportDeviceAlarm()
    {
        SessionId 	= "";
        ProductID 	= "";
        DeviceID 	= "";
        Operate_id 		= "";
        Explain 		= "";
        StateTime 	= "";
    }
    void dump()
    {}
}
;

//
// 网关配置信息接口
//
class CAgentMsg_requestDeviceConfig
{
public:
    std::string SessionId 	;//String
    std::string ProductID 	;//String 厂商编号(ID)

    CAgentMsg_requestDeviceConfig()
    {
        SessionId 	= "";
        ProductID 	= "";
    }
    void dump()
    {}
}
;


//
// 网关配置信息接口 - ack config
//
class CAgentMsg_requestDeviceConfig_ACK_Config
{
public:
    std::string Room ;//String 房间名
    //电信智慧E 家接口规范V0.1
    //- 48 -
    std::string DeviceType ;//String 设备类型
    std::string number ;//String 设备数量

    CAgentMsg_requestDeviceConfig_ACK_Config()
    {
        Room 	= "";
        DeviceType 	= "";
        number 	= "";
    }
    void dump()
    {}
}
;


//
// 网关配置信息接口 - ack
//
class CAgentMsg_requestDeviceConfig_ACK
{
public:
    std::string Result 	;//String
    std::string Reason 	;//String 厂商编号(ID)
    std::string Pointnum 	;//String 频点号
    std::vector<CAgentMsg_requestDeviceConfig_ACK_Config> m_config;
    CAgentMsg_requestDeviceConfig_ACK()
    {
        Result 	= "";
        Reason 	= "";
        Pointnum 	= "";
        m_config.clear();
    }
    void dump()
    {}
}
;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

//
// 请求配置上报接口
//
class CeHomeMsg_requestReportDeviceConfigParam
{
public:
    std::string UserId ;//String 用户编号
    std::string DeviceId ;//String 设备编号

    CeHomeMsg_requestReportDeviceConfigParam()
    {
        UserId = "";
        DeviceId = "";
    }
    void dump()
    {}
}
;

//
// 请求配置上报接口的返回消息
//
class CeHomeMsg_requestReportDeviceConfigParam_ACK
{
public:
    std::string Result 		;//string 结果
    std::string Reason 		;//string 失败原因
    std::string productID 	;//String 厂家编号ID
    std::string userID 		;//String 用户编号
    std::string deviceID 	;//string 设备编号
    CAgentMsg_initDeviceConfigParam_Seat m_Seat;

    CeHomeMsg_requestReportDeviceConfigParam_ACK()
    {
        Result 		="";
        Reason 		="";
        productID 	="";
        userID 		="";
        deviceID 	="";
    }
    void dump()
    {}
}
;


//
// 获取网关状态接口
//
class CeHomeMsg_getCtrlDeviceState
{
public:
    std::string DeviceID ;//String 设备编号
    CeHomeMsg_getCtrlDeviceState()
    {
        DeviceID = "" ;
    }
    void dump()
    {
        cout << "DeviceID:" << DeviceID << endl;
    }
}
;


//
//
//
class CeHomeMsg_getCtrlDeviceState_ACK
{
public:
    std::string Result 	;//string 结果
    //0:表示成功
    //1：失败原因参见Reason
    //2：HTTP 请求错误，失败原因
    //参见Reason
    std::string Reason 	;//string 失败原因
    std::string State 	;//string 在线/不在线
    CeHomeMsg_getCtrlDeviceState_ACK()
    {
        Result = "" ;
        Reason = "" ;
        State = "" ;
    }
    void dump()
    {}
}
;

class CeHomeMsg_register
{
public:
    std::string UserID ;//String 
    std::string DeviceID ;//String 
    CeHomeMsg_register()
    {
        UserID = "" ;
        DeviceID = "" ;
    }
    void dump()
    {
        cout << "UserID :" <<  UserID  << endl;
        cout << "DeviceID :" <<  DeviceID  << endl;
    }
}
;

//
//
//
class CeHomeMsg_register_ACK
{
public:
    std::string Result 	;//string 结果
    //0:表示成功
    //1：失败原因参见Reason
    //2：HTTP 请求错误，失败原因
    //参见Reason
    std::string Reason 	;//string 失败原因
    CeHomeMsg_register_ACK()
    {
        Result = "" ;
        Reason = "" ;
    }
    void dump()
    {
        cout << "Result :" <<  Result  << endl;
        cout << "Reason :" <<  Reason  << endl;
    }
}
;

//
// 获取设备状态接口
//
class CeHomeMsg_getDeviceStateList
{
public:
    std::string DeviceID ;//String 设备编号
    CeHomeMsg_getDeviceStateList()
    {
        DeviceID = "" ;
    }
    void dump()
    {}
}
;

class CeHomeMsg_getDeviceStateList_ACK_operate
{
public:
    std::string operate_id 		;//string 设备操作序号，与设备的开关号
    //			想匹配
    std::string operate_value 	;//string operate-value：设备操作类型的
    //		结果值，与operate-type 结合使
    //		用。当operate-type=0 时，
    //		operate-value=1 表示开，
    //		operate-value=2 表示关
    //		当operate-type=1 时，
    //		operate-value 表示具体的范围
    //	之间的值，如operate-value=325
    //	当operate-type=2 时，
    //	operate-value 表示具体的值，
    //	如operate-value=16
    std::string operate_ranage 	;//String 设备能控制最小值、最大值；例
    //			如：1-33
    std::string operate_type 	;//string operate-type：设备操作类型，0:
    //	开关标志；1：幅度标志：范围为
    //	0-999； 2：固定值方式：如温度
    //	为16,26 等；
    std::string operate_explain ;//string 说明
    std::string value_time 		;//string 状态时间，格式为YYYY-MM-DD
    //	HH:NN:SS（年-月-日时：分：秒）

    CeHomeMsg_getDeviceStateList_ACK_operate()
    {
        operate_id      = "";//string 设备操作序号，与设备的开关号
        operate_value   = "";//string operate-value：设备操作类型的
        operate_ranage  = "";//String 设备能控制最小值、最大值；例
        operate_type    = "";//string operate-type：设备操作类型，0:
        operate_explain = "";//string 说明
        value_time      = "";//string 状态时间，格式为YYYY-MM-DD
    }
    void dump()
    {}
}
;
class CeHomeMsg_getDeviceStateList_ACK
{
public:
    std::string result;//String
    std::string reason;//string 厂家ID

    //operate 操作设备组
    std::vector<CeHomeMsg_getDeviceStateList_ACK_operate> m_operate;

    CeHomeMsg_getDeviceStateList_ACK()
    {

        result 	= "";
        reason 	= "";
    }
    void dump()
    {}
}
;

//
// 控制命令接口 的operate数据
//
class CeHomeMsg_controlDevice_operate
{
public:
    std::string operate_id 		;//String 设备地址码
    std::string operate_ranage 	;//String 设备能控制最小值、最大值；例如：
    std::string operate_type 	;//string 设备类型
    std::string orderId 		;//string 命令码:
    std::string extendpara;
    //orderId :
    //1.单开
    //2.单关
    CeHomeMsg_controlDevice_operate()
    {
        operate_id      = "";
        operate_ranage  = "";
        operate_type    = "";
        orderId         = "";
        extendpara      = "";
    }
    void dump()
    {}
}
;


//
// 控制命令接口
//
class CeHomeMsg_controlDevice
{
public:
    std::string DeviceID 		;//String 设备编号（或主机号，指家庭中控
    //制设备唯一编号）
    std::vector<CeHomeMsg_controlDevice_operate> m_operate;
    //Operate 操作控制组
    CeHomeMsg_controlDevice()
    {
        DeviceID = "";
        m_operate.clear();
    }
    void dump()
    {}
}
;


//
// 控制命令接口的返回消息 - operate 数据
//
class CeHomeMsg_controlDevice_ACK_Operate
{
public:
    std::string operate_id 		;//String 操作编号
    std::string resultState 	;//String 操作返回结果
    std::string stateTime 		;//string 操作结果时间， 格式为
    //YYYY-MM-DD HH:NN:SS（年
    //-月-日时：分：秒）
    CeHomeMsg_controlDevice_ACK_Operate()
    {
        operate_id 	= "";
        resultState = "";
        stateTime 	= "";
    }
    void dump()
    {}
}
;
//
// 控制命令接口的返回消息
//
class CeHomeMsg_controlDevice_ACK
{
public:

    std::string Result 		;//string 	结果
    //	0:表示成功
    //	1：失败，失败原因参见Reason
    //	2：HTTP 请求错误，失败原因
    //	参见Reason
    std::string Reason 		;//string 	失败原因
    std::string deviceID 	;//String 设备编号
    std::vector<CeHomeMsg_controlDevice_ACK_Operate> m_operate;
    //Operate 操作控制组
    CeHomeMsg_controlDevice_ACK()
    {
        Result     = "";
        Reason     = "";
        deviceID   = "";

        m_operate.clear();
    }
    void dump()
    {}
}
;

//{"deviceid":"0x010189e5","operate":"1","value":"1"}
//
// 服务器返回的控制消息
//
#define INVALID_OPERATE 10000
class CTSHomeHostHttpMsg_Operate
{
public:

    std::string deviceid 		;
    std::string operate 		;
    std::string value 	;
    std::string devicetype;
    	
    long long m_ideviceid 	;
    int reserved_1;
    int reserved_2;
    int reserved_3;
    int reserved_4;
    int m_ioperate 		;
    int m_ivalue 	    ;
    int m_idevicetype;
    CTSHomeHostHttpMsg_Operate()
    {
        deviceid     = "";
        operate     = "";
        value   = "";
        devicetype   = "";

        m_ideviceid = 0	;
        m_ioperate 	= INVALID_OPERATE	;
        m_ivalue 	= 0 ;
        m_idevicetype = 0;
    }
    void dump()
    {
        cout << "CTSHomeHostHttpMsg_Operate::dump" << endl;
        cout << "\tdeviceid:" << deviceid << endl;
        cout << "\toperate :" << operate  << endl;
        cout << "\tvalue   :" << value    << endl;
        cout << "\tdevicetype   :" << devicetype    << endl;

        cout << "\tm_ideviceid:" << m_ideviceid << endl;
        cout << "\tm_ioperate :" << m_ioperate  << endl;
        cout << "\tm_ivalue   :" << m_ivalue    << endl;
        cout << "\tm_idevicetype   :" << m_idevicetype    << endl;
        //printf("\t\t(%#x,%d,%d)\n", m_ideviceid, m_ioperate, m_ivalue);
    }
}
;
#endif /* _TEHOMEMSG_H */
