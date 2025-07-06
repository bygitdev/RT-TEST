#include "MtAXL.h"
#include "AjinLib.h"
#include <math.h>

namespace AJIN
{
	CMtAXL::CMtAXL()
	{
		m_config.axisNo			= 0;
		m_config.inposBand		= 50;
		m_config.enCw			= TRUE;
		m_config.enCCw			= TRUE;
		m_config.enServoOn		= TRUE;
		m_config.enAlarm		= TRUE;
	}

	CMtAXL::~CMtAXL()
	{

	}


	//------------------------------------------------------------------
	void CMtAXL::SetCnt(double dCnt)
	{
		if(g_bNoDevice)
		{
			m_state.cmdCnt = m_state.realCnt = dCnt;
		}
		else
		{
			AxmStatusSetCmdPos(m_config.axisNo, dCnt);
			AxmStatusSetActPos(m_config.axisNo, dCnt);
		}
	}


	//------------------------------------------------------------------
	void CMtAXL::SetUnitPerPulse(double dUnit, long lPulse)
	{
		if(g_bNoDevice)
			return;
		 AxmMotSetMoveUnitPerPulse(m_config.axisNo, dUnit, lPulse);
	}

	//------------------------------------------------------------------
	void CMtAXL::SetElecGearRatio(long lNumerator, long lDenominator)
	{
		if(g_bNoDevice)
			return;
		AxmMotSetElectricGearRatio(m_config.axisNo, lNumerator, lDenominator);
	}


	//------------------------------------------------------------------
	void CMtAXL::ReadServoState(void)
	{
		DWORD dwStatus1 = 0;
		DWORD dwStatus2 = 0;


		if(100 == m_fsmNoDevice.Get())
		{
			if(m_fsmNoDevice.Once())
				m_tmAddCnt.Reset();

			if(m_fsmNoDevice.TimeLimit(m_nNoDeviceTimeLimit))
			{
				m_state.realCnt  = m_state.cmdCnt = m_profile.pos;
				m_fsmNoDevice.Set(C_IDLE);
			}
			else
			{
				int nElapsed = m_tmAddCnt.Elapsed();
				m_tmAddCnt.Reset();
				double dAddPulse = (m_profile.vel / 1000.0) * nElapsed;

				if(m_profile.pos > m_state.cmdCnt)
					m_state.cmdCnt  = m_state.cmdCnt + dAddPulse;
				else
					m_state.cmdCnt  = m_state.cmdCnt - dAddPulse;

				m_state.realCnt = m_state.cmdCnt;
			}
		}

		if(g_bNoDevice)
		{
			m_state.isAlarm		= FALSE;
			m_state.isCw		= FALSE;
			m_state.isCCw		= FALSE;
			m_state.isOrg		= FALSE;
			m_state.isServoOn	= TRUE;
			m_state.isDriving	= m_fsmNoDevice.IsRun();
		}
		else
		{
			// 상태 업데이트 시간 단축을 위해 카운트 관련 제외하고 매번 업데이트 하지 않음..
			m_nReadState++;
			m_nReadState = (m_nReadState % 10);

			if(0 == m_nReadState)
			{
				m_state.ajinErrCode = AxmSignalReadServoAlarm(m_config.axisNo, &dwStatus1);
				m_state.isAlarm = !!dwStatus1;
				if(!m_config.enAlarm)
					m_state.isAlarm = FALSE;
			}
			else if(2 == m_nReadState)
			{
				m_state.ajinErrCode = AxmSignalReadLimit(m_config.axisNo, &dwStatus1, &dwStatus2);
				m_state.isCw = !!dwStatus1;
				m_state.isCCw = !!dwStatus2;
				if(!m_config.enCw)
					m_state.isCw = FALSE;
				if(!m_config.enCCw)
					m_state.isCCw = FALSE;
			}
			else if(4 == m_nReadState)
			{
				m_state.ajinErrCode = AxmHomeReadSignal(m_config.axisNo, &dwStatus1);
				m_state.isOrg = !!dwStatus1;

				
			}
			else if(8 == m_nReadState)
			{
				m_state.ajinErrCode = AxmSignalIsServoOn(m_config.axisNo, &dwStatus1);
				m_state.isServoOn = !!dwStatus1;

				if(!m_config.enServoOn)
					m_state.isServoOn = TRUE;
			}

			m_state.ajinErrCode = AxmStatusReadInMotion(m_config.axisNo, &dwStatus1);
			m_state.isDriving = !!dwStatus1;

			double dCmdCnt = 0;
			m_state.ajinErrCode = AxmStatusGetCmdPos(m_config.axisNo, &dCmdCnt);
			m_state.cmdCnt = (int)dCmdCnt;

			double dRealCnt = 0;
			m_state.ajinErrCode = AxmStatusGetActPos(m_config.axisNo, &dRealCnt);
			if(0 != m_state.ajinErrCode)
			{
				if(m_state.isHome)
				{
					m_state.isHome = FALSE;
					printf("\n Axis[%d] Real-Cnt Read Err[%d]", m_config.axisNo, m_state.ajinErrCode);
				}
			}
			else
			{
				m_state.realCnt = (int)dRealCnt;
			}
		}
	}


