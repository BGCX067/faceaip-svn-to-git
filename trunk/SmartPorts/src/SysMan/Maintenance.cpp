/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Maintenance.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: Maintenance.cpp 5884 2013-06-25 05:13:08Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-06-25 05:13:08 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

//#include "ez_bit.h"
#include "EZThread.h"
#include "EZTimer.h"

#include "Maintenance.h"
#include <iostream>

#include "../Logs.h"

#ifdef FUNCTION_DATETIME_AND_NTP
#include "../Solar.h"
#endif

#ifndef _DEBUG_THIS
// #define _DEBUG_THIS
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

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
PATTERN_SINGLETON_IMPLEMENT(CMaintenance);

CMaintenance::CMaintenance():m_TimerSendGratuitousArp("SendGratuitousArp")
#ifndef STANDLONE_UTIL
        , m_TimerNTPClient("NTPClient")
#endif //STANDLONE_UTIL
{
    m_bMaintenanceAuto = false;

#ifdef FUNCTION_DATETIME_AND_NTP

    m_ttLastUpdate = 0;
    m_ttLastUpdateFailed = 0;
    m_LastNTPClient.UpdatePeriod = -1;

    iRunTimerProcSendNTPClient_Succ = 0;
    iRunTimerProcSendNTPClient_Failed = 0;
#endif //FUNCTION_DATETIME_AND_NTP
}

CMaintenance::~CMaintenance()
{}

void CMaintenance::Start(const bool bUseCfgAtStartForce, const bool bMaintenanceAuto)
{
    m_bMaintenanceAuto = bMaintenanceAuto;

    MaintenanceAtStart(bUseCfgAtStartForce);

    if (m_bMaintenanceAuto)
    {

        NetMaintenance();
        //m_TimerSendGratuitousArp.Start(this
        //                               , (TIMERPROC)&CMaintenance::TimerProcSendGratuitousArp
        //                             , 0
        //                           , 10 * 1000  /*period ms*/
        //                         , 0
        //                       , 10*1000
        //                    );

    }

#ifndef STANDLONE_UTIL
    m_CCfgNet.update();
    m_CCfgNetworkInterface.update();

    m_LastConfigNet = m_CCfgNet.getConfig();
    m_CCfgNet.attach(this,(TCONFIG_PROC)&CMaintenance::OnChangedConfigNet);
    m_CCfgNetworkInterface.attach(this,(TCONFIG_PROC)&CMaintenance::OnChangedConfigNetworkInterface);

#endif //STANDLONE_UTIL
#ifdef FUNCTION_DATETIME_AND_NTP

    m_CCfgNTPClient.update();

    m_LastNTPClient = m_CCfgNTPClient.getConfig();

    m_CCfgNTPClient.attach(this,(TCONFIG_PROC)&CMaintenance::OnChangedConfigNTPClient);

    if (m_CCfgNTPClient.getConfig().UpdatePeriod>0)
    {
        m_TimerSendGratuitousArp.Start(this
                                       , (TIMERPROC)&CMaintenance::TimerProcSendNTPClient
                                       , 0
                                       , m_CCfgNTPClient.getConfig().UpdatePeriod * 1000  /*period ms*/
                                       , 0
                                       , 10*1000
                                      );
    }
    LOG4CPLUS_INFO(LOG_SOLAR,"CMaintenance start ntp function ok");
#endif //FUNCTION_DATETIME_AND_NTP

    LOG4CPLUS_INFO(LOG_SOLAR,"CMaintenance start finished");
}

void CMaintenance::Stop()
{
    if (m_bMaintenanceAuto)
    {
        m_TimerSendGratuitousArp.Stop();
    }
}

