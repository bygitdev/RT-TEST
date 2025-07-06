#ifndef _INDEX_H_
#define _INDEX_H_

#include <Windows.h>
#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"



class CIndex
{
public:

	enum PosX
	{
		PX_IN_PNP				= 0, 
		PX_ROUTER_PRS			= 1,
		PX_ROUTER_LIVE_VI		= 2,
		PX_ROUTER_RUN			= 3,
		PX_OUT_PNP				= 4,
		PX_OUT_PNP_WAIT			= 5, // 수정 (OutPnp 동작시 회피위치)
		PX_CYL_BIT_SUPPLY_BOX	= 6,
		PX_CYL_BIT_ALIGN_F		= 7,
		PX_CYL_BIT_ALIGN_R		= 8,
		PX_SPD_BIT_CLAMP_F		= 9,
		PX_SPD_BIT_CLAMP_R		= 10,
		PX_SPD_BIT_EJECT		= 11,
		PX_SPD_BIT_EJECT_FORK	= 12,
		PX_MASK_PICKER			= 13, // Auto 및 Mask ADC에 모두 사용
		PX_ADC_STAGE			= 14,
		PX_ADC_MASK				= 15,
		PX_ADC_WAIT				= 16, // 진행중에 z축 간섭으로 Index 회피구간
		PX_ADC_OUTPNP			= 17, // Out Pnp P&P Pos
		PX_ADC_OUT_PICKER		= 18, // Out Picker P&P Pos // 추가
	};

	enum PosT
	{
		PT_IN_PNP				= 0, // Loading
		PT_ROUTER_PRS			= 1, 
		PT_ROUTER_LIVE_VI		= 2,
		PT_ROUTER_RUN			= 3,
		PT_OUT_PNP				= 4,
		PT_OUT_PNP_WAIT			= 5, // 수정 (OutPnp 동작시 회피위치)
		PT_CYL_BIT_SUPPLY_BOX	= 6, // 50개 배열 출 좌측하단으로 셋팅
		PT_CYL_BIT_ALIGN_F		= 7, // Spindle이 Bit을 Clamp 하기전에 bit Align Pos
		PT_CYL_BIT_ALIGN_R		= 8, // Spindle이 Bit을 Clamp 하기전에 bit Align Pos
		PT_SPD_BIT_CLAMP_F		= 9, // Spindle이 Bit을 Clamp 하기위한 Pos
		PT_SPD_BIT_CLAMP_R		= 10, // Spindle이 Bit을 Clamp 하기위한 Pos
		PT_SPD_BIT_EJECT		= 11,
		PT_MASK_PICKER			= 12,
		PT_ADC_STAGE			= 13,
		PT_ADC_MASK				= 14,
		PT_ADC_WAIT				= 15, // 진행중에 z축 간섭으로 Index 회피구간
		PT_ADC_OUTPNP			= 16,
		PT_ADC_OUT_PICKER		= 17, // Out Picker P&P Pos // 추가
	};

	enum Cmd
	{
		C_MASK_PICKUP_START		= 100, 
		C_MASK_PICKUP_01		,
		C_MASK_PICKUP_02		,
		C_MASK_PICKUP_03		,
		C_MASK_PICKUP_END		,

		C_MASK_PUTDN_START		= 200, 
		C_MASK_PUTDN_01			,
		C_MASK_PUTDN_02			,
		C_MASK_PUTDN_03			,
		C_MASK_PUTDN_END		,
	};

	enum State
	{
		S_IDLE						= 0,
		S_IN_PNP					, 
		S_LOAD_CHECK				,
		S_MASK_CLAMP				, 
		S_ROUTER_PRS				,  
		S_ROUTER_LIVE_VI			,  
		S_ROUTER_RUN				,
		S_MASK_UNCLAMP				, 
		S_OUTPNP					,
		S_BIT_SUPPLY_BOX			,
		S_BIT_EJECT_BOX				,
		S_BIT_ALIGN_F				,
		S_BIT_ALIGN_R				,
		S_BIT_SPINDLE_CLAMP_F		,
		S_BIT_SPINDLE_CLAMP_R		,
		S_ADC_WAIT_IN_PNP_PUTDN		,
		S_ADC_WAIT_IN_PNP_PICKUP	,
		S_ADC_WAIT_OUT_PNP_PUTDN	,
		S_ADC_WAIT_OUT_PNP_PICKUP	,
		S_ADC_MASK_PICKER_PICKUP	,
		S_ADC_MASK_PICKER_PUTDN		,
		S_ADC_IDLE					,
	};

