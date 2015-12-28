/*
 * ICIface.h
 *
 *  Created on: Apr 18, 2012
 *      Author: kyy
 */

#ifndef ICIFACE_H_
#define ICIFACE_H_

#include "ICLogical.h"

class CICIface
{
public:
	CICIface();
	virtual ~CICIface();

	virtual int Send(int nType, void *pBuf, int nLen);

	virtual CICLogical *GetLogicalPtr() { return m_pIcLogical; }

protected:
	int m_nWifiType;

    CICLogical *m_pIcLogical;
};

#endif /* ICIFACE_H_ */
