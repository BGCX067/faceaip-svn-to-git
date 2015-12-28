/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CmdsTypedef.h - _explain_
 *
 * Copyright (C) 2005 QiYang Technologies, All Rights Reserved.
 *
 * $Id: CmdsTypedef.h, v 1.0.0.1 2005-03-18 14:23:45 wjj Exp $
 *
 * _detail_explain_.
 * 
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _CMDSTYPEDEF_H
#define _CMDSTYPEDEF_H

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "../System/BaseTypedef.h"
#include "../Include/config-x.h"

#define STR_CFG "config"
#define STR_VDP "vdpara"
#define STR_TIM "timing"

#define ENCRYPT_DES	1
// des密码
#define ENCRYPT_DES_KEY "shouldtodo"
#define ENCRYPT_DES_KEY_MOBILE "mobileview"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  处理状态  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/* 事务处理状态 */
enum    EnTxStatus
{
    TxStInit  =  0,
    TxStWait  =  1,
    TxStNext  =  2,
    TxStTimeOut  =  3,
    TxStEnd   = -100,
};

/* 操作处理状态（预定的）*/
enum    EnOpStatus
{
    OpStBegin =  101,
    OpStBeginDataSafe,
    /* 验证通过后做事 */
    OpStEndDataSafe,
    /* 检查参数等是否合法 */
    OpStCheckValid,
    /* 将这个弄的特殊些 */
    OpStEnd = -1,
};

/*
 * 自定义操作过程状态
 * (建议1-100， 不要弄成 0 之类的特殊值)
 */
enum EnLoginOpStatus
{
    OpStLoginDo = 1,
};

enum EnNetWorkMonitorStatus
{
    /* 视频切换 */
    OpStChanSwtch = 1,
    /* 多画面预览 */
    OpStMMonitor,
};

enum EnCmdsRecDownloadStatus
{
    OpStRecordFile = 1,
};

enum EnOnAudioDataStatus
{
    /* 视频切换 */
    OpStOnAudioData = 1,
};

//
enum EnDefaultOperationOpStatus
{
    OpStDefaultOperationDo = 1,
};

//
enum EnSendData
{
    OpStSendBackData = 1,
};

enum EnInfoProcStatus
{
    OpStInfoProcDo = 1,
};

#ifdef WTN
#	define DEFAULT_PREVIEW_QUALITY 6
#else
#	define DEFAULT_PREVIEW_QUALITY 6
#endif
#define DEFAULT_PREVIEW_FRAME 4
#define DEFAULT_PREVIEW_SIZE 3

enum EnSendPacket
{
    OpStSendMonitorPacket = 1,
    OpStSendMonitorPacketIFrameOnly,
    OpStSendMonitorPacketAdapter,
    OpStSendRecordPacket,
    OpStSendAudioPacket,
    OpStSendSnapPacket,
    OpStSendMuticastPacket,
};
//
enum EnMonitorSendMode
{
    EnMonitorSendMode_Packet = OpStSendMonitorPacket,
    EnMonitorSendMode_IFrameOnly,
};

//
enum EnSysUpdateOpStatus
{
    OpStSysUpdate = 1,
};

enum EnUserManagementStatus
{
    OpStQueryAuth = 1,

    OpStAddGroup	,
    OpStDelGroup	,
    OpStModGroup	,
    OpStQueryGroup	,

    OpStAddUser		,
    OpStDelUser		,
    OpStModUser		,
    OpStQueryUser	,
    OpStModPwd		,
};

//
// 透明串口操作状态
//
enum EnShareComStatus
{
    OpStSetCom = 1		,
    OpStSendData2Com	,
    OpStSendBackComData	,
    OpStBookExtIO,
    OpStQueryExtIO,
    OpStSetExtIO,
};

enum EnNetKeyBoardStatus
{
    OpStParseData = 1,
};
//0x60设备控制
enum EnDeviceCtrl
{
    OpStReboot = 1,
    OpStShutdown,
    OpStHdd,
    OpStTestDevice=5,
    OpStSd =7,
    /* Begin:add by Nirenzhuo(10770) for task.NO.N/A 20071029 */
    OpSpringSnap=8,
    /* End:add by Nirenzhuo(10770) for task.NO.N/A */
    OpDelLog = 9,//日志清除
    OpStRecall = 10,	//恢复设备的默认设置　2007-11-28
};

//
// 主动发送报警数据
//
enum EnAlarmToCenter
{
    OpStAlarmToCenterConn = 1	,
    OpStAlarmToCenterSend	,
};
//报警操作
enum EnAlarmOpr
{
    OpStQueryAlarmInfo = 1,        //查询
    OpStBookAlarmInfo     ,        //订阅
    OpStCanselAlarmInfo   ,        //取消
};
//报警图片操作
enum EnAlarmOprPic
{
    OpStQueryAlarmPic = 1,        //查询
    OpStBookAlarmPic     ,        //订阅
    OpStCanselAlarmPic   ,        //取消
};

typedef int        TOpState;       //操作处理状态

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

enum EnDVRCommand
{
    //请求
    REQ_UPDATE_LOGIN= 0x10,			//升级客户端登录
    REQ_CHAN_SWTCH	= 0x11,			//视频切换
    REQ_CTRL_PANTZ	= 0x12,			//云台控制, 预置点设置, 灯光控制
    REQ_MODE_FOCUS	= 0x13,
    REQ_CTRL_FOCUS	= 0x14,
    REQ_CTRL_ZOOM	= 0x15,
    REQ_CTRL_IMAGE	= 0x16,
    REQ_CTRL_MONSW	= 0x17,			//画面风格(分割)切换
    REQ_CTRL_IRIS	= 0x18,
    REQ_CTRL_PRESET	= 0x19,
    REQ_CTRL_ALARM	= 0x1a,
    REQ_CTRL_LAMP	= 0x1b,
    REQ_CTRL_MENU	= 0x1c,
    REQ_SEND_AUDIO	= 0x1d,			//要求发送音频
    REQ_MODE_XCHGE	= 0x1e,			//设置传输方式
    REQ_SEND_TXMOD	= 0x1f,			//发送传输方式
    REQ_COMM_SEND	= 0x20,			//透明串口传送
    REQ_CTRL_RESET	= 0x21,
    REQ_DISK_PARAM	= 0x22,			//要求传送硬盘状态信息
    REQ_ALARM_INFO	= 0x23,			//请求报警输入输出状态
    REQ_SET_DTIME	= 0x24,			//DVR时间设置

