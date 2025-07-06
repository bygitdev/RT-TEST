#ifndef _LAMPBUZZER_H_
#define _LAMPBUZZER_H_

#include <Windows.h>
#include "..\..\BASE\BaseAll.h"

#define LAMP_TYPE_MAX (7)

typedef struct _EachConfig
{
	int nRed;
	int nYellow;
	int nGreen;
	int nBuzzer;
}EachConfig;


// nDm(950)~
typedef struct _Config
{
	EachConfig	_state[LAMP_TYPE_MAX];
	int	_nSkip;
	int	_nOffTime;
}Config;


class CLampBuzzer
{
private:
	//-------------------------------------------------------------------
	// Buzzer : Error, Alarm, Strip empty, Ball empty ¿œ∂ß∏∏...
	// Buzzer : 0(Off), 1~4(On)
	// Lamp   : 0(Off), 1(On), 2(Blink)
	enum Mode
	{
		RUN			= 0,
		ERR			= 1,
		ALARM		= 2, // warning
		EMPTY_FULL	= 3,
		STOP		= 4,
		UTIL_DOWN	= 5,
		RESERVED2	= 6,
	};

	enum LampState
	{
		OFF = 0,
		ON = 1,
		BLINK = 2,
	};

	Config*			m_pConfig;
	CTimer			m_tmBuzzer;
	CBlinkTimer		m_tmBlink;

	COutPoint		m_oLampR;
	COutPoint		m_oLampY;
	COutPoint		m_oLampG;
	COutPoint		m_oBuzzer[4];

	CFSM			m_fsmTrfState;

	int				m_nOldState;
	BOOL			m_bBuzzerOff;
	BOOL			m_bOnOff;

	int  GetState(void);
	void BuzzerOnOff(int nBuzzNo);

public:
	BOOL Init(void);
	void Run(void);
	void BuzzerOff(void);
	BOOL GetBlink(void);

	CLampBuzzer() {}
	virtual ~CLampBuzzer() {}
};


/////////////////////////////////////////////////////////////////////
extern CLampBuzzer g_lampBuzzer;
/////////////////////////////////////////////////////////////////////


#endif//_LAMPBUZZER_H_