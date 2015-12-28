/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigGeneral.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: ConfigGeneral.h 5884 2012-07-02 09:15:15Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __CONFIGGENERAL_H__
#define __CONFIGGENERAL_H__

#include "ConfigBase.h"
//!	普通配置
typedef struct tagCONFIG_GENERAL
{	
	int iLocalNo;
	std::string  strMachineName;
	std::string strProductID;
	std::string strVendor;
	std::string strSerialNumber;
	std::string strVersion;
	// 日志文件路径 需要事前创建
	std::string strLogFilePath;
	// 日志文件名 如果长度为0， 则输出到屏幕
	std::string strLogFileName;
	int maxFileSize;
	int maxBackupIndex;

	// 调试端口
	int portConsoleOverTcp;
} CONFIG_GENERAL;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<CONFIG_GENERAL, 1, 4> CConfigGeneral;

#endif //__CONFIGGENERAL_H__
