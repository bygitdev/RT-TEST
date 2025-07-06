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
	*  ��� Ŭ���̾�Ʈ�� ���� ���� �������̽�(interface) ����
	*
	*/

class DLLINTERFACE KamelasBase
{
public:
	virtual BOOL	__stdcall	open()=0;/**<	��� ä�� ����		*/		
	virtual	BOOL	__stdcall	close()=0;/**<	��� ä�� �ݱ�	*/			
	virtual	BOOL	__stdcall	isOpen()=0;/**<	��� ä�� ���� ����	*/			
	virtual	LPCTSTR	__stdcall	name() const=0;/**<	��� ä�� �ĺ��� �̸�	*/		
	virtual	UINT32	__stdcall	size() const=0;/**<	��� ä�� ũ��(����Ʈ)	*/		
	virtual	BOOL	__stdcall	recv(PVOID pBuffer, UINT32 milliSeconds)=0;/**<	����Ÿ ���Ź� Ÿ�Ӿƿ�	*/
	virtual	BOOL	__stdcall	send(PVOID pBuffer, UINT32 milliSeconds)=0;/**<	����Ÿ �۽Ź� Ÿ�Ӿƿ�	*/	
};


DLLINTERFACE KamelasBase*	__stdcall	CreateKamelas(BOOL bServer, TCHAR* lpszName, UINT32 cbSize);
DLLINTERFACE void			__stdcall	DestroyKamelas(KamelasBase* pBase);

}//namespace


#ifdef __cplusplus
}
#endif

#endif
