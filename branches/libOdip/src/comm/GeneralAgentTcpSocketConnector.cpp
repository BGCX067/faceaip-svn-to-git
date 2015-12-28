/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocketServer.h - _explain_
 *
 * Copyright (C) 2011 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocketServer.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -Tcp客户端实例-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "CommonInclude.h"
#include <iostream>
#include <string.h>
#include <assert.h>
#include "Def_ODIP.h"
#include "base64.h"
#include "ezBuffer.h"
#include "des.h"
#include "md5.h"
#include "str_opr.h"
#include "ez_system_api.h"

#include "GeneralAgentTcpSocketConnector.h"

#define ENCRYPT_DES_KEY "shouldtodo"
#define UNAME_PWD_SPLITER "&&"

CGeneralAgentTcpSocketConnector::CGeneralAgentTcpSocketConnector(ISocketHandler& h
        , std::string strSocketName
        , time_t Timeout
        , bool bReconn)
        : CGeneralAgentTcpSocket(h, strSocketName)
        , m_b_connected(false)
        , m_strSocketName(strSocketName)
        , m_ttTimeout(Timeout)
        , m_bReconn(bReconn)
{
    // initial connection timeout setting
    SetConnectTimeout(Timeout);

    SetDeleteByHandler();

    SetSockName(strSocketName);

    m_ttConnectTimeOut = Timeout;
    m_bReConnect = bReconn;

    m_strUName = "";
    m_strUPwd = "";

    m_enOdipStatus = enAppLogicStatus_Begin;

    m_tOnOdip = time(NULL);
    m_RealDataCallBack = NULL;

    m_ttOdipTimeOut = 2;

    m_iChannel = -1;

}

void CGeneralAgentTcpSocketConnector::ReNew()
{

    CEZLock guard(m_MutexRealPlayStatus);

    m_enOdipStatus = enAppLogicStatus_Begin;

    m_tOnOdip = 0;//time(NULL);

    for (size_t ii=0; ii<m_RealPlayStatus.size(); ii++)
    {
        m_RealPlayStatus[ii].ttReqed = 0;
        m_RealPlayStatus[ii].ttLastOndata = 0;
    }

    m_iChannel = -1;
}
CGeneralAgentTcpSocketConnector::~CGeneralAgentTcpSocketConnector()
{}

CGeneralAgentTcpSocketConnector *CGeneralAgentTcpSocketConnector::Reconnect()
{
    if (IsReConnect())
    {
        std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
        // same as handler
        CGeneralAgentTcpSocketConnector *p = new CGeneralAgentTcpSocketConnector(Handler(), m_strSocketName, m_ttTimeout, m_bReconn);
        assert(p);


        p->m_RealPlayStatus = this->m_RealPlayStatus;

        p->SetAccount(m_strUName, m_strUPwd);

        p->SetRealDataCallBack(m_RealDataCallBack);
        p->m_RealDataCallBack = this->m_RealDataCallBack;

        //__fline;
        //printf("%x %x\n", p->m_RealDataCallBack, this->m_RealDataCallBack);

        p -> SetDeleteByHandler();
        p -> Open(*ad);
        Handler().Add(p);
        return p;
    }
    else
    {
        return NULL;
    }
}


void CGeneralAgentTcpSocketConnector::OnConnectFailed()
{
    CGeneralAgentTcpSocketConnector *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CGeneralAgentTcpSocketConnector::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}
bool CGeneralAgentTcpSocketConnector::IsReConnect()
{
    return m_bReConnect;
}
void CGeneralAgentTcpSocketConnector::SetAccount(std::string strUName, std::string strUPwd)
{
    m_strUName = strUName;
    m_strUPwd = strUPwd;
}

void CGeneralAgentTcpSocketConnector::SetRealDataCallBack(void( *RealDataCallBack) (char *pBuffer, int dwBufSize, int ch, int stream))
{
    m_RealDataCallBack = RealDataCallBack;
}


