#ifndef _LOTINFO_H_
#define _LOTINFO_H_

#include <Windows.h>
#include "..\..\BASE\BaseAll.h"

using namespace std;
class CLotInfo
{
public:
	enum Cmd
	{
		C_TRACK_OUT_START	= 100,
		C_TRACK_OUT_01		,
		C_TRACK_OUT_END		,
	};


public:
	CLotInfo(){}
	virtual ~CLotInfo(){}

	CFSM		m_fsm;
	BOOL		m_bRun;

	CTimer		m_tmEmptyLotEnd;

	deque<LotHistory>	m_qLotHistory; // Lot ���� ����

	void AutoRun(void);
	void CycleRun(void);
	void Init(void);
	BOOL IsExistInMc();

	void LotInfoCopy(int Org, int Target);
	void LotInfoClear(int nNo);
	void LotInfoAllClear(bool ohtMode = false);
	BOOL PartIDComp(int lotInfo1, int lotInfo2);
	BOOL MergeLotIDComp(int lotInfo1, int lotInfo2);
	int  GetXOutCnt(int lotInfo);

	void LotFirstHistoryClear();
	void LotHistoryAllClear();
	void LotSplitAllClear();
	void LotHistorySort();

	void DelDeque(int nNo);
	/*
	m_qLotHistory.clear(); // ��� Clear
	m_qLotHistory.size();  // ���� ����
	m_qLotHistory.push_back(data); // �߰�
	m_qLotHistory.[0] // �迭������ ���
	m_qLotHistory.pop_front(); // ���� �տ� ���� ����
	*/

};


/////////////////////////////////////////////////////////////////////
extern CLotInfo g_lotInfo;
/////////////////////////////////////////////////////////////////////

#endif