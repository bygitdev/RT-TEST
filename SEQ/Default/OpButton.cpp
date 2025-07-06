#include "..\DEF\Includes.h"



/////////////////////////////////////////////////////////////////////
COpBtn g_opBtn;
/////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------

void COpBtn::Run(void)
{
	//Button Lamp
	BOOL bLampOnOff = g_lampBuzzer.GetBlink();

	if(g_opr.isAuto)
	{
		g_dOut.On(oBtnLampStartF);
	}
	else
	{
		if(g_opr.isCycleRun)
			g_dOut.Set(oBtnLampStartF, bLampOnOff);
		else
			g_dOut.Off(oBtnLampStartF);
	}

	g_dOut.Set(oBtnLampStopF, !g_opr.isAuto);

    if(0 < g_err.GetNo())
		g_dOut.Set(oBtnLampResetF, bLampOnOff);
    else
		g_dOut.Set(oBtnLampResetF, FALSE);

	g_dOut.Set(oBtnLampStartR, g_dOut.IsOn(oBtnLampStartF));
	g_dOut.Set(oBtnLampStopR, g_dOut.IsOn(oBtnLampStopF));
	g_dOut.Set(oBtnLampResetR, g_dOut.IsOn(oBtnLampResetF));

	g_dOut.Set(oMzInSwLamp, g_pNV->NDm(ldMzInSwOn));
	g_dOut.Set(oMzOutSwLamp, g_pNV->NDm(ldMzOutSwOn));

	Start(m_fsm[swSTART]);
	Stop(m_fsm[swSTOP]);
	Reset(m_fsm[swRESET]);

	MzLoadSw(m_fsm[swMzLoad]);
	MzUnLoadSw(m_fsm[swMzUnLoad]);
}


//-------------------------------------------------------------------
void COpBtn::Stop(CFSM& fsm)
{
	switch(fsm.Get())
	{
	case C_BUTTON_START:
		if(g_dIn.AOn(iBtnStopF) || g_dIn.AOn(iBtnStopR))
		{
			g_pNV->NDm(tenKeyJog) = 0;

			// 도어락 해제
			if(g_opr.isStop && !g_allHome.m_fsm.IsRun() && !g_opr.isCycleRun)
			{
				g_dOut.On(oDoorLock01);
				g_dOut.On(oDoorLock02);
				g_dOut.On(oDoorLock03);
				g_dOut.On(oDoorLock04);
				g_dOut.On(oDoorLock05);
				g_dOut.On(oDoorLock06);
				g_dOut.On(oDoorLock07);
				g_dOut.On(oDoorLock08);
//				g_dOut.On(oDoorLock09);
			}

			// Spindle Stop
			if(!g_routerF.m_fsm.IsRun())
			{
				g_routerF.m_pSpindleF->Actuate(pmOFF);
				g_routerF.m_pSpindleR->Actuate(pmOFF);
				g_routerF.m_pSolRouterIonizerF->Actuate(pmOFF);
				g_routerF.m_pSolRouterIonizerR->Actuate(pmOFF);
				g_routerF.m_pSolSpindleBlow->Actuate(pmOFF);
				if(TOP_BLOW)	
					g_pm[SOL_INDEX_STAGE_AIR_BLOW_0102].Actuate(pmOFF);
			}

			if(!g_routerR.m_fsm.IsRun())
			{
				g_routerR.m_pSpindleF->Actuate(pmOFF);
				g_routerR.m_pSpindleR->Actuate(pmOFF);
				g_routerR.m_pSolRouterIonizerF->Actuate(pmOFF);
				g_routerR.m_pSolRouterIonizerR->Actuate(pmOFF);
				g_routerR.m_pSolSpindleBlow->Actuate(pmOFF);
				if(TOP_BLOW)	
					g_pm[SOL_INDEX_STAGE_AIR_BLOW_0304].Actuate(pmOFF);
			}

			if(!g_dOut.IsOn(oPwrIonizer01))
				g_dOut.On(oPwrIonizer01, TRUE);
			if(!g_dOut.IsOn(oPwrIonizer02))
				g_dOut.On(oPwrIonizer02, TRUE);
			if(!g_dOut.IsOn(oPwrIonizer03))
				g_dOut.On(oPwrIonizer03, TRUE);
			if(!g_dOut.IsOn(oPwrIonizer04))
				g_dOut.On(oPwrIonizer04, TRUE);

			// CYCLE 정지
			g_tenkeyOpr.m_fsm.Set(C_IDLE);

			//g_ldmz.m_fsm.Set(C_IDLE);
			
			if (FALSE == g_bUserStop)
			{
				g_bUserStop = TRUE;
				NEGRETE_WRITE(g_TpBase.logAlarm(L"USER", L"EQP_STOP", L"US_001", L"OCCURRED", L"'DESCRIPTION'", L"'USER_STOP'"))
			}

			g_opr.isStop = TRUE;
			g_allHome.Cancel();
			fsm.Set(C_BUTTON_TIMER);
		}
		break;

	case C_BUTTON_TIMER:
		if(!g_dIn.AOn(iBtnStopF) && !g_dIn.AOn(iBtnStopR))
		{
			fsm.Set(C_BUTTON_END);
		}
		else
		{
			if(fsm.Delay(5000))
			{
				fsm.Set(C_BUTTON_END);
			}
		}
		break;

	case C_BUTTON_END:
		if(!fsm.Delay(30))
			break;
		if(!g_dIn.AOn(iBtnStopF) && !g_dIn.AOn(iBtnStopR))
			fsm.Set(C_BUTTON_START);
		break;

	default:
		fsm.Set(C_BUTTON_START);
		break;
	}
}