    REQ_NET_KEYBOARD_CTRL = 0x98,   /*!< 新的网络键盘控制 */
    REQ_BOARD_CTRL = 0x99,			//键盘控制

    REQ_USER_LOGIN	= 0xa0,
    REQ_STAT_QUERY	= 0xa1,			//a1 请求状态查询
    REQ_LOGS_PARAM	= 0xa2,			//请求日志信息
    REQ_CONF_PARAM	= 0xa3,			//请求系统配置
    REQ_INFO_SYSTM	= 0xa4,			//请求系统信息
    REQ_RECD_QUERY	= 0xa5,			//查询录像文件
    REQ_USER_PARAM	= 0xa6,			//查询用户信息
    REQ_FILE_ALARM	= 0xa7,			//查询报警文件
    REQ_CHAN_TITLE	= 0xa8,			//请求发送汉字
    REQ_CTRL_SWTCH	= 0xa9,			//请求改变控制权

    REQ_AUDIO_DATA	= 0xc0,			//对讲数据
    ACK_REQ_AUDIO_DATA	= 0x1d,			//对讲数据

    REQ_CONF_UPDAT	= 0xc1,			//请求配置更新
    REQ_RECD_PLAY	= 0xc2,			//录像文件回放
    REQ_PLAY_ALERT	= 0xc3,			//播放报警文件
    REQ_USER_UPDAT	= 0xc4,			//请求用户更新
    REQ_CTRL_RECRD	= 0xc5,			//请求录像控制
    REQ_CHAN_UPDAT	= 0xc6,			//请求通道标题
    REQ_BIOS_UPDAT	= 0xc7,			//刷新BIOS
    REQ_FONT_UPDAT	= 0xc8,			//请求字库更新
    REQ_PLAY_STOP	= 0xc9,			//停止播放文件
    REQ_CTRL_PLAY	= 0xca,			//录像文件时间控制
    REQ_RECD_DOWN	= 0xcb,			//录像文件下载
    REQ_PLAY_PAUSE	= 0xcc,			//录像回放暂停
    REQ_PROT_TITLE	= 0xcd,			//请求协议标题

    REQ_CHAN_COLOR	= 0xcf,			//请求通道颜色

    REQ_MODIFY_MAC = 0xd0,			//请求修改Mac地址
    ACK_MODIFY_MAC = 0xd0,			//应答修改Mac地址

    REQ_DEVICE_CTRL    = 0x60,              //请求设备控制,added,2005/11/2,wangk
    ACK_DEVICE_CTRL    = 0x60,              //应答设备控制请求,added,2005/11/2,wangk
    ACK_SEND_MESSAGE   = 0x66,              //发送录像消息,added,2006/04/12,wangk

    REQ_TEST_DEVICE	= 0x62,			//测试设备, 第9字节为测试类型, 见EnTestDeviceType
    REQ_INFO_PROC   = 0x61,			//信息处理, 把来自POS机的商品信息叠加到视频上
    ACK_INFO_PROC   = 0x61,			//信息处理, 把来自POS机的商品信息叠加到视频上

    REQ_DEVICE_QUERY= 0x82,         //设备轮巡
    ACK_DEVICE_QUERY= 0x82,         //返回设备状态或通道状态

    REQ_USER_FORCEIFRAME = 0x80,    //强制I-FRAME的功能
    ACK_USER_FORCEIFRAME = 0x80,
    REQ_USER_SETNETRATE  = 0x81,    //设置网络流量
    ACK_USER_SETNETRATE  = 0x81,

    ACK_INFO_NORMAL      = 0x64,    //普通应答消息
    REQ_SYSTEM_OPTQUERY  = 0x83,    //查询通道可选项配置
    ACK_SYSTEM_OPTQUERY  = 0x83,    //应答通道可选项配置

    REQ_ALARM_STAT  = 0x68,         //报警信息请求
    ACK_ALARM_STAT  = 0x69,         //报警信息应答

    //应答
    ACK_USER_LOGIN	= 0xb0,			//应答用户登陆
    ACK_STAT_QUERY	= 0xb1,			//应答状态查询
    ACK_LOGS_PARAM	= 0xb2,
    ACK_CONF_PARAM	= 0xb3,			//应答系统配置
    ACK_INFO_SYSTM	= 0xb4,			//应答系统信息
    ACK_USER_PARAM	= 0xb5,			//应答用户配置
    ACK_FILE_RECRD	= 0xb6,
    ACK_FILE_ALARM	= 0xb7,			//应答报警文件
    ACK_CHAN_TITLE	= 0xb8,			//应答通道标题
    ACK_FONT_UPDAT	= 0xb9,			//应答字库更新
    ACK_BIOS_UPDAT	= 0xba,
    ACK_RECD_PLAY	= 0xbb,			//发送下载和回放结束命令字
    ACK_CHAN_MONIT	= 0xbc,			//应答通道监视
    ACK_SOCK_TEST	= 0xbd,			//用于测试
    ACK_CTRL_SWTCH	= 0xbe,			//更改控制权
    ACK_CTRL_ROBED	= 0xbf,			//强行获得控制权
    ACK_SEND_AUDIO	= 0xc0,			//应答发送音频
    ACK_CHAN_COLOR	= 0xCF,			//返回通道颜色
    ACK_SEND_TXMOD	= 0xe0,			//传送方式
    ACK_DISK_PARAM	= 0xe1,			//应答传送硬盘状态信息
    ACK_FILE_PARAM	= 0xe2,			//发送文件信息
    ACK_STAT_ALARM	= 0xe3,			//报警布防状态
    ACK_PROT_TITLE	= 0xe4,			//应答协议标题

    ACK_USER_UPDAT	= 0xe5,

    ACK_ERROR_RET	= 0xee,			// 应答一般错误消息

    ACK_USER_STATE	= 0xf0,			//应答用户状态查询

