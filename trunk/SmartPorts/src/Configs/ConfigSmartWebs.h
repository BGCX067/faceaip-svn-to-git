/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigSmartWebs.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigSmartWebs.h 5884 2013-06-05 05:11:14Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-05 05:11:14  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigSmartWebs_H__
#define __ConfigSmartWebs_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigSmartWebs
{
    int SPort;// 端口
    int iRetries;// 端口试探上限
 
    // 终端状态维护/ 检查周期
    // 0- 不检查
    // >0 秒
    int CheckPeriod;
    // 超时时间 
    int PeerTimeout;

// 服务名称
    std::string strSName;
}
ConfigSmartWebs;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigSmartWebs, 1, 4> CConfigSmartWebs;

#endif //__ConfigSmartWebs_H__
