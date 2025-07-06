#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include <windows.h>
#include "..\KAMELAS\kamelaslib.h"

#pragma comment(lib, "kamelaslibDU.lib")

#define  BUFFER_SIZE      (1024 * 320) 

class CKamelasComm
{
private:
	KamelasBase* 	m_pBase;

public:	
	BOOL Init(int nSize);
	BOOL Recv(PBYTE p);
	BOOL Send(PBYTE p);


public:
	CKamelasComm();
	virtual ~CKamelasComm();
};


#endif // _COMMUNICATION_H_

