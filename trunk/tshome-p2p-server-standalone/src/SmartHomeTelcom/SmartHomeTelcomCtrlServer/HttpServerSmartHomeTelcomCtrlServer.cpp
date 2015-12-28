/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerSmartHomeTelcomCtrlServer.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: HttpServerSmartHomeTelcomCtrlServer.cpp 5884 2012-09-06 01:31:44Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-06 01:31:44  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "CommonInclude.h"
#include <string.h>
#include <stdio.h>

#include "HttpServerSmartHomeTelcomCtrlServer.h"
#include "../../Logs.h"
#include "../../Configs/ConfigSmartHomeTelcom.h"
#include "str_opr.h"
#include "StringOpr.h"
#include "../SmartHomeTelcomDef.h"
#include "app/server/EasyMysqlConn.h"
#include "app/common/md5_tools.h"
#include "app/server/types.h"
#include "tshome_typedef.h"
#include "app/datagram/remote_def.h"
#include "app/datagram/report_mgr.h"
#include "app/common/config.h"
#include "StringOpr.h"

#include "../../IpcServer/GeneralSocketProcessor.h"

// 单元测试用数据
//#define USE_UNITTEST_DATA

#define INSERT_TELCOM_USER_SQL "INSERT INTO t_jstelcom_user(userid, deviceId, deviceSN, homeId) VALUES('%s', '%s', '%s', %d)"

#define UPDATE_TELCOM_USER_SQL "UPDATE t_jstelcom_user SET homeId=%d WHERE userid='%s'"

#define INSERT_TS_HOME_SQL "INSERT INTO t_home(homeName, province,city, district, address, lastUpdateTime, maxUsers, services, registerTime) \
							VALUES('%s', 15, 171, 1559, 'unknown', NOW(), 10, 1, NOW())"

#define INSERT_TS_USER_SQL "INSERT INTO t_user(userName, pwd, role, userType, mobilephone, registerTime, lastLoginTime, innerPort, home) \
							VALUES('%s', '%s', 1, 0, 'unknown', NOW(), NOW(), 0, %d)"


#define FIND_ALL_SECURITY_DEVICE "SELECT \
									t_floor.floorName,t_room.roomName, t_data_point.id, \
									t_device.deviceName,t_devicetype.deviceTypeId,t_jstelcom_user.userid, \
									t_jstelcom_user.deviceId \
								FROM \
									t_device,t_room,t_floor,t_home,t_devicetype,t_jstelcom_user,t_data_point \
								WHERE \
									t_device.room = t_room.id \
									AND t_device.deviceType=t_devicetype.id \
									AND t_devicetype.id=4 \
									AND t_room.floorId=t_floor.id \
									AND t_floor.home=t_home.id \
									AND t_home.id=t_jstelcom_user.homeId \
									AND t_data_point.device = t_device.id \
									AND t_home.id IN (SELECT t_jstelcom_user.homeId FROM t_jstelcom_user)"