	//------------------------------------------------------------------
	BOOL CMtAXL::Update(void)
	{
		ReadServoState();
		AlarmClearProc();
		HomeSearchProc();

		m_state.isErr = (!m_state.isServoOn || m_state.isAlarm);
		m_state.isHoming = m_fsmHome.IsRun();
		

		if(m_state.isErr)
		{
			m_state.isHome = FALSE;
			if(m_fsmHome.IsRun())
				CancelHomeSearch();
			if(m_fsmDrv.IsRun())
				m_fsmDrv.Set(C_IDLE);
		}

		if(FALSE == m_state.isHome)
		{
			m_state.isPaused = FALSE;
		}

		switch(m_fsmDrv.Get())
		{
		case C_DRV_START:
			if(m_fsmDrv.Once())
			{
				if(g_bNoDevice)
				{
					m_fsmNoDevice.Set(100);
					m_nNoDeviceTimeLimit  = (int)(((m_profile.pos - m_state.realCnt) / m_profile.vel) * 1000); // mSec
					m_nNoDeviceTimeLimit = abs(m_nNoDeviceTimeLimit);
				}
			}
			else
			{
				if(g_bNoDevice)
				{
					if(m_fsmNoDevice.IsRun())
						break;
				}
				else
				{
					if(!m_fsmDrv.Delay(5))
						break;
				}

				m_fsmDrv.Set(C_DRV_END);
			}
			break;

		case C_DRV_STOP:
			if(m_state.isDriving)
				break;

			m_profile.pos = m_state.cmdCnt;
			m_fsmDrv.Set(C_IDLE);
			break;

		case C_DRV_PAUSED:
			if(m_state.isDriving)
				break;
			m_fsmDrv.Set(C_IDLE);
			break;

		case C_DRV_END:
			if(m_state.isDriving)
				break;
			m_profile.curIndex = m_profile.cmdIndex;
			m_fsmDrv.Set(C_IDLE);
			break;
		}

		return (TRUE);
	}

	//------------------------------------------------------------------
	// 실시간 real-count값이 필요할때..
	double CMtAXL::GetRealCnt(void)
	{
		if(g_bNoDevice)
			return (m_state.realCnt);

		double dRealCnt = 0;
		m_state.ajinErrCode = AxmStatusGetActPos(m_config.axisNo, &dRealCnt);

		if(0 == m_state.ajinErrCode)
			return (dRealCnt);
		else
			return (m_state.realCnt);
	}
	
	//------------------------------------------------------------------
	void CMtAXL::ServoOn(void)
	{
		if(g_bNoDevice)
		{
			m_state.isServoOn = TRUE;
			SetCnt(0);
		}
		else
		{
			AxmMotSetAccelJerk(m_config.axisNo, 100);
			AxmMotSetDecelJerk(m_config.axisNo, 100);
			AxmMotSetAccelUnit(m_config.axisNo, UNIT_SEC2);

			m_state.isServoOn = TRUE;
			SetCnt(m_state.realCnt);
			AxmSignalServoOn(m_config.axisNo, TRUE);
		}
	}


	//------------------------------------------------------------------
	void CMtAXL::ServoOff(void)
	{
		m_state.isServoOn = FALSE;

		if(!g_bNoDevice)
			AxmSignalServoOn(m_config.axisNo, FALSE);
	}

	

	//------------------------------------------------------------------
	void CMtAXL::AlarmClear(void)
	{
		if(g_bNoDevice)
			return;

		if(FALSE == m_fsmAlarm.IsRun())
			m_fsmAlarm.Set(C_CLEAR_ALARM_START);
	}

	

	//------------------------------------------------------------------
	void CMtAXL::AlarmClearProc(void)
	{
		if(g_bNoDevice)
			return;

		switch(m_fsmAlarm.Get())
		{
		case C_CLEAR_ALARM_START:
			AxmSignalServoAlarmReset(m_config.axisNo, TRUE);
			if(!m_fsmAlarm.Delay(500))
				break;
			m_fsmAlarm.Set(C_CLEAR_ALARM_END);
			break;

		case C_CLEAR_ALARM_END:
			AxmSignalServoAlarmReset(m_config.axisNo, FALSE);
			m_fsmAlarm.Set(C_IDLE);
			break;
		}
	}


	//------------------------------------------------------------------
	BOOL CMtAXL::IsRdy(int nIndex)
	{
		if(m_fsmDrv.IsRun() || m_fsmHome.IsRun() || m_state.isErr || m_state.isPaused)
			return (FALSE);
		if(!m_state.isHome || m_state.isDriving)
			return (FALSE);

		if(0 <= nIndex)
		{
			if(nIndex != m_profile.cmdIndex)
				return (false);
		}
		return (TRUE);
	}



