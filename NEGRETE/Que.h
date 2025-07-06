#ifndef QUE_H
#define QUE_H

#include "need.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define FILE_MAPPING_QUE CQue::GetQueAddress()

class CQuePimpl;
class DLL_TYPE CQue
{
public:
	CQue();
	virtual ~CQue();

public:
	virtual VOID	__stdcall Lock(void);
	virtual VOID	__stdcall Unlock(void);
	virtual VOID	__stdcall LogWrite(_char* Message);
	virtual BOOL	__stdcall LogRead(_char* p);
	virtual int		__stdcall GetQueCnt(int nQueMode);
	virtual int		__stdcall GetCurCnt(int nQueMode);
	virtual VOID	__stdcall SetCurCnt(int nQueMode);
	
	static CQue* __stdcall CQue::GetQueAddress(void)
	{
		static CQue QueAddress;
		return &QueAddress;
	};

	virtual	BOOL	__stdcall OpenFile(_char* cMyMap);
	virtual VOID	__stdcall CloseFileMapping(void);
	
private:
	CQuePimpl*		_pPimpl;

	virtual	VOID	__stdcall push(_char* msg);
	virtual VOID	__stdcall pop(void);

	//////////////////////////////////////////////////////////////////////////

	virtual BOOL	__stdcall FileMapping(_char* cMyMap);
	virtual BOOL	__stdcall FileCreateMapping(_char* cMyMap);
	virtual VOID	__stdcall Flush(void);
	virtual BOOL	__stdcall FileOpenMapping(_char* cMyMap);
};

#ifdef __cplusplus
}	/// extern "c"
#endif

#endif /// QUE_H