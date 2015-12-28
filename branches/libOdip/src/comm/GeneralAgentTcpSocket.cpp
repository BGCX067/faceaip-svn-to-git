/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocket.cpp - _explain_
 *
 * Copyright (C) 2011 WuJunjie(Joy.Woo@hotmail.com), All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocket.cpp 0001 2012-04-06 09:58:37Z WuJunjie $
 *
 *  Explain:
 *     -tcp 统一写，便于统一协议解析-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "CommonInclude.h"
#include "GeneralAgentTcpSocket.h"

#include "str_opr.h"
#include <string.h> /* memset */
CGeneralAgentTcpSocket::CGeneralAgentTcpSocket(ISocketHandler& h, std::string strSocketName) : TcpSocket(h)
{
    // 由Handle自己管理
    SetDeleteByHandler();
    // 数据不缓存
#ifdef __INTERNAL_ODIP_DATA_PARSE

    memset(&m_headLastOn, 0, sizeof(ODIP_HEAD_T));
#else

    DisableInputBuffer();
#endif //INTERNAL_ODIP_DATA_PARSE

    m_tOnData = 0;

    SetSockName(strSocketName);
}

#ifdef INTERNAL_ODIP_DATA_PARSE
// read方法写在这个地方是为了统一协议解析，因为无论客户端还是服务器
// 一个系统的协议体系应该是一样的， wujunjie 2012-4-9 15:35:31
void CGeneralAgentTcpSocket::OnRead()
{
    return TcpSocket::OnRead();
    unsigned int n = ibuf.GetLength();

    //__fline;
    //printf("data len:%d, time(NULL) %d -m_tOnData %d\n", n, time(NULL), m_tOnData);

    // 超过10s的数据视为 无效， 清理之， 防止客户端出错的时候发送错误数据
    if (n>0
        && time(NULL)-m_tOnData > ODIP_PROTOCOL_DATA_TIMEOUT)
    {
        __trip;
        printf("data timeout len:%d\n", n);

        ibuf.Remove(n);
        memset(&m_headLastOn, 0, sizeof(ODIP_HEAD_T));
    }

    //  超长， 错误数据直接全部释放
    if (n>ODIP_PROTOCOL_DATA_MAX_LEN)
    {
        __trip;
        printf("data too len:%d\n", n);

        ibuf.Remove(ibuf.GetLength());
        memset(&m_headLastOn, 0, sizeof(ODIP_HEAD_T));
        return;
    }

    // 先检查上次是否有未处理完的协议
    // 不可能有u8DataLen = 0的协议被缓存， 因为这样的协议直接处理掉了， 不用再等数据
    if (m_headLastOn.dvrip.dvrip_extlen > 0)
    {
        if (n >= m_headLastOn.dvrip.dvrip_extlen)
        {
            ibuf.Read((char *)m_OdipDataBuffer, m_headLastOn.dvrip.dvrip_extlen);
            __trip;
            OnCommand_ODIP(&m_headLastOn, m_OdipDataBuffer);

            ZERO_ODIP_HEAD_T(&m_headLastOn);
        }
    }

    n = ibuf.GetLength();
    __fline;
    printf("buf len:%d\n", n);

    while (n >= sizeof(ODIP_HEAD_T))
    {

        ibuf.Read((char *)&m_headLastOn, sizeof(ODIP_HEAD_T));

        n = ibuf.GetLength();
        __fline;
        printf("buf len:%d, ext:%d\n", n, m_headLastOn.dvrip.dvrip_extlen);

        // 长度不足， 下次处理
        if (n < m_headLastOn.dvrip.dvrip_extlen)
        {
            break;
        }

        if(m_headLastOn.dvrip.dvrip_extlen>0)
        {
            ibuf.Read((char *)m_OdipDataBuffer, m_headLastOn.dvrip.dvrip_extlen);
        }

        __trip;
        OnCommand_ODIP(&m_headLastOn, m_OdipDataBuffer);
        ZERO_ODIP_HEAD_T(&m_headLastOn);

        n = ibuf.GetLength();
        __fline;
        printf("buf len:%d\n", n);
    }

    return;
}
#endif // INTERNAL_ODIP_DATA_PARSE