	//------------------------------------------------------------------
	BOOL CMtAXL::InPos(int nIndex)
	{
		if(m_profile.cmdIndex != nIndex)
			return (false);

		double dMin = m_pTable->pos[nIndex] - m_config.inposBand;
		double dMax = m_pTable->pos[nIndex] + m_config.inposBand;

		if(dMin > m_state.realCnt)
			return (false);
		if(dMax < m_state.realCnt)
			return (false);

		return (true);
	}

	//------------------------------------------------------------------
	BOOL CMtAXL::InPos(int nIndex, double dInposBand)
	{
		if(m_profile.cmdIndex != nIndex)
			return (false);

		double dMin = m_pTable->pos[nIndex] - dInposBand;
		double dMax = m_pTable->pos[nIndex] + dInposBand;

		if(dMin > m_state.realCnt)
			return (false);
		if(dMax < m_state.realCnt)
			return (false);

		return (true);
	}

	//------------------------------------------------------------------
	BOOL CMtAXL::InPos(int nIndex, double dPos, double dInposBand)
	{
		if(m_profile.cmdIndex != nIndex)
			return (false);

		double dMin = dPos - dInposBand;
		double dMax = dPos + dInposBand;

		if(dMin > m_state.realCnt)
			return (false);
		if(dMax < m_state.realCnt)
			return (false);

		return (true);
	}



	//------------------------------------------------------------------
	BOOL CMtAXL::BetweenIdx(int nIndex1, int nIndex2, double dInposBand)
	{
		double dMin = 0, dMax = 0;

		double dAddInposBand = m_config.inposBand;
		if(0 < dInposBand)
			dAddInposBand = dInposBand;

		if(m_pTable->pos[nIndex1] < m_pTable->pos[nIndex2])
		{
			dMin = m_pTable->pos[nIndex1] - dAddInposBand;
			dMax = m_pTable->pos[nIndex2] + dAddInposBand;
		}
		else
		{
			dMin = m_pTable->pos[nIndex2] - dAddInposBand;
			dMax = m_pTable->pos[nIndex1] + dAddInposBand;
		}

		if(dMin > m_state.realCnt)
			return (false);
		if(dMax < m_state.realCnt)
			return (false);

		return (true);
	}


	//------------------------------------------------------------------
	BOOL CMtAXL::BetweenPos(double dPos1, double dPos2, double dInposBand)
	{
		double dMin = 0, dMax = 0;

		double dAddInposBand = m_config.inposBand;
		if(0 < dInposBand)
			dAddInposBand = dInposBand;

		if(dPos1 < dPos2)
		{
			dMin = dPos1 - dAddInposBand;
			dMax = dPos2 + dAddInposBand;
		}
		else
		{
			dMin = dPos2 - dAddInposBand;
			dMax = dPos1 + dAddInposBand;
		}

		if(dMin > m_state.realCnt)
			return (false);
		if(dMax < m_state.realCnt)
			return (false);

		return (true);
	}


	//------------------------------------------------------------------
	BOOL CMtAXL::BetweenPos(int nIndex, double dRange)
	{
		double dMin = 0, dMax = 0;

		dMin = m_pTable->pos[nIndex] - dRange;
		dMax = m_pTable->pos[nIndex] + dRange;

		if(dMin > m_state.realCnt)
			return (false);
		if(dMax < m_state.realCnt)
			return (false);

		return (true);
	}
	

	BOOL CMtAXL::ComparePos(int nIndex)
	{
		if(m_profile.cmdIndex == nIndex || m_profile.curIndex == nIndex)
			return (TRUE);

		return (FALSE);
	}//-------------------------------------------------------------------


	//------------------------------------------------------------------
	BOOL CMtAXL::Move(int nIndex, double dVel)
	{
		if(m_fsmDrv.IsRun() || m_fsmHome.IsRun() || m_state.isErr || m_state.isPaused)
			return (FALSE);
		

		m_profile.cmdIndex		= nIndex;
		m_profile.pos			= m_pTable->pos[nIndex];
		m_profile.acc			= m_pTable->acc[nIndex];

		if(0 == (int)dVel)
			m_profile.vel = m_pTable->vel[nIndex];
		else
			m_profile.vel = dVel;

		m_state.isFwdDir = (m_profile.pos > m_state.realCnt);

		if(g_bNoDevice)
			return (true);
		
		AxmMotSetProfilePriority(m_config.axisNo, PRIORITY_ACCELTIME);
		m_state.ajinErrCode = AxmMoveStartPos(m_config.axisNo, m_profile.pos, m_profile.vel, m_profile.acc, m_profile.acc);
		m_fsmDrv.Set(C_DRV_START);

		if(0 != m_state.ajinErrCode)
		{
			printf("\n Axis[%d] Move Err[%d]", m_config.axisNo, m_state.ajinErrCode);
		}
		return (!m_state.ajinErrCode);
	}

