/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ezSms.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ezSms.cpp 5884 2013-07-16 07:30:08Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-16 07:30:08 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <iostream>
#include "str_opr.h"
#include "ez_socket.h"

#include "EZThread.h"
#include "EZTimer.h"
#include "../Logs.h"

#include "../Stack/SmsStack.h"

#include "ezSms.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

// 每小时检查一次
#define EZSMS_UPDATE_PERIOD 3600
//异常的时候每分钟一次
#define EZSMS_UPDATE_PERIOD_FAILED 30

PATTERN_SINGLETON_IMPLEMENT(CezSms);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CezSms::CezSms() :CEZThread("CezSms", TP_DEFAULT)
{
    //trace("CezSms Enter--------\n");
    m_ttLastOn = 0;
}

CezSms::~CezSms()
{
    //trace("CezSms Leave--------\n");
}

BOOL CezSms::Start()
{
    if (m_bLoop)
    {
        return TRUE;
    }
    int ret = CreateThread();

    return ret;
}

BOOL CezSms::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = FALSE;
        DestroyThread();
    }
    return TRUE;
}

void CezSms::ThreadProc()
{
    while (m_bLoop)
    {
        DBG(
            __trip;
        );

        m_cfgGeneral.update();
        char strDateTime[GET_DATE_TIME_STRING_DEFAULT_STR_LEN] = {0};
        get_date_time_string(strDateTime, NULL);


        CSmsMsg_RegisterReq Msg;
        Msg.ProductID		= m_cfgGeneral.getConfig().strIEEI;
        Msg.AuthCode 	= "ezlibs.com";
        Msg.AuthName		= "ezlibs.com";
        Msg.Password	= "ezlibs.com";

        // 在下面自动获取，不必传送
        //Msg.m_LocalStatus.LocalIp = addr_v4==NULL?"":inet_ntoa(addr_v4->sin_addr);
        Msg.m_LocalStatus.LocalDateTime = strDateTime;

        ez_socket_t sockfd ;

        char recvline[HTTP_MAXLINE + 1];

        char *hname = "sms.ezlibs.com";
        char *hport = "60088";
        char *page = "/Sms/Register";
        //char *poststr = "{ \"AuthCode\" : \"VAuthCode\", \"AuthName\" : \"VAuthName\", \"LocalDateTime\" : \"VLocalDateTime\", \"LocalIp\" : \"VLocalIp\", \"Password\" : \"VPassword\", \"ProductID\" : \"VProductID\" }\n";
        //*******************************************************

        sockfd = ez_tcp_client(hname, hport);

        if (sockfd==INVALID_SOCKET)
        {
DBG(
            printf("failed.\n");
);
            sleep(EZSMS_UPDATE_PERIOD_FAILED);
            continue;
            //return -100;
        }
        //////////////////////////////////////////
        ///////
        struct sockaddr addr;
        struct sockaddr_in* addr_v4 = NULL;
        socklen_t addr_len = sizeof(addr);

        //获取local ip and port
        memset(&addr, 0, sizeof(addr));

        if (0 == getsockname(sockfd, &addr, &addr_len))
        {
            if (addr.sa_family == AF_INET)
            {
                addr_v4 = (sockaddr_in*)&addr;
                //cMonitorInf.nSourceIP = addr_v4->sin_addr.S_un.S_addr;
                //cMonitorInf.nSourcePort = ntohs(addr_v4->sin_port);

                Msg.m_LocalStatus.LocalIp = inet_ntoa(addr_v4->sin_addr);
            }
        }

        ///////
        CSmsStack __CSmsStack;
        __CSmsStack.Encode(Msg);

DBG(
        std::cout << __CSmsStack.ToString() << std::endl;
);
        // do not use fast mode
        int ret = ez_http_post(sockfd, hname, page, (char *)__CSmsStack.ToString().c_str(), recvline, HTTP_MAXLINE, 0);

        if (ret > 0)
        {
DBG(
            printf("recv(%d):\n--------------------\n%s\n---------------------\n", ret, recvline);
);

            // use ret;
            char *pRcv = strstr(recvline, "[");
            if (pRcv)
            {
                trim_blank(pRcv, 0);
                l_trim_c(pRcv, '[', 0);
                r_trim_c(pRcv, ']', 0);

                // ok
DBG(
                    __fline;
                    printf("pRcv(%d):\n--------------------\n%s\n---------------------\n", strlen(pRcv), pRcv);
);
                CSmsStack __SmsStack;
                __SmsStack.Parse(pRcv);
                if (__SmsStack.isValid())
                {
                    CSmsMsg_RegisterAck __ackMsg;

                    //todo 使用消息
                    //continue;
                    if (__SmsStack.Decode(__ackMsg))
                    {
DBG(
                        __ackMsg.dump();
);
                        if (__ackMsg.m_ackInfo.result=="1") //Unauthorized
                        {
                            // todo shutdown
DBG(
                            __trip;
);
                            system("shutdown -h now &");


                            LOG4CPLUS_INFO(LOG_OF_SMS, "Unauthorized device.");

                        }
                    }

                    m_ttLastOn = time(NULL);
                }


            }
        }
        else
        {
            printf("ez_http_post failed:%d\n", ret);
        }
        close (sockfd);

        //////////////////////////////////////////
        if (m_ttLastOn==0)
        {
            sleep(EZSMS_UPDATE_PERIOD_FAILED);
        }
        else
        {
            sleep(EZSMS_UPDATE_PERIOD);
        }
    }
}

