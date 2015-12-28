/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentHttpKeepConnector.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentHttpKeepConnector.cpp 5884 2012-07-10 04:11:10Z WuJunjie $
 *
 *  Explain:
 *     -
 *      http 连接器，
 *      保持连接，断开后自动再次连接
 *     -
 *
 *  Update:
 *     2012-07-10 04:11:10  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "CommonInclude.h"

#include <assert.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "str_opr.h"
#include "StringOpr.h"

#include <app/server/mysqlpool.h>

#include "../Logs.h"

#include "HttpKeepConnectorSmartHomeTelcom.h"
#include "AgentSmartHomeTelcom.h"
#include "../Configs/ConfigSmartHomeTelcom.h"

#define DEB(x)
#define DBG(x)

#define FIND_BY_ID_SQL "SELECT	\
							t_floor.floorName,t_room.roomName, t_device.id, t_data_point.id, \
							t_device.deviceName,t_devicetype.deviceTypeName,t_jstelcom_user.userid \
						FROM \
							t_device,t_room,t_floor,t_home,t_devicetype,t_jstelcom_user, t_data_point \
						WHERE \
							t_device.room = t_room.id \
							AND t_device.deviceType=t_devicetype.id \
							AND t_room.floorId=t_floor.id \
							AND t_floor.home=t_home.id \
							AND t_data_point.device = t_device.id \
							AND t_home.id=t_jstelcom_user.deviceId \
							AND t_home.id = %s"

#define FIND_ALL_SQL "SELECT	\
						t_floor.floorName,t_room.roomName, t_device.id, t_data_point.id, \
						t_device.deviceName,t_devicetype.deviceTypeId,t_jstelcom_user.userid, \
						t_jstelcom_user.deviceId \
					FROM \
						t_device,t_room,t_floor,t_home,t_devicetype,t_jstelcom_user, t_data_point \
					WHERE \
						t_device.room = t_room.id \
						AND t_device.deviceType=t_devicetype.id \
						AND t_room.floorId=t_floor.id \
						AND t_floor.home=t_home.id \
						AND t_home.id=t_jstelcom_user.homeId \
						AND t_data_point.device = t_device.id \
						AND t_home.id IN (SELECT t_jstelcom_user.homeId FROM t_jstelcom_user)"

//根据数据库，t_devicetype.id!=4表示所有非安防设备
/*
SELECT 
									t_floor.floorName,t_room.roomName, t_data_point.id, 
									t_device.deviceName,t_devicetype.deviceTypeId,t_jstelcom_user.userid, 
									t_jstelcom_user.deviceId 
								FROM 
									t_device,t_room,t_floor,t_home,t_devicetype,t_jstelcom_user, t_data_point 
								WHERE 
									t_device.room = t_room.id 
									AND t_device.deviceType=t_devicetype.id 
									AND t_devicetype.id!=4 
									AND t_room.floorId=t_floor.id 
									AND t_floor.home=t_home.id 
									AND t_home.id=t_jstelcom_user.homeId 
									AND t_data_point.device = t_device.id 
									AND t_home.id IN (SELECT t_jstelcom_user.homeId FROM t_jstelcom_user);
*/
#define FIND_ALL_HOME_DEVICE "SELECT \
									t_floor.floorName,t_room.roomName, t_data_point.id, \
									t_device.deviceName,t_devicetype.deviceTypeId,t_jstelcom_user.userid, \
									t_jstelcom_user.deviceId \
								FROM \
									t_device,t_room,t_floor,t_home,t_devicetype,t_jstelcom_user, t_data_point \
								WHERE \
									t_device.room = t_room.id \
									AND t_device.deviceType=t_devicetype.id \
									AND t_devicetype.id!=4 \
									AND t_room.floorId=t_floor.id \
									AND t_floor.home=t_home.id \
									AND t_home.id=t_jstelcom_user.homeId \
									AND t_data_point.device = t_device.id \
									AND t_home.id IN (SELECT t_jstelcom_user.homeId FROM t_jstelcom_user)"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define ACTION_URL_COMMENT "interUrl"
static std::string __g_strCurHeaderField = "";

#define BUF_LEN 1024*10
#define DCB_CALLBACK(x) int http_##x##_callback(http_parser *parser, const char *at, size_t length)


DCB_CALLBACK(header_field)
{
#define CODES \
    char str_buf[BUF_LEN];\
    memcpy(str_buf, at, length);\
    str_buf[length] = '\0';

    CODES;

    DBG(
        __fline;

        printf("%s : ", str_buf);
    );

    __g_strCurHeaderField = str_buf;

    return 0;
}

DCB_CALLBACK(header_value)
{
    CODES;
    //    DBG(
    //__fline;

    printf("%s\n", str_buf);
    //    );

    if (__g_strCurHeaderField ==ACTION_URL_COMMENT)
    {
        g_SmartHomeAgentTelcom.GetHttpKeepConnector()->OnURL(str_buf);
    }

    // reset when finished.
    __g_strCurHeaderField = "";

    return 0;
}

DCB_CALLBACK(body)
{
    CODES;

    //printf("body(%d) : \n%s\n", strlen(str_buf), str_buf);
    g_SmartHomeAgentTelcom.GetHttpKeepConnector()->OnBody(str_buf, strlen(str_buf));

    return 0;
}

DCB_CALLBACK(url)
{
    CODES
    DBG(printf("url : %s\n", str_buf););

    g_SmartHomeAgentTelcom.GetHttpKeepConnector()->OnURL(str_buf);

    return 0;
}

#define CB_CALLBACK(x) int http_##x##_callback(http_parser *parser)
#define TOKEN "----------------------------------------------"
	#define GET_PROMPT "<<<<<<<<<<< "
	#define SND_PROMPT ">>>>>>>>>>> "

CB_CALLBACK(message_begin)
{
    DBG(printf(TOKEN"message_begin""\n"););
    return 0;
}

CB_CALLBACK(message_complete)
{
    DBG(printf(TOKEN"message_complete""\n"););

    // 结束了， 初始化，等待下面的数据
    http_parser_init(parser, HTTP_RESPONSE);

    return 0;
}

