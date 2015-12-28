/*
 * ts_select_db_infos.h
 *
 *  Created on: 2012-1-17
 *      Author: pangt
 */

#ifndef TS_SELECT_DB_INFOS_H_
#define TS_SELECT_DB_INFOS_H_

#include <time.h>
#include <sys/types.h>

#include "sqlite3_ctrl.h"
#include "common/list.h"
#include "common/ts_err_code.h"

#define TS_HOST_INTERFACE_TYPE_KNX 1//主机口类型:KNX
#define TS_HOST_INTERFACE_TYPE_IP 2//主机口类型:IP口
#define TS_HOST_INTERFACE_TYPE_INFRARED 3//主机口类型:红外
#define TS_HOST_INTERFACE_TYPE_SERIAL 4//主机口类型:串口
#define TS_HOST_GATEWAY_TYPE_NO_GATEWAY 0//网关类型：无网关
#define TS_GATEWAY_TYPE_KNX 1//网关类型：KNX
#define TS_GATEWAY_TYPE_WIFI 2//网关类型：WIFI
#define TS_GATEWAY_TYPE_HOST 3//网关类型：主机口
#define TS_DEVICE_TYPE_WIFI 6
#define TS_KNX_GROUP_TYPE_CONTROL 1
#define TS_KNX_GROUP_TYPE_RES 2

#define TS_KNX_GROUP_PERMISSION_READ 1
#define TS_KNX_GROUP_PERMISSION_WRITE 2
#define TS_KNX_GROUP_PERMISSION_RW 3

#define TS_DEVICE_TYPE_LIGHT 1
#define TS_DEVICE_TYPE_CURTAIN 2
#define TS_DEVICE_TYPE_AC 3
#define TS_DEVICE_TYPE_PROTECT 4
#define TS_DEVICE_TYPE_VIDEO 5
#define TS_DEVICE_TYPE_THREATER 6
#define TS_DEVICE_TYPE_MUSIC 7
#define TS_DEVICE_TYPE_YARD 8

#define TS_KNX_GROUP_ADDR_INVALID -1

#define GROUP_ADDRESS_FORMAT "%d%d%d"

