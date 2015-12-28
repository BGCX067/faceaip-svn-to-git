/*
 * common_define.h
 *
 *  Created on: Feb 9, 2012
 *      Author: SongShanping
 */
#ifndef COMMON_DEFINE_H_
#define COMMON_DEFINE_H_

#include "ts_err_code.h"
#include "remote_def.h"

//software version
#define PRODUCT_ID	"TSHome-H1-100_1.1.3"
//configure file version
#define CONFIG_ID	"TSHome-H1-100_1.1.3"
//the product and vender id len
#define PRODUCT_VENDOR_LEN    13
//the version length
#define VERSION_LEN           19

//protocol version
#define PROTOCOL_VERSION	1

//common define
#define CHECK_CODE "tiansu"

//"cmdh:"
#define PJSIP_CMDH_HEADER ""
#define LEN_CMDH_HEADER strlen(PJSIP_CMDH_HEADER)

//"cmdc:"
#define PJSIP_CMDC_HEADER ""
#define LEN_CMDC_HEADER strlen(PJSIP_CMDC_HEADER)

//program name
#define TS_PRO_PATH "/opt/tshome-host/"
#define TS_PROGRAM_BIN_PATH "/opt/tshome-host/bin/"
//#define TS_PROGRAM_NAME "tshome-host-i686-pc-linux-gnu"
#define TS_PROGRAM_NAME "tshome-host"
#define TS_MONITOR_NAME "ts_monitor"
#define TS_MONITOR_WEB_NAME "ts_web_monitor"
#define TS_WEB_SERVER_NAME "webs"
#define TS_RUNNING 1
#define TS_NO_RUNNING 0

//web to gateway.so
#define TS_LIB_GATEWAY_2_WEB "/opt/tshome-host/lib/libGatewayManager.so"

// the serial port index which is used to Enocean.
#define PROTOCOL_ENOCEAN_SERIAL_DB_ID	3

//The number of people who can control the host programe
#define TS_NUM_CONTROL_HOST 5

#define PJ_SEVER_THREAD_ID 0

#define PJ_SERVER_THREAD_MEMORY_SIZE 4096

#define PJ_GATEWAY_THREAD_MEMORY_SIZE 2048

#define PJ_WEB_THREAD_MEMORY_SIZE 4096

#define SOFTWARE_BIG_VERSION		1

#define SOFTWARE_SMALL_VERSION		12

#define HOST_NAME_LEN   32

#define HOST_PWD_LEN    32

#define REPORT_HEADER_LEN (sizeof(remote_header_t))

#define DB_FILE_PATH  "/opt/tshome-host/db/tshome.db"
#define DB_FILE_PATH_BAK  "/opt/tshome-host/db/tshome.db_bak"

#define COM_SO_PATH "/opt/tshome-host/lib/"
#define GATEWAY_SO_PATH "/opt/tshome-host/lib/libGatewayManager.so"

#define PYTHON_CODE_PATH "/opt/tshome-host/bin/python/"

//the path of update bin.
#define UPDATE_BIN_PATH "/opt/tshome-host/update/"
#define UPDATE_BIN_FILE "upd.sh"
#define BACKUP_BIN_FILE "bak.sh"

//the path of config file after download.
#define CONFIG_FILE_SAVE_PATH "/opt/tshome-host/update/"   

//the path of software file after download.
#define SOFTWARE_FILE_SAVE_PATH "/opt/tshome-host/update/"   


#define TRUE 1

#define FALSE 0

/*
 begin: terminal info struct in host
 */
typedef struct _terminal_identify_info
{
	unsigned p_k;
	unsigned comp_id;
	char ip_addr[32];
	int addr_len;
	int32_t session_id;
	char user_name[32];
	int len_name;
	char passwd[32];
	int len_pwd;
	char aes_key[128];
	PROTOCOL_TYPE net_type;
}terminal_identify_info_t;
/*
 end
 */


/*
begin: communication between web function and background
 */
// Function code
typedef enum
{
	CONFIG_UPDATE = 1,			//configure update
	SOFTWARE_UPDATE = 2,		//software update
	STOP_HOST_NETWORK = 3,		//stop host network
	ON_OFF_HOST = 4		,		//on off host
	RESTART_HOST = 5		,	//restart host
	SET_ON_OFF_SERIAL = 6	,	//on off interface
	SET_ON_OFF_GW = 7	,		//on off interface
	SET_ON_OFF_INFR = 8	,		//on off interface
	GET_ON_OFF_SERIAL = 9	,	//on off interface
	GET_ON_OFF_GW = 10	,		//on off interface
	GET_ON_OFF_INFR = 11,		//on off interface
	WEB_UPDATE = 12,			//Update software
	EXPORT_LOG = 13,			//export log
	SET_LOACL= 14,				//set local
	WEB_UPD_GUIDE= 15		//update conf in guide
} WEB_FUN_CODE;

#define HOST_BACKGROUND_PROGRAM 	1
#define HOST_WEB_PROGRAM 			2

#define UNIX_DOMAIN "/tmp/UNIX.domain"

#define WEB_BUFF_LEN 		(sizeof(web_action_report_t)+1)

#define BUFF_START			 0x7F

typedef struct _web_action_report
{
	int start_flag;
	int fun_code;
	int source;
	int data;  // 0 or 1
	int obj;//id or channel
}web_action_report_t;
/*
 end
 */

#endif /* COMMON_DEFINE_H_ */


