#ifndef _PKGDATA_H_
#define _PKGDATA_H_


enum PKG
{
	//////////////////////////////////////////////////////////////////////////
	//   0~ 99 : �ý��� �������� ��� 
	// 100~299 ���� User ���� Data
	// 100~149 : Count 
	// 150~199 : Time (���� :sec)
	// 200~249 : Pitch (���� : mm)
	// 250~299 : ��Ÿ
	
	//////////////////////////////////////////////////////////////////////////
	// 100~149 : Count 
	mzSlotZCnt		= 100,
	bitZStepCnt		= 101, // Bit �� ���� ���� Z Count
	unitCnt			= 102,
	mzSlotYCnt		= 103,

	OutPnpVacSetValue    = 110,
	OutPnpVacRcpUseSkip  = 111,
	
	//////////////////////////////////////////////////////////////////////////
	// 150~199 : Time (���� :sec)
	optionSocammUseBlueBit	= 165,


	//////////////////////////////////////////////////////////////////////////
	// 200~249 : Pitch (���� : mm)
	mzSlotZOffset		 = 200,
	mzSlotYOffset		 = 201,

	// gerber file <-> Stage Jig ���� ����Ʈ�� ���� �Ÿ�
	gerberJigOffsetX	 = 201, // Ȯ�� �� ����
	gerberJigOffsetY	 = 202, // Ȯ�� �� ����
	gerberJigOffsetT	 = 203, // Ȯ�� �� ����
	LoaderZRailPosOffset = 204, // �߰�
	bitZStepPitch		 = 205, // ��Ʈ Step �� ��� ����
	bit1CutLength		 = 206, // ddm���� �̵� �߰� 
	
	pcbXLength					= 210, // ���� ������ �Ǵ� ���� Length �߰�
	pcbUnitCenterToEdgeXLength	= 211, // PCB ���� Unit�� �߽ɿ��� ���� Edge�� �Ÿ�

	//////////////////////////////////////////////////////////////////////////
	// 250~299 : ��Ÿ
	adcKitJobType		= 250, // ���� Job�� �´� adc Kit�� ���� No
	                           // 1���� Kit�� �������� Job�� ��Ī�� �� ����
	optionLotSplit		= 251, // Lot Split ��뿩�� �ɼ�

	//////////////////////////////////////////////////////////////////////////
	// 300 ~ 499 : Offset
	// ���� Parameter Offset ó�� ���� Data�� ����ǵ��� ����
	// �ű� Device Setup�ÿ� Offset Data�� Clear �Ǿ�� ��
	offsetLdMzPusherXFwd  = 300, // Loader
	offsetLdMzPusherX2D	  = 301, // Loader

	PcbLengthOptionUse = 303, // �ɼ� �������.

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
	// 900 ~ 1000 : mmi ���� PKG Para 
	underCutUse			= 900,  // 0:Not Use , 1:Use 
	underCutLength		= 901,  // Under Cutting ����

	maxPKG				= 1000,
};


//-------------------------------------------------------------------
// Double Type Data Memory
enum DDm
{
	/////////////////////////////////////////////////////////////////////
	//   0~ 99 : �ý��� �������� ��� 
	// 100~299 ���� User ���� Data
	// 100~149 : Count 
	// 150~199 : Time (���� :sec)
	// 200~249 : Pitch (���� : mm)
	// 250~299 : ��Ÿ
	
	/////////////////////////////////////////////////////////////////////
	//   0~ 99 : �ý��� �������� ��� 
	uph				= 0, // �ð��� ���귮
	machineTime		= 1,
	

	/////////////////////////////////////////////////////////////////////
	// AInput �ñ׳� (Volt)
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

	//Main ȭ�� �� ���÷���
	cycleTmRouterCutting1	= 20, // Router Cutting Down -> Up
	cycleTmRouterCutting2	= 21, // Router Cutting Down -> Up
	cycleTmRouterCutting3	= 22, // Router Cutting Down -> Up
	cycleTmRouterCutting4	= 23, // Router Cutting Down -> Up
	cycleTmOutPnp			= 24, // Out Pnp Put Down -> Put Down

	/////////////////////////////////////////////////////////////////////
	// AIO Diplay�� ���� ���ذ� ����
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

