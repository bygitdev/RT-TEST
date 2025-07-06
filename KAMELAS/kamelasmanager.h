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
	*  ���丮(KamelasFactory)�� ���� ������ �ν��Ͻ�(kamelasBase)���� 
	*  �������ִ� �Ŵ��� ��ü
	*  �ν��Ͻ��� �̸��� ���ڷ� �Ͽ� �� �ν��Ͻ��� �������̽��� ���ٰ����ϸ�
	*  
	*  ����) multi-thread-safe support
	*  ����) �ؽ�(hash) �����̳ʸ� ����Ͽ� ���� �ӵ� ��� ����(����)
	*  ����) �Ŵ��� ����� �ϴ� ���ø� Ŭ������ ����� �̸� ��ӹ޾� ��������(����)
	*
	*/
class KamelasManagerPimpl;
class DLLINTERFACE KamelasManager
{

public:
	KamelasBase&			operator[](LPCTSTR lpName);	/**< �̸����� ��ȸ		*/
	KamelasBase*			query(LPCTSTR lpName);/**<	�̸����� ��ȸ	*/				
	KamelasBase*			at(UINT32 index);/**< �迭 �ε����� ��ȸ		*/				
	bool					attach(const KamelasBase*);/**<	�߰�	*/			
	bool					detach(const KamelasBase*);/**<	����	*/			
	bool					operator+=(const KamelasBase*);/**<	�߰�	*/		
	bool					operator-=(const KamelasBase*);/**<	����	*/		
	UINT32					size();/**<	����	*/								
	void					clear();/**< ��� ����	*/						

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
