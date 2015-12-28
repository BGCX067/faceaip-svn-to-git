/*
 *  AQM_1000.h
 *
 *  Created on: Sep 11, 2012
 *      Author: kyy
 */

#ifndef AQM_1000_H_
#define AQM_1000_H_

#include "IrcProtocolIface.h"
#include "common/remote_def.h"


#define MAX_DATA_COUNT		4
#define MAX_DATA_LENGTH		256
#define MAX_BUFFER_LEN		1024


struct com_data
{
	int len;
	int index;
	char data[MAX_DATA_LENGTH];
};

int HandlerProcess(void *param);
int SendMsgProcess(void *param);


class AQM_1000_CLS : public CIrcProtocolIface
{
public:
	AQM_1000_CLS();
	virtual ~AQM_1000_CLS();
	
	virtual void AddData(int nComId, void *pBuf, int nLen);
	virtual int ParseMessage();
	
	virtual int GetSerialDeviceMessage(int nFunType, 
			int nAddress, int nData, char *pMsg, int &nLen);
	
public:
	void SetRun(bool bRun) { m_bRun = bRun; }
	bool GetRun() { return m_bRun; }
	void DoHandleWork();
	void DoSendMsgWork();
	int HandleMessage(com_data *data);
	int WaitForHandleSignal();
	int WaitForSendSignal();

private:
	void StartHandleAndSendThread();
	void EndHandleAndSendThread();
	
private:
	int nCurrentBufLen;
	char buf_data[MAX_BUFFER_LEN];
	
	com_data m_data[MAX_DATA_COUNT];
	THREAD_HANDLE m_handleThread;
	TIMER_COND m_handleCond;
	THREAD_MUTEX m_handleMutex;
	bool m_bRun;
	
	THREAD_HANDLE m_sendThread;
	TIMER_COND m_sendCond;
	THREAD_MUTEX m_sendMutex;

	int m_nReadIndex;
	int m_nWriteIndex;
};
#endif /* AQM_1000_H_ */