    REQ_SUB_CONN      = 0Xf1,                //子连接请求
    ACK_CHAN_MONIT_EX = 0xf2,//多画面预览，用于内部区分
    REQ_EXT_LOGIN      = 0Xf6,                //
    ACK_EXT_LOGIN      = 0Xf6,                //
    REQ_TEST_ONLY	= 0xfe,			//测试用
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    /*-+-+-+-+-+-+-+-+         模拟DVR用         +-+-+-+-+-+-+-+-+-+-+-+-*/
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    SIMULATE_FILE_DOWNLOAD,			// 文件下载，模拟DVR专用

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    /*-+-+-+-+-+-+-+-+         内部命令         -+-+-+-+-+-+-+-+-+-+-+-+-*/
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    INTERNAL_DEFAULT_OPERATION	= 0xff01,	// 默认操作类
    INTERNAL_RECORD_DOWNLOAD			,	// 记录数据下载（回放&数据下载）
    INTERNAL_CONNECTION_BROKEN			,	// 连接断开处理
    INTERNAL_SYSTEM_END_TASK			,	//线程结束的任务

    INTERNAL_ALARM_TO_CENTER_SDK		,	//主动上传报警信息到sdk
    /*-+-+-+-+-+-+-+-   以下命令为发送数据命令   +-+-+-+-+-+-+-+-+-+-+-+-*/
    /* 下面不能添加其他命令，顺序也不能变.
     *		原因参见: CNetWorkService::OnCommand
     */
    INTERNAL_SEND_PACKET		= 0xffa3,	// 发送Packet数据(视频)
    INTERNAL_SEND_DATA					,	// 发送数据
#ifdef XJSX_MODULE
    REQ_REGISTER_XJSX = 0x90,			//注册请求
    ACK_REGISTER_XJSX = 0x91,			//注册响应
    REQ_KEEP_ALIVE_XJSX =0x92,			//保活请求
    ACK_KEEP_ALIVE_XJSX = 0x93,			//保活响应
#endif
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
};

enum EnFrontKey
{
    NKEY_POWER = 0,
    NKEY_ENTER,
    NKEY_ESC,
    NKEY_UP,
    NKEY_DOWN,
    NKEY_LEFT,
    NKEY_RIGHT,
    NKEY_0,
    NKEY_1,
    NKEY_2,
    NKEY_3,
    NKEY_4,
    NKEY_5,
    NKEY_6,
    NKEY_7,
    NKEY_8,
    NKEY_9,
    NKEY_10,
    NKEY_11,
    NKEY_12,
    NKEY_13,
    NKEY_14,
    NKEY_15,
    NKEY_16,
    NKEY_SPLT,
    NKEY_DIV1,
    NKEY_DIV9,
    NKEY_ADDR,
    NKEY_INFO,
    NKEY_REC,
    NKEY_FN1,
    NKEY_FN2,
    NKEY_PLAY,
    NKEY_STOP,
    NKEY_SLOW,
    NKEY_FAST,
    NKEY_PREV,
    NKEY_NEXT,
    NKEY_JMPUP,
    NKEY_JMPDN,
    NKEY_10PLUS,
    NKEY_SHIFT,
    NKEY_BACK,
};