CB_CALLBACK(headers_complete)
{
    DBG(
        printf(TOKEN"headers_complete""\n");
    );
    return 0;
}
//////////////////////////////////////////////////
CHttpKeepConnectorSmartHomeTelcom::CHttpKeepConnectorSmartHomeTelcom(ISocketHandler& h, const std::string &strSocketName)
        :TcpSocket(h)
        //,CEZThread(CHttpKeepConnectorSmartHomeTelcom_THREAD_ATTR)
        ,m_b_connected(false)
{
    m_iConnTimes = 0;
    // initial connection timeout setting
    SetConnectTimeout(5);

    SetDeleteByHandler();

    SetSockName(strSocketName);
    m_tOnData = 0;


    m_httpParser = new http_parser;
    m_httpSettings = new http_parser_settings;
    assert(m_httpParser);
    assert(m_httpSettings);

    // 仅解析反馈的数据
    http_parser_init(m_httpParser, HTTP_RESPONSE);

    CConfigSmartHomeTelcom __cfg;
    __cfg.update();

#define NCB(n) http_##n##_callback
    #define SCB(x) m_httpSettings->on_##x = NCB(x)

    SCB(url);
    SCB(header_field);
    SCB(header_value);
    SCB(body);
    SCB(message_begin);
    SCB(headers_complete);
    SCB(message_complete);

    IniStatus();
    // 还没有连接
    m_iCommunicationStatus = EnCommunicationStatus_Initialize;
    m_ttLastSendData = 0;

    // 初始化上报设备
    m_iInitDeviceConfigParamNo = 0;

    m_strURL_requestAuth					= __cfg.getConfig().strURL_requestAuth					    ;
    m_strURL_initDeviceConfigParam			= __cfg.getConfig().strURL_initDeviceConfigParam			;
    m_strURL_reportCtrlDeviceState			= __cfg.getConfig().strURL_reportCtrlDeviceState			;
    m_strURL_reportDeviceStateList			= __cfg.getConfig().strURL_reportDeviceStateList			;
    m_strURL_reportAlarm					= __cfg.getConfig().strURL_reportAlarm					    ;
    m_strURL_register						= __cfg.getConfig().strURL_register						    ;
    m_strURL_requestReportDeviceConfigParam = __cfg.getConfig().strURL_requestReportDeviceConfigParam   ;
    m_strURL_getCtrlDeviceState             = __cfg.getConfig().strURL_getCtrlDeviceState               ;
    m_strURL_getDeviceStateList             = __cfg.getConfig().strURL_getDeviceStateList               ;
    m_strURL_controlDevice                  = __cfg.getConfig().strURL_controlDevice                    ;

}


CHttpKeepConnectorSmartHomeTelcom::~CHttpKeepConnectorSmartHomeTelcom()
{
    if (m_httpParser)
    {
        delete m_httpParser;
    }

    if (m_httpSettings)
    {
        delete m_httpSettings;
    }
}

CHttpKeepConnectorSmartHomeTelcom *CHttpKeepConnectorSmartHomeTelcom::Reconnect()
{
    m_iConnTimes++;

    std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
    CHttpKeepConnectorSmartHomeTelcom *p = new CHttpKeepConnectorSmartHomeTelcom(Handler());
    p -> SetDeleteByHandler();
    p -> Open(*ad);

    if (m_iConnTimes>10)
    {
        p -> SetConnectTimeout(m_iConnTimes);
    }

    Handler().Add(p);
    return p;
}


void CHttpKeepConnectorSmartHomeTelcom::OnConnectFailed()
{

    CHttpKeepConnectorSmartHomeTelcom *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CHttpKeepConnectorSmartHomeTelcom::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}

void CHttpKeepConnectorSmartHomeTelcom::SetCloseAndDelete()
{
    TcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CHttpKeepConnectorSmartHomeTelcom::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    m_iConnTimes = 0;
    // do not modify, wujj 2012-4-9 15:31:44 end
    ////////// add your code here
    SetCommunicationStatus(EnCommunicationStatus_Connected);

    char pBuff[GET_DATE_TIME_STRING_DEFAULT_STR_LEN];
    printf("CHttpKeepConnectorSmartHomeTelcom::OnConnect() - %s\n", get_date_time_string(pBuff, NULL));

    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "OnConnect()");
}
void CHttpKeepConnectorSmartHomeTelcom::OnReconnect()
{
    printf("CHttpKeepConnectorSmartHomeTelcom::OnReconnect()\n");
    SetCommunicationStatus(EnCommunicationStatus_Connecting);

    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "OnReconnect()");
}

void CHttpKeepConnectorSmartHomeTelcom::OnDisconnect()
{
    char pBuff[GET_DATE_TIME_STRING_DEFAULT_STR_LEN];
    printf("CHttpKeepConnectorSmartHomeTelcom::OnDisconnect() - %s\n", get_date_time_string(pBuff, NULL));

    LOG4CPLUS_DEBUG(LOG_SMARTHOMEJSTELCOM, "OnDisconnect()");

    //断开了， 设置一下变量
    IniStatus();
    // 设置状态为连接状态
    SetCommunicationStatus(EnCommunicationStatus_Initialize);
}

void CHttpKeepConnectorSmartHomeTelcom::OnConnectTimeout()
{
    printf("CHttpKeepConnectorSmartHomeTelcom::OnConnectTimeout()\n");
    SetCommunicationStatus(EnCommunicationStatus_Connecting);
}

void CHttpKeepConnectorSmartHomeTelcom::OnRawData(const char *buf,size_t len)
{
    m_tOnData = time(NULL);

    DBG(
        __fline;
        printf("CGeneralAgentTcpSocket::OnRawData:%d\n", len);

        std::string strData((char *)buf, len);
        printf("------------------------------------------\n");
        printf("%s", strData.c_str());
        printf("\n------------------------------------------\n");
    );

    int ret = http_parser_execute(m_httpParser, m_httpSettings, buf, len);
    ARG_USED(ret);

    DBG(
        printf("\nhttp_parser_execute ret : %d\n", ret);
    );
    //////////
    //有数据就调用， 另一种方式
    //OnCommand((char *)buf, (unsigned int)len);
    ///////////
}