#define INSERT_TS_DEVICE_SQL "INSERT INTO t_device(deviceName, description, room, permission, deviceType, subType, gateway, \
							zone, startRow, startColumn, rowCount, columnCount) \
							VALUES('%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)"

#define INSERT_TS_DATAPOINT_SQL "INSERT INTO t_data_point(device, ctrlType, datatype, grouptype, t_data_point.value) \
								VALUES(%d, %d, %d, %d, '%s')"

#define INSERT_TS_ENOCEAN_SQL "INSERT INTO t_enocean(dataPoint, enoceanId, choiceType, funcType, appType, rocker, autoLearned, totalTime, totalSteps) \
							VALUES(%d, '%s', %d, %d, %d, %d, %d, %d, %d)"

#define GET_LAST_INSERT_ID "SELECT LAST_INSERT_ID()"

#define DEFAULT_TS_USER_PASSWORD "888888"

#define TS_USER_PASSWORD_LEN MD5_LEN+1


extern ts_udb_mgr g_udb_mgr;


CHttpServerSmartHomeTelcomCtrlServer::CHttpServerSmartHomeTelcomCtrlServer(ISocketHandler& h) : HttpdSocket(h)
{
    m_strBody = "";
}

void CHttpServerSmartHomeTelcomCtrlServer::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_SMARTHOMEJSTELCOM, "CHttpServerSmartHomeTelcomCtrlServer::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerSmartHomeTelcomCtrlServer::~CHttpServerSmartHomeTelcomCtrlServer()
{
    LOG4CPLUS_INFO(LOG_SMARTHOMEJSTELCOM, "CHttpServerSmartHomeTelcomCtrlServer::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerSmartHomeTelcomCtrlServer::Exec()
{
    CreateHeader();
    GenerateDocument();
}
void CHttpServerSmartHomeTelcomCtrlServer::OnHeaderComplete()
{
    m_strBody = "";
}


void CHttpServerSmartHomeTelcomCtrlServer::CreateHeader()
{
    SetStatus("200");
    SetStatusText("OK");
    //fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    {
        size_t x = 0;
        for (size_t i = 0; i < GetUri().size(); i++)
            if (GetUri()[i] == '.')
                x = i;
        std::string ext = GetUri().substr(x + 1);
        if (ext == "gif" || ext == "jpg" || ext == "png")
            AddResponseHeader("Content-type", "image/" + ext);
        else
            AddResponseHeader("Content-type", "text/" + ext);
    }
    AddResponseHeader("Connection", "close");
    SendResponse();
}

void CHttpServerSmartHomeTelcomCtrlServer::GenerateDocument()
{
    std::string strSendBuf = "Hello CHttpServerSmartHomeTelcomCtrlServer.\n\r";

    strSendBuf += GetHttpDate()+"\r\n";
    strSendBuf += "Your Body:"+m_strBody+"\n\r";

    strSendBuf = "[{\"result\":\"100\",\"reason\":\"not support url:" + GetUrl() + "\"}]";


    Send(strSendBuf);
}

/** Chunk of http body data recevied. */
void CHttpServerSmartHomeTelcomCtrlServer::OnData(const char *p,size_t l)
{
    if (m_strBody.size()>1024 || l>1024)
    {
        LOG4CPLUS_ERROR(LOG_SMARTHOMEJSTELCOM, "m_strBody(" << m_strBody.size() << ") or l("<<l<<")"<<"too big");
        std::string strBadboy = "Bad boy.";
        CreateHeader();
        Send(strBadboy);

        Reset(); // prepare for next request
        SetCloseAndDelete();
        m_strBody = "";

        return;
    }

    m_strBody += p;
}

void CHttpServerSmartHomeTelcomCtrlServer::OnDataComplete()
{
    CConfigSmartHomeTelcom __cfg;
    __cfg.update();

    //LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "m_strBody:" << m_strBody);

    if (m_strBody.size()<3)
    {
        LOG4CPLUS_ERROR(LOG_SMARTHOMEJSTELCOM, "m_strBody(" << m_strBody.size() << ") should 3 or bigger");

        m_strBody =  "[{\"result\":\"101\",\"reason\":\"m_strBody.size() < 3\"}]";

        ReturnHttpMsg(m_strBody);

        m_strBody = "";

        return;
    }

    size_t ii;
    for(ii=m_strBody.length()-1;ii>0; ii--)
    {
        if (m_strBody.at(ii)==']')
        {
            break;
        }
    }
    //if (ii !=m_strBody.length()-1)
    if (ii > 3)
    {
        //m_strBody.erase(ii+1,m_strBody.length()-1);
        m_strBody.erase(ii,m_strBody.length()-1);
    }

    for(ii=0;ii<m_strBody.length()-1; ii++)
    {
        if (m_strBody.at(ii)=='[')
        {
            m_strBody.erase(0,ii+1);
            break;
        }
    }

    LOG4CPLUS_INFO(LOG_SMARTHOMEJSTELCOM, "Html request:"
                   << "\n\t\t Method: "<<GetMethod()
                   << "\n\t\t URL: "<<GetUrl()
                   << "\n\t\t Http version: "<<GetHttpVersion()
                   << "\n\t\t Body: "<<m_strBody << "\n\t\t\t");


    CTeHomeIF __CTeHomeIF;
    __CTeHomeIF.Parse(m_strBody);


    CreateHeader();

    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "Request URL:" << GetUrl());

    // 5.2.2	注册用户网关接口
    //	http://218.95.39.172:60080/SmartHomeTelcom/register
    if (GetUrl()==__cfg.getConfig().strURL_register)
    {
        Do_register(__CTeHomeIF);
    }
    else if (GetUrl()=="/SmartHomeTelcom/addDevice")
    {
        Do_addDevice(__CTeHomeIF);
    }
    //5.2.4	请求配置上报接口
    //	http://218.95.39.172:60080/SmartHomeTelcom/requestReportDeviceConfigParam
    else if (GetUrl()==__cfg.getConfig().strURL_requestReportDeviceConfigParam)
    {
        Do_requestReportDeviceConfigParam(__CTeHomeIF);
    }
    //5.2.6	获取网关状态接口 getCtrlDeviceState
    //	http://218.95.39.172:60080/SmartHomeTelcom/getCtrlDeviceState
    else if (GetUrl()==__cfg.getConfig().strURL_getCtrlDeviceState)
    {
        Do_getCtrlDeviceState(__CTeHomeIF);
    }
    //5.2.8	获取设备状态接口 getDeviceStateList
    //	http://218.95.39.172:60080/SmartHomeTelcom/getDeviceStateList
    //
    else if (GetUrl()==__cfg.getConfig().strURL_getDeviceStateList)
    {
        Do_getDeviceStateList(__CTeHomeIF);
    }
    //5.2.9	控制命令接口 controlDevice
    //	http://218.95.39.172:60080/SmartHomeTelcom/controlDevice
    else if (GetUrl()==__cfg.getConfig().strURL_controlDevice)
    {
        Do_controlDevice(__CTeHomeIF);
    }
    else
    {
        GenerateDocument();
    }

    Reset(); // prepare for next request
    SetCloseAndDelete();
    m_strBody = "";
}

int CHttpServerSmartHomeTelcomCtrlServer::ReturnHttpMsg(const std::string &strMsg)
{
    CreateHeader();

    Send(strMsg);
    Reset();
    SetCloseAndDelete();

    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_register( CTeHomeIF &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "Do_register");
    char sql[1000] = {0};
    CeHomeMsg_register   __op;
    JsonData.Decode(__op);
    __op.dump();
    // 构造返回消息
    CTeHomeIF __CTeHomeIF;

    CeHomeMsg_register_ACK Msg;

    // 开始使用数据
    int res = 0;
    MYSQL_RES *res_ptr = NULL;
    MYSQL_ROW sqlrow;
    int homeId = 0;
    //get a mysql connection from pool.
	EasyMysqlConn easyConn;
	MysqlConn conn = easyConn.getConnection();
	MYSQL *mysql = conn.mysql;
    cout << "in Do_register\n";
    if (NULL == mysql)
    {
        cout << "in Do_register conn is null\n";
        LOG4CPLUS_ERROR(LOG_DB, "pool_getConn");
        Msg.Result =  "1";
        Msg.Reason= "failed--conn is null";
        //return NULL;
    }else{
		cout << "in Do_register begin insert\n";
	    
		//暂时设homeid为0，待后续取得homeid后更新。
	    sprintf(sql, INSERT_TELCOM_USER_SQL, __op.UserID.c_str(), __op.DeviceID.c_str(), "null", homeId);
		cout << "in Do_register " << sql<<"\n";
		mysql_query(mysql, "SET AUTOCOMMIT=0");//设置为不自动提交，因为MYSQL默认立即执行
		mysql_query(mysql, "BEGIN");//开始事务定义
	    res = mysql_query(mysql, sql);
		//cout << "in Do_register " << INSERT_USER_SQL_TEST<<":"<< __op.UserID<<":"<< __op.DeviceID <<"\n";
	    //res = mysql_query(mysql, INSERT_USER_SQL_TEST);
	    if(res)
	    {
	        LOG4CPLUS_ERROR(LOG_DB, "insert t_jstelcom_user failed");
			Msg.Result =  "1";
    		Msg.Reason= "errorCode=";
			Msg.Reason += mysql_errno(mysql);
			Msg.Reason += " :";
			Msg.Reason += mysql_error(mysql);
			mysql_query(mysql, "ROOLBACK");//判断当执行失败时回滚
	    }else{
			LOG4CPLUS_ERROR(LOG_DB, "insert t_jstelcom_user success");
			Msg.Result =  "0";
    		Msg.Reason= "OK";
		}

#ifdef SMARTHOME_AGENT_JSTELECOM
		//如果在电信用户表里添加成功，在自动为其创建一个天溯用户，同一用户名。
		//用户可以使用天溯用户登录web进行配置等操作。
		if(Msg.Result == "0"){
			memset(sql, 0, sizeof(sql));
			sprintf(sql, INSERT_TS_HOME_SQL, __op.UserID.c_str());
			res = mysql_query(mysql, sql);
			if(res)
		    {
		        LOG4CPLUS_ERROR(LOG_DB, "insert t_home failed");
				Msg.Result =  "1";
	    		Msg.Reason= "errorCode=";
				Msg.Reason += mysql_errno(mysql);
				Msg.Reason += " :";
				Msg.Reason += mysql_error(mysql);
				mysql_query(mysql, "ROOLBACK");//判断当执行失败时回滚
		    }else{
				LOG4CPLUS_ERROR(LOG_DB, "insert t_home success");
				Msg.Result =  "0";
	    		Msg.Reason= "OK";
				//获得homeid
				res = mysql_query(mysql, GET_LAST_INSERT_ID);
				if(!res){
					res_ptr = mysql_use_result(mysql);
					while((sqlrow = mysql_fetch_row(res_ptr))){
						homeId = atoi(sqlrow[0]);
						cout << "homeid=" << homeId<<"\n";
					}
					mysql_free_result(res_ptr);
				}
			}
		}


        if(Msg.Result == "0")
        {
            memset(sql, 0, sizeof(sql));
            char s_encrypt[TS_USER_PASSWORD_LEN] = {0};
            mt_encrypt(DEFAULT_TS_USER_PASSWORD, s_encrypt);
            sprintf(sql, INSERT_TS_USER_SQL, __op.UserID.c_str(), s_encrypt, homeId);
            res = mysql_query(mysql, sql);
            if(res)
            {
                LOG4CPLUS_ERROR(LOG_DB, "insert t_home failed");
                Msg.Result =  "1";
                Msg.Reason= "errorCode=";
                Msg.Reason += mysql_errno(mysql);
                Msg.Reason += " :";
                Msg.Reason += mysql_error(mysql);
                mysql_query(mysql, "ROOLBACK");//判断当执行失败时回滚
            }
            else
            {
                LOG4CPLUS_ERROR(LOG_DB, "insert t_home success");
                Msg.Result =  "0";
                Msg.Reason= "OK";
            }
        }

        //此时用已有的homeid更新t_jstelcom_user
        if(Msg.Result == "0")
        {
            memset(sql, 0, sizeof(sql));
            sprintf(sql, UPDATE_TELCOM_USER_SQL, homeId, __op.UserID.c_str());
            res = mysql_query(mysql, sql);
            if(res)
            {
                mysql_query(mysql, "ROOLBACK");//判断当执行失败时回滚
            }
        }

#endif
        if(Msg.Result == "0")
        {
            mysql_query(mysql, "COMMIT");//提交事务
        }
    }
	cout << "end insert\n";
    __CTeHomeIF.Encode(Msg);

    std::string strSnd = "["+__CTeHomeIF.ToString()+"]";

    Send(strSnd);
    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "SendBack:" << strSnd);

    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_addDevice( CTeHomeIF &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "Do_addDevice");
    CAgentMsg_addDeviceConfigParam   __op;
    JsonData.Decode(__op);
    __op.dump();
    CTeHomeIF __CTeHomeIF;

    CeHomeMsg_register_ACK Msg;
    // 开始使用数据
    char sql[1000] = {0};
    int res = 0;
    MYSQL_RES *res_ptr = NULL;
    MYSQL_ROW sqlrow;
    int dataPointId = 0;
    int deviceId = 0;
    //get a mysql connection from pool.
	EasyMysqlConn easyConn;
	MysqlConn conn = easyConn.getConnection();
	MYSQL *mysql = conn.mysql;
    cout << "in Do_addDevice\n";
    if (NULL == mysql)
    {
        cout << "in Do_addDevice conn is null\n";
        LOG4CPLUS_ERROR(LOG_DB, "pool_getConn");
        Msg.Result =  "1";
        Msg.Reason= "failed--conn is null";
        //return NULL;
    }else{
		cout << "in Do_addDevice begin insert\n";
	    
		//插入t_device表。deviceName, description, room, permission, deviceType, subType, gateway, zone, startRow, startColumn, rowCount, columnCount
	    sprintf(sql, INSERT_TS_DEVICE_SQL, __op.deviceName.c_str(), __op.description.c_str(), atoi(__op.room.c_str()), atoi(__op.permission.c_str()),
	    		atoi(__op.deviceType.c_str()), atoi(__op.subType.c_str()), atoi(__op.gateway.c_str()), atoi(__op.zone.c_str()),
	    		atoi(__op.startRow.c_str()), atoi(__op.startColumn.c_str()), atoi(__op.rowCount.c_str()), atoi(__op.columnCount.c_str()));
		cout << "in Do_addDevice " << sql<<"\n";
		mysql_query(mysql, "SET AUTOCOMMIT=0");//设置为不自动提交，因为MYSQL默认立即执行
		mysql_query(mysql, "BEGIN");//开始事务定义
	    res = mysql_query(mysql, sql);
	    if(res)
	    {
	        LOG4CPLUS_ERROR(LOG_DB, "insert t_device failed");
			Msg.Result =  "1";
    		Msg.Reason= "errorCode=";
			Msg.Reason += mysql_errno(mysql);
			Msg.Reason += " :";
			Msg.Reason += mysql_error(mysql);
			mysql_query(mysql, "ROOLBACK");//判断当执行失败时回滚
	    }else{
			LOG4CPLUS_ERROR(LOG_DB, "insert t_device success");
			Msg.Result =  "0";
    		Msg.Reason= "OK";
			//获得deviceId
			res = mysql_query(mysql, GET_LAST_INSERT_ID);
			if(!res){
				res_ptr = mysql_use_result(mysql);
				while((sqlrow = mysql_fetch_row(res_ptr))){
					deviceId= atoi(sqlrow[0]);
					cout << "deviceId=" << deviceId<<"\n";
				}
				mysql_free_result(res_ptr);
			}
		}
		//在t_data_point 中插入记录。
		if(Msg.Result == "0"){
			memset(sql, 0, sizeof(sql));
			//device, ctrlType, datatype, grouptype, t_data_point.value
			sprintf(sql, INSERT_TS_DATAPOINT_SQL, atoi(__op.deviceId.c_str()), atoi(__op.ctrlType.c_str()), atoi(__op.datatype.c_str()), 
					atoi(__op.grouptype.c_str()), __op.value.c_str());
			res = mysql_query(mysql, sql);
			if(res)
		    {
		        LOG4CPLUS_ERROR(LOG_DB, "insert t_data_point failed");
				Msg.Result =  "1";
	    		Msg.Reason= "errorCode=";
				Msg.Reason += mysql_errno(mysql);
				Msg.Reason += " :";
				Msg.Reason += mysql_error(mysql);
				mysql_query(mysql, "ROOLBACK");//判断当执行失败时回滚
		    }else{
				LOG4CPLUS_ERROR(LOG_DB, "insert t_data_point success");
				Msg.Result =  "0";
	    		Msg.Reason= "OK";
				//获得dataPointId
				res = mysql_query(mysql, GET_LAST_INSERT_ID);
				if(!res){
					res_ptr = mysql_use_result(mysql);
					while((sqlrow = mysql_fetch_row(res_ptr))){
						dataPointId= atoi(sqlrow[0]);
						cout << "dataPointId=" << dataPointId<<"\n";
					}
					mysql_free_result(res_ptr);
				}
			}
		}


        if(Msg.Result == "0")
        {
            memset(sql, 0, sizeof(sql));
            //dataPoint, enoceanId, choiceType, funcType, appType, rocker, autoLearned, totalTime, totalSteps
            sprintf(sql, INSERT_TS_ENOCEAN_SQL, dataPointId, __op.enoceanId.c_str(), atoi(__op.choiceType.c_str()),
                    atoi(__op.funcType.c_str()), atoi(__op.appType.c_str()), atoi(__op.rocker.c_str()),
                    atoi(__op.autoLearned.c_str()), atoi(__op.totalTime.c_str()), atoi(__op.totalSteps.c_str()));
            res = mysql_query(mysql, sql);
            if(res)
            {
                LOG4CPLUS_ERROR(LOG_DB, "insert t_enocean failed");
                Msg.Result =  "1";
                Msg.Reason= "errorCode=";
                Msg.Reason += mysql_errno(mysql);
                Msg.Reason += " :";
                Msg.Reason += mysql_error(mysql);
                mysql_query(mysql, "ROOLBACK");//判断当执行失败时回滚
            }
            else
            {
                LOG4CPLUS_ERROR(LOG_DB, "insert t_enocean success");
                Msg.Result =  "0";
                Msg.Reason= "OK";
            }
        }

        if(Msg.Result == "0")
        {
            mysql_query(mysql, "COMMIT");//提交事务
        }
    }
	cout << "end insert\n";
    
    // 构造返回消息
    __CTeHomeIF.Encode(Msg);

    std::string strSnd = "["+__CTeHomeIF.ToString()+"]";

    Send(strSnd);
    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "SendBack:" << strSnd);

    return 0;
}
int CHttpServerSmartHomeTelcomCtrlServer::Do_requestReportDeviceConfigParam( CTeHomeIF &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "Do_requestReportDeviceConfigParam");
    CeHomeMsg_requestReportDeviceConfigParam   __op;
    JsonData.Decode(__op);
    __op.dump();
    // 开始使用数据

    // 构造返回消息

    CTeHomeIF __CTeHomeIF;

    CeHomeMsg_requestReportDeviceConfigParam_ACK Msg;
    CAgentMsg_initDeviceConfigParam_Seat __seat;
    CAgentMsg_initDeviceConfigParam_Seat_deploy __deploy;
    CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate __operate;

    __operate.operate_id      = TEST_OPERATE_ID_SW;
    __operate.operate_ranage  = ""; // 开关类的此无需，务必空， 不得填值
    __operate.operate_type    = "0"; //开关
    __operate.operate_value   = ezConvertToString(1); // 开关
    __operate.operate_explain = TEST_OPERATE_NAME;
    ezGBKToUTF8(__operate.operate_explain);

    // 物品
    __deploy.deploy_name = "大灯";
    ezGBKToUTF8(__deploy.deploy_name);

    __deploy.deploy_type ="2" ;//设备的类型，1：安防设备；2：家居设备
    __deploy.deviceType  ="1"  ;
    __deploy.device_model="tiansu-switch-601";
    __deploy.m_Operate.push_back(__operate);

    __operate.operate_id      = TEST_OPERATE_ID_SW;
    __operate.operate_ranage  = ""; // 开关类的此无需，务必空， 不得填值
    __operate.operate_type    = "0"; //开关
    __operate.operate_value   = "2"; // 关
    __operate.operate_explain = "大灯";
    ezGBKToUTF8(__operate.operate_explain);

    __seat.seat_name = "健身房";
    ezGBKToUTF8(__seat.seat_name);
    __seat.m_deploy.push_back(__deploy);

    Msg.Result = "0";
    Msg.Reason 	= "OK";

    Msg.productID 	= JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU;
    Msg.userID 		= "tiansuTest";
    Msg.deviceID 	= JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU;

    Msg.m_Seat = __seat;

    __CTeHomeIF.Encode(Msg);
    std::string strSnd = "["+__CTeHomeIF.ToString()+"]";


    Send(strSnd);
    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "SendBack:" << strSnd);

    return 0;
}
int CHttpServerSmartHomeTelcomCtrlServer::Do_getCtrlDeviceState( CTeHomeIF &JsonData)
{
	struct timeval tv_begin, tv_end;//handle takes time
	gettimeofday(&tv_begin, NULL);

    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "Do_getCtrlDeviceState");
    CeHomeMsg_getCtrlDeviceState_ACK Msg;
    CeHomeMsg_getCtrlDeviceState   __op;
    JsonData.Decode(__op);
    __op.dump();

    // 开始使用数据