//
// 查询系统参数信息
// 注意顺序不能改变
//
typedef enum __cfg_index_t {
    CFG_GENERAL =  0,	// 普通
    CFG_COMM   		,	// 串口
    CFG_NET    		,	// 网络
    CFG_RECORD 		,	// 录像		/*!< 2.42以后废弃不用，使用CFG_NEWRECORD */
    CFG_CAPTURE		,	// 图像设置	/*!< 2.42以后废弃不用，使用CFG_CURRENTCAPTURE */
    CFG_PTZ    		,	// 云台
    CFG_DETECT 		,	// 动态检测	/*!< 2.42以后废弃不用，使用CFG_EVENT */
    CFG_ALARM  		,	// 报警		/*!< 2.42以后废弃不用，使用CFG_EVENT */
    CFG_DISPLAY		,	// 显示
    CFG_TITLE  		= 10,	   // 通道标题
    CFG_MAIL  		= 11,	   // 邮件功能
    CFG_EXCAPTURE	= 12,	   // 预览图像设置
    CFG_PPPOE	    = 13,	   // pppoe设置
    CFG_NET_ARP	    = 14,	   // ARP设置
    CFG_SNIFFER     = 15,	   // 网络监视捕获设置
    CFG_DSPINFO     = 16,      // 编码能力信息
    CFG_VIDEOCOVER = 34,	//区域遮挡
    CFG_FTP         = 37,//17,       //FTP配置
    CFG_AUTOMAINTAIN= 18,      //自动维护
    CFG_NET_NTP=19, /*ntp配置-- ok 2010-12-6 11:51:38*/
    CFG_DSPBITRATE   = 24,    // DSP最大码流
    CFG_DHCP  = 35,
    CFG_WEB          = 36,
    CFG_WLANIP	= 39,	//无线模块IP配置
    CFG_DNS = 40,
    CFG_STORAGE = 41,//存储位置
#if defined(CAM_IPC) || defined(CAM_A6)
    CFG_CAM_CONTROL	 = 38,
#endif
    CFG_RECDDOWNLOAD = 42,
    CFG_AUDIOFORMAT	 = 43,		//音频编码格式
#ifdef DVR_PTZ
	CFG_PTZFUNC =44, 	//borui ptz functions
	CFG_PTZFPRESET = 45,
	CFG_PTZFTOUR =46,
	CFG_PTZFPATTERN =47,
#endif
    CFG_NEWRECORD	= 123,		/*!< 新的录像设置 */
    CFG_EVENT		= 124,	   //事件配置
    CFG_WORKSHEET   = 125,     //时间表配置信息
    CFG_COLOR       = 126,     // 颜色配置信息
    CFG_CURRENTCAPTURE = 127,  // 当前最新配置,为增加双码流功能后增加的
    CFG_NEWCAPTURE     = 128,  // 采用新的图像配置结构
    CFG_WLAN	= 131,	//wifi config
    CFG_TRANSFER_POLICY	=	133,//网络传输策略配置
    CFG_WI_ALARM  = 134,  //查询无线配置
    CFG_WLANSCAN = 135, //wifi scanning
    CFG_NEWDDNS = 140,		//新的ddns
	CFG_MOUSE = 141,//鼠标使用习惯配置
    CFG_VSP            = 190,   //第三方介入配置
    CFG_CAMIPC         = 200,   //CAMIPC配置
    CFG_LOCATION	   = 201,	/*!< 区域配置 */
    CFG_GUISET		   = 202,	/*!< GUI配置 */
    CFG_RECWORKSHEET   = 203, 	/*!< 录像工作表配置 */
    CFG_PTZALARM	   = 204,	/*!< 云台报警设备协议配置 */
    CFG_NETALARM	   = 205,	/*!< 网络报警 */
    CFG_ALMWORKSHEET   = 206,	/*!< 报警工作表配置 */
    CFG_MTDWORKSHEET   = 207,	/*!< 动检工作表配置 */
    CFG_PRESETNAME	   = 208,	/*!< 云台预知点名 */
    CFG_PTZTOUR		   = 209,	/*!< 巡航配置 */
    CFG_LOSS		   = 210,	/*!< 视频丢失 */
    CFG_BLIND		   = 211,   /*!< 视频遮挡 */
    CFG_PLAY		   = 212,	/*!< 回放配置 */
    CFG_USER		   = 213,	/*!< 用户配置 */
    CFG_GROUP		   = 214,	/*!< 用户组配置 */
    CFG_MONITORTOUR	   = 215,	/*!< 监视轮训 */
    CFG_TVADJUST	   = 216,	/*!< TV调节 */
    CFG_ATM	   		   = 217,	/*!< ATM配置 */
    CFG_STNOTEXIST	   = 218,	/*!< 无硬盘 */
    CFG_STFAILURE	   = 219,	/*!< 硬盘出错 */
    CFG_STLOWSPACE	   = 220,	/*!< 硬盘空间不足 */
    CFG_NETABORT	   = 221,	/*!< 断网事件配置 */
    CFG_VIDEOWIDGET	   = 222,	/*!< 视频装饰 */
    CFG_VIDEOMATRIX	   = 223,	/*!< 视频矩阵 */
#ifdef LOG_SUPPORT_CDJF
    CFG_SYSTEMTIME = 224,	//系统时间
#endif//LOG_SUPPORT_CDJF
#ifdef CAM_A6
    CFG_INFRAREDALARMWORKSHEET = 225,//无线报警工作表
#endif
    CFG_NET_COMMON = 230, /*通用网络配置*/
    CFG_NET_FTPSERVER , /*FTP服务器配置*/
    CFG_NET_FTPAPPLICATION , /*ftp应用配置*/
    CFG_NET_IPFILTER , /*IP过滤配置*/
    CFG_NET_MULTICAST , /*组播配置*/
    CFG_NET_PPPOE , /*pppoe配置*/
#ifdef FUNC_DDNS_DYNDNS
    CFG_NET_DDNS , /*ddns配置*/
#endif
    CFG_NET_ALARMSERVER , /*报警中心配置*/
    CFG_NET_VLAN = 238,
    CFG_NET_EMAIL , /*email配置*/
    CFG_NET_SNIFFER , /*抓包配置*/
#ifdef CDJF
    CFG_NET_CDJF,/*成都佳发的配置*/
    CFG_ALARM_CDJF,/*成都佳发的报警通道名称*/
#endif

#if defined(TRANS_FILE_CFG)
    CFG_TRANS_FILE,
#endif

#if defined(XINGWANG_CFG)
    CFG_NET_XINGWANG,
#endif
#if defined(NET_MEYE_CFG)
    CFG_NET_MEYE,
#endif

    CFG_NET_DHCP_INT,

#if defined(COM_SUPPORT_DAGUANGMING)
    CFG_DAGUANGMING_LASTUPFILE ,
#endif

#if defined(_FUNC_UPNP_MOUDLE)
    CFG_NET_UPNP,
#endif

    CFG_RECState = 245, /*报警动检录像状态*/
    /* Begin:Added by ChenChengBin 10782, 2008/2/21 PN: ShangHai Bell Alcatel */
#ifdef SHBELL
    CFG_NET_SHBELL,/* 上海BELL电信的配置信息 */
    CFG_NET_SHBELL_PRESET,/* 上海BELL电信的预置点配置信息 */
    CFG_NET_SHBELL_TOUR,/* 上海BELL电信的巡航配置信息 */
    CFG_CONFIG_PTZREGRESS, /* 预置点回归信息 */
    /* Begin:Added by ChenChengBin, 2008/10/28 PN: BELL_PICSHOOT */
#ifdef __BELL_PICSHOOT__
    CFG_CONFIG_UPLOAD_PICTURE, /* 报警图片上传配置信息 */
#endif
    /* End:Added by ChenChengBin, 2008/10/28 PN: BELL_PICSHOOT */
#endif
    /* End:Added by ChenChengBin 10782, 2008/2/21 PN: ShangHai Bell Alcatel */

    /* Begin:Added by ChenChengBin, 2008/7/28 PN: HZHS */
#ifdef HZHS
    CFG_NET_HZHS, /* 杭州华数接入配置信息 */
    CFG_MOTIONZONE_HZHS,
#endif
    /* End:Added by ChenChengBin, 2008/7/28 PN: HZHS */

#ifdef FUNC_IVIEWER_GLOBAL
    CFG_IVIEWER_GLOBAL,
#if defined(SZHW_TR069)
    CFG_NSS_TR069,
#endif
#endif
#ifdef SUPPORT_3G_MODULE 
	CFG_3G_MODULE,
#endif
#ifdef OEM_HANGZHENG
	CFG_HANGZHENG_PLATE,
#endif

#ifdef OEM_BORUI_A3
	CFG_BORUI_A3_GSM,//博睿A3定制,GSM配置信息
#endif

#if defined(MEGAEYES_MODULE)
    CFG_NET_MEGA,
    CFG_MEGA_FTP,
    CFG_MEGA_MOTION_ZONE,
    CFG_NET_MEGAEYES_PRESET,/* 互信互通的预置点配置信息 */
    CFG_NET_MEGAEYES_TOUR,/* 互信互通的巡航配置信息 */
#endif

#if defined(SKD_MODULE) || defined(KEDA_MODULE)
    CFG_NET_KEDA , /*第三方接入keda配置*/
    CFG_NET_KEDA_MARTIX,
    CFG_NET_KEDA_MOTION_KEDA,
    CFG_NET_KEDA_TIMELYREC_INFO,
#endif

#ifdef ZTE_NETVIEW
    CFG_NET_NETVIEW,/*中兴力维接入的配置*/
    CFG_OSD_NETVIEW,/*中兴力维附加OSD的配置*/
    CFG_NET_NETVIEW_PRESET,/*中兴力维的预置点配置信息 */
    CFG_NET_NETVIEW_TOUR,/* 中兴力维的巡航配置信息 */
#if defined(_NETVIEW_EXTEND_)
    CFG_ELSE_NETVIEW,
#endif
	#if defined(CNC_SJZ)
    CFG_PPWORKSHEET,
#endif
#endif
#if defined(H3C)
    CFG_NET_H3C , /*第三方接入keda配置*/
#endif
#if defined(LTM)
    CFG_NET_LTM , /*第三方接入LTM配置*/
#endif
#if defined(_ZTE_APP_)
    CFG_NET_ZTENJ,		/*第三方接入中兴南研所配置*/
    CFG_NET_ZTENJ_TR069,
    CFG_ZTE_ALARMIN,
    CFG_ZTE_ALARMOUT,
    CFG_ZTE_CHANNEL,
    CFG_BLDWORKSHEET,
#if defined(_WANG_TEST_)
    CFG_ELSE_ZTENJ,
#endif
#endif

#ifdef _USE_ZJGX_MODULE
    CFG_NET_ZJGX,//浙江公信
#endif
#ifdef XTREAM
    CFG_NET_BJXF,
    CFG_NET_BJXF_PRESET,
    CFG_NET_BJXF_TOUR,
#endif
#ifdef _XWZX_MODULE
    CFG_NET_XWZX,
    CFG_GENERAL_XWZX,
#endif
#ifdef _ZJGX300_
    CFG_NET_ZJGX300,
    CFG_NET_ZJGX300_PRESET,/*预置点配置信息 */
    CFG_NET_ZJGX300_TOUR,/* 巡航配置信息 */
#endif
#ifdef XJSX_MODULE
    CFG_NET_XJSX,
#endif

#ifdef _BeijingSX_MODULE
    CFG_NET_BJSX,
    CFG_NET_BJSX_SYSTEM,
#endif

#if defined(_USE_WJJH_MODULE)
    CFG_NET_WJJH,
    CFG_SNMP_WJJH,
    CFG_NET_WJJH_PRESET,/*预置点配置信息 */
    CFG_NET_WJJH_TOUR,/* 巡航配置信息 */
#endif
    CFG_REG_SERVER = 241,/*主动注册服务器配置*/
    CFG_INFO_VERSION =242,//定制型号信息配置    addby guoq 2011.2.24
    CFG_BRA8  = 243,//博瑞A8 的特殊配置
    CFG_ANVISI  = 244,//安维思的特殊配置

}CFG_INDEX;
typedef enum _wiAlarm_type_
{
    WIALARM_OUT = 1,
    WIALARM_REMOTE =2
};

