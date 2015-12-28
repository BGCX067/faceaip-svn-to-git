/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * main.c - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: main.c 5884 2013-05-28 09:01:25Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-05-28 09:01:25  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include	<signal.h>
#include	<unistd.h>
#include	<sys/types.h>

#include	"smart_webs.h"

static void	sigintHandler(int);

/*********************************** Code *************************************/
/*
 *	Main -- entry point from LINUX
 */
int main(int argc, char** argv)
{
    printf("$ smartwebs: main.c,v 2.6 %s %s Exp $\n\n", __DATE__, __TIME__);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigintHandler);
    signal(SIGTERM, sigintHandler);

    smart_webs_init();

    // -1<0 to use default port
    smart_webs_open(-1, -1);

    smart_webs_proc();

    // smart_webs_close();
    //
    // smart_webs_cleanup();

    return 0;
}

/*
 *	Exit cleanly on interrupt
 */
static void sigintHandler(int unused)
{


    smart_webs_close();

    smart_webs_cleanup();
}

