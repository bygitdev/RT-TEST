#ifndef _SIMPLEFUNC_H_
#define _SIMPLEFUNC_H_

#include <Windows.h>
#include "..\..\BASE\BaseAll.h"


enum _SPH_STATE_
{
	_INIT_SPH_		= 100,
	_START_SPH_		= 101,
	_HOLD_SPH_		= 102,
};




void SimpleFunc(void);	
void UpdateForMMI(void);
void ChkPkgData(PKG pkgNo, double min, double max);

void EctRst(void);
void OptionCheck(void);
void InterfaceAllOff(BOOL isRealTime);

BOOL SetSorterSefetyIndex(int nIdx);
BOOL SetSorterSefetyPnp();

#endif//_SIMPLEFUNC_H_