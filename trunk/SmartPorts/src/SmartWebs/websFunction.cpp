/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * embed_webs.c - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: embed_webs.c 5884 2013-04-08 04:23:26Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-04-08 04:23:26  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <string.h> /* for strncpy */
#include	<unistd.h>
#include	<sys/types.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*sleep*/

#include <str_opr.h> /* for strncpy */

#include "net_work_interface.h"

#include "../Configs/ConfigGeneral.h"
#include "../Configs/ConfigNetworkInterface.h"
#include "../Configs/ConfigNTPClient.h"
#include	"../Logs.h"
#include "../Solar.h"

//#include "../SysMan/Maintenance.h"

#include	"uemf.h"
#include	"wsIntrn.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;


#if 0
/*此例子取用一个参数*/

static int aspGetExa(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t	*want_para;

    if (ejArgs(argc, argv, T("%s"), &want_para) < 1)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    if (strcmp(want_para, "enable") == 0)
    {
        return websWrite(wp, T("%s"), "enable");
    }
    else
    {
        return websWrite(wp, T("%s"), "unsupport");
    }

    return 0;
}

static void formSetExa(webs_t wp, char_t *path, char_t *query)
{
    char_t	*devid_enocean0=NULL, *onoff_enocean0=NULL;

    unsigned int __device_id = 0x10189e5;
    //int __rocker = 1;
    int __up = 1; // 0 调暗， 1 调亮

    static int on_off_switch = 0;


    devid_enocean0 = websGetVar(wp, T("devid_enocean0"), T("0x10189e5"));
    onoff_enocean0 = websGetVar(wp, T("onoff_enocean0"), T("0"));

    if (devid_enocean0!=NULL)
    {
        sscanf(devid_enocean0, "%x", &__device_id);
    }
    if (onoff_enocean0!=NULL)
    {
        sscanf(onoff_enocean0, "%d", &__up);
    }
    else
    {
        __up = 0;
    }

    __up = on_off_switch;
    on_off_switch = !on_off_switch;

#ifdef DEV_ENOCEAN_SUPPORT
    //todo
    // your opr
    //enocean_device_onoff(__device_id, __rocker, __up);
#endif

    // ret page
#if 0

    websHeader(wp);
    websWrite(wp, T("<body>\n"));

    websWrite(wp, T("<h2>devid_enocean0: %s, onoff_enocean0: %s</h2>\n"), devid_enocean0, onoff_enocean0);

    websWrite(wp, T(PAGE_BTN_STR));

    websFooter(wp);
    websDone(wp, 200);
#else

    usleep(1000*100);
    websRedirect(wp, "form_ctrl_enocean.asp");
    return;

#endif
}
#endif //exa

//#define DEV_ENOCEAN_SUPPORT
#include "./enocean/enocean_mgr.h"
#ifdef DEV_ENOCEAN_SUPPORT
void enocean_callback(ENOCEAN_CALLBACK_PARAM_T *param);
typedef struct __enocean_device_status_t
{
    unsigned int device_id;                                             /* device id */
    int cur_status;                                            /* initial on/off status byte */
}
ENOCEAN_DEVICE_STATUS_T;

static ENOCEAN_DEVICE_STATUS_T g_dev_status[100];
#endif
// "<html>" in websHeader
#define PAGE_TOP_STR "<head>"\
					 "<title>SmartPorts</title>"\
					 "<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">"\
					 "</head>"\
					 "<body>"

#define PAGE_BTN_STR "</body>"


/******************************************************************************/
/*
 *	Test Javascript binding for ASP. This will be invoked when "aspTest" is
 *	embedded in an ASP page. See web/asp.asp for usage. Set browser to 
 *	"localhost/asp.asp" to test.
 */

