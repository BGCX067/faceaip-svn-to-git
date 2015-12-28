#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define TS_PING_COUNTS 5
int main() {
	int ret = EXIT_SUCCESS;
		
	char pings[TS_PING_COUNTS][64] = {"8.8.8.8","8.8.4.4","www.gov.cn","www.baidu.com","www.qq.com"};
	int i = 0;
	char cmd[128] = {0};

	for(i = 0; i < TS_PING_COUNTS; i++) {
		bzero(cmd,sizeof(cmd));
		sprintf(cmd,"ping %s -c 1",pings[i]);
		ret = system(cmd);
		if(EXIT_SUCCESS == ret) {
			break;
		}

	}

	return ret;
}
