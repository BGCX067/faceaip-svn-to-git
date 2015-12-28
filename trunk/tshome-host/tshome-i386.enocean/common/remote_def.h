/*
 * RemoteDef.h
 *
 *  Created on: Dec 23, 2011
 *      Author: SongShanping
 */

#ifndef REMOTEDEF_H_
#define REMOTEDEF_H_


#include <sys/types.h>
#include <stdbool.h>

#define TRUE 	1
#define FALSE	0

#define ERROR_CODE -1
#define INVALID_VALUE -1

#define HEAD_START 0x7F		//start of data gram

#define REPORT_MAX_LEN 512

#define HEADER_LEN (sizeof(remote_header_t))

// added by keyy------------------------------------
#define MAX_IP_LEN					32
#define MAX_KEY_FUN_NAME_LEN		32

#define TOTAL_COM_NUM				6
#define TOTAL_INFRARED_NUM			12
#define COM_MESSAGE_MAX_LEN			64
#define IR_MESSAGE_MAX_SHORT_LEN	768


#define CONF_URL_MAX_LEN			128
#define FILE_URL_MAX_LEN			256

#define CURRENT_STATE_LEN   		256


// not used.
/*
typedef enum
{
	IR_CODE_TYPE_STANDARD,
	IR_CODE_TYPE_RC6
} IR_CODE_TYPE;
*/
// added by keyy------------------------------------


#define TS_PRO_TYPE_HOST			0
#define TS_PRO_TYPE_GATEWAY			1

typedef enum
{
	INTERNET = 0,	//communication from internet
	INTRANET = 1	//communication from intranet
}PROTOCOL_TYPE;

typedef enum
{
	QUERY_FROM_HOST = 0x01,
	QUERY_FROM_WEB_UPD
} UPDATE_QUERY_FROM_TYPE;

// Function code
typedef enum 
{
	GLOBAL_ERROR = 0x00,	//global error
	HEARTBEAT = 0x01,		//heart beat
	ASK_SECRET_KEY = 0x02,	//ask for secret key
	PASS_SECRET_KEY = 0x03,	//pass secret key
	LOGIN = 0x04,			//login
	UPDATE_QUERY = 0x05,	//check configuration info
	UPDATE_ISSUE = 0x06,	//check software update
	P2P = 0x07,				//P2P
	DEVICE_OPERATION = 0x08,//device operation
	STOP_CONNECTION = 0x09,	//request to stop host network comm
	LOGOUT = 0xA,			//logout
	PASS_TERM_SESSION = 0xB, //server pass the terminal session id to host
	DEVICE_CONTROL = 0xc,	//device general control:req. resp. & auto back data
	DEVICE_STATE = 0xd		//device current state req. & resp
} FUNCTION_CODE;

typedef enum
{
	EQUAL_UNKNOWN = 0x00,	// unknown
	GREATER_THAN,			// >
	LESS_THAN,				// <
	EQUAL,					// =
	NOT_EQUAL,				// !=
	GREATER_EQUAL,			// >=
	LESS_EQUAL				// <=
}SYMBOL;
	

// Communication type code
typedef enum 
{
	REQUEST = 0x01,			//request
	RESPONSE,				//response
	ERROR,
	NOTRESPONSE				// do not response
} COMM_TYPE;

typedef enum
{
	GE_NO_LOGIN = 1,
	GE_NO_USER = 2,
    GE_PARSE_ERROR = 3,
    GE_NO_HOST_CONNECT = 4,
	GE_PROTOCOL_UNMATCH = 5,
	GE_CONTROL_NO_PERMISION = 6,
	GE_NO_TERMINAL_INFO = 7,
	GE_RE_LOGIN,
	GE_NET_HAS_STOPED,
	GE_OVER_MAX_CONNECT_COUNT
}GLOBAL_ERROR_TYPE;