//-------------------------------------------------------------------
void COpBtn::Start(CFSM& fsm)
{
	switch(fsm.Get())
	{
	case C_BUTTON_START:
		if((g_dIn.AOn(iBtnStartF) && !g_dIn.AOn(iBtnStopF) && !g_dIn.AOn(iBtnResetF)) || 
		   (g_dIn.AOn(iBtnStartR) && !g_dIn.AOn(iBtnStopR) && !g_dIn.AOn(iBtnResetR)))
		{
			g_pNV->NDm(tenKeyJog) = 0;
			if(0 < g_err.GetNo())
				break;

			if(g_opr.isDoorOpen)
			{
				g_err.Door(FALSE);
				break;
			}

			if(!g_pNV->NDm(mmiBtnAdcMode))
			{
				BOOL bKitTypeNotReady  = g_pNV->UseSkip(usIndex01) && (g_pNV->NDm(adcIndex01StageJobType) != g_pNV->Pkg(adcKitJobType));
				     bKitTypeNotReady |= g_pNV->UseSkip(usIndex02) && (g_pNV->NDm(adcIndex02StageJobType) != g_pNV->Pkg(adcKitJobType));
				     bKitTypeNotReady |= g_pNV->UseSkip(usIndex03) && (g_pNV->NDm(adcIndex03StageJobType) != g_pNV->Pkg(adcKitJobType));
				     bKitTypeNotReady |= g_pNV->UseSkip(usIndex04) && (g_pNV->NDm(adcIndex04StageJobType) != g_pNV->Pkg(adcKitJobType));
				     bKitTypeNotReady |= g_pNV->UseSkip(usIndex01) && (g_pNV->NDm(adcIndex01MaskJobType) != g_pNV->Pkg(adcKitJobType));
				     bKitTypeNotReady |= g_pNV->UseSkip(usIndex02) && (g_pNV->NDm(adcIndex02MaskJobType) != g_pNV->Pkg(adcKitJobType));
				     bKitTypeNotReady |= g_pNV->UseSkip(usIndex03) && (g_pNV->NDm(adcIndex03MaskJobType) != g_pNV->Pkg(adcKitJobType));
				     bKitTypeNotReady |= g_pNV->UseSkip(usIndex04) && (g_pNV->NDm(adcIndex04MaskJobType) != g_pNV->Pkg(adcKitJobType));
				     bKitTypeNotReady |= (g_pNV->NDm(adcOutPnpJobType) != g_pNV->Pkg(adcKitJobType));

				if(bKitTypeNotReady)
				{
					g_err.Save(ER_INDEX_OR_PICKER_KIT_TYPE_DIFFERENT);
					break;
				}
			}

			if(!g_opr.isAllHome || !g_opr.isStop || g_opr.isPausedStop)
				break;

			if(!g_dIn.AOn(iViAutoRun))
			{
				g_err.Save(ER_VI_NOT_READY);
				break;
			}

			if (TRUE == g_bEquipStop)
			{
				g_bEquipStop = FALSE;
				NEGRETE_WRITE(g_TpBase.logAlarm(L"EQUIPMENT", L"EQP_STOP", L"EQUIPMENT", L"RELEASED", L"'DESCRIPTION'", L"'ERROR_STOP'"))
			}
			if (TRUE == g_bUserStop)
			{
				g_bUserStop = FALSE;
				NEGRETE_WRITE(g_TpBase.logAlarm(L"USER", L"EQP_STOP", L"US_001", L"RELEASED", L"'DESCRIPTION'", L"'USER_STOP'"))
			}

			if(TOP_BLOW)
			{
				// Air Blow는 이물질 날림 현상으로 동작하지 않는다.
				// 확인 후 불필요 기능이면 삭제
				//g_pm[SOL_INDEX_STAGE_AIR_BLOW_0102].Actuate(pmON);
				//g_pm[SOL_INDEX_STAGE_AIR_BLOW_0304].Actuate(pmON);
			}

			m_tmStart.Reset();
			g_opr.isStop = FALSE;
			fsm.Set(C_BUTTON_END);
		}
		break;

	case C_BUTTON_END:
		if(!fsm.Delay(50))
			break;
		if((!g_dIn.AOn(iBtnStartF) && !g_dIn.AOn(iBtnStopF) && !g_dIn.AOn(iBtnResetF)) && 
		   (!g_dIn.AOn(iBtnStartR) && !g_dIn.AOn(iBtnStopR) && !g_dIn.AOn(iBtnResetR)))
			fsm.Set(C_BUTTON_START);
		break;

	default:
		fsm.Set(C_BUTTON_START);
		break;
	}
}


