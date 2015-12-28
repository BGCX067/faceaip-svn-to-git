/*
 * operation_gateway_mgr.h
 *
 *  Created on: Feb 9, 2012
 *      Author: SongShanping
 */

#ifndef OPERATION_GATEWAY_MGR_H_
#define OPERATION_GATEWAY_MGR_H_

#include "report_mgr.h"
#include "dev_ctrl/Conversion.h"
#include "start_up.h"
#include "common/common_define.h"
#include "common/remote_def.h"

typedef int (*INT_FUN_VOID)();

typedef int (*INT_FUN_VOIDP_INT)(void*, int);

typedef struct _knx_response_thread_param
{
    int device_id;
    KNX_GroupData grp_data;
    KNX_COMMAND_TYPE type;
}knx_response_thread_param;

/*
 * deal with the command to control device.
 */
int device_ctrl_handle(unsigned k, unsigned com_id, char *ctrl_msg, int len);

/*
 * start the gateway module.
 */
int start_gateway_module();

/*
 * end the gateway module.
 */
int end_gateway_module();


/*
 * callback of knx devices
 */
void on_search_response(int gw_index, char *ip, int port, int status);

KNX_DATA_TYPE get_data_type_from_dpt(KNX_DATA_POINT_TYPE dpt);

void knx_response_thread_handle(knx_response_thread_param* param);

void knx_response_handle(int device_id,
						KNX_GroupData grp_data,
						KNX_COMMAND_TYPE type);

void on_read_response(int device_id, KNX_GroupData grp_data);

void on_write_response(int device_id, KNX_GroupData grp_data);

void on_knx_connect(int gw_index, int success, int expId, char *strErr);

void on_knx_disconnect(int gw_index);

void on_rw_error(int gw_index,
					int exception_id,
					char *exceiption_dis,
					int success);

/*
 * callback of wifi devices
 */
void on_wifi_connect(int gw_index, int success);

void on_wifi_disconnect(int gw_index);

#endif /* OPERATION_GATEWAY_MGR_H_ */
