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
	*  ��ü�� ������ �Ҹ��� ����ϴ� ���丮 Ŭ����
	*  ��� �ν��Ͻ����� ���丮 Ŭ������ ����� ������ �����ϵ��� �����Ѵ�.
	*  ����(FLAG)�� ���� ����ڰ� ���ϴ� �ν��Ͻ��� �����Ѵ�.
	*  ���� ��ǻ�Ͱ� IPC�� ���� �����޸� ���(Memory Mapped File)�� �����Ѵ�.
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
	KamelasBase*	create(const KAMELASFLAG& flag);/**< kamelas base ����		*/
	void			remove(KamelasBase* pBase);/**<	kamelas base ����	*/

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
