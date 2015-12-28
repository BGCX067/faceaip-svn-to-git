/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigIPC.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: ConfigIPC.cpp 5884 2012-07-27 04:02:13Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-07-27 04:02:13  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigIPC.h"

template<> void exchangeTable<ConfigIPC>(CConfigExchange& xchg, CConfigTable& table, ConfigIPC& config, int index, int app)
{
    xchg.exchange(table, "IpcPort"    , config.IpcPort,  0, 65535 ,11113);

    xchg.exchange(table,"CheckPeriod", config.CheckPeriod,  0, 65535 ,0);
    xchg.exchange(table,"PeerTimeout", config.PeerTimeout,  0, 65535 ,300);
}
