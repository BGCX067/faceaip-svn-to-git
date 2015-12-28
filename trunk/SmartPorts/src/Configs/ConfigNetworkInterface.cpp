/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigNetworkInterface.cpp - brief
 *
 * Copyright (C) 2013 FaceMetro.com, All Rights Reserved.
 *
 * $Id: ConfigNetworkInterface.cpp 5884 2013-06-18 05:03:33Z WuJunjie $
 *
 *  Notes:
 *     -
 *      Õ¯¬ÁΩ”ø⁄≈‰÷√
 *     -
 *
 *  Update:
 *     2013-06-18 05:03:33  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigNetworkInterface.h"

template<>
void exchangeTable<CONFIG_NETWORKINTERFACE_T>(CConfigExchange& xchg, CConfigTable& table, CONFIG_NETWORKINTERFACE_T& config, int index, int app)
{
    xchg.exchange(table, "iIndex",config.iIndex,0,2550,index+1);
    xchg.exchange(table,"bDhcp",config.bDhcp,0,1,1);

    if (index == 0)
    {
        xchg.exchange(table,"bEnable",config.bEnable,0,1,1);
        xchg.exchange(table,"bDefault",config.bDefault,0,1,1);

        xchg.exchange(table, "strIp", config.strIp, "192.168.6.108");
        xchg.exchange(table, "strMask", config.strMask, "255.255.255.0");
        xchg.exchange(table, "strGateWay", config.strGateWay, "192.168.6.1");

        xchg.exchange(table, "strIFName", config.strIFName, "eth0");// rd only
    }
    else if (index == 1)
    {
        xchg.exchange(table,"bEnable",config.bEnable,0,1,1);
        xchg.exchange(table,"bDefault",config.bDefault,0,1,0);

        xchg.exchange(table, "strIp", config.strIp, "192.168.10.108");
        xchg.exchange(table, "strMask", config.strMask, "255.255.255.0");
        xchg.exchange(table, "strGateWay", config.strGateWay, "192.168.10.1");

        xchg.exchange(table, "strIFName", config.strIFName, "eth1");// rd only
    }
    else
    {
        xchg.exchange(table,"bEnable",config.bEnable,0,1,0);
        xchg.exchange(table,"bDefault",config.bDefault,0,1,0);

        xchg.exchange(table, "strIp", config.strIp, "192.168.0.108");
        xchg.exchange(table, "strMask", config.strMask, "255.255.255.0");
        xchg.exchange(table, "strGateWay", config.strGateWay, "192.168.0.1");

        xchg.exchange(table, "strIFName", config.strIFName, "");// rd only
    }

    xchg.exchange(table, "AutoMaintenance", config.iAutoMaintenance, 0, 1, 1);
    xchg.exchange(table, "strMac", config.strMac, "R");
}

