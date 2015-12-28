/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Solar.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: Solar.cpp 5884 2012-05-16 09:17:46Z WuJunjie $
 *
 *  Explain:
 *     -
 *      主调类
 *     -
 *
 *  Update:
 *     2012-05-16 09:17:46   WuJunjie  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "CommonInclude.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <pjlib.h>
#include <pj/log.h>

#include <ez_fs.h>
#include "Logs.h"

#include "EZThread.h"
#include "EZTimer.h"

#include "Configs/ConfigManager.h"

#include "Solar.h"
#include <app/common/config.h>

#include "app/server/EasyMysqlConn.h"
#include "StringOpr.h"
#include "SmartHomeTelcom/HttpKeepConnectorSmartHomeTelcom.h"


// config fname
const char * const ConfigFileSolar_1= "solar.cfg";
const char * const ConfigFileSolar_2 = "solar.cfg.duplicate";

const char * const copy_right = "@(#) Copyright (c) 2012 tiansu-china.com.  All rights reserved";
const char * const version_string = "v 0.9.5 - ";
const char * const desc="P_2012-7-24.Solar.1010";

#ifdef SMARTHOME_AGENT_JSTELECOM 
const char * const keepDBAliveSQL = "SELECT id,act,fn,rn,dvId,dpId,dn,dtId,jsuId,deId FROM t_trigger_for_datapoint LIMIT 0, 50";
const char * const deleteOldRecordSQL = "DELETE FROM t_trigger_for_datapoint WHERE id in (";
#else
const char * const keepDBAliveSQL = "select version()";
#endif

const char * const INSTANCE_PID_FILE = "/tmp/tshome-p2p-server.pid";

int already_running(const char *filename);

#include "app.h"
#include "./IpcServer/GeneralAgent.h"
#include "IpcServer/GeneralSocketProcessor.h"
#ifdef USE_HTTPKEEPCONNECTORSMARTHOME
#include "SmartHomeTelcom/AgentSmartHomeTelcom.h"

#endif // USE_HTTPKEEPCONNECTORSMARTHOME
#ifdef USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
#include "SmartHomeTelcom/SmartHomeTelcomCtrlServer/AgentSmartHomeTelcomCtrlServer.h"

#endif // 
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
PATTERN_SINGLETON_IMPLEMENT(CSolar);

CSolar::CSolar():m_TimerDBAlive("CSolar-KeepAlive-DB"), m_TimerMaintain("CSolar-Maintain"), lifeTimer()
{
    m_strVer = version_string;
    m_strVer += __DATE__;
    m_strVer += " ";
    m_strVer +=__TIME__;

    m_iRunPeriod = 0;
    m_ttStartTime = time(NULL);
}

CSolar::~CSolar()
{
    g_Config.saveFile();
}

void CSolar::ShowVersion()
{
    printf("---------------------------\n\n");
    printf("%s\n", copy_right);
    printf("%s\n", desc);
    printf("$ Solar: main.c,v 2.6 %s %s Exp $\n\n", __DATE__, __TIME__);
    printf("---------------------------\n");
}

