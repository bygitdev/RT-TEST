#include "..\def\Includes.h"


/////////////////////////////////////////////////////////////////////
CTrackOut g_trackOut;
/////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
void CTrackOut::AutoRun(void)
{
	CycleRun();

	if(m_fsm.IsRun())
		return;

	for(int color = 0; color < 8; color++)
	{
		if(m_pnTrackOutActivated[color] && !IsExistInMc(color))
		{
			m_fsm.Set(C_TRACK_OUT_START);
			break;
		}
	}

}



void CTrackOut::CycleRun(void)
{
	if(!m_fsm.IsRun())
		return;

	if(C_ERROR == m_fsm.Get())
	{
		if(0 < m_fsm.GetMsg())
			g_err.Save(m_fsm.GetMsg());

		m_fsm.Set(C_IDLE);
		return;
	}
	
	switch(m_fsm.Get())
	{
	case C_TRACK_OUT_START:
		if(m_fsm.Once())
		{
			colorNo = m_fsm.GetMsg();
			g_pNV->NDm(stateTrackOut) = STATE_IDLE;
		}
		else
		{
			if(g_pNV->NDm(mmiBitSkipTrackOut))
				m_fsm.Set(C_TRACK_OUT_END);
			else if(g_pNV->UseSkip(usTrackInOut))
				m_fsm.Set(C_TRACK_OUT_COMM);
			else
				m_fsm.Set(C_TRACK_OUT_END);
		}
		break;

	case C_TRACK_OUT_COMM:
		if(m_fsm.Once())
		{
			//g_pNV->NDm(trackOutColor) = colorNo;
			g_pNV->NDm(stateTrackOut) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_fsm.Set(C_ERROR, ER_TK_OUT_TM_OVER);
				break;
			}

			int state = g_pNV->NDm(stateTrackOut);

			switch(state)
			{
			case STATE_BUSY:
				break;

			case STATE_COMP:
				g_pNV->NDm(stateTrackOut) = STATE_IDLE;
				m_fsm.Set(C_TRACK_OUT_END);
				break;

			case STATE_ERR:
				m_fsm.Set(C_ERROR, ER_TK_OUT_NG);
				break;
			}
		}
		break;

	case C_TRACK_OUT_END:
		{
			m_pnTrackOutActivated[colorNo] = FALSE;
			g_pNV->NDm(mmiBitSkipTrackOut)   = FALSE;
			m_fsm.Set(C_IDLE);
		}
		break;
	}
}

//-------------------------------------------------------------------
void CTrackOut::Init(void)
{
	m_pnTrackOutActivated = &g_pNV->m_pData->ndm[isTrackActivated1];
}


//-------------------------------------------------------------------
BOOL CTrackOut::IsExistInMc(int color)
{
	BOOL isExist = FALSE;
	//isExist |= (g_pNV->NDm(existInMz) && (color == g_pNV->NDm(lotColorInMz)));
	//isExist |= (g_pNV->NDm(existRail) && (color == g_pNV->NDm(lotColorRail)));
	//isExist |= (g_pNV->NDm(existInPicker) && (color == g_pNV->NDm(lotColorInPicker)));
	//isExist |= (g_pNV->NDm(existFrontIndex) && (color == g_pNV->NDm(lotColorFrontIndex)));
	//isExist |= (g_pNV->NDm(existRearIndex) && (color == g_pNV->NDm(lotColorRearIndex)));
	//isExist |= (g_pNV->NDm(existFlipper) && (color == g_pNV->NDm(lotColorFlipper)));
	//isExist |= (g_pNV->NDm(existOutPicker) && (color == g_pNV->NDm(lotColorOutPicker)));
	//isExist |= (g_pNV->NDm(existOutRail) && (color == g_pNV->NDm(lotColorOutRail)));
	
	return (isExist);
}