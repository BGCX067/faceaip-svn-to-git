
#include<stddef.h>
#include<unistd.h>
#include"pthread.h"
#include <termios.h>
#include <stdlib.h>
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/ioctl.h"
#include "stdlib.h"
#include "termios.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "sys/time.h"
#include "errno.h"
#include "math.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "string.h"

int baud_rate[7] = {B2400, B4800, B9600, B19200, B38400, B57600, B115200};
int fd_232_0,fd_232_1,fd_232_2,fd_232_3, fd_ir;
fd_set read232;

void settty(int fd, unsigned char val)
{
	  struct termios opt;
	  tcgetattr(fd, &opt);
	  char baudrate, parity, stopbit,databit;
	  baudrate = val & 0x07;
	  parity   = (val >> 3) & 0x03;
	  databit  = (val >> 5) & 0x03;
	  stopbit  = val >> 7;
	  opt.c_cflag |= (CLOCAL | CREAD);
	  opt.c_cflag &= ~CRTSCTS;                 // 无硬件流控制 
	  if(0)//(stopbit)
	  {
		   opt.c_cflag |= CSTOPB;                // 2 位停止位
	  }
	  else
	  {
		   opt.c_cflag &= ~CSTOPB;                // 1 位停止位
	  }
	  opt.c_cflag &= ~CSIZE;
    switch(3)//(databit)
    {
		   case 0: opt.c_cflag |= CS5;break;
		   case 1: opt.c_cflag |= CS6;break;
		   case 2: opt.c_cflag |= CS7;break;
		   case 3: opt.c_cflag |= CS8;break;
		   default : break;
	  }
	  switch(0)//(parity)
	  {
		    case 0:                               //无奇偶校验位
		    {
			      opt.c_cflag &= ~PARENB;
			      break;
		    }
		    case 1:                              // 奇校验
		    {
			     opt.c_cflag |= PARENB;
			     opt.c_cflag |= PARODD;
		    	 opt.c_iflag |= (INPCK | ISTRIP);
			     break;
		    }
		    case 2:                             // 偶校验
		    {
			     opt.c_iflag |= (INPCK | ISTRIP);
			     opt.c_cflag |= PARENB;
			     opt.c_cflag |= PARODD;
			     break;
		    }
		    default: break;
    }
    opt.c_iflag&=~(IXON|IXOFF|IXANY);
    opt.c_iflag &= ~IGNPAR;
    opt.c_iflag &= ~(ICRNL|IGNCR);
    opt.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
    opt.c_oflag &= ~(ONLCR | OCRNL);
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
    opt.c_oflag &= ~OPOST; /*Output*/
    opt.c_cc[VTIME] = 0;
    opt.c_cc[VMIN]  = 0;
    cfsetispeed(&opt, baud_rate[2]);
    cfsetospeed(&opt, baud_rate[2]);
    tcflush(fd, TCIOFLUSH);
    //fcntl(fd, F_SETFL, 0);
    tcsetattr(fd, TCSANOW, &opt);
}


int main(void)
{
      unsigned char rec_buffer[32];
      
      int ret,i;
     	struct timeval tv;  
    
  	  fd_232_0 = open("/dev/ttySAC0",O_RDWR | O_NOCTTY | O_NDELAY);
  	  fd_232_1 = open("/dev/ttySAC1",O_RDWR | O_NOCTTY | O_NDELAY);
  	  fd_232_2 = open("/dev/ttySAC2",O_RDWR | O_NOCTTY | O_NDELAY);
  	  fd_232_3 = open("/dev/ttySAC3",O_RDWR | O_NOCTTY | O_NDELAY);
	  fd_ir    = open("/dev/IR_driver",O_RDWR);
  
  	  if(fd_232_0 < 0)
      {
  	  	perror("can't open device ttyS0\n");
  	  }
	  else if(fd_232_1 < 0)
      {
		  perror("can't open device ttyS1\n");
	  }
	  else if(fd_232_2 < 0)
      {
		  perror("can't open device ttyS2\n");
	  }
	  else if(fd_232_3 < 0)
      {
		  perror("can't open device ttyS3\n");
  	  }

  	  settty(fd_232_0,2);
  	  settty(fd_232_1,2);
  	  settty(fd_232_2,2);
  	  settty(fd_232_3,2);
  	  printf("\n RS232 communication \n");
  	  for(i=0;i<32;i++);
  	      rec_buffer[i] = i;
      
	   while(1)
	   {		   
	   	printf("\n send com 0\n");
	   	write(fd_232_0, rec_buffer, 32); 
	   	sleep(5);
		
	   	printf("\n send com 1\n");
	   	write(fd_232_1, rec_buffer, 32); 
	   	sleep(5);
		
	   	printf("\n send com 2\n");
	   	write(fd_232_2, rec_buffer, 32); 
	   	sleep(5);
		
	   	printf("\n send com 3\n");
	   	write(fd_232_3, rec_buffer, 32); 
	   	sleep(5);
	   	
	   	  	   
/*
	  
	  
	  settty(fd_232_1, 0x62);                                    //默认设置:B9600, 无奇偶检验
	  
	  FD_ZERO(&read232);
	  FD_SET(fd_232, &read232);

	  	
	  	
    tv.tv_sec = 0;                                          // timeout = 10Us
	  tv.tv_usec = 10;
	  while(1)
	  {
		    ret = 0;
		    FD_ZERO(&read232);
	      
		    ret = select(read232 + 1, &read232, NULL,NULL,&tv);
		    if(ret > 0)
		    {
		    	  if(FD_ISSET(fd_232_1, &read232))
		    	  {
		    	  	  usleep(25000);
				        read(fd_232_1, rec_buffer, 32);	
				        printf("\n ********************************** \n");
				        for(i=0; i<32; i++)
				        {
				    	     printf("ttyS0[%d]=%d  ,", i,rec_buffer[i]);
				    	     if(i%4 == 0)
				    		   printf("\n");
				        }
				        write(fd_232_1, rec_buffer, 32);   //把收到的回传给发送方
                WriteDataBuffer(&t_buffer_232, rec_buffer, 32);
		        }
			      else							
			          usleep(50000);
	  		
     }
     */
     
}

}






















































