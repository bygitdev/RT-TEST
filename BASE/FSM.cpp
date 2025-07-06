#include "FSM.h"

namespace FSM
{
	//---------------------------------------------------------------
	// ex) SeqLog(L"Hello World!!");
	// degugview를 이용함..
	void SeqLog(LPCTSTR lpszFormat, ...)
	{
		const int BUFF_LENGTH = 512;

		TCHAR lpszBuffer[BUFF_LENGTH]; //버퍼 크기.
		_stprintf_s(lpszBuffer, 10, _T("%S"),"[seq]: ");

		va_list fmtList;
		va_start(fmtList, lpszFormat);
		_vstprintf_s(&lpszBuffer[7], (BUFF_LENGTH -7), lpszFormat, fmtList);
		va_end(fmtList);

		::OutputDebugString(lpszBuffer);

	#ifdef _MSC_VER
			::OutputDebugString( _T("\n"));
	#endif
	}
	

	double GetSystemTimeH(void)
	{
		time_t curTm;
		time(&curTm);

		double dTime = curTm / 3600.0; // sec -> hour
		return (dTime);
	}//--------------------------------------------------------------


	CFSM::CFSM()
	{
		shouldRstDelay		= TRUE;
		shouldRstTimeLimit	= TRUE;
		isOnce				= TRUE;
		isStop				= FALSE;
		cmd					= C_IDLE;
		msg					= DEFAULT_MSG;
		errVal				= 0;
		step				= 0;
	}//--------------------------------------------------------------


	CFSM::~CFSM()
	{

	}//-----------------------------------------------------------------


	void CFSM::Set(int nCmd, int nMsg)
	{
		if(DEFAULT_MSG != nMsg)
		{
			msg = nMsg;
		}

		if(C_ERROR == nCmd)
		{
			if(DEFAULT_MSG != nMsg)
				errVal = nMsg;
			else
				errVal = 1;
		}
		else if(C_IDLE < nCmd)
		{
			errVal	= 0;
		}

		if(C_IDLE == nCmd)
		{
			isStop = FALSE;
			msg	= DEFAULT_MSG;
		}

		if(cmd != nCmd)
		{
			shouldRstDelay		= TRUE;
			shouldRstTimeLimit	= TRUE;
			isOnce			= TRUE;
			step			= 0;
		}

		cmd = nCmd;
	}//--------------------------------------------------------------



	int CFSM::Get(void)
	{
		return (cmd);
	}//-----------------------------------------------------------------


	BOOL CFSM::IsRun(void)
	{
		if(C_IDLE == cmd)
			return (FALSE);

		return (TRUE);
	}//--------------------------------------------------------------


	BOOL CFSM::Between(int nMin, int nMax)
	{
		if(nMin > Get())
			return (FALSE);
		if(nMax < Get())
			return (FALSE);

		return (TRUE);
	}//--------------------------------------------------------------


	int CFSM::GetErr(void)
	{
		return (errVal);
	}//--------------------------------------------------------------


	BOOL CFSM::Once(void)
	{
		BOOL bRet = isOnce;

		isOnce = FALSE;
		return (bRet);
	}//--------------------------------------------------------------


	BOOL CFSM::TimeLimit(ULONG lLimitTime)
	{
		if(shouldRstTimeLimit)
		{
			shouldRstTimeLimit = FALSE;
			limitTimer.Reset();
		}

		return limitTimer.TmOver(lLimitTime);
	}//--------------------------------------------------------------



	void  CFSM::RstErr(void)
	{
		errVal = 0;
	}//--------------------------------------------------------------


	void CFSM::RstDelay(void)
	{
		shouldRstDelay = TRUE;
		delayTimer.Reset();
	}//--------------------------------------------------------------


	void  CFSM::RstTimeLimit(void)
	{
		shouldRstTimeLimit = TRUE;
		limitTimer.Reset();
	}


	BOOL CFSM::Delay(ULONG lDelay) 
	{
		if(shouldRstDelay)
		{
			shouldRstDelay = FALSE;
			delayTimer.Reset();
		}

		return delayTimer.TmOver(lDelay);
	}//--------------------------------------------------------------


	ULONG CFSM::Elapsed(void)
	{
		if(shouldRstDelay)
		{
			shouldRstDelay = FALSE;
			delayTimer.Reset();
		}

		return delayTimer.Elapsed();
	}//--------------------------------------------------------------

	
	int CFSM::GetMsg(void)
	{
		return (msg);
	}


	//------------------------------------------------------------------
	int CFSM::GetStep(void)
	{
		return (step);
	}
	void CFSM::SetStep(int nStep, BOOL bRstDelay)
	{
		if(bRstDelay)
			RstDelay();

		step = nStep;
	}

	//------------------------------------------------------------------
	BOOL CFSM::IsStop(void)
	{
		return (isStop);
	}
	void CFSM::SetStop(BOOL bStop)
	{
		if(C_IDLE < cmd)
			isStop = TRUE;
	}
}