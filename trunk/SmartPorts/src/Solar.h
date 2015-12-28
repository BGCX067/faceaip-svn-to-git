/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Solar.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Solar.h 5884 2012-05-16 09:17:46Z WuJunjie $
 *
 *  Explain:
 *     -
 *      智能家居服务器
 *     -
 *
 *  Update:
 *     2012-05-16 09:17:46   WuJunjie  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __Solar_H__
#define __Solar_H__

#include "EZThread.h"
#include "EZTimer.h"

#include "ez_socket.h"
#include "net_work_interface.h"

#define GetPubIP_PERIOD 30

class CSolar : public CEZObject
{
public:
    PATTERN_SINGLETON_DECLARE(CSolar);

    CSolar();
    ~CSolar();

    void Initialize(int argc, char * argv[]);

    void Done();
    // kill(1)
    void Term();

    int Reboot();
    int Shutdown();

    int Restart();

    void haveException(int ExcNo=0, char *pMsg = NULL);
    void Cleanup(int iMode=0);
    void TimerProcDBLive();
    void TimerProcMaintain();
    void TimerProcGetPubIP();

    const std::string &GetVersion();
    const char *GetCopyRights();
    unsigned int GetRunPeriod();
    time_t GetStartTime();

    void MarkOnInternet();
    void MarkOnIntranet();

    bool IsOnInternet();
    const char * GetNatIP();

    bool IsOnIntranet();

private:
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 新增

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int SetPlatform();
    int InitializeLogs();

    void ShowVersion();

    CEZTimer          m_TimerDBAlive;          //数据库保活
    CEZTimer          m_TimerMaintain;          //保活功能定时器

    // 版本字符串
    std::string m_strVer;
    // 启动时间
    unsigned int m_iRunPeriod;

    time_t m_ttStartTime;


    CEZTimer          m_TimerPubIP;          //数据库保活

    CEZMutex m_mutexMark;
    time_t m_ttLastOnInternet;// 上次联网时间
    time_t m_ttLastOnIntranet;// 上次联内网时间
    char m_PubIP[IP4_STR_SIZ];

};

#define g_Solar (*CSolar::instance())

#endif //__Solar_H__