#ifdef USE_UNITTEST_DATA

    Msg.State = "1";
    Msg.Reason= "online"; // 网关编号
#else

    //search the host user.
    std::vector<ts_user> host_list;
	int result = g_udb_mgr.jstelcom.get_hosts_by_device_id(__op.DeviceID.c_str(), host_list);
    if(result == 0 && host_list.size() > 0)
    {
        //找到了该主机用户，则说明其在线。
        Msg.State = "1";
        Msg.Reason= "online"; // 网关编号
    }
    else
    {
        Msg.State = "0";
        Msg.Reason= "offline"; // 网关编号
    }
#endif
    // 构造返回消息
    CConfigSmartHomeTelcom __cfg;
    __cfg.update();

    CTeHomeIF __CTeHomeIF;

    //Msg.SessionId 	= m_strSessionId;
    Msg.Result =  "0";// JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU; // 厂商编号(ID)

    __CTeHomeIF.Encode(Msg);
    //SendPostStr(m_strHostPort.c_str(), "/smartHome/servlet/reportCtrlDeviceState", __CTeHomeIF.ToString().c_str());
    std::string strSnd = "["+__CTeHomeIF.ToString()+"]";


    Send(strSnd);
    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "SendBack:" << strSnd);
	
	gettimeofday(&tv_end, NULL);
	double tTime = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + tv_end.tv_usec - tv_begin.tv_usec;
	cout << "!!!!!!!Do_getCtrlDeviceState takes:"<< tTime << " us" << endl;
	
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_getCtrlDeviceState takes:"<< tTime << " us");

    return 0;
}
int CHttpServerSmartHomeTelcomCtrlServer::Do_getDeviceStateList( CTeHomeIF &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "Do_getDeviceStateList");

    CeHomeMsg_getDeviceStateList   __op;
    JsonData.Decode(__op);
    __op.dump();
    // 开始使用数据

    // 构造返回消息
    CTeHomeIF __CTeHomeIF;

    CeHomeMsg_getDeviceStateList_ACK Msg;
    Msg.result = "0";
    Msg.reason = "OK";

    CeHomeMsg_getDeviceStateList_ACK_operate _operate;
    _operate.operate_id      = TEST_OPERATE_ID_SW;
    _operate.operate_ranage  = ""; // 开关类的此无需，务必空， 不得填值
    _operate.operate_type    = "0"; //开关
    _operate.operate_value   = "2"; // 关
    _operate.operate_explain = "大灯";
    ezGBKToUTF8(_operate.operate_explain);
    char pBuff[32];
    _operate.value_time       = get_date_time_string(pBuff, NULL);

    Msg.m_operate.push_back(_operate);

    __CTeHomeIF.Encode(Msg);
    std::string strSnd = "["+__CTeHomeIF.ToString()+"]";


    Send(strSnd);
    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "SendBack:" << strSnd);

    return 0;
}


