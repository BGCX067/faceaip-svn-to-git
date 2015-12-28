/*
**	********************************************************************************
**                                    uart
**                           TRYON Software Project
**
**        (c) Copyright 2007-2010, Hangzhou TRYON Technology Co.Ltd.
**                              All Rights Reserved
**					         http://www.qiyangtech.com
**
**	File  Name   : uart.c
**	Description  : 
**	Modification : 2010/04/20		LZQ		Create the file
**	********************************************************************************
*/

#include "DVRDEF.H"
#include "DVRAPI.H"
#include "dvr_dev.h"
#include "dvr_ver.h"
#include "dvr_inc.h"
#include "uart.h"

#ifndef _DEBUG_THIS
//	#define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#define SERIAL232_INDEPENDENT		//LZQ， 独立RS232, 不与控制台复用

static int UartDeviceFd[UART_DEVICE_MAX]= {-1, -1, -1, -1/*stdout*/};
static int console_device_fd = -1;

int Uart_Device_reverse    = UART_DEVICE_0;
int Uart_Device_serial485  = UART_DEVICE_1;
int Uart_Device_frontboard = UART_DEVICE_2;
int Uart_Device_serial232  = UART_DEVICE_3;

extern struct termios OldOption;

#if 0
int UartDevGetStatus(UART_DEV_INFO_S *UartInfo)
{
	unsigned int ProductType = get_dvr_hwinfo(PRODUCT_TYPE);
	
	switch (ProductType){
		case PROD_D7004:
		case PROD_D7008:
			UartInfo->frontboard = 0;
			UartInfo->serial232 = 0;
			UartInfo->serial485 = 0;
			break;
		/*
		case PROD_D7016:
			UartInfo->frontboard = 0;
			UartInfo->serial232 = 0;
			UartInfo->serial485 = 1;	
			break;
		*/
		default:
			UartInfo->frontboard = 1;
			UartInfo->serial232 = 1;
			UartInfo->serial485 = 1;
	}

	return 0;
}
#endif
//供前面板，串口，云台设备调用
int UartDeviceCreate(UartDevice_e Index)
{
	if (Index == Uart_Device_serial232)
	{
#ifdef SERIAL232_INDEPENDENT 

		COMM_ATTR UartAttribute;
		
		if (UartDeviceOpen(Index) < 0)
		{
			return -1;
		}
		UartAttribute.baudrate = 115200 ;
		UartAttribute.databits = 8;
		UartAttribute.parity = commNoParity;
		UartAttribute.stopbits = commOneStopBit;
		
		if (UartSetAttribute(Index, &UartAttribute) < 0)
		{
			return -1;
		}
#else
		if (console_device_fd > 0)
		{
			return 0;
		}
		
		if ((console_device_fd = open(CONSOLE_DEVICE, O_RDWR)) < 0)
		{
			return -1;
		}
#endif
	}
	else if(Index == Uart_Device_frontboard)
	{
		COMM_ATTR UartAttribute;
	
		if (UartDeviceOpen(Index) < 0)
		{
			return -1;
		}

#ifdef _D6008_FRONTBOARD
		UartAttribute.baudrate = 4800 ;
#else
		UartAttribute.baudrate = 9600 ;
#endif
		UartAttribute.databits = 8;
		UartAttribute.parity = commNoParity;
		UartAttribute.stopbits = commOneStopBit;
	
		if (UartSetAttribute(Index, &UartAttribute) < 0)
		{
			return -1;
		}
	
	}
	else if(Index == Uart_Device_serial485)
	{
		COMM_ATTR UartAttribute;
	
		if (UartDeviceOpen(Index) < 0)

		{
			return -1;
		}
		UartAttribute.baudrate = 115200 ;
		//UartAttribute.baudrate = 9600 ;
		UartAttribute.databits = 8;
		UartAttribute.parity = commNoParity;
		UartAttribute.stopbits = commOneStopBit;

		if (UartSetAttribute(Index, &UartAttribute) < 0)
		{
			return -1;
		}
		
	}
	else if(Index == Uart_Device_reverse)
	{
		
	}
	else
	{
		printf("UartDeviceCreate failed\n");
		return -1;
	}
	
	return 0;
}

