#ifndef _LDMZINCONV_H_
#define _LDMZINCONV_H_

#include <Windows.h>
#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"

class CLdMzInConv
{
public:
	enum CmdMsg
	{
		msgLoaderMoveStopper1		=0,
		msgLoaderMoveStopper2		,
		msgLoaderMoveStopper3		,

		msgStopperStep2MoveStep1	,
		msgStopperStep3MoveStep1	,
		msgStopperStep3MoveStep2	,
	};


	enum Cmd
	{
		C_LOADER_TO_STOPPER_START	= 100, // 빈곳으로 바로 이동
		C_LOADER_TO_STOPPER_01		,
		C_LOADER_TO_STOPPER_02		,
		C_LOADER_TO_STOPPER_03		,
		C_LOADER_TO_STOPPER_END	,

		C_STOPPER_STEP_START	= 200, // 앞으로 한칸씩 이동
		C_STOPPER_STEP_01		,
		C_STOPPER_STEP_02		,
		C_STOPPER_STEP_03		,
		C_STOPPER_STEP_END		,

		C_START					= 300, // oht mode 아닐시 사용
		C_PAUSED_STOP			,
		C_END					,

	};


	enum State
	{
		S_IDLE				= 0,
		S_EMPTY_STOPPER_1	,
		S_EMPTY_STOPPER_2	,
		S_EMPTY_STOPPER_3	,
		S_ALL_EXIST			,

		S_ARRIVAL			, // oht mode가 아닐시 사용
		S_CONV				,
	};


public:
	CLdMzInConv();
	virtual ~CLdMzInConv() {}

public:
	CFSM			m_fsm;
	BOOL			m_bRun;
	
	CPneumatic*		m_pCylStopper01UD;
	CPneumatic*		m_pCylStopper02UD;
	CPneumatic*		m_pCylStopper03UD;
	
	void AutoRun(void);
	void CycleRun(void);
	void Init(void);

	int& ExistStopper01(void);
	int& ExistStopper02(void);
	int& ExistStopper03(void);

	BOOL IsReadyMzIn(void);
	BOOL IsReadyLoadInCall(void);

	void BeltRun(BOOL Run, BOOL Ccw = FALSE);

private:
	CTimer  m_tmStopper1ExistErr;
	CTimer  m_tmStopper2ExistErr;
	CTimer  m_tmStopper3ExistErr;


	BOOL m_bRdyMzIn;
	BOOL m_bRdyLoadInCall;
	
	int  GetState(void);
	BOOL IsErr(void);

	int  GetStopper1ExistErr(void);
	int  GetStopper2ExistErr(void);
	int  GetStopper3ExistErr(void);

	void CycleLoaderToStopper(void);
	void CycleStopperStep(void);

	void CycleConvMove(void);

};

/////////////////////////////////////////////////////////////////////
extern CLdMzInConv g_ldMzInConv;
/////////////////////////////////////////////////////////////////////

#endif