static int aspGetIF(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t	*if_name, *want_para;

    int ret = 0;
    char l_mac_get[MAC_STR_SIZ];

    CConfigNet __cfgNet;
    CConfigNetworkInterface __cfgNetworkInterface;
    __cfgNet.update();
    __cfgNetworkInterface.update();

    if (ejArgs(argc, argv, T("%s %s"), &if_name, &want_para) < 2)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    // 检查网络接口
    int index = -1;
    if (strcmp(if_name, "general")==0)
    {
        index=0;
    }
    else
    {
        for (int ii=0; ii<MAX_NETWORK_INTERFACE_NUM; ii++)
        {
            if (__cfgNetworkInterface.getConfig(ii).strIFName == if_name)
            {
                index = ii;
            }
        }
    }

    if (index<0)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    if (strcmp(want_para, "auto") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNetworkInterface.getConfig(index).bDhcp==1?"checked=checked":"");
    }
    // 下列重新获取，便于页面更新后可以查看是否设置成功
    else if (strcmp(want_para, "ip") == 0)
    {
        //int ez_ip_get(const char *p_if_name, char *ip_get, char *bcast_get, char *mask_get, char *mac_get);
        ret = ez_ip_get(if_name, l_mac_get, NULL, NULL, NULL);
        if (ret<0)
        {
            //strcpy(l_mac_get, "failed get");
            l_mac_get[0] = '\0';
        }
        return websWrite(wp, T("%s"), l_mac_get);
    }
    else if (strcmp(want_para, "mask") == 0)
    {
        ret = ez_ip_get(if_name, NULL, NULL, l_mac_get, NULL);
        if (ret<0)
        {
            //strcpy(l_mac_get, "failed get");
            l_mac_get[0] = '\0';
        }
        return websWrite(wp, T("%s"), l_mac_get);
    }
    else if (strcmp(want_para, "gateway") == 0)
    {
        ret = ez_get_gateway(if_name, l_mac_get);
        if (ret<0)
        {
            //strcpy(l_mac_get, "failed get");
            l_mac_get[0] = '\0';
        }
        return websWrite(wp, T("%s"), l_mac_get);
    }
    else if (strcmp(want_para, "mac") == 0)
    {
        ret = ez_ip_get(if_name, NULL, NULL, NULL, l_mac_get);
        if (ret<0)
        {
            //strcpy(l_mac_get, "failed get");
            l_mac_get[0] = '\0';
        }

        return websWrite(wp, T("%s"), l_mac_get);
    }
    else if (strcmp(want_para, "default") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNetworkInterface.getConfig(index).bDefault==1?"checked=checked":"");
    }
    else if (strcmp(want_para, "PrimaryDNS") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNet.getConfig().strPrimaryDNS.c_str());
    }
    else if (strcmp(want_para, "SecondaryDNS") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNet.getConfig().strSecondaryDNS.c_str());
    }
    else
    {
        return websWrite(wp, T("%d"), 0);
        //return -1;
    }

    return 0;
}

/******************************************************************************/
/*
 *	set form for posted data (in-memory CGI). This will be called when the
 *	form in web/form_set_eth.asp is invoked. Set browser to "localhost/form_set_eth.asp" to test.
 */
char_t *network_iface_inputname_auto[] =
    {"eth0_auto", "eth1_auto", "eth2_auto", "eth3_auto", "eth4_auto", "eth5_auto"
    };
char_t *network_iface_inputname_ipv4[] =
    {"eth0_ipv4", "eth1_ipv4", "eth2_ipv4", "eth3_ipv4", "eth4_ipv4", "eth5_ipv4"
    };
char_t *network_iface_inputname_mask[] =
    {"eth0_mask", "eth1_mask", "eth2_mask", "eth3_mask", "eth4_mask", "eth5_mask"
    };
char_t *network_iface_inputname_gwv4[] =
    {"eth0_gwv4", "eth1_gwv4", "eth2_gwv4", "eth3_gwv4", "eth4_gwv4", "eth5_gwv4"
    };
