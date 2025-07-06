#ifndef _LDMZL_H_
#define _LDMZL_H_

#include <Windows.h>
#include "..\..\BASE\BaseAll.h"

#define MAX_LOT_COLOR (9)

class CLdMz
{
public:
	enum PosY
	{
		PY_RAIL			= 0,
		PY_RCV			= 1,
		PY_EJECT		= 2,
		PY_RFID			= 3,
		PY_ALIGN		= 4,
	};

	enum PosZ
	{
		PZ_RCV			= 0,
		PZ_CLAMP_DN		= 1,
		PZ_RCV_BWD		= 2,
		PZ_RAIL			= 3,
		PZ_EJECT		= 4,
		PZ_CLAMP_UP		= 5,
		PZ_EJECT_BWD	= 6,
		PZ_RFID			= 7,
		PZ_ALIGN		= 8,
	};

	enum PosPusherX
	{
		PX_BWD			= 0,
		PX_FWD			= 1,
		PX_2D			= 2, // 사용하지 않지만 소스 유지 (선 푸셔 기능 옵션에선 사용)
	};

	enum Cmd
	{
		C_LOADING_START			= 100,
		C_LOADING_01			,
		C_LOADING_MOVE_FWD		,
		C_LOADING_MOVE_BWD		,
		C_LOADING_END			,

		C_ALIGN_START			= 200,
		C_ALIGN_01				,
		C_ALIGN_END				,

		C_RFID_READ_START		= 300,
		C_RFID_READ_01			,
		C_RFID_READ_02			,
		C_RFID_READ_END			,

		C_CARRIER_ID_READ_START	= 400,
		C_CARRIER_ID_READ_01	,
		C_CARRIER_ID_READ_02	,
		C_CARRIER_ID_READ_END	,

		C_MERGE_INFO_START		= 500,
		C_MERGE_INFO_01			,
		C_MERGE_INFO_02			,
		C_MERGE_INFO_END		,

		C_TRAY_INFO_START		= 600,
		C_TRAY_INFO_01			,
		C_TRAY_INFO_02			,
		C_TRAY_INFO_END			,

		C_PUSHER_START			= 700,
		C_PUSHER_CHK_COMP		,
		C_PUSHER_END			,

		C_RFID_WRITE_START		= 800,
		C_RFID_WRITE_01			,
		C_RFID_WRITE_02			,
		C_RFID_WRITE_03			,
		C_RFID_WRITE_END		,

		C_RFID_WRITE_CHECK_START= 900,
		C_RFID_WRITE_CHECK_01	,
		C_RFID_WRITE_CHECK_02	,
		C_RFID_WRITE_CHECK_03	,
		C_RFID_WRITE_CHECK_END	,

		C_EJECT_START			= 1000,
		C_EJECT_MOVE_FWD		,
		C_EJECT_END				,

		C_PART_NO_COMPARE_START	= 1100,
		C_PART_NO_COMPARE_01	,
		C_PART_NO_COMPARE_02	,
		C_PART_NO_COMPARE_03	,
		C_PART_NO_COMPARE_END	,

		C_AUTO_RECIPE_CHG_START	= 1200,
		C_AUTO_RECIPE_CHG_01	,
		C_AUTO_RECIPE_CHG_02	,
		C_AUTO_RECIPE_CHG_03	,
		C_AUTO_RECIPE_CHG_04	,
		C_AUTO_RECIPE_CHG_05	,
		C_AUTO_RECIPE_CHG_END	,
	};

	enum State
	{
		S_IDLE					= 0,
		S_LOADING_EXCEPTION		,
		S_EJECT_EXCEPTION		,
		S_LOADING				,
		S_ALIGN					,
		S_RFID_READ				,
		S_PART_NO_COMPARE		,    // Part No가 다른지를 비교
		S_AUTO_RECIPE_CHG		,    // Part No가 다를시 Auto Change를 위해 대기
		S_CARRIER_ID_READ		,
		S_MERGE_INFO			,
		S_TRAY_INFO				,
		S_WORK					,
		S_RFID_WRITE			,
		S_RFID_WRITE_CHECK		,
		S_EJECT					,
	};

public:
	CLdMz();
	virtual ~CLdMz() {}

public:
	CFSM			m_fsm;
	BOOL			m_bRun;
	
	BOOL			m_bManualOut;

	CMtAXL*			m_pMtZ;
	CMtAXL*			m_pMtY;
	CMtAXL*			m_pMtX; // Pusher

	CPneumatic*		m_pCylAlignFB;
	CPneumatic*		m_pCylClampOC;

	void AutoRun(void);
	void CycleRun(void);
	void Init(void);

	int& Exist(void);
	int& CmdSlotNo(void);
	int& CurSlotNo(void);

	double GetMzZSlotPos(int nNo);
	double GetMzYSlotPos(int nNo);

	BOOL IsReadyMtPusherXFwd();
	BOOL MoveMtPusherXFwd();
	BOOL IsReadyMtPusherX2D();
	BOOL MoveMtPusherX2D();
	BOOL	m_bNewRailInfo;

private:
	CTimer	m_tmExistErr;
	CTimer	m_tmMouthSen;
	CTimer	m_tmMouthSenWarn;
	CTimer  m_tmPusherWarn;

	BOOL	m_bReworkMz;
	BOOL	m_bCompAlign;
	BOOL	m_bCompRfidRead;
	BOOL	m_bCompPartNoCompare;
	BOOL	m_bCompAutoRecipeChg;
	BOOL	m_bCompCarrierIdRead;
	BOOL	m_bCompMergeInfo;
	BOOL	m_bCompTrayInfo;
	BOOL	m_bCompWork;
	BOOL	m_bNewMz;
	BOOL	m_bCompRfidWrite;
	BOOL	m_bCompRfidWriteCheck;

	int		m_nRfidRetryCnt;
	int		m_nTcRetryCnt;

	int		GetState(void);
	BOOL	IsErr(void);
	int		GetExistErr(void);

	void CycleLoading(void);
	void CycleMzAlign(void);
	void CycleReadRfid(void);
	void CyclePartNoCompare(void);
	void CycleAutoRecipeChg(void);
	void CycleCarrierIdRead(void);
	void CycleMergeInfo(void);
	void CycleTrayInfo(void);
	void CyclePusher(void);
	void CycleRfidWrite(void);
	void CycleRfidWriteCheck(void);
	void CycleEject(void);

	BOOL CanMtYMove(void);
};


/////////////////////////////////////////////////////////////////////
extern CLdMz g_ldMz;
/////////////////////////////////////////////////////////////////////







#endif// _INRAIL_H_