    //------------------------------------------------------------------
    BOOL CMtAXL::Move(int nIndex, double dPos, double dVel, double dAcc)
    {
        if(m_fsmDrv.IsRun() || m_fsmHome.IsRun() || m_state.isErr || m_state.isPaused)
            return (FALSE);
       
        m_profile.cmdIndex = nIndex;
        m_profile.pos = dPos;
        m_profile.acc = dAcc;
        m_profile.vel = dVel;

		m_state.isFwdDir = (m_profile.pos > m_state.realCnt);

        if(g_bNoDevice)
            return (true);

        AxmMotSetProfilePriority(m_config.axisNo, PRIORITY_ACCELTIME);
		m_state.ajinErrCode = AxmMoveStartPos(m_config.axisNo, m_profile.pos, m_profile.vel, m_profile.acc, m_profile.acc);
		m_fsmDrv.Set(C_DRV_START);

        if(0 != m_state.ajinErrCode)
        {
            printf("\n Axis[%d] Move Err[%d]", m_config.axisNo, m_state.ajinErrCode);
        }
        return (!m_state.ajinErrCode);
    }

	//--------------------------------------------------------------
	BOOL CMtAXL::PMove(int nIndex, double dPos, double dVel)
	{
		if(m_fsmDrv.IsRun() || m_fsmHome.IsRun() || m_state.isErr || m_state.isPaused)
			return (FALSE);
		
		m_profile.cmdIndex		= nIndex;
		m_profile.pos			= dPos;
		m_profile.acc			= m_pTable->acc[nIndex];

		if(0 == (int)dVel)
			m_profile.vel = m_pTable->vel[nIndex];
		else
			m_profile.vel = dVel;

		m_state.isFwdDir= (m_profile.pos > m_state.realCnt);

		if(g_bNoDevice)
			return (true);

		m_state.ajinErrCode = AxmMoveStartPos(m_config.axisNo, m_profile.pos, m_profile.vel, m_profile.acc, m_profile.acc);
		m_fsmDrv.Set(C_DRV_START);

		if(0 != m_state.ajinErrCode)
		{
			printf("\n Axis[%d] PMove Err[%d]", m_config.axisNo, m_state.ajinErrCode);
			printf("\n Index[%d] dPos[%d] dVel[%d] dAcc[%d]",nIndex, (int)m_profile.pos, (int)m_profile.vel, (int)m_profile.acc);
		}
		return (!m_state.ajinErrCode);
	}


	//--------------------------------------------------------------
	BOOL CMtAXL::Move2(int nIndex, double dPos, double dOverRidePos, double dOverRideVel, double dVel)
	{
		if(m_fsmDrv.IsRun() || m_fsmHome.IsRun() || m_state.isErr || m_state.isPaused)
			return (FALSE);
		
		m_profile.cmdIndex		= nIndex;
		m_profile.pos			= dPos;
		m_profile.acc			= m_pTable->acc[nIndex];
		if(1 > dVel)
			m_profile.vel	= m_pTable->vel[nIndex];
		else
			m_profile.vel	= dVel;

		m_state.isFwdDir = (m_profile.pos > m_state.realCnt);

		if(g_bNoDevice)
			return (true);

		m_state.ajinErrCode = AxmOverrideSetMaxVel(m_config.axisNo, 10000000);
		m_state.ajinErrCode = AxmOverrideVelAtPos(m_config.axisNo, dPos, m_profile.vel, m_profile.acc, m_profile.acc, dOverRidePos, dOverRideVel, COMMAND);
		m_fsmDrv.Set(C_DRV_START);
		
		if(0 != m_state.ajinErrCode)
		{
			printf("\n Axis[%d] Move2 Err[%d]", m_config.axisNo, m_state.ajinErrCode);
		}
		return (!m_state.ajinErrCode);
	}


	//--------------------------------------------------------------
	// SSCNet적용 안됨..
	BOOL CMtAXL::Move3(int nIndex, double dPos, int nOverRideCnt, int nOverRideMode)
	{
		if(m_fsmDrv.IsRun() || m_fsmHome.IsRun() || m_state.isErr || m_state.isPaused)
			return (FALSE);
		
		m_profile.cmdIndex		= nIndex;
		m_profile.pos			= dPos;
		m_profile.acc			= m_pTable->acc[nIndex];
		m_profile.vel			= m_pTable->vel[nIndex];

		m_state.isFwdDir = (m_profile.pos > m_state.realCnt);

		if(g_bNoDevice)
			return (true);

		m_state.ajinErrCode = AxmOverrideSetMaxVel(m_config.axisNo, 10000000);
		m_state.ajinErrCode = AxmOverrideVelAtMultiPos(m_config.axisNo, dPos, m_profile.vel, m_profile.acc, m_profile.acc,
												 nOverRideCnt, &m_overRide.pos[0], &m_overRide.vel[0],  
												 COMMAND, nOverRideMode); // overRideMode : OVERRIDE_POS_START
		m_fsmDrv.Set(C_DRV_START);

		if(0 != m_state.ajinErrCode)
		{
			printf("\n Axis[%d] Move3 Err[%d]", m_config.axisNo, m_state.ajinErrCode);
		}
		return (!m_state.ajinErrCode);
	}


