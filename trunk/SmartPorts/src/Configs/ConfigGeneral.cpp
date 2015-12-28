/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigGeneral.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigGeneral.cpp 5884 2012-07-02 09:15:19Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "ConfigGeneral.h"

template<>
void exchangeTable<CONFIG_GENERAL>(CConfigExchange& xchg, CConfigTable& table, CONFIG_GENERAL& config, int index, int app)
{
    //(CConfigTable& table, CConfigKey key, int& value, int min, int max, int dflt)
    xchg.exchange(table,"LocalNo",	   config.iLocalNo,	     0, 998 ,8		);
    xchg.exchange(table,"MachineName", config.strMachineName, "SmartPorts");// 
    xchg.exchange(table,"ProductID", config.strProductID, "123456");
    xchg.exchange(table,"Vendor", config.strVendor, "tiansu");
    xchg.exchange(table,"SerialNumber", config.strSerialNumber, "08620543482132792993272592000");
    xchg.exchange(table,"IEEI", config.strIEEI, "13359211641994461155");
    xchg.exchange(table,"Version", config.strVersion, "v 0.9.5");

    xchg.exchange(table,"LogFilePath", config.strLogFilePath, "./logs");
    xchg.exchange(table,"LogFileName", config.strLogFileName, "s-ports.log");
    xchg.exchange(table,"maxLogSize", config.maxFileSize,  0, 50*1024*1024 ,3*1024*1024);
    xchg.exchange(table,"maxLogFile", config.maxBackupIndex,  0, 10 ,6);

    xchg.exchange(table, "iLanguage"    , config.iLanguage    , 0,0xffff, 1); // 0 - auto(by user's), 1 - chn 2 - eng
    xchg.exchange(table, "iLanguageSupport"    , config.iLanguageSupport    , 0 , 0xffff, 0xffff); //mask

    xchg.exchange(table, "TimeZone"    , config.iTimeZone    , 0,33, 13);
    xchg.exchange(table, "DateTimeFormat"    , config.iDateTimeFormat    , 0,33, 0);// 0 - 2013-07-10 14:07:26
}