static void formSetIf(webs_t wp, char_t *path, char_t *query)
{
    char_t	*gen_pri_dns = NULL;
    char_t	*gen_sec_dns = NULL;

    char_t	*if_auto[MAX_NETWORK_INTERFACE_NUM] = {NULL};
    char_t	*if_ipv4[MAX_NETWORK_INTERFACE_NUM] = {NULL};
    char_t	*if_mask[MAX_NETWORK_INTERFACE_NUM] = {NULL};
    char_t	*if_gwv4[MAX_NETWORK_INTERFACE_NUM] = {NULL};

    char_t	*if_default = NULL;
    int	idefault = -1;
    int iret;


    CConfigNet __cfgNet;
    CConfigNetworkInterface __cfgNetworkInterface;
    __cfgNet.update();
    __cfgNetworkInterface.update();

    int iNetworkInterfaceNum = __cfgNet.getConfig().iNetworkInterfaceNum;

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    for(int ii=0; ii<iNetworkInterfaceNum; ii++)
    {
        if_auto[ii] = websGetVar(wp, T(network_iface_inputname_auto[ii]), NULL);
        if_ipv4[ii] = websGetVar(wp, T(network_iface_inputname_ipv4[ii]), NULL);
        if_mask[ii] = websGetVar(wp, T(network_iface_inputname_mask[ii]), NULL);
        if_gwv4[ii] = websGetVar(wp, T(network_iface_inputname_gwv4[ii]), NULL);

        trim_blank(if_auto[ii], 0);
        trim_blank(if_ipv4[ii], 0);
        trim_blank(if_mask[ii], 0);
        trim_blank(if_gwv4[ii], 0);
    }

    if_default = websGetVar(wp, T("default_eth"), NULL);
    gen_pri_dns = websGetVar(wp, T("PrimaryDNS"), NULL);
    gen_sec_dns = websGetVar(wp, T("SecondaryDNS"), NULL);
    trim_blank(if_default, 0);
    trim_blank(gen_pri_dns, 0);
    trim_blank(gen_sec_dns, 0);

    websHeader(wp);
    websWrite(wp, T(PAGE_TOP_STR));

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // check and set
    //dns
    if (0 == ip4_valid(gen_pri_dns))
    {
        if (__cfgNet.getConfig().strPrimaryDNS != gen_pri_dns)
            __cfgNet.getConfig().strPrimaryDNS = gen_pri_dns;
    }

    if (0 == ip4_valid(gen_sec_dns))
    {
        if (__cfgNet.getConfig().strSecondaryDNS != gen_sec_dns)
            __cfgNet.getConfig().strSecondaryDNS = gen_sec_dns;
    }

    // ip etc
    for(int ii=0; ii<iNetworkInterfaceNum; ii++)
    {
        // default -- only one
        __cfgNetworkInterface.getConfig(ii).bDefault = 0;
        if (__cfgNetworkInterface.getConfig(ii).strIFName == if_default)
        {
            idefault = ii;
            __cfgNetworkInterface.getConfig(ii).bDefault = 1;
        }

        // 设定了auto 其他值就取不到了
        if (if_auto[ii])
        {
            __fline;
            printf("if_auto[%d]:%s, pass the static ip value.\n", ii, if_auto[ii]);
            __cfgNetworkInterface.getConfig(ii).bDhcp = 1;
            continue;
        }

        if (0 != ip4_valid(if_ipv4[ii]))
        {
            __trip;
            printf("if_ipv4[%d]:%s\n", ii, if_ipv4[ii]);
            if_ipv4[ii] = NULL;
        }
        if (ip4_valid(if_mask[ii])<0)
        {
            __trip;
            printf("if_ipv4[%d]:%s\n", ii, if_mask[ii]);
            if_mask[ii] = NULL;
        }
        if (0 != ip4_valid(if_gwv4[ii]))
        {
            __trip;
            printf("if_ipv4[%d]:%s\n", ii, if_gwv4[ii]);
            if_gwv4[ii] = NULL;
        }

        //__fline;
        //printf("if [%d]:[%s] [%s] [%s]\n", ii, if_ipv4[ii], if_mask[ii], if_gwv4[ii]);

        iret = ip4_mask_gateway_match(if_ipv4[ii], if_mask[ii], if_gwv4[ii]);
        if (iret < 0)
        {
            websWrite(wp, T("<H2>parameter ERROR</H2><ul>"
                            "<li>iret: %d</li>"
                            "</ul>")
                      , iret
                     );

            __trip;
            printf("ip4_mask_gateway_match error [%d]:[%s] [%s] [%s]\n", iret, if_ipv4[ii], if_mask[ii], if_gwv4[ii]);
        }
        else
        {
            __fline;
            __cfgNetworkInterface.getConfig(ii).bDhcp = 0;
#if 1

            if (if_ipv4[ii]!=NULL && __cfgNetworkInterface.getConfig(ii).strIp != if_ipv4[ii])
            {
                __fline;
                printf("strIp changed.\n");
                __cfgNetworkInterface.getConfig(ii).strIp = if_ipv4[ii];
            }
            if (if_mask[ii]!=NULL && __cfgNetworkInterface.getConfig(ii).strMask != if_mask[ii])
            {
                __fline;
                printf("strMask changed.\n");
                __cfgNetworkInterface.getConfig(ii).strMask = if_mask[ii];
            }
            if (if_gwv4[ii]!=NULL && __cfgNetworkInterface.getConfig(ii).strGateWay != if_gwv4[ii])
            {
                __fline;
                printf("strMask changed.\n");
                __cfgNetworkInterface.getConfig(ii).strGateWay = if_gwv4[ii];
            }
#endif

        }
        DBG(
            printf("ip4_mask_gateway_match : %d\n\t%s\n\t%s\n\t%s\n\tResult -- %s\n"
                   , iret, if_ipv4[ii], if_mask[ii], if_gwv4[ii], iret==0?"OK":"Failed");
        );
    }
    // 页面未传
    if (idefault<0)
    {
        __cfgNetworkInterface.getConfig(0).bDefault = 1;
    }

    __cfgNetworkInterface.commit();
    __cfgNet.commit();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 打印输入信息
    for(int ii=0; ii<iNetworkInterfaceNum; ii++)
    {
        websWrite(wp, T("<h2>%s</h2><ul>"
                        "<li>auto: %s</li>"
                        "<li>ipv4: %s</li>"
                        "<li>mask: %s</li>"
                        "<li>gwv4: %s</li>"
                        "</ul>")
                  , __cfgNetworkInterface.getConfig(ii).strIFName.c_str()
                  , if_auto[ii]== NULL?"NULL":if_auto[ii]
                  , if_ipv4[ii]== NULL?"NULL":if_ipv4[ii]
                  , if_mask[ii]== NULL?"NULL":if_mask[ii]
                  , if_gwv4[ii]== NULL?"NULL":if_gwv4[ii]
                 );
    }

    websWrite(wp, T("<H2>General</H2><ul>"
                    "<li>default: %s</li>"
                    "<li>pdns: %s</li>"
                    "<li>sdns: %s</li>"
                    "</ul>")
              , if_default  == NULL?"NULL":if_default
              , gen_pri_dns == NULL?"NULL":gen_pri_dns
              , gen_sec_dns == NULL?"NULL":gen_sec_dns
             );
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    websWrite(wp, T(PAGE_BTN_STR));
    websFooter(wp);
    websDone(wp, 200);
}