	//------------------------------------------------------------------
	BOOL CMtAXL::RMove(double dPulse, double dVel)
	{
		if(m_fsmDrv.IsRun() || m_fsmHome.IsRun() || m_state.isErr || m_state.isPaused)
			return (FALSE);
		
		m_profile.pos = m_state.cmdCnt + dPulse;
		m_profile.acc = m_pTable->acc[m_config.homeIdx];
		m_profile.vel = dVel;

		m_state.isFwdDir = (m_profile.pos > m_state.realCnt);

		if(g_bNoDevice)
			return (true);

		m_state.ajinErrCode = AxmMoveStartPos(m_config.axisNo, m_profile.pos, m_profile.vel, m_profile.acc, m_profile.acc);
		m_fsmDrv.Set(C_DRV_START);

		if(0 != m_state.ajinErrCode)
		{
			printf("\n Axis[%d] RMove Err[%d]", m_config.axisNo, m_state.ajinErrCode);
		}
		return (!m_state.ajinErrCode);
	}


	//------------------------------------------------------------------
	BOOL CMtAXL::VelMove(int nIndex, int nDirection)
	{
		if(m_fsmDrv.IsRun() || m_fsmHome.IsRun() || m_state.isErr || m_state.isPaused)
			return (FALSE);

		m_profile.acc			= m_pTable->acc[nIndex];
		m_profile.vel			= m_pTable->vel[nIndex];

		if(0 >= nDirection)
			m_profile.vel = m_profile.vel * -1.0;

		if(g_bNoDevice)
			return (true);

		m_state.ajinErrCode = AxmMoveVel(m_config.axisNo, m_profile.vel, m_profile.acc, m_profile.acc);
		if(0 != m_state.ajinErrCode)
		{
			printf("\n Axis[%d] VelMove Err[%d]", m_config.axisNo, m_state.ajinErrCode);
		}
		return (!m_state.ajinErrCode);
	}


	//------------------------------------------------------------------
	void CMtAXL::Stop(BOOL bEStop)
	{
		if(!g_bNoDevice)
		{
			if(bEStop)
				AxmMoveEStop(m_config.axisNo);
			else
				AxmMoveSStop(m_config.axisNo);
		}
		m_fsmNoDevice.Set(C_IDLE);
		m_fsmDrv.Set(C_DRV_STOP);
	}


	//------------------------------------------------------------------
	void CMtAXL::Paused(void)
	{
		if(g_bNoDevice)
			return;

		if(m_state.isPaused)
			return;
		
		if(m_fsmHome.IsRun())
		{
			CancelHomeSearch();
			return;
		}

		if(m_state.isDriving)
		{
			if(m_state.isHome)
				m_state.isPaused = TRUE;

			AxmMoveSStop(m_config.axisNo);
			m_fsmDrv.Set(C_DRV_PAUSED);
			m_fsmNoDevice.Set(C_IDLE);
		}
	}

	//------------------------------------------------------------------
	void CMtAXL::Resume(void)
	{
		if(g_bNoDevice)
			return;

		if(m_state.isPaused)
		{
			m_state.isPaused = FALSE;
			m_state.ajinErrCode = AxmMoveStartPos(m_config.axisNo, m_profile.pos, m_profile.vel, m_profile.acc, m_profile.acc);
			m_fsmDrv.Set(C_DRV_START);

			if(0 != m_state.ajinErrCode)
			{
				m_state.isHome = FALSE;
				printf("\n Axis[%d] Resume Err[%d]", m_config.axisNo, m_state.ajinErrCode);
			}
		}
	}

	//------------------------------------------------------------------
	void CMtAXL::OverRideVel(int vel) // 구동중 속도 가변
	{
		if(g_bNoDevice)
			return;

		m_state.ajinErrCode = AxmOverrideVel(m_config.axisNo, vel);

		if(0 != m_state.ajinErrCode)
			printf("\n Axis[%d] Over Ride Err[%d]", m_config.axisNo, m_state.ajinErrCode);
	}

	//------------------------------------------------------------------
	void CMtAXL::StartHomeSearch(void)
	{
		if(m_fsmDrv.IsRun() || m_state.isErr)
		{
			printf("\n Axis[%d] Fail start home-search!!", m_config.axisNo);
			return;
		}

		if(C_IDLE == m_fsmHome.Get())
		{
			m_profile.cmdIndex = MIDX_JOG;
			m_profile.curIndex = MIDX_JOG;

			if(g_bNoDevice)
			{
				m_profile.cmdIndex = m_profile.curIndex = m_config.homeIdx;
				SetCnt(m_pTable->pos[m_config.homeIdx]);
				m_state.isPaused = FALSE;
				m_state.isHome = FALSE;
			}
			else
			{
				m_state.isPaused = FALSE;
				m_state.isHome = FALSE;
				m_state.ajinErrCode = AxmHomeSetStart(m_config.axisNo);
				m_fsmHome.Set(C_START_HOME);

				if(0 != m_state.ajinErrCode)
				{
					printf("\n Axis[%d] Fail start home-search Err[%d]!!", m_config.axisNo, m_state.ajinErrCode);
					m_fsmHome.Set(C_IDLE);
				}
			}
		}
	}

