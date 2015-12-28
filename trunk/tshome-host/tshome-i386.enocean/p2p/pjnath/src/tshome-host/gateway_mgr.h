/*
 * gateway_mgr.h
 *
 *  Created on: Feb 7, 2012
 *      Author: SongShanping
 */

#ifndef GATEWAY_MGR_H_
#define GATEWAY_MGR_H_

#include <stdio.h>
#include <stdlib.h>
#include "common/list.h"

#include <pjlib.h>

/*
 *data struct for gateway report queue
 */
typedef struct _gateway_report
{
    struct list_head list_node; //list node
    int type;                   // device type. no used now.
    void *data;                 // knx report msg
    int buff_len;               // len of msg
}gateway_report_t;

/*
 * destroy the list of gateway report.
 */
void destroy_gateway_report_list();

/*
 * Gateway call back method
 * send receive message buff to report process
 */
void recv_gateway_buff_callback(int type, void *data, int buff_len);

/*
 * Gateway report handle method
 */
int gateway_report_handle(void *param);

void deal_with_gateway_report(int type, void *data, int len);

void init_gateway_thread_environment();

void destroy_gateway_thread_environment();

void start_gateway_report_handle_thread(pj_pool_t *param);

void stop_gateway_report_handle_thread();

#endif /* GATEWAY_MGR_H_ */
