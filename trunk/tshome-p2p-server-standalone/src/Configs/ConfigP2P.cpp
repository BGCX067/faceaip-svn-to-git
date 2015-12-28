/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigP2P.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: ConfigP2P.cpp 5884 2012-07-27 04:01:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-07-27 04:01:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigP2P.h"

template<> void exchangeTable<ConfigP2P>(CConfigExchange& xchg, CConfigTable& table, ConfigP2P& config, int index, int app)
{
    xchg.exchange(table, "StunPort" , config.StunPort,  0, 65535 ,34400);
    xchg.exchange(table,"maxClient", config.maxClient,  0, 999999999 ,13*10240);

    xchg.exchange(table,"minUdpPort", config.minUdpPort,  0, 65535 ,49152);
    xchg.exchange(table,"maxUdpPort", config.maxUdpPort,  0, 65535 ,65535);

    xchg.exchange(table,"HostCheckPeriod", config.HostCheckPeriod,  0, 65535 ,0);
    xchg.exchange(table,"TermCheckPeriod", config.TermCheckPeriod,  0, 65535 ,0);
    xchg.exchange(table,"HostTimeout", config.HostTimeout,  0, 65535 ,300);
    xchg.exchange(table,"TermTimeout", config.TermTimeout,  0, 65535 ,300);
}
