/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigNet.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigNet.cpp 5884 2013-07-15 10:32:49Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-15 10:32:49 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigNet.h"

template<>
void exchangeTable<CONFIG_NET>(CConfigExchange& xchg, CConfigTable& table, CONFIG_NET& config, int index, int app)
{
    //
    xchg.exchange(table,"PrimaryDNS", config.strPrimaryDNS, "8.8.8.8");
    xchg.exchange(table,"SecondaryDNS", config.strSecondaryDNS, "208.67.222.222");

    xchg.exchange(table,"NetworkInterfaceNum",	   config.iNetworkInterfaceNum,	     0, MAX_NETWORK_INTERFACE_NUM, 2);
    xchg.exchange(table,"GarpPeriod",	   config.iGarpPeriod,	     0, 600, 60);

    xchg.exchange(table,"SshdPort",	   config.iSshdPort,	     0, 65535, 22);
}