int UartDeviceOpen(UartDevice_e Index)
{
	char DeviceName[NAME_MAX_LENGTH];

	if (UartDeviceFd[Index] > 0)
	{
		return 0;
	}
	
	if (Index >= UART_DEVICE_MAX || Index < 0 )
	{
		printf("Open %s failed\n", DeviceName);
		return -1;
	}
	
	sprintf(DeviceName, "%s%d", UART_DEVICE_HEAD, Index);

	if (Index == Uart_Device_serial485)
	{
		UartDeviceFd[Index] = open(DeviceName, O_RDWR | O_SYNC);
	}
	else
	{
		UartDeviceFd[Index] = open(DeviceName, O_RDWR);
	}
	if (UartDeviceFd[Index] < 0)
	{
		printf("Open %s failed\n", DeviceName);
		return (UartDeviceFd[Index] = -1);
	}

#ifdef SERIAL232_INDEPENDENT
#else
	if (Index == Uart_Device_serial232)
	{
		if(ioctl(console_device_fd , CONSOLE_SUSPEND, NULL) < 0)
		{
			printf("2CONSOLE_SUSPEND failed\n");
			return -1;
		}
	}	
#endif		

	return 0;
}

int UartDeviceDestory(UartDevice_e Index)
{
	if (Index >= UART_DEVICE_MAX || Index < 0)
	{
		printf("Index out of range\n");
		return -1;
	}
	return UartDeviceClose(Index);
}

int UartDeviceClose(UartDevice_e Index)
{
	if (Index >= UART_DEVICE_MAX || Index < 0)
	{
		printf("Index out of range\n");
		return -1;
	}
#ifdef SERIAL232_INDEPENDENT
#else
	if (Index == Uart_Device_serial232)
	{
		if(ioctl(console_device_fd, CONSOLE_RESUME, NULL) < 0)
		{
			printf("1CONSOLE_RESUME failed\n");
			return -1;
		}
	}
#endif	
	if (UartDeviceFd[Index] > 0)
	{
		close(UartDeviceFd[Index]);
		UartDeviceFd[Index] = -1;
	}
	
	
	return 0;
}



int UartDeviceRead(UartDevice_e Index, void *Buffer, DWORD Length)
{

	int Readlength;
	int ReadPosition = 0;
	
	if (Buffer == NULL || Length <= 0 || Index < 0 || Index >= UART_DEVICE_MAX)
	{
		printf("UartDeviceRead : Input params invailed\n");
		return -1;
	}
	
	while(Length)
	{
		Readlength = read(UartDeviceFd[Index], Buffer + ReadPosition, Length);
		//printf("###readlength = %d, buff = %x\n", Readlength, *((char *)(Buffer + ReadPosition)));
		if (Readlength < 0)
		{
			printf("UartDeviceRead : readlength < 0\n");
			return -1;
		}
		
		ReadPosition += Readlength;
		Length -= Readlength;
	
	}

	DBG( printf("UartDeviceRead : readlength :%d\n", ReadPosition); );
	return ReadPosition;
}

inline int UartDeviceReadNoblock(UartDevice_e Index, void *Buffer, DWORD Length)
{

	int Readlength = 0;
	int ret;
	struct timeval TimeOut;	
	fd_set Mask;
	int TryTimes = 3;
	int TotalLength = Length;// > 4 ? 4 : Length;
	
	while(Readlength != TotalLength)
	{
		TimeOut.tv_sec  = 0;
		TimeOut.tv_usec = 90000;//wait for 90ms
		FD_ZERO(&Mask);
		FD_SET(UartDeviceFd[Index], &Mask);

		ret = select(UartDeviceFd[Index] + 1, &Mask, NULL, NULL, &TimeOut);
		if(ret <= 0)
		{
			if (--TryTimes == 0)
			{
				printf("UartDeviceReadNoblock timeout\n");
				return -1;
			}
			continue;
		}

		ret = read(UartDeviceFd[Index], Buffer + Readlength, TotalLength - Readlength);
		//printf("###readlength = %d, buff = %x\n", Readlength, *((char *)(Buffer + ReadPosition)));
		if (ret < 0)
		{
			printf("UartDeviceRead : readlength < 0\n");
			return -1;
		}
		Readlength += ret;
	}
	return Readlength;
}