// Communication direction code
typedef enum 
{
    CLIENT = 0x00,			//client
	SERVER = 0x01,			//server
    HOST_S = 0x02,			//host
    ETS_CLIENT = 0x03		//ETS Client
} COMM_SOURCE;

typedef enum 
{
    CUT_Unkonw = 0,
    cut_1_LightOnOff = 1,
    cut_1_LightDimmer,
    cut_1_LightFault,
    cut_1_LightCurrent,
    
    cut_2_CurtainRightLeft = 10,
    cut_2_CurtainUpDown,
    cut_2_CurtainStop,
    cut_2_CurtainAngle,
    cut_2_CurtainDistance,
    cut_2_CurtainUp,
    cut_2_CurtainDown,
    cut_2_CurtainLeft,
    cut_2_CurtainRight,
    
	cut_3_AcWindSpeedLow = 30,
	cut_3_AcWindSpeedMid,
	cut_3_AcWindSpeedHigh,
	cut_3_AcModeCool,
	cut_3_AcModeHeat,
	cut_3_AcModeAuto,
	cut_3_AcModeWind,
	cut_3_AcModeDry,
    cut_3_AcModeComfort,
    cut_3_AcModeSleep,
    cut_3_AcModeEcon,
    cut_3_AcOnOff,
    cut_3_AcTempretrue,
    cut_3_AcTempCool,
    cut_3_AcTempHeat,
    cut_3_AcWindSpeedLMH,
	cut_3_AcModeHeatCool,
    
    cut_4_SecurityWindow = 60,
	cut_4_SecurityInf,
	cut_4_SecuritySmoke,
	cut_4_SecurityGas,
	cut_4_SecurityOn,
    cut_4_SecurityOff,
    cut_4_SecurityOnOff,
    
    cut_6_ThreaterInfra = 80,
    cut_6_ThreaterSerial
} CUT_CONTROLUINIT_TYPE;

// absolute dimming value is [0..100]
// relative dimming value is as the enum.
typedef enum _ENOCEAN_DIMMING_VALUE
{
	ENOCEAN_DIMMING_DOWN = 101,
	ENOCEAN_DIMMIMG_UP
}ENOCEAN_DIMMING_VALUE;


// KNX data point types
typedef enum
{
	DPT_Unknown = 0,
	DPT_Boolean,
	DPT_1BitCtrl,
	DPT_3BitCtrl,
	DPT_CharSet,
	DPT_8BitUnSign,
	DPT_8BitSign,
	DPT_2OctUnSign,
	DPT_2OctSign,
	DPT_2OctFloat,
	DPT_Time,
	DPT_Date,
	DPT_4OctUnSign,
	DPT_4OctSign,
	DPT_4OctFloat,
	DPT_Access,
	DPT_String
} KNX_DATA_POINT_TYPE;

//KNX Data types
typedef enum
{
	DATA_TYPE_UNKNOWN = 0,
	DATA_TYPE_BINARY,
	DATA_TYPE_ANALOG,
	DATA_TYPE_TEXT,
	DATA_TYPE_DATE,
	DATA_TYPE_TIME,
    DATA_TYPE_INT
} KNX_DATA_TYPE;

//IP type
typedef enum
{
    IPV4 = 0x00,
    IPV6
} IP_TYPE;

//KNX Read/Write command type
typedef enum
{
    READ = 0x00,			//command for get knx response
    WRITE,					//command for send knx control
    AUTOBACK
} KNX_COMMAND_TYPE;

//WIFI device types
typedef enum
{
	/* list order depending on db */
    INFRARED = 1,			//device of infrared
    COM,					//device of COM
	CUSTOMER_COM,			//device of CUSTOMER COM
	KNX,					//device of KNX
	ENOCEAN,				//device of EnOcean
	ZIGBEE,					//device of Zigbee
    WIFI,					//device of WIFI, not used
    IR_STUDY,				//study of infrared
    SCENE,                  //control of scene
    SERIAL_RES,				//serial port initiative response
} DEVICE_TYPE;