void CGeneralAgentTcpSocketConnector::SetCloseAndDelete()
{
    CGeneralAgentTcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CGeneralAgentTcpSocketConnector::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    // do not modify, wujj 2012-4-9 15:31:44 end

    ////////// add your code here
    ReNew();

    DBG_CODE(
        printf("CGeneralAgentTcpSocketConnector::OnConnect()\n");
    );

    m_enOdipStatus = enAppLogicStatus_Begin;
}

bool CGeneralAgentTcpSocketConnector::IsLogined()
{
    return m_enOdipStatus>=enAppLogicStatus_Logined;

}
int CGeneralAgentTcpSocketConnector::GetRemoteVersion(char *pVer, unsigned int bufLen)
{
    if (bufLen<=m_strRemoteVersion.length())
    {
        return -1;
    }

    strcpy(pVer, m_strRemoteVersion.c_str());

    return m_strRemoteVersion.length();
}

int CGeneralAgentTcpSocketConnector::Update()
{
    time_t ttNow = time(NULL);
    int iRet = -1;

    if(m_b_connected==false)
    {
        return 0;
    }
    // 连接之后登录
    if (IsConnected()
        && m_enOdipStatus<enAppLogicStatus_Logining)
    {
        iRet = Odip_ReqLogin();

        DBG_CODE(
            __fline;
            printf("Login ....\n");
        );
        return iRet;
    }

    // 登录超时
    if (m_enOdipStatus==enAppLogicStatus_Logining && (ttNow-m_tOnOdip>m_ttOdipTimeOut))
    {
        m_enOdipStatus=enAppLogicStatus_Begin;

        __fline;
        printf("Login timeout.\n");

        return iRet;
    }

    if (ttNow-m_tOnOdip > m_ttConnectTimeOut)
    {
        m_enOdipStatus=enAppLogicStatus_Begin;

        __fline;
        printf("m_tOnOdip timeout.\n");

        return iRet;
    }

    if (m_enOdipStatus >= enAppLogicStatus_Logined)
    {

        if (m_strRemoteVersion.size()<1)
        {
            return Odip_ReqRemoteInfo(0x08);
        }

        //__fline;
        //printf("m_RealPlayStatus.size():%d\n", m_RealPlayStatus.size());
        for (size_t ii=0; ii<m_RealPlayStatus.size(); ii++)
        {
            //__fline;
            //printf("m_RealPlayStatus.size(%d):%d\n", ii, m_RealPlayStatus[ii].ttReqed);

            // 已经发出的不要再发了；
            //printf("req:%ld, %ld, %ld\n", m_RealPlayStatus[ii].ttReqed, time(NULL), m_RealPlayStatus[ii].ttLastOndata);

            // todo 根据收到的数据情况（ ttLastOndata ）可以再发
            if(m_RealPlayStatus[ii].ttReqed<=0
               || (m_RealPlayStatus[ii].ttReqed>0
                   && time(NULL)-m_RealPlayStatus[ii].ttLastOndata>5))
            {
                //__trip;
                Odip_ReqRealPlay(m_RealPlayStatus[ii]);

            }
            else
            {
                //__trip;
            }
        }
    }
    //    __fline;
    //    printf("m_enOdipStatus:%d ttNow-m_tOnOdip:%ld, m_ttOdipTimeOut:%ld\n", m_enOdipStatus, ttNow-m_tOnOdip, m_ttOdipTimeOut);

    // 心跳
    if (m_enOdipStatus>=enAppLogicStatus_Logined
        &&(ttNow-m_tOnOdip)>(m_ttConnectTimeOut/2.0-0.5))
    {
        DBG_CODE(
            __fline;
            printf("KeepLive , Odip_ReqQueryStat\n");
        );

        Odip_ReqQueryStat();

        //Odip_ReqRemoteInfo
    }

#if 0
    if (0)//(m_enOdipStatus>=enAppLogicStatus_Logined && m_RealPlayStatus.size()<1)
    {
        // 请求实时视频
        MON_STATUS_T __mon;
        __mon.Channel = 1;
        __mon.Stream = 0;
        __mon.OnOff = 1;
        Odip_ReqRealPlay(__mon);
    }
#endif
    return 0;
}