//-------------------------------------------------------------------
void COpBtn::Reset(CFSM& fsm)
{
	switch(fsm.Get())
	{
	case C_BUTTON_START:
		if(g_dIn.AOn(iBtnResetF) || g_dIn.AOn(iBtnResetR))
		{
			g_pNV->NDm(tenKeyJog) = 0;
			g_lampBuzzer.BuzzerOff();
			g_wr.Clear();
			g_dOut.Off(oSorterError); // Error 처리

			for(int mtno = 0; mtno < MAX_MT_NO; mtno++)
			{
				if(TRUE == g_mt[mtno].m_state.isAlarm)
				{
					g_mt[mtno].ServoOff();
					g_mt[mtno].AlarmClear();
				}
				else if(!g_mt[mtno].m_state.isServoOn)
				{
					g_mt[mtno].ServoOn();
				}
				else if(TRUE == g_mt[mtno].m_state.isPaused)
				{
					if(FALSE == g_opr.isDoorOpen)
					{
						g_mt[mtno].Resume();
					}
				}
			}

			if(!g_dOut.IsOn(oPwrIonizer01))
				g_dOut.On(oPwrIonizer01, TRUE);
			if(!g_dOut.IsOn(oPwrIonizer02))
				g_dOut.On(oPwrIonizer02, TRUE);
			if(!g_dOut.IsOn(oPwrIonizer03))
				g_dOut.On(oPwrIonizer03, TRUE);
			if(!g_dOut.IsOn(oPwrIonizer04))
				g_dOut.On(oPwrIonizer04, TRUE);

			if(0 == g_err.GetNo())
				break;

			g_err.Clear();
			g_err.Run();

			fsm.Set(C_BUTTON_END);
		}
		break;

	case C_BUTTON_END:
		if(!fsm.Delay(50))
			break;
		if(!g_dIn.AOn(iBtnResetF) && !g_dIn.AOn(iBtnResetR))
			fsm.Set(C_BUTTON_START);
		break;

	default:
		fsm.Set(C_BUTTON_START);
		break;
	}
}


