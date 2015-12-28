/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralSocketProcessor.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralSocketProcessor.h 5884 2012-07-18 04:13:17Z WuJunjie $
 *
 *  Explain:
 *     -单独的消息处理对象-
 *
 *  Update:
 *     2012-07-18 04:13:17  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _GeneralSocketProcessor_H
#define _GeneralSocketProcessor_H
#include "EZThread.h"
#include "SocketMsgQue.h"
#include <Socket.h>

#define g_GeneralSocketProcessor (*CGeneralSocketProcessor::instance())
class CGeneralSocketProcessor : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CGeneralSocketProcessor);

    BOOL Start();
    BOOL Stop();
    void ThreadProc();

    BOOL SendSocketMsg(unsigned int	 iMsg,Socket *pSocket=NULL, const char *pData=NULL,size_t lDataLen=0,struct sockaddr *sa_from=NULL,socklen_t sa_len=0);
    BOOL RecvSocketMessage (SOCKET_MSG_NODE *pMsg, BOOL wait = TRUE);
private:
    CGeneralSocketProcessor();
    virtual ~CGeneralSocketProcessor();
    CSocketMsgQue* m_pSocketMsgQue;

};
#endif // file end