	aiChkTrg				= 90,  // 2021-11-24 ai chk��, 1�� 1st, 2�� 2nd
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
	lotMergeQty			= 102, // Lot Merge �ÿ� Max ���� (800)
							   // �� �۾� Lot�� 799�̰� Setting�� 800�� �̸� �� MGZ�� ��� Merge �ϵ��� �Ѵ�. (800++)
	lotMergeLimitQty	= 103, // Merge Max Lot (1200) // �߰�

	lotCurQty			= 105, 
	lotQtyCheckTime		= 106, 

	OutPnpVacCommon		= 110,
	/////////////////////////////////////////////////////////////////////
	// 150~199 : Time (���� : sec)
	lotMergeEndTime			= 150, // �����ð� ����� Empty Lot End�� ���� Time
//	ohtCallRetryTime		= 151, // �����ð� ����� Oht Call Retry Time
//	ohtPioSafetyWaitTime	= 152, // PIO ��� �� �˶� �߻��� ���� ��� �ð�
//	ohtPioStatusChgWaitTime	= 153, // ���� Stop �����̹Ƿ� Oht�� ���� �ʰڴٴ� ��� ��ȣ
	ScarpBoxSafetyTime		= 154, // Alarm -> Error ��û���� Ÿ���������� ����
	OutPnPPickUpTime		= 155,
	OutPnPPlaceTime			= 156,
	


	/////////////////////////////////////////////////////////////////////
	// 200~249 : Pitch (���� : mm)
	adcMzMaskSlotPitch	= 200,
	adcMzStageSlotPitch = 201,

	bitMaxLifeLength	= 203,
	 
	spindle1ZLimitPos	= 205, // �߰�
	spindle2ZLimitPos	= 206, // �߰�
	spindle3ZLimitPos	= 207, // �߰�
	spindle4ZLimitPos	= 208, // �߰�

	commonPcbLength		= 210, // M.2 963 Device�� �����̸� ���� �Է� �߰�
	commonPcbUnitCenterToEdgeXLength	= 211, // M.2 963 Device�� ����

	spindleResistanceLowLimit   = 249, // ��Ƽ���� ���� Low Limit �� (���� : ��) 
	/////////////////////////////////////////////////////////////////////
	// 250~299 : ��Ÿ
	spindleResistanceLimit		= 250, // ��Ƽ���� ���� Limit �� (���� : ��)

	// Prs ������ ���� ���� ����
	// �ش� ������ Fail�� Retry
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

	prsVisionOffsetLimitX		= 264, // Vision Prs ����� Limit
	prsVisionOffsetLimitY		= 265, // Vision Prs ����� Limit

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
	// Gentry Y Pos (�� �ణ�� �Ÿ� Y ��) 
	gentryYDisF			= 300, // Router Front Part
	gentryYDisR			= 301, // Router Rear Part


	/////////////////////////////////////////////////////////////////////
	// POINT2D
	// 700 ~ Router Front Bit �� Index �߽���ġ
	routerOrgPos01		= 320,	// x, y Ȯ�� �� ���� �����Ǹ� SysTeach�� ����
	routerOrgPos02		= 322,	// x, y 
	routerOrgPos03		= 324,	// x, y
	routerOrgPos04		= 326,  // x, y


	/////////////////////////////////////////////////////////////////////
	// mmi���� �޴� Data 
	mmiReadSpindleResistance	= 350,
	mmiReadViBtmDis				= 351, // Bit Btm �˻� ���ɿ� ����


	/////////////////////////////////////////////////////////////////////
	// 600 ~ Vision Prs Pos ����� SysTeach Pos
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
	// 900~949 : MMI ���
	mcTimeStop			= 900, // ���������� ��� ���� 
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
	// 0 ~ 99 : �ý��� �������� ��� (Main ��ũ�� �� �ɼ� ���)
	// User ���� Data ����
	jobNo					= 0,
	groupNo					= 1,
	screenNo				= 2,
	stateMachine			= 3, // (enMachineState)
	initViewChange			= 4, // ��ü �̴ϼȽÿ� ȭ�� ��ȯ flag