typedef enum
{
	UNKNOWN_DEVICE = 0,
	EPSON_ESC_VP21,
	BACKAUDIO_A61L,
	AUXDIO_8150,
	YODAR_4028B,
	ISTV,
	AQM_1000
} PREDEF_COM_DEV_TYPE;

typedef enum
{
	SWITCH_ON = 1,
	SWITCH_OFF
} SWITCH_TYPE;

typedef enum
{
	PLAY = 1,
	PAUSE,
	STOP
} PLAY_TYPE;

typedef enum
{
	PREV = 1,
	NEXT
} PLAY_PREV_NEXT;

typedef enum
{
	MUTE_OFF = 0,
	MUTE_ON
} MUTE_TYPE;

typedef enum
{
	VOLUME__10DB = 0,
	VOLUME__8DB,
	VOLUME__6DB,
	VOLUME__4DB,
	VOLUME__2DB,
	VOLUME_0DB,
	VOLUME_2DB,
	VOLUME_4DB,
	VOLUME_6DB,
	VOLUME_8DB,
	VOLUME_10DB
} MUSIC_VOLUME_VALUE;

typedef enum
{
	SOURCE_NOCHANGE = 0,
	SOURCE_MP3 = 1,
	SOURCE_TNR,
	SOURCE_DVD,
	SOURCE_PC,
	SOURCE_PC_2,
	SOURCE_TV,
	SOURCE_AUX,
	SOURCE_AUX2,
	SOURCE_IPOD,
	SOURCE_AM,
	SOURCE_FM,
	SOURCE_DVI_HDMI,
	SOURCE_HDMI,
	SOURCE_VIDEO,
	SOURCE_S_VIDEO,
	SOURCE_BNC,
	SOURCE_AV
} DATA_SOURCE;

typedef enum
{
	EQ_NORMAL = 0,
	EQ_POP,
	EQ_SOFT,
	EQ_CLASSIC,
	EQ_JAZZ,
	EQ_METAL
} MUSIC_EQ_TYPE;

typedef enum
{	
	VOLUME_UP = 1,
	VOLUME_DOWN
} VOLUME_CTRL;

typedef enum
{
	VOC_LEVEL_NORMAL = 0,
	VOC_LEVEL_HIGHT,
	VOC_LEVEL_VERY_HIGHT
} VOC_LEVEL_VALUE;


/*
//Auxdio music host ctrl type
typedef enum
{
}AUXDIO_CTRL_TYPE;

//Auxdio music auto response type id
typedef enum
{
	AUXDIO_AUTO_RES_SWITCH = 1,    	//SWITCH_TYPE
	AUXDIO_AUTO_RES_MAIN_VOL = 2,  	//int value : 0-100
	AUXDIO_AUTO_RES_TREBLE_VOL,    	//MUSIC_VOLUME_VALUE
	AUXDIO_AUTO_RES_BASS_VOL,	   	//MUSIC_VOLUME_VALUE
	AUXDIO_AUTO_RES_SOURCE,		   	//DATA_SOURCE
	AUXDIO_AUTO_RES_NAME,			//string value
	AUXDIO_AUTO_RES_U_DESK,			//U_DESK_VALUE
	AUXDIO_AUTO_RES_PLAY_STATUS,	//PLAY_TYPE
	AUXDIO_AUTO_RES_TIME_LEN,		//int value; total length: in seconds
	AUXDIO_AUTO_RES_TIME_PROGRESS,	//int value; current progress: in seconds
	AUXDIO_AUTO_RES_TNR_TYPE,		//TNR_TYPE
	AUXDIO_AUTO_RES_MUTE,			//int value; 1: mute, 0: un-mute
	AUXDIO_AUTO_RES_STEREO,			//int value; 1: stereo, 0: un-stereo
	AUXDIO_AUTO_RES_AM_CHANNEL,		//int value; 1-40
	AUXDIO_AUTO_RES_FM_CHANNEL		//int value; 1-40
}AUXDIO_AUTO_RESPONSE_TYPE;
*/