	//------------------------------------------------------------------
	void CMtAXL::CancelHomeSearch(void)
	{
		m_profile.curIndex     = MIDX_JOG;
		m_profile.nextIndex	= MIDX_JOG;
		m_state.isHome       = FALSE;
		m_state.isPaused     = FALSE;

		m_fsmNoDevice.Set(C_IDLE);

		if(g_bNoDevice)
			return;

		AxmMoveSStop(m_config.axisNo);
		m_fsmHome.Set(C_IDLE);
	}

	

	//------------------------------------------------------------------
	void CMtAXL::HomeSearchProc(void)
	{
		if(!m_fsmHome.IsRun())
			return;
		if(g_bNoDevice)
			return;

		if(C_ERROR == m_fsmHome.Get())
		{
			printf("\n Axis[%d] Fail home search Err[%X]!!", m_config.axisNo, m_fsmHome.GetErr());
			CancelHomeSearch();
			return;
		}
		else if(m_fsmHome.TimeLimit(60000))
		{
			m_fsmHome.Set(C_ERROR);
			return;
		}

		switch(m_fsmHome.Get())
		{
		case C_START_HOME:
			{
				if(!m_state.isServoOn || m_state.isAlarm)
				{
					Stop(FALSE);
					m_fsmHome.Set(C_ERROR);
					break;
				}

				DWORD dwResult;
				AxmHomeGetResult(m_config.axisNo, &dwResult);

				if(HOME_SEARCHING == dwResult)
					break;

				if(HOME_SUCCESS == dwResult)
					m_fsmHome.Set(C_END_HOME);
				else
					m_fsmHome.Set(C_ERROR, dwResult);
			}
			break;

		case C_END_HOME:
			if(m_fsmHome.Once())
			{
				m_profile.pos = m_pTable->pos[m_config.homeIdx];
				m_profile.acc = m_pTable->acc[m_config.homeIdx];
				m_profile.vel = m_pTable->vel[m_config.homeIdx] * 0.5;
				if(g_bNoDevice)
					break;
				AxmMoveStartPos(m_config.axisNo, m_profile.pos, m_profile.vel, m_profile.acc, m_profile.acc);
			}
			else
			{
				if(!m_fsmHome.Delay(500))
					break;
				if(m_state.isDriving)
					break;

				m_profile.cmdIndex = m_config.homeIdx;
				m_profile.curIndex = m_config.homeIdx;

				m_state.isHome = TRUE;
				m_fsmHome.Set(C_IDLE);
			}
			break;
		}
	}



	//------------------------------------------------------------------
	// ECT 설정..
	// ECT ENABLE은 ServoOff상태에서만 가능..
	// ECT WRITE는  ECT DISABLE상태에서만 가능..
	BOOL CMtAXL::SetEct(BOOL bEnable)
	{
		m_state.ajinErrCode = AxmCompensationEnable(m_config.axisNo, bEnable);
		if(m_state.ajinErrCode != 0)
		{
			if(bEnable)
			{
				if(m_state.isServoOn)
					printf("\n Axis[%d] Failed to ECT Enable!! : Servo off first!!", m_config.axisNo);
			}

			printf("\n ErrNo[%d] : Axis[%d] Failed to ECT Enable/Disable!!", m_state.ajinErrCode, m_config.axisNo);
		}

		return (!m_state.ajinErrCode);
	}


	//------------------------------------------------------------------
	// Master의 축번호가 더 낮아야함.
	// Master, Slave축은 같은 모션 IC안에 있어야함, 4축 단위..
	// uSlHomeUse     : 슬레이축 홈사용 우뮤 ( 0 - 2)
	//             (0 : 슬레이브축 홈을 사용안하고 마스터축을 홈을 찾는다.)
	//             (1 : 마스터축 , 슬레이브축 홈을 찾는다. 슬레이브 dSlOffset 값 적용해서 보정함.)
	//             (2 : 마스터축 , 슬레이브축 홈을 찾는다. 슬레이브 dSlOffset 값 적용해서 보정안함.)
	// dSlOffset      : 슬레이브축 옵셋값
	// dSlOffsetRange : 슬레이브축 옵셋값 레인지 설정
	int CMtAXL::SetGantry(void)
	{
		DWORD dwHomeUse, dwGantryOn;
		double dSlOffset, dSlRange;

		AxmGantryGetEnable(m_config.axisNo, &dwHomeUse, &dSlOffset, &dSlRange, &dwGantryOn);
		if(dwGantryOn)
		{
			printf("\n Gantry[%d] Already Enable!!", m_config.axisNo);
			return (0);
		}

		int nRet = AxmGantrySetEnable(m_config.axisNo, m_gantry.slaveAxis, m_gantry.homeMode, m_gantry.slaveOffset, 10000); // 1) 마스터축 , 슬레이브축 홈을 찾는다
		if(0 == nRet)
		{
			printf("\n Gantry[%d] Enable!!", m_config.axisNo);
			return (0);
		}
		else
		{
			printf("\n Fail Enable Gantry[%d]!!", m_config.axisNo);
			return (nRet);
		}
	}

