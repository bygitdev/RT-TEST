#ifndef _OUTPNP_H_
#define _OUTPNP_H_

#include <Windows.h>
#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"

class COutPnp
{
public:
	enum enType
	{
		PICKUP	= 0,
		PUTDOWN = 1,
	};

	enum PosY
	{
		PY_PICKUP_01		= 0,
		PY_PICKUP_02		= 1,
		PY_PICKUP_03		= 2,
		PY_PICKUP_04		= 3,
		PY_PUTDN_01			= 4,
		PY_PUTDN_02			= 5,
		PY_PUTDN_03			= 6,
		PY_PUTDN_04			= 7,
		PY_SCRAP_EJECT		= 8,
		PY_ADC_KIT_CLAMP	= 9,
	};

	enum PosZ
	{
		PZ_READY			= 0,
		PZ_PICKUP_01		= 1,
		PZ_PICKUP_02		= 2,
		PZ_PICKUP_03		= 3,
		PZ_PICKUP_04		= 4,
		PZ_PUTDN_01			= 5, // Sorter Pos
		PZ_PUTDN_02			= 6, // Sorter Pos
		PZ_PUTDN_03			= 7, // Sorter Pos
		PZ_PUTDN_04			= 8, // Sorter Pos
		PZ_SCRAP_FIX_DW		= 9,
		PZ_PICKUP_RETRY		= 10, 
		PZ_ADC_KIT_CLAMP	= 11, 
		PZ_SLOW_DN_OFFSET	= 12,
	};

	enum PosX
	{
		PX_READY			= 0, // Index Pos // Kit Clamp Pos
		PX_SORTER_01		= 1,
		PX_SORTER_02		= 2,
		PX_SORTER_03		= 3,
		PX_SORTER_04		= 4,
	};

	enum Cmd
	{
		C_PICKUP_START		= 100,
		C_PICKUP_01			,
		C_PICKUP_02			,
		C_PICKUP_03			,
		C_PICKUP_RETRY_1	,
		C_PICKUP_04			,
		C_PICKUP_05			,
		C_PICKUP_06			,
		C_PICKUP_07			,
		C_PICKUP_08			,
		C_PICKUP_09			,
		C_PICKUP_10			,
		C_PICKUP_ERR		,
		C_PICKUP_RETRY_2	,
		C_PICKUP_END		,

		C_PUTDN_START		= 200,
		C_PUTDN_01			,	
		C_PUTDN_02			,		
		C_PUTDN_03			,
		C_PUTDN_04			,
		C_PUTDN_05			,
		C_PUTDN_ERR			,
		C_PUTDN_END			,

		C_SCRAP_EJECT_START	= 300,
		C_SCRAP_EJECT_01	,
		C_SCRAP_EJECT_02	,
		C_SCRAP_EJECT_03	,
		C_SCRAP_EJECT_END	,

		C_ADC_PICKUP_START	= 400, // MGS로 Pickup 인지 PudDn인지 인자 전달
		C_ADC_PICKUP_01		,	
		C_ADC_PICKUP_02		,		
		C_ADC_PICKUP_03		,
		C_ADC_PICKUP_04		,
		C_ADC_PICKUP_05		,
		C_ADC_PICKUP_06		,
		C_ADC_PICKUP_END	,

		C_ADC_PUTDN_START	= 500, // MGS로 Pickup 인지 PudDn인지 인자 전달
		C_ADC_PUTDN_01		,	
		C_ADC_PUTDN_02		,		
		C_ADC_PUTDN_03		,
		C_ADC_PUTDN_04		,
		C_ADC_PUTDN_05		,
		C_ADC_PUTDN_06		,
		C_ADC_PUTDN_END		,
	};


	enum enState
	{
		S_IDLE		 = 0,
		S_INDEX		 = 1,
		S_ADC_PUTDN	 = 2,
		S_ADC_PICKUP = 3,
	};


public:
	COutPnp();
	virtual ~COutPnp(){}

public:
	CFSM			m_fsm;
	BOOL			m_bRun;

	CMtAXL*			m_pMtY;
	CMtAXL*			m_pMtZ;
	CMtAXL*			m_pMtX; // VGrip
		
	CPneumatic*		m_pCylScrapUD;
	CPneumatic*		m_pCylScrapOC_F;
	CPneumatic*		m_pCylScrapOC_R;

	CPneumatic*		m_pCylScrapFixUD_F;
	CPneumatic*		m_pCylScrapFixUD_R;
	CPneumatic*		m_pVac;
	CPneumatic*		m_pVacEject;
	CPneumatic*		m_pSolKitClampOC;

	int				m_nCurIndex;  // 0: Index01, 1: Index02, 2: Index03, 3: Index04, 
	int				m_nNextIndex; // 0: Index01, 1: Index02, 2: Index03, 3: Index04, 
	int				m_nRetryPcbInfo;
	
	void AutoRun();
	void CycleRun(void);
	void Init(void);

	int& ExistScrap(void);
	int& ExistPcb(void);
	int& ExistKit(void);
	int& KitJobType(void);


	void SetCylScrapFixUD(BOOL bAct, BOOL bType = PICKUP, BOOL bLog = FALSE);
	BOOL IsCylScrapFixUD(BOOL bAct, BOOL bType = PICKUP, BOOL bLog = FALSE);
	void SetCylScrapOC(BOOL bAct);
	BOOL IsCylScrapOC(BOOL bAct);
	BOOL AnalogVac(BOOL bAct);


	void SetSorterStageNo(int nIdx, int nReq);
	BOOL IsReadySorterIndex(int nIdx);
	BOOL IsReadySorterPnp(int nIdx);

	BOOL IsReadyMtOutPnpYPickUp(int nIdx);
	BOOL MoveMtOutPnpYPickUp(int nIdx);
	BOOL IsReadyMtOutPnpYPutDn(int nIdx);
	BOOL MoveMtOutPnpYPutDn(int nIdx);
	BOOL IsReadyMtOutPnpXSorter(int nIdx);
	BOOL MoveMtOutPnpXSorter(int nIdx);

	BOOL IsReadyMtOutPnpZPickUp(int nIdx);
	BOOL MoveMtOutPnpZPickUp(int nIdx);
	
	// Z Vel Change Pos
	BOOL IsReadyMtOutPnpZOverride(int nMtIdx);
	BOOL MoveMtOutPnpZOverride(int nMtIdx);

private:
	CTimer			m_tmExistErrScrap;
	CTimer			m_tmExistErrPcb;
	CTimer			m_tmExistErrKit;
	CTimer			m_tmOutPnpCycle;
	CTimer			m_tmOutPnpSafety;

	int		m_nQcCnt;
	int		m_nPickUpErrNo;
	int		m_nPickUpRetryFirst;
	int		m_nPickUpRetry;

	int  GetState(void);
	BOOL IsErr(void);
	int  GetExistErrScrap(void);
	int  GetExistErrPcb(void);
	int  GetExistErrKit(void);

	void SetNextIndex(void);
	void SetStageBusy(int nIdx);

	void CycleRunPickUp(void);
	void CycleRunPcbPutDn(void);
	void CycleRunScrapEject(void);
	void CycleRunAdcPickUp(void);
	void CycleRunAdcPutDn(void);



};


//////////////////////////////////////////////////////////////////////////
extern COutPnp g_outPnp;
//////////////////////////////////////////////////////////////////////////


#endif