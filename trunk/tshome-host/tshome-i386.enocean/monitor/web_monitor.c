/*
  This program is to monitor the tshome-host.
  The tshome-host is the child of the Monitor, and 
  the Monitor monitor and control tshome-host.
  When tshome-host is terminated, it will be restarted
  by the Monitor
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "common/common_define.h"
#include "log/ts_log_tools.h"
#include "keep_dog.h"

#define MAX_RETRY_TIMES				1
#define MAX_STR_LEN        100
#define WEB_NAME 					"webs"


void sig_handler(int sig)
{
    char cmd[MAX_STR_LEN] = {0};
    sprintf(cmd,"killall -15 %s", WEB_NAME);
    system(cmd);

    exit(0);
}

int free_memory(char **argv, int count)
{
    int i = 0;
    for(; i< count; i++) {
        if(argv[i] != NULL)
            free(argv[i]);
    }
}


int main(int argc, char *argv[])
{
    int ret, i, status;
    char *child_argv[100] = {0};
    pid_t pid;
    int j = 0;
    int exit_flag = 0;

    struct sigaction act;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGTERM, &act, 0);

    if (argc < 2)
    {
        log_err( "Usage:%s <exe_path> <args...>\n", argv[0]);
        return -1;
    }

#ifdef __ARM
	log_debug("keep dog!\n");
	//start_keep_dog();
#else
	log_debug("no dog\n");
#endif

    for (i = 0; i < argc - 1; ++i) {
        child_argv[i] = (char *)calloc(strlen(argv[i + 1]) + 1, sizeof(char));
        if(child_argv[i] == NULL) {
            for(j = 0; j < i; j++) 
                free(child_argv[j]);

            printf ("Alloc memory for args failed\n");
            return -1;
        }
        strncpy(child_argv[i], argv[i + 1], strlen(argv[i + 1]));
        child_argv[i][strlen(argv[i + 1])] = '\0';
    }

    while(1) {
        pid = fork(); 
        if (pid == -1) {
            log_err( "fork() error.errno:%d error:%s\n", errno, strerror(errno));
            break;
        }

        if (pid == 0) {
            sleep(1);
            ret = execv(argv[1], (char **)child_argv);
            log_err( "execv ret:%d errno:%d error:%s\n", ret, errno, strerror(errno));
            exit(0);
        }
 
        if (pid > 0) {
            pid = wait(&status);
            exit_flag = WIFEXITED(status);

            // If the sub process terminated normally.
            if(exit_flag) {
            	log_debug("The sub process terminated normally!");
            	log_debug("status=%d, sub_exit_flag=%d, sub_exit_value=%d\n", status, exit_flag, WEXITSTATUS(status));
            } else {
            	log_debug("The sub process doesn't terminated normally, sub_status=%d!\n", status);
            }
        }
    }

    free_memory(child_argv, argc - 1);

    return 0;
}

