/*
 * SceneKernel.h
 *
 *  Created on: May 18, 2012
 *      Author: kyy
 */

#ifndef SCENEKERNEL_H_
#define SCENEKERNEL_H_

#include <common/thread.h>
#include "KNXCondef.h"
#include "NodeList.h"
#include "db/ts_db_protocol_infos.h"

void * SceneInputProcess(void * pParam);
void * SendSceneProcess(void * pParam);
void * SendDelayItemProcess(void * pParam);

typedef struct _SceneInputNode
{
	NodeBase *pNext;
	ts_scene_io_info_t scene_in_info;
	int main_type;
	int sub_type;
	char original_value[32];
	int is_first_use;
}SceneInputNode;

typedef struct _SceneNode
{
	NodeBase *pNext;
	int nSceneId;
}SceneNode;

typedef struct _DelayReqItem
{
	NodeBase *pNext;
	ts_scene_io_info_t reqInfo;
	time_t time;
}DelayReqItem;


class CSceneKernel
{
public:
	CSceneKernel();
	virtual ~CSceneKernel();

	// inner interfaces
	void SetRun(bool bRun) { m_bRun = bRun; }
	bool IsRun() { return m_bRun; }

	int WaitForSignal();
	SceneNode * GetListHead();
	int DoRequest(SceneNode *pNode);
	int DeleteReqList();
	int SceneItemRequest(int nDevId, int nParam, char *strValue);

	int GetCtrlTypeAndValue(int nKeyFun, int &nCtrlType, int &nData, char * strValue);

	// outer interfaces
	int StartSceneThread();
	void StopSceneThread();

	int SceneRequest(int nSceneId);
	int AddSceneInputNode(SceneInputNode inputNode);
	int AddSceneRequestNode(int nSceneId);

	DelayReqItem * GetDelayItemListHead();
	int DoDelayItemRequest();
	int AddDelayItemRequestNode(ts_scene_io_info_t *pItem);
	int DeleteDelayReqList();
	CNodeList * GetDelayItemList() { return &m_delayReqList; }

	SceneInputNode * GetInputListHead();
	int DoInputRequest();
	int AddInputRequestNode(ts_scene_io_info_t *pItem);
	int DeleteInputReqList();
	CNodeList * GetInputReqList() { return &m_inputReqList; }
	int GetTriggerSceneList(SceneInputNode * pInputNode, ts_scene_info_t ** pScene_infos, int * pCount);
	int IsTheValueChangedAndMatch(SceneInputNode * pInputNode);
	int IsKnxConditionMatch(SceneInputNode * pInputNode, bool bJustMatchCurrVal);
	// int IsSerialResConditionMatch(SceneInputNode * pInputNode);

	int IsAllAndConditionsMatch(int nSceneId, bool bExceptSelf, SceneInputNode * pInputSelfNode);

private:
	bool m_bRun;
	pthread_t m_nReqThread;
	TimerStruct m_reqTimerCond;
	CNodeList m_reqList;
	pthread_mutex_t m_reqListMutex;

	pthread_t m_nDelayReqThread;
	CNodeList m_delayReqList;
	pthread_mutex_t m_delayReqListMutex;
	
	pthread_t m_nInputReqThread;
	CNodeList m_inputReqList;
	pthread_mutex_t m_inputReqListMutex;
};

#endif /* SCENEKERNEL_H_ */
