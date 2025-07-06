#ifndef _PKGDATA_H_
#define _PKGDATA_H_


enum PKG
{
	//////////////////////////////////////////////////////////////////////////
	//   0~ 99 : 시스템 고유설정 사용 
	// 100~299 까지 User 노출 Data
	// 100~149 : Count 
	// 150~199 : Time (단위 :sec)
	// 200~249 : Pitch (단위 : mm)
	// 250~299 : 기타
	
	//////////////////////////////////////////////////////////////////////////
	// 100~149 : Count 
	mzSlotZCnt		= 100,
	bitZStepCnt		= 101, // Bit 날 사용시 재사용 Z Count
	unitCnt			= 102,
	mzSlotYCnt		= 103,

	OutPnpVacSetValue    = 110,
	OutPnpVacRcpUseSkip  = 111,
	
	//////////////////////////////////////////////////////////////////////////
	// 150~199 : Time (단위 :sec)
	optionSocammUseBlueBit	= 165,


	//////////////////////////////////////////////////////////////////////////
	// 200~249 : Pitch (단위 : mm)
	mzSlotZOffset		 = 200,
	mzSlotYOffset		 = 201,

	// gerber file <-> Stage Jig 와의 시프트된 오차 거리
	gerberJigOffsetX	 = 201, // 확인 후 삭제
	gerberJigOffsetY	 = 202, // 확인 후 삭제
	gerberJigOffsetT	 = 203, // 확인 후 삭제
	LoaderZRailPosOffset = 204, // 추가
	bitZStepPitch		 = 205, // 비트 Step 별 사용 길이
	bit1CutLength		 = 206, // ddm에서 이동 추가 
	
	pcbXLength					= 210, // 연산 기준이 되는 자재 Length 추가
	pcbUnitCenterToEdgeXLength	= 211, // PCB 내부 Unit의 중심에서 우측 Edge의 거리

	//////////////////////////////////////////////////////////////////////////
	// 250~299 : 기타
	adcKitJobType		= 250, // 현재 Job에 맞는 adc Kit의 고유 No
	                           // 1개의 Kit에 여러개의 Job이 매칭될 수 있음
	optionLotSplit		= 251, // Lot Split 사용여부 옵션

	//////////////////////////////////////////////////////////////////////////
	// 300 ~ 499 : Offset
	// 개별 Parameter Offset 처리 기준 Data로 연산되도록 수정
	// 신규 Device Setup시에 Offset Data는 Clear 되어야 함
	offsetLdMzPusherXFwd  = 300, // Loader
	offsetLdMzPusherX2D	  = 301, // Loader

	PcbLengthOptionUse = 303, // 옵션 사용유무.

	offsetRailXRcvEnd	  = 310, // Rail
	offsetRailXAlign	  = 311, // Rail
	offsetRailX2D		  = 312, // Rail
	offsetRailXRcvStart	  = 313, // Rail

	offsetInPnpYPutDn1	  = 320, // InPnp
	offsetInPnpYPutDn2	  = 321, // InPnp
	offsetInPnpYPutDn3	  = 322, // InPnp
	offsetInPnpYPutDn4	  = 323, // InPnp
	
	offsetInPnpZPutDn1	  = 330, // InPnp
	offsetInPnpZPutDn2	  = 331, // InPnp
	offsetInPnpZPutDn3	  = 332, // InPnp
	offsetInPnpZPutDn4	  = 333, // InPnp

	offsetIndexXInPnp1	  = 340, // Index
	offsetIndexXInPnp2	  = 341, // Index
	offsetIndexXInPnp3	  = 342, // Index
	offsetIndexXInPnp4	  = 343, // Index
	offsetIndexXOutPnp1	  = 344, // Index
	offsetIndexXOutPnp2	  = 345, // Index
	offsetIndexXOutPnp3	  = 346, // Index
	offsetIndexXOutPnp4	  = 347, // Index

	offsetOutPnpYPickUp1  = 380, // OutPnp //ok
	offsetOutPnpYPickUp2  = 381, // OutPnp
	offsetOutPnpYPickUp3  = 382, // OutPnp
	offsetOutPnpYPickUp4  = 383, // OutPnp
	offsetOutPnpYPutDn1   = 384, // OutPnp //ok
	offsetOutPnpYPutDn2   = 385, // OutPnp
	offsetOutPnpYPutDn3   = 386, // OutPnp
	offsetOutPnpYPutDn4   = 387, // OutPnp

	offsetOutPnpZPickUp1  = 390, // OutPnp //ok
	offsetOutPnpZPickUp2  = 391, // OutPnp
	offsetOutPnpZPickUp3  = 392, // OutPnp
	offsetOutPnpZPickUp4  = 393, // OutPnp
	offsetOutPnpZPutDn1   = 394, // OutPnp //ok
	offsetOutPnpZPutDn2   = 395, // OutPnp
	offsetOutPnpZPutDn3   = 396, // OutPnp
	offsetOutPnpZPutDn4   = 397, // OutPnp