typedef enum
{
	BACK_EPSON = 1,
	BACK_BACKAUDIO,
	BACK_AUXDIO_02,
	BACK_AUXDIO_03,
	BACK_AUXDIO_04,
	BACK_AUXDIO_05,

	// com handled data
	BACK_SWITCH,
	BACK_PLAY,
	BACK_VOLUME,
	BACK_SOURCE,
	BACK_EQ,
	BACK_MUTE,
	BACK_SONG_NAME,

	BACK_AQM_1000
}BACK_MSG_TYPE;

typedef struct _backaudio_back
{
	unsigned char volume;
	unsigned char eq;
	unsigned char source;
}backaudio_back_t;

typedef struct _auxdio_back_02_t
{
	unsigned char address;
	unsigned char source;
	unsigned char on_off;
	unsigned char main_volume;
	unsigned char treble_volume;
	unsigned char bass_volume;
}auxdio_back_02_t;

typedef struct _auxdio_back_03_t
{
	unsigned char address;
	unsigned char len;
	char music_name[0];
}auxdio_back_03_t;

typedef struct _auxdio_back_04_t
{
	unsigned char address;
	unsigned char has_u_disk;
	unsigned char play_status;
	unsigned short total_sec;
	unsigned short current_sec;
}auxdio_back_04_t;

typedef struct _auxdio_back_05_t
{
	unsigned char address;
	unsigned char am_fm;
	unsigned char mute;
	unsigned char stereo;
	unsigned char channel;
	unsigned short frequency_100_times;
}auxdio_back_05_t;

typedef struct _com_back_info
{
	unsigned char back_type;
	char back_struct[0];
}com_back_info_t;

typedef struct _epson_esc_vp21_state
{
	int device_id;
	unsigned char power;
	unsigned char source;
	unsigned char av_power;
	unsigned char av_source;
} epson_esc_vp21_state;

typedef struct _backaudio_a61l_state
{
    int device_id;
	unsigned char address;
    unsigned char power;
    unsigned char play_state;
	backaudio_back_t back;
} backaudio_a61l_state;

typedef struct _auxdio_8150_state
{
    int device_id;
	unsigned char address;
	unsigned char play_state;
	auxdio_back_02_t back_02;
	auxdio_back_03_t back_03;
	auxdio_back_04_t back_04;
	auxdio_back_05_t back_05;
} auxdio_8150_state;

typedef struct _yodar_4028b_state
{
    int device_id;
	unsigned char address;
	unsigned char on_off;
	unsigned char play_pause;
	unsigned char volume;
	unsigned char source;
	unsigned char eq;
	unsigned char mute;
	unsigned char song_name[256];
} yodar_4028b_state;

typedef struct _aqm_1000_state
{
	int co2_concentration;
	float temperature;
	float humidity_per;
	unsigned char voc_level;
} aqm_1000_state;

typedef struct _general_single_state
{
	unsigned char type;
	unsigned char value;
} general_single_state;

typedef struct _general_string_state
{
	unsigned char type;
	unsigned char value[256];
	unsigned char len;
} general_string_state;



/*
//Back audio music auto response type id
typedef enum
{
	BACKAUDIO_AUTO_RES_VOLUME = 1,	//int value : 0-31
	BACKAUDIO_AUTO_RES_EQ,			//MUSIC_EQ_TYPE
	BACKAUDIO_AUTO_RES_SOURCE,		//DATA_SOURCE
	BACKAUDIO_AUTO_RES_DISTRICT		//int value
}BACKAUDIO_AUTO_RESPONSE_TYPE;
*/

//Epson AV TYPE
typedef enum
{
	EPSON_AV_MSEL_00 = 1,
	EPSON_AV_MSEL_01,
	EPSON_AV_MSEL_02
}EPSON_AV_TYPE;