int CGeneralAgentTcpSocketConnector::GetOdipStatus()
{
    return m_enOdipStatus;
}
int CGeneralAgentTcpSocketConnector::Odip_ReqLogin()
{
    ODIP_HEAD_T __header_dvr_ip;
    ZERO_ODIP_HEAD_T(&__header_dvr_ip);
    std::string strUname = m_strUName;
    std::string strPwd = m_strUPwd;

    char bufNameEncy[128];
    int lenNameEncy;
    char bufPwdEncy[128];
    int lenPwdEncy;

    lenNameEncy = DesEncrypt(bufNameEncy, (char *)strUname.c_str(), strUname.length(), ENCRYPT_DES_KEY, strlen(ENCRYPT_DES_KEY));
    lenPwdEncy = DesEncrypt(bufPwdEncy, (char *)strPwd.c_str(), strPwd.length(), ENCRYPT_DES_KEY, strlen(ENCRYPT_DES_KEY));


    char bufNameB64[256];
    int lenNameB64;
    char bufPwdB64[256];
    int lenPwdB64;

    lenNameB64 = ez_base64encode_len(lenNameEncy);
    lenPwdB64 = ez_base64encode_len(lenPwdEncy);

    ez_base64encode(bufNameB64, bufNameEncy, lenNameEncy);
    ez_base64encode(bufPwdB64, bufPwdEncy, lenPwdEncy);

    CezBuffer __buffer;

    DBG_CODE(
        __fline;
        std::cout <<strUname<< " " << strPwd<< " " <<bufNameB64<< " " << bufPwdB64<< std::endl;
    );
    __header_dvr_ip.dvrip.dvrip_cmd = REQ_USER_LOGIN;
    __header_dvr_ip.dvrip.dvrip_extlen = strlen(bufNameB64)+strlen(UNAME_PWD_SPLITER)+strlen(bufPwdB64);

    __buffer.Append((unsigned char *)&__header_dvr_ip, sizeof(__header_dvr_ip));
    __buffer.Append((unsigned char *)bufNameB64, strlen(bufNameB64));
    __buffer.Append((unsigned char *)UNAME_PWD_SPLITER, strlen(UNAME_PWD_SPLITER));
    __buffer.Append((unsigned char *)bufPwdB64, strlen(bufPwdB64));

    SendBuf((char *)__buffer.Buf(), __buffer.Size());

    m_enOdipStatus = enAppLogicStatus_Logining;

    return 0;
}

int CGeneralAgentTcpSocketConnector::Odip_ReqQueryStat()
{
    if (!IsLogined())
    {
        return -100;
    }
	
    ODIP_HEAD_T dvripHead;
    ZERO_ODIP_HEAD_T(&dvripHead);

    CezBuffer __buffer;

    dvripHead.dvrip.dvrip_cmd = REQ_STAT_QUERY;

    //char *strMonitor = "0||0||0||0||1||0||RemoteIP||RemotePort&&0||10||0||0||1||0||RemoteIP||RemotePort";
    //dvripHead.dvrip.dvrip_extlen = strlen(strMonitor);

    __buffer.Append((unsigned char *)&dvripHead, ODIP_HEAD_T_SIZE);
    //__buffer.Append((unsigned char *)strMonitor, dvripHead.dvrip.dvrip_extlen);

    SendBuf((char *)__buffer.Buf(), __buffer.Size());

    return 0;

}

