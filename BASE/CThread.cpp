#include "CThread.h"
#include <process.h>

CBaseThread::CBaseThread()
{
}

CBaseThread::~CBaseThread()
{
	//TerminateThread(m_hThread, 0);  
	CloseHandle(m_hThread);
}
//-------------------------------------------------------------------


BOOL CBaseThread::Begin(void)
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (LPVOID)this, 0, &m_nThreadID);

	return (TRUE);
}
//-------------------------------------------------------------------


UINT WINAPI CBaseThread::ThreadProc(LPVOID lpParam)
{
	CBaseThread* pTheread = (CBaseThread*)lpParam;
	pTheread->Run();
	return (0);
}
//-------------------------------------------------------------------


BOOL CBaseThread::ChkThreadExit(void)
{
	WaitForSingleObject(m_hThread, 2000);
	return (TRUE);
}
//-------------------------------------------------------------------




CLock::CLock()
{
	InitializeCriticalSection(&m_csLock);
}

CLock::~CLock()
{
	DeleteCriticalSection(&m_csLock);
}
//-------------------------------------------------------------------

void CLock::Lock(void)
{
	EnterCriticalSection(&m_csLock);
}
//-------------------------------------------------------------------


void CLock::UnLock(void)
{
	LeaveCriticalSection(&m_csLock);
}
//-------------------------------------------------------------------





