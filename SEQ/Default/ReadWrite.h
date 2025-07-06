#ifndef _READWRITE_H_
#define _READWRITE_H_

#include <windows.h>
#include "..\..\BASE\BaseAll.h"


class CUpdate
{
public:
	CUpdate(){}
	virtual ~CUpdate(){}

	double DisplayAir(double dRunUnit, double dRunVolt, int aioNo);

	void Input(void);											
	void Output(void);	
	void Motor(void);


};

//////////////////////////////////////////////////////////////////////////
extern CUpdate g_update;
//////////////////////////////////////////////////////////////////////////

#endif //_READWRITE_H_