/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevSerial.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: DevSerial.cpp 5884 2013-07-16 07:30:08Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-16 07:30:08 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <iostream>
//#include "str_opr.h"

#include "EZThread.h"
#include "EZTimer.h"

#include "DevSerial.h"

#ifndef _DEBUG_THIS
    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

CDevSerial* CDevSerial::_instance = NULL;

CDevSerial* CDevSerial::instance(void)
{
    if(NULL == _instance)
    {
        _instance = new CDevSerial();
    }
    return _instance;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CDevSerial::CDevSerial() :CEZThread("CDevSerial", TP_DEFAULT), m_sigData(1)
{
    //trace("CDevSerial Enter--------\n");
    //m_ttLastOn = 0;

    ezserial_init();
}

CDevSerial::~CDevSerial()
{
    //trace("CDevSerial Leave--------\n");

    for (std::vector<SERIAL_ATTR_T>::iterator iter=m_vecSerial.begin(); iter != m_vecSerial.end(); iter++)
    {
        if (iter->m_pSerial != NULL)
        {
            ezserial_close(iter->m_pSerial);
            iter->m_pSerial = NULL;
            //m_vecSerial.erase(iter);
            //return 0;
        }
    }

    ezserial_cleanup();
}

BOOL CDevSerial::Start(CEZObject * pObj, SIG_DevSerial_DATA::SigProc pProc)
{
    CEZLock _lock(m_MutexSig);

    if(m_sigData.Attach(pObj, pProc) < 0)
    {
        trace("attach error\n");
        return FALSE;
    }

    if (m_bLoop)
    {
        return TRUE;
    }

    if(m_vecSerial.size()<1)
    {
        SetDefaultConfig();
    }

    int ret = CreateThread();

    return ret;
}

BOOL CDevSerial::Stop(CEZObject * pObj, SIG_DevSerial_DATA::SigProc pProc)
{
    CEZLock _lock(m_MutexSig);

    if(m_sigData.Detach(pObj, pProc) == 0)
    {
        return TRUE;
    }

#if 0
    if(m_bLoop)
    {
        m_bLoop = FALSE;
        DestroyThread();
    }

    return TRUE;
#else

    return FALSE;
#endif

}

void CDevSerial::ThreadProc()
{
    while (m_bLoop)
    {
        // 目前直接用ezserial的线程转发数据
        sleep(100);
    }
}

void CDevSerial::OnData(int id, uint8_t *buf, int length)
{
    if (length > 0
        && buf)
    {
        m_sigData(id, buf, length);
    } // if
}
static void CDevSerial_SerialCallback(int id,
                                      uint8_t *buf,
                                      int length)
{
    CDevSerial::instance()->OnData(id, buf, length);
}

int CDevSerial::SendData(int id, uint8_t *buf, int length)
{
    for (std::vector<SERIAL_ATTR_T>::iterator iter=m_vecSerial.begin(); iter != m_vecSerial.end(); iter++)
    {
        if (id == iter->id)
        {
            // close
            ezserial_putdata(iter->m_pSerial, buf, length);

            return length;
        }
    }

    return -1;
}

// m_pSerial will be modify
int CDevSerial::AddSerial(SERIAL_ATTR_T &SerialAttr)
{
    CEZLock _lock(m_MutexSerial);

    // close
    for (std::vector<SERIAL_ATTR_T>::iterator iter=m_vecSerial.begin(); iter != m_vecSerial.end(); iter++)
    {
        if (SerialAttr.serial_dev_name == iter->serial_dev_name)
        {
            // close
            ezserial_close(iter->m_pSerial);
            m_vecSerial.erase(iter);

            break;
        }
    }

    SerialAttr.m_pSerial=ezserial_open(SerialAttr.serial_dev_name.c_str()
                                       , CDevSerial_SerialCallback
                                       , SerialAttr.id
                                       , SerialAttr.baud
                                       , SerialAttr.data_bits
                                       , SerialAttr.parity
                                       , SerialAttr.stop_bits);

    // not found
    if (SerialAttr.m_pSerial)
    {
        m_vecSerial.push_back(SerialAttr);
        return 0;
    }
    // else
    __trip;
    printf("ezserial_open(%s) failed:%s\n", SerialAttr.serial_dev_name.c_str(), ezserial_geterrormsg());

    return -1;
}

int CDevSerial::RemoveSerial(const std::string &strSerialName)
{
    CEZLock _lock(m_MutexSerial);

    for (std::vector<SERIAL_ATTR_T>::iterator iter=m_vecSerial.begin(); iter != m_vecSerial.end(); iter++)
    {
        if (strSerialName == iter->serial_dev_name)
        {
            ezserial_close(iter->m_pSerial);
            m_vecSerial.erase(iter);
            return 0;
        }
    }

    // not found
    printf("RemoveSerial(%s) not found.\n", strSerialName.c_str());

    return -1;
}
void CDevSerial::SetDefaultConfig()
{
    SERIAL_ATTR_T __cfg;

    __cfg.baud = 9600;
    __cfg.data_bits = 8;
    __cfg.parity = 0;
    __cfg.stop_bits = 1;                                                // 1, 15, 2
    __cfg.m_pSerial = NULL;

    __cfg.id = 33;              // conn at com33
    __cfg.serial_dev_name = "/dev/ttyO2";
    AddSerial(__cfg);

    __cfg.id = 34;              // conn at com??
    __cfg.serial_dev_name = "/dev/ttyO3";
    AddSerial(__cfg);

    __cfg.id = 35;              // conn at com??
    __cfg.serial_dev_name = "/dev/ttyO4";
    AddSerial(__cfg);
}
