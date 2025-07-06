#ifndef _USERDATADEF_H_
#define _USERDATADEF_H_

#include <windows.h>

#define UNIT_MAX		(10) 
#define UNIT_PT_MAX		(UNIT_MAX*2) 
#define BLOCK_PT_MAX	(4)
#define DEFAULT_VI_VAL	(-999)
#define RFID_FAIL_MAX	(3) 
#define GRIPPER_RETRY_MAX (3) 


enum enCommState
{
	STATE_IDLE		= 0,
	STATE_REQ		= 1,
	STATE_BUSY		= 2,
	STATE_COMP		= 3,
	STATE_ERR		= 4,
	STATE_PART_ERR	= 5,
};


enum enCommViResult
{
	RESULT_IDLE	= 0,
	RESULT_GOOD = 1,
	RESULT_NG	= 2,
};


enum enSorterInterface
{
	REQ_SORTER_NONE		= 0,
	REQ_SORTER_PNP		= 1, // PutDown
	REQ_SORTER_STAGE	= 2, // Pickup, Scrap, QC 까지 
};


enum enMachineState
{
	MC_STATE_STOP	  = 0,
	MC_STATE_AUTO	  = 1,
	MC_STATE_ERROR	  = 2,
	MC_STATE_CYCLE	  = 3,
	MC_STATE_EMG	  = 4,
	MC_STATE_RUN_DOWN = 5,
};


enum enLotInfo
{
	LOT_INFO_MGZ			= 0,
	LOT_INFO_RAIL			= 1,
	LOT_INFO_INPNP			= 2,
	LOT_INFO_INDEX01		= 3,
	LOT_INFO_INDEX02		= 4,
	LOT_INFO_INDEX03		= 5,
	LOT_INFO_INDEX04		= 6,
	LOT_INFO_OUTPNP			= 7,
	LOT_INFO_OLD_RAIL		= 8,  // 이전 작업한 마지막 Rail Lot 정보
	LOT_INFO_OLD_OUTPNP		= 9,  // Sorter에 전송되어진 마지막 Lot 정보
	LOT_INFO_OLD_MGZ		= 10, // Oht 사용시 진입단에서 확인해야 함
	LOT_INFO_CONV_ARRIVAL	= 11, // MGZ 배출시 정보
	LOT_INFO_CONV_BUFFER1	= 12,
	LOT_INFO_CONV_BUFFER2	= 13,
};


enum enIndex
{
	INDEX_IDLE	= -1,
	INDEX_01	= 0,
	INDEX_02	= 1,
	INDEX_03	= 2,
	INDEX_04	= 3,
};


enum enSpindle
{
	SPINDLE_IDLE	= -1,
	SPINDLE_01		= 0,
	SPINDLE_02		= 1,
	SPINDLE_03		= 2,
	SPINDLE_04		= 3,
};


enum enRouterFR
{
	ROUTER_FR_IDLE	= -1,
	ROUTER_F		= 0,
	ROUTER_R		= 1,
};


enum enRouterPart
{
	ROUTER_PART_IDLE	= -1,
	ROUTER_PART_F		= 0,
	ROUTER_PART_R		= 1,
};


enum enIndexFR
{
	INDEX_FR_IDLE	= -1,
	INDEX_F			= 0,
	INDEX_R			= 1,
};


enum enGerberPos
{
	POS_START		= 0,
	POS_END			= 1,
	POS_MID_01		= 2,
	POS_MID_02		= 3,
	POS_MID_03		= 4,
	POS_MID_04		= 5,
};


enum enLR
{
	L = 0,
	R = 1,
	LR = 2,
};


enum enAdc1DState
{
	ADC_1D_IDLE	= 0,
	ADC_1D_TRIG	= 1,
	ADC_1D_BUSY	= 2,
	ADC_1D_COMP	= 3,
	ADC_1D_ERR	= 4,
};


enum enIndexReqPosNo
{
	REQ_BIT_IDLE			= 0,
	REQ_BIT_SUPPLY_BOX		= 1,
	REQ_BIT_EJECT_BOX		= 2,
	REQ_BIT_ALIGN_F			= 3,
	REQ_BIT_ALIGN_R			= 4,
	REQ_BIT_SPINDLE_CLAMP_F	= 5,
	REQ_BIT_SPINDLE_CLAMP_R	= 6,
};


enum enAdcRailState
{
	ADC_RAIL_IDLE		= 0,
	ADC_RAIL_PICKUP		= 1,
	ADC_RAIL_PUSH		= 2,
};


enum enAdcKitInfo
{
	ADC_KIT_IDLE		= 0,
	ADC_KIT_STAGE_01	= 1,
	ADC_KIT_STAGE_02	= 2,
	ADC_KIT_STAGE_03	= 3,
	ADC_KIT_STAGE_04	= 4,
	ADC_KIT_MASK_01		= 5,
	ADC_KIT_MASK_02		= 6,
	ADC_KIT_MASK_03		= 7,
	ADC_KIT_MASK_04		= 8,
	ADC_KIT_PICKER		= 9,
};


enum enAdcMzPickerNo
{
	ADC_MZ_PICKER_IDLE	= -1,
	ADC_MZ_PICKER_01	= 0,
};

