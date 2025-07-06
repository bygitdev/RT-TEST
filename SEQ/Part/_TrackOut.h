#ifndef _TRACKOUT_H_
#define _TRACKOUT_H_

#include <Windows.h>
#include "..\..\BASE\BaseAll.h"




class CTrackOut
{
public:
	enum Cmd
	{
		C_TRACK_OUT_START	= 100,
		C_TRACK_OUT_COMM	= 101,
		C_TRACK_OUT_END		= 199,
	};


public:
	CTrackOut(){}
	virtual ~CTrackOut(){}

	CFSM		m_fsm;
	BOOL		m_bRun;

	int*		m_pnTrackOutActivated;
	int			colorNo;

	void AutoRun(void);
	void CycleRun(void);
	void Init(void);
	BOOL IsExistInMc(int color);
};


/////////////////////////////////////////////////////////////////////
extern CTrackOut g_trackOut;
/////////////////////////////////////////////////////////////////////

#endif