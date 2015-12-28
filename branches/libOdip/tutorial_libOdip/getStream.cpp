/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * getStream.cpp - _explain_
 *
 * Copyright (C) 2012 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: getStream.cpp 0001 2012-04-06 09:58:43Z WuJunjie $
 *
 *  Explain:
 *     - 登录、获取视频的例子 -
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*sleep*/

#include "nvrnetapi.h"


void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void* dwUser)
{
    __fline;
    printf(" g_RealDataCallBack_V30 Get data at :%ld, the size is:%d.\n", time(NULL), dwBufSize);
}

void CALLBACK g_DataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser)
{
    __fline;
    printf(" g_DataCallBack data,the size is %d.\n", dwBufSize);
}

void CALLBACK g_StdDataCallBack(int lRealHandle, unsigned int dwDataType, unsigned char *pBuffer, unsigned int dwBufSize, unsigned int dwUser)
{
    __fline;
    printf(" g_StdDataCallBackGet data,the size is %d.\n", dwBufSize);
}

int main(int argc, char *argv[])
{
    NET_DVR_Init();

    //设置连接时间，与重连时间
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);

    long lUserID = -1;
    //login
    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    //lUserID = NET_DVR_Login_V30((char *)"faceaip.3322.org", 9056, (char *)"a", (char *)"a", &struDeviceInfo);
    lUserID = NET_DVR_Login_V30((char *)"192.168.10.246", 9056, (char *)"a", (char *)"a", &struDeviceInfo);
    if (lUserID < 0)
    {
        __fline;
        printf("Login error, %d\n", NET_DVR_GetLastError());
        return -1;
    }
    else
    {
        __fline;
        printf("Login Succeeded\n");
    }

    //NET_DVR_SetExceptionCallBack_V30(0, NULL,g_ExceptionCallBack, NULL);

    long lRealPlayHandle;
    NET_DVR_CLIENTINFO ClientInfo =
        {
            0
        };

#ifdef _WIN32

    Sleep(3000);  //millisecond
#elif  defined(__linux__)

    //while(1)
    {

        ClientInfo.lChannel     = 1;  //channel NO. 0-n
        ClientInfo.lLinkMode    = 0;
        ClientInfo.sMultiCastIP = NULL;

        lRealPlayHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo, g_RealDataCallBack_V30, NULL, 0);
        if (lRealPlayHandle < 0)
        {
            __fline;
            printf("NET_DVR_RealPlay_V30 error:%ld\n", lRealPlayHandle);
            sleep(1);
            //continue;
            return -1;
        }
        printf("NET_DVR_RealPlay_V30  ok, waite 10s\n");

        sleep(10);    //second


        NET_DVR_StopRealPlay(lRealPlayHandle);
        printf("NET_DVR_StopRealPlay  ok, waite 5s\n");

        sleep(5);    //second
    }
#endif

    printf("NET_DVR_StopRealPlay .\n");

    //stop
    NET_DVR_StopRealPlay(lRealPlayHandle);
    NET_DVR_Logout_V30(lUserID);
    NET_DVR_Cleanup();

    return 0;
}
