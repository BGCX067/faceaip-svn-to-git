/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigDB.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: ConfigDB.h 5884 2012-07-02 09:15:22Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// ConfigDB.h
#ifndef __CONFIGDB_H__
#define __CONFIGDB_H__

#include "ConfigBase.h"

//!	http客户端
typedef struct tagConfigDB
{	
    // 设备接口
    std::string strHost 		;
    int iPort 		;
    std::string strUser 		;
    std::string strPwd 		;
    std::string strDatabase 		;
} ConfigDB;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<tagConfigDB, 1, 4> CConfigDB;

#endif //__CONFIGDB_H__