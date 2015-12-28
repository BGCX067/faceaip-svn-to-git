//update  vim /etc/rsyslog.conf
//set :  *.*;auth,authpriv.none      -/var/log/syslog
#include "ts_log_tools.h"

int main(int argc,char *argv[]) {
	int ret = EXIT_SUCCESS;

	log_open("panda");

	printf("abc-------------->\n");

	syslog(LOG_DEBUG,"panda fly!\n");

	if(argc>1) {
		log_debug("%s\n",argv[1]);
	}else {
		log_debug("\n");
	}

	log_close();

	return ret;
}