void CSolar::Initialize(int argc, char * argv[])
{
    ShowVersion();
    //SetPlatform();

    g_TimerManager.Start();
    g_ThreadManager.RegisterMainThread(ThreadGetID());

    //g_Config.useDoubleCfgFile(1);
    g_Config.initialize(ConfigFileSolar_1, ConfigFileSolar_2);
    InitializeLogs();
    g_Config.saveFile();

    LOG4CPLUS_INFO(LOG_SOLAR, "Starting(" << desc << ") ...");

//#if 0
    // comment it when development
#if 0
    LOG4CPLUS_INFO(LOG_SOLAR, "is already_running ...");
    if (already_running(INSTANCE_PID_FILE) != 0)
    {
        // important msg, write to console and log files.
        LOG4CPLUS_ERROR(LOG_SOLAR, "Allowed to run only one instance, pls check");
        LOG4CPLUS_INFO(LOG_SOLAR, "Exit ...");

        printf("Allowed to run only one instance, pls check\n");
        printf("Exit ...\n");

        exit(-1);
    }
#endif

    //begin -- start server
    // 启动顺序请慎重修改

    // 开启原服务器
    LOG4CPLUS_INFO(LOG_SOLAR, "StartPJTurnServer ...");
    StartPJTurnServer();

    // 启动处理线程
    LOG4CPLUS_INFO(LOG_SOLAR, "g_GeneralSocketProcessor ...");
    g_GeneralSocketProcessor.Start();

    LOG4CPLUS_INFO(LOG_SOLAR, "g_GeneralAgentTSIPC ...");
    g_GeneralAgentTSIPC.Start(NULL);
    
    //江苏电信版本的相关模块
#ifdef USE_HTTPKEEPCONNECTORSMARTHOME
    g_SmartHomeAgentTelcom.Start();
#endif

#ifdef USE_HTTPSERVERSMARTHOMETELCOMCTRLSERVER
    g_AgentSmartHomeTelcomCtrlServer.Start();
#endif

	// 启动维护定时器
	m_TimerDBAlive.Start(this
	                                , (TIMERPROC)&CSolar::TimerProcDBLive
	                                , 0
	                                , 3 * 1000 /*period ms*/
	                                , 0
	                                , 10*1000
	                               );
	m_TimerMaintain.Start(this
	                                , (TIMERPROC)&CSolar::TimerProcMaintain
	                                , 0
	                                , 1 * 1000 /*每秒执行一次，请勿修改*/
	                                , 0
	                                , 10*1000
	                               );

	
	lifeTimer.start(0, NULL);
    //end -- start server

    // 启动完毕， 保存一次配置文件
    g_Config.saveFile();
    printf("%s Initialize end, running ...\n", argv[0]);
	
    LOG4CPLUS_INFO(LOG_SOLAR, argv[0] << " Initialize end");
}

int pj_runnable;
pj_thread_t *pt;
pj_thread_t *logoutThread;

int sndto_client(void *data);

int logoutUserProc(void * data);
void CSolar::StartPJTurnServer()
{
    CConfigP2P __cfgP2P;
    __cfgP2P.update();
    int iPjSvrPort = __cfgP2P.getConfig().StunPort;

    //char *method = (char *)"main";
    // 同全局变量重名
    pj_turn_srv *ts_pj_srv = NULL;
    int result = 0;
    //pj_status_t status;
    char b_en[16] = { 0 };

    result = ts_pj_server_create(iPjSvrPort, &ts_pj_srv);
    if(NULL == ts_pj_srv)
    {
        //PJ_LOG(4,(__FILE__, "%s:create pj_turn_server failed", method));
        LOG4CPLUS_ERROR(LOG_SOLAR, "ts_pj_server_create:"<<result);
        return;
    }

    //create user database manager
    result = ts_udb_create();
    if (result != TS_SUCCESS)
    {
        LOG4CPLUS_ERROR(LOG_SOLAR, "ts_udb_create:"<<result);
        return;
    }


    strcpy((char *) b_en, AES_PWD);
    result = ipc_handler_init(b_en, sizeof(b_en), g_GeneralSocketProcessor.GetIPC_hander());
    if(result != TS_SUCCESS)
    {
        LOG4CPLUS_ERROR(LOG_SOLAR, "ipc_handler_init:"<<result);
        return;
    }

    void *arg = (void *)ts_pj_srv;
    result = ts_pj_server_start(&arg);
    // 启动数据发送线程
    if(result == TS_SUCCESS && NULL != ts_pj_srv)
    {
        pj_runnable = TS_TRUE;
        //start a new pjsip thread to work
        pj_thread_create(ts_pj_srv->core.pool, ts_pj_srv->obj_name, &sndto_client,
                         ts_pj_srv, 0, 0, &pt);

		pj_thread_create(ts_pj_srv->core.pool, ts_pj_srv->obj_name, &logoutUserProc,
                         ts_pj_srv, 0, 0, &logoutThread);
        //__fline;
        //printf("pj_thread_create\n");
    }
    g_GeneralSocketProcessor.SetPJ_turn_srv(ts_pj_srv);

    LOG4CPLUS_INFO(LOG_SOLAR, "StartPJTurnServer Finished");

    return;
}

int CSolar::SetPlatform()
{
    system("ulimit -c unlimited");

    return 0;
}

// logs of pjlib
static void __pj_log_func(int level, const char *data, int len)
{
    LOG4CPLUS_INFO(LOG_OF_PJLIB, "Lvl:" << level << " Len:" << len << " " <<data);
}

