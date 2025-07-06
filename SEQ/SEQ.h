#ifndef _SEQEXCUTE_H_
#define _SEQEXCUTE_H_

#include <Windows.h>
#include "..\BASE\BaseAll.h"
#pragma comment(lib, "winmm.lib")





//-------------------------------------------------------------------
// Main Seq Class
class CSEQ
{
private:
	enum State
	{
		S_INIT		 = 100,
		S_INIT_AJIN  = 101,
		S_INIT_MOTOR = 102,
		S_INIT_CLASS = 103,
		S_EMG_ON	 = 200,
		S_EMG_OFF	 = 201,
		S_RUN		 = 900,
	};

	CTimer tmAxisZHome;

	CFSM	m_fsmUPH;
	double	m_dUPHElapsed;

	CFSM	m_fsmMcTime;
	double  m_dMcTime;

	CFSM	m_mainState;
	void Seq(void);


	BOOL InitPnematic(void);
	BOOL InitAjin(bool shouldInit);
	BOOL InitNV(void);
	BOOL InitClass(void);
	BOOL InitEtc(void);

	void EmgOn(void);
	void EmgOff(void);
	void SafetyOutOff(void);

public:
	CSEQ(){}
	virtual ~CSEQ(){}

	void Main(void);
};

//////////////////////////////////////////////////////////////////////////
extern CSEQ  g_seq;
//////////////////////////////////////////////////////////////////////////



#endif//_SEQEXCUTE_H_