#ifndef _PIPER_SVR_H_
#define _PIPER_SVR_H_

#include "Interface.h"
#include "Que.h"

#ifdef __cplusplus
extern "C"
{
#endif

	class CPiperPimpl;
	class DLL_TYPE CPiper : public CPiperInterface
	{
	public:
		CPiper();
		virtual ~CPiper(){};

	public:
		virtual BOOL	__stdcall	Init(BOOL RorW, _char* cMyName, _char* cMyMap);
		virtual VOID  	__stdcall 	Close(void);

	private:
		CPiperPimpl*	_pPimpl;
	};

	DLL_TYPE CPiper*	__stdcall	CreatePiper(void);
	DLL_TYPE void		__stdcall	DistoryPiper(CPiper* pPiper);

#ifdef __cplusplus
}	/// extern "c"
#endif

#endif	/// _PIPER_SVR_H_