int CGeneralAgentTcpSocketConnector::Odip_ReqRealPlay(MON_STATUS_T &MonData)
{
    if (!IsLogined())
    {
        return -100;
    }

    CEZLock guard(m_MutexRealPlayStatus);

    ODIP_HEAD_T dvripHead;
    ZERO_ODIP_HEAD_T(&dvripHead);

    CezBuffer __buffer;

    std::string strMon = "";

    MonData.Struct2String(strMon);
    //strMon = "0||0||0||0||1||0||RemoteIP||RemotePort&&0||1||0||0||1||0||RemoteIP||RemotePort";
    // 0||1||0||-1||-1||-1||N/A||-1

    dvripHead.dvrip.dvrip_cmd = REQ_CHAN_SWTCH;

    dvripHead.dvrip.dvrip_p[18] = 99; // string mode

    //std::string strMon
    dvripHead.dvrip.dvrip_extlen = strMon.size();
    DBG_CODE(
        __fline;
        printf("Odip_ReqRealPlay(%d):%s\n", dvripHead.dvrip.dvrip_extlen, strMon.c_str());
    );
    __buffer.Append((unsigned char *)&dvripHead, ODIP_HEAD_T_SIZE);
    __buffer.Append((unsigned char *)strMon.c_str(), dvripHead.dvrip.dvrip_extlen);
    DBG_CODE(
        __fline;
        printf("Odip_ReqRealPlay:%s\n", strMon.c_str());
    );
    SendBuf((char *)__buffer.Buf(), __buffer.Size());

    DBG_CODE(
        __fline;
        printf("Odip_ReqRealPlay:%s\n", strMon.c_str());
    );

    /////////////////
    size_t ii;
    for (ii=0; ii<m_RealPlayStatus.size(); ii++)
    {
        if (m_RealPlayStatus[ii] == MonData)
        {
            break;
        }
    }

    // 请求发出时间
    MonData.ttReqed=time(NULL);
    // insert
    if (ii >= m_RealPlayStatus.size() )
    {
        m_RealPlayStatus.push_back(MonData);
    }// update
    else
    {
        m_RealPlayStatus[ii] = MonData;
    }
    ////////////////////

    return ii;
}

int CGeneralAgentTcpSocketConnector::Odip_ReqRealPlay(std::vector<MON_STATUS_T> &RealPlayStatus)
{
    if (!IsLogined())
    {
        return -100;
    }

    int iRet = 0;

    for (size_t ii=0; ii<RealPlayStatus.size(); ii++)
    {
        iRet = Odip_ReqRealPlay(RealPlayStatus[ii]);

        if (iRet<0)
        {
            return iRet;
        }
    }

    return 0;
}

int CGeneralAgentTcpSocketConnector::Odip_ReqStopRealPlay(int iHandle)
{
    if (!IsLogined())
    {
        return -100;
    }

    if (iHandle >= (int)m_RealPlayStatus.size() )
    {
        __trip;
        return -1;
    }

    CEZLock guard(m_MutexRealPlayStatus);

    MON_STATUS_T __mSt = m_RealPlayStatus[iHandle];

    __mSt.OnOff = 0;
    __mSt.ttReqed=time(NULL);

    ODIP_HEAD_T dvripHead;
    ZERO_ODIP_HEAD_T(&dvripHead);

    CezBuffer __buffer;

    std::string strMon = "";

    __mSt.Struct2String(strMon);
    //strMon = "0||0||0||0||1||0||RemoteIP||RemotePort&&0||1||0||0||1||0||RemoteIP||RemotePort";
    // 0||1||0||-1||-1||-1||N/A||-1

    dvripHead.dvrip.dvrip_cmd = REQ_CHAN_SWTCH;

    dvripHead.dvrip.dvrip_p[18] = 99; // string mode

    //std::string strMon
    dvripHead.dvrip.dvrip_extlen = strMon.size();
    DBG_CODE(
        __fline;
        printf("Odip_ReqRealPlay(%d):%s\n", dvripHead.dvrip.dvrip_extlen, strMon.c_str());
    );
    __buffer.Append((unsigned char *)&dvripHead, ODIP_HEAD_T_SIZE);
    __buffer.Append((unsigned char *)strMon.c_str(), dvripHead.dvrip.dvrip_extlen);
    DBG_CODE(
        __fline;
        printf("Odip_ReqRealPlay:%s\n", strMon.c_str());
    );
    SendBuf((char *)__buffer.Buf(), __buffer.Size());

    DBG_CODE(
        __fline;
        printf("Odip_ReqRealPlay:%s\n", strMon.c_str());
    );

    /////////////////

    m_RealPlayStatus[iHandle] = __mSt;
    ////////////////////

    return iHandle;
}

