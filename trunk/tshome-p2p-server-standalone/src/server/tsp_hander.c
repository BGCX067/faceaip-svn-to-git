/*
 * tsp_handler.c
 *
 *  Created on: 2012-2-18
 *      Author: sunzq
 */
#include <app/common/config.h>

#include "CommonInclude.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mysql/mysql.h>
#include <pthread.h>
#include <pjlib.h>
#include "app.h"
#include "EZThread.h"
#include "../IpcServer/GeneralSocketProcessor.h"
#if defined( SMARTHOME_AGENT_JSTELECOM) && defined( USE_HTTPKEEPCONNECTORSMARTHOME)
#include "../SmartHomeTelcom/HttpKeepConnectorSmartHomeTelcom.h"
#include "../SmartHomeTelcom/AgentSmartHomeTelcom.h"
#endif

#include "string.h"
#include "str_opr.h"
#include "../Logs.h"
#include "time.h"
#include <queue>
extern ts_udb_mgr g_udb_mgr;

static tsp_handler *this_tsp_handler = NULL;


// 标记，注意下列三个务必定义为字符串，代码中用strlen取长度
const char * const hostToken = "SDPH";		// 主机向服务器发送
const char * const clientTokenWan = "SDPL";// 外网 终端向服务器发送
const char * const clientTokenLan = "SDPIL";// 内网模式，终端直接向主机发送
// 最小数据包定义
const size_t MIN_PACKET_LEN = 7;
const int SYSTEM_PERMISSION_STOP_HOST_NET = 0x80;
        
CEZMutex g_mutexCommunicationresponse;
CEZMutex g_mutexTspHandlerDestroy;
CEZMutex g_mutexTspHandlerLogout;
CEZMutex g_mutexTspHandlerRequest;

// 扩大，防止越界
static u_char send_buff[1500];		//send to web IPC server.

//for log
//static char *method;					//current method name,used for logging.
static char msg[1024];					//message for logging.

static std::queue<ts_user> logoutQueue;
static sem_t sem_logout;
extern int pj_runnable;
extern pj_thread_t *logoutThread;


/**************prototype**************/
static void set_rsa(int field, void *rsa);

static RSA * init_rsa();

static int handle_remote_request(pj_turn_srv *srv,
                                 const char *userName,
                                 const unsigned int un_len,
                                 const pj_uint8_t *rcv_buff,
                                 const unsigned int rcv_len,
                                 const char *addr,
                                 const unsigned int addr_len,
                                 const unsigned int port,
                                 pj_uint8_t *snd_buff,
                                 unsigned int *snd_len,
                                 user_sdp *sdp);

static int handle_remote_response(pj_turn_srv *srv,
                                  const unsigned char *rcv_buff,
                                  const unsigned int rcv_len);

static int handle_remote_timeout(pj_turn_srv *srv,
                                 const char *username,
                                 const unsigned int un_len,
                                 const char *ip,
                                 const int ip_len,
                                 const int port,
                                 user_sdp *sdp);

static int handle_remote_destroy(pj_turn_srv *srv,
                                 const char *username,
                                 const unsigned int un_len,
                                 const char *ip,
                                 const int ip_len,
                                 const int port,
                                 user_sdp *sdp);


/********************public API********************/

pj_status_t tsp_handler_init(pj_turn_srv *pj_srv)
{
    assert(pj_srv != NULL);
    pj_srv->core.handler.rsa = init_rsa();
    //set callbacks
    pj_srv->core.handler.handle_request = &handle_remote_request;
    pj_srv->core.handler.handle_response = &handle_remote_response;
    pj_srv->core.handler.handle_timeout = &handle_remote_timeout;
    pj_srv->core.handler.handle_destroy = &handle_remote_destroy;
    pj_srv->core.handler.update = &set_rsa;

    this_tsp_handler = &pj_srv->core.handler;

	int ini2 = sem_init(&sem_logout, 0, 0);
    if(ini2 != 0)
    {
        LOG4CPLUS_ERROR(LOG_TURN, "sem init failed");
        exit(1);
    }
    return PJ_SUCCESS;
}




/*
set a RSA instance
*/
static void set_rsa(int field, void *r)
{
    (void)field;
    this_tsp_handler->rsa = (RSA *)r;
}

/*
get the current RSA instance.
*/
RSA * init_rsa()
{
    RSA *rsa = NULL;
    rsa = rt_create_rsa();
    return rsa;
}

// if return TS_INVALID, not login, otherwise logined
static int has_login(const int sessionId)
{
    return g_udb_mgr.mapping.get_home_id(sessionId);
}
/*
fill error response
*/
static void fill_error_res(u_char *snd_buff, unsigned int *snd_len, unsigned short fun_code,
                           int sessionId, unsigned short error_code)
{
    global_error_t err;
    memset(&err, 0 , sizeof(err));
    int body_len = sizeof(global_error_t) - sizeof(remote_header_t);
    init_common_header(&err.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len, fun_code,
                       ERROR, SERVER);
    err.error_code = error_code;
    err.session_id = sessionId;
    memset((char *)err.space, 0 , sizeof(err.space));
    fill_global_error(snd_buff, &err);
    *snd_len = sizeof(global_error_t);
}

ts_user *find_user(const char *username, unsigned u_len, ts_user &NewUser)
{
    ts_user *user = g_udb_mgr.user.search_user(username, u_len, NewUser);
	
    return user; 
}

/**
 *将与指定的终端配对的主机sdp恢复成可重复使用状态。
 *homeId:
 *username:终端用户名
 */
static int restore_host_sdp(int homeId, const char *username, unsigned u_len, user_sdp *sdp)
{
	std::vector<ts_user> host_list;
	int result = g_udb_mgr.home.get_hosts(homeId, host_list);
	if(result == 0 && host_list.size() > 0)
	{
		//at present, one home just have one host
		ts_user host_user = host_list[0];

		if(host_user.sdpVec.size() > 0)
		{
			size_t i = 0;
			for(; i < host_user.sdpVec.size(); i++)
			{
				if(!strcmp((const char *)host_user.sdpVec[i].client_name,(const char *)username))
				{
					//printf("in restore_host_sdp, host_list[0].sdpVec[%d].flag=%c\n", i, host_list[0].sdpVec[i].flag);
					host_user.sdpVec[i].flag = 's';/*single*/
					memset(host_user.sdpVec[i].client_name, 0, sizeof(host_user.sdpVec[i].client_name));
					//m_mutexCommunicationStatus.Enter();
					host_list[0] = host_user;
					//printf("in restore_host_sdp, host_list[0].sdpVec[%d].flag=%c\n", i, host_list[0].sdpVec[i].flag);
					g_udb_mgr.home.set_hosts(homeId, host_list);
					//m_mutexCommunicationStatus.Leave();
					if(NULL != sdp){
						memcpy(sdp, &(host_user.sdpVec[i]), sizeof(user_sdp));
					}else{
						LOG4CPLUS_DEBUG(LOG_TURN, "restore sdp is null, can't copy");
					}
					break;
				}
			}
		}
	}
	return TS_SUCCESS;
}