void CHttpKeepConnectorSmartHomeTelcom::OnURL(std::string strURL)
{
    m_strCurrentURL = strURL;
}
const std::string& CHttpKeepConnectorSmartHomeTelcom::GetUrl()
{
    return m_strCurrentURL;
}
void CHttpKeepConnectorSmartHomeTelcom::OnBody(char *str_buf
        , unsigned int ulCmdDataLen)
{
    //printf("CHttpKeepConnectorSmartHomeTelcom::OnBody, len:%d\n", ulCmdDataLen);

    //__fline;
    //cout << ">>>>>>>>>>>>>> OnBody, url:" << GetUrl() << endl;

    // 测试用
    if (GetUrl()=="/SmartHomeTiansu/controlDevice")
    {
        __fline;
        cout << "###########################################" << endl;
        //SendPostStr(GetUrl(), str_buf);
        return;
    }
    else
    {
        ;
    }


    trim_blank(str_buf, strlen(str_buf));

    if(str_buf[0] != '[')
    {
        __trip;

        cout << GET_PROMPT
        <<"Wrong data("
        <<strlen(str_buf)
        <<"):" \
        << endl \
        << TOKEN\
        << endl\
        << str_buf\
        << endl\
        << TOKEN\
        << endl;

        return;
    }
    else
    {
        //    DBG(
        cout << GET_PROMPT <<"On JSON data("<<strlen(str_buf)<<"):" \
        << endl \
        << TOKEN\
        << endl\
        << str_buf\
        << endl\
        << TOKEN\
        << endl;
        //	);
        DBG(
            LOG4CPLUS_INFO(LOG_SMARTHOMEJSTELCOM, GET_PROMPT <<"On JSON data("<<strlen(str_buf)<<"):" \
                           << endl \
                           << TOKEN\
                           << endl\
                           << str_buf\
                           << endl\
                           << TOKEN\
                           << endl);
        );
    }
    trim_c(str_buf, '[', strlen(str_buf));
    trim_c(str_buf, ']', strlen(str_buf));

    //printf("body(%d) : \n%s\n", strlen(str_buf), str_buf);

    CTeHomeIF __CTeHomeIF;
    __CTeHomeIF.Parse(str_buf);

    switch(m_iCommunicationStatus)
    {
        case EnCommunicationStatus_FactoryAuthing:
        {
            CAckMsgSession __op;
            __CTeHomeIF.Decode(__op);
            DBG(
                __trip;
                __op.dump();
            );
            // 认证成功
            if (__op.result=="0")
            {
                m_strSessionId = __op.SessionId;
                std::cout << "FactoryAuthed Succeeded, Reason:" \
                << __op.reason \
                << std::endl \
                <<"\tNext -> EnCommunicationStatus_initDeviceConfigParaming"\
                << std::endl;

                SetCommunicationStatus(EnCommunicationStatus_FactoryAuthed);
            }
            else
            {
                std::cout << "! FactoryAuthed Failed, Reason:"
                << __op.reason
                << std::endl
                <<"\tNext -> EnCommunicationStatus_FactoryAuthing, waite "<< CMD_TIMEOUT_SMARTHOME_JSTELECOM <<" seconds."
                << std::endl;
            }

            break;
        }
        case EnCommunicationStatus_initDeviceConfigParaming:
        {
            CAckMsgSimple __op;
            __CTeHomeIF.Decode(__op);
            //__fline;
            //cout << "EnCommunicationStatus_initDeviceConfigParaming\n";
            //__op.dump();

            if (__op.result=="0")
            {
                //std::cout << "EnCommunicationStatus_initDeviceConfigParamed, SessionId:" << m_strSessionId << std::endl;
                std::cout << "initDeviceConfigParam Succeeded, Reason:" \
                << __op.reason \
                << std::endl \
                <<"\tNext -> EnCommunicationStatus_reportCtrlDeviceStateing"\
                << std::endl;

                //上报完毕再改状态
                //SetCommunicationStatus(EnCommunicationStatus_initDeviceConfigParamed);

            }
            else
            {
                std::cout << "! initDeviceConfigParam Failed, Reason:"
                << __op.reason
                << std::endl
                //<<"\tNext -> EnCommunicationStatus_initDeviceConfigParaming, waite "<< CMD_TIMEOUT_SMARTHOME_JSTELECOM <<" seconds."
                <<"\tNext -> EnCommunicationStatus_reportCtrlDeviceStateing, for dbg."
                << std::endl;

                // stutes set to end for debug
                //SetCommunicationStatus(EnCommunicationStatus_initDeviceConfigParamed);
            }

            break;
        }
        case EnCommunicationStatus_reportCtrlDeviceStateing:
        {
            CAckMsgSimple __op;
            __CTeHomeIF.Decode(__op);
            DBG(
                __fline;
                cout << "EnCommunicationStatus_reportCtrlDeviceStateing\n";
                __op.dump();
            );
            if (__op.result=="0")
            {
                //std::cout << "EnCommunicationStatus_reportCtrlDeviceStateed, SessionId:" << m_strSessionId << std::endl;
                std::cout << "reportCtrlDeviceState Succeeded, Reason:" \
                << __op.reason \
                << std::endl \
                <<"\tNext -> EnCommunicationStatus_reportDeviceStateListing"\
                << std::endl;

                SetCommunicationStatus(EnCommunicationStatus_reportCtrlDeviceStateed);
            }
            else
            {
                std::cout << "! reportCtrlDeviceState Failed, Reason:"
                << __op.reason
                << std::endl
                //<<"\tNext -> EnCommunicationStatus_reportCtrlDeviceStateing, waite "<< CMD_TIMEOUT_SMARTHOME_JSTELECOM <<" seconds."
                <<"\tNext -> EnCommunicationStatus_reportDeviceStateListing, for dbg."
                << std::endl;

                // stutes set to end for debug
                //SetCommunicationStatus(EnCommunicationStatus_reportCtrlDeviceStateed);
            }
            break;
        }
        case EnCommunicationStatus_reportDeviceStateListing:
        {

            CAckMsgSimple __op;
            __CTeHomeIF.Decode(__op);
            DBG(
                __fline;
                cout << "EnCommunicationStatus_reportDeviceStateListing\n";
                __op.dump();
            );
            if (__op.result=="0")
            {
                //std::cout << "EnCommunicationStatus_reportCtrlDeviceStateed, SessionId:" << m_strSessionId << std::endl;
                DBG(
                    std::cout << "\treportDeviceStateList Succeeded, Reason:" \
                    << __op.reason \
                    << std::endl \
                    <<"\tNext -> EnCommunicationStatus_reportAlarming"\
                    << std::endl;
                );
                SetCommunicationStatus(EnCommunicationStatus_reportDeviceStateListed);
                // skip alarm ,,
                //SetCommunicationStatus(EnCommunicationStatus_reportDeviceStateListing);
                //测试用 5s后上报报警
                SetCommunicationStatus(EnCommunicationStatus_reportAlarming);

            }
            else
            {
                std::cout << "! reportDeviceStateListExample Failed, Reason:"
                << __op.reason
                << std::endl
                //<<"\tNext -> EnCommunicationStatus_reportDeviceStateListing, waite "<< CMD_TIMEOUT_SMARTHOME_JSTELECOM <<" seconds."
                <<"\tNext -> EnCommunicationStatus_reportAlarming, for dbg."
                << std::endl;

                // stutes set to end for debug
                //SetCommunicationStatus(EnCommunicationStatus_reportDeviceStateListed);
            }

            break;
        }
        case EnCommunicationStatus_reportAlarming:
        {

            CAckMsgSimple __op;
            __CTeHomeIF.Decode(__op);
            DBG(
                __fline;
                cout << "EnCommunicationStatus_reportAlarming\n";
                __op.dump();
            );
            if (__op.result=="0")
            {
                //std::cout << "EnCommunicationStatus_reportCtrlDeviceStateed, SessionId:" << m_strSessionId << std::endl;
                std::cout << "reportAlarmExample Succeeded, Reason:" \
                << __op.reason \
                << std::endl \
                <<"\tNext -> EnCommunicationStatus_reportAlarmed"\
                << std::endl;

                //SetCommunicationStatus(EnCommunicationStatus_reportAlarmed);

                // 测试用5s之后上报设备状态
                SetCommunicationStatus(EnCommunicationStatus_reportDeviceStateListing);
            }
            else
            {
                std::cout << "! reportAlarmExample Failed, Reason:"
                << __op.reason
                << std::endl
                //<<"\tNext -> EnCommunicationStatus_reportAlarmed, waite "<< CMD_TIMEOUT_SMARTHOME_JSTELECOM <<" seconds."
                <<"\tNext -> EnCommunicationStatus_FactoryAuthing, start dbg again, waite "<< CMD_TIMEOUT_SMARTHOME_JSTELECOM <<" seconds."
                << std::endl;

                // 测试用
                SetCommunicationStatus(EnCommunicationStatus_reportDeviceStateListing);
            }

            break;
        }
        default:
        {
            break;
        }
    }


}