int CMaintenance::MaintenanceAtStart(bool bForce)
{
    // 根据配置来初始化系统
    if (bForce)
    {
        CConfigNet __cfgNet;
        CConfigNetworkInterface __cfgNetworkInterface;
        __cfgNet.update();
        __cfgNetworkInterface.update();


        int iret;
        char str_ip_get_1[MAC_STR_SIZ]={0};
        char str_ip_get_2[MAC_STR_SIZ]={0};


        // dns
        iret = ez_dns_get(str_ip_get_1, str_ip_get_2);

        if (iret<1
            ||__cfgNet.getConfig().strPrimaryDNS != str_ip_get_1
            ||__cfgNet.getConfig().strSecondaryDNS != str_ip_get_2)
        {
            DBG(
                __fline;
                printf("Use dns\n");
            );

            iret = ez_dns_set(__cfgNet.getConfig().strPrimaryDNS.c_str()
                              , __cfgNet.getConfig().strSecondaryDNS.c_str());
            DBG(
                __fline;
                printf("ez_dns_set:%d -- [%s][%s]\n"
                       , iret
                       , __cfgNet.getConfig().strPrimaryDNS.c_str()
                       , __cfgNet.getConfig().strSecondaryDNS.c_str());
            );

        }
        else
        {
            DBG(
                __fline;
                printf("Dns OK, noneed set again\n");
            );
        }

        for (int ii=0; ii<__cfgNet.getConfig().iNetworkInterfaceNum; ii++)
        {
            CONFIG_NETWORKINTERFACE_T &__cfg = __cfgNetworkInterface.getConfig(ii);

            iret = EthMaintenance(&__cfg);
            DBG(
                __fline;
                printf("EthMaintenance(%d):%d\n", ii, iret);
            );
        }
    }
    else
    {}

    return 0;
}


void CMaintenance::TimerProcSendGratuitousArp()
{
    //__fline;
    //printf("%s\n", __FUNCTION__);
    static unsigned int iRunTimes=0;

    int iret;

    CConfigNetworkInterface __cfgNetworkInterface;
    __cfgNetworkInterface.update();
    CConfigNet __cfgNet;
    __cfgNet.update();


    for (int ii=0; ii<__cfgNet.getConfig().iNetworkInterfaceNum; ii++)
    {
        CONFIG_NETWORKINTERFACE_T &__cfgNETWORKINTERFACE = __cfgNetworkInterface.getConfig(ii);
        DBG(
            __fline;
            printf("ii:%d auto:%d, en:%d, name:%s\n", ii
                   ,__cfgNETWORKINTERFACE.iAutoMaintenance
                   ,__cfgNETWORKINTERFACE.bEnable
                   ,__cfgNETWORKINTERFACE.strIFName.c_str());
        );
        //continue;


        if (1 ==__cfgNETWORKINTERFACE.bEnable
            && 1 ==__cfgNETWORKINTERFACE.iAutoMaintenance)
        {
            iret = ez_send_gratuitous_arp(__cfgNETWORKINTERFACE.strIFName.c_str());
            iRunTimes++;

            LOG4CPLUS_INFO(LOG_MAT,"ez_send_gratuitous_arp:" << __cfgNETWORKINTERFACE.strIFName << "iRunTimes:" << iRunTimes);

            DBG(
                if ((iRunTimes%8)==0)
            {
                __fline;
                printf("ez_send_gratuitous_arp ii:%d iRunTimes:%d auto:%d, en:%d, name:%s\n", ii, iRunTimes
                       ,__cfgNETWORKINTERFACE.iAutoMaintenance
                       ,__cfgNETWORKINTERFACE.bEnable
                       ,__cfgNETWORKINTERFACE.strIFName.c_str())
                    ;
                }
            );
        }
    }
}