	int CMtAXL::ResetGantry(void)
	{
		int nRet = AxmGantrySetDisable(m_config.axisNo, m_gantry.slaveAxis);
		return (nRet);
	}


	/********************************************************************
	- Trigger Class
	********************************************************************/


	//////////////////////////////////////////////////////////////////////////
	// trig_time          : 트리거 출력 시간 
	//                    : 1usec - 최대 50msec ( 1 - 50000 까지 설정)
	// trig_level         : 트리거 출력 레벨 유무  => LOW(0),     HIGH(1)
	// source_select      : 사용할 기준 위치       => COMMAND(0), ACTUAL(1)
	// interrupt_use      : 인터럽트 설정          => DISABLE(0), ENABLE(1)
	DWORD CMtQITrigger::SetTrigger(long axisNo, double trig_time, DWORD trig_level, DWORD source_select, DWORD interrupt_use)
	{
		m_lAxisNo			= axisNo;
		m_dbTriggerTime		= trig_time;
		m_dwTriggerLevel	= trig_level;
		m_dwTriggerSource	= source_select;
		m_dwInterruptUse	= interrupt_use;

		m_dwLastError = AxmTriggerSetTimeLevel(m_lAxisNo, m_dbTriggerTime, m_dwTriggerLevel,m_dwTriggerSource, m_dwInterruptUse);
		return (m_dwLastError);
	}


	//////////////////////////////////////////////////////////////////////////
	// uMethod : PERIOD_MODE  0x0 : 현재 위치를 기준으로 dPos를 위치 주기로 사용한 주기 트리거 방식
	//           ABS_POS_MODE 0x1 : 트리거 절대 위치에서 트리거 발생, 절대 위치 방식
	// dPos    : 주기 선택시 : 위치마다위치마다 출력하기때문에 그 위치
	//           절대 선택시 : 출력할 그 위치, 이 위치와같으면 무조건 출력이 나간다.
	DWORD CMtQITrigger::StartTrigAbsPeriod(DWORD out_method, double pos)
	{
		m_dwLastError =  AxmTriggerSetAbsPeriod (m_lAxisNo, out_method, pos);
		return (m_dwLastError);
	}


	DWORD CMtQITrigger::StartTrigBlock(double start_pos, double end_pos, double period_pos)
	{
		m_dwLastError = AxmTriggerSetBlock (m_lAxisNo, start_pos, end_pos, period_pos);
		return (m_dwLastError);
	}


	DWORD CMtQITrigger::StartTrigOnlyAbs(long trig_no, double* pTrig_pos)
	{
		m_dwLastError =  AxmTriggerOnlyAbs (m_lAxisNo, trig_no, pTrig_pos);
		return (m_dwLastError);
	}

	DWORD CMtQITrigger::ResetTrigger(void)
	{
		m_dwLastError = AxmTriggerSetReset (m_lAxisNo);
		return (m_dwLastError);
	}

	DWORD CMtQITrigger::StartTrigOneShot(void)
	{
		m_dwLastError = AxmTriggerOneShot(m_lAxisNo);
		return (m_dwLastError);
	}

	DWORD CMtQITrigger::VirtualMove(double dPos, double dVel, double dAcc)
	{
		m_dwLastError = AxmMoveVel(m_lAxisNo, dVel, dAcc, dAcc);
		//m_dwLastError = AxmMoveStartPos(m_lAxisNo, dPos, dVel, dAcc, dAcc);
		return (m_dwLastError);
		
	}

	DWORD CMtQITrigger::VirtualStop(void)
	{
		m_dwLastError = AxmMoveEStop(m_lAxisNo);
		return (m_dwLastError);
	}

	DWORD CMtQITrigger::SetCnt(double dCnt)
	{
		m_dwLastError = AxmStatusSetCmdPos(m_lAxisNo, dCnt);
		m_dwLastError = AxmStatusSetActPos(m_lAxisNo, dCnt);
		return (m_dwLastError);
	}