//Begin yuxy_070418,暂时放着
typedef enum _event_type_
{
    EVENT_ALARM = 1,	//本地报警事件
    EVENT_NETALARM,		//网络报警事件
    EVENT_DECALARM,		//解码器报警事件
    EVENT_MOTION,		//动态检测事件
    EVENT_BLIND,		//遮挡检测事件
    EVENT_LOSS,			//视频丢失事件
    EVENT_NOHDD,		//无硬盘事件
    EVENT_ERRHDD,		//硬盘出错事件
    EVENT_NOSPACEHDD,	//硬盘空间不足
    EVENT_NETABORT, 	//断网事件
    EVENT_CODER_ALARM,	//编码器事件
    EVENT_INFRARED_ALARM,//红外报警事件
    EVNET_ALL,			//确保放在最后
};
//End yuxy_070418

//工作表类型
typedef enum _worksheet_type_
{
    WORKSHEET_TYPE_RECORD = 1,	//普通录像
    WORKSHEET_TYPE_ALARM,		//本地报警
    WORKSHEET_TYPE_NETALARM,	//网络报警
    WORKSHEET_TYPE_MOTION,		//动态检测
    WORKSHEET_TYPE_BLIND,		//遮挡检测
    WORKSHEET_TYPE_LOSS,		//视频丢失
    WORKSHEET_TYPE_NOHDD,		//无硬盘
    WORKSHEET_TYPE_ERRHDD,		//硬盘出错
    WORKSHEET_TYPE_NOSPACEHDD,	//硬盘无空间
    WORKSHEET_TYPE_NETABORT,	//断网
    WORKSHEET_TYPE_DECALARM,	//解码器报警
    WORKSHEET_TYPE_CODINGALARM,  //编码器报警
    WORKSHEET_TYPE_InfraredAlarm, //无线报警
    WORKSHEET_TYPE_ALL,			//一般工作表类型在这之前添加
};

//!报警类型
typedef enum _alarm_type_t
{
    ALARM_IN     = 1,    //外部报警
    ALARM_MOTION    ,    //动态监测报警
    ALARM_LOSS      ,    //视频丢失报警
    ALARM_BLIND     ,    //视频遮挡
    ALARM_AUDIO     ,    //音频报警
    ALARM_DISKLOW   ,    //硬盘低容量报警
    ALARM_DISKERROR ,    //硬盘错误报警
    ALARM_NODISK    ,    //无硬盘报警
    ALARM_DECODER   ,    //解码器报警
    ALARM_ENCODER,       //编码器报警
    ALARM_EMERGENCY,	 //紧急报警事件
    ALARM_INFRARED = 12, //红外报警事件

#ifdef OEM_BORUI_A3
	ALARM_POWERCUT = 13,  //断电
	ALARM_BATTERYFAILURE = 14, //电池故障
	ALARM_GSMBAD = 15,//GSM故障
	ALARM_GSMABORT = 16,//有线网络断开
	ALARM_GSMARP = 17,//内部ip冲突
#endif

    ALARM_ALL,    //一般报警如果需要添加，这前添加

    //下面是特殊的报警，从151开始
    ALARM_COMM  = 151,  //串口故障
};

//!串口报警
typedef struct __com_alm_info
{
	U8 para[128];
}
COM_ALM_INFO;

//!抓图参数
typedef struct _snap_param
{
	U32 Ver;            //抓图的通道
	U32 SnapSecond;     //时间 - 秒
	U32 SnapMSecond;    //时间 - 毫秒
	U32 SnapQuality;    //画质1~6
	U32 Reserved[4];
}
SNAP_PARAMS;

//!应用配置是否成功
typedef enum __cfg_app_ret_t {
    CFG_APP_FAILED = -100,		//应用配置失败
    CFG_APP_SAVECFG_FAILED = -99,	//保存配置失败

    CFG_APP_SUCCESSFUL = 0,		//应用配置成功

    CFG_NET_GATEWAY_FAILED,	//网关设置失败,仍然归类为成功
    CFG_APP_NEED_RESTART,	//重启应用程序后才能应用
    CFG_APP_NEED_REBOOT,	//重启系统后才能应用
};

