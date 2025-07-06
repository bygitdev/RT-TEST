#include "..\def\Includes.h"

////////////////////////////////////////////////////////////////////
CLampBuzzer g_lampBuzzer;
/////////////////////////////////////////////////////////////////////


#define LAMP_BUZZER_CONFIG	(towerLampR1)


//-------------------------------------------------------------------
void CLampBuzzer::Run(void)
{
	m_bOnOff = m_tmBlink.Blink(TRUE, 500, 500);

	int nCurState = GetState();

	if(m_nOldState != nCurState)
	{
		m_tmBuzzer.Reset();
		m_nOldState = nCurState;
		m_bBuzzerOff = FALSE;
	}

	if(g_pNV->NDm(buzzerSkip))
		m_bBuzzerOff = TRUE;

	// Tower Lamp
	if(ON == m_pConfig->_state[nCurState].nGreen)
		m_oLampG.On();
	else if(OFF == m_pConfig->_state[nCurState].nGreen)
		m_oLampG.Off();
	else
		m_oLampG.Set(m_bOnOff);

	if(ON == m_pConfig->_state[nCurState].nRed)
		m_oLampR.On();
	else if(OFF == m_pConfig->_state[nCurState].nRed)
		m_oLampR.Off();
	else
		m_oLampR.Set(m_bOnOff);

	if(ON == m_pConfig->_state[nCurState].nYellow)
		m_oLampY.On();
	else if(OFF == m_pConfig->_state[nCurState].nYellow)
		m_oLampY.Off();
	else
		m_oLampY.Set(m_bOnOff);

	//Buzzer On/Off
	if(m_bBuzzerOff)
	{
		BuzzerOnOff(0);
	}
	else
	{
		int nBuzzerOnTime = (m_pConfig->_nOffTime * 1000); // 초단위 설정
		BOOL bSkip = !!m_pConfig->_nSkip;

		if(bSkip || m_tmBuzzer.TmOver(nBuzzerOnTime))
			BuzzerOnOff(0);
		else
			BuzzerOnOff(m_pConfig->_state[nCurState].nBuzzer);
	}
}


//-------------------------------------------------------------------
BOOL CLampBuzzer::Init(void)
{
	m_pConfig = (Config*)&g_pNV->NDm(LAMP_BUZZER_CONFIG);

	m_oLampR.m_nNo = oTowerLampR;
	m_oLampY.m_nNo = oTowerLampY;
	m_oLampG.m_nNo = oTowerLampG;
	m_oBuzzer[0].m_nNo = oBuzzer01;
	m_oBuzzer[1].m_nNo = oBuzzer02;
	m_oBuzzer[2].m_nNo = oBuzzer03;
	m_oBuzzer[3].m_nNo = oBuzzer04;
	m_bBuzzerOff = TRUE;
	return (TRUE);
}


//-------------------------------------------------------------------
int CLampBuzzer::GetState(void)
{
	int nType = STOP;

	if(g_opr.isAuto)
	{
		if(0 < g_wr.GetNo())
			nType = ALARM;
		else
			nType = RUN;
	}
	else
	{
		if(0 < g_err.GetNo())
			nType = ERR;
		else
		{
			if(0 < g_wr.GetNo())
				nType = ALARM;
		}
	}

	return nType;
}


//-------------------------------------------------------------------
void CLampBuzzer::BuzzerOnOff(int nBuzzNo)
{
	m_oBuzzer[0].Set((1 == nBuzzNo));
	m_oBuzzer[1].Set((2 == nBuzzNo));
	m_oBuzzer[2].Set((3 == nBuzzNo));
	m_oBuzzer[3].Set((4 == nBuzzNo));
}


//-------------------------------------------------------------------
void CLampBuzzer::BuzzerOff(void)
{
	m_bBuzzerOff = TRUE;
}


//-------------------------------------------------------------------
BOOL CLampBuzzer::GetBlink(void)
{
	return (m_bOnOff);
}
