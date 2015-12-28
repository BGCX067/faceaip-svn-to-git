/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigEnOcean.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigEnOcean.cpp 5884 2012-07-02 09:15:25Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "ConfigDatabase.h"

#define MYSQLD_HOST "localhost"
#define MYSQLD_USER "root"
#define MYSQLD_PWDD "tshomedb0607"
#define MYSQLD_DBNA "remoteserver"
#define MYSQLD_PORT 0
#define MYSQLD_PORT_STR "0"

template<> void exchangeTable<ConfigDatabase>(CConfigExchange& xchg, CConfigTable& table, ConfigDatabase& config, int index, int app)
{
    xchg.exchange(table, "Host" ,     config.strHost     , MYSQLD_HOST);
    xchg.exchange(table, "Port" ,     config.iPort     ,  0, 65535 ,MYSQLD_PORT);
    xchg.exchange(table, "UserName" ,     config.strUser     , MYSQLD_USER);
    xchg.exchange(table, "Passwd" ,      config.strPwd      , MYSQLD_PWDD);
    xchg.exchange(table, "Database" , config.strDatabase , MYSQLD_DBNA);
}