	stateJobNameCheck		= 5, // Sorter�� Job Name Check �Ͽ� Error �߻�
	adcKitOldJobType		= 6, // ���� Device�� Old Job Type

	autoRecipeChgJobNo		= 10, // Auto Device Change ����������� mmi���� Change �ؾ��� Job No �߰�
	autoRecipeChgGroupNo	= 11, // Auto Device Change ����������� mmi���� Change �ؾ��� Grop No �߰�

	/////////////////////////////////////////////////////////////////////
	// MMi Main Speed Btn
	// 20 ~ : mmi screen option
	mmiBtnElevLock			= 20, // Auto �� MGZ Strip ���� Lock
	mmiBtnBuzzerLock		= 21, // Buzzer Lock
	mmiBtnEjectLdMz			= 22, // MGZ ���� ���� (��Ű�� ����)
	mmiBtnRework			= 23, // ���� MGZ�� rework �Ű������� �˸�
	mmiBtnAdcMode			= 24, // On�� Run�ÿ� ADC ����
	mmiAllHomeComp			= 25, // All Home �� �Ϸ�Ǿ��ٴ� ����

	mmiSpindle01BitChange	= 30, // ����� �޴��� 1ȸ�� Bit Change
	mmiSpindle02BitChange	= 31,
	mmiSpindle03BitChange	= 32,
	mmiSpindle04BitChange	= 33,
	
	mmiIndex01PrsOnceSkip	= 34, // ����� �޴��� 1ȸ�� Prs Skip
	mmiIndex02PrsOnceSkip	= 35, // ����� �޴��� 1ȸ�� Prs Skip
	mmiIndex03PrsOnceSkip	= 36, // ����� �޴��� 1ȸ�� Prs Skip
	mmiIndex04PrsOnceSkip	= 37, // ����� �޴��� 1ȸ�� Prs Skip


	/////////////////////////////////////////////////////////////////////
	// 50 ~ : ���� ���� Ȯ��

	existMzLoadZArrival		= 45,
	existMzLoadZExist		= 46,
	existMzInStopper01		= 47, // ����
	existMzInStopper02		= 48, // ����
	existMzInStopper03		= 49, // ����

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
	existRouterCylBitF		= 71, // Bit Change�� ���� Clamp�� bit ���翩��
	existRouterCylBitR		= 72,
	existRedIndexBitAlign01	= 73, // Bit Change�� ���� Align Part�� bit ���翩��
	existRedIndexBitAlign02	= 74, 
	existRedIndexBitAlign03	= 75, 
	existRedIndexBitAlign04	= 76,

	existBlueIndexBitAlign01 = 77, // Bit Change�� ���� Aling Part�� bit ���翩��
	existBlueIndexBitAlign02 = 78, 
	existBlueIndexBitAlign03 = 79,
	existBlueIndexBitAlign04 = 80,

	existInPnpClampKit		= 83, // InPnp�� Clamp Kit ����		
						 

	/////////////////////////////////////////////////////////////////////
	// 96 ~ : Tenkey Mode
	tenKeyJog				= 96,	// tenkey jog mode���� Ȯ�ο�
	jogAxisNo				= 97,	// 0~
	jogSpeed				= 98,	// um
	screenTenkey			= 99,

	/////////////////////////////////////////////////////////////////////
	// 100 ~ : pio sensor mmi display (���񺰷� io no�� Ʋ���Ƿ� ndm���� ����)
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
	existKitMaskPicker01	= 146, // �߰�
	existKitMaskPicker02	= 147, // �߰�
	existKitMaskPicker03	= 148, // �߰�
	existKitMaskPicker04	= 149, // �߰�
	existKitStage01			= 150,
	existKitStage02			= 151,
	existKitStage03			= 152,
	existKitStage04			= 153,
	existKitMask01			= 154,
	existKitMask02			= 155,
	existKitMask03			= 156,
	existKitMask04			= 157,
	existKitOutPnp			= 158,
	existKitMovePicker		= 159, // Index 1���� OutPicker�� �̵���Ŵ

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
	//210 ~ : mmi read router data ���� ������ ���ؼ� �߰�
	mmiRouterIndexNo		= 210, // Index 1, 2, 3, 4
	mmiRouterPathNo			= 211, // Cutting Path No
	mmiRouterPathPos		= 212, // Cutting Pos (Start, End, Mid)
	
