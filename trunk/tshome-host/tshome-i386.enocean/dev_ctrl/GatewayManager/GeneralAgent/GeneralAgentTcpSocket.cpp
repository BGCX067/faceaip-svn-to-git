/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocket.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocket.cpp 0001 2012-04-06 09:58:37Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "CommonInclude.h"
#include "GeneralAgentTcpSocketServer.h"

#include <string.h> /* memset */
CGeneralAgentTcpSocket::CGeneralAgentTcpSocket(ISocketHandler& h) : TcpSocket(h)
{
    // ��Handle�Լ�����
    SetDeleteByHandler();
    // ��ݲ�����
    //DisableInputBuffer();

    memset(&m_headLastOn, 0, sizeof(remote_header_t));
    m_tOnData = 0;
}

// read����д������ط���Ϊ��ͳһЭ���������Ϊ���ۿͻ��˻��Ƿ�����
// һ��ϵͳ��Э����ϵӦ����һ��ģ� wujunjie 2012-4-9 15:35:31

void CGeneralAgentTcpSocket::OnRead()
{
    size_t n = ibuf.GetLength();

    //__fline;
    //printf("data len:%d, time(NULL) %d -m_tOnData %d\n", n, time(NULL), m_tOnData);

    // ����10s�������Ϊ ��Ч�� ����֮�� ��ֹ�ͻ��˳����ʱ���ʹ������
    if (n>0
        && time(NULL)-m_tOnData > WIFI_AGENT_PROTOCOL_DATA_TIMEOUT)
    {
        __trip;
        printf("data timeout len:%d\n", n);

        ibuf.Remove(n);
        memset(&m_headLastOn, 0, sizeof(remote_header_t));
    }


    TcpSocket::OnRead();
    m_tOnData = time(NULL);
    n = ibuf.GetLength();

    //  ������ �������ֱ��ȫ���ͷ�
    if (n>WIFI_AGENT_PROTOCOL_DATA_MAX_LEN)
    {
        __trip;
        printf("data too len:%d\n", n);

        ibuf.Remove(ibuf.GetLength());
        memset(&m_headLastOn, 0, sizeof(remote_header_t));
        return;
    }

    // ����ĸ��ʵ�ʶ�������
    unsigned char wifiAgentData[32];

    // �ȼ���ϴ��Ƿ���δ�������Э��
    // ��������u8DataLen = 0��Э�鱻���棬 ��Ϊ�����Э��ֱ�Ӵ�����ˣ� �����ٵ����
    size_t nDataLen = ntohs(m_headLastOn.length);
    if (nDataLen > 0)
    {
    	if (n >= nDataLen)
        {
        	ibuf.Read((char *)wifiAgentData, nDataLen);

        	OnCommand_WifiAgent(&m_headLastOn, wifiAgentData);
        	memset(&m_headLastOn, 0, sizeof(remote_header_t));
    	}
		else
		{
			return;
		}	
    }

    n = ibuf.GetLength();

    while (n >= sizeof(remote_header_t))
    {

        ibuf.Read((char *)&m_headLastOn, sizeof(remote_header_t));

        n = ibuf.GetLength();

        // ���Ȳ��㣬 �´δ���
        nDataLen = ntohs(m_headLastOn.length);
		if (n < nDataLen)
        {
            break;
        }


        ibuf.Read((char *)wifiAgentData, nDataLen);

        OnCommand_WifiAgent(&m_headLastOn, wifiAgentData);
        memset(&m_headLastOn, 0, sizeof(remote_header_t));

        n = ibuf.GetLength();
    }

    return;
}

void CGeneralAgentTcpSocket::OnRawData(const char *buf,size_t len)
{
    m_tOnData = time(NULL);
    //    __fline;
    //    printf("CGeneralAgentTcpSocket::OnRawData:%d\n", len);

    //////////
    //����ݾ͵��ã� ������
    OnCommand((char *)buf, (unsigned int)len);
    ///////////
}

void CGeneralAgentTcpSocket::OnCommand(char *pCmdData, unsigned int ulCmdDataLen)
{
    DBG_CODE( printf("CGeneralAgentTcpSocket::OnCommand, len:%d\n", ulCmdDataLen); );
}

void CGeneralAgentTcpSocket::OnCommand_WifiAgent(remote_header_t *pCmd, unsigned char *pData)
{
    DBG_CODE( printf("OnCommand_WifiAgent %d %d %d %d %d %d %d %d\n", 
		pCmd->start,
		pCmd->protocol_type,
		ntohs(pCmd->length),
		ntohs(pCmd->serial_number),
		pCmd->function_code,
		pCmd->comm_type,
		pCmd->comm_source,
		ntohs(pCmd->proto_ver)); );
}

void CGeneralAgentTcpSocket::SetWifiPtr(CWifiKernel *pWifi)
{
	m_pWifi = pWifi;
}
