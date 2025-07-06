#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <Windows.h>
#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"


class CRouter
{
public:
	enum PosY
	{
		//////////////////////////////////////////////////////////////////////////
		// Ready Pos = 0으로 Setting
		// Front Gentry = 0, Rear Gentry = pos 확인, 일단은 0으로 Setting
		// 두 축간의 Ready Pos에서 두 축간의 거리 확인 후 DDM에 Setting
		// (gentryYDisF, gentryYDisR)
		// 개별 Pos
		PY_READY							= 0,
		PY_CYL_BIT_CLAMP_RED				= 1, 
		PY_CYL_BIT_ALIGN_RED_F				= 2, // Index에 안착
		PY_CYL_BIT_ALIGN_RED_R				= 3, 		

		//////////////////////////////////////////////////////////////////////////
		// Position은 Motor 동기화 이후에 Setup
		// Front Pos
		PY_VI_PRS_F							= 5, 
		PY_VI_LIVE_F						= 6, 
		PY_ROUTER_F							= 7, 
		PY_SPD_BIT_EJECT_01_03_RED_F		= 8,
		PY_SPD_BIT_CLAMP_01_03_RED_F		= 9,
		PY_SPD_WIRE_CHECK_F					= 10, // ESD
		PY_SPD_BIT_VI_F						= 11, 
		PY_SPD_BIT_VERIFY_F					= 12, 
		PY_SPD_BIT_EJECT_BLUE_02			= 13,
		PY_SPD_BIT_CLAMP_BLUE_02_R			= 14,

		// Rear Pos	
		PY_VI_PRS_R							= 15,
		PY_VI_LIVE_R						= 16, 
		PY_ROUTER_R							= 17,
		PY_SPD_BIT_EJECT_02_04_RED_R		= 18,
		PY_SPD_BIT_CLAMP_02_04_RED_R		= 19,
		PY_SPD_WIRE_CHECK_R					= 20,
		PY_SPD_BIT_VI_R						= 21,
		PY_SPD_BIT_VERIFY_R					= 22, 
		PY_SPD_BIT_EJECT_BLUE_03			= 23,
		PY_SPD_BIT_CLAMP_BLUE_03_R			= 24,

		PY_CYL_BIT_CLAMP_BLUE				= 26,
		PY_CYL_BIT_ALIGN_BLUE_F				= 27,
		PY_CYL_BIT_ALIGN_BLUE_R				= 28,

		PY_SPD_BIT_COLOR_F					= 30,
		PY_SPD_BIT_COLOR_R					= 31,
	};


	enum PosZ
	{
		PZ_READY						= 0, // 최상위 Up Pos
		PZ_MOVE_UP						= 1, // Routing 일 때 Up Pos
		PZ_MOVE_DW						= 2, // Routing 일 때 Down Pos
		PZ_PRS							= 3,
		PZ_SPD_BIT_EJECT_RED			= 4,
		PZ_SPD_BIT_CLAMP_RED			= 5,
		PZ_SPD_EDS_CHECK				= 6,
		PZ_SPD_BIT_VI					= 7, 
		PZ_SPD_BIT_VERIFY				= 8, 
		PZ_SPD_BIT_EJECT_MID_UP_RED		= 9,
		PZ_SLOW_DN_OFFSET				= 10,

		PZ_SPD_BIT_EJECT_BLUE			= 12,
		PZ_SPD_BIT_CLAMP_BLUE			= 13,
		PZ_SPD_BIT_EJECT_MID_UP_BLUE	= 14,
 	};


	enum PosW
	{
		PW_READY				= 0, // Home 한 위치
		PW_ROUTER				= 1, // Router 연산 위치
	};
		
	enum enCmd
	{
		C_ROUTER_START			= 100,
		C_ROUTER_FRONT			,
		C_ROUTER_REAR			,
		C_ROUTER_03				,
		C_ROUTER_04				,
		C_ROUTER_05				,
		C_ROUTER_END			,

		C_LIVE_VI_START			= 200,
		C_LIVE_VI_RST			,
		C_LIVE_VI_FRONT			,
		C_LIVE_VI_REAR			,
		C_LIVE_VI_03			,
		C_LIVE_VI_END			,

		C_SPD_BIT_EJECT_START	= 300,
		C_SPD_BIT_EJECT_01		,	
		C_SPD_BIT_EJECT_02		,		
		C_SPD_BIT_EJECT_03		,
		C_SPD_BIT_EJECT_04		,
		C_SPD_BIT_EJECT_05		,
		C_SPD_BIT_EJECT_ERR		,
		C_SPD_BIT_EJECT_END		,

		C_SPD_BIT_CLAMP_START	= 400,
		C_SPD_BIT_CLAMP_01		,	
		C_SPD_BIT_CLAMP_02		,		
		C_SPD_BIT_CLAMP_03		,
		C_SPD_BIT_CLAMP_04		,
		C_SPD_BIT_CLAMP_05		,
		C_SPD_BIT_CLAMP_ERR		,
		C_SPD_BIT_CLAMP_END		,