int CMaintenance::EthMaintenance(CONFIG_NETWORKINTERFACE_T *pCfgIF)
{
    int iret;

    if (pCfgIF->bEnable != 1)
    {
        //__fline;
        //printf("EthMaintenance skip pCfgIF->strIFName:%s\n", pCfgIF->strIFName.c_str());

        return -1;
    }

    // dhcp
    // todo, use
    if (pCfgIF->bDhcp)
    {
        //-R,--release    Release IP on quit
        // 如果带这个参数，udhcpc退出的时候会将接口参数去除
        // 这里不需要其去除
        //char strCmd[] = {"                   udhcpc -R -b -p /var/run/udhcpc.eth0.pid -i eth0 &"};
        char strCmd[] = {"                   udhcpc -b -p /var/run/udhcpc.eth0.pid -i eth0 &"};
        snprintf(strCmd, strlen(strCmd), "udhcpc -b -p /var/run/udhcpc.%s.pid -i %s &"
                 , pCfgIF->strIFName.c_str()
                 , pCfgIF->strIFName.c_str());

        __fline;
        printf("strCmd:%s\n", strCmd);
        return system(strCmd);
        //dhcp
    }

    //DBG(
    printf("ez_ip_set %s, %s, %s ", pCfgIF->strIFName.c_str(), pCfgIF->strIp.c_str(), pCfgIF->strMask.c_str());
    //);
    iret = ez_ip_set(pCfgIF->strIFName.c_str(), pCfgIF->strIp.c_str(), pCfgIF->strMask.c_str());

    if (iret<0)
    {
        //DBG(
        printf("Failed(%d)\n", iret);
        //);
    }
    else
    {
        //DBG(
        printf("Succeeded\n");
        //);
    }

    DBG(
        printf("ez_set_gateway:%s, %s ", pCfgIF->strIp.c_str(), pCfgIF->strGateWay.c_str());
    );
    iret=ez_set_gateway(pCfgIF->strIp.c_str(), pCfgIF->strMask.c_str(), pCfgIF->strGateWay.c_str());
    if (iret<0)
    {
        DBG(
            printf("Failed(%d)\n", iret);
        );
    }
    else
    {
        DBG(
            printf("Succeeded\n");
        );
    }

    // default gw
    if (1 == pCfgIF->bDefault)
    {
        DBG(
            printf("ez_set_default_gateway(%s) \n", pCfgIF->strGateWay.c_str());
        );
        iret=ez_set_default_gateway(pCfgIF->strGateWay.c_str());
        if (iret<0)
        {
            DBG(
                printf("Failed(%d),sudo?\n", iret);
            );
        }
        else
        {
            DBG(
                printf("Succeeded\n");
            );
        }
    }
    return 0;
}

int CMaintenance::NetMaintenance()
{
    CConfigNet __cfgNet;
    __cfgNet.update();

    if (__cfgNet.getConfig().iGarpPeriod>0)
    {
        m_TimerSendGratuitousArp.Start(this
                                       , (TIMERPROC)&CMaintenance::TimerProcSendGratuitousArp
                                       , 0
                                       , __cfgNet.getConfig().iGarpPeriod * 1000  /*period ms*/
                                       , 0
                                       , 10*1000
                                      );
        DBG(
            printf("TimerProcSendGratuitousArp start: %d\n", __cfgNet.getConfig().iGarpPeriod);
        );
    }

    return 0;
}

#ifndef STANDLONE_UTIL
void CMaintenance::OnChangedConfigNet(CConfigNet* pConfigNet, int &ret)
{
    int iret;

    if (NULL == pConfigNet)
    {
        __trip;
        return ;
    }

    // set dns
    if (m_LastConfigNet.strPrimaryDNS!=pConfigNet->getConfig().strPrimaryDNS
        ||m_LastConfigNet.strSecondaryDNS!=pConfigNet->getConfig().strSecondaryDNS)
    {
        //是否有 差异不需再检查，内部保证变化了才触发
        iret = ez_dns_set(pConfigNet->getConfig().strPrimaryDNS.c_str()
                          , pConfigNet->getConfig().strSecondaryDNS.c_str());
        DBG(
            __fline;
            printf("ez_dns_set:%d -- [%s][%s]\n"
                   , iret
                   , pConfigNet->getConfig().strPrimaryDNS.c_str()
                   , pConfigNet->getConfig().strSecondaryDNS.c_str());
        );

        if (iret<0)
        {
            LOG4CPLUS_ERROR(LOG_MAT,"ez_dns_set:" << iret);
        }

        m_LastConfigNet.strPrimaryDNS = pConfigNet->getConfig().strPrimaryDNS;
        m_LastConfigNet.strSecondaryDNS = pConfigNet->getConfig().strSecondaryDNS;
    }

    // garp
    if (m_LastConfigNet.iGarpPeriod != pConfigNet->getConfig().iGarpPeriod)
    {
        if (m_LastConfigNet.iGarpPeriod>0)
        {
            m_TimerSendGratuitousArp.Stop();
        }

        iret = NetMaintenance();

        if (iret<0)
        {
            LOG4CPLUS_ERROR(LOG_MAT,"NetMaintenance:" << iret);
        }

        m_LastConfigNet.iGarpPeriod = pConfigNet->getConfig().iGarpPeriod;
    }

}