// 순서 뒤바뀌면 안됨. ndm에 동일한 순서의 Exist가 있음
enum enAdcMzKitNo
{
	ADC_MZ_KIT_IDLE			 = -1,
	ADC_MZ_TOP_KIT_STAGE_01  = 0,
	ADC_MZ_TOP_KIT_STAGE_02  = 1,
	ADC_MZ_TOP_KIT_STAGE_03  = 2,
	ADC_MZ_TOP_KIT_STAGE_04  = 3,
	ADC_MZ_TOP_KIT_MASK_01   = 4,
	ADC_MZ_TOP_KIT_MASK_02   = 5,
	ADC_MZ_TOP_KIT_MASK_03   = 6,
	ADC_MZ_TOP_KIT_MASK_04   = 7,
	ADC_MZ_TOP_KIT_PICKER	 = 8,
	ADC_MZ_BTM_KIT_STAGE_01  = 9,
	ADC_MZ_BTM_KIT_STAGE_02  = 10,
	ADC_MZ_BTM_KIT_STAGE_03  = 11,
	ADC_MZ_BTM_KIT_STAGE_04  = 12,
	ADC_MZ_BTM_KIT_MASK_01   = 13,
	ADC_MZ_BTM_KIT_MASK_02   = 14,
	ADC_MZ_BTM_KIT_MASK_03   = 15,
	ADC_MZ_BTM_KIT_MASK_04   = 16,
	ADC_MZ_BTM_KIT_PICKER	 = 17,
};


enum enAdcZPosType
{
	POS_TYPE_ALIGN		= 0, 
	POS_TYPE_GRIP_DN	= 1, 
	POS_TYPE_GRIP_UP	= 2, 
};


enum enJobType
{
	JOB_TYPE_IDLE	= 0, 
};


enum enLotStartType
{
	LOT_IDLE	= 0, 
	LOT_NOMAL	= 1, 
	LOT_SPLIT	= 2, 
};

enum enOptionState
{
	S_SKIP		= 0,
	S_USE		= 1,
};


typedef struct tagMCOperation
{
	BOOL isEmg;
	BOOL isStop;
	BOOL isAuto;
	BOOL isPausedStop;
	BOOL isDoorOpen;
	BOOL isDoorUnlock;
	BOOL isSafetyBeam;
	BOOL isCycleRun;
	BOOL isAllHome;
	BOOL isDryRun;
}MCOPR;

// Vision Live 기준 좌측 우측 값
typedef struct tagSystemTeach
{
	double dViX1;
	double dViY1;
	double dViX2;
	double dViY2;
}SYS_TEACH;


typedef struct tagIndexSystemTeach
{
	SYS_TEACH inPnp;
	SYS_TEACH router;
	SYS_TEACH qc;
}INDEX_SYS_TEACH;


typedef struct tagPrsResult
{
	XYT	block;
	POINT2D shrinkage;
	XYT	unit[UNIT_MAX];
}PRS_RESULT;


typedef struct tagIndexMemory
{
	int state;
	int routerCmdCnt;
	int routerCurCnt;
	int compMaskClamp;
	int compRouterPrs;
	int compRouterLiveVi;
	int compRouterRun;
	int compMaskUnClamp;
	int compLoadCheck;
	int compPRSFail;
	int compOutPnp;
}INDEX_MEMORY;


// mmi -> seq Gerber Data 
typedef struct _Gerber
{
	// ㄷ  형태로 동작할 수도 있으므로 Cut Point Up Down 않고 동작해야 함
	double dLineType; // 0: Line, 1: ㄷ, 2: 원호 (사용안함)
	double dX_Start;  // 거버 시작점 x
	double dY_Start;  // 거버 시작점 y
	double dX_End;    // 거버 끝점 x
	double dY_End;    // 거버 끝점 y
	double dX_Mid_01;  
	double dY_Mid_01;
	double dX_Mid_02;
	double dY_Mid_02;
	double dX_Mid_03;
	double dY_Mid_03;
	double dX_Mid_04;
	double dY_Mid_04;
	double dX_Arc;    // 거버 원호 중간점 x
	double dY_Arc;    // 거버 원호 중간점 y
	double dSpare16;
	double dSpare17;
	double dSpare18;
	double dSpare19;
	double dSpare20;
}GERBER;


typedef struct _RouterData
{
	GERBER rtData[100];
}ROUTER_DATA, PATH_OFFSET;


typedef struct _GerberPath
{
	GERBER data[500];
}GERBER_PATH;


typedef struct _UnitPrsInfo
{
	double dX01;  
	double dY01;  
	double dX02;    
	double dY02;
}UNIT_PRS_INFO;


typedef struct _GerberUnitPrsPos
{
	UNIT_PRS_INFO prsUnitPos[UNIT_MAX]; // Unit Max 20ea 
}GERBER_PRS_UNIT_POS;


typedef struct _GerberBlockPrsPos
{
	POINT2D	ptXY[BLOCK_PT_MAX];
}GERBER_BLOCK_PRS_POS;


typedef struct _BitInfo
{
	int nExist;	 // Bit 존재여부
	int nLength; // 현재 사용 Length
	int nZStep;  // 현재 사용 Z Step
	int nSpare04;
	int nSpare05;
}BIT_INFO;



#endif//_USERDATADEF_H_