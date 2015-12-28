
#include "ts_log_tools.h"

#define TS_PUT_TYPE_STDOUT 0
#define TS_PUT_TYPE_STDERR 1

//cp va_list to a string
#define set_str(buffer,len) \
	va_list v_arg_list; \
	va_start(v_arg_list, format); \
	vsnprintf(buffer, len, format, v_arg_list); \
	va_end(v_arg_list);\

static const char * ts_log_level[] = {
"emerg:",  /* system is unusable */
"alert:",   /* action must be taken immediately */
"crit:",   /* critical conditions */
"err:",   /* error conditions */
"warning:",   /* warning conditions */
"notice:",   /* normal but significant condition */
"info:",   /* informational */
"debug:",   /* debug-level messages */
};

void log_open(char *pro_name) {
	if((NULL != pro_name) && (strlen(pro_name) > 0)) {
		openlog(pro_name,LOG_PID,LOG_USER);
	} 
}

void log_close() {
	closelog();
}

static void log_write(int put_type,int priority, const char * buffer) {
	//print it to screen
#ifdef TS_PRINT
	if(TS_PUT_TYPE_STDOUT == put_type) {
		printf("%s",buffer);
	} else {
		fprintf(stderr,"%s",buffer);
	}
#endif
	//Write to syslog
	char buff[TS_LOG_MAX_SIZE] = {0};
	snprintf(buff,sizeof(buff),"%s%s",ts_log_level[priority],buffer);
	syslog(priority,"%s",buff);
}

static void log_write_va(int put_type,int priority, char * format,...) {
	char buffer	[TS_LOG_MAX_SIZE] = {0};
	set_str(buffer,sizeof(buffer));
	log_write(put_type,priority,buffer);
}

void log_debug(char *format, ...) {
	char buffer	[TS_LOG_MAX_SIZE] = {0};
	set_str(buffer,sizeof(buffer));
	log_write(TS_PUT_TYPE_STDOUT,LOG_DEBUG,buffer);
}

void log_debug_web(char *format, ...) {
	char buffer	[TS_LOG_MAX_SIZE] = {0};
	set_str(buffer,sizeof(buffer));
	log_write(TS_PUT_TYPE_STDERR,LOG_DEBUG,buffer);
}

void log_emerg(char *format, ...) {
	char buffer	[TS_LOG_MAX_SIZE] = {0};
	set_str(buffer,sizeof(buffer));
	log_write(TS_PUT_TYPE_STDOUT,LOG_EMERG,buffer);
}

void log_alert(char *format, ...) {
	char buffer	[TS_LOG_MAX_SIZE] = {0};
	set_str(buffer,sizeof(buffer));
	log_write(TS_PUT_TYPE_STDOUT,LOG_ALERT,buffer);
}

void log_crit(char *format, ...) {
	char buffer	[TS_LOG_MAX_SIZE] = {0};
	set_str(buffer,sizeof(buffer));
	log_write(TS_PUT_TYPE_STDOUT,LOG_CRIT,buffer);
}

void log_err(char *format, ...) {
	char buffer	[TS_LOG_MAX_SIZE] = {0};
	set_str(buffer,sizeof(buffer));
	log_write(TS_PUT_TYPE_STDOUT,LOG_ERR,buffer);
}

void log_warning(char *format, ...) {
	char buffer	[TS_LOG_MAX_SIZE] = {0};
	set_str(buffer,sizeof(buffer));
	log_write(TS_PUT_TYPE_STDOUT,LOG_WARNING,buffer);
}

void log_notice(char *format, ...) {
	char buffer	[TS_LOG_MAX_SIZE] = {0};
	set_str(buffer,sizeof(buffer));
	log_write(TS_PUT_TYPE_STDOUT,LOG_NOTICE,buffer);
}

void log_info(char *format, ...) {
	char buffer	[TS_LOG_MAX_SIZE] = {0};
	set_str(buffer,sizeof(buffer));
	log_write(TS_PUT_TYPE_STDOUT,LOG_INFO,buffer);
}

