

#ifndef _AJINLIB_H_
#define _AJINLIB_H_

#include <windows.h>

#include ".\AXL\AXL.h"
#include ".\AXL\AXDev.h"
#include "IOAXL.h"
#include "MtAXL.h"

#pragma comment(lib, "AXL.lib")

namespace AJIN
{
	const bool g_bNoDevice = false;

	class CAjinLib
	{
	private:
		long	m_lIrqNo;

	public:
        BOOL Open(bool bReset = true);
        BOOL Close();
        BOOL LoadMotorPara();
        BOOL SSCNetIII(long lBoardNo);
        BOOL IsAxisCntErr(long lMaxMtNo);

		CAjinLib();
		virtual ~CAjinLib();
	};
}
/////////////////////////////////////////////////////////////////////
extern AJIN::CAjinLib  g_ajinLib;
/////////////////////////////////////////////////////////////////////
using namespace AJIN;


#endif // _AJINLIB_H_
