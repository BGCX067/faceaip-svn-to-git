/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigNTPClient.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigNTPClient.cpp 5884 2013-07-10 10:49:44Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-10 10:49:44 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigNTPClient.h"

template<> void exchangeTable<ConfigNTPClient>(CConfigExchange& xchg, CConfigTable& table, ConfigNTPClient& config, int index, int app)
{
    xchg.exchange(table, "strNtpServer"    , config.strNtpServer    , "time.nist.gov");
    xchg.exchange(table, "iNtpPort"    , config.iNtpPort    , 0, 65535 ,123);

    xchg.exchange(table,"TimeZoneAuto", config.iTimeZoneAuto,  0, 1, 0);
    xchg.exchange(table,"UpdatePeriod", config.UpdatePeriod,  0, 65535 ,600);
}

