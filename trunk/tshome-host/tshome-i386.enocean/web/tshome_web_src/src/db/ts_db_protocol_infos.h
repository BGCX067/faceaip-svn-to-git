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

#define TS_HOST_INTERFACE_TYPE_KNX 1//
#define TS_HOST_INTERFACE_TYPE_IP 2//
#define TS_HOST_INTERFACE_TYPE_INFRARED 3//
#define TS_HOST_INTERFACE_TYPE_SERIAL 4//
#define TS_HOST_GATEWAY_TYPE_NO_GATEWAY 0//
#define TS_GATEWAY_TYPE_KNX 1//
#define TS_GATEWAY_TYPE_WIFI 2//
#define TS_GATEWAY_TYPE_HOST 3//
#define TS_DEVICE_TYPE_WIFI 6

//#define TS_KNX_GROUP_TYPE_CONTROL 1
//#define TS_KNX_GROUP_TYPE_RES 2

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

#define TS_WIFI_DEV_TYPE_SERIAL 0
#define TS_WIFI_DEV_TYPE_INFRARED 1

#define TS_DEV_SUBTYPE_INFRARED 1
#define TS_DEV_SUBTYPE_SERIAL 2
#define TS_DEV_SUBTYPE_SERIAL_CUSTOM 3
#define TS_DEV_SUBTYPE_KNX 4
#define TS_DEV_SUBTYPE_ENOCEAN 5
#define TS_DEV_SUBTYPE_ZIGBEE 6

#define TS_INFRCODE_TYPE_0 0
#define TS_INFRCODE_TYPE_1 1

#define GROUP_ADDRESS_FORMAT "%d%d%d"

#define TS_ZONE_UNUSED -1

#define TS_SCENE_IS_USE 1
#define TS_SCENE_NOT_USE 0

#define TS_KNX_VALUE_FIRST_USE 0
#define TS_KNX_VALUE_NOT_FIRST_USE 1

#define TS_CONDITION_AND 0
#define TS_CONDITION_OR 1

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
	int32_t session_id; //session id
	int32_t home_device_id; //device unique id at home
	u_char device_type; //device type
	u_char cmd_type; //KNX command type, READ/WRITE
	//	u_char main_addr; //main group address
	//	u_char mid_addr; //middle group address
	//	u_char logical_addr; //logical group address
	u_int16_t group_addr; //knx group address;
	u_char result_code; //result code
	u_char data_len; //data length
	u_char data_type; //group data type, 5 types
	u_char space[3];
	u_char data[33]; //group data value
	u_char time[25]; //group data value
	int8_t is_first_use;
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
int32_t db_upd_knx_device_curr_value(ts_knx_control_info_t knx_control_res_t, int32_t gateway_id);

/**
 * get the device passageway info
 * @param group_head_list : knx_control_info
 * @return whether the function run success
 */
int32_t db_get_interface_gateway(int32_t home_device_id, int32_t *gateway_type, int32_t *gateway_id);

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
int32_t db_get_devtype_and_protrol(int32_t comm_id, char *file_name, int32_t * device_type);
//int32_t db_get_serial_device_info(int32_t comm_id, int32_t *gateway_id, int32_t *zone, char *file_name, char *comm_port);

//int32_t db_get_serial_device_info(int32_t device_id, char *file_name);
int32_t db_get_serial_device_info(int32_t device_id, int32_t *comm_id, int32_t *gateway_id, int32_t *zone, char *file_name, char *comm_port);

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

typedef struct _ts_interface_serial_info
{
	int32_t id;
	int32_t interface_id;
	int32_t baud_rate;
	int32_t data_bit;
	float stop_bit;
	int32_t parity_check;
	int32_t comm_id;
	char comm_port[64];
} ts_host_interface_serial_info;

typedef struct _ts_interface_serial_info_list
{
	struct list_head list;
	ts_host_interface_serial_info host_serial;
} ts_host_interface_serial_info_list;

int32_t db_get_all_serial_details(struct list_head * head_list, ts_host_interface_serial_info_list * serial_infos, int32_t * counts);
int32_t db_get_serial_details(ts_host_interface_serial_info * serial_info);
int32_t db_set_serial_details(ts_host_interface_serial_info serial_info);
int32_t db_del_serial_details(ts_host_interface_serial_info serial_info);

//get knx group data type by addr and gateway id
int32_t db_get_knx_group_type(int32_t gateway_id, ts_knx_control_info_t * knx_control);

//get gateway type by id
int32_t db_get_gateway_by_id(int32_t geteway_id, int32_t * gateway_type);

//get next readable group
int32_t db_get_next_readable_group(int32_t * n_group_id, int32_t * gateway_id, int16_t * grp_addr);

//get serial datagram
int32_t db_get_serial_datagram(int32_t deviceid, int32_t key_id, char * text, int32_t * len);

//int32_t db_get_device_type
int32_t db_get_device_type_by_id(int32_t * device_type, int32_t device_id);

