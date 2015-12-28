/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigNetworkInterface.h - brief
 *
 * Copyright (C) 2013 FaceMetro.com, All Rights Reserved.
 *
 * $Id: ConfigNetworkInterface.h 5884 2013-06-18 05:03:19Z WuJunjie $
 *
 *  Notes:
 *     -
 *      网络接口配置
 *     -
 *
 *  Update:
 *     2013-06-18 05:03:19  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigNetworkInterface_H__
#define __ConfigNetworkInterface_H__

//#include "ConfigBase.h"
#include "ConfigNet.h"
//!
typedef struct tagConfigNetworkInterface_t
{
    int iIndex;// 内部编号
    int bEnable;// true or false
    int bDefault;// true or false
    int bDhcp;// true or false, dhcp

    std::string strIp;//static ip
    std::string strMask; // static mask
    std::string strGateWay; // gw

    std::string strIFName;//rd only, get from sys
    std::string strMac; // rd only, get from sys

    int iAutoMaintenance; //send gratuitous arp Periodically 
}
CONFIG_NETWORKINTERFACE_T;

typedef TConfig<CONFIG_NETWORKINTERFACE_T, MAX_NETWORK_INTERFACE_NUM/*结构数目, 即最多?个网口*/, 4/*观察者最大数目*/> CConfigNetworkInterface;

#endif //__ConfigNetworkInterface_H__