void CHttpKeepConnectorSmartHomeTelcom::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG_CODE( printf("CHttpKeepConnectorSmartHomeTelcom::OnCommand, len:%d\n", ulCmdDataLen); );


    //////////
}


void CHttpKeepConnectorSmartHomeTelcom::Update()
{
    int iRet = 0;
    switch(m_iCommunicationStatus)
    {
        case EnCommunicationStatus_Connected:
        {
            iRet = requestAuth();
            if (0 == iRet)
            {
                SetCommunicationStatus(EnCommunicationStatus_FactoryAuthing);
            }

            break;
        }
        //
        case EnCommunicationStatus_FactoryAuthing:
        {
            if (time(NULL) - m_ttLastSendData>5)
            {
                // 重新认证
                SetCommunicationStatus(EnCommunicationStatus_Connected);
                cout << "FactoryAuthing time out" << endl;
            }
            break;
        }
        case EnCommunicationStatus_FactoryAuthed:
        {
            iRet = initDeviceConfigParam();
            if (0 == iRet)
            {
                SetCommunicationStatus(EnCommunicationStatus_initDeviceConfigParaming);
            }
            break;
        }
        case EnCommunicationStatus_initDeviceConfigParaming:
        {
            if (time(NULL) - m_ttLastSendData>5)
            {
                // 设置为本步骤上一个状态，以便再次开始
                SetCommunicationStatus(EnCommunicationStatus_FactoryAuthed);
                cout << "initDeviceConfigParaming time out" << endl;
            }
            if (m_iInitDeviceConfigParamNo<(unsigned int)getDeviceNum())
            {
                //以便继续
                SetCommunicationStatus(EnCommunicationStatus_FactoryAuthed);
            }
            else
            {
                SetCommunicationStatus(EnCommunicationStatus_initDeviceConfigParamed);
            }

            break;
        }
        case EnCommunicationStatus_initDeviceConfigParamed:
        {
            iRet = reportCtrlDeviceState();

            if (0 == iRet)
            {
                SetCommunicationStatus(EnCommunicationStatus_reportCtrlDeviceStateing);
            }
            // 上传完毕
            m_iInitDeviceConfigParamNo = 0;
            deviceVec.clear();

            break;
        }
        case EnCommunicationStatus_reportCtrlDeviceStateing:
        {
            if (time(NULL) - m_ttLastSendData>5)
            {
                // 设置为本步骤上一个状态，以便再次开始
                SetCommunicationStatus(EnCommunicationStatus_initDeviceConfigParamed);
                cout << "reportCtrlDeviceStateing time out" << endl;
            }
            break;
        }
        case EnCommunicationStatus_reportCtrlDeviceStateed:
        {
            //break;
            iRet = reportDeviceStateListExample();
            if (0 == iRet)
            {
                SetCommunicationStatus(EnCommunicationStatus_reportDeviceStateListing);
            }
            break;
        }
        case EnCommunicationStatus_reportDeviceStateListing:
        {
            if (time(NULL) - m_ttLastSendData>5)
            {
                // 设置为本步骤上一个状态，以便再次开始
                SetCommunicationStatus(EnCommunicationStatus_reportCtrlDeviceStateed);
                cout << "reportDeviceStateListing time out" << endl;
            }
            break;
        }
        case EnCommunicationStatus_reportDeviceStateListed:
        {
            //SetCommunicationStatus(EnCommunicationStatus_reportDeviceStateListing);
            break;

            iRet = reportAlarmExample();
            if (0 == iRet)
            {
                SetCommunicationStatus(EnCommunicationStatus_reportAlarming);
            }
            break;
        }
        case EnCommunicationStatus_reportAlarming:
        {
            if (time(NULL) - m_ttLastSendData>5)
            {
                // 设置为本步骤上一个状态，以便再次开始
                SetCommunicationStatus(EnCommunicationStatus_reportDeviceStateListed);
                //cout << "reportAlarming time out" << endl;
            }
            break;
        }
        case EnCommunicationStatus_reportAlarmed:
        {
            // 最后一步无事可做，
            if (time(NULL) - m_ttLastSendData>5)
            {
                SetCommunicationStatus(EnCommunicationStatus_reportAlarming);
                cout << "reportAlarming again." << endl;
            }

            break;
        }
        default:
        {
            break;
        }
    }

}
int CHttpKeepConnectorSmartHomeTelcom::SetCommunicationStatus(int CommunicationStatus)
{
    CEZGuard guard(m_mutexCommunicationStatus);
    if (CommunicationStatus>EnCommunicationStatus_Waite || CommunicationStatus<EnCommunicationStatus_Initialize)
    {
        __trip;
        return -1;
    }

    m_iCommunicationStatus = CommunicationStatus;
    m_ttLastSendData = time(NULL);

    return 0;
}