	offsetOutPnpXSorter1   = 400, // OutPnp //ok
	offsetOutPnpXSorter2   = 401, // OutPnp
	offsetOutPnpXSorter3   = 402, // OutPnp
	offsetOutPnpXSorter4   = 403, // OutPnp

	SpindleESD_Value_01    = 411,
	SpindleESD_Value_02	   = 412,
	SpindleESD_Value_03	   = 413,
	SpindleESD_Value_04	   = 414,

	//////////////////////////////////////////////////////////////////////////
	// 900 ~ 1000 : mmi 전용 PKG Para 
	underCutUse			= 900,  // 0:Not Use , 1:Use 
	underCutLength		= 901,  // Under Cutting 깊이

	maxPKG				= 1000,
};


//-------------------------------------------------------------------
// Double Type Data Memory
enum DDm
{
	/////////////////////////////////////////////////////////////////////
	//   0~ 99 : 시스템 고유설정 사용 
	// 100~299 까지 User 노출 Data
	// 100~149 : Count 
	// 150~199 : Time (단위 :sec)
	// 200~249 : Pitch (단위 : mm)
	// 250~299 : 기타
	
	/////////////////////////////////////////////////////////////////////
	//   0~ 99 : 시스템 고유설정 사용 
	uph				= 0, // 시간당 생산량
	machineTime		= 1,
	

	/////////////////////////////////////////////////////////////////////
	// AInput 시그널 (Volt)
	srcOutPnpFlow		=8,
	srcOutPnpReg		=9,

	srcIonizer01		=10,
	srcIonizer02		=11,
	srcIonizer03		=12,
	srcIonizer04		=13,
	srcMainAir			=14,
	srcIndexFlow01		=15,
	srcIndexFlow02		=16,
	srcIndexFlow03		=17,
	srcIndexFlow04		=18,

	//Main 화면 등 디스플레이
	cycleTmRouterCutting1	= 20, // Router Cutting Down -> Up
	cycleTmRouterCutting2	= 21, // Router Cutting Down -> Up
	cycleTmRouterCutting3	= 22, // Router Cutting Down -> Up
	cycleTmRouterCutting4	= 23, // Router Cutting Down -> Up
	cycleTmOutPnp			= 24, // Out Pnp Put Down -> Put Down

	/////////////////////////////////////////////////////////////////////
	// AIO Diplay를 위한 기준값 저장
	IonizerRunUnit01		= 30,
	IonizerRunVolt01		= 31,
	IonizerRunUnit02		= 32,
	IonizerRunVolt02		= 33,
	IonizerRunUnit03		= 34,
	IonizerRunVolt03		= 35,
	IonizerRunUnit04		= 36,
	IonizerRunVolt04		= 37,
	indexFlowRunUnit01		= 38,
	indexFlowRunVolt01		= 39,
	indexFlowRunUnit02		= 40,
	indexFlowRunVolt02		= 41,
	indexFlowRunUnit03		= 42,
	indexFlowRunVolt03		= 43,
	indexFlowRunUnit04		= 44,
	indexFlowRunVolt04		= 45,
	MainAirRunUnit			= 46,
	MainAirRunVolt			= 47,
	OutPnpRegAirRunUnit		= 48,
	OutPnpRegAirRunVolt		= 49,
	OutPnpAirFlowRunUnit	= 50,
	OutPnpAirFlowRunVolt	= 51,

	aiChkTrg				= 90,  // 2021-11-24 ai chk용, 1은 1st, 2는 2nd
	aiChkNum				= 91,
	aiChkInput_ai_data1		= 92,
	aiChkInput_display1		= 93,
	aiChkInput_ai_data2		= 94,
	aiChkInput_display2		= 95,
	aiChkInput_init_data	= 96,
	aiChkInput_ratio_data	= 97,

	/////////////////////////////////////////////////////////////////////
	// 100~149 : Count 
	indexBitBoxXCnt		= 100,
	indexBitBoxYCnt		= 101,
	lotMergeQty			= 102, // Lot Merge 시에 Max 수량 (800)
							   // 현 작업 Lot이 799이고 Setting이 800개 이면 그 MGZ은 모두 Merge 하도록 한다. (800++)
	lotMergeLimitQty	= 103, // Merge Max Lot (1200) // 추가

	lotCurQty			= 105, 
	lotQtyCheckTime		= 106, 

	OutPnpVacCommon		= 110,
	/////////////////////////////////////////////////////////////////////
	// 150~199 : Time (단위 : sec)
	lotMergeEndTime			= 150, // 일정시간 경과후 Empty Lot End을 위한 Time
//	ohtCallRetryTime		= 151, // 일정시간 경과후 Oht Call Retry Time
//	ohtPioSafetyWaitTime	= 152, // PIO 통신 충 알람 발생을 위한 대기 시간
//	ohtPioStatusChgWaitTime	= 153, // 설비가 Stop 상태이므로 Oht를 받지 않겠다는 대기 신호
	ScarpBoxSafetyTime		= 154, // Alarm -> Error 요청으로 타임조정으로 변경
	OutPnPPickUpTime		= 155,
	OutPnPPlaceTime			= 156,
	


