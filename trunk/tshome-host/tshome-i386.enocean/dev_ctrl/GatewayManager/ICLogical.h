/*
 * ICLogical.h
 *
 *  Created on: Apr 18, 2012
 *      Author: kyy
 */

#ifndef ICLOGICAL_H_
#define ICLOGICAL_H_

#include "NodeList.h"
#include "common/remote_def.h"

class CICIface;

class CICLogical
{
public:
	CICLogical();
	~CICLogical();

	void StartSendThread();
	void StopSendThread();

	int AddRequestNode(int nAction, void *pBuf, int nLen);
	int SendWifiMessage();

	int WaitSendCond();
	int HandleMessage();
	int DoHandleTcpClientMsg(void *pBuf, int nLen);
	int DoHandleTcpServerMsg(void *pBuf, int nLen);
	int DoHandleIcDirectMsg(void *pBuf, int nLen);

	int HandleComReq(void *pBuf, int nLen, bool bCustomerDev = false);
	int HandleInfraredReq(void *pBuf, int nLen);
	int HandleIrStudyReq(void *pBuf, int nLen);
	int HandleCustomerComReq(void *pBuf, int nLen);
	
	int HandleEnoceanReq(void *pBuf, int nLen, int *nDatapointType, 
		int *nValueAfterCtrl, int &nEnoceanOnOffDatapointId, int &nEnoceanDimmingDatapointId);


	int RefreshDeviceState(int nComDevType, int nDevId, int nArea, int com_ctrl_type, int nData);

	bool IsSendThreadRun() { return m_bSendThreadRun; }

	void SetICIface(CICIface *pICIface) { m_pICIface = pICIface; }
	CICIface *GetICIface() { return m_pICIface; }
	void ResetCurInfo();
	void SetCurInfo(int nDevId = -1, int nChannel = -1,
			char *strProtocol = NULL, int nCtrlType = -1, int nCtrlData = -1);

	bool SetTcpCSignal(kic_head_t * pRes);
	bool SetTcpSSignal(int nChannel, char *strProtocol);
	bool SetIcSignal(int nChannel, char *strProtocol);

	int ToAddSceneInputNode(kic_head_t * pKicHead);

private:
    THREAD_MUTEX m_sendListMutex;
    CNodeList m_sendList;

    THREAD_HANDLE m_hSendThread;
    TimerStruct m_sendTimer;
    bool m_bSendThreadRun;

	CICIface * m_pICIface;

	TimerStruct m_tcpCTimer;
	TimerStruct m_tcpSTimer;
	TimerStruct m_icTimer;

	int m_nDevId;
	int m_nChannel;
	char m_strProtocol[32];
	int m_nCtrlType;
	int m_nCtrlData;
};


#endif /* ICLOGICAL_H_ */
