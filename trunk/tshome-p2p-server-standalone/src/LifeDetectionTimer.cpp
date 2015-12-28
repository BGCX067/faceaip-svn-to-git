
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
#include "time.h"

#include <pjlib.h>
#include <pj/log.h>

#include <ez_fs.h>
#include "Logs.h"

#include "EZThread.h"
#include "EZTimer.h"

#include "Configs/ConfigManager.h"
#include "Configs/ConfigP2P.h"

#include "LifeDetectionTimer.h"
#include <app/common/config.h>


#include "app.h"
#include "./IpcServer/GeneralAgent.h"
#include "IpcServer/GeneralSocketProcessor.h"

#include <vector>
#include <iostream>

using namespace std;
//心跳请求提前次数
#define AHEAD_TIMES 2

extern ts_udb_mgr g_udb_mgr;

void logoutUser(ts_user * user);

LifeDetectionTimer::LifeDetectionTimer()
{
	hostPeriod = 0;
	termPeriod = 0;
	hostTimeout = 0;
	termTimeout = 0;
}

LifeDetectionTimer::~LifeDetectionTimer()
{
}

void LifeDetectionTimer::start(int argc, char * argv[])
{
	CConfigP2P m_configP2P;
	m_configP2P.update();
	hostPeriod = m_configP2P.getConfig().HostCheckPeriod;
	termPeriod = m_configP2P.getConfig().TermCheckPeriod;
	hostTimeout = m_configP2P.getConfig().HostTimeout;
	termTimeout = m_configP2P.getConfig().TermTimeout;


	if(hostPeriod > 0)
	{
		// 主机维护定时器
		m_detectionHostTimer.Start(this
		                                , (TIMERPROC)&LifeDetectionTimer::TimerProcHost
		                                , 0
		                                , hostPeriod*1000
		                                , 0
		                                , 20*1000);
		cout << "Host Detection Timer started, period=" << hostPeriod <<"s,hostTimeout="<< hostTimeout <<"s>>>>>>>>>\n";
	}
	
	if(termPeriod > 0)
	{
		// 主机维护定时器
		m_detectionTermTimer.Start(this
		                                , (TIMERPROC)&LifeDetectionTimer::TimerProcTerm
		                                , 0
		                                , termPeriod*1000
		                                , 0
		                                , 20*1000);
		
		cout << "Term Detection Timer started, period=" << termPeriod <<"s,termTimeout="<< termTimeout<<"s>>>>>>>>>\n";
	}

	//TODO web目前已由本端心跳模块维护，同样使用solar.cfg中的配置。
}


void LifeDetectionTimer::setHostPeriod(int period)
{
	hostPeriod = period;
}

int LifeDetectionTimer::getHostPeriod()
{
	return hostPeriod;
}

void LifeDetectionTimer::setTermPeriod(int period)
{
	termPeriod = period;
}

int LifeDetectionTimer::getTermPeriod()
{
	return termPeriod;
}

int LifeDetectionTimer::getHostWarningLimit()
{
	return (hostTimeout - hostPeriod*AHEAD_TIMES);
}

int LifeDetectionTimer::getTermWarningLimit()
{
	return (termTimeout - termPeriod*AHEAD_TIMES);
}


void LifeDetectionTimer::TimerProcHost()
{
	struct timeval t_a, t_b;//handle takes time
	double tTime = 0;
	gettimeofday(&t_a, NULL);

	std::vector<ts_user> host_list;
    int result = g_udb_mgr.home.get_allHosts(host_list);
    if(result == TS_SUCCESS && host_list.size() > 0)
    {
    	int warningLinit = getHostWarningLimit();
    	for(size_t i = 0; i < host_list.size(); i++){
			ts_user user = host_list[i];
        	time_t liveTime = user.liveTime;
			time_t now = time(NULL);
			if((now - liveTime) < warningLinit)
			{
				//空代码块，为了提高效率。
			}
			else if((now - liveTime) < hostTimeout && (now - liveTime) >= warningLinit)//即将失效
			{
				user_sdp sdp = user.sdpVec[0];
				heartbeat_req_t req;
				memset(&req, 0, sizeof(heartbeat_req_t));
				int body_len = sizeof(heartbeat_req_t) - sizeof(remote_header_t);
				init_common_header(&req.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len, HEARTBEAT, REQUEST, SERVER);
				req.session_id = user.session_id;
				g_GeneralSocketProcessor.sendHeartbeat(sdp.addr, sdp.port, &req);
				LOG4CPLUS_DEBUG(LOG_TURN, "User("<< user.username << ") "<< sdp.addr << ":" << sdp.port <<" will be timeout");
			}
			else//已经失效
			{
				LOG4CPLUS_DEBUG(LOG_TURN, "User("<< user.username << ") timeout");
				logoutUser(&user);
			}
			
    	}
    }
	gettimeofday(&t_b, NULL);
    tTime = (t_b.tv_sec - t_a.tv_sec) * 1000 + (t_b.tv_usec - t_a.tv_usec)/1000.0;
    LOG4CPLUS_DEBUG(LOG_TURN, "TimerProcHost takes:"<< tTime << " ms");
}

void LifeDetectionTimer::TimerProcTerm()
{
	struct timeval t_a, t_b;//handle takes time
	double tTime = 0;
	gettimeofday(&t_a, NULL);
	
	std::vector<ts_user> mobile_list;
	int result = g_udb_mgr.home.get_allMobiles(mobile_list);
	if(result == TS_SUCCESS && mobile_list.size() > 0)
	{
		int warningLinit = getTermWarningLimit();
		for(size_t i = 0; i < mobile_list.size(); i++){
			ts_user user = mobile_list[i];
			time_t liveTime = user.liveTime;
			time_t now = time(NULL);
			if((now - liveTime) < warningLinit)
			{
				//空代码块，为了提高效率。
			}
			else if((now - liveTime) < termTimeout && (now - liveTime) >= warningLinit)//即将失效
			{
				user_sdp sdp = user.sdpVec[0];
				heartbeat_req_t req;
				memset(&req, 0, sizeof(heartbeat_req_t));
				int body_len = sizeof(heartbeat_req_t) - sizeof(remote_header_t);
				init_common_header(&req.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len, HEARTBEAT, REQUEST, SERVER);
				req.session_id = user.session_id;
				g_GeneralSocketProcessor.sendHeartbeat(sdp.addr, sdp.port, &req);
				LOG4CPLUS_DEBUG(LOG_TURN, "User("<< user.username << ") "<< sdp.addr << ":" << sdp.port <<" timeout");
			}
			else//已经失效
			{
				LOG4CPLUS_DEBUG(LOG_TURN, "User("<< user.username << ") timeout");
				logoutUser(&user);
			}
		}
	}
	gettimeofday(&t_b, NULL);
    tTime = (t_b.tv_sec - t_a.tv_sec) * 1000 + (t_b.tv_usec - t_a.tv_usec)/1000.0;
    LOG4CPLUS_DEBUG(LOG_TURN, "TimerProcTerm takes:"<< tTime << " ms");
}

void LifeDetectionTimer::TimerProcWeb()
{
	//TODO
}

