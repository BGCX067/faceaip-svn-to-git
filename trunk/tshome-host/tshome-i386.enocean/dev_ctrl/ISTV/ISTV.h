/*
 *  ISTV.h
 *
 *  Created on: Sep 4, 2012
 *      Author: kyy
 */

#ifndef ISTV_H_
#define ISTV_H_

#include "IrcProtocolIface.h"
#include "common/remote_def.h"

class ISTV_041 : public CIrcProtocolIface
{
public:
	ISTV_041() {}
	virtual ~ISTV_041() {}
	
	virtual void AddData(int nComId, void *pBuf, int nLen){ return ; }
	virtual int GetSerialDeviceMessage(int nFunType, 
			int nAddress, int nData, char *pMsg, int &nLen);
};
#endif /* ISTV_H_ */
