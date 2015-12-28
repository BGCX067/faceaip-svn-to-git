/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigConsoleOverTcp.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigConsoleOverTcp.cpp 5884 2013-06-07 11:28:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-07 11:28:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/



#include "ConfigConsoleOverTcp.h"

template<> void exchangeTable<ConfigConsoleOverTcp>(CConfigExchange& xchg, CConfigTable& table, ConfigConsoleOverTcp& config, int index, int app)
{
    xchg.exchange(table, "SPort"    , config.SPort,       0, 65535 ,10023);

    xchg.exchange(table,"CheckPeriod", config.CheckPeriod,  0, 65535 ,0);
    xchg.exchange(table,"PeerTimeout", config.PeerTimeout,  0, 65535 ,300);
}
