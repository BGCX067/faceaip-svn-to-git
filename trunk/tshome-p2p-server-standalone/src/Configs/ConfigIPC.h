/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigIPC.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: ConfigIPC.h 5884 2012-07-27 04:02:19Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-07-27 04:02:19  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __CONFIGIPC_H__
#define __CONFIGIPC_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigIPC
{
    int IpcPort;// 端口
 
    // 终端状态维护/ 检查周期
    // 0- 不检查
    // >0 秒
    int CheckPeriod;
    // 超时时间 
    int PeerTimeout;
}
ConfigIPC;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigIPC, 1, 4> CConfigIPC;

#endif //__CONFIGIPC_H__