//发送设备操作报文
int CHttpServerSmartHomeTelcomCtrlServer::sendDeviceCtrlMsg(char *ip,
                       const int port,
                       const char *operate_id,
                       const char *value,
                       const u_char value_len,
                       const int32_t session_id)
{
    struct timeval tv_begin, tv_end;
    gettimeofday(&tv_begin, NULL);

    device_control_req_enocean ctrl;
    memset(&ctrl, 0, sizeof(device_control_req_enocean));
    int body_len = sizeof(device_control_req_enocean) - sizeof(remote_header_t);
    init_common_header(&ctrl.header, 0, TSHOME_PROTOCOL_VERSION, body_len, COMMON_DEVICE_CONTROL,
                       REQUEST, SERVER);
    ctrl.read_write= WRITE;
    ctrl.data_len = 1;//value_len;
    //memcpy(ctrl.data, value, value_len);
    //ezConvertFromString(ctrl.data, value); // 单字节的会转成ascii码值

	
    ctrl.data = atoi(value); //待修正后解注error: incompatible types in assignment of ??int?ˉ to ??unsigned char [0]?ˉ

	
    //ezConvertFromString(ctrl.datapoint_id, operate_id);
    ctrl.datapoint_id = atoi(operate_id);
    ctrl.datapoint_id = htonl(ctrl.datapoint_id);
    ctrl.session_id = htonl(session_id);//add htonl 10.24

    //	 __fline;
    //	 printf ("0x%0x, %d, %d, %d, %d, %s, %d\n"
    //	 , ctrl.session_id
    //	 , ctrl.datapoint_id
    //	 , ctrl.read_write
    //	 , ctrl.data_len
    //	 , ctrl.data
    //	 , value
    //	 , atoi(value)
    //	 );

    g_GeneralSocketProcessor.sendDeviceCtrlEnocean(ip, port, &ctrl);
	
	gettimeofday(&tv_end, NULL);
	double tTime = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + tv_end.tv_usec - tv_begin.tv_usec;;
	//__fline
	//cout << "!!!!!!! sendDeviceCtrlMsg takes:"<< tTime << " us" << endl;
	
    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_controlDevice(CTeHomeIF &JsonData)
{
	struct timeval tv_begin, tv_end;//handle takes time
	gettimeofday(&tv_begin, NULL);

    CeHomeMsg_controlDevice_ACK Msg;
    CeHomeMsg_controlDevice   __op;
    CeHomeMsg_controlDevice_operate op;
    JsonData.Decode(__op);
    __op.dump();
    // 开始使用数据
#ifdef USE_UNITTEST_DATA

    Msg.Result  = "0" ;
    Msg.Reason  = "OK" ;

#else

    int vecSize = __op.m_operate.size();
    //cout << "vecSize=" << vecSize << endl;
    ts_user host_user;
    user_sdp sdp;
    //search the host user.
    std::vector<ts_user> host_list;
    int result = g_udb_mgr.jstelcom.get_hosts_by_device_id(__op.DeviceID.c_str(), host_list);
    if(result == 0 && host_list.size() > 0)
    {
        // get the only one host user in this list.
        host_user = host_list[0];
        sdp = host_user.sdpVec[0];

        for(int i = 0 ; i < vecSize ; i++)
        {
            op = __op.m_operate[i];

            LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "NO:" << i << ", op.operate_id:"<< op.operate_id << ", op.orderId="<<op.orderId<<",op.extendpara="<<op.extendpara);

            //if(op.orderId == "1" && atoi(op.extendpara.c_str()) > 0)//单开，并且扩展参数是有效数字
            if(op.orderId == "1")//单开
            {
                /*char *value = (char *)op.extendpara.c_str();
                sendDeviceCtrlMsg(sdp->addr, sdp->port, op.operate_id.c_str(), value, strlen(value), host_user->session_id);*/
                sendDeviceCtrlMsg(sdp.addr, sdp.port, op.operate_id.c_str(), "1", 1, host_user.session_id);
            }
            if(op.orderId == "2")//单关
            {
                sendDeviceCtrlMsg(sdp.addr, sdp.port, op.operate_id.c_str(), "0", 1, host_user.session_id);
            }
            if(op.orderId == "4")//全开
            {
                if(op.operate_type == "1")//
                {
                }
                //TODO
            }
            if(op.orderId == "5")//全关
            {
                //TODO
            }
            if(op.orderId == "6" || op.orderId == "7")//加|| 减
            {
                char *value = (char *)op.extendpara.c_str();
				
                sendDeviceCtrlMsg(sdp.addr, sdp.port, op.operate_id.c_str(), value, strlen(value), host_user.session_id);
            }
        }
        Msg.Result  = "0" ;
        Msg.Reason  = "OK" ;
    }
    else
    {
        //主机不在线时，立即回复失败。
        Msg.Result= "1";
        Msg.Reason= "offline";
        LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "offline");
    }