static int do_when_logout(const char *username, unsigned u_len, user_sdp *sdp)
{
    CEZGuard guard(g_mutexTspHandlerLogout);
    LOG4CPLUS_DEBUG(LOG_TURN, "do_when_logout");

    if(NULL != username)
    {
        ts_user user;
        ts_user *res = find_user(username, u_len, user);
        if(NULL != res)
        {
            //method = "do_when_logout";
            int sessionId = user.session_id;
            int homeid = g_udb_mgr.mapping.get_home_id(sessionId);

            if(homeid > 0)
            {
                sprintf(msg, "destroy host info for user(%s),homeId=%d,sessionId=%d", username,homeid,sessionId);
	            LOG4CPLUS_DEBUG(LOG_TURN, msg);
	            g_udb_mgr.home.remove_host(homeid, sessionId);
				
#if defined( SMARTHOME_AGENT_JSTELECOM) && defined( USE_HTTPKEEPCONNECTORSMARTHOME)

                //上报给电信服务器
                std::string deviceId;
                int res = g_udb_mgr.jstelcom.get_deviceId_by_homeId(homeid, deviceId);
                if(res == TS_SUCCESS)
                {
                   Socket *p_01 = g_SmartHomeAgentTelcom.GetSocketByName(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME);
                   CHttpKeepConnectorSmartHomeTelcom *pHttpKeepConnectorSmartHomeTelcom = dynamic_cast<CHttpKeepConnectorSmartHomeTelcom *>(p_01);
                   
                   if (pHttpKeepConnectorSmartHomeTelcom)
                   {	
                   	// OnOFF 1 开 0 关
                       pHttpKeepConnectorSmartHomeTelcom->reportCtrlDeviceState(deviceId, 0);
                   }
                }
                
#endif
	            sprintf(msg, "destroy mobile info for user(%s),homeId=%d,sessionId=%d", username,homeid,sessionId);
	            LOG4CPLUS_DEBUG(LOG_TURN, msg);
	            g_udb_mgr.home.remove_mobile(homeid, sessionId);
            }
			else
			{
				homeid = user.home_id;
			}

            sprintf(msg, "destroy temp info for user(%s),homeId=%d,sessionId=%d", username,homeid,sessionId);
            LOG4CPLUS_DEBUG(LOG_TURN, msg);
            g_udb_mgr.user.remove_usr(username, u_len);

            sprintf(msg, "destroy sessionId-homeId mapping info for user(%s),homeId=%d,sessionId=%d", username,homeid,sessionId);
            LOG4CPLUS_DEBUG(LOG_TURN, msg);
            g_udb_mgr.mapping.remove_mapping(sessionId);

            //set sdp.flag = 's',be reuseful
            restore_host_sdp(homeid, username, u_len, sdp);
            return TS_SUCCESS;
        }
        else
        {
            sprintf(msg,"user(%s) is not exist!\n", username);
            LOG4CPLUS_DEBUG(LOG_TURN, msg);
            return TS_FAILED;
        }
    }
    return NULL;
}

int logoutUserProc(void * data)
{
	while(pj_runnable)
    {
        sem_wait(&sem_logout);
		ts_user user = logoutQueue.front();
		logoutQueue.pop();
		pj_turn_allocation_destroy_by_user(g_GeneralSocketProcessor.GetPJ_server(), &user);
		do_when_logout(user.username, strlen(user.username), NULL);
    }
    pj_thread_join(logoutThread);
    pj_thread_destroy(logoutThread);
    return 0;
}

void logoutUser(ts_user *user)
{
	if(NULL != user)
	{
		logoutQueue.push(*user);
		sem_post(&sem_logout);
	}
}


static void refreshUserAliveTime(ts_user *user, u_char type)
{
	if(type == HOST_S)
	{
		user->liveTime = time(NULL);
		g_udb_mgr.home.save_host(user->home_id, user);
	}
	else if(type == CLIENT)
	{
		user->liveTime = time(NULL);
		g_udb_mgr.home.save_mobile(user->home_id, user);
	}
}


