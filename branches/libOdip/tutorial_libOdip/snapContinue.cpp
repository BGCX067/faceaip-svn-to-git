/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * snapContinue.cpp - _explain_
 *
 * Copyright (C) 2012 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: snapContinue.cpp 0001 2013-01-30 10:28:13Z WuJunjie $
 *
 *  Explain:
 *     - 
 *        登录、登出、登录、一直获取图片的例子
 *        登录后会 自动重连， 可以关闭dvr然后再开启，本程序会自动继续抓图
 *     -
 *
 *  Update:
 *     2013-01-30 10:28:15 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> /*sleep*/

#include "nvrnetapi.h"


void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void* dwUser)
{
    __fline;
    printf(" g_RealDataCallBack_V30 Get data at :%ld, the size is:%d.\n", time(NULL), dwBufSize);
}
// 保存为图片的例子
bool g_wait;
FILE *g_PicFile;
int g_iSuccessaFile;
void CALLBACK g_SnapCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void* dwUser)
{
    __fline;
    printf(" g_SnapCallBack Get data at :%ld, the size is:%d.\n", time(NULL), dwBufSize);

    if (dwBufSize==0)
    {
        if (g_PicFile)
        {
            fclose(g_PicFile);
            g_PicFile = NULL;
            printf("fclose ok.\n");
            g_iSuccessaFile = 1;
        }

        //g_wait = false;

        printf("Picture trans over.\n");
    }
    else //if (g_iSuccessaFile==0)
    {
#if 1//将图片保存到设定的路径下
        char strFileName[64];
        sprintf(strFileName, "%sch%d-%ld.jpg"
                , "./"
                , 0/*pDvrip->dvrip.dvrip_p[0]*/
                , time(NULL));//设定路径与文件名
        if (g_PicFile == NULL)
        {
            g_PicFile = fopen(strFileName, "rb");
            if (g_PicFile)
            {
                fclose(g_PicFile);
                g_PicFile = NULL;
                strcat(strFileName, ".rubbish");
            }
            else
            {
                g_PicFile = fopen(strFileName, "wb");
            }

        }

        if (g_PicFile)
        {
            //printf("%4d %4d -> %4d.\n", iPacketIdx, GetTickCount(), dwBufSize);
            //dumpBuffer(stdout, (unsigned char *)(pData+32), dwBufSize, SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD/* | SHOW_LINER*/);

            unsigned int iSize = fwrite((unsigned char *)pBuffer, dwBufSize, 1, g_PicFile);//保存图片
            printf("%s(%d) Write:%d\n"
                   , strFileName
                   , dwBufSize
                   , iSize);
            //fclose(fp);
        }
        else
        {
            printf("%s open failed.\n", strFileName);
        }
        //Test_Monitor(0, 0);
        //g_wait = false;
#endif // if 0

    }
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

    ////////////////////////
    // 登入登出测试
    if (1)
    {
        lUserID = NET_DVR_Login_V30((char *)"192.168.10.246", 9056, (char *)"admin", (char *)"12345", &struDeviceInfo);
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

        printf("waite 5 sec(s) ...\n");
        sleep(5);

        __fline;
        printf("NET_DVR_Logout_V30 ...\n");
        NET_DVR_Logout_V30(lUserID);
        NET_DVR_Cleanup();
    }

    printf("waite 5 sec(s) ...\n");
    sleep(5);

    //////////////////////////////////////////////
    //再次登入
    __fline;
    printf("NET_DVR_Login_V30 ...\n");
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

    NET_DVR_CLIENTINFO ClientInfo =
        {
            0
        };

#ifdef _WIN32
    Sleep(3000);  //millisecond
#elif  defined(__linux__)

    // 抓图测试，一直抓图
    while (1)
    {

        g_PicFile = NULL;
        g_iSuccessaFile = 0;
        int iRet;

        iRet = NET_DVR_RemoteSnap(lUserID, &ClientInfo, g_SnapCallBack, NULL, 0);
        if (iRet < 0)
        {
            __fline;
            printf("NET_DVR_RemoteSnap error:%d\n", iRet);
            sleep(5);
            continue;
            //return -1;
        }
        printf("NET_DVR_RemoteSnap  ok, waite 5s\n");

        sleep(5);    //second
    }

#endif

    // 取版本号
    while(1)
    {
        char verstring[128];
        int iRetGetVersion = NET_DVR_GetRemoteVersion(verstring, sizeof(verstring));

        if (iRetGetVersion>=0)
        {
            printf("NET_DVR_GetRemoteVersion ok,verstring(%d):%s\n", iRetGetVersion, verstring);
        }

        sleep(1);    //second
    }

    //stop
    NET_DVR_Logout_V30(lUserID);
    NET_DVR_Cleanup();

    return 0;
}