	/////////////////////////////////////////////////////////////////////
	// 200~249 : Pitch (단위 : mm)
	adcMzMaskSlotPitch	= 200,
	adcMzStageSlotPitch = 201,

	bitMaxLifeLength	= 203,
	 
	spindle1ZLimitPos	= 205, // 추가
	spindle2ZLimitPos	= 206, // 추가
	spindle3ZLimitPos	= 207, // 추가
	spindle4ZLimitPos	= 208, // 추가

	commonPcbLength		= 210, // M.2 963 Device가 기준이며 길이 입력 추가
	commonPcbUnitCenterToEdgeXLength	= 211, // M.2 963 Device가 기준

	spindleResistanceLowLimit   = 249, // 멀티미터 측정 Low Limit 값 (단위 : 옴) 
	/////////////////////////////////////////////////////////////////////
	// 250~299 : 기타
	spindleResistanceLimit		= 250, // 멀티미터 측정 Limit 값 (단위 : 옴)

	// Prs 검증을 위한 오차 범위
	// 해당 오차값 Fail시 Retry
	routerPrsVerfyLimitX		= 251,
	routerPrsVerfyLimitY		= 252,
	
	indexBitBoxXPitch			= 253,
	indexBitBoxYPitch			= 254,
	
	prsResultLimitX				= 255,
	prsResultLimitY				= 256,
	prsResultLimitT				= 257,

	routerVelStart				= 260, // Vel mm/sec
	routerVelEnd				= 261, // Vel mm/sec
	routerVelViStart			= 262, // Vel mm/sec
	routerVelViEnd				= 263, // Vel mm/sec

	prsVisionOffsetLimitX		= 264, // Vision Prs 결과값 Limit
	prsVisionOffsetLimitY		= 265, // Vision Prs 결과값 Limit

	indexAirFlowMinValue1		= 266,
	indexAirFlowMinValue2		= 267,
	indexAirFlowMinValue3		= 268,
	indexAirFlowMinValue4		= 269,

	ldcVisionOffsetLimitX		= 270, 
	ldcVisionOffsetLimitY		= 271, 

	SpindelTimeCountF			= 280, 
	SpindelTimeCountR			= 281, 
	
	spindle1DistBrokenToFlowdown = 290,
	spindle2DistBrokenToFlowdown = 291,
	spindle3DistBrokenToFlowdown = 292,
	spindle4DistBrokenToFlowdown = 293,

	OutPnpRetryCnt				 = 295,

	/////////////////////////////////////////////////////////////////////
	// Gentry Y Pos (두 축간의 거리 Y 값) 
	gentryYDisF			= 300, // Router Front Part
	gentryYDisR			= 301, // Router Rear Part


	/////////////////////////////////////////////////////////////////////
	// POINT2D
	// 700 ~ Router Front Bit 의 Index 중심위치
	routerOrgPos01		= 320,	// x, y 확인 후 컨셉 정리되면 SysTeach에 포함
	routerOrgPos02		= 322,	// x, y 
	routerOrgPos03		= 324,	// x, y
	routerOrgPos04		= 326,  // x, y


	/////////////////////////////////////////////////////////////////////
	// mmi에서 받는 Data 
	mmiReadSpindleResistance	= 350,
	mmiReadViBtmDis				= 351, // Bit Btm 검사 동심원 지름


	/////////////////////////////////////////////////////////////////////
	// 600 ~ Vision Prs Pos 연산용 SysTeach Pos
	//typedef struct tagIndexSystemTeach
	//{
	//	SYS_TEACH inPnp;
	//	SYS_TEACH router;
	//	SYS_TEACH qc;
	//}INDEX_SYS_TEACH;
	sysTeach01			= 400,
	sysTeach02			= 420,
	sysTeach03			= 440,
	sysTeach04			= 460,


	/////////////////////////////////////////////////////////////////////
	// 800 ~ Prs Result
	//typedef struct tagPrsResult
	//{
	//	XYT	block;
	//	POINT2D shrinkage;
	//	XYT	unit[UNIT_MAX];
	//}PRS_RESULT;
	prsResult01			= 500,
	prsResult02			= 550,
	prsResult03			= 600,
	prsResult04			= 650,
	

	/////////////////////////////////////////////////////////////////////
	// 900~949 : MMI 사용
	mcTimeStop			= 900, // 시퀀스에서 사용 안함 
	mcTimeError			= 901, 
	mcTimeRun			= 902, 


	SetBlush01			= 920,
	SetBlush02			= 921,

	SetOutPnpPadLifeCount	= 922, 

	ActBrush01			= 960,
	ActBrush02			= 961,

	ActOutPnpPadLifeCount	= 962, 

	maxDDM				= 1000,
};


