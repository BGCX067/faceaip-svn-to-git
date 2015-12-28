/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigMan.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigMan.h 5884 2012-07-02 09:15:02Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __CONFIG_MAN_H__
#define __CONFIG_MAN_H__

// 以zip压缩格式存放
// 这里注释， 以便手工修改， 2012-6-21 16:36:36 wujunjie
// #define HAVE_ZLIB_ZFILE

#include <string.h>
#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"
#include <json.h>
#include "ConfigBase.h"

//#ifdef HAVE_ZLIB_ZFILE
#include "zlib.h"
//#else
#include "File.h"
//#endif //HAVE_ZLIB_ZFILE

class CConfigMan;
class CSaveDelay : public CEZThread
{
public:
    CSaveDelay();
    ~CSaveDelay();
    int Start(CConfigMan * pCConfigMan);
    int Stop();
    void ThreadProc();
private:
    CConfigMan *m_pCConfigMan;
};
class CConfigMan : public CEZObject
{
public:
    CConfigMan();
    virtual ~CConfigMan();
    //! 初始化
    // 不传参数使用默认名字，且为单文件方式
    // 传一个参数使用该名字，为单文件方式
    // 传两个参数使用该名字，为双文件方式
    virtual int initialize(std::string mfile="", std::string sfile="");
    //! 保存文件
    void saveFile();
    int recallConfig(const char* name, const char* user = NULL, int index = -1);
    int recallConfigAll(const char* user = NULL);

protected:
    typedef std::map<std::string, CConfigExchange*> CONFIG_MAP;
    typedef std::map<long, std::string> CONFIG_MAPR; // ID, Name, id use obj addr
    typedef std::map<std::string, long> MAP_TRANSLATE; // insert(valueType(CFG_NAME_General, CFG_IDX_General));
    //typedef std::map<std::string, int>::value_type valueType;// MAP_TRANSLATE val

    CConfigTable m_configAll;				/*!< 配置总表 */
    int m_changed;							/*!< 配置表变化了 */
#ifdef HAVE_ZLIB_ZFILE

    gzFile m_fileConfig;					/*!< 配置文件 */
#else

    CFile m_fileConfig;
#endif

    CONFIG_MAP m_map;						/*!< 配置名称和配置类指针映射表 */
    CONFIG_MAPR m_mapReverse;				/*!< 配置ID和配置名称和映射表 */

    MAP_TRANSLATE m_mapTranslate;			/*!< 配置名称和配置类型映射表*/

    CEZTimer m_Timer;
    std::string m_stream;	// 字符串流

    CSaveDelay				m_saveDelay;

    std::string m_strConfigFileFirst;
    std::string m_strconfigFileSecond;
    // 是否双文件 1是，默认不是
    int m_iDoubleCfgFile;

    // 装载配置
    void setupConfig(const char* name, long cfgIDX, CConfigExchange& xchg);
    void onConfigChanged(CConfigExchange& xchg, int& ret);
    //CConfigTable& locate(CConfigTable& table, const char* name);
    const char* nameFromID(int id);
    CConfigExchange* xchgFromName(const char* name);
    int readConfig(const char* chPath, std::string& input);
    void onTimer(PARAM arg);
};

#endif //__CONFIG_MAN_H__
