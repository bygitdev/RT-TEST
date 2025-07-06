#ifndef _INPNP_H_
#define _INPNP_H_

#include <Windows.h>
#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"

class CInPnp
{
public:
	enum PosY
	{
		PY_RAIL				= 0,
		PY_PUTDN_01			= 1,
		PY_PUTDN_02			= 2,
		PY_PUTDN_03			= 3,
		PY_PUTDN_04			= 4,
		PY_ADC_STAGE_01		= 5,
		PY_ADC_STAGE_02		= 6,
		PY_ADC_STAGE_03		= 7,
		PY_ADC_STAGE_04		= 8,
		PY_ADC_MASK_01		= 9,
		PY_ADC_MASK_02		= 10,
		PY_ADC_MASK_03		= 11,
		PY_ADC_MASK_04		= 12,
		PY_ADC_RAIL			= 13,
		PY_ADC_OUTPNP		= 14, // Out Picker P&P Pos // 추가
	};


	enum PosW
	{
		PW_RAIL_OPEN			= 0,
		PW_RAIL_PICKUP			= 1,
		PW_ADC_OPEN				= 2,
		PW_ADC_STAGE_PICKUP		= 3,
		PW_ADC_MASK_PICKUP		= 4,
		PW_ADC_PICKER_PICKUP	= 5,
	};


	enum PosZ
	{
		PZ_READY				= 0,
		PZ_PCB_RAIL				= 1,
		PZ_PCB_PUTDN_01			= 2,
		PZ_PCB_PUTDN_02			= 3,
		PZ_PCB_PUTDN_03			= 4,
		PZ_PCB_PUTDN_04			= 5,
		PZ_ADC_RAIL_STAGE		= 6,
		PZ_ADC_RAIL_MASK		= 7,
		PZ_ADC_RAIL_PICKER		= 8,
		PZ_ADC_STAGE_01			= 9,
		PZ_ADC_STAGE_02			= 10,
		PZ_ADC_STAGE_03			= 11,
		PZ_ADC_STAGE_04			= 12,
		PZ_ADC_MASK_01			= 13,
		PZ_ADC_MASK_02			= 14,
		PZ_ADC_MASK_03			= 15,
		PZ_ADC_MASK_04			= 16,
		PZ_SLOW_DN_OFFSET		= 17, // 설정값 이전위치에서 감속도 진행됨 (모든 Z Pos에 적용) 
		PZ_ADC_PICKER_INDEX		= 18, // 인덱스 1번에서 Out Picker를 Pickup/PutDown 하는 위치

	};


	enum Cmd
	{
		C_PCB_PICKUP_START		= 100, 
		C_PCB_PICKUP_01			,
		C_PCB_PICKUP_02			,
		C_PCB_PICKUP_03			,
		C_PCB_PICKUP_END		,

		C_PCB_PUTDN_START		= 200,
		C_PCB_PUTDN_01			,
		C_PCB_PUTDN_02			,
		C_PCB_PUTDN_03			,
		C_PCB_PUTDN_END			,

		C_ADC_RAIL_PICKUP_START	= 300,
		C_ADC_RAIL_PICKUP_01	,	
		C_ADC_RAIL_PICKUP_02	,		
		C_ADC_RAIL_PICKUP_03	,
		C_ADC_RAIL_PICKUP_04	,
		C_ADC_RAIL_PICKUP_END	,

		C_ADC_RAIL_PUTDN_START	= 400,
		C_ADC_RAIL_PUTDN_01		,	
		C_ADC_RAIL_PUTDN_02		,		
		C_ADC_RAIL_PUTDN_03		,
		C_ADC_RAIL_PUTDN_04		,
		C_ADC_RAIL_PUTDN_END	,
				
		C_ADC_INDEX_PICKUP_START= 500,
		C_ADC_INDEX_PICKUP_01	,	
		C_ADC_INDEX_PICKUP_02	,		
		C_ADC_INDEX_PICKUP_03	,
		C_ADC_INDEX_PICKUP_04	,
		C_ADC_INDEX_PICKUP_END	,

		C_ADC_INDEX_PUTDN_START	= 600,
		C_ADC_INDEX_PUTDN_01	,	
		C_ADC_INDEX_PUTDN_02	,		
		C_ADC_INDEX_PUTDN_03	,
		C_ADC_INDEX_PUTDN_04	,
		C_ADC_INDEX_PUTDN_END	,
	};


	enum State
	{
		S_IDLE				= 0,
		S_RAIL				,
		S_INDEX_PUTDN		,
		S_ADC_RAIL_PICKUP	,
		S_ADC_RAIL_PUTDN	,
		S_ADC_INDEX_PICKUP	,
		S_ADC_INDEX_PUTDN	,
	};

public:
	CInPnp();
	virtual ~CInPnp(){}

public:
	CFSM			m_fsm;
	BOOL			m_bRun;

	CMtAXL*			m_pMtW;
	CMtAXL*			m_pMtY;
	CMtAXL*			m_pMtZ;
		
	void AutoRun();
	void CycleRun(void);
	void Init(void);

	int& ExistPcb(void); // 자재 존재여부

	int& ExistKit(void); // Kit Clamp 여부
	int& KitJobType(void); 
	int& KitInfo(void);  

	BOOL IsReadyMtInPnpYPcbPutDn(int nIdx);
	BOOL MoveMtInPnpYPcbPutDn(int nIdx);
	
	// Z Vel Change Pos
	BOOL IsReadyMtInPnpZOverride(int nMtIdx);
	BOOL MoveMtInPnpZOverride(int nMtIdx);

private:
	CTimer			m_tmExistErr;

	int				m_nPutDnIndex; // 0: Index01, 1: Index02, 2: Index03, 3: Index04, 

	int  GetState(void);
	BOOL IsErr(void);
	int  GetExistErr(void);
	int  GetExistKitErr(void);

	void CycleRunPcbPickUp(void);
	void CycleRunPcbPutDn(void);
	void CycleRunAdcRailPickUp(void);
	void CycleRunAdcRailPutDn(void);
	void CycleRunAdcIndexPickUp(void);
	void CycleRunAdcIndexPutDn(void);
	
	int	 GetPcbPutDnIndex(void);

};


//////////////////////////////////////////////////////////////////////////
extern CInPnp g_inPnp;
//////////////////////////////////////////////////////////////////////////


#endif