/*
called by pjnath structs, used to handle a protocol datagram.
*/
static int handle_remote_request(pj_turn_srv *srv,
                                 const char *userName,
                                 const unsigned int un_len,
                                 const unsigned char *rcv_buff,
                                 const unsigned int rcv_len,
                                 const char *addr,
                                 const unsigned int addr_len,
                                 const unsigned int port,
                                 unsigned char *snd_buff,
                                 unsigned int *snd_len,
                                 user_sdp *sdp)
{
    CEZGuard guard(g_mutexTspHandlerRequest);
    struct timeval t_a, t_b;//handle takes time
    double tTime = 0;
    gettimeofday(&t_a, NULL);
    char username[50] = {0};
    memcpy(username,userName,un_len);
    size_t send;
    if(rcv_buff && rcv_len >= MIN_PACKET_LEN)
    {
        // sdph: sdp of host

        char sdpSearial[10];
        int serial = 0;
        int maxSize = 0;
		
        if (!strncasecmp((const char *)hostToken, (const char *)rcv_buff, strlen(hostToken)))
        {
            strncpy(sdpSearial, (const char *)(rcv_buff+5), sizeof(sdpSearial)-1);
            serial = atoi(sdpSearial);

            ts_user user;
            ts_user *res = find_user(username, un_len, user);
            if(NULL == res)
            {
                int result = g_udb_mgr.user.select_user(username, un_len, user);
                if(result != 0)
                {
                    fill_error_res((u_char *)snd_buff, snd_len, 0, 0, GE_NO_USER);
                    return PJ_SUCCESS;
                }

                maxSize = user.home.maxUsers/2 + 1;
                user_sdp init_sdp;
                for(int i = 0; i < maxSize; i++)
                {
                    user.sdpVec.push_back(init_sdp);
                }
				
            }

            int home__id = user.home_id;
            LOG4CPLUS_DEBUG(LOG_TURN, "SDPH(" <<serial<< ") case, username=" << username << ", homeid=" <<home__id );

            //the new sdp.
            user_sdp sdp4vec;
            memcpy(sdp4vec.addr, addr, addr_len);
            memcpy(sdp4vec.sdp, rcv_buff, rcv_len);
            sdp4vec.port = port;
            sdp4vec.flag = 's';
			LOG4CPLUS_DEBUG(LOG_TURN, " sdp4Vec.sdp=" << sdp4vec.sdp << ",port=" << sdp4vec.port );
            int vecSize = user.sdpVec.size();
            if(serial >= vecSize)
            {
                LOG4CPLUS_DEBUG(LOG_TURN, " in SDPH case, too many sdp" << ", homeid=" <<home__id );
                return 0;
            }
            if(serial == 0)
            {//login
                sdp4vec.flag = 'p';
                user.reference = 0;
				strcpy(sdp4vec.client_name, "server");
            }


            int cmped = 0;//sdp是否重复
            if (vecSize > 0)
            {
                if(strncasecmp(user.sdpVec[serial].sdp, sdp4vec.sdp, sizeof(sdp4vec.sdp)) == 0)
                {
                    cmped = 1;
                }
            }
            /*
              *if an user relogin, and his allocation still save in server memery, we must destroy 
              *his old allocation to free some memery and socket resource, in order to let other user
              *can reuse this resource.
              */
            if(!cmped)
            {
                user_sdp oldSdp = user.sdpVec[serial];
                if(oldSdp.port != 0){
					pj_turn_allocation *oldAlloc = pj_turn_allocation_find_by_sdp(srv, &oldSdp);
                    pj_turn_allocation_shutdown(oldAlloc);
                	user.reference--;
                }
                user.sdpVec[serial] = sdp4vec;
                user.reference++;
				//临时用户表和登录用户表同步更新。
				g_udb_mgr.tempUserMap[username] = user;
				if(has_login(user.session_id) != TS_INVALID)
				{
					g_udb_mgr.home.save_host(home__id, &user);
				}
            }


            *snd_len = 0;//if this is a host, response nothing
        }
        else if(!strncasecmp((const char *)clientTokenWan, (const char *)rcv_buff, strlen(clientTokenWan)))
        {
            // sdpl: sdp of terminal
            
            ts_user user;
            ts_user *res= find_user(username, un_len, user);
            if(NULL == res)
            {
                int result = g_udb_mgr.user.select_user(username, un_len, user);
                if(result != 0)
                {
                    fill_error_res((u_char *)snd_buff, snd_len, 0, 0, GE_NO_USER);
                    return PJ_SUCCESS;
                }
                user_sdp init_sdp;
                user.sdpVec.push_back(init_sdp);
            }
            int home__id = user.home_id;
            LOG4CPLUS_DEBUG(LOG_TURN, " in SDPL case, username=" << username << ", homeid=" <<home__id );

            user_sdp sdp4vec;
            memcpy(sdp4vec.addr, addr, addr_len);
            memcpy(sdp4vec.sdp, rcv_buff, rcv_len);
            sdp4vec.port = port;
            sdp4vec.flag = 's';

            int vecSize = user.sdpVec.size();
            if(vecSize == 0)
            {//login
                sdp4vec.flag = 'p';
                user.reference = 0;
            }

            int cmped = 0;//sdp是否重复
            if (vecSize > 0)
            {
                int i = 0;
                for(; i < vecSize; i++)
                {
                    if(strncasecmp(user.sdpVec[i].sdp, sdp4vec.sdp, sizeof(sdp4vec.sdp)) == 0)
                    {
                        cmped = 1;
                        break;
                    }
                }
            }
            /*
              *if an user relogin, and his allocation still save in server memery, we must destroy 
              *his old allocation to free some memery and socket resource, in order to let other user
              *can reuse this resource.
              */
            if(!cmped)
            {
                user_sdp oldSdp = user.sdpVec[0];
                if(oldSdp.port != 0){
					
					//处理登出
					user_sdp host_sdp;
					int res = do_when_logout(username, un_len, &host_sdp);
					if(res == TS_SUCCESS){
						//向旧的终端发出被挤掉的提示。
						u_char buf[500] = {0}; 
						unsigned int len = 0;
						fill_error_res(buf, &len, GLOBAL_ERROR, user.session_id, GE_RE_LOGIN);
						send_data_to_client (srv,
	                                         (const u_char *)buf,
	                                         len,
	                                         //PJ_FALSE,
	                                         oldSdp.addr,
	                                         oldSdp.port);
						//pj_turn_allocation_restore(pj_turn_allocation_find_by_sdp(srv, &host_sdp));
					}
					
	                pj_turn_allocation_shutdown(pj_turn_allocation_find_by_sdp(srv, &oldSdp));
                }
                user.sdpVec[0] = sdp4vec;
                user.reference = 1;
				g_udb_mgr.tempUserMap[username] = user;
            }

            std::vector<ts_user> user_list ;
            int result = g_udb_mgr.home.pair_user(home__id, 1, user_list);
            if(result == 0 && user_list.size() > 0)
            {
                //at present, one home just have one host
                ts_user host_user = user_list[0];
                LOG4CPLUS_DEBUG(LOG_TURN, "in case sdpl: host_user name="<<host_user.username<<",homeId="<<host_user.home_id);
                user_sdp sdp2;
				LOG4CPLUS_DEBUG(LOG_TURN, "sdp2.port="<<sdp2.port);
                if(host_user.sdpVec.size() > 0)
                {
                    size_t i = 0;
                    for(; i < host_user.sdpVec.size(); i++)
                    {
                    	LOG4CPLUS_DEBUG(LOG_TURN, "host_user.sdpVec["<<i<<"].flag="<<host_user.sdpVec[i].flag);
                    	LOG4CPLUS_DEBUG(LOG_TURN, "host_user.sdpVec["<<i<<"].client_name="<<host_user.sdpVec[i].client_name);
                    	LOG4CPLUS_DEBUG(LOG_TURN, "host_user.sdpVec["<<i<<"].port="<<host_user.sdpVec[i].port);
						
                        if(host_user.sdpVec[i].flag == 's')
                        {
                            host_user.sdpVec[i].flag = 'p';/*paired*/
                            memcpy(host_user.sdpVec[i].client_name, username, strlen(username));
                            user_list[0] = host_user;
                            g_udb_mgr.home.set_hosts(home__id, user_list);
                            sdp2 = host_user.sdpVec[i];
                            break;
                        }
                    }

                }

				LOG4CPLUS_DEBUG(LOG_TURN, "sdp2.port="<<sdp2.port);
                if(0 != sdp2.port)
                {
                    //send a mobile sdp data to host
                    send_data_to_client (srv,
                                         rcv_buff,
                                         rcv_len,
	                                     //PJ_FALSE,
                                         sdp2.addr,
                                         sdp2.port);
                    //fill the send buffer with a host sdp data
                    *snd_len = strlen((const char *)sdp2.sdp)+1;
                    memcpy(snd_buff, sdp2.sdp, *snd_len - 1);
                    sprintf(msg, "Paired for %s:%d,%s,SDPH is%s,\naddr=%s,port=%d",sdp4vec.addr, sdp4vec.port, sdp4vec.sdp, sdp2.sdp, sdp2.addr, sdp2.port);
                    LOG4CPLUS_DEBUG(LOG_TURN, msg );
                    LOG4CPLUS_DEBUG(LOG_TURN, "paired successful" );
                }
                else
                {
                    *snd_len = 0;
                    LOG4CPLUS_DEBUG(LOG_TURN, "No valid SDPH for this paired");
                    //此处应该回复连接上限的错误
                    fill_error_res((u_char *)snd_buff, snd_len, 0, 0, GE_OVER_MAX_CONNECT_COUNT);
                }
            }
            else
            {
                LOG4CPLUS_DEBUG(LOG_TURN, "no host user");
                fill_error_res((u_char *)snd_buff, snd_len, 0, 0, GE_NO_HOST_CONNECT);
            }
        }
        else
        {
            remote_header_t header;
            memset(&header, 0, sizeof(remote_header_t));
            u_char *recv_buff = (u_char *)(rcv_buff);
            DEB_CODE(
                __fline;printf("recv_buff\n");
                dumpBuffer(stdout, (unsigned char *)recv_buff,
                           rcv_len,
                           SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
            );
            if(pass_common_header(recv_buff, &header))
            {
                // parse a header, if receive buffer is null,return now.
                return PJ_SUCCESS;
            }

            if(header.protocol_type != 0)
            {
                //if the datagram is not a remote protocol type, do nothing, return now.
                return PJ_SUCCESS;
            }
            int body_len = 0;
			LOG4CPLUS_ERROR(LOG_TURN, "DEBUG---------function_code=" << (int)header.function_code);
            switch(header.function_code)
            {
                case HEARTBEAT:
                {
					int sessionid = TS_INVALID;
					int homeid = TS_INVALID;
					if(header.comm_type == REQUEST)
					{ 
						heartbeat_req_t heart;
						memset(&heart, 0, sizeof(heartbeat_req_t));
						int res = parse_heartbeat_req(recv_buff, &heart);
						if(!res)
						{
							sessionid = heart.session_id;
							homeid = g_udb_mgr.mapping.get_home_id(sessionid);
						}
					}
					else if(header.comm_type == RESPONSE)
					{
						heartbeat_res_t heart;
						memset(&heart, 0, sizeof(heartbeat_res_t));
						int res = parse_heartbeat_res(recv_buff, &heart);
						if(!res)
						{
							sessionid = heart.session_id;
							homeid = g_udb_mgr.mapping.get_home_id(sessionid);
						}
					}
					if(homeid != TS_INVALID)
					{
						ts_user *pUserFinded = NULL;
						std::vector<ts_user> user_list;
						int result = TS_INVALID;
						if(header.comm_source == HOST_S)
						{
							result = g_udb_mgr.home.get_hosts(homeid, user_list);
							if(result == 0 && user_list.size() > 0)
							{
								size_t i = 0;
								for(; i < user_list.size(); i++)
								{
									if(user_list[i].session_id == sessionid)
									{
										pUserFinded = &(user_list[i]);
										break;
									}
								}
							}

							if(NULL == pUserFinded)
		                    {
		                        fill_error_res((u_char *)snd_buff, snd_len, header.function_code, 0, GE_NO_USER);
		                        break;
		                    }
						}
						else if(header.comm_source == CLIENT)
						{
							result = g_udb_mgr.home.get_mobiles(homeid, user_list);
							if(result == 0 && user_list.size() > 0)
							{
								size_t i = 0;
								for(; i < user_list.size(); i++)
								{
									if(user_list[i].session_id == sessionid)
									{
										pUserFinded = &(user_list[i]);
										break;
									}
								}
							}

							if(NULL == pUserFinded)
		                    {
		                        fill_error_res((u_char *)snd_buff, snd_len, header.function_code, 0, GE_NO_USER);
		                        break;
		                    }
						}

						refreshUserAliveTime(pUserFinded, header.comm_source);

						heartbeat_res_t heartRes;
						memset(&heartRes, 0, sizeof(heartbeat_res_t));
						body_len = sizeof(heartbeat_res_t) - sizeof(remote_header_t);
						init_common_header(&heartRes.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len, header.function_code, RESPONSE, SERVER);
						heartRes.session_id = sessionid;
						heartRes.message = INERRANT;//ok status

						fill_heartbeat_res((u_char *)snd_buff, &heartRes);
						*snd_len = sizeof(heartbeat_res_t);
						
	                    LOG4CPLUS_DEBUG(LOG_TURN, "HEARTBEAT : userName = " << username
							<< ",homeId = " << homeid
							<< ",sessionId = " << sessionid
							<< ",liveTime = " << pUserFinded->liveTime);
					}else{
						fill_error_res((u_char *)snd_buff, snd_len, header.function_code, sessionid, GE_NO_LOGIN);
                        LOG4CPLUS_DEBUG(LOG_TURN, "HEARTBEAT GE_NO_LOGIN userName = " << username <<" sessionId = " << sessionid);
					}
                    break;
                }
                case ASK_SECRET_KEY:
                {
                    LOG4CPLUS_DEBUG(LOG_TURN, "ASK_SECRET_KEY ");
                    ask_secret_req_t ask_req;
                    memset(&ask_req, 0, sizeof(ask_secret_req_t));
                    if(!parse_ask_secretkey_req(recv_buff, &ask_req)
                       && ask_req.header.comm_type == REQUEST)
                    {
                        ask_secret_res_t ask_res;
                        memset(&ask_res, 0, sizeof(ask_secret_res_t));
                        body_len = sizeof(ask_secret_res_t) - sizeof(remote_header_t);
                        init_common_header(&ask_res.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len , header.function_code,
                                           RESPONSE, SERVER);
                        RSA *rsa = this_tsp_handler->rsa;
                        char * rs = rt_get_rsa_publicKey(rsa);
                        int offsize = sizeof(ask_res.secret_key) - strlen(rs);
                        memset(ask_res.secret_key, '0', sizeof(ask_res.secret_key));
                        memcpy(ask_res.secret_key + offsize, rt_get_rsa_publicKey(rsa),
                               sizeof(ask_res.secret_key));
                        memcpy(ask_res.secret_mode, rt_get_rsa_bigNum(rsa),
                               strlen(rt_get_rsa_bigNum(rsa)));
                        fill_ask_secretkey_res((u_char *)(snd_buff), &ask_res);
                        *snd_len = sizeof(ask_secret_res_t);
                    }
                    break;
                }
                case PASS_SECRET_KEY:
                {
                    LOG4CPLUS_DEBUG(LOG_TURN, "PASS_SECRET_KEY ");
                    pass_secret_req_t pass_req;
                    memset(&pass_req, 0, sizeof(pass_secret_req_t));
                    if(!parse_pass_secretkey_req((u_char *)rcv_buff, &pass_req)
                       && pass_req.header.comm_type == REQUEST)
                    {
                        ts_user __NewUser;
                        ts_user *user = find_user(username, un_len, __NewUser);

                        if(NULL == user)
                        {
                            fill_error_res((u_char *)snd_buff, snd_len, header.function_code, 0, GE_NO_USER);
                            break;
                        }
                        if(NULL == this_tsp_handler->rsa)
                        {
                            LOG4CPLUS_ERROR(LOG_TURN, "the RSA do not initialize ");
							break;
                        }
                        char temp_key[RSA_TEXT_LEN];
                        rt_decrypt((char *)pass_req.secret_key,temp_key , this_tsp_handler->rsa);
                        memcpy(user->aeskey, temp_key, sizeof(user->aeskey));
                        pass_secret_res_t pass_res;
                        memset(&pass_res, 0, sizeof(pass_secret_res_t));
                        body_len = sizeof(pass_secret_res_t) - sizeof(remote_header_t);
                        init_common_header(&pass_res.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len, header.function_code,
                                           RESPONSE, SERVER);
                        unsigned int session_id = 0;//user->sessionId;//a2i((const char *)user->sessionId, sizeof(user->sessionId));
                        session_id = g_udb_mgr.user.gen_usr_session_id(); // generate a session id, but don`t save
                        user->session_id = session_id;
                        pass_res.session_id = session_id;
						g_udb_mgr.tempUserMap[username] = __NewUser;

                        fill_pass_secretkey_res((u_char *)snd_buff, &pass_res);

                        *snd_len = sizeof(pass_secret_res_t);
                    }
                    else
                    {
                        fill_error_res((u_char *)snd_buff, snd_len, header.function_code, 0, GE_PARSE_ERROR);
                    }
                    break;
                }
                case LOGIN:
                {
                    LOG4CPLUS_DEBUG(LOG_TURN, "LOGIN ");
                    login_req_t login_req;
                    memset(&login_req, 0, sizeof(login_req_t));
                    if(!parse_login_req((u_char *)rcv_buff, &login_req)
                       && login_req.header.comm_type == REQUEST)
                    {
                        // fill a login_res_t object
                        login_res_t login_res;
                        memset(&login_res, 0, sizeof(login_res_t));
                        body_len = sizeof(login_res_t) - sizeof(remote_header_t);
                        init_common_header(&login_res.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len, header.function_code,
                                           RESPONSE, SERVER);
                        memset(login_res.space, 0, sizeof(login_res.space)/sizeof(u_char));
                        *snd_len = sizeof(login_res_t);
                        ts_user __NewUser;
                        ts_user *selected_user = find_user(username, un_len, __NewUser);

                        ts_user login_user;
                        //if(login_user->aeskey == null) return : no aeskey error code
                        aes_encode aes_ins2 ;
                        memset(&aes_ins2, 0, sizeof(aes_encode));

                        if(NULL == selected_user)
                        {
                            fill_error_res((u_char *)snd_buff, snd_len, header.function_code, 0, GE_NO_USER);
                            break;
                        }
                        //create AES
                        at_create_key_by_string((const u_char *)selected_user->aeskey, &aes_ins2);
                        at_decrypt((char *)login_req.user_name, (char *)login_user.username,
                                   login_req.username_len, aes_ins2);
                        at_decrypt((char *)login_req.password, (char *)login_user.pwd,
                                   (int16_t)login_req.password_len, aes_ins2);
                        LOG4CPLUS_DEBUG(LOG_TURN, "username:" << login_user.username<<" Pwd:" << login_user.pwd);

                        int result = TS_FAILED;
                        result = g_udb_mgr.user.verify_usr(login_user.username, strlen(login_user.username),
                                                           login_user.pwd, strlen(login_user.pwd));
#if 0
if(strcmp(login_user.username, "2ndbao") == 0){
	//让指定的用户配对成功，登录失败，
	//用来测试某个终端在配对成功后但没有登录服务器成功时，
	//会导致该终端一直占着某个sdp，从而出乎意料的出现"连接上限"
	result = TS_FAILED;
	LOG4CPLUS_DEBUG(LOG_TURN, "let username:" << login_user.username<<" login failed!");
	sleep(5);
}
#endif
                        int session_id = selected_user->session_id;
                        int homeid = selected_user->home_id;
                        if(result == 0 && (session_id == login_req.session_id))
                        { // login success

							selected_user->loginTime = time(NULL);
							selected_user->liveTime = selected_user->loginTime;
                            // sort the user
                            if(login_req.header.comm_source == HOST_S)
                            {
                                g_udb_mgr.home.save_host(homeid, selected_user);
                                //上报给电信服务器
                                std::string deviceId;
                                int res = g_udb_mgr.jstelcom.get_deviceId_by_homeId(homeid, deviceId);
                                if(res != TS_SUCCESS)
                                {
                                   LOG4CPLUS_ERROR(LOG_TURN, "get_deviceId_by_homeId-->" << homeid);
                                   break;
                                }
#if defined( SMARTHOME_AGENT_JSTELECOM) && defined( USE_HTTPKEEPCONNECTORSMARTHOME)
                                Socket *p_01 = g_SmartHomeAgentTelcom.GetSocketByName(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME);
                                CHttpKeepConnectorSmartHomeTelcom *pHttpKeepConnectorSmartHomeTelcom = dynamic_cast<CHttpKeepConnectorSmartHomeTelcom *>(p_01);
                                
                                if (pHttpKeepConnectorSmartHomeTelcom)
                                {
                					// OnOFF 1 开 0 关
                                    pHttpKeepConnectorSmartHomeTelcom->reportCtrlDeviceState(deviceId, 1);
                                }
#endif
                                LOG4CPLUS_DEBUG(LOG_TURN, "Host user login success: userName = "<< selected_user->username 
									<< " homeId = " << selected_user->home_id 
									<< " sessionId = " << selected_user->session_id
									<< " liveTime = " << selected_user->liveTime);
                            }
                            else if(login_req.header.comm_source == CLIENT)
                            {
                                g_udb_mgr.home.save_mobile(homeid, selected_user);
                                LOG4CPLUS_DEBUG(LOG_TURN, "Mobile user login success: userName = "<< selected_user->username 
									<< " homeId = " << selected_user->home_id 
									<< " sessionId = " << selected_user->session_id
									<< " liveTime = " << selected_user->liveTime);

                                //tell the peer's address
                                std::vector<ts_user> host_list;
                                int result = g_udb_mgr.home.pair_user(homeid, 1, host_list);
                                if (result == 0 && host_list.size() > 0)
                                {
                                    //at present, one home just have one host
                                    ts_user host_user = host_list[0];
									user_sdp host_user_sdp = host_user.sdpVec[0];
                                    struct in_addr inp;
                                    inet_aton(host_user_sdp.addr, &inp);
                                    login_res.peer_ip = inp.s_addr;
                                    login_res.peer_port = host_user_sdp.port;
                                }
								
                            }
                            g_udb_mgr.mapping.put_session_home_id(session_id, homeid);
                        }
                        else
                        {
                            result = 0x01;
                        }
                        login_res.session_id = session_id;
                        login_res.status = result;
                        //end fill login_res_t
                        fill_login_res((u_char *)snd_buff, &login_res);
                    }
                    break;
                }
                case UPDATE_QUERY:
                {
                    LOG4CPLUS_DEBUG(LOG_TURN, "UPDATE_QUERY");
                    update_req_t check_req;
                    memset(&check_req, 0, sizeof(update_req_t));
                    if(!parse_updateconfig_req((u_char *)rcv_buff, &check_req)
                       && check_req.header.comm_type == REQUEST)
                    {
                        int session_id = check_req.session_id;
                        if(has_login(session_id) != TS_INVALID)
                        {//login successfully
                        	ts_user __NewUser;

                            ts_user *logined_user = find_user(userName, un_len, __NewUser);

							refreshUserAliveTime(logined_user, check_req.header.comm_source);
							
                            IPC_DG_CHECK_UPDATE_REQ cfg_req;
                            memset(&cfg_req, 0, sizeof(IPC_DG_CHECK_UPDATE_REQ));
                            cfg_req.header.start = 0xFF;
                            cfg_req.header.protocol = 0x01;
                            short len = sizeof(IPC_DG_CHECK_UPDATE_REQ) - sizeof(IPC_HEADER);
                            cfg_req.header.length = len;
                            cfg_req.header.searilNum = 0;
                            cfg_req.header.function = CHECK_UPDATE;
                            cfg_req.header.type = IPC_REQUEST;
                            memset(cfg_req.header.space, 0, sizeof(cfg_req.header.space));
                            int homeId = g_udb_mgr.mapping.get_home_id(session_id);
                            cfg_req.homeId = homeId;
                            cfg_req.sessionId = session_id;
                            cfg_req.last_time = check_req.latest_time;
                            memcpy(cfg_req.client_aes, logined_user->aeskey, sizeof(cfg_req.client_aes));
                            cfg_req.query_from = check_req.query_from;
                            cfg_req.pid_len = strlen((char *)check_req.product_id);
                            memcpy(cfg_req.product_id, check_req.product_id, sizeof(cfg_req.product_id));
                            LOG4CPLUS_DEBUG(LOG_TURN, "product_id("<< cfg_req.product_id <<")len(" << (int)cfg_req.pid_len << ")");
                            LOG4CPLUS_DEBUG(LOG_TURN, "rcv_len("<< rcv_len <<") sizeof(check_req)(" << sizeof(check_req) << ")");

                            ipc_handler *handler = g_GeneralSocketProcessor.GetIPC_hander();
                            handler->outgoing_handler(handler->aes_ins, send_buff, &cfg_req, CHECK_UPDATE);
                            send = sizeof(IPC_HEADER)+ntohs(cfg_req.header.length);
							std::vector<IPCPEER_T> dstPeer;
							int webStatus = g_GeneralSocketProcessor.GetWebPeers(dstPeer);
							if(webStatus > 0)
							{
	                            int status = g_GeneralSocketProcessor.Send2Webs(0, (const char *)send_buff, send);
	                            if (status<0) //send error // 0 - success, 1 failed, 2012-8-18 11:08:35
	                            {
	                                LOG4CPLUS_ERROR(LOG_TURN, "send_to_web error("<< status << ") send:"<< send);
									//回复客户端下载服务器连接错误
									fill_error_res((u_char *)snd_buff, snd_len, header.function_code, session_id, GE_WEB_CONNECT_ERROR);
	                            }
							}
							else
							{
									//回复客户端下载服务器连接错误
									fill_error_res((u_char *)snd_buff, snd_len, header.function_code, session_id, GE_WEB_CONNECT_ERROR);
							}

                            memset(send_buff, 0, sizeof(send_buff));
                        }
                        else
                        {
                            fill_error_res((u_char *)snd_buff, snd_len, header.function_code, session_id, GE_NO_LOGIN);
                            LOG4CPLUS_DEBUG(LOG_TURN, "UPDATE_QUERY GE_NO_LOGIN");
                        }
                    }
                    break;
                }
                case P2P:
                {
                    LOG4CPLUS_DEBUG(LOG_TURN, "P2P");
                    //by pjnath.
                    break;
                }
                case DEVICE_OPERATION:
                {
                    LOG4CPLUS_DEBUG(LOG_TURN, "DEVICE_OPERATION");
					ts_user __NewUser;
                    ts_user *logined_user = (ts_user *)find_user(username, un_len, __NewUser);
					
					refreshUserAliveTime(logined_user, header.comm_source);
                    handle_remote_response(srv, rcv_buff, rcv_len);
                    //do nothing on server
                    break;
                }
                case STOP_CONNECTION:
                {
                    LOG4CPLUS_DEBUG(LOG_TURN, "STOP_CONNECTION");

					if(header.comm_type == REQUEST)
					{
	                    stop_host_connect_req_t deny_req;
	                    memset(&deny_req, 0, sizeof(host_deny_connect_req_t));
	                    if(!parse_stophostnet_req((u_char *)rcv_buff, &deny_req)
	                       && deny_req.header.comm_type == REQUEST)
	                    {
	                        int sessionId = deny_req.session_id;
	                        if(has_login(sessionId) != TS_INVALID)
	                        {
	                        	int hasPermission = g_udb_mgr.user.check_usr_sys_permission(username, SYSTEM_PERMISSION_STOP_HOST_NET);
								LOG4CPLUS_DEBUG(LOG_TURN, "hasPermission=" << hasPermission);

								if(hasPermission != TS_FALSE){
		                        	ts_user __NewUser;
		                            ts_user *logined_user = (ts_user *)find_user(username, un_len, __NewUser);
									
									refreshUserAliveTime(logined_user, deny_req.header.comm_source);
									
		                            aes_encode aes_ins3;
		                            memset(&aes_ins3, 0, sizeof(aes_encode));
		                            at_create_key_by_string((const u_char *)logined_user->aeskey, &aes_ins3);
		                            char check_str[6];
		                            at_decrypt((char *)deny_req.check_code, check_str,
		                                       6, aes_ins3);
		                            if(!strcmp("tiansu", (const char *)check_str))
		                            {
		                                //find host user
		                                ts_user *host_user = NULL;
		                                int home_id = g_udb_mgr.mapping.get_home_id(sessionId);
		                                std::vector<ts_user> host_list;
		                                int result = g_udb_mgr.home.get_hosts(home_id, host_list);
		                                if(result == 0 && host_list.size() > 0)
		                                {
		                                    //at present, one home just have one host
		                                    host_user = &(host_list[0]);
		                                }
		                                if(NULL != host_user)
		                                {
		                                    logined_user->stop_host_sender = STOP_HOST_SENDER_TRUE;
											refreshUserAliveTime(logined_user, deny_req.header.comm_source);
											
		                                    host_deny_connect_req_t to_host;
		                                    memset(&to_host, 0, sizeof(host_deny_connect_req_t));
				                            body_len = sizeof(host_deny_connect_req_t) - sizeof(remote_header_t);
				                            init_common_header(&to_host.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len, header.function_code,
	                                               				REQUEST, SERVER);
		                                    to_host.session_id = host_user->session_id;
											to_host.report_id = 0;
											u_char buff[512];
											fill_stophostnet_req_to_host(buff, &to_host);
		                                    user_sdp sdp2 = host_user->sdpVec[0];
											send = sizeof(host_deny_connect_req_t);
		                                    send_data_to_client (srv,
		                                                         (const unsigned char*)buff,
		                                                         send,
			                                         			 //PJ_FALSE,
		                                                         sdp2.addr,
		                                                         sdp2.port);
		                                    *snd_len = 0;// response nothing,immediately
		                                }
		                                else
		                                {
		                                    fill_error_res((u_char *)snd_buff, snd_len, header.function_code, sessionId, GE_NO_HOST_CONNECT);
		                                }
	                            	}
	                            }
								else
								{
									stop_host_connect_res_t stop_res;
									memset(&stop_res, 0, sizeof(stop_host_connect_res_t));
									body_len = sizeof(stop_host_connect_res_t) - sizeof(remote_header_t);
				                    init_common_header(&stop_res.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len, header.function_code,
				                                       RESPONSE, SERVER);
									stop_res.res_code = NO_PERMISSION;
									stop_res.session_id = sessionId;
									u_char buff[500] = {0};
									fill_stophostnet_res(buff, &stop_res);
									send = sizeof(stop_host_connect_res_t);

	                                send_data_to_client (srv,
	                                                     (const unsigned char*)buff,
	                                                     send,
		                                         		 //PJ_FALSE,
	                                                     addr,
	                                                     port);
								}
	                        }
	                        else
	                        {
	                            fill_error_res((u_char *)snd_buff, snd_len, header.function_code, sessionId, GE_NO_LOGIN);
	                        }
	                    }
					}
                    break;
                }
                case LOGOUT:
                {
                    LOG4CPLUS_DEBUG(LOG_TURN, "LOGOUT");
                    logout_req_t logout_req;
                    memset(&logout_req, 0, sizeof(logout_req_t));
                    if(!parse_logout_req((u_char *)rcv_buff, &logout_req)
                       && logout_req.header.comm_type == REQUEST)
                    {
                        int sessionId = logout_req.session_id;
                        if(has_login(sessionId) != TS_INVALID)
                        {
                            LOG4CPLUS_DEBUG(LOG_TURN, "User logout...");
                            do_when_logout(username, un_len, sdp);

                            logout_res_t logout_res;
                            memset(&logout_res, 0, sizeof(logout_res_t));
                            body_len = sizeof(logout_res_t) - sizeof(remote_header_t);
                            init_common_header(&logout_res.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len, header.function_code,
                                               RESPONSE, SERVER);
                            logout_res.session_id = logout_req.session_id;
                            logout_res.res_code = 0x00;
                            memset(logout_res.space, 0, sizeof(logout_res.space));
                            fill_logout_res((u_char *)snd_buff, &logout_res);
                            *snd_len = sizeof(logout_res_t);
                        }
                        else
                        {
                            fill_error_res((u_char *)snd_buff, snd_len, header.function_code, sessionId, GE_NO_LOGIN);
                        }
                    }
                    break;
                }
				case COMMON_DEVICE_CONTROL:
                {
                    LOG4CPLUS_DEBUG(LOG_TURN, "COMMON_DEVICE_CONTROL");
                    device_control_res ctrl_res;
                    memset(&ctrl_res, 0, sizeof(device_control_res));
                    char data[256] = {0};
                    //int i;
                   // printf("recv_buff: len=%d\n", rcv_len);
                   // for(i = 0; i < rcv_len; i++){
                   //     printf("%d,", recv_buff[i]);
                    //}
                   // printf("\n");
                    if(!parse_device_ctrl_res((u_char *)rcv_buff, &ctrl_res, data)
                       && ctrl_res.header.comm_type == RESPONSE && ctrl_res.read_write == 0x02)
                    {
                        //printf("data: len=%d\n", (int)ctrl_res.data_len);
                        //for(i = 0; i < (int)ctrl_res.data_len; i++){
                        //    printf("%d,", data[i]);
                        //}
                       // printf("\n");
                        int sessionId = ctrl_res.session_id;
						LOG4CPLUS_DEBUG(LOG_TURN, "sessionId=" << sessionId);
						LOG4CPLUS_DEBUG(LOG_TURN, "sizeof="<< sizeof(device_control_res)<<",datapoint_id="<< ctrl_res.datapoint_id <<",data_len=" << (int)ctrl_res.data_len << ", data="<<(char *)data);
						int homeId = g_udb_mgr.mapping.get_home_id(sessionId);
                        if(homeId != TS_INVALID)
                        {
                            //上报给电信服务器
							std::string deviceId;
							int res = g_udb_mgr.jstelcom.get_deviceId_by_homeId(homeId, deviceId);
							if(res != TS_SUCCESS)
							{
								LOG4CPLUS_ERROR(LOG_TURN, "get_deviceId_by_homeId-->" << homeId);
								break;
							}

							device_datapoint info;
							res = g_udb_mgr.jstelcom.get_device_by_datapoint(ctrl_res.datapoint_id, info);
							if(res != TS_SUCCESS)
							{
								LOG4CPLUS_ERROR(LOG_TURN, "get_device_by_datapoint-->" << ctrl_res.datapoint_id);
								break;
							}

#if defined( SMARTHOME_AGENT_JSTELECOM) && defined( USE_HTTPKEEPCONNECTORSMARTHOME)
					        Socket *p_01 = g_SmartHomeAgentTelcom.GetSocketByName(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME);
					        CHttpKeepConnectorSmartHomeTelcom *pHttpKeepConnectorSmartHomeTelcom = dynamic_cast<CHttpKeepConnectorSmartHomeTelcom *>(p_01);

							sprintf(data, "%d", (int)data[0]);
							cout << endl << data << endl;
					        if (pHttpKeepConnectorSmartHomeTelcom)
					        {
					        	if(info.deviceType != "4")
					        	{
    					            pHttpKeepConnectorSmartHomeTelcom->reportDeviceStateList(deviceId, 
    																				info.datapointId, 
    																				data, 
    																				"0-1", 
    																				"0", 
    																				"reportDeviceState");
					        	}
								else
								{
                                    pHttpKeepConnectorSmartHomeTelcom->reportAlarm(deviceId, 
    																				info.datapointId, 
    																				data, 
    																				"0-1", 
    																				"0", 
    																				"reportAlarm");
								}
					        }
#endif
                        }
                        else
                        {
                            fill_error_res((u_char *)snd_buff, snd_len, header.function_code, sessionId, GE_NO_LOGIN);
                        }
                    }
                    break;
                }
                default:
                break;

            }

            //}
        }
    }
    else
    {
        LOG4CPLUS_ERROR(LOG_TURN, "rcv_len("<<rcv_len <<") should > 0");
    }

    gettimeofday(&t_b, NULL);
    tTime = (t_b.tv_sec - t_a.tv_sec) * 1000 + (t_b.tv_usec - t_a.tv_usec)/1000.0;
    LOG4CPLUS_DEBUG(LOG_TURN, "HANDLE END takes:"<< tTime << " ms");
    return PJ_SUCCESS;
}

static int handle_remote_response(pj_turn_srv *srv, const unsigned char *rcv_buff, const unsigned int rcv_len)
{
    LOG4CPLUS_DEBUG(LOG_TURN, "handle_remote_response");
	#if 0
    int i = 0;
    for(;i<rcv_len;i++){
    printf("%d,",rcv_buff[i]);
    }
    printf(", rcv_len=%d\n", rcv_len);
	#endif
    DBG_CODE(
        __fline;printf("recv_buff(%d)\n", rcv_len);
        dumpBuffer(stdout, (unsigned char *)rcv_buff,
                   rcv_len,
                   SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
    );
    CEZGuard guard(g_mutexCommunicationresponse);
    if(NULL != rcv_buff && rcv_len >= sizeof(remote_header_t))
    {
        remote_header_t header;
        size_t send;
        memset(&header, 0, sizeof(remote_header_t));
        u_char *recv_buff = (u_char *)(rcv_buff);
        if(pass_common_header(recv_buff, &header))
        {
            // parse a header, if receive buffer is null,return now.
            return PJ_SUCCESS;
        }
        if(header.protocol_type != 0)
        {
            //if the datagram is not a remote protocol type, do nothing, return now.
            return PJ_SUCCESS;
        }
        switch(header.function_code)
        {
            case STOP_CONNECTION:
            {
                LOG4CPLUS_DEBUG(LOG_TURN, "STOP_CONNECTION");
                host_deny_connect_res_t deny_res;
                memset(&deny_res, 0, sizeof(host_deny_connect_res_t));
                if(!parse_stophostnet_res_from_host((u_char *)rcv_buff, &deny_res)
                   && deny_res.header.comm_type == RESPONSE)
                {
                    int sessionId = deny_res.session_id;
                    int home_id = g_udb_mgr.mapping.get_home_id(sessionId);
                    std::vector<ts_user> mobile_list;
                    int result = g_udb_mgr.home.get_mobiles(home_id, mobile_list);
					u_char buff[512];

					stop_host_connect_res_t stop_res;
					memset(&stop_res, 0, sizeof(stop_host_connect_res_t));
					int body_len = sizeof(stop_host_connect_res_t) - sizeof(remote_header_t);
                    init_common_header(&stop_res.header, INTERNET, TSHOME_PROTOCOL_VERSION, body_len, header.function_code,
                                       RESPONSE, SERVER);
					stop_res.res_code = 'x';
					stop_res.res_code = deny_res.res_code;
					LOG4CPLUS_DEBUG(LOG_TURN, "stop host net res_code(0S,1F)=" << (int)stop_res.res_code);
                    if(result == 0 && mobile_list.size() > 0)
                    {
                        if(deny_res.res_code == DENY_SUCCESS)
                        {
                            //if success, notify all mobiles.
                            size_t i = 0;
                            for(; i < mobile_list.size(); i++)
                            {
                            	ts_user user = mobile_list[i];
                                stop_res.session_id = user.session_id;

								fill_stophostnet_res(buff, &stop_res);
								send = sizeof(stop_host_connect_res_t);
                                user_sdp sdp = user.sdpVec[0];

                                pj_status_t status = send_data_to_client (srv,
					                                                     (const unsigned char*)buff,
					                                                     send,
						                                         		 //PJ_FALSE,
					                                                     sdp.addr,
					                                                     sdp.port);
								if(status == PJ_SUCCESS)
								{
									LOG4CPLUS_DEBUG(LOG_TURN, "send success to client " << sdp.addr << ":" << sdp.port);
								}
								else
									LOG4CPLUS_DEBUG(LOG_TURN, "send failed to client " << sdp.addr << ":" << sdp.port);
								memset(buff, 0, sizeof(buff));

								if (user.stop_host_sender == STOP_HOST_SENDER_TRUE)
                                {
									//还原该用户的状态
									user.stop_host_sender = STOP_HOST_SENDER_FALSE;
									g_udb_mgr.home.save_mobile(user.home_id, &user);
								}
                            }
                        }
                        else
                        {
                            //some errs take about
                            size_t i = 0;
                            for(; i < mobile_list.size(); i++)
                            {
                            	ts_user user = mobile_list[i];
                                if (user.stop_host_sender == STOP_HOST_SENDER_TRUE)
                                {
									stop_res.session_id = user.session_id;

									fill_stophostnet_res(buff, &stop_res);
									send = sizeof(stop_host_connect_res_t);
                                    user_sdp sdp = user.sdpVec[0];

                                    pj_status_t status = send_data_to_client (srv,
					                                                         (const unsigned char*)&buff,
					                                                         send,
						                                         			 //PJ_FALSE,
					                                                         sdp.addr,
					                                                         sdp.port);
									if(status == PJ_SUCCESS)
									{
										LOG4CPLUS_DEBUG(LOG_TURN, "send success to client " << sdp.addr << ":" << sdp.port);
									}
									else
										LOG4CPLUS_DEBUG(LOG_TURN, "send failed to client " << sdp.addr << ":" << sdp.port);
									memset(buff, 0, sizeof(buff));

									//还原该用户的状态
									user.stop_host_sender = STOP_HOST_SENDER_FALSE;
									g_udb_mgr.home.save_mobile(user.home_id, &user);
                                    break;
                                }
                            }

                        }

                    }

					if(deny_res.report_id > 0)
					{
	                    //send to web
	                    IPC_DG_HOST_SERV to_web_res;
	                    memset(&to_web_res, 0, sizeof(IPC_DG_HOST_SERV));
	                    to_web_res.header.protocol = 0x01;
	                    to_web_res.header.start = 0xFF;
	                    int len_ = sizeof(IPC_DG_HOST_SERV) - sizeof(IPC_HEADER);
	                    to_web_res.header.length = len_;
	                    to_web_res.header.type = IPC_RESPONSE;
	                    to_web_res.header.searilNum = deny_res.report_id;
	                    to_web_res.header.function = HOST_SERV_REQ;
	                    memset(to_web_res.header.space, 0, sizeof(to_web_res.header.space));
	                    to_web_res.homeId = (home_id);
	                    memset(to_web_res.userName, 0, sizeof(to_web_res.userName));
	                    to_web_res.cmd_state = deny_res.res_code+2;

	                    ipc_handler *handler = g_GeneralSocketProcessor.GetIPC_hander();
	                    handler->outgoing_handler(handler->aes_ins, buff, &to_web_res, HOST_SERV_REQ);
	                    send = sizeof(IPC_HEADER)+ntohs(to_web_res.header.length);
	                    int status = g_GeneralSocketProcessor.Send2Webs(0, (const char *)buff, send);
						
	                    memset(buff, 0, sizeof(buff));

	                    if (status<0) //send error // 0 - success, 1 failed, 2012-8-18 11:08:35
	                    {
	                        LOG4CPLUS_ERROR(LOG_TURN, "send_to_web error("<< status << ") send:"<< send);
	                    }
					}
                }
                break;
            }
            case DEVICE_OPERATION:
            {
                LOG4CPLUS_DEBUG(LOG_TURN, "DEVICE_OPERATION");
                ir_study_res_t wifi_res;
                memset(&wifi_res, 0, sizeof(ir_study_res_t));
                if(!parse_wifictrl_res((u_char *)rcv_buff, &wifi_res)
                   && wifi_res.kic_head.header.comm_type == RESPONSE
                   && wifi_res.kic_head.device_type == IR_STUDY)
                {
                    int sessionId = wifi_res.kic_head.session_id;
                    int reportId = wifi_res.kic_head.home_device_id;
                    int homeid = g_udb_mgr.mapping.get_home_id(sessionId);
                    IPC_DG_INF_LEARN inf_res;
                    inf_res.header.start = 0xFF;
                    inf_res.header.protocol = 0x01;
                    int len_ = sizeof(IPC_DG_INF_LEARN) - sizeof(IPC_HEADER);
                    //len_ = htons(len_);
                    inf_res.header.length = len_;
                    inf_res.header.searilNum = reportId;
                    inf_res.header.function = INFRARED_LEARN;
                    inf_res.header.type = IPC_RESPONSE;
                    inf_res.homeId = homeid;
                    inf_res.gateway_id = wifi_res.gateway_id;
                    inf_res.channel = wifi_res.channel;
                    inf_res.key = wifi_res.function_code;
                    inf_res.result = wifi_res.res_code;
                    u_char buff[512];

                    ipc_handler *handler = g_GeneralSocketProcessor.GetIPC_hander();
                    handler->outgoing_handler(handler->aes_ins, buff, &inf_res, INFRARED_LEARN);
                    send = sizeof(IPC_HEADER)+ntohs(inf_res.header.length);
                    int status = g_GeneralSocketProcessor.Send2Webs(0, (const char *)buff, send);

                    if (status<0) //send error // 0 - success, 1 failed, 2012-8-18 11:08:35
                    {
                        LOG4CPLUS_ERROR(LOG_TURN, "send_to_web error("<< status << ") send:"<< send);
                    }

                }
                break;
            }
            default :
            break;
        }
    }
    else
    {
        LOG4CPLUS_ERROR(LOG_TURN, "rcv_len:"<<rcv_len);
    }
    LOG4CPLUS_DEBUG(LOG_TURN, "END handle_remote_response");
    return PJ_SUCCESS;
}

static int handle_remote_timeout(pj_turn_srv *srv,
                                 const char *username,
                                 const unsigned int un_len,
                                 const char *ip,
                                 const int ip_len,
                                 const int clt_port,
                                 user_sdp *sdp)
{
    LOG4CPLUS_DEBUG(LOG_TURN, "User connection timeout");
    return handle_remote_destroy(srv, username, un_len, ip, ip_len, clt_port, sdp);
}

static int handle_remote_destroy(pj_turn_srv *srv,
                                 const char *username,
                                 const unsigned int un_len,
                                 const char *ip,
                                 const int ip_len,
                                 const int clt_port,
                                 user_sdp *sdp)
{
    CEZGuard guard(g_mutexTspHandlerDestroy);

    LOG4CPLUS_DEBUG(LOG_TURN, "User's SDP destroy");
	ts_user user;
	ts_user *res = find_user(username, un_len, user);
    if (NULL != res)
    {
        user.reference--;
        LOG4CPLUS_DEBUG(LOG_TURN, "SDP destroy:"<<username);
        int ref = user.reference;
        LOG4CPLUS_DEBUG(LOG_TURN, "alive sdp count:"<<ref);
        if(user.reference <= 0)
        {
            do_when_logout(username, un_len, sdp);
            LOG4CPLUS_DEBUG(LOG_TURN, "do_when_logout:"<<username);
            return PJ_SUCCESS;
        }
        else if(has_login(user.session_id) != TS_INVALID)//user.reference > 0 && has_login(user.session_id) != TS_INVALID
        {
            if(user.sdpVec.size() > 0)
            {
                std::vector<user_sdp>::iterator p;

                for(p = user.sdpVec.begin(); p != user.sdpVec.end(); p++)
                {
                    if (strncasecmp(p->addr, ip, ip_len) == 0 && p->port == clt_port)
                    {
                        user.sdpVec.erase(p);
						user_sdp newSdp;
						user.sdpVec.push_back(newSdp);
                        g_udb_mgr.home.save_host(user.home_id, &user);
                        break;
                    }
                }
            }
        }
    }
    else
    {
        LOG4CPLUS_DEBUG(LOG_TURN, "not found user:"<<username);
    }
    return PJ_SUCCESS;
}