//-------------------------------------------------------------------
// Int Type Data Memory
enum NDm
{
	//////////////////////////////////////////////////////////////////////////
	// 0 ~ 99 : 시스템 고유설정 사용 (Main 스크린 및 옵션 사용)
	// User 노출 Data 없음
	jobNo					= 0,
	groupNo					= 1,
	screenNo				= 2,
	stateMachine			= 3, // (enMachineState)
	initViewChange			= 4, // 전체 이니셜시에 화면 전환 flag

	stateJobNameCheck		= 5, // Sorter와 Job Name Check 하여 Error 발생
	adcKitOldJobType		= 6, // 이전 Device의 Old Job Type

	autoRecipeChgJobNo		= 10, // Auto Device Change 시퀀스진행시 mmi에서 Change 해야할 Job No 추가
	autoRecipeChgGroupNo	= 11, // Auto Device Change 시퀀스진행시 mmi에서 Change 해야할 Grop No 추가

	/////////////////////////////////////////////////////////////////////
	// MMi Main Speed Btn
	// 20 ~ : mmi screen option
	mmiBtnElevLock			= 20, // Auto 중 MGZ Strip 공급 Lock
	mmiBtnBuzzerLock		= 21, // Buzzer Lock
	mmiBtnEjectLdMz			= 22, // MGZ 강제 배출 (텐키에 연결)
	mmiBtnRework			= 23, // 다음 MGZ이 rework 매거진임을 알림
	mmiBtnAdcMode			= 24, // On후 Run시에 ADC 진행
	mmiAllHomeComp			= 25, // All Home 이 완료되었다는 변수

	mmiSpindle01BitChange	= 30, // 사용자 메뉴얼 1회성 Bit Change
	mmiSpindle02BitChange	= 31,
	mmiSpindle03BitChange	= 32,
	mmiSpindle04BitChange	= 33,
	
	mmiIndex01PrsOnceSkip	= 34, // 사용자 메뉴얼 1회성 Prs Skip
	mmiIndex02PrsOnceSkip	= 35, // 사용자 메뉴얼 1회성 Prs Skip
	mmiIndex03PrsOnceSkip	= 36, // 사용자 메뉴얼 1회성 Prs Skip
	mmiIndex04PrsOnceSkip	= 37, // 사용자 메뉴얼 1회성 Prs Skip


	/////////////////////////////////////////////////////////////////////
	// 50 ~ : 자재 유무 확인

	existMzLoadZArrival		= 45,
	existMzLoadZExist		= 46,
	existMzInStopper01		= 47, // 수정
	existMzInStopper02		= 48, // 수정
	existMzInStopper03		= 49, // 수정

	existMzOutBuffer2		= 53,
	existMzOutBuffer1		= 54,
	existMzOutArrival		= 55,
//							= 55,
//							= 56,
	existMzOutTemp			= 57,
	existLdMz				= 58,
	existRail				= 59,
	existInPnp				= 60,
	existIndex01			= 61,
	existIndex02			= 62,
	existIndex03			= 63,
	existIndex04			= 64,
	existIndexScrap01		= 65,
	existIndexScrap02		= 66,
	existIndexScrap03		= 67,
	existIndexScrap04		= 68,
	existOutPnpScrap		= 69,
	existOutPnpPcb			= 70,
	existRouterCylBitF		= 71, // Bit Change를 위한 Clamp의 bit 존재여부
	existRouterCylBitR		= 72,
	existRedIndexBitAlign01	= 73, // Bit Change를 위한 Align Part의 bit 존재여부
	existRedIndexBitAlign02	= 74, 
	existRedIndexBitAlign03	= 75, 
	existRedIndexBitAlign04	= 76,

	existBlueIndexBitAlign01 = 77, // Bit Change를 위한 Aling Part의 bit 존재여부
	existBlueIndexBitAlign02 = 78, 
	existBlueIndexBitAlign03 = 79,
	existBlueIndexBitAlign04 = 80,

	existInPnpClampKit		= 83, // InPnp의 Clamp Kit 유무		
						 

	/////////////////////////////////////////////////////////////////////
	// 96 ~ : Tenkey Mode
	tenKeyJog				= 96,	// tenkey jog mode인지 확인용
	jogAxisNo				= 97,	// 0~
	jogSpeed				= 98,	// um
	screenTenkey			= 99,

	/////////////////////////////////////////////////////////////////////
	// 100 ~ : pio sensor mmi display (설비별로 io no가 틀리므로 ndm으로 구성)
// 	pioDisplayiOhtValId		= 100,
// 	pioDisplayiOhtCs0		= 101,
// 	pioDisplayiOhtCs1		= 102,			
// 	pioDisplayiOhtCs2		= 103,			
// 	pioDisplayiOhtCs3		= 104,			
// 	pioDisplayiOhtTrReq		= 105,			
// 	pioDisplayiOhtBusy		= 106,			
// 	pioDisplayiOhtCompt		= 107,							
	
// 	pioDisplayoOhtLdReq		= 110,			
// 	pioDisplayoOhtUldReq	= 111,			
// 	pioDisplayoOhtAbort		= 112,			
// 	pioDisplayoOhtReady		= 113,

// 	pioOutputoOhtLdReq		= 120,			
// 	pioOutputoOhtUldReq		= 121,			
// 	pioOutputoOhtAbort		= 122,			
// 	pioOutputoOhtReady		= 123,

//	pioOutputReset			= 130,		