int CSolar::InitializeLogs()
{
    //	初始化日志系统
    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    log4cplus::tstring pattern = LOG4CPLUS_TEXT("%D %-5p %-5c - %m [%l]%n"); // c , model
    SharedAppenderPtr append_1;
    if (__cfgGeneral.getConfig().strLogFileName.size()<=0)
    {
        append_1=new ConsoleAppender();
    }
    else
    {
        std::string strFilePathName = __cfgGeneral.getConfig().strLogFilePath +"/"+  __cfgGeneral.getConfig().strLogFileName;
        printf("Initialize Logs:%s, MaxSize:%d Files:%d - "
			, strFilePathName.c_str()
			, __cfgGeneral.getConfig().maxFileSize
			, __cfgGeneral.getConfig().maxBackupIndex);
        int iret = ez_creat_dir(__cfgGeneral.getConfig().strLogFilePath.c_str());
        if (iret<0)
            printf("failed\n");
        else
            printf("success\n");

        append_1 = new RollingFileAppender(LOG4CPLUS_TEXT(strFilePathName)
                                   , __cfgGeneral.getConfig().maxFileSize
                                   , __cfgGeneral.getConfig().maxBackupIndex);
    }
    append_1->setName(LOG4CPLUS_TEXT("TsHome"));
    append_1->setLayout( std::auto_ptr<Layout>(new PatternLayout(pattern)) );
    Logger::getRoot().addAppender(append_1);
    NDCContextCreator _context(LOG4CPLUS_TEXT("tss"));

    pj_log_set_level(10);
    pj_log_set_log_func( __pj_log_func );

    return 0;
}

void CSolar::Done()
{
    while (1)
    {
        CEZSemaphore sem;
        sem.Pend();
    }
}

void CSolar::Term()
{
    LOG4CPLUS_INFO(LOG_SOLAR, "CSolar::Term");

    return;
}

void CSolar::haveException(int ExcNo, char *pMsg)
{
    LOG4CPLUS_INFO(LOG_SOLAR, "CSolar::haveException:" << ExcNo << " Msg:" << pMsg);

    return;
}

/* set advisory lock on file */
int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;  /* write lock */
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;  //lock the whole file

    return(fcntl(fd, F_SETLK, &fl));
}

int already_running(const char *filename)
{
    int fd;
    char buf[16];

    fd = open(filename, O_RDWR | O_CREAT, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
    if (fd < 0)
    {
        printf("can't open %s: %m\n", filename);
        exit(-1);
    }

    /* 先获取文件锁 */
    if (lockfile(fd) == -1)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            //printf("file: %s already locked\n", filename);
            close(fd);
            return -1;
        }
        printf("can't lock %s: %m\n", filename);
        exit(-1);
    }
    /* 写入运行实例的pid */
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);

    return 0;
}