int CGeneralAgentTcpSocketConnector::Odip_RemoteRecordCtrl(unsigned int BitMASK)
{
    if (!IsLogined())
    {
        return -100;
    }

    ODIP_HEAD_T __header_dvr_ip;
    ZERO_ODIP_HEAD_T(&__header_dvr_ip);

    __header_dvr_ip.dvrip.dvrip_cmd = REQ_CTRL_RECRD;
    //__header_dvr_ip.dvrip.dvrip_p[0] = 0;
    for(int ii=0; ii<16; ii++)
    {
        __header_dvr_ip.dvrip.dvrip_p[ii] = (BitMASK >> ii) & 0x1;
    }


    SendBuf((char *)&__header_dvr_ip, ODIP_HEAD_T_SIZE);

    //printf("%d -- Odip_RemoteRecordCtrl 0x%0x\n", time(NULL), BitMASK);
    return 0;
}


int CGeneralAgentTcpSocketConnector::Odip_ReqRemoteInfo(int iInfo)
{
    if (!IsLogined())
    {
        return -100;
    }

    ODIP_HEAD_T __header_dvr_ip;
    ZERO_ODIP_HEAD_T(&__header_dvr_ip);

    __header_dvr_ip.dvrip.dvrip_cmd = REQ_INFO_SYSTM;

    __header_dvr_ip.dvrip.dvrip_p[0] = iInfo;

    SendBuf((char *)&__header_dvr_ip, ODIP_HEAD_T_SIZE);

    //printf("%d -- Odip_RemoteRecordCtrl 0x%0x\n", time(NULL), BitMASK);
    return 0;

}


int CGeneralAgentTcpSocketConnector::Odip_ReqSnap(MON_STATUS_T &MonData)
{
    if (!IsLogined())
    {
        return -100;
    }

    ODIP_HEAD_T dvripHead;
    ZERO_ODIP_HEAD_T(&dvripHead);

    dvripHead.dvrip.dvrip_cmd = REQ_CHAN_SWTCH;

    CezBuffer __buffer;

    struct _snap_param_new __snap;
    memset(&__snap, 0, sizeof(struct _snap_param_new));
    dvripHead.dvrip.dvrip_p[20] = 10;
    dvripHead.dvrip.dvrip_extlen = sizeof(__snap);
    __snap.Channel = MonData.Channel;
    __snap.mode = 0;  // 0 自动关闭， 1
    __snap.Opr = 1;//times; // 0关闭  1 开启

    __buffer.Append((unsigned char *)&dvripHead, ODIP_HEAD_T_SIZE);
    __buffer.Append((unsigned char *)&__snap, sizeof(struct _snap_param_new));


    SendBuf((char *)__buffer.Buf(), __buffer.Size());

    //printf("%d -- Odip_RemoteRecordCtrl 0x%0x\n", time(NULL), BitMASK);

    return 0;

}

void CGeneralAgentTcpSocketConnector::OnReconnect()
{
    DBG_CODE(
        printf("CGeneralAgentTcpSocketConnector::OnReconnect()\n");
    );
}

void CGeneralAgentTcpSocketConnector::OnDisconnect()
{
    DBG_CODE(
        printf("CGeneralAgentTcpSocketConnector::OnDisconnect()\n");
    );
    m_enOdipStatus=enAppLogicStatus_Begin;
}