#endif
    // 构造返回消息

    CTeHomeIF __CTeHomeIF;

    Msg.deviceID  = JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU;

    CeHomeMsg_controlDevice_ACK_Operate _operate;

    _operate.operate_id =op.operate_id;
    _operate.resultState="1"   ;
    char pBuff[32];
    _operate.stateTime       = get_date_time_string(pBuff, NULL);

    Msg.m_operate.push_back(_operate);

    __CTeHomeIF.Encode(Msg);
    std::string strSnd = "["+__CTeHomeIF.ToString()+"]";

    Send(strSnd);
    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "SendBack:" << strSnd);
	
	gettimeofday(&tv_end, NULL);
	double tTime = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + tv_end.tv_usec - tv_begin.tv_usec;
	//__fline
	cout << "!!!!!!!Do_controlDevice takes:"<< tTime << " us" << endl;

    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_controlDevice takes:"<< tTime << " us");

    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_AddAccount( CTeHomeIF &JsonData)
{
    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_DelAccount( CTeHomeIF &JsonData)
{
    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_ModAccount( CTeHomeIF &JsonData)
{
    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_QryAccount( CTeHomeIF &JsonData)
{
    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_AddDevice( CTeHomeIF &JsonData)
{
    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_DelDevice( CTeHomeIF &JsonData)
{
    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_ModDevice( CTeHomeIF &JsonData)
{
    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_QryDevice( CTeHomeIF &JsonData)
{
    return 0;
}

int CHttpServerSmartHomeTelcomCtrlServer::Do_ControlDevice( CTeHomeIF &JsonData)
{
    return 0;
}

