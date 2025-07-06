#ifndef _TENKEY_H_
#define _TENKEY_H_


/********************************************************************
	|1|2|3| - 0
	|4|5|6| - 1
	|7|8|9| - 2
	|*|0|#| - 3
	 | | |
	 4 5 6 
	 0 1 2
********************************************************************/


#include <windows.h>
#include <stdio.h>
#include "FSM.h"


namespace TENKEY
{
#define _ROW_CNT_   4
#define _COL_CNT_   3

#define HOME(n)		(n)
#define STEP(n)		(n + 1000)


	enum enKeyDef
	{
		CLR_KEY   = -1,
		SET_KEY   = -2,
		NO_KEY    = -3,
	};

	enum enTenkeyCmd
	{
		C_INIT_TENKEY		= 100,
		C_KEY_DISABLE		= 101,
		C_KEY_RELEASE		= 102,
		C_KEY_PRESSED		= 199,
		
		C_BLINK_START		= 200,
		C_BLINK_OFF         = 201,
		C_BLINK_ON          = 299,
	};

	typedef struct _SegData_
	{
		WORD lDigit:4;
		WORD mDigit:4;
		WORD hDigit:1;
		WORD reserved:7;
	}SEGMENT;

	typedef struct _KeyPad_
	{
		WORD col:3;
		WORD row:4;
		WORD reserved:9;
	}KEY_PAD;

	class CTenkey
	{
	public:
		void SetIO(PUSHORT pInAddr, PUSHORT pOutAddr);
		int	 GetTenkeyNo(void);		
		
		void Enable(void);
		void Disable(void);

		int  GetSegNo(void);
		int	 GetKeyPadNo(void);

	private:
		BOOL		m_bRst;
		SEGMENT*	m_pSegmentOutput;
		SEGMENT		m_tmpSegment;
		SEGMENT		m_segment;
		KEY_PAD*	m_pKeyPad;

		CFSM		m_fsmKey;
		CFSM		m_fsmBlink;
		
		void Display(void);
		int  ConvHexToDec(WORD wVal);

	public:
		CTenkey();
		virtual ~CTenkey();
	};
}

using namespace TENKEY;

#endif// _TENKEY_H_