//-------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
void COpBtn::MzLoadSw(CFSM& fsm)
{
	switch(fsm.Get())
	{
	case C_BUTTON_START:

		// In,Out Flag
		m_AutoConFlag = 0;

		if(g_dIn.AOn(iMzLoadSW))
		{
			m_AutoConFlag++;
		}

		if(g_dIn.AOn(iMzUnLoadSW)) 
		{
			m_AutoConFlag++;
		}

		if (m_AutoConFlag > 1)
		{
			if(g_dIn.AOn(iMzLoadZExist) && !g_dIn.AOn(iMzLoadZMid) && !g_dIn.AOn(iMzLoadZArrival))
			{
				m_AutoConIn = 1;
				g_pNV->NDm(ldMzInSwOn) = 0;
				g_pNV->NDm(ldMzOutSwOn) = 0;
				g_dOut.Off(oMzInSwLamp);
				g_dOut.Off(oMzOutSwLamp);
			}
			if(g_dIn.AOn(iMzLoadZMid) && g_dIn.AOn(iMzLoadZArrival))
			{
				m_AutoConOut = 1;
				g_pNV->NDm(ldMzInSwOn) = 0;
				g_pNV->NDm(ldMzOutSwOn) = 0;
				g_dOut.Off(oMzInSwLamp);
				g_dOut.Off(oMzOutSwLamp);
			}
			break;
		}

		if(m_AutoConIn == 1)
		{
			if(pmUP!=g_MgzLoadZ.m_pCylStopperUD->GetPos(300))
				g_MgzLoadZ.m_pCylStopperUD->Actuate(pmUP);

			g_dOut.On(oAcMgzLoadRun);
			m_AutoConIn = 2;
		}
		else if(m_AutoConIn == 2)
		{
			if(m_AutoConIn == 2 && g_dIn.AOn(iMzLoadZArrival)) 
			{
				if(!fsm.Delay(500))
					break;

				g_dOut.Off(oAcMgzLoadRun);
				m_AutoConIn = 0;
				fsm.Set(C_BUTTON_END);
				fsm.RstDelay();
			}
		}
		if(m_AutoConOut == 1)
		{
			g_dOut.On(oAcMgzLoadRun);
			g_dOut.On(oAcMgzLoadDir);
			m_AutoConOut = 2;
		}
		else if(m_AutoConOut == 2)
		{
			if(m_AutoConOut == 2 && !g_dIn.AOn(iMzLoadZExist) && !g_dIn.AOn(iMzLoadZMid) && !g_dIn.AOn(iMzLoadZArrival))
			{
				g_dOut.Off(oAcMgzLoadRun);
				g_dOut.Off(oAcMgzLoadDir);
				m_AutoConOut = 0;
				fsm.Set(C_BUTTON_END);
			}
		}

		if(g_dIn.AOn(iMzLoadSW) && !g_dIn.AOn(iMzUnLoadSW) && !g_dOut.IsOn(oMzOutSwLamp))
		{
			if(!g_MgzLoadZ.m_fsm.IsRun() && !g_pNV->NDm(ldMzOutSwOn))
			{
				if(!g_pNV->NDm(ldMzInSwOn) && g_dIn.AOn(iMzLoadZMid) && g_dIn.AOn(iMzLoadZArrival)) 
					g_pNV->NDm(ldMzInSwOn) = 1;
				else
					g_pNV->NDm(ldMzInSwOn) = 0;
			}
			fsm.Set(C_BUTTON_END);
		}
		break;

	case C_BUTTON_END:
		if(!fsm.Delay(50))
			break;
		if(!g_dIn.AOn(iMzLoadSW))
		{
			fsm.Set(C_BUTTON_START);
		}
		break;

	default:
		fsm.Set(C_BUTTON_START);
		break;
	}
}

//---------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
void COpBtn::MzUnLoadSw(CFSM& fsm)
{
	switch(fsm.Get())
	{
	case C_BUTTON_START:
		if(g_dIn.AOn(iMzUnLoadSW))
		{
			if(!g_ldMzOutConv.m_fsm.IsRun() && !g_dIn.AOn(iMzLoadSW))
			{
				if(!g_pNV->NDm(ldMzOutSwOn) && !g_dIn.AOn(iMzLoadZMid) && !g_dIn.AOn(iMzLoadZExist) && !g_dIn.AOn(iMzLoadZArrival)) 
					g_pNV->NDm(ldMzOutSwOn) = 1;
				else 
					g_pNV->NDm(ldMzOutSwOn) = 0;
			}
			fsm.Set(C_BUTTON_END);
		}
		break;

	case C_BUTTON_END:
		if(!fsm.Delay(50))
			break;
		if(!g_dIn.AOn(iMzUnLoadSW))
		{
			fsm.Set(C_BUTTON_START);
		}
		break;

	default:
		fsm.Set(C_BUTTON_START);
		break;
	}
}

