/*
 * web_mgr.c
 *
 *  Created on: Apr 9, 2012
 *      Author: apple
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/un.h>

#include "web_mgr.h"
#include "operation_server_mgr.h"
#include "db/ts_db_conn_infos.h"
#include "common/thread.h"

extern int g_is_net_connect_stoped;

extern THREAD_MUTEX mux_lock_network_stop_flag;

int listen_fd = -1;
int com_fd = -1;
char recv_buf[WEB_BUFF_LEN] = {0};
char send_buf[WEB_BUFF_LEN] = {0};
int backlog = 5 ;


// pj thread handle.
pj_thread_t *web_handle = NULL;

//
THREAD_MUTEX mux_lock_web_action = PTHREAD_MUTEX_INITIALIZER;


void start_web_action_thread(pj_pool_t *param)
{
    
    pj_status_t status = pj_thread_create(param,
                                          "web_action_thread",
                                          web_action_handle,
                                          NULL,
                                          PJ_WEB_THREAD_MEMORY_SIZE,
                                          0,
                                          &web_handle);
}

void stop_web_action_thread()
{
    printf(">>>> Into stop_web_action_thread ...\n");
    
	close(listen_fd);
	close(com_fd);
    unlink(UNIX_DOMAIN);
    
    printf(">>>> have stop_web_action_thread ...\n");
}

// the action of thread
int web_action_handle(void *param)
{
	int ret;
	int len;
	int rval;
	struct sockaddr_un clt_addr;
	struct sockaddr_un srv_addr;

	listen_fd=socket(AF_UNIX,SOCK_STREAM,0);
	if(listen_fd<0) {
		perror(">>>>>> In web client socket: cannot create communication socket");
        // destroy the thread
        pj_thread_join(web_handle);
        pj_thread_destroy(web_handle);
		return -1;
	}

	//set server addr_param
	srv_addr.sun_family=AF_UNIX;
	strcpy(srv_addr.sun_path,UNIX_DOMAIN);

	unlink(UNIX_DOMAIN);

	//bind listen_fd & srv_addr
	ret=bind(listen_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
	if(ret==-1) {
		perror(">>>>>> In web client socket: cannot bind server socket");
		close(listen_fd);
		unlink(UNIX_DOMAIN);
        // destroy the thread
        pj_thread_join(web_handle);
        pj_thread_destroy(web_handle);
		return -1;
	}

	//listen listen_fd
	ret=listen(listen_fd,backlog);
	if(ret==-1) {
		perror(">>>>>> In web client socket: cannot listen the client connect request");
		close(listen_fd);
		unlink(UNIX_DOMAIN);
        // destroy the thread
        pj_thread_join(web_handle);
        pj_thread_destroy(web_handle);
		return -1;
	}

	//have connect request use accept
	len = sizeof(struct sockaddr);


    do {
        
        com_fd = accept(listen_fd, (struct sockaddr *)&clt_addr, (socklen_t *)&len);
        
        printf(">>>>>>>>In web client socket: accept socket: %d\n",com_fd);
        
        if (com_fd == -1) {	
            perror(">>>>>> In web client socket: accept error");
            close(listen_fd);
            unlink(UNIX_DOMAIN);
            // destroy the thread
            pj_thread_join(web_handle);
            pj_thread_destroy(web_handle);
            return -1;
        }
        
        memset(recv_buf, 0, sizeof(recv_buf));
        rval = read(com_fd,recv_buf,sizeof(recv_buf));
        if (rval < 0) {
            perror(">>>>>> In web client socket: reading stream message error");
            close(listen_fd);
            close(com_fd);
            unlink(UNIX_DOMAIN);
            // destroy the thread
            pj_thread_join(web_handle);
            pj_thread_destroy(web_handle);
            return -1;

        } else if (rval > 0){
            printf(">>>>>>>>In web client socket: receive ending: %s\n",recv_buf);

            // handle recevie buff
            web_action_report_t *web_action = (web_action_report_t*) recv_buf;

            int start = web_action->start_flag;
            int source = web_action->source;
            int fun_code = web_action->fun_code;
            
            printf(">>>> start = %d\n",start);
            printf(">>>> source = %d\n",source);
            printf(">>>> fun_code = %d\n",fun_code);
            printf(">>>> data = %d\n",web_action->data);
            printf(">>>> obj = %d\n",web_action->obj);
            
            if (start == BUFF_START && source == HOST_WEB_PROGRAM) {
                switch (fun_code) {
                    case WEB_UPDATE: {
                        printf(">>>>>> In web client socket: UPDATE QUERY FROM WEB\n");
                        web_update();
                    }
                        break;
                    case STOP_HOST_NETWORK: {
                        printf(">>>>>> In web client socket: STOP HOST NETWORK\n");
                        int data = web_action->data; // 0:close; 1:open
                        int result = stop_host_network_from_web(data);

                        memset(send_buf, 0, sizeof(send_buf));
                        web_action_report_t *send_data =
                                (web_action_report_t *)send_buf;

                        send_data->data = result;
                        send_data->fun_code = STOP_HOST_NETWORK;
                        send_data->source = HOST_BACKGROUND_PROGRAM;
                        send_data->start_flag = BUFF_START;

                        int send_len = write(com_fd, send_buf,sizeof(send_buf));
                        if (send_len == WEB_BUFF_LEN) {  //send success
                            printf(">>>>>> In web client socket: send respose success: %s\n",send_buf);
                        } else if (send_len < 0){
                            perror(">>>>>> In web client socket: send on stream error\n");

                            close(com_fd);
                            close(listen_fd);
                            unlink(UNIX_DOMAIN);
                            // destroy the thread
                            pj_thread_join(web_handle);
                            pj_thread_destroy(web_handle);
                            printf (">>>>>> In web client socket: web thread over ... \n");
                            return -1;
                        } else {
                            printf (">>>>>> In web client socket: buff lost ... \n");
                        }
                    }
                        break;
                    case SET_ON_OFF_SERIAL: {
                        printf(">>>>>> In web client socket: SET_ON_OFF_SERIAL\n");
                        
                        int data = web_action->data;
                        int obj = web_action->obj;
                        int result = protocol_serial_open(obj, data);
                        
                        memset(send_buf, 0, sizeof(send_buf));
                        web_action_report_t *send_data =
                        (web_action_report_t *)send_buf;
                        
                        send_data->data = result;
                        send_data->fun_code = SET_ON_OFF_SERIAL;
                        send_data->source = HOST_BACKGROUND_PROGRAM;
                        send_data->start_flag = BUFF_START;
                        
                        int send_len = write(com_fd, send_buf,sizeof(send_buf));
                        if (send_len == WEB_BUFF_LEN) {  //send success
                            printf(">>>>>> In web client socket: send respose success: %s\n",send_buf);
                        } else if (send_len < 0){
                            perror(">>>>>> In web client socket: send on stream error\n");
                            
                            close(com_fd);
                            close(listen_fd);
                            unlink(UNIX_DOMAIN);
                            // destroy the thread
                            pj_thread_join(web_handle);
                            pj_thread_destroy(web_handle);
                            printf (">>>>>> In web client socket: web thread over ... \n");
                            return -1;
                        } else {
                            printf (">>>>>> In web client socket: buff lost ... \n");
                        }
                    }
                        break;
                    case SET_ON_OFF_GW: {
                        printf(">>>>>> In web client socket: SET_ON_OFF_GW\n");

                        int data = web_action->data;
                        int obj = web_action->obj;
                        int result = protocol_gateway_open(obj, data);
                        
                        memset(send_buf, 0, sizeof(send_buf));
                        web_action_report_t *send_data =
                        (web_action_report_t *)send_buf;
                        
                        send_data->data = result;
                        send_data->fun_code = SET_ON_OFF_GW;
                        send_data->source = HOST_BACKGROUND_PROGRAM;
                        send_data->start_flag = BUFF_START;
                        
                        int send_len = write(com_fd, send_buf,sizeof(send_buf));
                        if (send_len == WEB_BUFF_LEN) {  //send success
                            printf(">>>>>> In web client socket: send respose success: %s\n",send_buf);
                        } else if (send_len < 0){
                            perror(">>>>>> In web client socket: send on stream error\n");
                            
                            close(com_fd);
                            close(listen_fd);
                            unlink(UNIX_DOMAIN);
                            // destroy the thread
                            pj_thread_join(web_handle);
                            pj_thread_destroy(web_handle);
                            printf (">>>>>> In web client socket: web thread over ... \n");
                            return -1;
                        } else {
                            printf (">>>>>> In web client socket: buff lost ... \n");
                        }
                    }
                        break;
                    case SET_ON_OFF_INFR: {
                        printf(">>>>>> In web client socket: SET_ON_OFF_INFR\n");
                        
                        int data = web_action->data;
                        int obj = web_action->obj;
                        int result = protocol_ifrared_open(obj, data);
                        
                        memset(send_buf, 0, sizeof(send_buf));
                        web_action_report_t *send_data =
                        (web_action_report_t *)send_buf;
                        
                        send_data->data = result;
                        send_data->fun_code = SET_ON_OFF_INFR;
                        send_data->source = HOST_BACKGROUND_PROGRAM;
                        send_data->start_flag = BUFF_START;
                        
                        int send_len = write(com_fd, send_buf,sizeof(send_buf));
                        if (send_len == WEB_BUFF_LEN) {  //send success
                            printf(">>>>>> In web client socket: send respose success: %s\n",send_buf);
                        } else if (send_len < 0){
                            perror(">>>>>> In web client socket: send on stream error\n");
                            
                            close(com_fd);
                            close(listen_fd);
                            unlink(UNIX_DOMAIN);
                            // destroy the thread
                            pj_thread_join(web_handle);
                            pj_thread_destroy(web_handle);
                            printf (">>>>>> In web client socket: web thread over ... \n");
                            return -1;
                        } else {
                            printf (">>>>>> In web client socket: buff lost ... \n");
                        }
                    }
                        break;
                    case GET_ON_OFF_SERIAL: {
                        printf(">>>>>> In web client socket: GET_ON_OFF_SERIAL\n");

                        int data;
                        int obj = web_action->obj;
                        protocol_is_serial_open(obj, &data);
                        
                        memset(send_buf, 0, sizeof(send_buf));
                        web_action_report_t *send_data =
                        (web_action_report_t *)send_buf;
                        
                        send_data->data = data;
                        send_data->fun_code = GET_ON_OFF_SERIAL;
                        send_data->source = HOST_BACKGROUND_PROGRAM;
                        send_data->start_flag = BUFF_START;
                        
                        int send_len = write(com_fd, send_buf,sizeof(send_buf));
                        if (send_len == WEB_BUFF_LEN) {  //send success
                            printf(">>>>>> In web client socket: send respose success: %s\n",send_buf);
                        } else if (send_len < 0){
                            perror(">>>>>> In web client socket: send on stream error\n");
                            
                            close(com_fd);
                            close(listen_fd);
                            unlink(UNIX_DOMAIN);
                            // destroy the thread
                            pj_thread_join(web_handle);
                            pj_thread_destroy(web_handle);
                            printf (">>>>>> In web client socket: web thread over ... \n");
                            return -1;
                        } else {
                            printf (">>>>>> In web client socket: buff lost ... \n");
                        }
                        
                    }
                        break;
                    case GET_ON_OFF_GW: {
                        printf(">>>>>> In web client socket: GET_ON_OFF_GW\n");
                        
                        int data;
                        int obj = web_action->obj;
                        protocol_is_gateway_open(obj, &data);
                        
                        memset(send_buf, 0, sizeof(send_buf));
                        web_action_report_t *send_data =
                        (web_action_report_t *)send_buf;
                        
                        send_data->data = data;
                        send_data->fun_code = GET_ON_OFF_GW;
                        send_data->source = HOST_BACKGROUND_PROGRAM;
                        send_data->start_flag = BUFF_START;
                        
                        int send_len = write(com_fd, send_buf,sizeof(send_buf));
                        if (send_len == WEB_BUFF_LEN) {  //send success
                            printf(">>>>>> In web client socket: send respose success: %s\n",send_buf);
                        } else if (send_len < 0){
                            perror(">>>>>> In web client socket: send on stream error\n");
                            
                            close(com_fd);
                            close(listen_fd);
                            unlink(UNIX_DOMAIN);
                            // destroy the thread
                            pj_thread_join(web_handle);
                            pj_thread_destroy(web_handle);
                            printf (">>>>>> In web client socket: web thread over ... \n");
                            return -1;
                        } else {
                            printf (">>>>>> In web client socket: buff lost ... \n");
                        }
                    }
                        break;
                    case GET_ON_OFF_INFR: {
                        printf(">>>>>> In web client socket: GET_ON_OFF_INFR\n");
                        
                        int data;
                        int obj = web_action->obj;
                        protocol_is_ifrared_open(obj, &data);
                        
                        memset(send_buf, 0, sizeof(send_buf));
                        web_action_report_t *send_data =
                        (web_action_report_t *)send_buf;
                        
                        send_data->data = data;
                        send_data->fun_code = GET_ON_OFF_INFR;
                        send_data->source = HOST_BACKGROUND_PROGRAM;
                        send_data->start_flag = BUFF_START;
                        
                        int send_len = write(com_fd, send_buf,sizeof(send_buf));
                        if (send_len == WEB_BUFF_LEN) {  //send success
                            printf(">>>>>> In web client socket: send respose success: %s\n",send_buf);
                        } else if (send_len < 0){
                            perror(">>>>>> In web client socket: send on stream error\n");
                            
                            close(com_fd);
                            close(listen_fd);
                            unlink(UNIX_DOMAIN);
                            // destroy the thread
                            pj_thread_join(web_handle);
                            pj_thread_destroy(web_handle);
                            printf (">>>>>> In web client socket: web thread over ... \n");
                            return -1;
                        } else {
                            printf (">>>>>> In web client socket: buff lost ... \n");
                        }
                        
                    }
                        break;
                    default:
                        break;
                }
            }else {
                printf(">>>>>> In web client socket: the buff is not avalible struct\n");
            }

        }
        
        close(com_fd);


    }while(TRUE);

	close(listen_fd);
	close(com_fd);
    unlink(UNIX_DOMAIN);
    
    printf(">>>>>> In web client socket: socket has colsed\n");
    // destroy the thread
    pj_thread_join(web_handle);
    pj_thread_destroy(web_handle);

	return 0;
}

void web_update()
{
	update_query(QUERY_FROM_WEB_UPD);
}

/*
// update
void web_update()
{
	update_query();
}

//update software
void software_update()
{
	char big_v = (char)SOFTWARE_BIG_VERSION;
	char small_v = (char)SOFTWARE_SMALL_VERSION;
	software_update_action(get_server_session_id(),
							big_v,
							small_v,
							HOST_S);
}
*/

//stop host network by web
int stop_host_network_from_web(int open)
{
    printf(">>>>>> In stop_host_network_from_web: request = %d\n",open);
	LOCK_MUTEX(&mux_lock_network_stop_flag);
	g_is_net_connect_stoped = open==TRUE?FALSE:TRUE;
	UNLOCK_MUTEX(&mux_lock_network_stop_flag);
    if (TRUE == g_is_net_connect_stoped) {
        db_set_is_stop_net(TS_STOP_NET);
    }else if (FALSE == g_is_net_connect_stoped) {
        db_set_is_stop_net(TS_NOT_STOP_NET);
    }
	return g_is_net_connect_stoped;
}
