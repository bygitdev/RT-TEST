#ifndef _CTHREAD_H_
#define _CTHREAD_H_

#include <windows.h>

class CBaseThread
{
public:
	HANDLE	m_hThread;
	UINT	m_nThreadID;

	virtual void Run(void) = 0; 
	static UINT WINAPI ThreadProc(LPVOID lpParam);

	BOOL Begin(void);
	BOOL ChkThreadExit(void);

	CBaseThread();
	virtual ~CBaseThread();
	
};


class CLock
{
public:
	CRITICAL_SECTION	m_csLock;

	void Lock(void);
	void UnLock(void);
	
	CLock();
	virtual ~CLock();
};




/********************************************************************
// test case..

class CThreadTest : public CThread
{
public:
	CThreadTest(){}
	virtual ~CThreadTest(){}

	virtual void Run(void);
};


void CThreadTest::Run(void)
{
	printf("\n Thread Run!!");
	while(1)
	{
		Sleep(100);
	}
}

void main(void)
{
	CThreadTest gTest;
	gTest.Begin(); // therad ½ÇÇà..

	while(1)
	{
		Sleep(100);
	}
}
********************************************************************/



#endif // _CTHREAD_H_

