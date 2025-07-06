#ifndef _TENKEYPROC_H_
#define _TENKEYPROC_H_


#include <Windows.h>
#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"

#define _HOME_		0
#define _STEP_		1000



class CTenkeyOpr
{
public:
	CTenkeyOpr(){}
	virtual ~CTenkeyOpr(){}

public:
	enum Cmd
	{
		C_POS_ROUTER_START	= 100,
		C_POS_ROUTER_01		,
		C_POS_ROUTER_02		,
		C_POS_ROUTER_END	,

		C_POS_VISION_START	= 200,
		C_POS_VISION_01		,
		C_POS_VISION_02		,
		C_POS_VISION_END	,
	};

	CFSM m_fsm;
	CFSM m_fsmJog;
	CTenkey m_tenkey;

	BOOL    m_bDisable;
	
	BOOL DoorSafety(int nVal);

	void Run(void);
	void Auto(void);
	void Cycle(void);
	BOOL IsErr(void);
	void Init(void);

	void Jog(void);
	void CyclePosRouter(void);
	void CyclePosVision(void);

};

/////////////////////////////////////////////////////////////////////
extern CTenkeyOpr g_tenkeyOpr;
/////////////////////////////////////////////////////////////////////

#endif//_TENKEYPROC_H_