int UartDeviceWrite(UartDevice_e Index, void *Buffer, DWORD Length)
{

	int Writelength;
	int WritePosition = 0;

	if (Buffer == NULL || Length <= 0 || Index < 0 || Index >= UART_DEVICE_MAX)
	{
		printf("UartDeviceWrite : Input params invailed\n");
		return -1;
	}
	
	while(Length)
	{
		Writelength = write(UartDeviceFd[Index], Buffer + WritePosition, Length);
		
		if (Writelength < 0)
		{
			printf("UartDeviceWrite : writelength < 0, Index = %d, Length = %d\n", Index, Length);
			return -1;
		}
		
		WritePosition += Writelength;
		Length -= Writelength;
	}

DBG(	printf("UartDeviceWrite : writelength :%d\n", WritePosition); );
	return WritePosition;
}


int UartDeviceWriteNoblock(UartDevice_e Index, void *Buffer, DWORD Length)
{
	
	int Writelength;

	if (Buffer == NULL || Length <= 0 || Index < 0 || Index >= UART_DEVICE_MAX)
	{
		printf("UartDeviceWrite : Input params invailed\n");
		return -1;
	}
	
	Writelength = write(UartDeviceFd[Index], Buffer, Length);
	if (Writelength < 0)
	{
		printf("UartDeviceWrite : writelength < 0, Index = %d, Length = %d\n", Index, Length);
		return -1;
	}

	return Writelength;
}

int UartDevicePurge(UartDevice_e Index, DWORD Flag)
{
	return 0;
}

