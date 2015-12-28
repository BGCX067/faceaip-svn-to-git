/*
 * Backaudio_A61L.h
 *
 *  Created on: May 14, 2012
 *      Author: kyy
 */

#ifndef BACKAUDIO_A61L_H_
#define BACKAUDIO_A61L_H_

#include "IrcProtocolIface.h"
#include "common/remote_def.h"

class CBackaudio_A61L : public CIrcProtocolIface
{
public:
	CBackaudio_A61L()	{ }
	virtual ~CBackaudio_A61L() {}

	virtual void AddData(int nComId, void *pBuf, int nLen);
	virtual int ParseMessage();
	virtual int GetSerialDeviceMessage(int nFunType,
			int nAddress, int nData, char *pMsg, int &nLen);
};


#endif /* BACKAUDIO_A61L_H_ */