	/////////////////////////////////////////////////////////////////////
	// 150 ~ : Kit Exist Index
	existKitMaskPicker01	= 146, // 추가
	existKitMaskPicker02	= 147, // 추가
	existKitMaskPicker03	= 148, // 추가
	existKitMaskPicker04	= 149, // 추가
	existKitStage01			= 150,
	existKitStage02			= 151,
	existKitStage03			= 152,
	existKitStage04			= 153,
	existKitMask01			= 154,
	existKitMask02			= 155,
	existKitMask03			= 156,
	existKitMask04			= 157,
	existKitOutPnp			= 158,
	existKitMovePicker		= 159, // Index 1번이 OutPicker를 이동시킴

	/////////////////////////////////////////////////////////////////////
	// 160 ~ 180 : Kit Exist MGZ
	existAdcRail			= 160, 
	existAdcTopMzStage01	= 161, 
	existAdcTopMzStage02	= 162, 
	existAdcTopMzStage03	= 163, 
	existAdcTopMzStage04	= 164, 
	existAdcTopMzMask01		= 165, 
	existAdcTopMzMask02		= 166, 
	existAdcTopMzMask03		= 167, 
	existAdcTopMzMask04		= 168, 
	existAdcTopMzPicker		= 169, 
	existAdcBtmMzStage01	= 170,
	existAdcBtmMzStage02	= 171,
	existAdcBtmMzStage03	= 172,
	existAdcBtmMzStage04	= 173,
	existAdcBtmMzMask01		= 174,
	existAdcBtmMzMask02		= 175,
	existAdcBtmMzMask03		= 176,
	existAdcBtmMzMask04		= 177,
	existAdcBtmMzPicker		= 178,
	
	/////////////////////////////////////////////////////////////////////
	//210 ~ : mmi read router data 개별 동작을 위해서 추가
	mmiRouterIndexNo		= 210, // Index 1, 2, 3, 4
	mmiRouterPathNo			= 211, // Cutting Path No
	mmiRouterPathPos		= 212, // Cutting Pos (Start, End, Mid)
	
	/////////////////////////////////////////////////////////////////////
	//213 ~ : mmi Vision Result
	mmiInPnpPrsErr			= 213, // 0 : Nomal, 1 : Error
	mmiRouterFErr			= 214, // 0 : Nomal, 1 : Error
	mmiRouterRErr			= 215, // 0 : Nomal, 1 : Error
	
	mmiBtmViErr				= 217, // 0 : Nomal, 1 : Error

	mmiNeedSpindleESD		= 220, // Spindle ESD Check -> Bit Change 이후 오전 6시에 Reset 1회/1일 검사
	                               // Nomal:0, 1:검사요청

	/////////////////////////////////////////////////////////////////////
	//230 ~ : mmi state req read count
	lotSplitCount			= 230, // Lot Split 요청시 Lot 분리를 요청하는 모 Count
	                               // A Lot 200 개 중 150 분리요청시 A Lot은 150개, A-1 Lot은 


	/////////////////////////////////////////////////////////////////////
	// 250 ~ : mmi <-> seq Comm State (enCommState)
//	stateOhtInCall			= 250, // Full Carrier Move Req
//	stateOhtOutCall			= 251, // Empty Carrier Move Req
	stateLotMerge			= 253, // 사용 안함
	stateRfidRead			= 254, // 
	stateRfidWrite			= 255,
	stateRfidWriteCheck		= 256, // 추가	
	statePmsTop				= 257, //2D Pcb Info Router Req (Top 실패시 Btm Rear)
	statePmsBtm				= 258, //2D Pcb Info Router Req
	stateSpindleESDCheck	= 259,
	stateAdc1D				= 260, 
	stateOutPnpInfo			= 261, // OutPnp 동작시 Sorter에 전달해야 할 정보요청
	stateCarrierIdRead		= 262, // RFID Data 전송
	stateLotMergeInfo		= 263, // MergeInfo Req (MGZ 정보)
	stateManualLotIn		= 264, 
	stateLotStart			= 265, // Lot Start (첫번재 Frame 만 전달)
	statePcbInfo			= 266, // 2D Pms 전송
	stateLotInfoLog			= 267, // Lot End Log Save // 추가
	stateTrayInfoReq		= 268, // Tray Info Req    // 추가
	statePortInfoChg		= 269, // Port Info Change (Port 내의 MGZ 상태 전송)
	stateEqpStatusChange	= 270, // Eqp Status Change (?)
	stateVersionUpdate		= 271, // Version Update (?)