/*arg1:id, arg2 ret value*/
static int aspGetEnoceanStatus(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t	*if_name, *want_para;

    if (ejArgs(argc, argv, T("%s %s"), &if_name, &want_para) < 2)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    if (strcmp(want_para, "button_style") == 0)
    {
#ifdef DEV_ENOCEAN_SUPPORT
        if (g_dev_status[0].cur_status>0)
#else

        if (0)
#endif

        {
            return websWrite(wp, T("%s"), "background-color: #00ff00;");
        }
        else
        {
            return websWrite(wp, T("%s"), "background-color: #ff0000;");
        }
    }
    else
        if (strcmp(want_para, "button_value") == 0)
        {
#ifdef DEV_ENOCEAN_SUPPORT
            if (g_dev_status[0].cur_status>0)
#else

            if (0)
#endif

            {
                return websWrite(wp, T("%s"), "On");
            }
            else
            {
                return websWrite(wp, T("%s"), "Off");
            }
        }


    return 0;
}

static void formCtrlEnocean(webs_t wp, char_t *path, char_t *query)
{
    char_t	*devid_enocean0=NULL, *onoff_enocean0=NULL;

    unsigned int __device_id = 0x10189e5;
    int __rocker = 1;
    int __up = 1; // 0 调暗， 1 调亮

    static int on_off_switch = 0;


    devid_enocean0 = websGetVar(wp, T("devid_enocean0"), T("0x10189e5"));
    onoff_enocean0 = websGetVar(wp, T("onoff_enocean0"), T("0"));

    if (devid_enocean0!=NULL)
    {
        sscanf(devid_enocean0, "%x", &__device_id);
    }
    if (onoff_enocean0!=NULL)
    {
        sscanf(onoff_enocean0, "%d", &__up);
    }
    else
    {
        __up = 0;
    }

    __up = on_off_switch;
    on_off_switch = !on_off_switch;

#ifdef DEV_ENOCEAN_SUPPORT

    enocean_device_onoff(__device_id, __rocker, __up);
#endif
    ///////
    usleep(1000*100);
    websRedirect(wp, "form_ctrl_enocean.asp");
    return;

    websHeader(wp);
    websWrite(wp, T("<body>\n"));

    websWrite(wp, T("<h2>devid_enocean0: %s, onoff_enocean0: %s</h2>\n"), devid_enocean0, onoff_enocean0);

    websWrite(wp, T(PAGE_BTN_STR));

    websFooter(wp);
    websDone(wp, 200);
}