int CHttpKeepConnectorSmartHomeTelcom::IniStatus()
{
    SetCommunicationStatus(EnCommunicationStatus_Connected);
    m_strSessionId = "";
    //m_ttLastSendData = 0;

    return 0;
}

int CHttpKeepConnectorSmartHomeTelcom::requestAuth()
{
    CConfigSmartHomeTelcom __cfg;
    __cfg.update();

    CTeHomeIF __CTeHomeIF;

    CAgentMsg_requestAuth Msg;
    Msg.ProductID		= __cfg.getConfig().ProductID;// JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU;
    Msg.ProductName 	= __cfg.getConfig().ProductName;// "SmartHome@Nanjing Tiansu Automation Control System Co., Ltd";
    Msg.AuthUser		= __cfg.getConfig().AuthUser;// JSTELECOM_SMARTHOME_USERNAME_TIANSU;
    Msg.AuthPassword	= __cfg.getConfig().AuthPassword;// JSTELECOM_SMARTHOME_PASSWD_TIANSU;

    __CTeHomeIF.Encode(Msg);

    SendPostStr( m_strURL_requestAuth, __CTeHomeIF.ToString());

    return 0;
}

int CHttpKeepConnectorSmartHomeTelcom::initDeviceConfigParam()
{
    CTeHomeIF __CTeHomeIF;
    int iRet;
    CAgentMsg_initDeviceConfigParam Msg;

    iRet = getDevice(Msg);

    if (iRet==0)
    {
        //Msg.dump();

        __CTeHomeIF.Encode(Msg);
        SendPostStr( m_strURL_initDeviceConfigParam, __CTeHomeIF.ToString());
    }
    else
    {// 上报完毕
        //
    }
    return 0;
}

int CHttpKeepConnectorSmartHomeTelcom::reportDeviceList(CAgentMsg_initDeviceConfigParam &Device, int UpDown)
{
    CConfigSmartHomeTelcom __cfg;
    __cfg.update();

    //UpDown 暂时没有作用
    CTeHomeIF __CTeHomeIF;

    Device.productID	= __cfg.getConfig().ProductID;
    Device.sessionId = m_strSessionId;

    __CTeHomeIF.Encode(Device);
    SendPostStr( m_strURL_initDeviceConfigParam, __CTeHomeIF.ToString());

    return 0;
}

int CHttpKeepConnectorSmartHomeTelcom::reportCtrlDeviceState()
{
    CConfigSmartHomeTelcom __cfg;
    __cfg.update();

    CTeHomeIF __CTeHomeIF;

    CAgentMsg_ReportCtrlDeviceState Msg;

    Msg.SessionId 	= m_strSessionId;
    Msg.ProductID 	=  __cfg.getConfig().ProductID;// JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU; // 厂商编号(ID)
    Msg.DeviceID 	= JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU; // 网关编号
    Msg.State 		= "1";
    char pBuff[GET_DATE_TIME_STRING_DEFAULT_STR_LEN];
    Msg.StateTime 	= get_date_time_string(pBuff, NULL);

    Msg.strIP = __cfg.getConfig().strURLRegisterServerHost;

    __CTeHomeIF.Encode(Msg);
    SendPostStr( m_strURL_reportCtrlDeviceState, __CTeHomeIF.ToString());

    return 0;
}
int CHttpKeepConnectorSmartHomeTelcom::reportDeviceStateListExample()
{
    CConfigSmartHomeTelcom __cfg;
    __cfg.update();
    //__fline;
    //printf("CHttpKeepConnectorSmartHomeTelcom::reportDeviceStateListExample\n");
    static int operate_value = 0;
    operate_value=!operate_value;

    CTeHomeIF __CTeHomeIF;

    CAgentMsg_reportDeviceStateList Msg;
    Msg.sessionId = m_strSessionId;
    Msg.productID = __cfg.getConfig().ProductID;// JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU;
    Msg.deviceID  = "None";//JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU;

    CAgentMsg_reportDeviceStateList_operate __operate;

    char pBuff[GET_DATE_TIME_STRING_DEFAULT_STR_LEN];
    __operate.value_time       = get_date_time_string(pBuff, NULL);
    __operate.operate_id      = "NONE";//TEST_OPERATE_ID_SW;
    __operate.operate_ranage  = ""; // 开关类的此无需，务必空， 不得填值
    __operate.operate_type    = "0"; //开关
    __operate.operate_value   = ezConvertToString(operate_value); // 开关
    __operate.operate_explain = TEST_OPERATE_NAME;
    ezGBKToUTF8(__operate.operate_explain);

    Msg.m_operate.push_back(__operate);

    __CTeHomeIF.Encode(Msg);
    SendPostStr( m_strURL_reportDeviceStateList, __CTeHomeIF.ToString());

    return 0;
}
int CHttpKeepConnectorSmartHomeTelcom::reportAlarmExample()
{
    static int alarm_value = 0;
    alarm_value=!alarm_value;

    CTeHomeIF __CTeHomeIF;

    CAgentMsg_reportAlarm Msg;
    Msg.sessionId = m_strSessionId;
    //Msg.productID = "productID";
    Msg.deviceID  = JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU;

    CAgentMsg_reportDeviceStateList_operate _operate;
    _operate.operate_id       = TEST_OPERATE_ID_AL;
    _operate.operate_value    = ezConvertToString(alarm_value);
    _operate.operate_ranage   = "1";
    _operate.operate_type     = "0";
    _operate.operate_explain  = "窗磁";
    char pBuff[GET_DATE_TIME_STRING_DEFAULT_STR_LEN];
    _operate.value_time       = get_date_time_string(pBuff, NULL);//"2012-5-17 10:13:52";

    Msg.m_operate.push_back(_operate);
    __CTeHomeIF.Encode(Msg);

    SendPostStr( m_strURL_reportAlarm, __CTeHomeIF.ToString());

    return 0;
}