	/////////////////////////////////////////////////////////////////////
	//213 ~ : mmi Vision Result
	mmiInPnpPrsErr			= 213, // 0 : Nomal, 1 : Error
	mmiRouterFErr			= 214, // 0 : Nomal, 1 : Error
	mmiRouterRErr			= 215, // 0 : Nomal, 1 : Error
	
	mmiBtmViErr				= 217, // 0 : Nomal, 1 : Error

	mmiNeedSpindleESD		= 220, // Spindle ESD Check -> Bit Change ���� ���� 6�ÿ� Reset 1ȸ/1�� �˻�
	                               // Nomal:0, 1:�˻��û

	/////////////////////////////////////////////////////////////////////
	//230 ~ : mmi state req read count
	lotSplitCount			= 230, // Lot Split ��û�� Lot �и��� ��û�ϴ� �� Count
	                               // A Lot 200 �� �� 150 �и���û�� A Lot�� 150��, A-1 Lot�� 


	/////////////////////////////////////////////////////////////////////
	// 250 ~ : mmi <-> seq Comm State (enCommState)
//	stateOhtInCall			= 250, // Full Carrier Move Req
//	stateOhtOutCall			= 251, // Empty Carrier Move Req
	stateLotMerge			= 253, // ��� ����
	stateRfidRead			= 254, // 
	stateRfidWrite			= 255,
	stateRfidWriteCheck		= 256, // �߰�	
	statePmsTop				= 257, //2D Pcb Info Router Req (Top ���н� Btm Rear)
	statePmsBtm				= 258, //2D Pcb Info Router Req
	stateSpindleESDCheck	= 259,
	stateAdc1D				= 260, 
	stateOutPnpInfo			= 261, // OutPnp ���۽� Sorter�� �����ؾ� �� ������û
	stateCarrierIdRead		= 262, // RFID Data ����
	stateLotMergeInfo		= 263, // MergeInfo Req (MGZ ����)
	stateManualLotIn		= 264, 
	stateLotStart			= 265, // Lot Start (ù���� Frame �� ����)
	statePcbInfo			= 266, // 2D Pms ����
	stateLotInfoLog			= 267, // Lot End Log Save // �߰�
	stateTrayInfoReq		= 268, // Tray Info Req    // �߰�
	statePortInfoChg		= 269, // Port Info Change (Port ���� MGZ ���� ����)
	stateEqpStatusChange	= 270, // Eqp Status Change (?)
	stateVersionUpdate		= 271, // Version Update (?)

	stateSeeLot				  = 273, // SeeLot
	statePioStatusChgInStart  = 274, // Pio Status Chg (MGZ�� ���� ���� ����)
	statePioStatusChgOutStart = 275, // Pio Status Chg (MGZ�� ������ ���� ����)
	statePioStatusChgInStop	  = 276, // Pio Status Chg (MGZ�� ���� ���� ���� ����)
	statePioStatusChgOutStop  = 277, // Pio Status Chg (MGZ�� �������� ���� ���� ����)

	statePartNoCompare		= 278, // Auto Device Change�� ���� Part No �� ���
	                                 // 0=idle, 1=req, 2=busy, 3=comp(partNo����), 
	                                 // 4=err(PartNo�ٸ� ������̽��� ����), 
	                                 // 5=Auto Change (PartNo�� ���� �׷쿡 ������), 
	stateAutoRecipeChg		= 279, // Auto Device Change Step
	stateLotSplit			= 280, // Lot Split Req �߰�
	stateSplitLotStart		= 281, // Lot Split Lot ID�� Lot Start Req �߰�

	outPnpScrapExisDelayTime = 299, // scrap exist x1800 ���� Ƣ�� �������� ���� ������ �߰�
	outPnpPcbIndex			= 300, // PCB�� ��� Index���� Pickup �� ������ ���
	spindleESDNo			= 301, // ��� Spindle�� ESD �˻縦 �Ѱ�����.. Log ������ ���


	spindleSpeedUpload0102	= 310, // Spindle Speed Server Upload Req = 1
	spindleSpeedUpload0304	= 311, // Spindle Speed Server Upload Req = 2
	prsDataLog12			= 312, // 0: Nomal, Index No Write 1, 2 // �߰�
	prsDataLog34			= 313, // 0: Nomal, Index No Write 3, 4 // �߰�

