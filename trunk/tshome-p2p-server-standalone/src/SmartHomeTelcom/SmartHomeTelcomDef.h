/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SmartHomeTelcomDef.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: SmartHomeTelcomDef.h 5884 2012-09-06 04:47:15Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-06 04:47:15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _SmartHomeTelcomDef_H
#define _SmartHomeTelcomDef_H

//jstecome
#if (defined(SMARTHOME_JSTELECOM))
//#define DEFAULT_NEXT_HTTP_HOST_JS "202.102.108.171"
//#define DEFAULT_NEXT_HTTP_PORT_JS 8080
//#define URL_requestAuth "/smartHome/servlet/requestAuth"

#elif (defined(SMARTHOME_TIANSU))

//#define DEFAULT_NEXT_HTTP_HOST_JS "192.168.10.127"
//#define DEFAULT_NEXT_HTTP_PORT_JS 80

//#define URL_requestAuth "/TSRemote/smarthome/requestAuth.c"
#endif // SMARTHOME_JSTELECOM

#define JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU "61"
#define JSTELECOM_SMARTHOME_USERNAME_TIANSU "tiansu"
#define JSTELECOM_SMARTHOME_PASSWD_TIANSU "111111"

#define JSTELECOM_SMARTHOME_CTRL_URL "http://www.ts-home.cn:9000/smarthome/ctrl"

//#define JSTELECOM_SMARTHOME_PRODUCT_ID_TIANSU "tiansu001" // 暂时不用
#define JSTELECOM_SMARTHOME_DEVICE_ID_TIANSU "tiansu000001"

#define CMD_TIMEOUT_SMARTHOME_JSTELECOM 5

// 测试数据
#define TEST_OPERATE_ID_SW  "00010D11030100B6"
#define TEST_OPERATE_ID_SW_TIAOG  "00010D11030100B7"
#define TEST_OPERATE_ID_AL  "A0010D11030100B6"
#define TEST_OPERATE_NAME  "大灯"

typedef struct __generalservercfg_t
{
    unsigned short IsUse;
    unsigned short Port;
    char strIP[16];
    // will find by name
    std::string strServerName;
}
GENERALSERVERCFG_T;

#endif // _SmartHomeTelcomDef_H