typedef enum
{
	// values of functions
/* ISTV function key values */
	FUN_DIRECTION_UP = 1,
	FUN_DIRECTION_DOWN,
	FUN_DIRECTION_LEFT,
	FUN_DIRECTION_RIGHT,
	FUN_FORWARD_1,
	FUN_FORWARD_2,
	FUN_BACKWARD_1,
	FUN_BACKWARD_2,
	FUN_OK,
	FUN_EPG,
	FUN_HOME,
	FUN_MENU,
	FUN_RECALL,
	FUN_BACK,
	FUN_LANGUAGE,
	FUN_CAPTION
}FUNCTION_KEY_VALUE;

typedef enum
{
	CHANNEL_UP = 1,
	CHANNEL_DOWN
}CHANNEL_UPDOWN;

typedef enum
{
	TV_MODE_WELCOME = 1,
	TV_MODE_3D_0,
	TV_MODE_3D_1,
	TV_MODE_3D_2,
	TV_MODE_3D_3,
	TV_MODE_3D_4,
	TV_MODE_3D_5,
	TV_MODE_3D_6,
	TV_MODE_3D_7,
}TV_MODE_VALUE;


//ctrl type
typedef enum
{
/*	EPSON_ESC_VP21 ctrl type	*/
	CTRL_SWITCH = 1,
	CTRL_SOURCE,
	CTRL_AV_SWITCH,
	CTRL_AV_TYPE,

/*	BACKAUDIO_A61L ctrl type	*/
	//CTRL_SWITCH,
	CTRL_PLAY,
	CTRL_PREV_NEXT,
	CTRL_VOLUME,
	//CTRL_SOURCE,
	CTRL_DIRECTORY,
	CTRL_EQ_MODE,
	
/*	AUXDIO_8150 ctrl type	*/
	//CTRL_SWITCH,
	//CTRL_PLAY,
	//CTRL_PREV_NEXT,
	//CTRL_MAIN_VOL,
	CTRL_TREBLE_VOL,
	CTRL_BASS_VOL,
	//CTRL_SOURCE,
	CTRL_TNR_TYPE,				//TNR_TYPE
	CTRL_CHANNEL_AM,			//int value: 1-40
	CTRL_CHANNEL_FM,			//int value: 1-40
	//CTRL_DIRECTORY

/*	YODAR ctrl type	*/
	//CTRL_SWITCH,
	//CTRL_PLAY,
	//CTRL_PREV_NEXT,
	//CTRL_VOLUME,
	//CTRL_SOURCE,
	//CTRL_EQ_MODE,
	CTRL_MUTE,

/*	ISTV ctrl type */
	//CTRL_MUTE,
	//CTRL_SWITCH,
	//CTRL_VOLUME,	(V
	CTRL_CHANNEL,
	CTRL_CHANNEL_UPDOWN,
	//CTRL_SOURCE,	(SOURCE_AV SOURCE_TV _DVD)
	//CTRL_PLAY,		(PALY/PAUSE/STOP)
	CTRL_TV_MODE,
	CTRL_FUNCTION_KEY
}CTRL_TYPE;


// to chang depending on the table "t_keyfunction" in db
typedef enum
{
	CTRL_ON = 1,
	CTRL_OFF,
}CTRL_TYPE_IN_DB;

//Deny host service response code
typedef enum
{
    DENY_SUCCESS = 0x00,	//stop host network communication success
    DENY_FAILED,			//stop host network communication failed
    NO_PERMISSION			//no permission to stop host network communication
} DENY_HOST_RES_CODE;

//update configration modes
typedef enum
{
    NO_UPDATE = 0x00,		//no need to update
	MUST_UPDATE,            //must to update
	RECOMMAND_UPDATE		//recommand to update
//    DATA_UPDATE,			//need to update data
//    DATABASE_UPDATE		//need to update database structure
} UPDATE_CONFIG_MODE;

