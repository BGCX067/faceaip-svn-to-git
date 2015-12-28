//update  vim /etc/rsyslog.conf
//set :  *.*;auth,authpriv.none      -/var/log/syslog

#ifndef __TS_LOG_TOOLS_H__
#define __TS_LOG_TOOLS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>

#define TS_LOG_MAX_SIZE 4000

#define TS_PRINT

#ifdef __cplusplus
extern "C"
{
#endif

void log_open(char *pro_name);

void log_close();

//void log_write(int put_type,int priority, const char * buff);

//void log_write_va(int put_type,int priority, char * format,...);


void log_emerg(char *format, ...);

void log_alert(char *format, ...);

void log_crit(char *format, ...);

void log_err(char *format, ...);

void log_warning(char *format, ...);

void log_notice(char *format, ...);

void log_info(char *format, ...);

void log_debug(char *format, ...);

void log_debug_web(char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