void CMaintenance::OnChangedConfigNetworkInterface(CConfigNetworkInterface* pConfig, int &ret)
{
    int iret;

    CConfigNet __cfgNet;
    __cfgNet.update();

    const char *pCmd = "killall udhcpc";

    DBG(
        __fline;
        printf("strCmd:%s\n", pCmd);
    );
    system(pCmd);

    for (int ii=0; ii<__cfgNet.getConfig().iNetworkInterfaceNum; ii++)
    {
        CONFIG_NETWORKINTERFACE_T &__cfg = pConfig->getConfig(ii);

        iret = EthMaintenance(&__cfg);

        DBG(
            __fline;
            printf("EthMaintenance(%d):%d\n", ii, iret);
        );
    }

}
#endif //STANDLONE_UTIL

#ifdef FUNCTION_DATETIME_AND_NTP
void CMaintenance::OnChangedConfigNTPClient(CConfigNTPClient* pConfigNTPClient, int &ret)
{
    //__trip;

    int oldUpdatePeriod = m_LastNTPClient.UpdatePeriod;

    m_LastNTPClient = pConfigNTPClient->getConfig();

    if (oldUpdatePeriod!=pConfigNTPClient->getConfig().UpdatePeriod)
    {
        if (oldUpdatePeriod>0 )
        {
            m_TimerSendGratuitousArp.Stop();

            LOG4CPLUS_INFO(LOG_MAT,"m_TimerSendGratuitousArp.Stop");
        }

        if (pConfigNTPClient->getConfig().UpdatePeriod>0)
        {
            m_TimerSendGratuitousArp.Start(this
                                           , (TIMERPROC)&CMaintenance::TimerProcSendNTPClient
                                           , 0
                                           , pConfigNTPClient->getConfig().UpdatePeriod * 1000  /*period ms*/
                                           , 0
                                           , 10*1000
                                          );

            LOG4CPLUS_INFO(LOG_MAT,"m_TimerSendGratuitousArp.Start, period:" << pConfigNTPClient->getConfig().UpdatePeriod);
        }
    }

}

void CMaintenance::TimerProcSendNTPClient()
{
    //__fline;
    //printf("%s\n", __FUNCTION__);
    int iret;

    iret = ez_ntp_cli(m_LastNTPClient.strNtpServer.c_str());
    if (iret>=0)
    {

        m_ttLastUpdate = time(NULL);
        iRunTimerProcSendNTPClient_Succ++;

        g_Solar.MarkOnInternet();
    }
    else
    {
        m_ttLastUpdateFailed = time(NULL);
        iRunTimerProcSendNTPClient_Failed++;
    }
    DBG(
        __fline;
        printf("ez_ntp_cli %s -- %d\n", m_LastNTPClient.strNtpServer.c_str(), iret);
    );
}

time_t CMaintenance::GetLastUpdateFailed()
{
    return m_ttLastUpdateFailed;// 更新时间 或者更改时间
}

time_t CMaintenance::GetLastUpdate()
{
    return m_ttLastUpdate;// 更新时间 或者更改时间
}

unsigned int CMaintenance::GetRunTimerProcSendNTPClient_Succ()
{
    return iRunTimerProcSendNTPClient_Succ;
}

unsigned int CMaintenance::GetRunTimerProcSendNTPClient_Failed()
{
    return iRunTimerProcSendNTPClient_Failed;
}

#endif //FUNCTION_DATETIME_AND_NTP