//client types
typedef enum
{
    HOST_C = 0x00,			//host
    IPHONE = 0x10,			//iphone
    IPAD = 0x11,			//ipad
    ANDROID = 0x20			//android
} CLIENT_TYPES;

//Login status
typedef enum
{
    LOGIN_SUCCESS = 0x00,	//login success
    LOGIN_FAILED,			//login failed
    LOGIN_RELOG
} LOGIN_STATUS;

//Logout status
typedef enum
{
	LOGOUT_SUCCESS = 0x00,	//login success
	LOGOUT_FAILED			//login failed
} LOGOUT_STATUS;

//Time struct
typedef struct _time_data
{
    u_short	year;
    u_char	month;
    u_char	day;
    u_char	hour;
    u_char	minute;
    u_char	second;
    u_char	weekday;
}time_data_t;

//Data area
typedef union _data_area
{
    double		ana_data;	//analogic value
    int 		bin_data;	//binary value
    char		txt_data[16];	//string value
    time_data_t	time_data;	//date or time value
}data_area_t;

//Group Data
typedef struct _group_data
{ 
	int16_t     group_addr;
	u_char		grp_data_type;
    data_area_t	data;
}group_data_t;


// Common header 
typedef struct _remote_header
{
    u_char	start;          //mark data gram start гн 0x7F
    u_char	protocol_type;	//protocol type
    int16_t	length;			//data gram length
    int16_t	serial_number;	//version
    u_char	function_code;	//function code
    u_char	comm_type;		//communication type, request/response/error
    u_char	comm_source;	//communication source
    int16_t	proto_ver;		//protocol version
	u_char	space;		//space alignment
} remote_header_t;

//Heart beat request/response
typedef struct _heartbeat_req
{
	remote_header_t header;	//data gram header
	int32_t	session_id;		//session id
} heartbeat_req_t;

typedef struct _heartbeat_res
{
	remote_header_t header;	//data gram header
    u_char	message;		//notice client whether the IP is changed.
    						//0-no changed; 1-changed.
    int32_t	session_id;		//session id
    u_char	space[3];		//space alignment
} heartbeat_res_t;

// response secret key from server
typedef struct _ask_secret_req
{
	remote_header_t header;	//data gram header
}ask_secret_req_t;

typedef struct _ask_secret_res
{
	remote_header_t header;	//data gram header
	u_char	secret_key[8];	//secret key
	u_char	secret_mode[256];//secret mode
}ask_secret_res_t;

typedef struct _pass_secret_req
{
	remote_header_t header;	//data gram header
	u_char	secret_key[128];//secret key, encrypted by 1024-RSA
}pass_secret_req_t;

typedef struct _pass_secret_res
{
	remote_header_t header;	//data gram header
	int32_t	session_id;		//session id
}pass_secret_res_t;

// Login message
typedef struct _login_req
{
	remote_header_t	header;	//data gram header
	int32_t	session_id;	//session id
	u_char	username_len;	//length of user name
    u_char	user_name[32];	//user name, encrypted by 1024-RSA
    u_char	password_len;	//length of password
    u_char	password[32];	//password, encrypted by 1024-RSA
    u_char	space[2];		//space alignment
} login_req_t;

typedef struct _login_res
{
	remote_header_t	header;	//data gram header
    int32_t	session_id;		//session id
    int32_t peer_ip;        //peer ip
    int32_t peer_port;      //peer port
    u_char	status;			//login status
    u_char	space[3];		//space alignment
} login_res_t;

// Update configuration
#pragma pack(4)
typedef struct _update_req
{
	remote_header_t	header;		//data gram header
	int32_t	session_id;			//session id
    unsigned long long latest_time;		//the latest update time by second
	u_char	query_from;			//space alignment
	u_char	product_id[64];		// product identify
} update_req_t;