#ifdef DEV_ENOCEAN_SUPPORT

void enocean_callback(ENOCEAN_CALLBACK_PARAM_T *param)
{
    ENOCEAN_CALLBACK_PARAM_T *__pa = (ENOCEAN_CALLBACK_PARAM_T *)param;
DBG(
    __fline;
    printf("\tenocean_callback u32DeviceID:0x%0x, u8Type:0x%0x, ucFunction:0x%0x, bOn:%d, value:%f\n"
           , (unsigned int)__pa->u32DeviceID
           , __pa->u8Type
           , __pa->ucFunction
           , __pa->bOn
           , __pa->fValue);
);
    g_dev_status[0].device_id = __pa->u32DeviceID;
    g_dev_status[0].cur_status = __pa->bOn;

}
#endif //DEV_ENOCEAN_SUPPORT


/*此例子取用一个参数*/
static int aspGetDateTime(int eid, webs_t wp, int argc, char_t **argv)
{
    //__fline;
    //printf("%s\n", __FUNCTION__);

    char_t	*want_para;

    if (ejArgs(argc, argv, T("%s"), &want_para) < 1)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    CConfigGeneral __cfgGeneral;
    CConfigNTPClient __cfgNTPClient;

    __cfgGeneral.update();
    __cfgNTPClient.update();

    if (strcmp(want_para, "ntp_enable") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNTPClient.getConfig().UpdatePeriod>0?"checked=checked":"");
    }
    else if (strcmp(want_para, "ntp_server") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNTPClient.getConfig().strNtpServer.c_str());
    }
    else if (strcmp(want_para, "ntp_checkperiod") == 0)
    {
        return websWrite(wp, T("%d"), __cfgNTPClient.getConfig().UpdatePeriod>=5?__cfgNTPClient.getConfig().UpdatePeriod:600);
    }
    else if (strcmp(want_para, "autotimezone") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNTPClient.getConfig().iTimeZoneAuto==1?"checked=checked":"");
    }

    else if (strcmp(want_para, "sys_date_time") == 0)
    {
        char pBuff[32];
        get_date_time_string(pBuff, NULL);
        return websWrite(wp, T("%s"), pBuff);
    }
    else if (strcmp(want_para, "sys_timezone") == 0)
    {
        return websWrite(wp, T("%d"), __cfgGeneral.getConfig().iTimeZone);
    }
    else if (strcmp(want_para, "sys_date_format") == 0)
    {
        return websWrite(wp, T("%d"), __cfgGeneral.getConfig().iDateTimeFormat);
    }
    else
    {
        return websWrite(wp, T("%s"), "unsupport");
    }

    return 0;
}

