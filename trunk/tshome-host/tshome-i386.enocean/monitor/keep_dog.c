#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "keep_dog.h"

#define WDOG                    1
#define ON                      1
#define OFF                     0


int fd_led;  
void wdi_commu(void)
{
    while(1)
    {
        ioctl(fd_led,ON,WDOG); 
        usleep(1000);                        //..1ms
        ioctl(fd_led,OFF,WDOG); 
	usleep(500000);           
    }
}


void start_keep_dog(void)
//void main(void)
{
    pthread_t ptid_wdi;
    pthread_attr_t attr;

    fd_led = open("/dev/IR_Led",0);
    if(fd_led < 0) {
        perror("can't open device IR_Led");
    }

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
  	  
    pthread_create(&ptid_wdi, &attr,  (void*)&wdi_commu, NULL);
    fd_led = open("/dev/IR_Led",0);
    if(fd_led < 0) {
        perror("can't open device IR_Led");
    }

//    while(1);
}



