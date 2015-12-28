/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ctrlRemotRecord.cpp - _explain_
 *
 * Copyright (C) 2012 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: ctrlRemotRecord.cpp 0001 2013-01-07 21:58:43Z WuJunjie $
 *
 *  Explain:
 *     - 控制录像的例子 -
 *
 *  Update:
 *     2013-01-07 21:58:43 WuJunjie Create
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

    //Set callback function of getting stream.
    long lRealPlayHandle;
    NET_DVR_CLIENTINFO ClientInfo =
        {
            0
        };

    ClientInfo.lChannel     = 1;  //channel NO. 0-n
    ClientInfo.lLinkMode    = 0;
    ClientInfo.sMultiCastIP = NULL;


#ifdef _WIN32
    Sleep(3000);  //millisecond
#elif  defined(__linux__)

    unsigned int mskONOFF = 0x0f;
    BOOL bRet;

    while(1)
        //if(1)
    {
        printf("%ld -- NET_DVR_RemoteRecordCtrl 0x%0x \n", time(NULL), mskONOFF);

        bRet = NET_DVR_RemoteRecordCtrl(mskONOFF);

        if (bRet==TRUE)
            printf("%ld -- NET_DVR_RemoteRecordCtrl 0x%0x - OK\n", time(NULL), mskONOFF);
        else
            printf("%ld -- NET_DVR_RemoteRecordCtrl 0x%0x - FAILED\n", time(NULL), mskONOFF);


        sleep(10);

        if (mskONOFF==0)
            mskONOFF = 0x0f;
        else
            mskONOFF = 0;
    }
#endif

    printf("NET_DVR_StopRealPlay .\n");

    //stop
    NET_DVR_StopRealPlay(lRealPlayHandle);
    NET_DVR_Logout_V30(lUserID);
    NET_DVR_Cleanup();

    return 0;
}