static void formSetDateTime(webs_t wp, char_t *path, char_t *query)
{
    char_t *p_ntp_enable     	= websGetVar(wp, T("ntp_enable"), 		NULL);
    char_t *p_ntp_server     	= websGetVar(wp, T("ntp_server"), 		NULL);
    char_t *p_ntp_checkperiod	= websGetVar(wp, T("ntp_checkperiod"), 	NULL);
    char_t *p_autotimezone   	= websGetVar(wp, T("autotimezone"), 	NULL);
    char_t *p_sys_date_time  	= websGetVar(wp, T("sys_date_time"), 	NULL);
    char_t *p_sys_timezone   	= websGetVar(wp, T("sys_timezone"), 	NULL);
    char_t *p_sys_date_format	= websGetVar(wp, T("sys_date_format"), 	NULL);

    //int i_ntp_enable = 0;
    int i_ntp_checkperiod = 0;
    //int i_autotimezone = 0;
    int i_sys_timezone = 0;
    //int i_sys_date_format = 0;

    // use ntp
    CConfigGeneral __cfgGeneral;
    CConfigNTPClient __cfgNTPClient;

    __cfgGeneral.update();
    __cfgNTPClient.update();

    if (p_ntp_enable!=NULL)
    {
        if (p_ntp_server!=NULL)
        {
            __cfgNTPClient.getConfig().strNtpServer = p_ntp_server;
        }

        if (p_ntp_checkperiod != NULL)
        {
            sscanf(p_ntp_checkperiod, "%d", &i_ntp_checkperiod);
        }
        __cfgNTPClient.getConfig().UpdatePeriod = i_ntp_checkperiod>5?i_ntp_checkperiod:5;

    }
    // use input time
    else
    {
        __cfgNTPClient.getConfig().UpdatePeriod = 0;

        if (p_sys_date_time!=NULL)
        {
            int retval = ez_set_system_time(p_sys_date_time);
            ARG_USED(retval);
            DBG(
                __fline;
                printf("SetSystemTime:%s -- %d\n", p_sys_date_time, retval);
            );
        }
    }

    if (p_sys_timezone != NULL)
    {
        sscanf(p_sys_timezone, "%d", &i_sys_timezone);

        if (i_sys_timezone>=0 && i_sys_timezone<=33)
        {
            __cfgGeneral.getConfig().iTimeZone = i_sys_timezone;
        }
        else
        {
            LOG4CPLUS_ERROR(LOG_OF_SWEBS, "i_sys_timezone NOT valid:"<<i_sys_timezone);
        }
    }

    __cfgNTPClient.commit();
    __cfgGeneral.commit();

#if 1

    websHeader(wp);
    websWrite(wp, T("<body>\n"));

    websWrite(wp, T("<p>ntp_enable     :%s</p>\n"), p_ntp_enable      == NULL?"NULL":p_ntp_enable     );
    websWrite(wp, T("<p>ntp_server     :%s</p>\n"), p_ntp_server      == NULL?"NULL":p_ntp_server     );
    websWrite(wp, T("<p>ntp_checkperiod:%s</p>\n"), p_ntp_checkperiod == NULL?"NULL":p_ntp_checkperiod);
    if (p_autotimezone)
        websWrite(wp, T("<p>autotimezone   :%s</p>\n"), p_autotimezone    == NULL?"NULL":p_autotimezone   );
    websWrite(wp, T("<p>sys_date_time  :%s</p>\n"), p_sys_date_time   == NULL?"NULL":p_sys_date_time  );
    websWrite(wp, T("<p>sys_timezone   :%s</p>\n"), p_sys_timezone    == NULL?"NULL":p_sys_timezone   );
    if (p_sys_date_format)
        websWrite(wp, T("<p>sys_date_format:%s</p>\n"), p_sys_date_format == NULL?"NULL":p_sys_date_format);


    websWrite(wp, T(PAGE_BTN_STR));

    websFooter(wp);
    websDone(wp, 200);

#else

    usleep(1000*100);
    websRedirect(wp, "form_ctrl_enocean.asp");
    return;

#endif
}