void CGeneralAgentTcpSocket::OnRawData(const char *buf,size_t len)
{
    m_tOnData = time(NULL);
    DBG_CODE(
        __fline;
        printf(" %s:%d\n", __FUNCTION__, len);
    );
    //////////
    //有数据就调用， 另一种方式
    OnCommand((char *)buf, (unsigned int)len);
    ///////////
}

void CGeneralAgentTcpSocket::OnCommand(char *pCmdData, unsigned int ulCmdDataLen)
{
        DBG_CODE(
    __fline;
    printf("CGeneralAgentTcpSocket::OnCommand, len:%d\n", ulCmdDataLen);
      );
    {
//        dumpBuffer(stdout, ( unsigned char *)pCmdData, ulCmdDataLen,
//                   SHOW_HEXAD | SHOW_ASCII | SHOW_BINAR | SHOW_LINER);

        //return;

        g_cabRecvBuff.Append((unsigned char *)pCmdData, ulCmdDataLen);
        ODIP_HEAD_T *pDvrip = (ODIP_HEAD_T *)g_cabRecvBuff.Buf();

        while (g_cabRecvBuff.Size() >= ODIP_HEAD_T_SIZE
               && g_cabRecvBuff.Size() >= (ODIP_HEAD_T_SIZE + pDvrip->dvrip.dvrip_extlen))
        {
            ///////////cmd_Analisys(pDvrip, ODIP_HEAD_T_SIZE + pDvrip->dvrip.dvrip_extlen);
            DBG_CODE(
                __fline;
                printf("OnCommand_ODIP -- Cmd:%#x, p0:%d, extLen:%d\n"
                       , pDvrip->dvrip.dvrip_cmd
                       , pDvrip->dvrip.dvrip_p[0]
                       , pDvrip->dvrip.dvrip_extlen);
            );

            //__trip;
            OnCommand_ODIP(pDvrip, ((unsigned char *)pDvrip)+ODIP_HEAD_T_SIZE);

            //OnCommand_ODIP

            //		printf("buf size:%d, pop:%d\n",
            //			g_cabRecvBuff.Size(),
            //			ODIP_HEAD_T_SIZE + pDvrip->dvrip.dvrip_extlen);
            //g_cabLastCMD.Reset();
            //g_cabLastCMD.Append((unsigned char *)g_cabRecvBuff.Buf(), ODIP_HEAD_T_SIZE + pDvrip->dvrip.dvrip_extlen);

            g_cabRecvBuff.Pop(NULL, ODIP_HEAD_T_SIZE + pDvrip->dvrip.dvrip_extlen);
            //		printf("buf size:%d\n", g_cabRecvBuff.Size());

            pDvrip = (ODIP_HEAD_T *)g_cabRecvBuff.Buf();
        }

        //
        // 缓存超大，数据错误
        //
        if (g_cabRecvBuff.Size() > MAX_DVRIP_PACKET_SIZE)
        {
            __trip;
            printf("!!Wrong recv buff:%d\n", g_cabRecvBuff.Size());

            //dumpBuffer(stderr, (U8 *)g_cabRecvBuff.Buf(), 32,
            //       SHOW_HEXAD | SHOW_ASCII | SHOW_BINAR | SHOW_LINER);

            g_cabRecvBuff.Reset();
        }
    }
}

#ifdef INTERNAL_ODIP_DATA_PARSE
void CGeneralAgentTcpSocket::OnCommand_ODIP(const ODIP_HEAD_T *pCmd, const unsigned char *pData)
{
    DBG_CODE(
        __fline;
        printf("OnCommand_ODIP -- Cmd:%#x, p0:%d, extLen:%d\n"
               , pCmd->dvrip.dvrip_cmd
               , pCmd->dvrip.dvrip_p[0]
               , pCmd->dvrip.dvrip_extlen); );
}
#endif
