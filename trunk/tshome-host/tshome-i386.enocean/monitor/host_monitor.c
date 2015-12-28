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

#include "common/ts_err_code.h"
#include "common/common_define.h"
#include "log/ts_log_tools.h"

#include "db/ts_db_tools.h"
#include "db/ts_db_web_infos.h"
#include "db/ts_db_conn_infos.h"

#define OUTER_MAX_ARGS        8
#define INNER_MAX_ARGS        2
#define MAX_STR_LEN     100

char *outer_child_argv[OUTER_MAX_ARGS];
char *inner_child_argv[INNER_MAX_ARGS];
int mode = TS_IS_LOCAL;

void sig_handler(int sig)
{
    char cmd[100] = {0};
    sprintf(cmd,"killall -15 %s", TS_PROGRAM_NAME);
    system(cmd);
    exit(0);
}

/*
 * function		: get login string like path[CONTROL_LEN].
 * internet is	: path = bin-path_&_process	argv = -t server_ip:server_port -u user -p pwd
 * intranet is	: path = bin-path_&_process	argv = [NULL]
 */
int get_child_process_str(char *path)
{
    int res = EXIT_FAILURE;
    char param[256] = {0};
    ts_host_base_user_info base_info;

    res = db_get_is_local(&mode);
    if(res != EXIT_SUCCESS) {
        return res;
    }

    sprintf(path, "%s%s", TS_PROGRAM_BIN_PATH, TS_PROGRAM_NAME);

    if(mode == TS_IS_LOCAL) {
	log_debug("Local Mode");
        strcpy (inner_child_argv[0], "tshome");
        inner_child_argv[1] = NULL;
        return EXIT_SUCCESS;
    }
	
    res = db_get_base_user_info(&base_info);
    if(res != EXIT_SUCCESS) {
        return res;
    }

    log_debug("Remote Mode\n");

    strcpy (outer_child_argv[0], "tshome-host");
    strcpy(outer_child_argv[1], "-t");
    memset (param, 0, 256);
    sprintf(param, "%s:%d", base_info.server_name, base_info.server_port);
    strcpy(outer_child_argv[2], param);
    strcpy(outer_child_argv[3], "-u");
    memset (param, 0, 256);
    sprintf(param, "%s", base_info.login_user_name);
    strcpy(outer_child_argv[4], param);
    strcpy(outer_child_argv[5], "-p");
    memset (param, 0, 256);
    sprintf(param, "%s", base_info.login_passwd);
    strcpy(outer_child_argv[6], param);
    outer_child_argv[7] = NULL;

    return EXIT_SUCCESS;
}


int main(int argc, char *argv[])
{
    int ret, i, status;
    pid_t pid;
    char child_path[256] = {0};
    int exit_flag = 0;

    struct sigaction act;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGTERM, &act, 0);

    if (argc != 1)
    {
        log_err( "Usage:%s\n", argv[0]);
        return -1;
    }

    //outer mode args init
    for (i = 0; i < OUTER_MAX_ARGS - 1; i++) {
        outer_child_argv[i] = (char *)malloc(MAX_STR_LEN);
        if (NULL == outer_child_argv[i]) {
            int j;
            for (j = 0; j < i; j++)
                free(outer_child_argv[i]);
            return -1;
        }
        memset (outer_child_argv[i], 0, MAX_STR_LEN);
    }
    //inner mode args init
    for (i = 0; i < INNER_MAX_ARGS - 1; i++) {
        inner_child_argv[i] = (char *)malloc(MAX_STR_LEN);
        if (NULL == inner_child_argv[i]) {
            int j;
            for (j = 0; j < i; j++)
                free(inner_child_argv[i]);
            return -1;
        }
        memset (inner_child_argv[i], 0, MAX_STR_LEN);
    }

    while(1) {
        ret = get_child_process_str(child_path);

        if (ret != EXIT_SUCCESS) {
            printf ("Packaging child arguments failed\n");
            return -1;
        }		

        pid = fork();
        if (pid == -1) {
            log_err( "fork() error.errno:%d error:%s\n", errno, strerror(errno));
            break;
        }

        if (pid == 0) {
            sleep(12);
            if (TS_IS_LOCAL == mode)
                ret = execv(child_path, (char **) inner_child_argv);
            else
                ret = execv(child_path, (char **) outer_child_argv);
            log_err( "host_monitor execv ret:%d errno:%d error:%s\n", ret, errno, strerror(errno));
            exit(1);
        }

        if (pid > 0) {
            pid = wait(&status);
            exit_flag = WIFEXITED(status);

            // If the sub process terminated normally.
            if(exit_flag) {
            	log_debug("host_monitor the sub process terminated normally!\n");
            	log_debug("status=%d, sub_exit_flag=%d, sub_exit_value=%d\n", status, exit_flag, WEXITSTATUS(status));
            } else {
	        log_debug("host monitor the sub process doesn't terminated normally, sub_status=%d!\n", status);
            }
        }
    }

    for (i = 0; i < OUTER_MAX_ARGS - 1; i++) {
        free(outer_child_argv[i]);
    }
    for (i = 0; i < INNER_MAX_ARGS - 1; i++) {
        free(inner_child_argv[i]);
    }
    return 0;
}
