/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * mainSysMan.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: mainSysMan.cpp 5884 2013-06-25 04:23:04Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-06-25 04:23:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h> /*sleep*/

#include "EZThread.h"
#include "EZTimer.h"

#include "Maintenance.h"
#ifdef _DEBUG
	#define DEB(x)
	#define DBG(x)
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

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

void signal_proc(int signo);
void this_usage(int argc, char *argv[]);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// config fname
const char * const ConfigFileSolar_1= "smartPorts.cfg";
const char * const ConfigFileSolar_2 = "smartPorts.cfg.duplicate";
#include "ConfigManagerNetInit.h"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int main(int argc, char * argv[])
{
    int c;
    bool bUseCfgAtStartForce = false;
    bool bMaintenanceAuto = false;
    char *cvalue = NULL;

    while ((c = getopt (argc, argv, "aif:")) != -1)
    {
        switch (c)
        {
            case 'a':
            {
                bMaintenanceAuto = true;
                break;
            }
            case 'i':
            {
                bUseCfgAtStartForce = true;
                break;
            }
            case 'f':
            {
                cvalue = optarg;
                break;
            }
            case '?':
            {
                if (optopt == 'f')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            }
            default:
            {
                // abort ();
            }
        }
    }

    DBG(
        printf("\n\n\n");
        __fline;
        printf ("bUseCfgAtStartForce = %d, bMaintenanceAuto = %d\n",
                bUseCfgAtStartForce, bMaintenanceAuto);
    );

    if (cvalue==NULL
        ||!(bUseCfgAtStartForce || bMaintenanceAuto))
    {
        this_usage(argc, argv);
        return 0;
    }

    if (bUseCfgAtStartForce || bMaintenanceAuto)
    {
        if (cvalue)
        {
            int iret = g_Config.initialize(cvalue, "");
            if (iret<0)
            {
                //__trip;
                printf("Config open failed (%s).\n", cvalue);
                exit(-1);
            }
        }
        else
        {
            printf("-i <config file pathname> needed.\n");
            exit(0);
        }

        g_TimerManager.Start();
        g_ThreadManager.RegisterMainThread(ThreadGetID());
        signal(SIGPIPE, signal_proc);

        signal(SIGTERM, signal_proc);
        signal(SIGKILL, signal_proc);
        signal(SIGQUIT, signal_proc);
        signal(SIGINT, signal_proc);

    }

    printf("Start: %s%s ...\n"
           , bUseCfgAtStartForce?"[INI DEVICE]":""
           , bMaintenanceAuto?"[AUTO MAINTAIN]":"");
    // 初始化
    g_Maintenance.Start(bUseCfgAtStartForce, bMaintenanceAuto);

    // 自动维护
    while (bMaintenanceAuto)
    {
        CEZSemaphore sem;
        sem.Wait();
    }


    return 0;
}

void signal_proc(int signo)
{
    if(signo == SIGTERM 	\
       || signo == SIGKILL \
       || signo == SIGQUIT \
       || signo == SIGINT   \
      )
    {
        g_Maintenance.Stop();
        exit(0);
    }
    else if(signo == SIGPIPE)
    {
        //g_Maintenance.haveException(signo, (char *)"on signal:SIGPIPE");
    }
}
const char * const copy_right = "@(#) Copyright (c) 2013 ezlibs.com.  All rights reserved";
const char * const desc="P_2013-6-25.SmartPorts.1010";

void ShowVersion()
{
    printf("---------------------------\n\n");
    printf("%s\n", copy_right);
    //printf("%s\n", desc);
    printf("$ Solar: %s,v 2.6 %s %s Exp $\n\n", __FILE__, __DATE__, __TIME__);
    printf("---------------------------\n");
}

void this_usage(int argc, char *argv[])
{
    ShowVersion();

    printf ("Usage: %s [-afi]\n\n", argv[0]);

    printf ("Sys initializate and maintain\n\n");

    printf ("Options:\n");
    printf ("\t -f      <config filename> Config filename, all function need it\n");
    printf ("\t -a      Auto maintain, send gratuitous arp ...\n");
    printf ("\t -i      Use config file to initialize this device\n");
    printf ("\t Function Options should select one or more \n");
}

