#ifndef _MGZLOADZ_H_
#define _MGZLOADZ_H_

#include <Windows.h>
#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"

class CMgzLoadZ
{
public:
	enum Posz
	{
		PZ_DOWN_POS =0,
		PZ_LOAD_POS =1,
		PZ_UNLOAD_POS =2,
	};

	enum Cmd
	{
		C_LOADING_START		= 200,
		C_LOADING_01			,
		C_LOADING_02			,
		C_LOADING_03			,
		C_LOADING_PAUSE_STOP	,
		C_LOADING_END			,

		C_UNLOADING_START	= 300, 
		C_UNLOADING_01			,
		C_UNLOADING_02			,
		C_UNLOADING_03			,
		C_UNLOADING_PAUSE_STOP	,
		C_UNLOADING_END			,
	};


	enum State
	{
		S_IDLE				= 0,
		S_LOADING			,
		S_LOAD_WAIT			,
		S_UNLOAD_WAIT		,
		S_UNLOADING			,
	};


public:
	CMgzLoadZ();
	virtual ~CMgzLoadZ() {}

public:
	CFSM			m_fsm;
	BOOL			m_bRun;

	CMtAXL*			m_pMtZ;
	CPneumatic*		m_pCylStopperUD;
	
	void AutoRun(void);
	void CycleRun(void);
	void Init(void);

	int& ExistLoadExist(void);
	int& ExistLoadArrival(void);

	BOOL IsReadyLoadIn(void);
	BOOL IsReadyLoadOut(void);

	void BeltRun(BOOL Run, BOOL Ccw = FALSE);

private:
	CTimer  m_tmLoadZArrivalErr;
	CTimer  m_tmLoadZExistErr;

	BOOL m_bRdyMzIn;
	BOOL m_bRdyMzOut;
	
	int  GetState(void);
	BOOL IsErr(void);

	int  GetLoadZArrivalErr(void);
	int  GetLoadZExistErr(void);

	void CycleLoadingStep(void);
	void CycleUnLoadingStep(void);
};

/////////////////////////////////////////////////////////////////////
extern CMgzLoadZ g_MgzLoadZ;
/////////////////////////////////////////////////////////////////////

#endif
