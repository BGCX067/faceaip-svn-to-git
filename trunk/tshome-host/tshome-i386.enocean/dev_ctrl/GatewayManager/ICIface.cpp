/*
 * ICIface.cpp
 *
 *  Created on: Apr 18, 2012
 *      Author: kyy
 */

#include "ICIface.h"
#include "KNXCondef.h"

CICIface::CICIface()
{
	m_nWifiType = IC_UNKNOWN;

	m_pIcLogical = new CICLogical();
}

CICIface::~CICIface()
{
	if(m_pIcLogical != NULL)
	{
		delete m_pIcLogical;
		m_pIcLogical = NULL;
	}
}

int CICIface::Send(int nType, void *pBuf, int nLen)
{
	return -1;
}
