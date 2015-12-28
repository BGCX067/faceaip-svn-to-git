

#ifndef KNXCONDEF_H
#define KNXCONDEF_H

#include "KNXCondef.h"

class DECLSPEC_DLL_EXPORT CNodeList
{
public:
	CNodeList();
		
	NodeBase *pHead;
	NodeBase *pTail;
	int nNodeCount;

	int GetCount();
	void AddHead(NodeBase *pNode);
	void AddTail(NodeBase *pNode);
	void DeleteHead();
	void DeleteAll();
	NodeBase * GetAndSeperateHeader();
};

#endif