		C_SPD_ESD_CHECK_START	= 500, 
		C_SPD_ESD_CHECK_01		,	
		C_SPD_ESD_CHECK_02		,		
		C_SPD_ESD_CHECK_03		,
		C_SPD_ESD_CHECK_04		,
		C_SPD_ESD_CHECK_RETRY	,
		C_SPD_ESD_CHECK_END		,

		C_CYL_BIT_CLAMP_START	= 600,
		C_CYL_BIT_CLAMP_01		,	
		C_CYL_BIT_CLAMP_02		,		
		C_CYL_BIT_CLAMP_03		,
		C_CYL_BIT_CLAMP_04		,
		C_CYL_BIT_CLAMP_END		,

		C_CYL_COLOR_CLAMP_START	= 700,
		C_CYL_COLOR_CLAMP_01		,	
		C_CYL_COLOR_CLAMP_END		,

		C_CYL_BIT_ALIGN_START	= 800,
		C_CYL_BIT_ALIGN_01		,	
		C_CYL_BIT_ALIGN_02		,		
		C_CYL_BIT_ALIGN_03		,
		C_CYL_BIT_ALIGN_04		,
		C_CYL_BIT_ALIGN_END		,

		C_SPD_BIT_VI_START		= 900,
		C_SPD_BIT_VI_01			,	
		C_SPD_BIT_VI_02			,		
		C_SPD_BIT_VI_03			,
		C_SPD_BIT_VI_04			,
		C_SPD_BIT_VI_05			,
		C_SPD_BIT_VI_06			,
		C_SPD_BIT_VI_END		,

		C_SPD_BIT_VERIFY_START	= 1000,
		C_SPD_BIT_VERIFY_01		,	
		C_SPD_BIT_VERIFY_02		,		
		C_SPD_BIT_VERIFY_03		,
		C_SPD_BIT_VERIFY_04		,
		C_SPD_BIT_VERIFY_END	,

		C_PRS_START				= 1100, // Router에서 3Point Prs
		C_PRS_INIT				, 
		C_PRS_RST				,
		C_PRS_MOVE				,
		C_PRS_TRIG				,
		C_PRS_END				,

		C_LDC_START				= 1200,
		C_LDC_INIT				, 
		C_LDC_RST				,
		C_LDC_MOVE				,
		C_LDC_TRIG				,
		C_LDC_END				,
	};


	enum State
	{
		S_IDLE					= 0,
		S_READY					,
		S_ROUTER				, // Live Vision 포함
		S_SPD_BIT_EJECT			,
		S_SPD_BIT_CLAMP_F		,
		S_SPD_BIT_CLAMP_R		,
		S_SPD_WIRE_CHECK		, // 통전검사 + 카본 전류측정
		S_CYL_BIT_CLAMP			,
		S_CYL_BIT_ALIGN_F		,
		S_CYL_BIT_ALIGN_R		,
		S_SPD_BIT_COLOR			,
		S_SPD_BIT_VI			, // 동심 확인
		S_SPD_BIT_BROKEN_CHECK	, // 부러짐 및 아래로 쳐짐 Check
	};


public:
	CRouter();
	virtual ~CRouter(){}

public:
	CFSM			m_fsm;
	BOOL			m_bRun;

	CMtAXL*			m_pMtY;
	CMtAXL*			m_pMtW;
	CMtAXL*			m_pMtZ_F;
	CMtAXL*			m_pMtZ_R;
	
	CIndex*			m_pIndexF;
	CIndex*			m_pIndexR;

	CPneumatic*		m_pCylBitClampUD;
	CPneumatic*		m_pCylBitClampOC;
	CPneumatic*		m_pSolSpdChuckOC_F;
	CPneumatic*		m_pSolSpdChuckOC_R;
	CPneumatic*		m_pSolSpindleBlow;
	CPneumatic*		m_pSolRouterIonizerF;
	CPneumatic*		m_pSolRouterIonizerR;
	CPneumatic*		m_pSpindleF; // Spindle을 pm 처럼 사용
	CPneumatic*		m_pSpindleR;

	int		m_nId;					// Router Part Front/Rear 구분

	BIT_INFO*	m_pInfoBitF;
	BIT_INFO*	m_pInfoBitR;

	int		m_nCurIndex; // 0: IndexFront, 1: IndexRear
	int		m_nNextIndex; // 0: IndexFront, 1: IndexRear

	int		m_nBitBrokenErrCntF;
	int		m_nBitBrokenErrCntR;
	int		m_nBitDownErrCntF;
	int		m_nBitDownErrCntR;

	int		m_nBitChangeIdx;
	int		m_nBitWireIdx;
	int		m_nBitCurIdx;
	int		m_nBitVisionIdx;
	int		m_nBitBrokenIdx;

	int		m_nBitAlignRetry;


	// Router 조건 확인
	BOOL	m_bNeedESDCheckF;
	BOOL	m_bNeedESDCheckR;
	BOOL	m_bNeedBitChangeF;
	BOOL	m_bNeedBitChangeR;
	BOOL	m_bNeedBitColorF;
	BOOL	m_bNeedBitColorR;
	BOOL	m_bNeedBitVisionF;
	BOOL	m_bNeedBitVisionR;
	BOOL	m_bNeedBitBrokenCheckF;
	BOOL	m_bNeedBitBrokenCheckR;