	stateSeeLot				  = 273, // SeeLot
	statePioStatusChgInStart  = 274, // Pio Status Chg (MGZ을 받을 것을 전송)
	statePioStatusChgOutStart = 275, // Pio Status Chg (MGZ을 가져갈 것을 전송)
	statePioStatusChgInStop	  = 276, // Pio Status Chg (MGZ을 받지 않을 것을 전송)
	statePioStatusChgOutStop  = 277, // Pio Status Chg (MGZ을 가져가지 않을 것을 전송)

	statePartNoCompare		= 278, // Auto Device Change를 위한 Part No 비교 기능
	                                 // 0=idle, 1=req, 2=busy, 3=comp(partNo동일), 
	                                 // 4=err(PartNo다름 모든디바이스에 없음), 
	                                 // 5=Auto Change (PartNo가 동일 그룹에 존재함), 
	stateAutoRecipeChg		= 279, // Auto Device Change Step
	stateLotSplit			= 280, // Lot Split Req 추가
	stateSplitLotStart		= 281, // Lot Split Lot ID로 Lot Start Req 추가

	outPnpScrapExisDelayTime = 299, // scrap exist x1800 센서 튀는 현상으로 인해 딜레이 추가
	outPnpPcbIndex			= 300, // PCB가 몇번 Index에서 Pickup 한 것인지 기억
	spindleESDNo			= 301, // 몇번 Spindle이 ESD 검사를 한것인지.. Log 생성시 사용


	spindleSpeedUpload0102	= 310, // Spindle Speed Server Upload Req = 1
	spindleSpeedUpload0304	= 311, // Spindle Speed Server Upload Req = 2
	prsDataLog12			= 312, // 0: Nomal, Index No Write 1, 2 // 추가
	prsDataLog34			= 313, // 0: Nomal, Index No Write 3, 4 // 추가

	/////////////////////////////////////////////////////////////////////
	// 320 ~ : Spindle ESD Check
	flagSpindleESDCheck01	= 320, // Spindle ESD Check 해야 한다는 변수 1회/1일
	flagSpindleESDCheck02	= 321,
	flagSpindleESDCheck03	= 322,
	flagSpindleESDCheck04	= 323,


	/////////////////////////////////////////////////////////////////////
	// 330 ~ : flag
	flagAllLotClear			= 330,
	flagMtIndexOffsetMove	= 331, // Mt Index Move시 Offset 적용 유무
	                               // Motor 창에서는 0, Offset Setup 창에서는 1 Write
	flagViTcpReconnect		= 332, // Vision 연결신호를 IO로 Vision에 신호
	flagSplitIDLotStart		= 333, // 추가 Split Lot ID로 Lot Change

	flagSplitInfo			= 335, // 추가 Split Lot ID를 MMI에서 Rail단에 Write 할 수 있또록 함

	flagLotMergeComp		= 340, // 추가 LotMerge 완료시 SplitLot 진행을 위한 시그널

	/////////////////////////////////////////////////////////////////////
	// 내부 Memory 기억변수 
//	ohtPioStatusChgInStart	= 350, // 0 : PioStop 상태, 1 : 통신재개 PIO_RESTART 시그널 전송 완료
//	ohtPioStatusChgOutStart	= 351, // 0 : PioStop 상태, 1 : 통신재개 PIO_RESTART 시그널 전송 완료

//	ohtECMRComp				= 352,
//	ohtFCMRComp				= 353,

	ldMzCmdSlotNo			= 360, // 추가 loader Step 정보를 수정이 필요할 때가 있어 추가함
	ldMzCurSlotNo			= 361, // 추가

	ldMzInSwOn				= 370,
	ldMzOutSwOn				= 371,

	/////////////////////////////////////////////////////////////////////
	// 내부 Memory 기억변수 (typedef struct tagIndexMemory)
	indexMemory01			= 400,
	indexMemory02			= 420,
	indexMemory03			= 440,
	indexMemory04			= 460,

	/////////////////////////////////////////////////////////////////////
	// Bit 정보 (typedef struct _SpindleBit)
	routerBitInfo01			= 510,
	routerBitInfo02			= 515,
	routerBitInfo03			= 520,
	routerBitInfo04			= 525,

	/////////////////////////////////////////////////////////////////////
	// Bit Box 정보
	// Max 50 개 중 몇개를 사용하고 남았는지 수량으로만 표시 (49) = Max, (-1) = Empty)
	// 설비 전면 기준 좌하단부터 Start
	// 04 03 02 01 00
	// ~  ~  ~  ~  ~
	// 49 48 47 46 45

	RedindexBitBoxCurCnt01				= 530, // Index01
	RedindexBitBoxCurCnt04				= 531, // Index04

	RedindexBitSupplyBoxClear01			= 532, // Bit Supply Box를 Off->On 했다는 변수
	RedindexBitSupplyBoxClear04			= 533, // Bit Supply Box를 Off->On 했다는 변수

	BlueindexBitBoxCurCnt02				= 535, // Index02
	BlueindexBitBoxCurCnt03				= 536, // Index03

	BlueindexBitSupplyBoxClear02		= 537, // Bit Supply Box를 Off->On 했다는 변수
	BlueindexBitSupplyBoxClear03		= 538, // Bit Supply Box를 Off->On 했다는 변수
	
