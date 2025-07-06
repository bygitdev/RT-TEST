#ifndef _LDMZOUTCONV_H_
#define _LDMZOUTCONV_H_

#include <Windows.h>
#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"

class CLdMzOutConv
{
public:
	enum CmdMsg
	{
		msgBuffer1Empty	=0,
		msgBuffer2Empty	,
		msgBufferOut	,
	};

	enum Cmd
	{
		C_START					= 100,
		C_PAUSED_STOP			,
		C_01					,
		C_02					,
		C_END					,

		C_MANUAL_START			= 200,
		C_MANUAL_PAUSED_STOP	,
		C_MANUAL_01				,
		C_MANUAL_02				,
		C_MANUAL_END			,
	};

	enum State
	{
		S_IDLE			= 0,
		S_ARRIVAL		,
		S_BUFFER1		,
		S_BUFFER2		,
		S_ALL_EMPTY		,
	};

public:
	CLdMzOutConv();
	virtual ~CLdMzOutConv(){}

public:
	CFSM			m_fsm;
	BOOL			m_bRun;
	CPneumatic*		m_pCylStopperL;
	CPneumatic*		m_pCylStopperR;

	void AutoRun();
	void CycleRun(void);
	void Init(void);

	int& ExistBuffer1(void);
	int& ExistBuffer2(void);
	int& ExistArrival(void);

	BOOL IsReadyMzOut(void);

	void BeltRun(BOOL Run, BOOL Ccw = FALSE);

private:
	BOOL	m_bRdyMzOut;
	BOOL	m_bMzOutSenOn;
	CTimer  m_tmExistErrBuffer1;
	CTimer  m_tmExistErrBuffer2;
	CTimer  m_tmExistErrArrival;
	CTimer	m_tmOutMzFull;


	void AutoMove(void);
	void ManualMove(void);

	int  GetState(void);
	BOOL IsErr(void);

	int  GetExistErrBuffer1(void);
	int  GetExistErrBuffer2(void);
	int  GetExistErrArrival(void);
	
	void CycleConvMoveAuto(void);
	void CycleConvMoveManual(void);
};


//////////////////////////////////////////////////////////////////////////
extern CLdMzOutConv g_ldMzOutConv;
//////////////////////////////////////////////////////////////////////////



#endif//_LDMZOUTCONV_H_