void CGeneralAgentTcpSocketConnector::OnConnectTimeout()
{
    printf("CGeneralAgentTcpSocketConnector::OnConnectTimeout()\n");
}

int CGeneralAgentTcpSocketConnector::FindRealPlayStatus(int Channel, int Stream)
{
    size_t ii;

    for (ii=0; ii<m_RealPlayStatus.size(); ii++)
    {
        if (m_RealPlayStatus[ii].Channel == Channel
            && m_RealPlayStatus[ii].Stream == Stream)
        {
            return ii;
        }
    }

    return -1;
}

#ifdef INTERNAL_ODIP_DATA_PARSE
// 通用头 pCmd
// 数据 pData， 长度见 pCmd->u8DataLen
void CGeneralAgentTcpSocketConnector::OnCommand_ODIP(const ODIP_HEAD_T *pDvrip, const unsigned char *pData)
{
    CGeneralAgentTcpSocket::OnCommand_ODIP(pDvrip, pData);
    time(&m_tOnOdip);

    switch (pDvrip->dvrip.dvrip_cmd)
    {
        case ACK_USER_LOGIN:
        {
            m_enOdipStatus = enAppLogicStatus_Logined;
            if (0 == pDvrip->dvrip.dvrip_p[0])
            {

                m_iChannel = pDvrip->dvrip.dvrip_p[2];

                DBG_CODE(
                    __fline;
                    printf("Login Succeeded, CH:%d.\n", m_iChannel);
                );
            }
            else
            {
                DBG_CODE(
                    __fline;
                    printf("Login Failed.\n");
                );
            }
            break;
        }
        case ACK_CHAN_MONIT:
        {
            int retCh = pDvrip->dvrip.dvrip_p[0];
            int ch = retCh%m_iChannel;
            int Stream = retCh/m_iChannel;
            DBG_CODE(
                printf("ACK_CHAN_MONIT ch:%d, len:%d, %d, %d\n", pDvrip->dvrip.dvrip_p[0], pDvrip->dvrip.dvrip_extlen, ch, Stream);
            );

            int find = FindRealPlayStatus(ch, Stream);
            // dvr返回错误
            if (find<0)
            {
                // 抓图的时候不能
                //__trip;

            }
            else
            {
                m_RealPlayStatus[find].ttLastOndata = time(NULL);
            }


            if (m_RealDataCallBack)
            {
                m_RealDataCallBack((char *)pData
                                   , pDvrip->dvrip.dvrip_extlen
                                   , pDvrip->dvrip.dvrip_p[0]
                                   , 0);
            }
            else
            {
                DBG_CODE(
                    __fline;
                    printf(" %x\n", this->m_RealDataCallBack);
                );
            }

            break;
        }
        case ACK_INFO_SYSTM:
        {
            if (pDvrip->dvrip.dvrip_p[0]== 0x08)
            {
                std::string __strData((char *)&pDvrip->dvrip.dvrip_p[24], pDvrip->dvrip.dvrip_extlen);
                if (__strData.size()>=1)
                {
                    m_strRemoteVersion = __strData;
                    //__fline;
                    //std::cout << m_strRemoteVersion << std::endl;
                }

            }
            break;
        }
        default:
        {
            DBG_CODE(
                __fline;printf("OnCommand_ODIP ^_^\n");
            );
            break;
        }
    } // switch (pDvrip->dvrip.dvrip_cmd)

}
#endif

#include <sstream>

int MON_STATUS_T::Struct2String(std::string &strMon)
{
    std::stringstream __ss;
    __ss \
    << MonType	 \
    << "||" << Channel	 \
    << "||" << Stream	 \
    << "||" << Frames	 \
    << "||" << OnOff	 \
    << "||" << TransProtocol	 \
    << "||" << RemoteIP	 \
    << "||" << RemotePort;

    strMon = __ss.str();

    return 0;
}
