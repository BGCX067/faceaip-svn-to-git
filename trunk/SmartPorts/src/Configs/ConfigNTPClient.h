/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigNTPClient.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigNTPClient.h 5884 2013-07-10 10:50:00Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-10 10:50:00 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigNTPClient_H__
#define __ConfigNTPClient_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigNTPClient
{
    std::string strNtpServer;
    int iNtpPort;// 端口

    int iTimeZoneAuto;
 
    // 更新周期(sec)
    // 0- 不检查 不使用ntp
    // >0 秒
    int UpdatePeriod;
}
ConfigNTPClient;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigNTPClient, 1, 4> CConfigNTPClient;

#endif //__ConfigNTPClient_H__