	enum PartState
	{
		PS_IDLE					= 0,
		PS_IN_PNP				= 1,
		PS_LOAD_CHECK			= 2, 
		PS_MASK_CLAMP			= 3, 
		PS_ROUTER_PRS			= 4,
		PS_ROUTER_LIVE_VI		= 5, 
		PS_ROUTER_RUN			= 6,
		PS_MASK_UNCLAMP			= 7, 
		PS_OUTPNP				= 8,
	};

public:
	CIndex();
	virtual ~CIndex() {}

public:
	CFSM			m_fsm;
	BOOL			m_bRun;

	CMtAXL*			m_pMtX;
	CMtAXL*			m_pMtT;

	CPneumatic*		m_pCylMaskFB_L;
	CPneumatic*		m_pCylMaskFB_R;
	CPneumatic*		m_pCylMaskUD;
	CPneumatic*		m_pCylMaskPickerUD;
	CPneumatic*		m_pCylMaskPickerOC;
	CPneumatic*		m_pCylDustShutterOC;
	CPneumatic*		m_pSolStageKitOC;
	CPneumatic*		m_pSolStageBlow;

	int				m_nId;
	PRS_RESULT*		m_pPrsResult;

	INDEX_SYS_TEACH*	m_pSysTeach;
	INDEX_MEMORY*		m_pMem;

	GERBER_BLOCK_PRS_POS*	m_pGbPrsBlockPos;
	GERBER_PRS_UNIT_POS*	m_pGbPrsUnitPos;

	POINT2D* m_pRouterOrgPos;

	BOOL	m_bReqBitSupplyPos;
	BOOL	m_bReqBitEjectPos;
	BOOL	m_bReqBitAlignFPos;
	BOOL	m_bReqBitAlignRPos;
	BOOL	m_bReqBitSpdClampFPos;
	BOOL	m_bReqBitSpdClampRPos;

	void AutoRun(void);
	void CycleRun(void);
	void Init(int nNo, INDEX_SYS_TEACH* pSysTeach, INDEX_MEMORY* pIndexMemory, POINT2D* pRouterOrgPos);
	void Init2(PRS_RESULT* pPrsResult);
	void SetHW(CMtAXL* pMtX, CMtAXL* pMtT, CPneumatic* pPmMaskFBL, CPneumatic* pPmMaskFBR, CPneumatic* pPmMaskUD);
	void SetHW2(CPneumatic* pCylMaskPickerUD, CPneumatic* pCylMaskPickerOC);
	void SetHW3(CPneumatic* pSolStageKitOC, CPneumatic* pCylDustShutterOC);
	void SetIndexPosReq(int nReqNo);
		
	int& ExistPcb(void);
	int& ExistScrap(void);

	int& ExistKitStage(void);
	int& ExistKitMask(void);
	int& ExistKitMovePicker(void);
	int& ExistKitMaskPicker(void);

	int& PrsOnceSkip(void);


	BOOL CanMove(int nTargetIndex = -1);

	BOOL IsReadyInPnp(void);
	BOOL IsReadyRouter(void);
	BOOL IsReadyRouterPrs(void);
	BOOL IsReadyLoadCheck(void);
	BOOL IsReadyLiveVision(void);

	BOOL IsReadyBitClamp(void);
	BOOL IsReadyBitAlignF(void);
	BOOL IsReadyBitAlignR(void);
	BOOL IsReadySpdBitClampF(void);
	BOOL IsReadySpdBitClampR(void);
	BOOL IsReadySpdBitEject(void);
	BOOL IsReadyOutPnp(void);

	BOOL IsReadyAdcInPnpPickUpPicker(void);
	BOOL IsReadyAdcInPnpPickUpStage(void);
	BOOL IsReadyAdcInPnpPickUpMask(void);
	BOOL IsReadyAdcInPnpPutDnPicker(void);
	BOOL IsReadyAdcInPnpPutDnStage(void);
	BOOL IsReadyAdcInPnpPutDnMask(void);
	BOOL IsReadyAdcOutPnpPickUpPicker(void);
	BOOL IsReadyAdcOutPnpPutDnPicker(void);