typedef struct _update_res
{
	remote_header_t header;		//data gram header
	int32_t	session_id;			//session id
	u_char	query_from;		//query source: such as 0x01 for host, 0x02 for host_web_log
    u_char	conf_upd_mode;		//configure update mode, such as must update, recommend update
    u_char	conf_url_len;		//the length of down load URL of configure
    u_char	conf_url[128];		//configure URL body
    u_char	file_upd_mode;		//file update mode, such as must update, recommend update
    u_char	file_url_len;		//the length of down load URL of file
    u_char	file_url[256];		//file URL body
	u_char	md5[48];			//md5 to the file
} update_res_t;

// Update issue
typedef struct _update_issue_req
{
	remote_header_t	header;	//data gram header
	int32_t	session_id;		//session id
} update_issue_req_t;

/* not used any more
typedef struct _update_issue_res
{
	remote_header_t header;	//data gram header
	int32_t	session_id;		//session id
} update_issue_res_t;
*/

// p2p
typedef struct _p2p_connect_req
{
	remote_header_t header;	//data gram header
	int32_t	session_id;		//session id
} p2p_connect_req_t;

typedef struct _p2p_connect_res
{
	remote_header_t header;	//data gram header
	int32_t	self_session_id;//self session id
	int32_t	terminal_session_id;//terminal session id
    u_char	ip_type;		//IP type, IPV4/IPV6
    u_char	ip[16];			//IP address
    int16_t	port;			//port
    u_char	space;			//space alignment
} p2p_connect_res_t;

//KNX/Wifi/Ifrared/Com Device control Head
typedef struct _kic_head
{
	remote_header_t	header;	//data gram header
	int32_t	session_id;		//session id
	u_char	device_type;	//device type
	int32_t	home_device_id;//device unique id at home, report id in ir_study_req_t
} kic_head_t;

//KNX Device control message, req and res are same message. 
typedef struct _knx_control_report
{
	kic_head_t kic_head;	//KNX/Wifi/Ifrared/Com Device control Head
    u_char	cmd_type;		//KNX command type, READ/WRITE
    int16_t  address;		//KNX Group address
    u_char	data[16];		//group data value
    u_char	result_code;	//result code
    u_char	data_len;		//data length
    u_char	data_type;		//group data point type, 17 types
    u_char	space;			//space alignment
} knx_control_report_t;

//WIFI Device control  request/response message
//IR Study request/response message
typedef struct _wifi_control_req
{
	kic_head_t kic_head;	//KNX/Wifi/Ifrared Device control Head
    int16_t	function_code;	//WIFI/INFRARED; IR study-keynum.
    u_char	space[2];			//space alignment
} wifi_control_req_t;

typedef struct _wifi_control_res
{
	kic_head_t kic_head;	//KNX/Wifi/Ifrared/Com Device control Head
    int16_t	function_code;	//WIFI/INFRARED/COM-FUNCTION_CODE; IR study-keynum.
    u_char	res_code;		//error code , 0-success, 1-failed.
    u_char	space;			//space alignment
} wifi_control_res_t;

//IR Study request/response message
typedef struct _ir_study_req
{
	kic_head_t kic_head;
    int32_t  gateway_id;	//gateway id
    u_char	channel;		//INFRARED channel
    int16_t	function_code;	//IR study-keynum.
    u_char	space;			//space alignment
} ir_study_req_t;

typedef struct _ir_study_res
{
    kic_head_t kic_head;
    int32_t  gateway_id;	//gateway id
    u_char	channel;		//INFRARED channel
    int16_t	function_code;	//IR study-keynum.
    u_char	res_code;		//error code , 0-success, 1-failed.
} ir_study_res_t;

//COM Device control message
typedef struct _com_control_req
{
	kic_head_t kic_head;	//Com Device control Head
    u_char	com_ctrl_type;	//COM control type
	u_char	res_code;		//error code , 0-success, 1-failed.
    int32_t	data;			//data
} com_control_req_t;

typedef struct _com_control_res
{
	kic_head_t kic_head;	//Com Device control Head
	u_char	com_ctrl_type;	//COM control type
	u_char	res_code;		//error code , 0-success, 1-failed.
	int32_t	data;			//data
} com_control_res_t;

