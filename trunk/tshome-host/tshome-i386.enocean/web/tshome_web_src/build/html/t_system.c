#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	int ret = EXIT_SUCCESS;

	ret = system("./guide_down_conf.sh");
	printf("-------------------------------------------------->ret = %d\n",ret);

	syslog("-------------------------------------->ret = %d\n",ret);

	return ret;
}
