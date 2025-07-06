#ifndef _OPBUTTON_H_
#define _OPBUTTON_H_


#include <Windows.h>
#include "..\..\BASE\BaseAll.h"



class COpBtn
{
public:
	enum swDef
	{
		swSTART			= 0,
		swSTOP		    = 1,
		swRESET			= 2,
		swMzLoad		= 3,
		swMzUnLoad		= 4,

		swMAX			= 10,
	};

	enum Cmd
	{
		C_BUTTON_START  = 100,
		C_BUTTON_TIMER  = 198,
		C_BUTTON_END    = 199,
	};

	CFSM	m_fsm[swMAX];

	COpBtn() {}
	virtual ~COpBtn() {}

	CTimer	m_tmStart;
	void Run(void);

	int	m_AutoConIn, m_AutoConOut, m_AutoConFlag;

private:
	BOOL m_bSW[swMAX];

	void Stop(CFSM& fsm);
	void Start(CFSM& fsm);
	void Reset(CFSM& fsm);
	void MzLoadSw(CFSM& fsm);
	void MzUnLoadSw(CFSM& fsm);

};


/////////////////////////////////////////////////////////////////////
extern COpBtn g_opBtn;
/////////////////////////////////////////////////////////////////////

#endif//_OPBUTTON_H_