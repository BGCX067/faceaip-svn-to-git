
#ifndef CEPSON_ESC_VP21_H
#define CEPSON_ESC_VP21_H


#include "IrcProtocolIface.h"
#include "common/remote_def.h"

class CEpsonEscVp21 : public CIrcProtocolIface
{
public:
	CEpsonEscVp21()	{ }
	virtual ~CEpsonEscVp21() {}

	virtual void AddData(int nComId, void *pBuf, int nLen);
	virtual int ParseMessage();
	virtual int GetSerialDeviceMessage(int nFunType, int nAddress, int nData, char *pMsg, int &nLen);
};

#endif
