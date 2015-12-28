/*
 * IPCProtocolDef.h
 *
 *  Created on: 2012-2-7
 *      Author: sunzq
 */

#ifndef IPCPROTOCOLDEF_H_
#define IPCPROTOCOLDEF_H_

#include <sys/types.h>

// function code for header
typedef enum {
	UNKNOWN = 0x00,
	IPC_HEARTBEAT = 0x01,
	HOST_SERV_REQ = 0x02,
	NOTIFY_UPDATE = 0x03,
	CHECK_UPDATE = 0x04,
	ASK_UDPSYS_STATES = 0x05,
	CTRL_UDPSYS = 0x06,
	INFRARED_LEARN = 0x07,
	IPC_GLOBAL_ERROR = 0x08
}IPC_FUN_CODE;

// the datagram packet request type
typedef enum{
	IPC_REQUEST = 0x00,
	IPC_RESPONSE = 0x0F
}IPC_PACKET_TYPE;

//host service state constants
typedef enum{
	REQ_STOP_HOST = 0x00,
	REQ_STOP_ASK = 0x01,
	RES_S_STOPED = 0x02,
	RES_S_RUNNING = 0x03
}IPC_HOST_SERV_CONS;

//configuration update model
typedef enum{
	TALBE_CONTENT = 0x01,
	TABLE_STRUCT = 0x02
}IPC_UPDATE_MOD;

//command for udp-subsystem control
typedef enum{
	STOP_UDPSYS = 0x00,
	START_UDPSYS = 0x01,
	RESTART_UDPSYS = 0x02,
	OPEN_HEART = 0x03,
	CLOSE_HEART = 0x04
}IPC_CTRL_UDPSYS_CMD;

// common header
typedef struct{
	u_char start; // start flag
	u_char protocol; // protocol type
	int16_t length;	 //body length
	int32_t searilNum;
	u_char function; // function code
	u_char type;
	u_char space[2];
}IPC_HEADER;

typedef struct{
        IPC_HEADER header;
}IPC_DG_HEART;


// stop host service datagram structs
typedef struct{
	IPC_HEADER header;
	int32_t homeId;
	char userName[31]; // string : user name, nOcts
	u_char homeId_len; // length of the raw homeid string
	u_char cmd_state; //cmd for request,and state for response.
	u_char space[3];
} IPC_DG_HOST_SERV;

//notify update datagram structs
typedef struct{
	IPC_HEADER header;
	u_char homeId_len; // length of the raw homeid string
	int32_t homeId;
	/*char url[256]; // deciphered configuration update URL, nOcts
	u_char homeId_len;
	IPC_UPDATE_MOD update_mod;
	u_char base_len; //original text length
	u_char space;*/
}IPC_DG_NOTIFY_UPDATE;
/*
typedef struct{
	IPC_HEADER header;
        int32_t homeId;
	int32_t sessionId;
	u_char space[3];
}IPC_DG_NOTIFY_UPDATE_RES;
*/
//download configuration datagram structs
typedef struct{
	IPC_HEADER header;
	int64_t last_time;
	int32_t homeId;
	int32_t sessionId;
	u_char client_aes[16];
	u_char homeId_len; // length of the raw homeid string
	u_char query_from;
	u_char pid_len;
	u_char product_id[64];
}IPC_DG_CHECK_UPDATE_REQ;

typedef struct{
        IPC_HEADER header;
        int32_t homeId;
        int32_t sessionId;
	u_char homeId_len; // length of the raw homeid string
	u_char query_from;
        u_char cfg_force;
        u_char cfg_url_len;
	u_char cfg_url[128];
        u_char soft_force;
	u_char soft_url_len;
	u_char soft_url[256];
	u_char soft_md5[48];
}IPC_DG_CHECK_UPDATE_RES;
/*
typedef struct{
	IPC_HEADER	header;	//data gram header
	int32_t session_id;
	u_char	big_version;	//big version
	u_char	small_version;	//small version
	u_char	client_type;	//client type; HOST/IPHONE/IPAD/ANDROID
	u_char space;
} IPC_DG_SOFTWARE_REQ;

typedef struct{
	IPC_HEADER	header;	//data gram header
	int32_t session_id;
	u_char	url[256];			//URL of down load
} IPC_DG_SOFTWARE_RES;
*/

//ask udp subsystem run states request datagram structs
typedef struct{
	IPC_HEADER header;
}IPC_DG_ASK_UDPSYS_REQ;

//ask udp subsystem run states response datagram structs
typedef struct{
	IPC_HEADER header;
	int64_t runnedTime;
	int32_t m_num;
	int32_t port_num;
	u_char service;
	u_char heart;
	u_char thread_num; //used process numbers
	u_char space;
}IPC_DG_ASK_UDPSYS_RES;

//contrl udp-subsystem datagram structs
typedef struct{
	IPC_HEADER header;
	int cmd; //ctrl cmd,@see IPC_CTRL_UDPSYS_CMD
	u_char space[3];
}IPC_DG_CTRL_UDPSYS;

//infrared learning datagram structs
typedef struct{
	IPC_HEADER header;
	int32_t homeId;
	int32_t gateway_id;
	u_char homeId_len;
	u_char channel;// the infrared controller channel
	int16_t key;
	u_char result; // the learn result.Only use for response, 0x00 indicate success.
}IPC_DG_INF_LEARN;

//-----------------------global error----------------------------
typedef enum{
	ERR_SRV_STOPPED = 0x0001,
	ERR_HOST_UNLOGIN = 0x0002,
	ERR_USER_NO_PERMISSION = 0x0003,
	ERR_SRV_BUSY = 0X0004,
	ERR_SRV_STARTED = 0x0005,
	ERR_NO_SUCH_HOME = 0x0006,
	ERR_HOST_STOPPED_NET
}IPC_ERR_CONS;

typedef struct{
	IPC_HEADER header;
	int16_t err;
	u_char space[2];
}IPC_DG_ERR;
#endif /* IPCPROTOCOLDEF_H_ */