	/////////////////////////////////////////////////////////////////////
	// 600 ~ : ADC Job No + Kit Info
	adcMzTopJobType			= 600,
	adcMzBtmJobType			= 610, 
	
	adcRailKitJobType		= 620, // InPnp Pickup을 위한 2D reading Data
	adcRailKitInfo			= 621, // (enum enAdcKitInfo)
	adcRailState			= 622, // (enum enAdcRailState)
	adc1DJobType			= 623, 
	adc1DKitInfo			= 624, // (enum enAdcKitInfo)

	adcIndex01StageJobType		= 630,
	adcIndex02StageJobType		= 631,
	adcIndex03StageJobType		= 632,
	adcIndex04StageJobType		= 633,
	adcIndex01MaskJobType		= 634,
	adcIndex02MaskJobType		= 635,
	adcIndex03MaskJobType		= 636,
	adcIndex04MaskJobType		= 637,
	adcOutPnpJobType			= 638,
	adcIndexMovePickerJobType	= 639,

	adcInPnpKitJobType			= 700, 
	adcInPnpKitInfo				= 701, // (enum enAdcKitInfo)

//	ohtInCallComp				= 710, // 내부 기억 변수 
//	ohtOutCallComp				= 711, // 내부 기억 변수 
	needAutoRecipeChg			= 712, // 내부 기억 변수 Auto Device Chg가 필요하다는 변수

	setupmode					= 777,

	/////////////////////////////////////////////////////////////////////
	// 800~899 : Gem Event로 사용
	// enum enCommState 로 상태 주고받음
	gemMzLoadingStart			= 800,
	gemMzLoadingEnd				= 801,
	gemInRailPcbReadStart		= 802,
	gemInRailPcbReadEnd			= 803,
	gemInPnpPcbPickupStart		= 804,
	gemInPnpPcbPickupEnd		= 805,

	gemStagePcbPrs01Start		= 810,
	gemStagePcbPrs02Start		= 811,
	gemStagePcbPrs03Start		= 812,
	gemStagePcbPrs04Start		= 813,
	gemStagePcbPrs01End			= 814,
	gemStagePcbPrs02End			= 815,
	gemStagePcbPrs03End			= 816,
	gemStagePcbPrs04End			= 817,
	gemStageRouter01Start		= 818,
	gemStageRouter02Start		= 819,
	gemStageRouter03Start		= 820,
	gemStageRouter04Start		= 821,
	gemStageRouter01End			= 822,
	gemStageRouter02End			= 823,
	gemStageRouter03End			= 824,
	gemStageRouter04End			= 825,
	gemBitChange01Start			= 826,
	gemBitChange02Start			= 827,
	gemBitChange03Start			= 828,
	gemBitChange04Start			= 829,
	gemBitChange01End			= 830,
	gemBitChange02End			= 831,
	gemBitChange03End			= 832,
	gemBitChange04End			= 833,
	gemBitVision01Start			= 834,
	gemBitVision02Start			= 835,
	gemBitVision03Start			= 836,
	gemBitVision04Start			= 837,
	gemBitVision01End			= 838,
	gemBitVision02End			= 839,
	gemBitVision03End			= 840,
	gemBitVision04End			= 841,
	gemBitCheck01Start			= 842, // Down, Broken Check
	gemBitCheck02Start			= 843,
	gemBitCheck03Start			= 844,
	gemBitCheck04Start			= 845,
	gemBitCheck01End			= 846,
	gemBitCheck02End			= 847,
	gemBitCheck03End			= 848,
	gemBitCheck04End			= 849,
	gemOutPnpPcbPickupStart		= 850,
	gemOutPnpPcbPickupEnd		= 851,
	gemOutPnpPcbTrfToSorterStart= 852,
	gemOutPnpPcbTrfToSorterEnd	= 853,

	gemTraceInfoMcStatus		= 860,
	gemTraceInfoStage01			= 861, // MainAir, Spindle Speed, Flow, ionizer
	gemTraceInfoStage02			= 862, // MainAir, Spindle Speed, Flow, ionizer
	gemTraceInfoStage03			= 863, // MainAir, Spindle Speed, Flow, ionizer
	gemTraceInfoStage04			= 864, // MainAir, Spindle Speed, Flow, ionizer
	gemRecipeUpload				= 865, // Recipe Upload (Lot Start시에 한번만 올림)
	gemTrackOut					= 866, // Track Out

	gemRemoteStop				= 890, // FDC Drop시에 EQP Stop 기능 (mmi->seq)
	                                   // 0:nomal, 1:alarm

	/////////////////////////////////////////////////////////////////////
	// 900~949 : MMI 사용
	timeStop				= 900, // 시퀀스에서 사용 안함 
	timeError				= 901, 
	timeRun					= 902, 
	recordDate				= 903, 

	productCountDay			= 910, // Day
	productCountShift1		= 911, // Shift 별 생산 수량
	productCountShift2		= 912, // Shift 별 생산 수량
	productCountShift3		= 913, // Shift 별 생산 수량

