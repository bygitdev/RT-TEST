#ifndef _PNEUMATIC_H_
#define _PNEUMATIC_H_

#include <Windows.h>
#include "SeqTimer.h"
#include "..\HW\IOAXL.h"

namespace PNEUMATIC
{
	#define pmFWD		1
	#define pmUP		1
	#define pmON        1
	#define pmOPEN		1
	#define pmLEFT		1

	#define pmBWD		0
	#define pmDOWN		0
	#define pmOFF       0
	#define pmCLOSE		0
	#define pmRIGHT		0

	#define pmFREE		2
	#define pmUNCERTAIN	(-1)

	#define pmDUMMY_IO  (-1)


	/********************************************************************
	Pneumatic Class
	********************************************************************/
	class  CPneumatic
	{
	public:
		CPneumatic(){}
		virtual ~CPneumatic(){}


	private:
		int m_nTimeLimit;
		int m_nCurDriection;
		int m_nErrorAct;
		int m_nErrorCode;

		int m_nNo;

        AJIN::CInPoint m_di1;
        AJIN::CInPoint m_di2;
        AJIN::COutPoint m_do1;
        AJIN::COutPoint m_do2;

		CTimer m_tmErr;
		CTimer m_tmDelay;

	public:
		BOOL Set(int nCylNo, int iOnOpUpFwd, int iOffClDnBwd, int oOnOpUpFwd, int oOffClDnBwd);	/// negrete
		void SetErr(int nLimitTime, int nErrCode, int nAct);

		void Actuate(int nCmdDirection);
		int  GetPos(int nDelay = 10);
		int  GetErr(void);
		int	 GetNo(void);

		int GetOnIO(int cmdAct, int nInOut);
		int GetOffIO(int cmdAct, int nInOut);

	private:
		void SetOut(BOOL bOut1, BOOL bOut2);
	};
}


using namespace PNEUMATIC;



#endif//_PNEUMATIC_H_
