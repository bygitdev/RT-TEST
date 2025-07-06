#ifndef THREAD_H
#define THREAD_H

#include "need.h"

#ifdef __cplusplus
extern "C"
{
#endif

class DLL_TYPE CThread
{
public:
	HANDLE		  m_hThread;
	UINT		  m_unThreadID;

	virtual	DWORD	__stdcall Execute(void) = 0;
	
	static	UINT	__stdcall _Thread(LPVOID p)
	{
		CThread *pThis = (CThread*)p;
		pThis->Execute();
		return 0;
	};

	virtual	VOID	__stdcall Create(void);
	virtual	DWORD	__stdcall SuspendThread(void);
	virtual	DWORD	__stdcall ResumeThread(void);
	virtual	DWORD	__stdcall CloseThread(BOOL bNowKill = FALSE);
public:
	CThread();
	virtual ~CThread();
};

#ifdef __cplusplus
}	/// extern "c"
#endif

#endif	// THREAD_H