	BOOL	m_bReqReadyPos;
	BOOL	m_bReqRouterBitReadyPos;

	POINT2D	m_viPrsData;

	void AutoRun();
	void CycleRun(void);

	void InitNomal(int nNo, CIndex* pIndexF, CIndex* pIndexR);
	void InitNv(BIT_INFO* pInfoBitF, BIT_INFO* pInfoBitR);
	void InitMt(CMtAXL* pMtY, CMtAXL* pMtW, CMtAXL* pMtZ_F, CMtAXL* pMtZ_R);
	void InitPm1(CPneumatic* pPmBitClampUD, CPneumatic* pPmBitClampOC);

	void InitPm3(CPneumatic* pPmSolSpinldeChuckOC_F, CPneumatic* pPmSolSpinldeChuckOC_R);
    void InitPm4(CPneumatic* pSolSpindleBlow);
    void InitPm5(CPneumatic* pSolRouterIonizerF, CPneumatic* pSolRouterIonizerR);
	void InitPm6(CPneumatic* pSpindleF, CPneumatic* pSpindleR);

	int& IsPartUseSkip(void);
	int& IsIndexFUseSkip(void);
	int& IsIndexRUseSkip(void);
	int& ExistCylBitClamp(void);
	int& ExistCylBitAlignRedF(void);
	int& ExistCylBitAlignRedR(void);
	int& ExistCylBitAlignBlueF(void);
	int& ExistCylBitAlignBlueR(void);

	int& flagSpindleESDCheckF(void);
	int& flagSpindleESDCheckR(void);

	int& flagSpidleBitChangeF(void);
	int& flagSpidleBitChangeR(void);

	BOOL IsReadyReadyPos(void); // router Part의 공유 Pos를 회피하기 위한 조건확인

	BOOL GentryMtYWMove(int nIndexY, int nIndexW);
	BOOL GentryMtYWPMove(int nIndexY, int nIndexW, double dPosY, double dPosW, double dVel = 0);
	BOOL IsGentryMtYWRdy(int nIndexY, int nIndexW);
	BOOL IsGentryMtYWPRdy(int nIndexY, int nIndexW, double dPosY, double dPosW);

	double GetBitYOffset();
	double& HeightDistBrokenToFlowDown(int nSpindleNo);

	int	 GetBitChangeIndex(void);
	int	 GetBitWireIndex(void);
	int	 GetBitVisionIndex(void);
	int	 GetBitBrokenIndex(void);

	double GetZStepPos(int nSpindle); // ROUTER_F, ROUTER_R
	
	BOOL   IsSpindle2PinPitch(); // ROUTER_PART_F, ROUTER_PART_R

	// Z Vel Change Pos
	BOOL IsReadyMtSpindleZOverrideF(int nMtIdx);
	BOOL MoveMtSpindleZOverrideF(int nMtIdx);
	BOOL IsReadyMtSpindleZOverrideR(int nMtIdx);
	BOOL MoveMtSpindleZOverrideR(int nMtIdx);


private:
	CTimer m_tmRouterCycle;
	CTimer m_tmExistBitAlignFErr;
	CTimer m_tmExistBitAlignRErr;

	int  m_nLiveViPos;
	BOOL m_bTestPrs;

	POINT2D	m_viPrsBlock[BLOCK_PT_MAX];
	BOOL m_bReadyPos;
	int  m_nPrsBlockCnt;
	int  m_nPrsRetry;
	int  m_nPrsArrayYRetry; // PRS Fail시에 Y 방향 Pitch 이동하여 PRS 진행

	int  m_nLdcBlockCnt;
	int  m_nLdcArrayY;

	int  m_nESDRetry;
	int  m_nErrESD;

	int  GetState(void);
	BOOL IsErr(void);
	int  GetExistBitAlignFErr(void);
	int  GetExistBitAlignRErr(void);

	void CycleRunRouter(void);
	void CycleRunLiveVision(void);
	void CycleRunSpdBitEject(void);
	void CycleRunSpdBitClamp(void);
	void CycleRunSpdESDCheck(void);
	void CycleRunCylBitClamp(void);
	void CycleRunCylColorClamp(void);
	void CycleRunCylBitAlign(void);
	void CycleRunSpdBitVision(void);
	void CycleRunSpdBitVerify(void);
	void CycleRunPrs(void);
	void CycleRunLoadCheck(void);

	BOOL IsMtRdy(void);

	BOOL CreatePrsData(int nIdx, int nArrayYCnt = 0);
	BOOL CreatePrsDataVerify(int nIdx);

	BOOL GetPrsDataLimit(void);
	BOOL GetLdcDataLimit(void);

};


void RouterInit(void);

//////////////////////////////////////////////////////////////////////////
extern CRouter g_routerF; // 앞단
extern CRouter g_routerR; // 뒷단
//////////////////////////////////////////////////////////////////////////


#endif