	//////////////////////////////////////////////////
	// nPos -> enum enGerberPos 
	//enum enGerberPos
	//{
	//	POS_START		= 0,
	//	POS_END			= 1,
	//	POS_MID_01		= 2,
	//	POS_MID_02		= 3,
	//	POS_MID_03		= 4,
	//	POS_MID_04		= 5,
	//};
	POINT2D GetRouterPrsPos(int nCnt, int nArrayYCnt = 0); 
	XYT	GetRouterPrsVerifyPos(int nPos);
	XYT GetRouterPos(int nCnt, int nPos);
	XYT	GetRouterLiveViPos(int nCnt, int nPos);
	POINT2D	GetBitBoxSupplyPos(int nCnt);
	
	int GetGerberLineType(int nCnt);
	POINT2D	GetGerberPos(int nCnt, int nPos);
	POINT2D	GetGerberOffset(int nCnt, int nPos);
	POINT2D	GetGerberSubOffset(int nCnt, int nPos);

	POINT2D GetBlockCenCoord(int nBlockNo);
	POINT2D GetMarkCoord(int nCnt);

	BOOL GetBlockPrsResultErr();

	void SetCylMaskFixUD(BOOL bAct);
	BOOL IsCylMaskFixUD(BOOL bAct, int nDelay);
	void SetCylMaskFixFB(BOOL bAct);
	BOOL IsCylMaskFixFB(BOOL bAct, int nDelay);

	BOOL CylIndexMaskFixAct(BOOL bAct); //pmOPEN, pmCLOSE //INIT = -1, OPEN = , CLOSE

	BOOL IsInterfaceSorterIndex(int nIdx);

	BOOL IsReadyMtIndexXInPnp();
	BOOL MoveMtIndexXInPnp();

	BOOL IsReadyMtIndexXOutPnp();
	BOOL MoveMtIndexXOutPnp();



private:
	CTimer		m_tmExistErr;
	CTimer		m_tmExistKitStageErr;
	CTimer		m_tmExistKitMaskErr;
	CTimer		m_tmExistKitMovePickerErr;
	CTimer		m_tmExistKitMaskPickerErr;
	
	BOOL	m_bRdyInPnp;
	BOOL	m_bRdyRouterPrs;
	BOOL	m_bRdyLoadCheck;
	BOOL	m_bRdyRouterLiveVi;
	BOOL	m_bRdyRouterRun;
	BOOL	m_bRdyOutPnp;

	BOOL	m_bRdyRouterCylBitClamp;
	BOOL	m_bRdyRouterCylBitAlignF;
	BOOL	m_bRdyRouterCylBitAlignR;
	BOOL	m_bRdyRouterSpdBitClampF;
	BOOL	m_bRdyRouterSpdBitClampR;
	BOOL	m_bRdyRouterSpdBitEject;

	BOOL	m_bRdyAdcInPnpPickUpPicker;
	BOOL	m_bRdyAdcInPnpPickUpStage;
	BOOL	m_bRdyAdcInPnpPickUpMask;
	BOOL	m_bRdyAdcInPnpPutDnPicker;
	BOOL	m_bRdyAdcInPnpPutDnStage;
	BOOL	m_bRdyAdcInPnpPutDnMask;
	BOOL	m_bRdyAdcOutPnpPickUpPicker;
	BOOL	m_bRdyAdcOutPnpPutDnPicker;


	int  GetState(void);
	BOOL IsErr(void);

	int  GetKitStageExistErr(void);
	int  GetKitMaskExistErr(void);
	int  GetKitMovePickerExistErr(void);
	int  GetKitMaskPickerExistErr(void);

	void SetRouterCurIndex(int nIdx);

	BOOL ExistErr(void);
	BOOL InitCyl(void);

	void CycleRunMaskPickerPickUp(void);
	void CycleRunMaskPickerPutDn(void);



};

void IndexInit(void);


//////////////////////////////////////////////////////////////////////////
extern CIndex g_index01;
extern CIndex g_index02;
extern CIndex g_index03;
extern CIndex g_index04;
//////////////////////////////////////////////////////////////////////////

#endif//_INDEX_H_