int CHttpKeepConnectorSmartHomeTelcom::reportCtrlDeviceState(std::string strDeviceID, int OnOFF)
{
    CConfigSmartHomeTelcom __cfg;
    __cfg.update();

    CTeHomeIF __CTeHomeIF;

    CAgentMsg_ReportCtrlDeviceState Msg;

    Msg.SessionId 	= m_strSessionId;
    Msg.ProductID 	=  __cfg.getConfig().ProductID;// JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU; // 厂商编号(ID)
    Msg.DeviceID 	= strDeviceID;//JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU; // 网关编号
    Msg.State 		= (OnOFF==1?"1":"0");
    char pBuff[GET_DATE_TIME_STRING_DEFAULT_STR_LEN];
    Msg.StateTime 	= get_date_time_string(pBuff, NULL);

    Msg.strIP = __cfg.getConfig().strURLRegisterServerHost;

    __CTeHomeIF.Encode(Msg);
    SendPostStr( m_strURL_reportCtrlDeviceState, __CTeHomeIF.ToString());

    return 0;
}

    // 上报设备运行态信息
int CHttpKeepConnectorSmartHomeTelcom::reportDeviceStateList(std::string strDeviceID	\
                    , std::string strOperate_id	\
                    , std::string strOperate_value	\
                    , std::string strOperate_ranage	\
                    , std::string strOperate_type	\
                    , std::string strOperate_explain	\
                   )
{
    CConfigSmartHomeTelcom __cfg;
    __cfg.update();
    //__fline;
    //printf("CHttpKeepConnectorSmartHomeTelcom::reportDeviceStateListExample\n");
    static int operate_value = 0;
    operate_value=!operate_value;

    CTeHomeIF __CTeHomeIF;

    CAgentMsg_reportDeviceStateList Msg;
    Msg.sessionId = m_strSessionId;
    Msg.productID = __cfg.getConfig().ProductID;// JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU;
    Msg.deviceID  = strDeviceID;//JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU;

    CAgentMsg_reportDeviceStateList_operate __operate;

    char pBuff[GET_DATE_TIME_STRING_DEFAULT_STR_LEN];
    __operate.value_time       = get_date_time_string(pBuff, NULL);
    __operate.operate_id      = strOperate_id;//TEST_OPERATE_ID_SW;
    __operate.operate_ranage  = strOperate_ranage; // 开关类的此无需，务必空， 不得填值
    __operate.operate_type    = strOperate_type; //开关
    __operate.operate_value   = strOperate_value;//ezConvertToString(operate_value); // 开关
    __operate.operate_explain = strOperate_explain;
    ezGBKToUTF8(__operate.operate_explain);

    Msg.m_operate.push_back(__operate);

    __CTeHomeIF.Encode(Msg);
    SendPostStr( m_strURL_reportDeviceStateList, __CTeHomeIF.ToString());

    return 0;
}
	
int CHttpKeepConnectorSmartHomeTelcom::reportAlarm(std::string strDeviceID	\
        , std::string strOperate_id	\
        , std::string strOperate_value	\
        , std::string strOperate_ranage	\
        , std::string strOperate_type	\
        , std::string strOperate_explain	\
                                                  )
{
    static int alarm_value = 0;
    alarm_value=!alarm_value;

    CTeHomeIF __CTeHomeIF;

    CAgentMsg_reportAlarm Msg;
    Msg.sessionId = m_strSessionId;
    //Msg.productID = "productID";
    Msg.deviceID  = strDeviceID; //JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU;

    CAgentMsg_reportDeviceStateList_operate _operate;
    _operate.operate_id       = strOperate_id;
    _operate.operate_value    = strOperate_value;//ezConvertToString(alarm_value);
    _operate.operate_ranage   = strOperate_ranage;
    _operate.operate_type     = strOperate_type;
    _operate.operate_explain  = strOperate_explain;
    char pBuff[GET_DATE_TIME_STRING_DEFAULT_STR_LEN];
    _operate.value_time       = get_date_time_string(pBuff, NULL);//"2012-5-17 10:13:52";

    Msg.m_operate.push_back(_operate);
    __CTeHomeIF.Encode(Msg);

    SendPostStr( m_strURL_reportAlarm, __CTeHomeIF.ToString());
    return 0;
}

ssize_t CHttpKeepConnectorSmartHomeTelcom::SendPostStr(const std::string &page, const std::string &poststr)
{
    std::stringstream ssPost;

    ssPost
    <<"POST "<<page<<" HTTP/1.1\r\n"
    << "User-Agent: " << HTTP_USER_AGENT << "\r\n"
    << "Host: " << GetRemoteAddress() << ":" << GetRemotePort() << "\r\n"
    << "Accept: */*\r\n";

    if (m_strSessionId.size()>0)
    {
        ssPost
        << "Cookie: JSESSIONID="<<m_strSessionId<<"\r\n";
    }

    ssPost
    // 下列在自有平台上失败
    //<< "Content-Type: application/x-www-form-urlencoded\r\n"
    << "Content-Type: text/html;charset=utf-8\r\n"
    << "Content-length: "<<poststr.size()+2<<"\r\n\r\n"
    << "["<<poststr<<"]";

    DBG(
        __fline;
        cout << endl<< ssPost.str() << endl;
    );
    Send(ssPost.str());
    //DBG(
    cout <<  "*** ["<<page<<"] ***\n"<<SND_PROMPT<<"Send("<<poststr.size()+2<<") JSON:\n"<<TOKEN<<"\n["<<poststr<<"]\n"<<TOKEN<<"\n";
    //);

    //LOG4CPLUS_INFO(LOG_SMARTHOMEJSTELCOM, "\n*** ["<<page<<"] ***\n"<<SND_PROMPT<<"Send("<<poststr.size()+2<<") JSON:\n"<<TOKEN<<"\n["<<poststr<<"]\n"<<TOKEN<<"\n");

    return ssPost.str().size();
}
#include "app.h"