std::vector<CAgentMsg_initDeviceConfigParam> procDBLiveImpl()
{
	int res = 0;
	MYSQL_RES *res_ptr = NULL;
	//get a mysql connection from pool.
	EasyMysqlConn easyConn;
	MysqlConn conn = easyConn.getConnection();
	MYSQL *mysql = conn.mysql;
	std::vector<CAgentMsg_initDeviceConfigParam> deviceVec;
	if (NULL == mysql)
	{
		__fline;
		printf("in TimerProcDBLive conn is null\n");
		LOG4CPLUS_ERROR(LOG_DB, "pool_getConn");
	}else{
		res = mysql_query(mysql, keepDBAliveSQL);
		if(!res){
#ifdef SMARTHOME_AGENT_JSTELECOM 
			MYSQL_ROW sqlrow;
			res_ptr = mysql_store_result(mysql);
			if(res_ptr)
			{
				std::string deleteSql(deleteOldRecordSQL);
				cout
				<<"id\t|action\t|floorName\t|roomName\t|deviceSN\t|datapointId\t|deviceName\t|deviceType\t|userName\t|deviceId\t|\n";
				cout << "result counter = " << mysql_num_rows(res_ptr) << "\n";
				while((sqlrow = mysql_fetch_row(res_ptr)))
				{
					printf("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", sqlrow[0],sqlrow[1],sqlrow[2],sqlrow[3],sqlrow[4],sqlrow[5],sqlrow[6],sqlrow[7],sqlrow[8],sqlrow[9]);

					deleteSql.append(sqlrow[0]).append(",");

					CAgentMsg_initDeviceConfigParam __device;
					CAgentMsg_initDeviceConfigParam_Seat_deploy __deploy;
					CAgentMsg_initDeviceConfigParam_Seat_deploy_Operate __operate;

					// 设备
					__operate.operate_id	  = sqlrow[5];
					__operate.operate_ranage  = ""; // 开关类的此无需，务必空， 不得填值
					__operate.operate_type	  = "0"; //开关
					__operate.operate_value   = "2"; // 关
					__operate.operate_explain = sqlrow[6];
					ezGBKToUTF8(__operate.operate_explain);
					// 物品
					__deploy.deploy_name = sqlrow[6];
					ezGBKToUTF8(__deploy.deploy_name);

					std::string deploy_type = sqlrow[7];
					if(deploy_type == "4")
						deploy_type = "1";
					else
						deploy_type = "2";
					
					__deploy.deploy_type =deploy_type ;//设备的类型，1：安防设备；2：家居设备

					std::string deviceType = sqlrow[7];
					if(deviceType == "2")
						deviceType = "5";
					else if(deviceType == "3")
						deviceType = "2";
					else if(deviceType == "6")
						deviceType = "3";
					else if(deviceType == "8")
						deviceType = "7";
					else
						deviceType = "0";
					
					__deploy.deviceType  = deviceType;
					__deploy.m_Operate.push_back(__operate);

					__device.m_Seat.m_deploy.push_back(__deploy);
					__device.m_Seat.seat_name = sqlrow[2];
					__device.m_Seat.seat_name +=  sqlrow[3];

					ezGBKToUTF8(__device.m_Seat.seat_name);

					__device.requestMark = sqlrow[1];
					__device.sessionId	= "";
					__device.productID	= "";
					__device.userID 		= sqlrow[8];
					__device.deviceID	= sqlrow[9]; //

					//加入设备列表
					deviceVec.push_back(__device);
				}
				mysql_free_result(res_ptr);
				size_t size = deviceVec.size();
				if(size > 0)
				{
					//deleteSql 删除最后的, 并拼接)，完成sql
					deleteSql.replace(deleteSql.find_last_of(','), 1, 1, ')');
					cout<<"deleteSql=" << deleteSql<<endl;
					//删除已取出的记录
					res = mysql_query(mysql, deleteSql.c_str());
				}

			}
#else
			res_ptr = mysql_use_result(mysql);
			if(res_ptr)
				mysql_free_result(res_ptr);
#endif
		}
	}
	return deviceVec;
}

void CSolar::TimerProcDBLive()
{
#ifdef SMARTHOME_AGENT_JSTELECOM 
	std::vector<CAgentMsg_initDeviceConfigParam> deviceVec = procDBLiveImpl();
	//将记录上报电信服务器
	Socket *p_01 = g_SmartHomeAgentTelcom.GetSocketByName(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME);
    CHttpKeepConnectorSmartHomeTelcom *pHttpKeepConnectorSmartHomeTelcom = dynamic_cast<CHttpKeepConnectorSmartHomeTelcom *>(p_01);
    
    if (pHttpKeepConnectorSmartHomeTelcom)
    {
		std::vector<CAgentMsg_initDeviceConfigParam>::iterator p;
        for(p = deviceVec.begin(); p != deviceVec.end(); p++)
        {
			// upDown 1 上线 0 下线	2更新	
			int upDown = -1;
			if(p->requestMark == "add")
				upDown = 1;
			else if(p->requestMark == "delete")
				upDown = 0;
			else if(p->requestMark == "update")
				upDown = 2;
				
			if(upDown != -1)
	        	pHttpKeepConnectorSmartHomeTelcom->reportDeviceList(*p, upDown);
        }
    }
#else
	procDBLiveImpl();
#endif
}

void CSolar::TimerProcMaintain()
{
    // 为了计数准确，不依赖系统时间，自己维护
    m_iRunPeriod ++;
}

const std::string & CSolar::GetVersion()
{
    return m_strVer;
}

unsigned int CSolar::GetRunPeriod()
{
    return m_iRunPeriod;
}

time_t CSolar::GetStartTime()
{
    return m_ttStartTime;
}

