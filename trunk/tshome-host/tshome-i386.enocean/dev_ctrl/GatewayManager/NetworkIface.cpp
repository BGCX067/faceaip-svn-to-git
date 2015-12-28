//
//  NetworkIface.cpp
//  tsbus_knx
//
//  Created by kyy on 11-5-23.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#include "NetworkIface.h"

#include <ifaddrs.h>
#include <string.h>

#ifdef _WIN32
	// Get Local Ip Address
	int _GetLocalIp(char *ip)
	{
		char host_name[255];
		// get localhost name
		if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)
		{
			printf("Error %d when getting local host name\n", WSAGetLastError());
			return 1;
		}
		//printf("Host name is: %s\n", host_name);
		struct hostent *phe = gethostbyname(host_name);
		if (phe == 0)
		{
			printf("Yow! Bad host lookup.");
			return 1;
		}

		// get all of the ip.
		for (int i = 0; phe->h_addr_list[i] != 0; ++i)
		{
			struct in_addr addr;
			memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));

			strcpy(ip, inet_ntoa(addr));
		}
		return 0;
	}

#else

	// Get Local Ip Address
	void _GetLocalIp(char *ip)
	{
		struct ifaddrs * ifAddrStruct=NULL;
		struct ifaddrs * ifa=NULL;
		void * tmpAddrPtr=NULL;

		getifaddrs(&ifAddrStruct);

		for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
			if (ifa ->ifa_addr->sa_family == AF_INET) {
				// check it is IP4
				// is a valid IP4 Address
				tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
				//char addressBuffer[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);
				if(strcmp(ifa->ifa_name, "en1") == 0)
				{
					break;
					//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
				}
			} else if (ifa->ifa_addr->sa_family == AF_INET6) {
				// check it is IP6
				// is a valid IP6 Address
				tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
				char addressBuffer[INET6_ADDRSTRLEN];
				inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
				//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
			}
		}
		if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
	}
#endif