//
int32_t db_get_serial_device_curr(int32_t device_id, int32_t *type_id, int32_t *zone, char * curr_state);
int32_t db_set_serial_device_curr(int32_t device_id, int32_t type_id, int32_t zone, const char * curr_state);

//get device type by device id
int32_t db_get_serial_device_type_by_id(int32_t * device_type, int32_t device_id);

//db_get_who_am i
int32_t db_get_who_am_i(int32_t * who);

/**
 * record infrared study code
 * @key code:the key code of the gateway
 */
int32_t db_set_infrared_code(int32_t gateway_id, int32_t key_code, char * text, int32_t type, int32_t len);

/**
 * get infrared study code
 * @key code:the key code of the gateway
 */
int32_t db_get_infrared_code(int32_t gateway_id, int32_t key_code, char * text, int32_t type, int32_t * len);

typedef struct _ts_scene_io_info_t
{
	int32_t dev_id;
	int32_t param; //knx:group addr,infr:unicode ,self serial:key_id,not self serial : key_function_id
	char value[32]; //knx:group value,infr:null,serial:if exits,set value like channel;
	int32_t delay; //delay time,unit : second
	float threshold;
} ts_scene_io_info_t;

typedef struct _ts_scene_io_info_list_t
{
	struct list_head list;
	ts_scene_io_info_t scene_info;
} ts_scene_io_info_list_t;

typedef struct _ts_scene_info_t
{
	int32_t id;
	int32_t is_use;
	int32_t and_or;
} ts_scene_info_t;

//typedef struct _ts_scene_info_list_t
//{
//	struct list_head list;
//	ts_scene_info_t scene_info;
//} ts_scene_info_list_t;

int32_t db_get_grp_id_by_addr(int32_t addr, int32_t dev_id, int32_t * grp_id);
//int32_t db_get_scene_id(int32_t ** scene_ids, int32_t * count, ts_scene_io_info_t scene_info,int32_t **is_use);
int32_t db_get_scene_infos(struct list_head * list, ts_scene_io_info_list_t * scene_infos, int32_t scene_id);

/**
 * give back threshold
 *
 */
int32_t db_get_scene_id(ts_scene_io_info_t *scene_in_info, int32_t * count, ts_scene_info_t **scene_info, int8_t is_use);

int32_t db_get_key_func_name(int key_func_id, char * key_fun_name);

//set string to hex		
void string_2_hex(char * text, char * result);

//set hex to string
void hex_2_string(char * text, int32_t len, char *result);

int32_t db_get_dev_by_serial(int32_t serial, int32_t *dev_id);

int32_t db_reset_knx_curr();

//int32_t db_get_all_scene_inputs(int scene_id, int * count, ts_scene_io_info_t *** in_infos);
int32_t db_get_all_scene_inputs(int scene_id, int * count, ts_scene_io_info_t ** in_infos);

int32_t db_reset_serial_dev_curr();

typedef struct _ts_serial_protrol_dev_t
{
	int32_t device_id;
	int32_t area;
	int32_t com_id;
} ts_serial_protrol_dev_t;

typedef struct _ts_serial_protrol_dev_list_t
{
	struct list_head list;
	ts_serial_protrol_dev_t devs;
} ts_serial_protrol_dev_list_t;

typedef struct _ts_dps_t
{
	int32_t dp_id;
	int32_t ctrl_type;
} ts_dps_t;

typedef struct _ts_dps_list_t
{
	struct list_head list;
	ts_dps_t dp_info;
} ts_dps_list_t;

int32_t db_get_serial_devs_by_protrol(struct list_head * list_head, ts_serial_protrol_dev_list_t * devs, char * pro_name);

int32_t db_get_device_infos_by_dp(int32_t dp_id, int32_t *dev_id, int32_t *subtype, int32_t *gateway_id, int32_t *gateway_type);

int32_t db_get_ctrltype_by_dp(int32_t dp_id, int32_t *ctrl_type);

int32_t db_get_enocean_id_by_dp(int32_t dp_id, u_int32_t *enocean_id, int32_t *rocker);

int32_t db_get_dp_by_enocean_id(int32_t e_id, struct list_head * list, ts_dps_list_t * dps);

int32_t db_get_dp_infos_by_dp(int32_t dp, int32_t *max_steps, int32_t *max_times);

int32_t db_get_dp_curr_value(int32_t dp, char * curr_value);

int32_t db_set_dp_curr_value(int32_t dp, char * curr_value);

int32_t db_get_dps_by_same_dev(int32_t dp, struct list_head * list, ts_dps_list_t * dps);

int32_t db_get_comm_by_enocean_id(u_int32_t e_id, ts_host_interface_serial_info * serial_info);

int32_t db_get_infr_driver(char * driver_name);

int32_t db_set_infr_driver(char * driver_name);

#ifdef __cplusplus
}
#endif

#endif /* TS_SELECT_DB_INFOS_H_ */

