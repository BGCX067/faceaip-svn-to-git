/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TeleSerialHandler.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: TeleSerialHandler.h 0001 2012-04-06 09:54:05Z WuJunjie $
 *
 *  Explain:
 *     -·´Ó¦¶Ñ-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _TeleSerialHandler_H
#define _TeleSerialHandler_H
#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>
#include "TeleSerialLog.h"

#ifdef TeleSerial_FUNC_SIGNAL_ONDATA
#include "EZObject.h"
#include "EZSignals.h"
typedef TSignal3<Socket *, const char *, size_t> SIG_TeleSerialHandler_DATA;
#endif //TeleSerial_FUNC_SIGNAL_ONDATA
class CTeleSerialHandler : public SocketHandler
{
public:
    CTeleSerialHandler(CTeleSerialLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void Update();

    Socket *GetSocketByName(std::string strServerName);
    int CloseAndDeleteAll();
    void OnSerial(int id, const char *pdat, size_t len);

#ifdef TeleSerial_FUNC_SIGNAL_ONDATA

    void OnDataCall(Socket *, const char *buf,size_t len);
    bool AddSignal(CEZObject * pObj, SIG_TeleSerialHandler_DATA::SigProc pProc);
    bool DelSignal(CEZObject * pObj, SIG_TeleSerialHandler_DATA::SigProc pProc);
private:

    CEZMutex m_MutexSig;
    SIG_TeleSerialHandler_DATA m_sigData;
#endif //TeleSerial_FUNC_SIGNAL_ONDATA

private:
};

#endif // _TeleSerialHandler_H
