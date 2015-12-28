/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigNet.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigNet.h 5884 2013-07-15 10:32:42Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-15 10:32:42 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigNet_H__
#define __ConfigNet_H__

#include "ConfigBase.h"

// very important, max interface I support
#define MAX_NETWORK_INTERFACE_NUM 4

//!	普通配置
typedef struct tagCONFIG_NET
{
    // network
    std::string strPrimaryDNS;//dns1
    std::string strSecondaryDNS;//dns2

    // 在系统初始化的时候修正为正确的数值
    int iNetworkInterfaceNum;//total num of lan wifi etc

    int iGarpPeriod;//garp >=0, 0-disable

    int iSshdPort;// >=0, 0-disable
}
CONFIG_NET;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<CONFIG_NET, 1, 4> CConfigNet;

#endif //__ConfigNet_H__

