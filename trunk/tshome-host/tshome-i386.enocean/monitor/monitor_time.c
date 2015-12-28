#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SLEEP_TIME 30000000

int main(int argc,char *argv[]) 
{
	int ret  = EXIT_SUCCESS;

	do {
		system("ntpdate -s -u 192.168.10.3 192.43.244.18 210.72.145.44 210.0.235.14 59.124.196.8");
		usleep(SLEEP_TIME);
	}while(1);

	return ret;
}