	//------------------------------------------------------------------
	BOOL MultiMove(CMtAXL* axis1, CMtAXL* axis2, int nIndex, double dVel)
	{
		if(axis1->m_fsmDrv.IsRun() || axis1->m_fsmHome.IsRun() || axis1->m_state.isErr || axis1->m_state.isPaused)
			return (FALSE);
		if(axis2->m_fsmDrv.IsRun() || axis2->m_fsmHome.IsRun() || axis2->m_state.isErr || axis2->m_state.isPaused)
			return (FALSE);

		long lArraySize = 2;
		long lArrayAxisNo[2] = {axis1->m_config.axisNo, axis2->m_config.axisNo};

		axis1->m_profile.cmdIndex = nIndex;
		axis1->m_profile.pos = axis1->m_pTable->pos[nIndex];
		axis1->m_profile.acc = axis1->m_pTable->acc[nIndex];

		axis2->m_profile.cmdIndex = nIndex;
		axis2->m_profile.pos = axis2->m_pTable->pos[nIndex];
		axis2->m_profile.acc = axis2->m_pTable->acc[nIndex];

		if(0 == (int)dVel)
		{
			axis1->m_profile.vel = axis2->m_pTable->vel[nIndex];
			axis2->m_profile.vel = axis2->m_pTable->vel[nIndex];
		}
		else
		{
			axis1->m_profile.vel = dVel;
			axis2->m_profile.vel = dVel;
		}

		double dArryPos[2] = {axis1->m_profile.pos, axis2->m_profile.pos};
		double dArryVel[2] = {axis1->m_profile.vel, axis2->m_profile.vel};
		double dArryAcc[2] = {axis1->m_profile.acc, axis2->m_profile.acc};

		axis1->m_state.isFwdDir = (axis1->m_profile.pos > axis1->m_state.realCnt);

		if(g_bNoDevice)
			return (true);
		
		AxmMotSetProfilePriority(axis1->m_config.axisNo, PRIORITY_ACCELTIME);
		AxmMotSetProfilePriority(axis2->m_config.axisNo, PRIORITY_ACCELTIME);
	
		DWORD ajinErrCode = AxmMoveStartMultiPos(lArraySize, lArrayAxisNo, dArryPos, dArryVel, dArryAcc, dArryAcc);
	
		axis1->m_fsmDrv.Set(C_DRV_START);
		axis2->m_fsmDrv.Set(C_DRV_START);

		if(0 != ajinErrCode)
		{
			printf("\n Axis[%d], Axis[%d], Multi Move Err[%d]", axis1->m_config.axisNo, axis2->m_config.axisNo, ajinErrCode);
		}
		return (!ajinErrCode);
	}


	//------------------------------------------------------------------
	BOOL MultiRMove(CMtAXL* axis1, CMtAXL* axis2, double dPulse, double dVel)
	{
		if(axis1->m_fsmDrv.IsRun() || axis1->m_fsmHome.IsRun() || axis1->m_state.isErr || axis1->m_state.isPaused)
			return (FALSE);
		if(axis2->m_fsmDrv.IsRun() || axis2->m_fsmHome.IsRun() || axis2->m_state.isErr || axis2->m_state.isPaused)
			return (FALSE);

		long lArraySize = 2;
		long lArrayAxisNo[2] = {axis1->m_config.axisNo, axis2->m_config.axisNo};

		axis1->m_profile.pos = axis1->m_state.cmdCnt + dPulse;
		axis1->m_profile.acc = axis1->m_pTable->acc[axis1->m_config.homeIdx];
		axis1->m_profile.vel = dVel;

		axis2->m_profile.pos = axis1->m_state.cmdCnt + dPulse;
		axis2->m_profile.acc = axis1->m_pTable->acc[axis1->m_config.homeIdx];
		axis2->m_profile.vel = dVel;

		axis1->m_state.isFwdDir = (axis1->m_profile.pos > axis1->m_state.realCnt);
		axis2->m_state.isFwdDir = (axis2->m_profile.pos > axis2->m_state.realCnt);

		double dArryPos[2] = {axis1->m_profile.pos, axis2->m_profile.pos};
		double dArryVel[2] = {axis1->m_profile.vel, axis2->m_profile.vel};
		double dArryAcc[2] = {axis1->m_profile.acc, axis2->m_profile.acc};

		if(g_bNoDevice)
			return (true);

		DWORD ajinErrCode = AxmMoveStartMultiPos(lArraySize, lArrayAxisNo, dArryPos, dArryVel, dArryAcc, dArryAcc);
	
		axis1->m_fsmDrv.Set(C_DRV_START);
		axis2->m_fsmDrv.Set(C_DRV_START);

		if(0 != ajinErrCode)
		{
			printf("\n Axis[%d], Axis[%d], Multi Move Err[%d]", axis1->m_config.axisNo, axis2->m_config.axisNo, ajinErrCode);
		}
		return (!ajinErrCode);
	}


	//------------------------------------------------------------------
	void MultiStop(CMtAXL* axis1, CMtAXL* axis2, BOOL bEStop)
	{
		long lArraySize = 2;
		long lArrayAxisNo[2] = {axis1->m_config.axisNo, axis2->m_config.axisNo};
	
		if(!g_bNoDevice)
		{
			//if(bEStop)
			//	AxmMoveMultiEStop(lArraySize, lArrayAxisNo);
			//else
			//	AxmMoveMultiSStop(lArraySize, lArrayAxisNo);
		}
	
		axis1->m_fsmNoDevice.Set(C_IDLE);
		axis2->m_fsmNoDevice.Set(C_IDLE);
	
		axis1->m_fsmDrv.Set(C_DRV_STOP);
		axis2->m_fsmDrv.Set(C_DRV_STOP);
	}

}