//!查询系统信息
typedef enum __sys_info_t {
    SYSINFO_RECFILESYS =  0	,	// DVR录像文件系统信息
    SYSINFO_SPLIT 		,	// 分割
    SYSINFO_CAPTURE   	,	// 捕获
    SYSINFO_EXCAPTURE   ,	// 多画面捕获
    SYSINFO_MOTIONDETECT,	// 动态检测
    SYSINFO_LOSSDETECT 	,	// 视频丢失
    SYSINFO_BINDDETECT 	,	// 黑屏检测
}SYS_INFO;

//!产品标记
enum product_t {
    /* 视毫 */
    DH_DVR_H	=		0,
    /* D8104 */
    DEVICE_TYPE_D8104	=		1,
    /* D8008 */
    DEVICE_TYPE_D8008	=		2,
    /* D8404 */
    DEVICE_TYPE_D8404	=		3,
    /* ATM机器 */
    DH_DVR_ATM	=		4,
    /* 网络视频服务器 */
    DH_NVS		=		5,
    /* 视频服务器0404e */
    DH_NVS_E	=		6,
    /*B系列NVS*/
    DH_NVS_B	=		10,
    /*H系列NVS*/
    DH_NVS_C	=		11,

    DH_CAM_SDIP	=		50,
    DH_CAM_IPC	=		51,
    DH_HISI_IPC	=		55,

    /*A6机型*/
    DH_DVR_A6   =       60,
};

enum EnLoginResult
{
    EnLoginResult_Succeeded = 0,	// login succeeded
    EnLoginResult_Failed = 1,		// login failed
    EnLoginResult_Logined = 3,		// login yet
};

//!测试设备
enum EnTestDeviceType
{
    EnTestDevice_Null = 0,
    EnTestDevice_Smtp = 1,			// 测试Smtp配置
};

#define MAX_FILE_NUM_ONCE		16		//网络查询，一次最多16个文件
#ifdef _USE_NET_MODULE
#define MAX_CODETYPE			CHL_FUNCTION_NUM
#define INVALID_CODEVALUE       CHL_FUNCTION_NUM+1
#else
#define MAX_CODETYPE			1
#define INVALID_CODEVALUE       2
#endif

//!通道数量
#ifdef _USE_NET_MODULE
	#define TOTAL_CHANNEL_NUM N_SYS_CH
	#define TOTAL_AUDIO_IN_NUM N_AUDIO_IN
#else
	#define TOTAL_CHANNEL_NUM 16
	#define TOTAL_AUDIO_IN_NUM 1
#endif

extern int g_nAlarmIn;
extern int g_nAlarmOut;
extern int g_nCapture;
extern int g_nPlay;

//!通道监视切换
enum NetworkMonitorStatus
{
    CHANNEL_MONITOR_OFF=0,
    CHANNEL_MONITOR_ON=1,
    CHANNEL_MONITOR_NOCHANGE=2,
};

//!音频数据获取
enum GetAudioDataStatus
{
    CHANNEL_AUDIO_DATA_OFF=0,
    CHANNEL_AUDIO_DATA_ON=1,
};

enum EnReplayStatus
{
    OnReplay = 1,
    OnPause,
    OnStop,
    OnOthers,
};

enum EnVideoStandard
{
    VideoStandard_PAL = 0x01,
    VideoStandard_NTSC = 0x00,
};

enum EnCtrlErrorList		//控制权限
{
    NO_CTRL_POWER = 0x10,	//无控制权力
    NO_PRIVILEGE,			//权限不够
    NO_RESOURCE,			//资源不足
    USERNUMEXCEED,			//用户数超出
    USERINEXISTENCE,		//用户已存在
    PERMITLAND,				//允许登陆
    OWNER,					//控制权拥有者
    BUSYNESS,				//服务机忙
    INCLAIMING,				//正在申请
    SUCCESS,				//成功
    CLAIM,					//要求控制权
    REFUSE,					//拒绝
    BEREFT,					//被剥夺
    INUDPWATCHING,			//有用户在UDP监视
    FREEBOOTING,			//强行获得控制权
    RELEASE,				//释放控制权
    POWERSTATUS,			//权限状态 20
    TCPONLYONE,				//每一路只给一个用户监视
    MONITORBEREFT,			//该通道监视被剥夺
};


//!普通信息返回码 对应命令 0x64
enum GeneralMsgRetCode
{
    GeneralMsgRetCode_Success = 0, // 无错
    GeneralMsgRetCode_Busy = 1, // 系统忙
    GeneralMsgRetCode_NoAuth = 2, // 无权限
    GeneralMsgRetCode_ConnFull = 3, // 连接满
    GeneralMsgRetCode_IpNotAllowed = 4, // IP 不被容许
};

//!设置的码流类型
enum SetBitRate
{
    BitRate1 = 64,
    BitRate2 = 128,
    BitRate3 = 256,
    BitRate4 = 512,
    BitRate5 = 768,
    BitRate6 = 1024,
    BitRate7 = 65536,
};

#define CHECK_NETUSER_AUTH(X)				\
	if (false == GetOperator(m_pMsg->iDlgNo)->isValidAuthority((X)))	\
	{		\
		trace("!!! [%d] has not the auth(DVRG_AUTHORITY_ACCOUNT).\n",		\
		      GetOperator(m_pMsg->iDlgNo)->GetUID() );		\
		\
		SetState(OpStEnd, iThrdIdx);		\
		return OpStEnd;		\
	}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * 对应命令: 0xa4
 */
//!查询系统属性信息
typedef struct __sysattr_t
{
	unsigned char iVerMaj;          // BOIS主版本号(软件)
	unsigned char iVerSub;          // BOIS次版本号(软件)
	unsigned char iVideoInCaps;     // 视频输入接口数量
	unsigned char iAudioInCaps;     // 音频输入接口数量
	unsigned char iAlarmInCaps;     // 报警输入接口数
	unsigned char iAlarmOutCaps;    // 报警输出接口数
	unsigned char iIdePortNum;      // 硬盘接口数
	unsigned char iAetherNetPortNum;// 网络接口数
	unsigned char iUsbPortNum;      // USB接口数
	unsigned char iComPortNum;      // 串口数
	unsigned char iParallelPortNum; // 并口口数
	unsigned char iReserved_NULL0;  // 空字节, 为了对齐
	unsigned short year;            // 编译日期: 年
	unsigned char  month;           // 编译日期: 月
	unsigned char  day;             // 编译日期: 日
	unsigned char iVerUser;         // BOIS用户版本号(软件)
	unsigned char iVerModify;       // BOIS修改版本号(软件)
	unsigned char iWebVersion[4];   // WEB版本
	unsigned char WebYear;          // 预留
	unsigned char WebMonth;         // 预留
	unsigned char WebDay;           // 预留
	unsigned char iDecodeChanNum;     // 本地解码(回放)路数
	unsigned char iSpeechInCaps;     // 对讲输入接口数量
	unsigned char iSpeechOutCaps;     // 对讲输出接口数量
	unsigned char iVGAPortNum;     // VGA接口数量
	unsigned char iTVOutPortNum;     // TV输出接口数量
	unsigned char iIsMutiEthIf;     // 网卡能力支持 0:单网卡 1 多网卡
	unsigned char iReserved_07;     // 预留
}
SYSATTR_T;