#ifdef __cplusplus
extern "C"
{
#endif

struct _ts_knx_group
{
	struct list_head list;
	int32_t groudbd;
	char address[50];
	int32_t dataType;
	int32_t dataValue;
	int32_t minValue;
	int32_t maxValue;
	int32_t controlUnitId;
	int32_t groupType;
}typedef ts_knx_group;

/**
 * get knx group infos
 */
void db_get_knx_group(struct list_head * group_head_list, ts_knx_group * p_knx_group);

//get all interface of host
struct _host_interface
{
	int32_t id;
	u_char desc[64];
	int32_t type;
	u_char type_name[64];
	u_char info[64];
}typedef ts_host_interface_t;

struct _host_interface_list
{
	struct list_head list;
	ts_host_interface_t host_interface;
}typedef ts_host_interface_list_t;

/**
 * @param group_head_list : head_addr
 * @param host_interface ：list for host's interface infomations
 * @return whether the function run success
 */
int32_t db_get_host_interfaces(struct list_head * head_list, ts_host_interface_list_t * host_interface);

//(2)查询所有KNX、Wifi网关通路的信息
struct _gateway
{
	int32_t id;
	u_char name[32];
	u_char gatewayIP[50];
	int32_t gatewayPort;
	u_char description[255];
	int32_t getewayType;
	u_char gateway_type_name[32];
}typedef ts_gateway_t;

struct _gateway_list
{
	struct list_head list;
	ts_gateway_t gateway;
}typedef ts_gateway_list_t;

struct _passageway
{
	ts_host_interface_t host_interface; //the interface info of the host
	int32_t gateway_counts; //the counts of gateway which connect with the gateway
	ts_gateway_list_t * gateway_list; //the gate informations of the interface
	struct list_head gateway_heead;
}typedef ts_passageway_t;

struct _passageways_list
{
	struct list_head list;
	ts_passageway_t passageway;
}typedef ts_passageway_list_t;

/**
 * get all ip passage infos
 * @param group_head_list : head_addr
 * @param passageways ：list for passageways
 * @return whether the function run success
 */
int32_t db_get_all_lines(struct list_head * head_list, ts_passageway_list_t * passageways);

/**
 * get all ip passage infos
 * @param group_head_list : head_addr
 * @param passageways ：list for passageways
 * @param counts ：counts
 * @return whether the function run success
 */
int32_t db_get_all_lines_counts(struct list_head * head_list, ts_passageway_list_t * passageways, int32_t * counts);

typedef struct _knx_control_info
{
	u_char session_id[4]; //session id
	u_char device_type; //device type
	u_char home_device_id[4]; //device unique id at home
	u_char cmd_type; //KNX command type, READ/WRITE
	//	u_char main_addr; //main group address
	//	u_char mid_addr; //middle group address
	//	u_char logical_addr; //logical group address
	u_int16_t group_addr; //knx group address;
	u_char result_code; //result code
	u_char data_len; //data length
	u_char data_type; //group data type, 5 types
	u_char data[16]; //group data value
	u_char time[24]; //group data value
} ts_knx_control_info_t;

typedef struct _knx_control_info_list
{
	struct list_head list;
	ts_knx_control_info_t knx_info;
} ts_knx_control_info_list_t;

/**
 * get the group of knx value
 * @param group_head_list : knx_control_info
 * @return whether the function run success
 */
int32_t db_get_knx_device_curr_value(ts_knx_control_info_t * knx_control_info);

/**
 * set the group of knx value
 * @param group_head_list : knx_control_info
 * @return whether the function run success
 */
int32_t db_upd_knx_device_curr_value(ts_knx_control_info_t knx_control_res_t);

//(6）查询Wifi设备的主机口及网关信息；
/**
 * get the device passageway info
 * @param group_head_list : knx_control_info
 * @return whether the function run success
 */
int32_t db_get_interface_gateway(int32_t home_device_id, int32_t *interface_id, int32_t *gateway_id);

/**
 * get infrared device info : gateway id,interface id ,channel id
 * @return whether the function run success
 */
//int32_t db_get_infrared_channel(int32_t device_id, int32_t * channel_id);
int32_t db_get_infrared_device_info(int32_t home_device_id, int32_t *interface_id, int32_t *gateway_id, int32_t *channel_id);

/**
 * get the serial info ,interface id,gateway id,zone
 * @param group_head_list : knx_control_info
 * @return whether the function run success
 */
int32_t db_get_serial_device_info(int32_t home_device_id, int32_t *interface_id, int32_t *gateway_id, int32_t *zone, char *file_name, char *comm_port);

/**
 * insert,delete,update,select passageway
 * @return whether the function run success
 */
int32_t db_protocol_add_gateway_of_line(int32_t gateway_id, int32_t interface_id);
int32_t db_protocol_del_gateway_of_line(int32_t gateway_id);
int32_t db_protocol_modify_gateway_of_line(int32_t gateway_id, char *ip, int32_t port);

/**
 * get the group value by permission
 * @param group_head_list : knx_control_info
 * @return whether the function run success
 */
int32_t db_get_knx_groups(int32_t gateway_id, struct list_head * head, ts_knx_control_info_list_t * knx_group_list, int32_t permission);

//1、查询所有串口信息；
int32_t db_get_host_serial_interfaces(struct list_head * head_list, ts_host_interface_list_t * host_interface);

//2、查询红外通路总数；
int32_t db_get_host_infrared_count(int32_t * count);

typedef struct _ts_interface_serial_info
{
	int32_t id;
	int32_t interface_id;
	int32_t baud_rate;
	int32_t data_bit;
	float stop_bit;
	int32_t parity_check;
	int32_t comm_id;
	char comm_port[10];
} ts_host_interface_serial_info;

typedef struct _ts_interface_serial_info_list
{
	struct list_head list;
	ts_host_interface_serial_info host_serial;
} ts_host_interface_serial_info_list;

//3、查询/修改串口参数；
int32_t db_get_all_serial_details(struct list_head * head_list, ts_host_interface_serial_info_list * serial_infos, int32_t * counts);
int32_t db_get_serial_details(ts_host_interface_serial_info * serial_info);
int32_t db_set_serial_details(ts_host_interface_serial_info serial_info);
int32_t db_del_serial_details(ts_host_interface_serial_info serial_info);

//5、查询自定义串口设备按键的报文；
int32_t db_get_serial_datagram(int32_t deviceid, int32_t key_id, char * text);

//6、查询/更新主机口的连接状态；
int32_t db_get_interface_conn_state(int32_t id, int32_t * is_conn);
int32_t db_set_interface_conn_state(int32_t id, int32_t is_conn);

//7、查询/更新串口设备的当前信息。
int32_t db_get_serial_device_curr(int32_t device_id, char * curr_state);
int32_t db_set_serial_device_curr(int32_t device_id, const char * curr_state);

#ifdef __cplusplus
}
#endif

#endif /* TS_SELECT_DB_INFOS_H_ */
