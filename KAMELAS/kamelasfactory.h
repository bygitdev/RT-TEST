#ifndef KAMELASFACTORYH
#define KAMELASFACTORYH

#include "kamelasbase.h"


#ifdef __cplusplus
extern "C"
{
#endif

namespace kamelaslib
{

/**
	*  Kamelas Factory  
	*
	*	2009.6
	*	Hong Chan, Choi
	*	hccoi@koses.co.kr
	* 
	*  객체의 생성및 소멸을 담당하는 팩토리 클래스
	*  모든 인스턴스들은 팩토리 클래스를 사용해 생성및 해제하도록 유도한다.
	*  인자(FLAG)를 통해 사용자가 원하는 인스턴스를 생성한다.
	*  로컬 컴퓨터간 IPC를 위한 공유메모리 통신(Memory Mapped File)를 제공한다.
	*
	*/


typedef enum eType
{
	// deprecated except shared memory model

	TYPE_SHAREDMEMORY_SERVER	= 0,
	TYPE_SHAREDMEMORY_CLIENT,

}TYPE;

typedef struct tagKAMELASFLAG
{
	TYPE		type;					/// connection type
	UINT32		size;					/// size of bytes
	TCHAR		name[MAX_PATH];			/// your instance name
}KAMELASFLAG;	


class DLLINTERFACE KamelasFactory
{
public:
	KamelasBase*	create(const KAMELASFLAG& flag);/**< kamelas base 생성		*/
	void			remove(KamelasBase* pBase);/**<	kamelas base 삭제	*/

public:
	KamelasFactory();
	virtual ~KamelasFactory();
};

}//namespace

void DLLINTERFACE WINAPI GetFactory(kamelaslib::KamelasFactory** pFactory);

#ifdef __cplusplus
}
#endif

#endif