//查找指定网关或主机下的设备。
//return  设备数量
int loadDeviceById(std::vector<CAgentMsg_initDeviceConfigParam> &deviceVec, char *deviceId)
{
    int deviceNum = 0;
    int res = 0;
    char queryStr[1000];
    MYSQL_RES *res_ptr = NULL;
    MYSQL_ROW sqlrow;
    //get a mysql connection from pool.
    EasyMysqlConn easyConn;
    MysqlConn conn = easyConn.getConnection();

    MYSQL *mysql = conn.mysql;
    if (NULL == mysql)
    {
        LOG4CPLUS_ERROR(LOG_DB, "pool_getConn");
        return -1;
    }
    sprintf(queryStr, FIND_BY_ID_SQL, deviceId);
    res = mysql_query(mysql, queryStr);
    if(res)
    {
        LOG4CPLUS_ERROR(LOG_DB, "query error");
    }
    else
    {
        res_ptr = mysql_use_result(mysql);
        if(res_ptr)
        {
            std::stringstream ssPost;
            ssPost
            <<"楼层\t|房间\t|设备序号\t|设备名\t|设备类型\t|用户名\t|\n";
            while((sqlrow = mysql_fetch_row(res_ptr)))
            {
                ssPost
                <<sqlrow[0]<<sqlrow[1]<<sqlrow[2]<<sqlrow[3]<<sqlrow[4]<<sqlrow[5];
                CAgentMsg_initDeviceConfigParam __device;
                CAgentMsg_initDeviceConfigParam_Seat_deploy __deploy;
                CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate __operate;

                // 设备
                __operate.operate_id      = sqlrow[2];
                __operate.operate_ranage  = ""; // 开关类的此无需，务必空， 不得填值
                __operate.operate_type    = "0"; //开关
                __operate.operate_value   = "2"; // 关
                __operate.operate_explain = "大灯";
                ezGBKToUTF8(__operate.operate_explain);
                // 物品
                __deploy.deploy_name = sqlrow[3];
                ezGBKToUTF8(__deploy.deploy_name);

                __deploy.deploy_type ="2" ;//设备的类型，1：安防设备；2：家居设备
                __deploy.deviceType  = sqlrow[4];
                __deploy.m_Operate.push_back(__operate);

                __device.m_Seat.m_deploy.push_back(__deploy);
                __device.m_Seat.seat_name = sqlrow[0];
                __device.m_Seat.seat_name +=  sqlrow[1];

                ezGBKToUTF8(__device.m_Seat.seat_name);

                __device.requestMark = "add";
                __device.sessionId 	= "";
                __device.productID 	= "";
                __device.userID 		= sqlrow[5];//t_jstelcom_user.userId
                __device.deviceID 	= deviceId; //

                //加入设备列表
                deviceVec.push_back(__device);
            }
            mysql_free_result(res_ptr);
            deviceNum = deviceVec.size();
        }
    }
    return deviceNum;
}