//scene control request & response message
typedef struct _scene_control_report
{
	remote_header_t header; //data gram header
	int32_t	session_id;		//session id
	u_char	device_type;	//device type
	int32_t	scene;			//device unique id at home
    u_char	space[3];		//space alignment
} scene_control_report_t;

//Serial port initiative response report
typedef struct _serial_initiative_res
{
	kic_head_t kic_head;	//KNX/Wifi/Ifrared/Com Device control Head
	u_char	response_type;	//?
	u_char	data_len;		//data length.
	u_char	space[2];			// format for 4 bytes
	char  data[0];			//data
} serial_initiative_res_t;

typedef struct _device_control_req
{
	remote_header_t header; 	// data gram header
	int32_t session_id;			// session id
	int32_t datapoint_id;		// data point id
	unsigned char read_write;	// READ/WRITE/AUTOBACK
	unsigned char data_len;		// length of data
	unsigned char data[0];		// data body
}device_control_req;

typedef struct _device_control_res
{
	remote_header_t header; 	// data gram header
	int32_t session_id;
	int32_t datapoint_id;
	unsigned char read_write;
	unsigned char res_code;
	unsigned char data_len;
	unsigned char data[0];
}device_control_res;

typedef struct _device_state_req
{
	remote_header_t header; 	// data gram header
	int32_t session_id;
	int32_t home_device_id;
}device_state_req;

typedef struct _device_state_res
{
	remote_header_t header; 	// data gram header
	int32_t session_id;
	int32_t home_device_id;
	unsigned char res_code;
	unsigned char data_len;
	unsigned char data[0];
}device_state_res;




//Host deny connect message, receive from server
typedef struct _host_deny_connect_req
{
	remote_header_t header;	//data gram header
	int32_t	session_id;	//session id
	int32_t report_id;
} host_deny_connect_req_t;

typedef struct _host_deny_connect_res //handle in host
{
	remote_header_t header;	//data gram header
	int32_t	session_id;		//session id
	int32_t report_id;
    u_char	res_code;		//return host status or error code,
    						//see DENY_HOST_RES_CODE
    u_char	space[3];		//space alignment
} host_deny_connect_res_t;

typedef struct _stop_host_connect_req
{
	remote_header_t header;	//data gram header
	int32_t	session_id;		//session id
	u_char	check_code[16];	//"tiansu"
} stop_host_connect_req_t;

typedef struct _stop_host_connect_res //handle in host
{
	remote_header_t header;	//data gram header
	int32_t	session_id;		//session id
    u_char	res_code;		//return host status or error code,
    						//see DENY_HOST_RES_CODE
    u_char	space[3];		//space alignment
} stop_host_connect_res_t;

//Log out from server
typedef struct _logout_req
{
	remote_header_t	header;	//data gram header
	int32_t	session_id;		//session id
} logout_req_t;

typedef struct _logout_res
{
	remote_header_t	header;	//data gram header
	int32_t	session_id;		//session id
	u_char	res_code;		//return status or error code
    u_char	space[3];		//space alignment
} logout_res_t;

//server pass the terminal session id to host
typedef struct _pass_term_session_req
{
	remote_header_t header;	//data gram header
	int32_t	session_id;		//host session id
	int32_t	term_session_id;	//terminal session id
	u_char	user_name[32];	//user name
} pass_term_session_req_t;

typedef struct _pass_term_session_res
{
	remote_header_t header;	//data gram header
	int32_t	session_id;		//host session id
	int32_t	term_session_id;	//terminal session id
} pass_term_session_res_t;


typedef enum
{
	HADNOT_LOGIN			//had not login
} REMOTE_ERROR_CODE;

typedef struct _global_error
{
	remote_header_t	header;	//data gram header
	int32_t	session_id;		//session id
	int16_t	error_code;		//error code
	u_char	space[2];		//space alignment
}global_error_t;

#endif
