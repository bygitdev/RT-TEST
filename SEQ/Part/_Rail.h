#ifndef _RAIL_H_
#define _RAIL_H_

#include <Windows.h>
#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"

class CRail
{
public:
	enum PosX
	{
		PX_WAIT			= 0, // 가장 뒤에 대기위치
		PX_RCV_START	= 1, // Pusher 동작시 Pcb Clamp Pos
		PX_RCV_END		= 2,
		PX_ALIGN		= 3,
		PX_2D			= 4,
	};

	enum Cmd
	{
		C_RCV_START			= 100,
		C_RCV_01			,
		C_RCV_02			,
		C_RCV_2D_START		,
		C_RCV_2D_01			,
		C_RCV_2D_01_RETRY	,
		C_RCV_2D_02			,
		C_RCV_2D_03			,
		C_RCV_2D_03_RETRY	,
		C_RCV_2D_04			,
		C_RCV_2D_END		,
		C_RCV_03			,
		C_RCV_04			,
		C_RCV_05			,
		C_RCV_END			,

		C_PCB_INFO_START	= 200,
		C_PCB_INFO_01		,
		C_PCB_INFO_02		,
		C_PCB_INFO_03		,
		C_PCB_INFO_END		,

		C_LOT_MERGE_START	= 300,
		C_LOT_MERGE_01		,
		C_LOT_MERGE_02		,
		C_LOT_MERGE_03		,
		C_LOT_MERGE_END		,

		C_LOT_START_START	= 400,
		C_LOT_START_01		,
		C_LOT_START_02		,
		C_LOT_START_03		,
		C_LOT_START_END		,

		C_LOT_SPLIT_START	= 500,
		C_LOT_SPLIT_01		,
		C_LOT_SPLIT_02		,
		C_LOT_SPLIT_03		,
		C_LOT_SPLIT_END		,

	};

	enum State
	{
		S_IDLE		= 0,
		S_WAIT		,
		S_RCV		,
		S_PCB_INFO	,
		S_LOT_SEND	,
		S_LOT_SPLIT	,
		S_PNP		,
	};

public:
	CRail();
	virtual ~CRail() {}

public:
	CFSM		m_fsm;
	BOOL		m_bRun;

	BOOL		m_bCompRcv;
	BOOL		m_bComp2D;
	BOOL		m_bCompPcbInfo;
	BOOL		m_bCompLotSend;
	BOOL		m_bManualLotIn;
	BOOL		m_bCompLotSplit;
	BOOL		m_bRemove;
	BOOL		m_bLotMgzFirst; // MGZ Change 이후 첫번째 프레임

	CMtAXL*			m_pMtGrip;

	CPneumatic*		m_pCylGripOC;
	CPneumatic*		m_pCylGripFB;

	void AutoRun(void);
	void CycleRun(void);
	void Init(void);

	int& Exist(void);
	int& LotColor(void);

	BOOL IsReadyLdMz(void);
	BOOL IsReadyInPnp(void);

	BOOL IsReadyMtRailXRevEnd();
	BOOL MoveMtRailXRevEnd();
	BOOL IsReadyMtRailXAlign();
	BOOL MoveMtRailXAlign();
	BOOL IsReadyMtRailX2D();
	BOOL MoveMtRailX2D();
	BOOL IsReadyMtRailXRevStart();
	BOOL MoveMtRailXRevStart();

private:
	CTimer		m_tmExistErr;

	BOOL		m_bRdyLdMz;
	BOOL		m_bRdyInPnp;

	int			m_nGripCnt;
	int			m_n2dRetryCnt;
	int			m_nTcRetryCnt;
	int			m_n2DRetryCnt;

	int  GetState(void);
	BOOL IsErr(void);
	int  GetExistErr(void);

	void CycleRcv(void);
	void CyclePcbInfo(void);
	void CycleLotMerge(void);
	void CycleLotStart(void);
	void CycleLotSplit(void);

};


/////////////////////////////////////////////////////////////////////
extern CRail g_rail;
/////////////////////////////////////////////////////////////////////

#endif //_INRAIL_H_
