#ifndef _ALLHOME_H_
#define _ALLHOME_H_

#include <windows.h>
#include "..\..\BASE\BaseAll.h"

class CAllHome
{
public:

	enum homeMsg
	{
		hmLoader	= 0x00000001,
		hmRail		= 0x00000002,
		hmInPnp		= 0x00000004,
		hmIndex01	= 0x00000008,
		hmIndex02	= 0x00000010,
		hmIndex03	= 0x00000020,
		hmIndex04	= 0x00000040,
		hmRouterF	= 0x00000080,
		hmRouterR	= 0x00000100,
		hmOutPnp	= 0x00000200,
		hmADC		= 0x00000400,
		hmMGZLoadZ	= 0x00000800,

		hmAllHome	= 0xffffffff,
	};

	CAllHome() {}
	virtual ~CAllHome() {}

	void Run(void);

	void Start(int nMsg = 0);
	void Cancel(void);


	CFSM    m_fsm;

	BOOL	m_bInPnpError; // 1È¸¸¸ Error Check

private:
	BOOL IsError(void);

	BOOL PartLoader(void);
	BOOL PartInRail(void);
	BOOL PartInPnp(void);
	BOOL PartIndex01(void);
	BOOL PartIndex02(void);
	BOOL PartIndex03(void);
	BOOL PartIndex04(void);
	BOOL PartRouterF(void); // 1,2
	BOOL PartRouterR(void); // 3,4
	BOOL PartOutPnp(void);
	BOOL PartADC(void);
	BOOL PartMGZLoadZ(void);

};

//////////////////////////////////////////////////////////////////////////
extern CAllHome g_allHome;
//////////////////////////////////////////////////////////////////////////

#endif//_ALLHOME_H_


