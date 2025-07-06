#ifndef KAMELASBASEH
#define KAMELASBASEH

#include <windows.h>
#include <tchar.h>


#ifdef KAMELASLIB_STATIC
	#define DLLINTERFACE 
#else
	#ifdef KAMELASLIB_EXPORTS
	#define DLLINTERFACE __declspec(dllexport)
	#else
	#define DLLINTERFACE __declspec(dllimport)
	#endif
#endif


#ifdef __cplusplus
extern "C"
{
#endif

namespace kamelaslib
{

/** Kamelas Base Interface
	*
	*	2009.6
	*	Hong Chan, Choi
	*	hccoi@koses.co.kr
	* 
	*  통신 클라이언트를 위한 공용 인터페이스(interface) 제공
	*
	*/

class DLLINTERFACE KamelasBase
{
public:
	virtual BOOL	__stdcall	open()=0;/**<	통신 채널 개방		*/		
	virtual	BOOL	__stdcall	close()=0;/**<	통신 채널 닫기	*/			
	virtual	BOOL	__stdcall	isOpen()=0;/**<	통신 채널 개방 여부	*/			
	virtual	LPCTSTR	__stdcall	name() const=0;/**<	통신 채널 식별자 이름	*/		
	virtual	UINT32	__stdcall	size() const=0;/**<	통신 채널 크기(바이트)	*/		
	virtual	BOOL	__stdcall	recv(PVOID pBuffer, UINT32 milliSeconds)=0;/**<	데이타 수신및 타임아웃	*/
	virtual	BOOL	__stdcall	send(PVOID pBuffer, UINT32 milliSeconds)=0;/**<	데이타 송신및 타임아웃	*/	
};


DLLINTERFACE KamelasBase*	__stdcall	CreateKamelas(BOOL bServer, TCHAR* lpszName, UINT32 cbSize);
DLLINTERFACE void			__stdcall	DestroyKamelas(KamelasBase* pBase);

}//namespace


#ifdef __cplusplus
}
#endif

#endif
