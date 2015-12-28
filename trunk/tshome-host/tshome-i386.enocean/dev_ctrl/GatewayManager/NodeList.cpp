

#include "NodeList.h"
CNodeList::CNodeList()
{
	pHead = 0;
	pTail = 0;
	nNodeCount = 0;
}

int CNodeList::GetCount()
{
	return nNodeCount;
}

void CNodeList::AddTail(NodeBase *pNode)
{
	if(pHead == 0 && pTail == 0)
	{
		pHead = pNode;
		pTail = pNode;
		nNodeCount = 1;
	}
	else if(pTail != 0)
	{
		pTail->pNext = pNode;
		pTail = pNode;
		nNodeCount++;
	}
}

void CNodeList::AddHead(NodeBase *pNode)
{
	if(pHead == 0)
	{
		pHead = pNode;
		nNodeCount = 1;

		if(pTail == 0)
		{
			pTail = pNode;
		}
	}
	else if(pTail != 0)
	{
		pNode->pNext = pHead;
		pHead = pNode;
		nNodeCount++;
	}
}

void CNodeList::DeleteHead()
{
	if(pHead != 0)
	{
		NodeBase *pHeadNext = pHead->pNext;
		delete pHead;
		pHead = pHeadNext;
		
		nNodeCount--;

		if(pHead == 0)
		{
			pTail = 0;
			nNodeCount = 0;
		}
	}
}

void CNodeList::DeleteAll()
{
	while(pHead != 0)
	{
		DeleteHead();
	}
	pTail = 0;
	nNodeCount = 0;
}

NodeBase * CNodeList::GetAndSeperateHeader()
{
	NodeBase *pResult = 0;
	if(pHead != 0)
	{
		NodeBase *pHeadNext = pHead->pNext;
		pResult = pHead;
		pResult->pNext = 0;
		nNodeCount--;

		pHead = pHeadNext;
		if(nNodeCount == 0)
		{
			pHead = 0;
			pTail = 0;
			nNodeCount= 0;
		}
	}

	return pResult;
}
