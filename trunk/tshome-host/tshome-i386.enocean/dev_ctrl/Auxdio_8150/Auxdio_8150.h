/*
 * Auxdio_8150.h
 *
 *  Created on: May 14, 2012
 *      Author: kyy
 */

#ifndef BACKAUDIO_A61L_H_
#define BACKAUDIO_A61L_H_

#include "IrcProtocolIface.h"
#include "common/remote_def.h"


class CAuxdio_8150 : public CIrcProtocolIface
{
public:
	CAuxdio_8150()	{ memset(m_bVolumeUp, false, sizeof(bool) * 16); }
	virtual ~CAuxdio_8150() {}

	virtual void AddData(int nComId, void *pBuf, int nLen);
	virtual int ParseMessage();
	virtual int GetSerialDeviceMessage(int nFunType,
			int nAddress, int nData, char *pMsg, int &nLen);

private:
	bool m_bVolumeUp[16];
};


#endif