	/////////////////////////////////////////////////////////////////////
	// 320 ~ : Spindle ESD Check
	flagSpindleESDCheck01	= 320, // Spindle ESD Check �ؾ� �Ѵٴ� ���� 1ȸ/1��
	flagSpindleESDCheck02	= 321,
	flagSpindleESDCheck03	= 322,
	flagSpindleESDCheck04	= 323,


	/////////////////////////////////////////////////////////////////////
	// 330 ~ : flag
	flagAllLotClear			= 330,
	flagMtIndexOffsetMove	= 331, // Mt Index Move�� Offset ���� ����
	                               // Motor â������ 0, Offset Setup â������ 1 Write
	flagViTcpReconnect		= 332, // Vision �����ȣ�� IO�� Vision�� ��ȣ
	flagSplitIDLotStart		= 333, // �߰� Split Lot ID�� Lot Change

	flagSplitInfo			= 335, // �߰� Split Lot ID�� MMI���� Rail�ܿ� Write �� �� �ֶǷ� ��

	flagLotMergeComp		= 340, // �߰� LotMerge �Ϸ�� SplitLot ������ ���� �ñ׳�

	/////////////////////////////////////////////////////////////////////
	// ���� Memory ��ﺯ�� 
//	ohtPioStatusChgInStart	= 350, // 0 : PioStop ����, 1 : ����簳 PIO_RESTART �ñ׳� ���� �Ϸ�
//	ohtPioStatusChgOutStart	= 351, // 0 : PioStop ����, 1 : ����簳 PIO_RESTART �ñ׳� ���� �Ϸ�

//	ohtECMRComp				= 352,
//	ohtFCMRComp				= 353,

	ldMzCmdSlotNo			= 360, // �߰� loader Step ������ ������ �ʿ��� ���� �־� �߰���
	ldMzCurSlotNo			= 361, // �߰�

	ldMzInSwOn				= 370,
	ldMzOutSwOn				= 371,

	/////////////////////////////////////////////////////////////////////
	// ���� Memory ��ﺯ�� (typedef struct tagIndexMemory)
	indexMemory01			= 400,
	indexMemory02			= 420,
	indexMemory03			= 440,
	indexMemory04			= 460,

	/////////////////////////////////////////////////////////////////////
	// Bit ���� (typedef struct _SpindleBit)
	routerBitInfo01			= 510,
	routerBitInfo02			= 515,
	routerBitInfo03			= 520,
	routerBitInfo04			= 525,

	/////////////////////////////////////////////////////////////////////
	// Bit Box ����
	// Max 50 �� �� ��� ����ϰ� ���Ҵ��� �������θ� ǥ�� (49) = Max, (-1) = Empty)
	// ���� ���� ���� ���ϴܺ��� Start
	// 04 03 02 01 00
	// ~  ~  ~  ~  ~
	// 49 48 47 46 45

	RedindexBitBoxCurCnt01				= 530, // Index01
	RedindexBitBoxCurCnt04				= 531, // Index04

	RedindexBitSupplyBoxClear01			= 532, // Bit Supply Box�� Off->On �ߴٴ� ����
	RedindexBitSupplyBoxClear04			= 533, // Bit Supply Box�� Off->On �ߴٴ� ����

	BlueindexBitBoxCurCnt02				= 535, // Index02
	BlueindexBitBoxCurCnt03				= 536, // Index03

	BlueindexBitSupplyBoxClear02		= 537, // Bit Supply Box�� Off->On �ߴٴ� ����
	BlueindexBitSupplyBoxClear03		= 538, // Bit Supply Box�� Off->On �ߴٴ� ����
	
	/////////////////////////////////////////////////////////////////////
	// 600 ~ : ADC Job No + Kit Info
	adcMzTopJobType			= 600,
	adcMzBtmJobType			= 610, 
	
	adcRailKitJobType		= 620, // InPnp Pickup�� ���� 2D reading Data
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

//	ohtInCallComp				= 710, // ���� ��� ���� 
//	ohtOutCallComp				= 711, // ���� ��� ���� 
	needAutoRecipeChg			= 712, // ���� ��� ���� Auto Device Chg�� �ʿ��ϴٴ� ����

