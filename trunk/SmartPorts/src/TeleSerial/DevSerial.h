/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevSerial.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: DevSerial.h 5884 2013-07-16 07:30:00Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-16 07:30:00 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __DevSerial_H__
#define __DevSerial_H__

#include <vector>
#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include "ezserial.h"

#define COM_NAME_LENGTH         64
class SERIAL_ATTR_T
{
public:
    int id;                                                       // id of com record in db
    std::string serial_dev_name;//[COM_NAME_LENGTH]; // /dev/ttyUSB0 ...
    int baud;
    int data_bits;
    int parity;
    int stop_bits;                                                // 1, 15, 2

    ezserial_t *m_pSerial;
    SERIAL_ATTR_T()
    {
        id = 0;
        baud = 115200;
        data_bits = 8;
        parity = 0;
        stop_bits = 1;

        m_pSerial = NULL;
    }

};

typedef TSignal3<int, void *, int> SIG_DevSerial_DATA;

class CDevSerial : public CEZThread
{
public:
    // name is the key
    // m_pSerial will be modify
    int AddSerial(SERIAL_ATTR_T &SerialAttr);
    int RemoveSerial(const std::string &strSerialName);

    static CDevSerial* instance(void);

    // id data datalen
    BOOL Start(CEZObject * pObj, SIG_DevSerial_DATA::SigProc pProc);
    BOOL Stop(CEZObject * pObj, SIG_DevSerial_DATA::SigProc pProc);

    void ThreadProc();

    void OnData(int id, uint8_t *buf, int length);
    int SendData(int id, uint8_t *buf, int length);

private:
    CDevSerial();
    virtual ~CDevSerial();
    static CDevSerial* _instance;

    CEZMutex m_MutexSig;
    SIG_DevSerial_DATA m_sigData;

    bool m_bRunning;

    CEZMutex m_MutexSerial;
    std::vector<SERIAL_ATTR_T> m_vecSerial;

    void SetDefaultConfig();
};

#endif //__DevSerial_H__
