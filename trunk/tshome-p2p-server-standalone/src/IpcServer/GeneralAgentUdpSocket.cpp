/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentUdpSocket.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentUdpSocket.cpp 5884 2012-07-19 04:21:23Z WuJunjie $
 *
 *  Explain:
 *     -udp实例-
 *
 *  Update:
 *     2012-4-11 9:32:49 WuJunjie Create
 *     2012-7-19 16:19:02 支持向处理线程发送消息的模型
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "CommonInclude.h"
#include <pjlib.h>
#include <stdio.h>
#include <string.h>
#include <Utility.h>

#include "GeneralAgentDef.h"
//
#include "../Logs.h"

#include "../Configs/ConfigManager.h"

#ifdef USE_GENERALSOCKETPROCESSOR
#include "GeneralSocketProcessor.h"
#endif //USE_GENERALSOCKETPROCESSOR

#include "GeneralAgentUdpSocket.h"

//#define DBG(x) x
#define DBG(x)


CGeneralAgentUdpSocket::CGeneralAgentUdpSocket(ISocketHandler& h, std::string strSocketName)
        :UdpSocket(h)
{
    SetSockName(strSocketName);
	
    m_BytesSent = 0;
    m_BytesReceived = 0;
}

void CGeneralAgentUdpSocket::Update()
{
    //__fline;printf("void CGeneralAgentUdpSocket::Update() -- %ld\n", time(NULL));

    // 去除超时peer
    OnPeer();
}
#include <iostream>
void CGeneralAgentUdpSocket::OnRawData(const char *p,size_t l,struct sockaddr *sa_from,socklen_t sa_len)
{

	m_BytesReceived += l;
	
    OnPeer(sa_from, sa_len);

    //向处理线程发送消息
#ifdef USE_GENERALSOCKETPROCESSOR

    g_GeneralSocketProcessor.SendSocketMsg(0, this, p, l, sa_from, sa_len);

    struct sockaddr_in sa;
    memcpy(&sa,sa_from,sa_len);
    ipaddr_t a;
    memcpy(&a,&sa.sin_addr,4);
    std::string ip;
    Utility::l2ip(a,ip);

    LOG4CPLUS_INFO(LOG_IPC, "Received " << l << " Bytes from:"<< ip << ":" << ntohs(sa.sin_port));
    //std::cout << "Received " << l << " Bytes from:"<< ip << ":" << ntohs(sa.sin_port) << std::endl ;
#else
#if 1

    char buf[1024];
    struct sockaddr_in sa;
    memcpy(&sa,sa_from,sa_len);
    ipaddr_t a;
    memcpy(&a,&sa.sin_addr,4);
    std::string ip;
    Utility::l2ip(a,ip);

    snprintf(buf, sizeof(buf), "Received %d bytes from: %s:%d\n", l,ip.c_str(),ntohs(sa.sin_port));
    printf(buf);

    printf("%s\n",static_cast<std::string>(p).substr(0,l).c_str());

    sendto(this->GetSocket(), buf, strlen(buf), 0, sa_from, sa_len);
    sendto(this->GetSocket(), static_cast<std::string>(p).substr(0,l).c_str(), static_cast<std::string>(p).substr(0,l).size(), 0, sa_from, sa_len);
#endif

#endif
}

// 向所有已知客户端发送消息
int CGeneralAgentUdpSocket::SendtoAllPeer( const char *pData, size_t lDataLen)
{
    int iret = 0;
    if (pData==NULL
    	|| lDataLen<=0
    	|| lDataLen > 1500)
    {
        //__trip;
        //printf("input not valid datalen:%d\n", lDataLen);
        LOG4CPLUS_ERROR(LOG_IPC, "invalid para, lDataLen:" << lDataLen);
        
        return ERR_CODE_PARAM_NOT_VALID;
    }

    m_BytesSent += lDataLen;


    // 去除超时peer
    // update中单独处理了，此处注释 wujj 2012-11-13 11:08:50
    // OnPeer();
    if (m_IpcPeer.size()==0)
    {
        //__trip;
        LOG4CPLUS_ERROR(LOG_IPC, "webserver not valid");
        return ERR_CODE_SENDTO_NOT_TERMINAL;
    }

    for( it = m_IpcPeer.begin(); it < m_IpcPeer.end(); ++it)
    {
        iret = sendto(this->GetSocket(), pData, lDataLen, 0, &(it->PeerAddr), it->PeerAddrLen);
        if (iret<0)
        {
        	//__fline;
        	//printf("sendto error:%s\n", strerror(errno));
        	LOG4CPLUS_ERROR(LOG_IPC, "sendto error:" << strerror(errno));
        }
    }

    return lDataLen;
}

int CGeneralAgentUdpSocket::GetPeer(std::vector<IPCPEER_T> &dstPeer)
{
    dstPeer = m_IpcPeer;
    return m_IpcPeer.size();
}

void CGeneralAgentUdpSocket::OnPeer(struct sockaddr *pPeer, socklen_t sa_len)
{
    //int ii;
    time_t ttNow = time(NULL);
    int iNewLocation = -1;
    time_t __timeout;

    CConfigIPC __cfgIPC;
    __cfgIPC.update();

        std::string ip;
        struct sockaddr_in sa;
if (pPeer && sa_len>0)
{
        memcpy(&sa,pPeer,sa_len);
        ipaddr_t a;
        memcpy(&a,&sa.sin_addr,4);
        Utility::l2ip(a,ip);
}
    // 删除超时的，以及
    for( it=m_IpcPeer.begin(); it!=m_IpcPeer.end(); )
    {
        // 更新
        if (pPeer && sa_len>0
            && sa_len==it->PeerAddrLen
            && memcmp(&it->PeerAddr, pPeer, sa_len)==0)
        {
            __timeout = ttNow-it->ttLastOnData;

            it->ttLastOnData = ttNow;

            iNewLocation = 0;
            DBG(
                __fline;
                printf("update %ld \n", __timeout);
            );
LOG4CPLUS_INFO(LOG_IPC, "Update:" << __timeout << " peer:"<< ip << ":" << ntohs(sa.sin_port));
        }

        // 删除超时
        __timeout = ttNow-it->ttLastOnData;
        if(__timeout >= __cfgIPC.getConfig().PeerTimeout)
        {
            it = m_IpcPeer.erase(it);
            DBG(
                __fline;
                printf("erase %ld \n", __timeout);
            );
        }
        else
        {
            it++;
        }
    }


    if (pPeer && sa_len>0
        && iNewLocation == -1)
    {
        // 加入
        IPCPEER_T __peer;
        memcpy(&__peer.PeerAddr,pPeer, sa_len);
        assert(__peer.PeerAddrLen == sa_len);
        __peer.ttLastOnData = ttNow;

        m_IpcPeer.push_back(__peer);

        //__fline;
        //printf("New peer: %s:%d at:%ld\n", ip.c_str(),ntohs(sa.sin_port), ttNow);
        LOG4CPLUS_INFO(LOG_IPC, "New peer:"<< ip << ":" << ntohs(sa.sin_port));
    }

    // 调试用
    DBG(
        for( it = m_IpcPeer.begin(); it < m_IpcPeer.end(); ++it)
    {
        printf("ttLastOnData:%ld\n", it->ttLastOnData)
            ;
        }
    );
    return;
}

/** Outgoing traffic counter. */
uint64_t CGeneralAgentUdpSocket::GetBytesSent(bool clear )
{
    return m_BytesSent;
}

/** Incoming traffic counter. */
uint64_t CGeneralAgentUdpSocket::GetBytesReceived(bool clear )
{
    return m_BytesReceived;
}