static int aspGetSys(int eid, webs_t wp, int argc, char_t **argv)
{
    //__fline;
    //printf("%s\n", __FUNCTION__);

    char_t	*want_para;

    if (ejArgs(argc, argv, T("%s"), &want_para) < 1)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    if (strcmp(want_para, "sys_name") == 0)
    {
        return websWrite(wp, T("%s"), __cfgGeneral.getConfig().strMachineName.c_str());
    }
    else if (strcmp(want_para, "sys_copyright") == 0)
    {
        return websWrite(wp, T("%s"), g_Solar.GetCopyRights());
    }
    else if (strcmp(want_para, "sys_product_id") == 0)
    {
        return websWrite(wp, T("%s"), __cfgGeneral.getConfig().strProductID.c_str());
    }
    else if (strcmp(want_para, "sys_serial_no") == 0)
    {
        return websWrite(wp, T("%s"), __cfgGeneral.getConfig().strSerialNumber.c_str());
    }
    else if (strcmp(want_para, "sys_version") == 0)
    {
        return websWrite(wp, T("%s"), __cfgGeneral.getConfig().strVersion.c_str());
    }
    else if (strcmp(want_para, "sys_language") == 0)
    {
        return websWrite(wp, T("%d"), __cfgGeneral.getConfig().iLanguage);
    }
	
    else if (strcmp(want_para, "sys_run_period") == 0)
    {
        return websWrite(wp, T("%d"), g_Solar.GetRunPeriod());
    }
    else if (strcmp(want_para, "sys_start_date_time") == 0)
    {
        return websWrite(wp, T("%d"), g_Solar.GetStartTime());
    }
    else if (strcmp(want_para, "sys_internet") == 0)
    {
        return websWrite(wp, T("%s"), g_Solar.IsOnInternet()?"ON":"OFF");
    }
    else if (strcmp(want_para, "sys_nat_ip") == 0)
    {
        return websWrite(wp, T("%s"),  g_Solar.IsOnInternet()?g_Solar.GetNatIP():"NotValid");
    }
    else
    {
        return websWrite(wp, T("%s"), "unsupport");
    }

    return 0;
}