//查找所有网关或主机下的设备。
//return设备数量
int loadAllDevices(std::vector<CAgentMsg_initDeviceConfigParam> &deviceVec)
{
    int deviceNum = 0;
    int res = 0;
    MYSQL_RES *res_ptr = NULL;
    MYSQL_ROW sqlrow;
    //cout << "in loadAllDevices()\n";
    //get a mysql connection from pool.
    EasyMysqlConn easyConn;
    MysqlConn conn = easyConn.getConnection();
    MYSQL *mysql = conn.mysql;
    if (NULL == mysql)
    {
        LOG4CPLUS_ERROR(LOG_DB, "pool_getConn");
        return NULL;
    }

    res = mysql_query(mysql, FIND_ALL_SQL);
    if(res)
    {
        LOG4CPLUS_ERROR(LOG_DB, "query error");
    }
    else
    {
        res_ptr = mysql_store_result(mysql);
        if(res_ptr)
        {

            cout
            <<"floor\t|room\t|deviceSN\t|dataPoint\t|deviceName\t|deviceType\t|userName\t|deviceId\t|\n";
            cout << "result counter = " << mysql_num_rows(res_ptr) << "\n";
            while((sqlrow = mysql_fetch_row(res_ptr)))
            {
                printf("%s,%s,%s,%s,%s,%s,%s,%s\n", sqlrow[0],sqlrow[1],sqlrow[2],sqlrow[3],sqlrow[4],sqlrow[5],sqlrow[6],sqlrow[7]);
                CAgentMsg_initDeviceConfigParam __device;
                CAgentMsg_initDeviceConfigParam_Seat_deploy __deploy;
                CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate __operate;

                // 设备
                __operate.operate_id      = sqlrow[3];
                __operate.operate_ranage  = ""; // 开关类的此无需，务必空， 不得填值
                __operate.operate_type    = "0"; //开关
                __operate.operate_value   = "2"; // 关
                __operate.operate_explain = sqlrow[4];
                ezGBKToUTF8(__operate.operate_explain);
                // 物品
                __deploy.deploy_name = sqlrow[4];
                ezGBKToUTF8(__deploy.deploy_name);

				std::string deploy_type = sqlrow[5];
				if(deploy_type == "4")
					deploy_type = "1";
				else
					deploy_type = "2";
				
                __deploy.deploy_type =deploy_type ;//设备的类型，1：安防设备；2：家居设备

				std::string deviceType = sqlrow[5];
				if(deviceType == "2")
					deviceType = "5";
				else if(deviceType == "3")
					deviceType = "2";
				else if(deviceType == "4")
					deviceType = "8";
				else if(deviceType == "6")
					deviceType = "3";
				else if(deviceType == "8")
					deviceType = "7";
				else
					deviceType = "0";

                __deploy.deviceType  = deviceType;
                __deploy.m_Operate.push_back(__operate);

                __device.m_Seat.m_deploy.push_back(__deploy);
                __device.m_Seat.seat_name = sqlrow[0];
                __device.m_Seat.seat_name +=  sqlrow[1];

                ezGBKToUTF8(__device.m_Seat.seat_name);

                __device.requestMark = "add";
                __device.sessionId 	= "";
                __device.productID 	= "";
                __device.userID 		= sqlrow[6];
                __device.deviceID 	= sqlrow[7]; //

                //加入设备列表
                deviceVec.push_back(__device);
            }
            mysql_free_result(res_ptr);
            deviceNum = deviceVec.size();
        }
    }
    return deviceNum;
}
// 单元测试用数据
//#define USE_UNITTEST_DATA
ssize_t CHttpKeepConnectorSmartHomeTelcom::getDeviceNum(int reload)
{
    //printf("in getDeviceNum\n");
#ifdef USE_UNITTEST_DATA
    return 3;
#else

    if(reload || deviceVec.size() <= 0)
    {
        m_iInitDeviceConfigParamNo = 0;
        deviceVec.clear();
        return loadAllDevices(deviceVec);
    }
    else
        return deviceVec.size();
#endif// USE_UNITTEST_DATA
}
int CHttpKeepConnectorSmartHomeTelcom::getDevice( CAgentMsg_initDeviceConfigParam &DeviceCfg, int No)
{
    // 测试数据， 从1开始
#ifdef USE_UNITTEST_DATA
    if (No==-1)
    {
        m_iInitDeviceConfigParamNo++;
    }
    else if (No>0)
    {
        m_iInitDeviceConfigParamNo = No;
    }
    DBG(
        __fline;
        cout << "m_iInitDeviceConfigParamNo:"<< m_iInitDeviceConfigParamNo <<endl;
    );
    // switch
    if (m_iInitDeviceConfigParamNo==1)
    {
        CAgentMsg_initDeviceConfigParam_Seat_deploy __deploy;
        CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate __operate;
        // 设备
        __operate.operate_id      = TEST_OPERATE_ID_SW;
        __operate.operate_ranage  = ""; // 开关类的此无需，务必空， 不得填值
        __operate.operate_type    = "0"; //开关
        __operate.operate_value   = "2"; // 关
        __operate.operate_explain = "大灯-exp";
        ezGBKToUTF8(__operate.operate_explain);
        // 物品
        __deploy.deploy_name = "大灯-deploy_name";
        ezGBKToUTF8(__deploy.deploy_name);

        __deploy.deploy_type ="2" ;//设备的类型，1：安防设备；2：家居设备
        __deploy.deviceType  ="1"  ;
        __deploy.m_Operate.push_back(__operate);

        DeviceCfg.m_Seat.m_deploy.push_back(__deploy);
        DeviceCfg.m_Seat.seat_name = "健身房- seat_name";
        ezGBKToUTF8(DeviceCfg.m_Seat.seat_name);

        DeviceCfg.requestMark = "add";
        DeviceCfg.sessionId 	= m_strSessionId;
        DeviceCfg.productID 	= JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU;
        DeviceCfg.userID 		= "tiansuTest";
        DeviceCfg.deviceID 	= JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU; //
    }
    // dimm switch
    else if (m_iInitDeviceConfigParamNo==2)
    {
        CAgentMsg_initDeviceConfigParam_Seat_deploy __deploy;
        CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate __operate;
        // 设备
        __operate.operate_id      = TEST_OPERATE_ID_SW_TIAOG;
        __operate.operate_ranage  = "1-100"; // 开关类的此无需，务必空， 不得填值
        __operate.operate_type    = "1"; //开关
        __operate.operate_value   = "2"; // 关
        __operate.operate_explain = "调光灯";
        ezGBKToUTF8(__operate.operate_explain);
        // 物品
        __deploy.deploy_name = "调光灯";
        ezGBKToUTF8(__deploy.deploy_name);

        __deploy.deploy_type ="2" ;//设备的类型，1：安防设备；2：家居设备
        __deploy.deviceType  ="1"  ;
        __deploy.m_Operate.push_back(__operate);

        DeviceCfg.m_Seat.m_deploy.push_back(__deploy);
        DeviceCfg.m_Seat.seat_name = "健身房";
        ezGBKToUTF8(DeviceCfg.m_Seat.seat_name);

        DeviceCfg.requestMark = "add";
        DeviceCfg.sessionId 	= m_strSessionId;
        DeviceCfg.productID 	= JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU;
        DeviceCfg.userID 		= "tiansuTest";
        DeviceCfg.deviceID 	= JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU; //
    }
    // alarm
    else if (m_iInitDeviceConfigParamNo==3)
    {
        CAgentMsg_initDeviceConfigParam_Seat_deploy __deploy;
        CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate __operate;   // 设备
        __operate.operate_id      = TEST_OPERATE_ID_AL;
        __operate.operate_ranage  = ""; // 开关类的此无需，务必空， 不得填值
        __operate.operate_type    = "0"; //开关
        __operate.operate_value   = "2"; // 关
        __operate.operate_explain = "窗磁";
        ezGBKToUTF8(__operate.operate_explain);
        // 物品
        __deploy.deploy_name = "窗磁";
        ezGBKToUTF8(__deploy.deploy_name);

        __deploy.deploy_type ="1" ;//设备的类型，1：安防设备；2：家居设备
        __deploy.deviceType  ="0"  ;// 0：其它
        __deploy.m_Operate.push_back(__operate);

        DeviceCfg.m_Seat.m_deploy.push_back(__deploy);
        DeviceCfg.m_Seat.seat_name = "健身房";
        ezGBKToUTF8(DeviceCfg.m_Seat.seat_name);

        DeviceCfg.requestMark = "add";
        DeviceCfg.sessionId 	= m_strSessionId;
        DeviceCfg.productID 	= JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU;
        DeviceCfg.userID 		= "tiansuTest";
        DeviceCfg.deviceID 	= JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU; //
    }
    else
    {
        return -1;
    }
#else

    if (m_iInitDeviceConfigParamNo==0)
    {
        int iDevNum = getDeviceNum(1);
        cout << "if (m_iInitDeviceConfigParamNo==0)" << "iDevNum="<<iDevNum<<"\n";
        if (iDevNum < 1)
        {
            LOG4CPLUS_ERROR(LOG_SMARTHOMEJSTELCOM, "no device");
            return -1;
        }
    }

    if (No==-1)
    {
        cout << "if (No==-1)" << "m_iInitDeviceConfigParamNo="<<m_iInitDeviceConfigParamNo<<"\n";
        if(m_iInitDeviceConfigParamNo >= deviceVec.size())
        {
            m_iInitDeviceConfigParamNo = 0;
            deviceVec.clear();

            LOG4CPLUS_INFO(LOG_SMARTHOMEJSTELCOM, "no more device");

            return -1;
        }
    }
    else if (No>0)
    {
        m_iInitDeviceConfigParamNo = No;
        cout << "else if (No>0)" << "m_iInitDeviceConfigParamNo="<<m_iInitDeviceConfigParamNo<<"\n";
        if(m_iInitDeviceConfigParamNo >= deviceVec.size())
        {
            return -2;
        }
    }
    __fline;
    cout << "m_iInitDeviceConfigParamNo:"<< m_iInitDeviceConfigParamNo <<endl;

    CAgentMsg_initDeviceConfigParam device = deviceVec[m_iInitDeviceConfigParamNo];
    DeviceCfg.m_Seat = device.m_Seat;
    DeviceCfg.deviceID = device.deviceID;
    DeviceCfg.productID	= JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU;
    DeviceCfg.requestMark = device.requestMark;
    DeviceCfg.sessionId = m_strSessionId;
    DeviceCfg.userID = device.userID;

    m_iInitDeviceConfigParamNo++;

#endif
    //
    return 0;
}