//!查询视频属性信息
typedef struct __sysattr_video_caps
{
	unsigned int iCifCaps; 		// 大小
	unsigned int iTypeCaps;		// 编码
	unsigned int iFrameCaps;	// 帧率
	unsigned int iQualityCaps;	// 画质
	unsigned char iCifFrames;	// 每个芯片编码总帧数 单位：cif/s
	unsigned char _iRev_00;		// 保留
	unsigned char _iRev_01;		// 保留
	unsigned char _iRev_02;		// 保留
	unsigned int iReserved_01;	// 保留
	unsigned int iReserved_02;	// 保留
	unsigned int iReserved_03;	// 保留
	unsigned char iStandard;	// 制式
	unsigned char iRev_00;		// 保留
	unsigned char iRev_01;		// 保留
	unsigned char iRev_02;		// 保留
}
SYSATTR_VIDEO_CAPS_T;
//!iFrameCaps对应定义
typedef enum capture_frame_t
{
    cap_frame_01,
    cap_frame_02,
    cap_frame_03,
    cap_frame_06,
    cap_frame_12,
    cap_frame_20,
    cap_frame_25,
    cap_frame_30,
};

//! 查询语音对讲属性
typedef struct __dialog_caps_t
{
	// 支持的音频数据个数的数量
	U8 iAudioTypeNum;		/*!< 是否支持语音对讲 */
	// 保留位
	U8 iRev[31];
}
DIALOG_CAPS;

typedef struct __cam_caps
{
	U8 	iBrightnessEn;	// 亮度 可调 1 可， 0不可
	U8  iContrastEn;		// 对比度 可调
	U8  iColorEn;		// 色度 可调
	U8  iGainEn;			// 增益 可调
	U8  iSaturationEn;	// 饱和度 可调
	U8  iBacklightEn;	// 背光补偿 可调
	U8  iExposureEn;		// 曝光选择 可调
	U8  iColorConvEn;	// 自动彩黑转换 可调
	U8  iAttrEn;			// 属性选项  1 可， 0不可
	U8  iMirrorEn;	     // 镜像  1 支持， 0不支持
	U8  iFlipEn;	          // 翻转   1 支持， 0不支持
	U8  iRev[125];		// 保留
}
CAM_CAPS;

/// 摄像头控制结构
typedef struct cam_control
{
	U8  Exposure; ///< 曝光模式 1-6:手动曝光等级; 0:自动曝光
	U8  Backlight; ///< 背光补偿 true:打开补偿; false:停止补偿
	U8  AutoColor2BW; ///< 自动彩黑转换 true:打开转换; false:停止转换
	U8  Mirror;	     //< 镜像  1 开， 0关
	U8  Flip;	          //< 翻转  1 开， 0关
	U8    iRev[123];
}
CAM_CONTROL;

typedef struct __audio_attr_t
{
	// 支持的编码类型
	U16 iAudioType;			// 1:PCM, 2:G711a
	// 位
	U16 iAudioBit;			// 用实际的值表示， 如8位 则填值为8
	// 支持的采样率
	U32 dwSampleRate;			// 用实际的值表示， 如8k 则填值为8000
	// 保留位
	U8	iRev[64];
}
AUDIO_ATTR_T;

typedef enum audio_forat_t
{
    AUDIO_FORMAT_PCM8 = 1,
    AUDIO_FORMAT_G711a = 2,
    AUDIO_FORMAT_AMR = 3,
    AUDIO_FORMAT_G711u = 4,
    AUDIO_FORMAT_G726 = 5,
    AUDIO_FORMAT_IMAADPCM = 6
};

//!iQualityCaps对应定义
typedef enum capture_quality_t
{
    cap_quality_00,
    cap_quality_01,
    cap_quality_02,
    cap_quality_03,
    cap_quality_04,
    cap_quality_05,
};

//!查询字符集属性信息
typedef struct __sys_string_support
{
	U8  Version;		// 本配置的版本信息
	U32 Type;       	// 支持的类型， 掩码表示， 从低到高依表示：自定义|UTF-8
	U8  iReserved_00;	// 预留
	U8  iReserved_01;	// 预留
	U8  iReserved_02;	// 预留
	U8  iReserved_03;	// 预留
	U8  iReserved_04;	// 预留
	U8  iReserved_05;	// 预留
	U8  iReserved_06;	// 预留
	U8  iReserved_07;	// 预留
}
SYS_STRING_SUPPORT;

enum EnEncodingType
{
    ENCODING_UTF8 = 0x00000001,
};

/* 登陆属性 */
typedef struct __login_attribute
{
	/* 是否必须登陆设备方可使用， 0－要登陆，1－不必登陆 */
	U8	iShouldLogin;
	/* 支持的登陆用户名最大长度 */
	U8	iUNameMaxLen;
	/* 支持的密码最大长度 */
	U8	iUPwdMaxLen;
	/* 支持的加密方式 */
	U8	iEncryptType;/* 从低到高，按位表示加密方式：0-3des, */
	U8  iReserved[28];/* 保留 */
}
LOGIN_ATTRIBUTE;


#if defined(FUNC_GDYT_ATM) || defined(FUNC_HZDS_ATM) || defined (FUNC_TJYL_ATM)
enum EnATMTradeType
{
    ATM_TRADE_TYPE_QUERY = 0x0101,
    ATM_TRADE_TYPE_WITHDRAW = 0x0102,
    ATM_TRADE_TYPE_MODPWD = 0x0103,
    ATM_TRADE_TYPE_TRANSFER = 0x0104,
    ATM_TRADE_TYPE_DEPOSIT = 0x0105,
};
#endif

