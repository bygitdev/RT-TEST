#ifndef _CHECKERROR_H_
#define _CHECKERROR_H_

#include <windows.h>
#include "..\..\BASE\BaseAll.h"



enum enEmgState
{
	C_EMG_ON			= 100,
	C_EMG_OFF			= 101,
	C_EMG_END			= 199,
};

class CCheckErr : public CError
{
public:
	CCheckErr()
	{
		nOldJobNo   = -1;
		nOldGroupNo = -1;
	}

	~CCheckErr(){}

	CTimer	m_tmDoorLock;

	BOOL m_bLdSafetyBeam;
	BOOL m_bScrapSafetyBeam;

	int nOldJobNo;
	int nOldGroupNo;

	void Run(void);
	void Door(BOOL isNoErr = FALSE);
	BOOL ChkMtSafety(int nMt);
	BOOL ChkMtIndexMove(int nMtNo, int nIndexNo);



private:
	CFSM	m_fsmEmg;

	CTimer	m_tmLdSafety;
	CTimer	m_tmScrapSafety;	
	
	void Motor(void);
	void Pneumatic(void);
	void Etc(void);

};


//////////////////////////////////////////////////////////////////////////
extern CCheckErr g_err;
extern CError	 g_wr;
//////////////////////////////////////////////////////////////////////////





#endif//_CHECKERROR_H_