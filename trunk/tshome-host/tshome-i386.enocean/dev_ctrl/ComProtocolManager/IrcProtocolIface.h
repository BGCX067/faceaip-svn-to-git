
#ifndef CIRCPROTOCOLIFACE_H
#define CIRCPROTOCOLIFACE_H

#include "IrcTypedefStruct.h"
#include "common/remote_def.h"
#include "common/thread.h"


#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif
#ifndef __rnline
	#define __rnline printf("%s(%d)\n", __FILE__, __LINE__);
#endif


#define COM_BUF_ZISE		4096
#define MAX_DEVICE_COUNT	256

	typedef struct _TimerStruct_t
	{
		TIMER_COND timerCond;
		THREAD_MUTEX timerMutex;

		// time out value
		#ifdef _WIN32
			DWORD timeout;
		#else
			struct timespec timeout;
		#endif

		bool bWaitObject;
	} TimerStruct_t;


class CIrcProtocolIface
{
// open interfaces
public:
	CIrcProtocolIface();
	virtual ~CIrcProtocolIface();

	virtual void SetProtocolName (char * strProtocolName)
	{
		if(strProtocolName != NULL)
		{
			strcpy(m_strProtocolName, strProtocolName);
		}
	}
	virtual char * GetProtocolName() { return m_strProtocolName; }

	virtual void SetCallbacks(OnParsedData pOnParsedData, 
		ToSendComDataByComIndex pToSendComDataByComIndex,
		ToSendComDataByProtoAddr pToSendSComDataByProtoAddr)
	{ 
		m_pOnParsedData = pOnParsedData;
		m_pToSendComDataByComIndex = pToSendComDataByComIndex;
		m_pToSendSComDataByProtoAddr = pToSendSComDataByProtoAddr;
	}
	virtual OnParsedData GetOnParsedData() { return m_pOnParsedData; }

	/*
	 * nComId = [0, (TOTAL_COM_NUM - 1)].
	 */
	virtual void AddData(int nComId, void *pBuf, int nLen);
	virtual int ParseMessage() { return 0; }

	virtual int GetSerialDeviceMessage(int nFunType,
			int nAddress, int nData, char *pMsg, int &nLen) { return 0; }

	virtual void AddComIndex(int nComIndex);
	virtual void AddDeviceAddress(int nComIndex, int nDevAddr);

	virtual int GetDeviceAddressCount() { return m_nDeviceAddressCount; }

protected:
	char m_strProtocolName[32];
	OnParsedData m_pOnParsedData;
	ToSendComDataByComIndex m_pToSendComDataByComIndex;
	ToSendComDataByProtoAddr m_pToSendSComDataByProtoAddr;
	char *m_comBuf[TOTAL_COM_NUM];
	int m_nCurLen[TOTAL_COM_NUM];
	bool m_bNeedParse[TOTAL_COM_NUM];
	TimerStruct_t m_timer[TOTAL_COM_NUM];

	bool m_bComIndex[TOTAL_COM_NUM];
	int m_nDeviceAddressCount;
	int m_nDeviceAddress[TOTAL_COM_NUM][MAX_DEVICE_COUNT];
};

extern "C"
{
	CIrcProtocolIface * CreateInstance();
	void DeleteInstance(CIrcProtocolIface *p);
}


#endif
