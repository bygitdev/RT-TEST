#include "Communication.h"

CKamelasComm::CKamelasComm()
{

}

CKamelasComm::~CKamelasComm()
{
	kamelaslib::DestroyKamelas(this->m_pBase);
	this->m_pBase = NULL;
}

BOOL CKamelasComm::Init(int nSize)
{
	this->m_pBase = kamelaslib::CreateKamelas(TRUE, L"MMI2SEQ", nSize);

	BOOL bSuccess = this->m_pBase->open();
	return (bSuccess);
}

BOOL CKamelasComm::Recv(PBYTE p)
{
	BOOL bRet = this->m_pBase->recv(p, 1);
	return (bRet);
}


BOOL CKamelasComm::Send(PBYTE p)
{
	BOOL bRet = this->m_pBase->send(p, 10);
	return (bRet);
}