	/////////////////////////////////////////////////////////////////////
	// 950~999 : Lamp&Buzzer Setting
	towerLampR1				= 950,
	towerLampY1				= 951,
	towerLampG1				= 952,
	buzzer1					= 953,
	towerLampR2				= 954,
	towerLampY2				= 955,
	towerLampG2				= 956,
	buzzer2					= 957,
	towerLampR3				= 958,
	towerLampY3				= 959,
	towerLampG3				= 960,
	buzzer3					= 961,
	towerLampR4				= 962,
	towerLampY4				= 963,
	towerLampG4				= 964,
	buzzer4					= 965,
	towerLampR5				= 966,
	towerLampY5				= 967,
	towerLampG5				= 968,
	buzzer5					= 969,
	towerLampR6				= 970,
	towerLampY6				= 971,
	towerLampG6				= 972,
	buzzer6					= 973,
	towerLampR7				= 974,
	towerLampY7				= 975,
	towerLampG7				= 976,
	buzzer7					= 977,
	buzzerSkip				= 978,
	buzzerOffTime			= 979,

	prs20Retry				= 990, // PRS 20회 Retry

	maxNDM = 1000,
};


//-------------------------------------------------------------------
// Use/Skip
enum UseSkip
{
	// 0~99 : 설비공통
	usTcServer				= 1,
	usIonizer				= 2,
	usMzRightStart			= 3,
	usArts					= 4,
//	usOhtOut				= 5,  
	usRfid					= 6,
	usMzClampAlign			= 7,
	usBitBroken				= 8,
	usBitHeight				= 9,
	
	usLight					= 10,
	usIndex01				= 11,
	usIndex02				= 12,
	usIndex03				= 13,
	usIndex04				= 14,

	usLoadCheck				= 16,
	usRouterLiveVision		= 17, // Router Motion 검증용 On시 
	usRouterPartF			= 18, // Router Front Part 사용유무 
	usRouterPartR			= 19, // Router Rear Part 사용유무
	usAdc1DBarcode			= 20,
	usBitChange				= 21,
	usSpindleESDCheck		= 22, // Spindle Wire (Multimeter)
	usBitVision				= 23, // Side, Btm Vision 검사
	usRouterPrs				= 24, // 3Point Prs
	usMaskKitSensorSkip		= 25, // 수정
	usRfidPartNoCompare		= 26, // PartNoDiff 비교 옵션 (Server에서 비교하기 전까지 사용)
	usAutoRecipeChg			= 27,
	usSecsGem				= 28,
	usInCylinderSkip		= 29, // in part의 실린더 사용하지 않도록 하는 옵션
	usPusherOverload		= 30, // Pusher의 OverLoad 센서 확인후 에러 사용/미사용
	
	// mmi 화면에 보이지 않음
	usSpindle2Skip			= 33, // Router Front Part의 Rear Spindle Skip
	usSpindle4Skip			= 34, // Router Rear Part의 Rear Spindle Skip
	usTestRouterPrsVerify	= 35,

	usBitColor				= 39,

	maxUSESKIP				= 500,
};


//-------------------------------------------------------------------
// gerber para
enum gerberPara
{
	/////////////////////////////////////////////////////////////////////
	// spindle Cutting기준의 배열 Array (전체 unit 배열은 Pkg에 있음)
	arrayPathCnt		= 0,
	arrayXCnt			= 1,
	arrayYCnt			= 2,
	arrayXPitch			= 3,
	arrayYPitch			= 4,
	qcViCnt				= 5, // Qc 검사를 하기 위한 Pos가 몇개인지.
	
	arraySpindleYCnt	= 6, // Spindle 사이의 Array Cnt ()


	/////////////////////////////////////////////////////////////////////
	//typedef struct _GerberBlockPrs
	//{
	//	POINT2D	ptXY[4];
	//}GERBER_BLOCK_PRS_POS;
	prsBlockPos			= 10,

	/////////////////////////////////////////////////////////////////////
	// Unit Prs 당 수량
	// mmi -> seq Gerber Data 
	//typedef struct _GerberUnitPrsPos
	//{
	//	UNIT_PRS_INFO prsUnitPos[UNIT_MAX]; // Unit Max 20ea 
	//}GERBER_PRS_UNIT_POS;	
	prsUnitPos			= 100, // 구조체 배열 만큼 할당. 80 // 확인 후 사용


	/////////////////////////////////////////////////////////////////////
	//typedef struct _GerberUnitPrs
	//{
	//	POINT2D ptXY[4];
	//}GERBER_QC;
	qcPos				= 200, // 사용 안함


	maxPARA				= 1000,
};


//-------------------------------------------------------------------
// gerber data
enum gerberData
{
	gerberPos			= 0,    // org gerber
	offsetIndex01		= 2000, // Offset
	offsetIndex02		= 4000, //
	offsetIndex03		= 6000, //
	offsetIndex04		= 8000, //

	maxDATA				= 10000,
};






#endif // _PKGDATA_H_
