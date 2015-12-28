/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Def_ODIP.h - _explain_
 *
 * Copyright (C) 2012 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: GeneralAgentDef.h 0001 2012-4-11 9:32:40Z WuJunjie $
 *
 *  Explain:
 *     -Common define-
 *
 *  Update:
 *     2012-4-11 9:32:49 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _DEF_ODIP_H
#define _DEF_ODIP_H

//协议解析器
#define INTERNAL_ODIP_DATA_PARSE 1

#ifdef WIN32
	#ifndef LITTLE_ENDIAN
		#define LITTLE_ENDIAN 1234
		#define BIG_ENDIAN 4321
		#define BYTE_ORDER LITTLE_ENDIAN 
	#endif
#else
	#include <endian.h>
#endif


//#define	DVRIP_VERSION	2
#define	DVRIP_VERSION	0 // single thread

typedef struct dvrip
{
	unsigned char	dvrip_cmd;		/* command  */
	unsigned char	dvrip_r0;		/* reserved */
	unsigned char	dvrip_r1;		/* reserved */
#if BYTE_ORDER == LITTLE_ENDIAN 
	unsigned char	dvrip_hl:4,		/* header length */
					dvrip_v :4;		/* version */
#endif
#if BYTE_ORDER == BIG_ENDIAN 
	unsigned char	dvrip_v :4,		/* version */
					dvrip_hl:4;		/* header length */
#endif
	unsigned int	dvrip_extlen;	/* ext data length */
	unsigned char	dvrip_p[24];	/* inter params */
}DVRIP;

typedef union
{
	struct dvrip	dvrip; /* struct def */
	unsigned char	c[32]; /* 1 byte def */
	unsigned short	s[16]; /* 2 byte def */
	unsigned int	l[8];  /* 4 byte def */
}ODIP_HEAD_T;
#define ODIP_HEAD_T_SIZE sizeof(ODIP_HEAD_T)
#define ZERO_ODIP_HEAD_T(X) memset((X), 0, ODIP_HEAD_T_SIZE);			\
							(X)->dvrip.dvrip_hl = ODIP_HEAD_T_SIZE/4;	\
							(X)->dvrip.dvrip_v = DVRIP_VERSION;




enum EnDVRCommand
{
    //请求
    REQ_USER_LOGIN	= 0xa0,
    REQ_STAT_QUERY	= 0xa1,			//a1 请求状态查询
    REQ_CHAN_SWTCH	= 0x11,			//视频切换

    //应答
    ACK_USER_LOGIN	= 0xb0,			//应答用户登陆
    ACK_STAT_QUERY	= 0xb1,			//应答状态查询
    ACK_CHAN_MONIT	= 0xbc,			//应答通道监视

    REQ_CTRL_RECRD	= 0xc5,			//请求录像控制
    REQ_INFO_SYSTM	= 0xa4,			//请求系统信息
    ACK_INFO_SYSTM	= 0xb4,			//应答系统信息
};

typedef struct _snap_param_new
{
	int Channel;        //抓图的通道
	int res[2];
	int mode;       //抓图模式 0：表示请求一帧,1：表示定时发送请求
	int res1;
	int CmdSerial;    //请求序列号
	unsigned char  Opr;// 0: stop 1:start
	unsigned char  Res[3];
	int Reserved[3];
}snap_param_new;


/* 回放是32K */
#define MAX_DVRIP_PACKET_SIZE 32*1024*2

#endif // _DEF_ODIP_H
