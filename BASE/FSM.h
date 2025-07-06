#ifndef _FSM_H_
#define _FSM_H_

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "SeqTimer.h"


namespace FSM
{
	void SeqLog(LPCTSTR lpszFormat, ...);
	double GetSystemTimeH(void);
	

	#define C_IDLE			0
//	#define C_START			100
	#define C_ERROR			(-1)
	#define DEFAULT_MSG		(-1)
	#define SEQ_DBG			SeqLog(L"%s %d", __FILE__, __LINE__)

	class CFSM  
	{
	private:
		CTimer	delayTimer;
		CTimer	limitTimer;

		BOOL	shouldRstDelay;
		BOOL	shouldRstTimeLimit;
		BOOL	isOnce;
		BOOL	isStop;
		int		step;
		int		errVal;
		int		cmd;
		int		msg;
		


	public:
		CFSM();
		virtual ~CFSM();

		void  Set(int nCmd, int nMsg = DEFAULT_MSG);
		int   Get(void);

		BOOL  IsRun(void);
		BOOL  Once(void);
		BOOL  Between(int nMin, int nMax);
		int   GetErr(void);
		
		void  RstErr(void);
		void  RstDelay(void);
		void  RstTimeLimit(void);
		BOOL  Delay(ULONG lDelay); 
		ULONG Elapsed(void);
		BOOL  TimeLimit(ULONG lLimitTime);

		int   GetMsg(void);

		int   GetStep(void);
		void  SetStep(int nStep, BOOL bRstDelay = TRUE);
		BOOL  IsStop(void);
		void  SetStop(BOOL bStop);
		
	};
}

using namespace FSM;

#endif