//! 预览图像设置
typedef struct __config_ex_capture_t
{
	U8		 iQuality;		// 画质
	U8		 iReserved[31];	// 保留
}
CONFIG_EX_CAPTURE_T;

//!云台名称属性, REQ_PROT_TITLE, 第9字节为0
typedef struct _net_ptz_name
{
	char		ProtocolName[12];	// 12字节的协议名称
	U32			BaudRate; 			// 4字节的波特率
	U8			DataBits; 			// 1字节的数据位
	U8			StopBits; 			// 1字节的停止位
	U8			Parity; 			// 1字节的奇偶校验
	U8			Resove;				// 1字节保留
}
NET_PTZ_NAME;

//!串口名称属性, REQ_PROT_TITLE, 第9字节为1
typedef struct _net_comm_name
{
	char		ProtocolName[12];	// 12字节的协议名称
	U32			BaudRate; 			// 4字节的波特率
	U8			DataBits; 			// 1字节的数据位
	U8			StopBits; 			// 1字节的停止位
	U8			Parity; 			// 1字节的奇偶校验
	U8			Resove;				// 1字节保留
}
NET_COMM_NAME;

//!设备轮询信息
typedef struct _device_query_info
{
	U8          Devicestat;
	U8          InAlarmCaps;
	U8          OutAlarmCaps;
	U8          iReserv;
}
DEVICE_QUERY_INFO;

//!通道轮巡信息
typedef struct _channel_state
{
	U8 ChannleLoss[32];
	U8 DspState[32];
}
CHANNEL_STATE;

//! 内部使用，
// for : INTERNAL_ALARM_TO_CENTER_SDK
typedef struct __config_alarm_info_t
{
	// 报警类型
	U32	 iAlarmType;
	// 报警掩码
	int	iAlarmMask;
}
CONFIG_ALARM_INFO_T;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/**
* 帧定位信息
*/

#define FRAME_LOCATION_INFO_NUM 50
#define FRAME_TYPE_I	1
#define FRAME_TYPE_P	0
#define FRAME_TYPE_B	2
#define FRAME_TYPE_AUDIO	3
#define FRAME_TYPE_OTHERS	4
#define FRAME_TYPE_NULL	0xFF

//! 帧定位信息
typedef struct __frame_location_info_t
{
	U8 iFrameType;				/*!< 帧类型 */
	U8 iFrameFlag;				/*!< 帧头尾的标识 */
	U16 iFramePos;				/*!< 帧的起始位置 */
	U32 iFrameLength;			/*!< 帧的长度，可能跨块 */
	U16 iDataLength;				/*!< 帧在本块中的长度 */
	U8 iReserve[2];				/*!< 保留 */
}
FRAME_LOCATION_INFO_T;

//! 包头的信息定义
typedef struct __packet_head_t
{
	FRAME_LOCATION_INFO_T fliFrameInfo[FRAME_LOCATION_INFO_NUM];		/*!< 所有帧信息 */
	U8			iPacketInfo;							/*!< 打包信息 */
	U8			iReserve[7];							/*!< 保留 */
}
PACKET_HEAD_T;

//
#if 0
enum EnMonitorSendMode
{
    // 宽带，处理效率优先；适用于宽带，共监视视频小于16路的情况
    EnMonitorSendMode_WidthBandHighPerformance = 1,
    // 宽带，画面延迟小；适用于宽带，多路监视，避免因为设备性能因素引起画面延迟
    EnMonitorSendMode_WidthBandRealtime,
    // 宽带，内部自动处理，在延迟和流畅间平衡
    EnMonitorSendMode_WidthBandAuto,
    // 窄带, 尽量多发送数据
    EnMonitorSendMode_NarrowBandHighPerformance,
    // 窄带, 尽量减少画面延迟
    EnMonitorSendMode_NarrowBandRealtime,
    // 窄带, 内部自动处理，在延迟和流畅间平衡
    EnMonitorSendMode_NarrowBandAuto,
    // 只传I帧, 一般用于调试
    EnMonitorSendMode_IFrameOnly,
    // 只传I和P帧, 一般用于调试
    EnMonitorSendMode_IAndPFrame,
    // 自动处理
    EnMonitorSendMode_Auto,
};
#endif
typedef struct __para_req_media_udpmode
{
	IPDEF 		tRemoteIp;		//
	LISTENPORT 	tRemotePort;	// 0-65535
	U8			iChannel;		// 0-n
	U8			iOpr;			// 0-Stop, 1-Start
	U8          iSteamType;
	U8			iReserved[31];	// reserved
}
PARA_REQ_MEDIA_UDPMODE;

#include "../Include/DVRDEF.H"

/// 捕获辅助码流支持特性结构
typedef struct NET_CAPTURE_EXT_STREAM
{
	DWORD ExtraStream;						///< 用channel_t的位来表示支持的功能。
	DWORD CaptureSizeMask[64];	///< 每一个值表示对应分辨率支持的辅助码流。
}
NET_CAPTURE_EXT_STREAM;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifdef __cplusplus
extern "C"
{
#endif

	extern int *fps_jw;

	//! 编码方式
	int ExCaptureSetEncode(int iChannel, int iEncode);

	//! 大小
	int ExCaptureSetSize(int iChannel, int iSize);

	//! 画质  	1-6 档
	int ExCaptureSetQuality(int iChannel, int iQuality);

	//! 帧率  	1 2 3 6 12 25
	int ExCaptureSetFrame(int iChannel, int iFrame);

	/*! 设置主机IP
	 * \param pHostIp 主机IP
	 * \param pNetmask 子网掩码
	 */
	int NetSetHostIP(const char *pHostIp,
	                 const char *pNetmask);
	/*! 设置网关
	 * \param pGageway 网关
	 */
	int NetSetGateway(const char *pGageway);
	/*! 取主机IP
	 * \param pHostIp 主机IP
	 * \param iHLen 主机IP长度
	 * \param pNetmask 子网掩码
	 * \param iNLen 子网掩码长度
	 */
	int NetGetHostIP(char *pHostIp,
	                 const int iHLen,
	                 char *pNetmask,
	                 const int iNLen);
	/*! 取网关地址
	 * \param pGageway 网关
	 * \param iGLen 网关长度
	 */
	int NetGetGateway(char *pGageway,
	                  const int iGLen);

#ifdef __cplusplus
}
#endif

#endif // _CMDSTYPEDEF_H