static void formSetSys(webs_t wp, char_t *path, char_t *query)
{
    char_t *p_sys_reboot     	= websGetVar(wp, T("sys_reboot"), 		NULL);
    char_t *p_sys_restore     	= websGetVar(wp, T("sys_restore"), 		NULL);

    char_t *p_sys_name		    = websGetVar(wp, T("sys_name"		 ), NULL);
    char_t *p_sys_product_id	= websGetVar(wp, T("sys_product_id"	 ), NULL);
    char_t *p_sys_serial_no	    = websGetVar(wp, T("sys_serial_no"	 ), NULL);
    char_t *p_sys_version	    = websGetVar(wp, T("sys_version"	 ), NULL);
    char_t *p_sys_language	    = websGetVar(wp, T("sys_language"	 ), NULL);
    // ret page

    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

#if 1

    websHeader(wp);
    websWrite(wp, T("<body>\n"));

    if (p_sys_name)
    {
        trim_blank(p_sys_name, 0);
        int __len = strlen(p_sys_name);
        if (__len>0 && __len<256)
        {
            __cfgGeneral.getConfig().strMachineName = p_sys_name;
        }
        websWrite(wp, T("<p>p_sys_name     :%s</p>\n"), p_sys_name      == NULL?"NULL":p_sys_name     );
    }

    if (p_sys_language)
    {
        trim_blank(p_sys_language, 0);
        int __len = strlen(p_sys_language);
        if (__len>0 && __len<128)
        {
            __cfgGeneral.getConfig().iLanguage = atoi(p_sys_language);
        }

        websWrite(wp, T("<p>p_sys_language     :%s</p>\n"), p_sys_language      == NULL?"NULL":p_sys_language     );
    }

    if (p_sys_product_id)
    {
        trim_blank(p_sys_product_id, 0);
        int __len = strlen(p_sys_product_id);
        if (__len>0 && __len<256)
        {
            __cfgGeneral.getConfig().strProductID = p_sys_product_id;
        }

        websWrite(wp, T("<p>p_sys_product_id     :%s</p>\n"), p_sys_product_id      == NULL?"NULL":p_sys_product_id     );
    }
    if (p_sys_serial_no)
        websWrite(wp, T("<p>p_sys_serial_no     :%s</p>\n"), p_sys_serial_no      == NULL?"NULL":p_sys_serial_no     );
    if (p_sys_version)
        websWrite(wp, T("<p>p_sys_version     :%s</p>\n"), p_sys_version      == NULL?"NULL":p_sys_version     );

    if (p_sys_reboot)
    {

        websWrite(wp, T("<p>p_sys_reboot     :%s</p>\n"), p_sys_reboot      == NULL?"NULL":p_sys_reboot     );
        websWrite(wp, T("<p>reboot ...</p>\n"));
    }

    if (p_sys_restore)
    {
        websWrite(wp, T("<p>p_sys_restore     :%s</p>\n"), p_sys_restore      == NULL?"NULL":p_sys_restore     );
        websWrite(wp, T("<p>restore ...</p>\n"));
    }

    websWrite(wp, T(PAGE_BTN_STR));

    websFooter(wp);
    websDone(wp, 200);

#else

    usleep(1000*100);
    websRedirect(wp, "form_ctrl_enocean.asp");
    return;

#endif

    __cfgGeneral.commit();

    if (p_sys_reboot)
    {
        g_Solar.Reboot();
    }

}


#ifdef __cplusplus
extern "C"
{
#endif

    void websFunctionInit()
    {
#ifdef DEV_ENOCEAN_SUPPORT

        ezserial_init();
        enocean_uart_init(DEFAULT_ENOCEAN_DEVICE_INTERFACE);
        //enocean_uart_init("/dev/ttySAC3");
        enocean_uart_open();
        enocean_uart_callback(enocean_callback);
        memset(&g_dev_status, 0, sizeof(g_dev_status));
#endif

        websAspDefine(T("aspGetIF"), aspGetIF);
        websFormDefine(T("formSetIf"), formSetIf);

        websFormDefine(T("formCtrlEnocean"), formCtrlEnocean);
        websAspDefine(T("aspGetEnoceanStatus"), aspGetEnoceanStatus);

        websAspDefine(T("aspGetDateTime"), aspGetDateTime);
        websFormDefine(T("formSetDateTime"), formSetDateTime);

        websAspDefine(T("aspGetSys"), aspGetSys);
        websFormDefine(T("formSetSys"), formSetSys);
    }

    void websFunctionCleanup()
    {
#ifdef DEV_ENOCEAN_SUPPORT
        enocean_uart_close();
        ezserial_cleanup();
#endif

    }
#ifdef __cplusplus
}
#endif

