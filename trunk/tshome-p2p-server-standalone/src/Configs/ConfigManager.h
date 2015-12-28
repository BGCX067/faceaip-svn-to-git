/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigManager.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: ConfigManager.h 5884 2012-07-02 09:15:02Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __CONFIG_MANAGER_H__
#define __CONFIG_MANAGER_H__

// 以zip压缩格式存放
// 这里注释， 以便手工修改， 2012-6-21 16:36:36 wujunjie 
// #define HAVE_ZLIB_ZFILE

#include <string.h>
#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"
#include <json.h>

#ifdef HAVE_ZLIB_ZFILE
#include "zlib.h"
#else
#include "File.h"
#endif //HAVE_ZLIB_ZFILE

class CSaveDelay : public CEZThread
{
public:
    CSaveDelay();
    ~CSaveDelay();
    int Start();
    int Stop();
    void ThreadProc();

};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "ConfigGeneral.h"
#include "ConfigP2P.h"
#include "ConfigIPC.h"
#include "ConfigDB.h"
#ifdef SMARTHOME_AGENT_JSTELECOM
#include "ConfigSmartHomeTelcom.h"
#endif

typedef enum __cfg_index_t {
    CFG_IDX_GENERAL =  0,	// 普通
    CFG_IDX_IPC,			// 同web通讯
    CFG_IDX_P2P,			// p2p配置
    CFG_IDX_DB,				// 数据库配置
    
#ifdef SMARTHOME_AGENT_JSTELECOM
    CFG_IDX_SMARTHOMETELCOM,				// 江苏电信智慧e家
#endif
    CFG_IDX_ALL,			// 实际没有这种配置， 便于恢复全部等用
}CFG_INDEX;

#define CFG_NAME_GENERAL "General"
#define CFG_NAME_IPC "IPC"
#define CFG_NAME_P2P "P2P"
#define CFG_NAME_DB "DB"
#ifdef SMARTHOME_AGENT_JSTELECOM
#define CFG_NAME_SMARTHOME_JSTELECOM "SMARTHOME_JSTELECOM"
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

class CConfigManager : public CEZObject
{
private:
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //各个子配置
    CConfigGeneral		m_configGeneral;	/*!< 普通配置 */
    CConfigIPC			m_configIPC;	/*!< IPC */
    CConfigP2P			m_configP2P;	/*!< dsd */
    CConfigDB			m_configDB;	/*!< dsd */
#ifdef SMARTHOME_AGENT_JSTELECOM
    CConfigSmartHomeTelcom			m_configSmartHomeTelcom;	/*!< dsd */
#endif
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

private:
    CConfigManager();
    ~CConfigManager();

    typedef std::map<std::string, CConfigExchange*> CONFIG_MAP;
    typedef std::map<int, std::string> CONFIG_MAPR;
    typedef std::map<std::string, int> MAP_TRANSLATE;
    typedef std::map<std::string, int>::value_type valueType;

public:
    PATTERN_SINGLETON_DECLARE(CConfigManager);

    //! 属性设置
    // iDoubleCfgFile 0 关闭双配置功能， 默认关闭， 1 开启双配置功能
    void useDoubleCfgFile(int iDoubleCfgFile = 1);
    //! 初始化
    void initialize(std::string mfile="", std::string sfile="");
    int recallConfig(const char* name, const char* user = NULL, int index = -1);
    int recallConfigAll(const char* user = NULL);
    //! 保存文件
    void saveFile();
    //! 设置默认配置，供GUI界面和网络模块调用
    int SetDefaultConfig(int iConfigType);

private:
    void setupConfig(const char* name, CConfigExchange& xchg);
    void onConfigChanged(CConfigExchange& xchg, int& ret);
    //CConfigTable& locate(CConfigTable& table, const char* name);
    const char* nameFromID(int id);
    CConfigExchange* xchgFromName(const char* name);
    int readConfig(const char* chPath, std::string& input);
    void onTimer(PARAM arg);
    //!恢复网络端不是保存在配置文件中的配置
    int setDefaultNetConfig();

private:
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
    int m_iDoubleCfgFile;
};

#define g_Config (*CConfigManager::instance())

#endif //__CONFIG_MANAGER_H__