int UartSetAttribute(UartDevice_e Index, COMM_ATTR *ParmaAttribute_p)
{

	struct termios Option;
	COMM_ATTR *Attribute_p = ParmaAttribute_p;
//	printf("###############UartSetAttribute Index = %d, UartDeviceFd[Index] = %d\n", Index, UartDeviceFd[Index]);
	memset(&Option, 0, sizeof(struct termios));
	tcgetattr(UartDeviceFd[Index], &Option);
//	sleep(2);
	if (Index >= UART_DEVICE_MAX || Index < 0 || UartDeviceFd[Index] < 0)
	{
		printf("UartSetAttribute %d failed\n", Index);
		return -1;
	}

	if (Index != Uart_Device_reverse)
	{
		cfmakeraw(&Option);
	}
	else
	{
		Option = OldOption;
	}
	//set speed
	switch (Attribute_p->baudrate)
	{
		case 50:
			cfsetispeed(&Option, B50);
			cfsetospeed(&Option, B50);
			break;
		case 75:
			cfsetispeed(&Option, B75);
			cfsetospeed(&Option, B75);
			break;
		case 110:
			cfsetispeed(&Option, B110);
			cfsetospeed(&Option, B110);
			break;
		case 134:
			cfsetispeed(&Option, B134);
			cfsetospeed(&Option, B134);
			break;
		case 150:
			cfsetispeed(&Option, B150);
			cfsetospeed(&Option, B150);
			break;
		case 200:
			cfsetispeed(&Option, B200);
			cfsetospeed(&Option, B200);
			break;
		case 300:
			cfsetispeed(&Option, B300);
			cfsetospeed(&Option, B300);
			break;
		case 600:
			cfsetispeed(&Option, B600);
			cfsetospeed(&Option, B600);
			break;
		case 1200:
			cfsetispeed(&Option, B1200);
			cfsetospeed(&Option, B1200);
			break;
		case 1800:
			cfsetispeed(&Option, B1800);
			cfsetospeed(&Option, B1800);
			break;
		case 2400:
			cfsetispeed(&Option, B2400);
			cfsetospeed(&Option, B2400);
			break;
		case 4800:
			cfsetispeed(&Option, B4800);
			cfsetospeed(&Option, B4800);
			break;
		case 9600:
			cfsetispeed(&Option, B9600);
			cfsetospeed(&Option, B9600);
			break;
		case 19200:
			cfsetispeed(&Option, B19200);
			cfsetospeed(&Option, B19200);
			break;
		case 38400:
			cfsetispeed(&Option, B38400);
			cfsetospeed(&Option, B38400);
			break;
		case 57600:
			cfsetispeed(&Option, B57600);
			cfsetospeed(&Option, B57600);
			break;
		case 115200:
			cfsetispeed(&Option, B115200);
			cfsetospeed(&Option, B115200);
			break;
#if 0
		case 230400:
			cfsetispeed(&opt, B230400);
			cfsetospeed(&opt, B230400);
			break;
		case 460800:
			cfsetispeed(&opt, B460800);
			cfsetospeed(&opt, B460800);
			break;
		case 500000:
			cfsetispeed(&opt, B500000);
			cfsetospeed(&opt, B500000);
			break;
		case 576000:
			cfsetispeed(&opt, B576000);
			cfsetospeed(&opt, B576000);
			break;
		case 921600:
			cfsetispeed(&opt, B921600);
			cfsetospeed(&opt, B921600);
			break;
		case 1000000:
			cfsetispeed(&opt, B1000000);
			cfsetospeed(&opt, B1000000);
			break;
		case 1152000:
			cfsetispeed(&opt, B1152000);
			cfsetospeed(&opt, B1152000);
			break;
		case 1500000:
			cfsetispeed(&opt, B1500000);
			cfsetospeed(&opt, B1500000);
			break;
		case 2000000:
			cfsetispeed(&opt, B2000000);
			cfsetospeed(&opt, B2000000);
			break;
		case 2500000:
			cfsetispeed(&opt, B2500000);
			cfsetospeed(&opt, B2500000);
			break;
		case 3000000:
			cfsetispeed(&opt, B3000000);
			cfsetospeed(&opt, B3000000);
			break;
		case 3500000:
			cfsetispeed(&opt, B3500000);
			cfsetospeed(&opt, B3500000);
			break;
		case 4000000:
			cfsetispeed(&opt, B4000000);
			cfsetospeed(&opt, B4000000);
			break;
#endif
		default:
			printf("Unsupported baudrate %d\n", Attribute_p->baudrate);
			return -1;
	}

	//set parity
//	printf("Attribute_p->parity = %d\n", Attribute_p->parity);
	switch (Attribute_p->parity)
	{
		case commNoParity:				// none			
			Option.c_cflag &= ~PARENB;	// disable parity	
			Option.c_iflag &= ~INPCK;	// disable parity check	
			break;
		case commOddParity:			// odd			
			Option.c_cflag |= PARENB;	// enable parity	
			Option.c_cflag |= PARODD;	// odd			
			Option.c_iflag |= INPCK;	// enable parity check	
//			Option.c_iflag |= (PARODD | PARENB);
			break;
		case commEvenParity:			// even			
			Option.c_cflag |= PARENB;	// enable parity
			Option.c_cflag &= ~PARODD;	// even			
			Option.c_iflag |= INPCK;	// enable parity check	
//			Option.c_cflag &= ~ PARENB;
//			Option.c_cflag &= ~PARODD;
			break;
		case commMarkParity:
			Option.c_cflag |= PARENB;	/* enable parity	*/
			Option.c_cflag |= PARODD;	/* parity bit is always 1	*/
			break;
		case commSpaceParity:
			Option.c_cflag |= PARENB;	/* enable parity	*/
			break;
		default:
			printf("Unsupported parity %d\n", Attribute_p->parity);
			return -1;
	}

	//set data bits
	Option.c_cflag &= ~CSIZE;
	switch (Attribute_p->databits)
	{
		case 5:
			Option.c_cflag |= CS5;
			break;
		case 6:
			Option.c_cflag |= CS6;
			break;
		case 7:
			Option.c_cflag |= CS7;
			break;
		case 8:
			Option.c_cflag |= CS8;
			break;
		default:
			printf("Unsupported data bits %d\n", Attribute_p->databits);
			return -1;
	}

	//set stop bits
	Option.c_cflag &= ~CSTOPB;
	switch (Attribute_p->stopbits)
	{
		case commOneStopBit:
			Option.c_cflag &= ~CSTOPB;
			break;
		case commOne5StopBits:
			break;

		case commTwoStopBits:
			Option.c_cflag |= CSTOPB;
			break;
		default:
			printf("Unsupported stop bits %d\n", Attribute_p->stopbits);
			return -1;
	}
	Option.c_cc[VTIME] = 0;
	Option.c_cc[VMIN]	= 1;			// block until data arrive 
	
	tcflush(UartDeviceFd[Index], TCIOFLUSH);
	if (tcsetattr(UartDeviceFd[Index], TCSANOW, &Option) < 0)
	{
		printf("Tcsetattr\n");
		return -1;
	}
	
	return 0;
}

//原始工作模式
//opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
//opt.c_oflag &= ~OPOST;
//tcsetattr(fd, TCSANOW, &opt);


void UartSetFd(int Index, int Fd)
{
	UartDeviceFd[Index]  = Fd;
}
