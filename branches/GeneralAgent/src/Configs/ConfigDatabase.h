/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigDatabase.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigDatabase.h 5884 2012-07-02 09:15:22Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// ConfigDatabase.h
#ifndef __ConfigDatabase_H__
#define __ConfigDatabase_H__

#include "ConfigBase.h"

//!	http客户端
typedef struct tagConfigDatabase
{	
    // 设备接口
    std::string strHost 		;
    int iPort 		;
    std::string strUser 		;
    std::string strPwd 		;
    std::string strDatabase 		;
} ConfigDatabase;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<tagConfigDatabase, 1, 4> CConfigDatabase;

#endif //__ConfigDatabase_H__
