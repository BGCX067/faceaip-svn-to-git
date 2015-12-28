/*
 * Yodar_4028B.h
 *
 *  Created on: Aug 31, 2012
 *      Author: kyy
 */

#ifndef YODAR_4028B_H_
#define YODAR_4028B_H_

#include "IrcProtocolIface.h"
#include "common/remote_def.h"



#define MAX_DATA_COUNT		50
#define MAX_DATA_LENGTH		256


struct com_data
{
	int len;
	int index;
	char data[MAX_DATA_LENGTH];
};


int HandlerProcess(void *param);


class Yodar_4028B : public CIrcProtocolIface
{
public:
	Yodar_4028B();
	virtual ~Yodar_4028B();

	virtual void AddData(int nComId, void *pBuf, int nLen);
	virtual int GetSerialDeviceMessage(int nFunType,
			int nAddress, int nData, char *pMsg, int &nLen);

	//virtual int ParseMessage() { return 0; }

public:
	void SetRun(bool bRun) { m_bRun = bRun; }
	bool GetRun() { return m_bRun; }
	void DoHandleWork();
	int HandleMessage(com_data *data);
	int WaitForSignal();

private:
	void StartHandleThread();
	void EndHandleThread();
	
private:
	com_data m_data[MAX_DATA_COUNT];
	THREAD_HANDLE m_handleThread;
	TIMER_COND m_handleCond;
	THREAD_MUTEX m_handleMutex;
	bool m_bRun;

	int m_nReadIndex;
	int m_nWriteIndex;
};


#endif /* YODAR_4028B_H_ */

