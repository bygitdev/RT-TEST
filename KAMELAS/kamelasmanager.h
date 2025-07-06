#ifndef KamelasManagerH
#define KamelasManagerH

#include <windows.h>
#include <tchar.h>
#include "Kamelasbase.h"

#ifdef __cplusplus
extern "C"
{
#endif

namespace kamelaslib
{
/**
	*  Kamelas Manager  
	*
	*	2009.6
	*	Hong Chan, Choi
	*	hccoi@koses.co.kr
	* 
	*  팩토리(KamelasFactory)로 부터 생성된 인스턴스(kamelasBase)들을 
	*  관리해주는 매니저 객체
	*  인스턴스의 이름을 인자로 하여 각 인스턴스의 인터페이스를 접근가능하며
	*  
	*  참고) multi-thread-safe support
	*  참고) 해쉬(hash) 컨테이너를 사용하여 접근 속도 향상 가능(숙제)
	*  참고) 매니저 기능을 하는 템플릿 클래스를 만들어 이를 상속받아 구현가능(숙제)
	*
	*/
class KamelasManagerPimpl;
class DLLINTERFACE KamelasManager
{

public:
	KamelasBase&			operator[](LPCTSTR lpName);	/**< 이름으로 조회		*/
	KamelasBase*			query(LPCTSTR lpName);/**<	이름으로 조회	*/				
	KamelasBase*			at(UINT32 index);/**< 배열 인덱스로 조회		*/				
	bool					attach(const KamelasBase*);/**<	추가	*/			
	bool					detach(const KamelasBase*);/**<	삭제	*/			
	bool					operator+=(const KamelasBase*);/**<	추가	*/		
	bool					operator-=(const KamelasBase*);/**<	삭제	*/		
	UINT32					size();/**<	개수	*/								
	void					clear();/**< 모두 삭제	*/						

private:
	KamelasManagerPimpl*	_pPimpl;

public:
	KamelasManager();	
	virtual ~KamelasManager();
};

}//namespace

void DLLINTERFACE WINAPI GetManager(kamelaslib::KamelasManager** pManager);

#ifdef __cplusplus
}
#endif

#endif