	setupmode					= 777,

	/////////////////////////////////////////////////////////////////////
	// 800~899 : Gem Event�� ���
	// enum enCommState �� ���� �ְ����
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
	gemRecipeUpload				= 865, // Recipe Upload (Lot Start�ÿ� �ѹ��� �ø�)
	gemTrackOut					= 866, // Track Out

	gemRemoteStop				= 890, // FDC Drop�ÿ� EQP Stop ��� (mmi->seq)
	                                   // 0:nomal, 1:alarm

	/////////////////////////////////////////////////////////////////////
	// 900~949 : MMI ���
	timeStop				= 900, // ���������� ��� ���� 
	timeError				= 901, 
	timeRun					= 902, 
	recordDate				= 903, 

	productCountDay			= 910, // Day
	productCountShift1		= 911, // Shift �� ���� ����
	productCountShift2		= 912, // Shift �� ���� ����
	productCountShift3		= 913, // Shift �� ���� ����

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

	prs20Retry				= 990, // PRS 20ȸ Retry

	maxNDM = 1000,
};


//-------------------------------------------------------------------
// Use/Skip
enum UseSkip
{
	// 0~99 : �������
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
	usRouterLiveVision		= 17, // Router Motion ������ On�� 
	usRouterPartF			= 18, // Router Front Part ������� 
	usRouterPartR			= 19, // Router Rear Part �������
	usAdc1DBarcode			= 20,
	usBitChange				= 21,
	usSpindleESDCheck		= 22, // Spindle Wire (Multimeter)
	usBitVision				= 23, // Side, Btm Vision �˻�
	usRouterPrs				= 24, // 3Point Prs
	usMaskKitSensorSkip		= 25, // ����
	usRfidPartNoCompare		= 26, // PartNoDiff �� �ɼ� (Server���� ���ϱ� ������ ���)
	usAutoRecipeChg			= 27,
	usSecsGem				= 28,
	usInCylinderSkip		= 29, // in part�� �Ǹ��� ������� �ʵ��� �ϴ� �ɼ�
	usPusherOverload		= 30, // Pusher�� OverLoad ���� Ȯ���� ���� ���/�̻��
	
	// mmi ȭ�鿡 ������ ����
	usSpindle2Skip			= 33, // Router Front Part�� Rear Spindle Skip
	usSpindle4Skip			= 34, // Router Rear Part�� Rear Spindle Skip
	usTestRouterPrsVerify	= 35,

	usBitColor				= 39,

	maxUSESKIP				= 500,
};


//-------------------------------------------------------------------
// gerber para
enum gerberPara
{
	/////////////////////////////////////////////////////////////////////
	// spindle Cutting������ �迭 Array (��ü unit �迭�� Pkg�� ����)
	arrayPathCnt		= 0,
	arrayXCnt			= 1,
	arrayYCnt			= 2,
	arrayXPitch			= 3,
	arrayYPitch			= 4,
	qcViCnt				= 5, // Qc �˻縦 �ϱ� ���� Pos�� �����.
	
	arraySpindleYCnt	= 6, // Spindle ������ Array Cnt ()


	/////////////////////////////////////////////////////////////////////
	//typedef struct _GerberBlockPrs
	//{
	//	POINT2D	ptXY[4];
	//}GERBER_BLOCK_PRS_POS;
	prsBlockPos			= 10,

	/////////////////////////////////////////////////////////////////////
	// Unit Prs �� ����
	// mmi -> seq Gerber Data 
	//typedef struct _GerberUnitPrsPos
	//{
	//	UNIT_PRS_INFO prsUnitPos[UNIT_MAX]; // Unit Max 20ea 
	//}GERBER_PRS_UNIT_POS;	
	prsUnitPos			= 100, // ����ü �迭 ��ŭ �Ҵ�. 80 // Ȯ�� �� ���


	/////////////////////////////////////////////////////////////////////
	//typedef struct _GerberUnitPrs
	//{
	//	POINT2D ptXY[4];
	//}GERBER_QC;
	qcPos				= 200, // ��� ����


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
