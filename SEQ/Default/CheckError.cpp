#include "..\def\Includes.h"

//////////////////////////////////////////////////////////////////////////
CCheckErr g_err;
CError	  g_wr;
//////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
void CCheckErr::Run(void)
{
	Door(TRUE); 
	Motor();
	Pneumatic();
	Etc();

	int nErrCode = GetNo();

	if(0 == nErrCode)
		return;

	if (FALSE == g_opr.isStop)
	{
		if (FALSE == g_bEquipStop)
		{
			g_bEquipStop = TRUE;
			NEGRETE_WRITE(g_TpBase.logAlarm(L"EQUIPMENT", L"EQP_STOP", L"EQUIPMENT", L"OCCURRED", L"'DESCRIPTION'", L"'ERROR_STOP'"))
		}
	}

	g_opr.isStop = TRUE;
}


//-------------------------------------------------------------------
void CCheckErr::Door(BOOL isNoErr)
{
	m_bLdSafetyBeam		= FALSE;
	m_bScrapSafetyBeam  = FALSE;

	// OHT mode에서는 SafetyLoader를 보지 않는다.
	// Door 만 확인 하고 AC Belt 구동부만 정지 후 구동	
	if(g_dIn.BOn(iSafetyLift) && !g_opr.isDryRun)
		m_bLdSafetyBeam = TRUE;
	if(g_dIn.BOn(iSafetyScrapBox) && !g_opr.isDryRun)
		m_bScrapSafetyBeam = TRUE;

	// AC Mtor 동작조건이 이상함 임시 막음 처리
	// Loader Safety Beam이 외부에 달려 있어 oht Mode에서는 Door 센서만 보도록 한다.
	if(m_bLdSafetyBeam)
	{
		g_wr.Save(WR_LOADER_SAFETYBEAM);

		if(m_tmLdSafety.TmOver(600 * 1000)) // 10분
		{
			g_err.Save(ER_SAFETY_BEAM_LD);
		}		
	}
	else
	{
		g_wr.Del(WR_LOADER_SAFETYBEAM);
		m_tmLdSafety.Reset();
	}

	if(m_bScrapSafetyBeam)
	{
		g_wr.Save(WR_SCRAP_BOX_SAFETYBEAM);

		DWORD ScrapTime = (DWORD)(g_pNV->DDm(ScarpBoxSafetyTime) * 1000.0);

		if(m_tmScrapSafety.TmOver(ScrapTime)) //  Alarm -> Error 변경요청으로, 타이머 파라미터 추가
		{
			g_err.Save(ER_SAFETY_BEAM_SCRAP_BOX);
		}
	}
	else
	{
		g_wr.Del(WR_SCRAP_BOX_SAFETYBEAM);
		m_tmScrapSafety.Reset();
	}
	
	if(g_pNV->NDm(setupmode))
	{
		g_opr.isDoorOpen = FALSE;
		g_opr.isDoorUnlock = FALSE;
	}
	else
	{
		g_opr.isDoorOpen = g_dIn.BOn(iDoorClose01) || g_dIn.BOn(iDoorClose02) || 
						   g_dIn.BOn(iDoorClose03) || g_dIn.BOn(iDoorClose04) || 
						   g_dIn.BOn(iDoorClose05) || g_dIn.BOn(iDoorClose06) || 
						   g_dIn.BOn(iDoorClose07) || g_dIn.BOn(iDoorClose08) || 
						   g_dIn.BOn(iDoorClose09) || g_dIn.BOn(iDoorClose10) ||
						   g_dIn.BOn(iDoorClose11) || g_dIn.BOn(iDoorClose12) || 
						   g_dIn.BOn(iDoorClose13) || g_dIn.BOn(iDoorClose14) || 
						   g_dIn.BOn(iDoorClose15) || 
						   g_dIn.BOn(iDoorClose16) || 
						   g_dIn.BOn(iDoorClose17) ||
						   g_dIn.BOn(iScrapBoxExist);
//		g_opr.isDoorOpen |= (g_dIn.BOn(iSafetyLift) || g_dIn.BOn(iSafetyScrapBox)); 
// 						   g_dIn.BOn(iDoorClose18) || 
// 						   g_dIn.BOn(iDoorClose19) || g_dIn.BOn(iDoorClose20) ||
// 						   g_dIn.BOn(iDoorClose21) || g_dIn.BOn(iDoorClose22);

		g_opr.isDoorUnlock = g_dIn.AOn(iDoorLock01) || g_dIn.AOn(iDoorLock02) || g_dIn.AOn(iDoorLock03) || 
							 g_dIn.AOn(iDoorLock04) || g_dIn.AOn(iDoorLock05) || g_dIn.AOn(iDoorLock06) ||
							 g_dIn.AOn(iDoorLock07) || g_dIn.AOn(iDoorLock08);// ||
//							 g_dIn.BOn(iDoorLock09); 
	}

	if(g_opr.isDoorOpen)
	{
		BOOL isPausedErr = FALSE;

		for(int nMtNo = 0; nMtNo < MAX_MT_NO; nMtNo++)
		{
			isPausedErr = !g_mt[nMtNo].m_state.isPaused;
			isPausedErr &= (!g_pNV->NDm(tenKeyJog));

			if(isPausedErr)
				g_mt[nMtNo].Paused();
		}
	}

	if(!g_opr.isStop || g_opr.isCycleRun || g_allHome.m_fsm.IsRun())
	{
		//if(m_tmDoorLock.TmOver(1000))
		//{
			if(!g_pNV->NDm(setupmode))
			{
				g_dOut.Off(oDoorLock01);
				g_dOut.Off(oDoorLock02);
				g_dOut.Off(oDoorLock03);
				g_dOut.Off(oDoorLock04);
				g_dOut.Off(oDoorLock05);
				g_dOut.Off(oDoorLock06);
				g_dOut.Off(oDoorLock07);
				g_dOut.Off(oDoorLock08);
				//g_dOut.Off(oDoorLock09);

				if(m_tmDoorLock.TmOver(1000))
				{
					if(g_dIn.AOn(iDoorLock01))
						g_err.Save(ER_DOOR_UNLOCK_01);
					else if(g_dIn.AOn(iDoorLock02))
						g_err.Save(ER_DOOR_UNLOCK_02);
					else if(g_dIn.AOn(iDoorLock03))
						g_err.Save(ER_DOOR_UNLOCK_03);
					else if(g_dIn.AOn(iDoorLock04))
						g_err.Save(ER_DOOR_UNLOCK_04);
					else if(g_dIn.AOn(iDoorLock05))
						g_err.Save(ER_DOOR_UNLOCK_05);
					else if(g_dIn.AOn(iDoorLock06))
						g_err.Save(ER_DOOR_UNLOCK_06);
					else if(g_dIn.AOn(iDoorLock07))
						g_err.Save(ER_DOOR_UNLOCK_07);
					else if(g_dIn.AOn(iDoorLock08))
						g_err.Save(ER_DOOR_UNLOCK_08);
					else
					{
						m_tmDoorLock.Reset();
					}
				}
			}
			//}
	}
	else
	{
		m_tmDoorLock.Reset();
	}

	BOOL shouldSaveErr = (!g_opr.isStop || g_opr.isCycleRun || g_allHome.m_fsm.IsRun());

	if(!shouldSaveErr && isNoErr)
		return;

	if(g_opr.isDoorOpen)
	{
		if(g_dIn.BOn(iDoorClose01))
			g_err.Save(ER_DOOR_OPEN_01);
		if(g_dIn.BOn(iDoorClose02))
			g_err.Save(ER_DOOR_OPEN_02);
		if(g_dIn.BOn(iDoorClose03))
			g_err.Save(ER_DOOR_OPEN_03);
		if(g_dIn.BOn(iDoorClose04))
			g_err.Save(ER_DOOR_OPEN_04);
		if(g_dIn.BOn(iDoorClose05))
			g_err.Save(ER_DOOR_OPEN_05);
		if(g_dIn.BOn(iDoorClose06))
			g_err.Save(ER_DOOR_OPEN_06);
		if(g_dIn.BOn(iDoorClose07))
			g_err.Save(ER_DOOR_OPEN_07);
		if(g_dIn.BOn(iDoorClose08))
			g_err.Save(ER_DOOR_OPEN_08);
		if(g_dIn.BOn(iDoorClose09))
			g_err.Save(ER_DOOR_OPEN_09);
		if(g_dIn.BOn(iDoorClose10))
			g_err.Save(ER_DOOR_OPEN_10);
		if(g_dIn.BOn(iDoorClose11))
			g_err.Save(ER_DOOR_OPEN_11);
		if(g_dIn.BOn(iDoorClose12))
			g_err.Save(ER_DOOR_OPEN_12);
		if(g_dIn.BOn(iDoorClose13))
			g_err.Save(ER_DOOR_OPEN_13);
		if(g_dIn.BOn(iDoorClose14))
			g_err.Save(ER_DOOR_OPEN_14);
		if(g_dIn.BOn(iDoorClose15))
			g_err.Save(ER_DOOR_OPEN_15);
		if(g_dIn.BOn(iDoorClose16))
			g_err.Save(ER_DOOR_OPEN_16); // Loader Door 개방
		if(g_dIn.BOn(iDoorClose17))
			g_err.Save(ER_DOOR_OPEN_17); 
		if(g_dIn.BOn(iScrapBoxExist)) 
			g_err.Save(ER_DOOR_OPEN_SCRAP_BOX); 
// 		if(g_dIn.BOn(iDoorClose18))
// 			g_err.Save(ER_DOOR_OPEN_18);
// 		if(g_dIn.BOn(iDoorClose19))
// 			g_err.Save(ER_DOOR_OPEN_19);
// 		if(g_dIn.BOn(iDoorClose20))
// 			g_err.Save(ER_DOOR_OPEN_20);
// 		if(g_dIn.BOn(iDoorClose21))
// 			g_err.Save(ER_DOOR_OPEN_21);
// 		if(g_dIn.BOn(iDoorClose22))
// 			g_err.Save(ER_DOOR_OPEN_22);
	}

	//if(!g_opr.isStop || g_opr.isCycleRun || g_allHome.m_fsm.IsRun())
	//{
	//	if(g_opr.isDoorUnlock)
	//	{
	//		if(g_dIn.AOn(iDoorLock01))
	//			g_err.Save(ER_DOOR_UNLOCK_01);
	//		if(g_dIn.AOn(iDoorLock02))
	//			g_err.Save(ER_DOOR_UNLOCK_02);
	//		if(g_dIn.AOn(iDoorLock03))
	//			g_err.Save(ER_DOOR_UNLOCK_03);
	//		if(g_dIn.AOn(iDoorLock04))
	//			g_err.Save(ER_DOOR_UNLOCK_04);
	//		if(g_dIn.AOn(iDoorLock05))
	//			g_err.Save(ER_DOOR_UNLOCK_05);
	//		if(g_dIn.AOn(iDoorLock06))
	//			g_err.Save(ER_DOOR_UNLOCK_06);
	//		if(g_dIn.AOn(iDoorLock07))
	//			g_err.Save(ER_DOOR_UNLOCK_07);
	//		if(g_dIn.AOn(iDoorLock08))
	//			g_err.Save(ER_DOOR_UNLOCK_08);
	//		 		if(g_dIn.BOn(iDoorLock09))
	//		 			g_err.Save(ER_DOOR_UNLOCK_09); // Loader Door 개방
	//	}
	//}
	//else
	//{
	//	m_tmDoorLock.Reset();
	//}
	
}


//-------------------------------------------------------------------
void CCheckErr::Motor(void)
{
	int nNo = 0;
	BOOL bBrokenAllHome = TRUE;

	g_opr.isPausedStop = FALSE;

	for(nNo = 0; nNo < MAX_MT_NO; nNo++)
	{
		if(!g_opr.isEmg)
		{
			if(g_mt[nNo].m_state.isAlarm)
				g_err.Save(ER_MT_ALARM + (nNo + 1));

			if(!g_mt[nNo].m_state.isServoOn)
				g_err.Save(ER_MT_SERVO_OFF + (nNo + 1));
		}

		if(!g_mt[nNo].m_state.isHome && g_opr.isAllHome)
		{
			g_err.Save(ER_MT_HOME + (nNo + 1));
		}

		if(g_mt[nNo].m_state.isHome)
		{
			if(g_mt[nNo].m_state.isCw || g_mt[nNo].m_state.isCCw)
			{
				g_mt[nNo].CancelHomeSearch();
				g_err.Save(ER_MT_LIMIT + (nNo + 1));
			}
		}

		bBrokenAllHome &= !g_mt[nNo].m_state.isHome;
		g_opr.isPausedStop |= g_mt[nNo].m_state.isPaused;
	}

	if(g_opr.isPausedStop)
		g_wr.Save(WR_PAUSED_STOP);
	else
		g_wr.Del(WR_PAUSED_STOP);

	if(bBrokenAllHome)
		g_opr.isAllHome = FALSE;

	if(!g_opr.isAllHome)
		g_err.Save(ER_ALL_HOME);
}


//-------------------------------------------------------------------
void CCheckErr::Pneumatic(void)
{
	for(int nNo = 0; nNo < MAX_PM_NO; nNo++)
	{
		Save(g_pm[nNo].GetErr());
	}
}


//-------------------------------------------------------------------
void CCheckErr::Etc(void)
{
	// Dry-Run
	if(g_opr.isDryRun)
		g_wr.Save(WR_DRY_RUN);

	if(g_opr.isAuto)
	{
		if(!g_dIn.AOn(iSorterAutoRun))
			g_wr.Save(WR_SORTER_AUTO_RUN);
		else
			g_wr.Del(WR_SORTER_AUTO_RUN);
	}
	else
	{
		g_wr.Del(WR_SORTER_AUTO_RUN);
	}

	if(g_opr.isAuto)
	{
		if(!g_dIn.AOn(iViAutoRun))
			g_err.Save(ER_VI_NOT_READY);
	}

	// Main Air
	if(!g_dIn.AOn(iMainAir))
		g_err.Save(ER_MAIN_AIR);

	// Fire Motoring
	if(g_dIn.BOn(iFireMonitoring))
		g_err.Save(ER_FIRE_MONITORING);

	//device change
	BOOL bDeviceChange  = (g_pNV->NDm(jobNo) != nOldJobNo);
		 bDeviceChange |= (g_pNV->NDm(groupNo) != nOldGroupNo);
		 bDeviceChange &= !g_ldMz.m_fsm.Between(CLdMz::C_AUTO_RECIPE_CHG_START, CLdMz::C_AUTO_RECIPE_CHG_END);

	if(bDeviceChange)
	{
		nOldJobNo = g_pNV->NDm(jobNo);
		nOldGroupNo = g_pNV->NDm(groupNo);

		for(int mtNo = 0; mtNo < MAX_MT_NO; mtNo++)
		{
			g_mt[mtNo].CancelHomeSearch();
		}

		g_opr.isAllHome = FALSE;

		copy2Mtd();
		_sprintf(g_cRecipeId, L"RCP_%04d", (int)((g_pNV->NDm(groupNo) * 1000) + g_pNV->NDm(jobNo)));
	}
}



//-------------------------------------------------------------------
// 25계통 안전 확인.... 조그 또는 절대 상대 위치 동작시 예외처리
BOOL CCheckErr::ChkMtSafety(int nMt)
{
	if(MT_INPNP_Y == nMt)
	{
		if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY)) 
		{
			g_err.Save(ER_IN_PNP_Z_NOT_READY_POS);
			return (FALSE);
		}
	}
	else if(MT_INDEX_X_01 == nMt || MT_INDEX_X_02 == nMt || MT_INDEX_X_03 == nMt || MT_INDEX_X_04 == nMt)
	{
		if(!g_mt[MT_OUTPNP_Z].IsRdy(COutPnp::PZ_READY))
		{
			g_err.Save(ER_MT_OUTPNP_Z_NOT_READY_POS);
			return (FALSE);
		}
	}

	//// 매거진 로딩 상태에서 클램프 오픈 후 이동하지 말것
	//if(g_DI.AOn(iMzLoading1) || g_DI.AOn(iMzLoading2) || g_InMZ.Exist())
	//{
	//	if(g_DO.IsOn(oMzClampUp))
	//	{
	//		if((mtInMzX == nMt) || (mtInPickerZ == nMt))
	//		{
	//			g_wr.Save(WR_CLAMP_OPEN);
	//			return (FALSE);
	//		}
	//	}
	//}

	//// Strip Pusher 전진 상태에서 이동하지 말것
	//if(g_DI.AOn(iStripPusherFwd))
	//{
	//	if((mtInMzX == nMt) || (mtInPickerZ == nMt))
	//	{
	//		g_wr.Save(WR_STRIP_PUSHER_FWD);
	//		return (FALSE);
	//	}
	//}

	/*
	if(MT_INDEX_X_01 == nMt) // 0 
	{
		if(CIndex::PX_IN_PNP == nIndexNo)
		{
		}
	}
	else if(MT_INDEX_X_02 == nMt) // 1
	{

	}
	else if(MT_ROUTER_Y_01 == nMt) // 2
	{

	}
	else if(MT_ROUTER_W_01 == nMt) // 3
	{

	}
	else if(MT_INDEX_X_03 == nMt) // 4
	{

	}
	else if(MT_INDEX_X_04 == nMt) // 5
	{

	}
	else if(MT_ROUTER_Y_02 == nMt) // 6
	{

	}
	else if(MT_ROUTER_W_02 == nMt) // 7
	{

	}
	else if(MT_LD_Y == nMt) // 8
	{

	}
	else if(MT_LD_Z == nMt) // 9
	{

	}
	else if(MT_PUSHER_X == nMt) // 10
	{

	}
	else if(MT_RAIL_GRIPPER_X == nMt) //11
	{

	}
	else if(MT_INPNP_Y == nMt) //12
	{

	}
	else if(MT_INPNP_Z == nMt) //13
	{

	}
	else if(MT_INPNP_CLAMP_Y == nMt) //14
	{

	}
	else if(MT_INDEX_T_01 == nMt) //15 
	{

	}
	else if(MT_INDEX_T_02 == nMt) //16
	{

	}
	else if(MT_INDEX_T_03 == nMt) //17
	{

	}
	else if(MT_INDEX_T_04 == nMt) //18
	{

	}
	else if(MT_SPINDLE_Z_01 == nMt) //19
	{

	}
	else if(MT_SPINDLE_Z_02 == nMt) //20
	{

	}
	else if(MT_SPINDLE_Z_03 == nMt) //21
	{

	}
	else if(MT_SPINDLE_Z_04 == nMt) //22
	{

	}
	else if(MT_OUTPNP_Y == nMt) //23
	{

	}
	else if(MT_OUTPNP_Z == nMt) //24
	{

	}
	else if(MT_OUTPNP_X == nMt) //25
	{

	}
	else if(MT_ADC_Z == nMt) //26
	{

	}
	else if(MT_ADC_X == nMt) //27
	{

	}
	*/


	return (TRUE);
}


BOOL CCheckErr::ChkMtIndexMove(int nMtNo, int nIndexNo)
{
	BOOL bReady = TRUE;
	
	if(!g_mt[nMtNo].IsRdy())
	{
		g_wr.Save(WR_NOT_READY_MT_INDEX_X_01 + nMtNo);
		return (FALSE);
	}
	else if(MT_INDEX_X_01 == nMtNo || MT_INDEX_X_02 == nMtNo) // 0, 1
	{
		if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY))
		{
			g_wr.Save(WR_MT_INPNP_Z_NOT_READY_POS);
			return (FALSE);
		}
		else if(!g_mt[MT_SPINDLE_Z_01].IsRdy(CRouter::PZ_READY))
		{
			g_wr.Save(WR_MT_SPINDLE_Z_01_NOT_READY_POS);
			return (FALSE);
		}
		else if(!g_mt[MT_SPINDLE_Z_02].IsRdy(CRouter::PZ_READY))
		{
			g_wr.Save(WR_MT_SPINDLE_Z_02_NOT_READY_POS);
			return (FALSE);
		}
		else if(!g_mt[MT_OUTPNP_Z].IsRdy(COutPnp::PZ_READY))
		{
			g_wr.Save(WR_MT_OUTPNP_Z_NOT_READY_POS);
			return (FALSE);
		}
		else if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].GetPos(100))
		{
			g_wr.Save(WR_CYL_ROUTER_BIT_CHANGE_CLAMP_01_NOT_UP);
			return (FALSE);
		}

		if(MT_INDEX_X_01 == nMtNo)
		{
			if(pmUP!=g_pm[CYL_MASK_KIT_PICKER_UD_01].GetPos(300))
			{
				g_wr.Save(WR_CYL_MASK_PICKER_01_NOT_UP);
				return(FALSE);
			}
		}
		else if(MT_INDEX_X_02 == nMtNo)
		{
			if(pmUP!=g_pm[CYL_MASK_KIT_PICKER_UD_02].GetPos(300))
			{
				g_wr.Save(WR_CYL_MASK_PICKER_02_NOT_UP);
				return(FALSE);
			}
		}
	}
	else if(MT_ROUTER_Y_01 == nMtNo) // 2 
	{
		if(!g_mt[MT_SPINDLE_Z_01].IsRdy(CRouter::PZ_READY))
		{
			g_wr.Save(WR_MT_SPINDLE_Z_01_NOT_READY_POS);
			return (FALSE);
		}
		else if(!g_mt[MT_SPINDLE_Z_02].IsRdy(CRouter::PZ_READY))
		{
			g_wr.Save(WR_MT_SPINDLE_Z_02_NOT_READY_POS);
			return (FALSE);
		}
		else if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].GetPos(100))
		{
			g_wr.Save(WR_CYL_ROUTER_BIT_CHANGE_CLAMP_01_NOT_UP);
			return (FALSE);
		}
		else if(!g_mt[MT_ROUTER_Y_02].IsRdy())
		{
			g_wr.Save(WR_NOT_READY_MT_ROUTER_Y_02);
			return (FALSE);
		}
		else if(CRouter::PY_SPD_WIRE_CHECK_F == nIndexNo || CRouter::PY_SPD_BIT_VI_F == nIndexNo ||
		        CRouter::PY_SPD_WIRE_CHECK_R == nIndexNo || CRouter::PY_SPD_BIT_VI_R == nIndexNo)
		{
			if(!g_mt[MT_ROUTER_Y_02].IsRdy(CRouter::PY_READY))
			{
				g_wr.Save(WR_MT_ROUTER_Y_02_NOT_READY_POS);
				return (FALSE);
			}
		}
	}
	else if(MT_ROUTER_W_01 == nMtNo) // 3
	{
		if(!g_mt[MT_SPINDLE_Z_02].IsRdy(CRouter::PZ_READY))
		{
			g_wr.Save(WR_MT_SPINDLE_Z_02_NOT_READY_POS);
			return (FALSE);
		}
	}
	else if(MT_INDEX_X_03 == nMtNo || MT_INDEX_X_04 == nMtNo) // 4, 5
	{
		if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY))
		{
			g_wr.Save(WR_MT_INPNP_Z_NOT_READY_POS);
			return (FALSE);
		}
		else if(!g_mt[MT_SPINDLE_Z_03].IsRdy(CRouter::PZ_READY))
		{
			g_wr.Save(WR_MT_SPINDLE_Z_03_NOT_READY_POS);
			return (FALSE);
		}
		else if(!g_mt[MT_SPINDLE_Z_04].IsRdy(CRouter::PZ_READY))
		{
			g_wr.Save(WR_MT_SPINDLE_Z_04_NOT_READY_POS);
			return (FALSE);
		}
		else if(!g_mt[MT_OUTPNP_Z].IsRdy(COutPnp::PZ_READY))
		{
			g_wr.Save(WR_MT_OUTPNP_Z_NOT_READY_POS);
			return (FALSE);
		}
		else if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].GetPos(100))
		{
			g_wr.Save(WR_CYL_ROUTER_BIT_CHANGE_CLAMP_02_NOT_UP);
			return (FALSE);
		}

		if(MT_INDEX_X_03 == nMtNo)
		{
			if(pmUP!=g_pm[CYL_MASK_KIT_PICKER_UD_03].GetPos(300))
			{
				g_wr.Save(WR_CYL_MASK_PICKER_03_NOT_UP);
				return(FALSE);
			}
		}
		else if(MT_INDEX_X_04 == nMtNo)
		{
			if(pmUP!=g_pm[CYL_MASK_KIT_PICKER_UD_04].GetPos(300))
			{
				g_wr.Save(WR_CYL_MASK_PICKER_04_NOT_UP);
				return(FALSE);
			}
		}
	}
	else if(MT_ROUTER_Y_02 == nMtNo) // 6
	{
		if(!g_mt[MT_SPINDLE_Z_03].IsRdy(CRouter::PZ_READY))
		{
			g_wr.Save(WR_MT_SPINDLE_Z_03_NOT_READY_POS);
			return (FALSE);
		}
		else if(!g_mt[MT_SPINDLE_Z_04].IsRdy(CRouter::PZ_READY))
		{
			g_wr.Save(WR_MT_SPINDLE_Z_04_NOT_READY_POS);
			return (FALSE);
		}
		else if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].GetPos(100))
		{
			g_wr.Save(WR_CYL_ROUTER_BIT_CHANGE_CLAMP_02_NOT_UP);
			return (FALSE);
		}
		else if(!g_mt[MT_ROUTER_Y_01].IsRdy())
		{
			g_wr.Save(WR_NOT_READY_MT_ROUTER_Y_01);
			return (FALSE);
		}
		else if(CRouter::PY_SPD_WIRE_CHECK_F == nIndexNo || CRouter::PY_SPD_BIT_VI_F == nIndexNo ||
		        CRouter::PY_SPD_WIRE_CHECK_R == nIndexNo || CRouter::PY_SPD_BIT_VI_R == nIndexNo)
		{
			if(!g_mt[MT_ROUTER_Y_01].IsRdy(CRouter::PY_READY))
			{
				g_wr.Save(WR_MT_ROUTER_Y_01_NOT_READY_POS);
				return (FALSE);
			}
		}
	}
	else if(MT_ROUTER_W_02 == nMtNo) // 7
	{
		if(!g_mt[MT_SPINDLE_Z_04].IsRdy(CRouter::PZ_READY))
		{
			g_wr.Save(WR_MT_SPINDLE_Z_04_NOT_READY_POS);
			return (FALSE);
		}
	}
	else if(MT_LD_Y == nMtNo) // 8
	{
		if(!g_mt[MT_PUSHER_X].IsRdy(CLdMz::PX_BWD) || !g_mt[MT_PUSHER_X].m_state.isOrg)
		{
			g_wr.Save(WR_MT_PUSHER_X_NOT_BWD_POS);
			return (FALSE);
		}
		else if(g_dIn.BOn(iMzClampPcbJam))
		{
			g_wr.Save(WR_LD_MZ_CLAMP_PCB_JAM_NOT_OFF);
			return (FALSE);
		}
		else if(g_dIn.AOn(iRailExistStart))
		{
			g_wr.Save(WR_RAIL_EXIST_START_NOT_OFF);
			return (FALSE);
		}
	}
	else if(MT_LD_Z == nMtNo) // 9
	{
		if(!g_mt[MT_LD_Y].IsRdy(CLdMz::PY_RAIL) && 
			!g_mt[MT_LD_Y].IsRdy(CLdMz::PY_RFID) && 
			!g_mt[MT_LD_Y].IsRdy(CLdMz::PY_ALIGN))
		{
			g_wr.Save(WR_MT_LD_Y_NOT_READY_POS);
			return (FALSE);
		}
		if(!g_mt[MT_PUSHER_X].IsRdy(CLdMz::PX_BWD) || !g_mt[MT_PUSHER_X].m_state.isOrg)
		{
			g_wr.Save(WR_MT_PUSHER_X_NOT_BWD_POS);
			return (FALSE);
		}
		else if(g_dIn.BOn(iMzClampPcbJam))
		{
			g_wr.Save(WR_LD_MZ_CLAMP_PCB_JAM_NOT_OFF);
			return (FALSE);
		}
		else if(g_dIn.AOn(iRailExistStart))
		{
			g_wr.Save(WR_RAIL_EXIST_START_NOT_OFF);
			return (FALSE);
		}
	}
	else if(MT_PUSHER_X == nMtNo) // 10
	{
	}
	else if(MT_RAIL_GRIPPER_X == nMtNo) //11
	{
		if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY))
		{
			g_wr.Save(WR_MT_INPNP_Z_NOT_READY_POS);
			return (FALSE);
		}
		else if(g_dIn.AOn(iRailExistStart))
		{
			g_wr.Save(WR_RAIL_EXIST_START_NOT_OFF);
			return (FALSE);
		}
		else if(g_dIn.AOn(iRailExistMid1))
		{
			g_wr.Save(WR_RAIL_EXIST_MID_NOT_OFF);
			return (FALSE);
		}
		else if(g_dIn.AOn(iRailExistMid2))
		{
			g_wr.Save(WR_RAIL_EXIST_MID2_NOT_OFF);
			return (FALSE);
		}
		else if(g_dIn.AOn(iRailExistEnd))
		{
			g_wr.Save(WR_RAIL_EXIST_END_NOT_OFF);
			return (FALSE);
		}
		else if(g_dIn.AOn(iRailGripperExist))
		{
			g_wr.Save(WR_RAIL_GRIPPER_EXIST_NOT_OFF);
			return (FALSE);
		}
		else if(g_pNV->NDm(existRail))
		{
			g_wr.Save(WR_RAIL_EXIST_MEMORY_NOT_OFF);
			return (FALSE);
		}
	}
	else if(MT_INPNP_Y == nMtNo) //12
	{
		if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY))
		{
			g_wr.Save(WR_MT_INPNP_Z_NOT_READY_POS);
			return (FALSE);
		}
	}
	else if(MT_INPNP_Z == nMtNo) //13
	{
	}
	else if(MT_INPNP_CLAMP_Y == nMtNo) //14
	{
	}
	else if(MT_INDEX_T_01 == nMtNo) //15 
	{
	}
	else if(MT_INDEX_T_02 == nMtNo) //16
	{
	}
	else if(MT_INDEX_T_03 == nMtNo) //17
	{
	}
	else if(MT_INDEX_T_04 == nMtNo) //18
	{
	}
	else if(MT_SPINDLE_Z_01 == nMtNo) //19
	{
	}
	else if(MT_SPINDLE_Z_02 == nMtNo) //20
	{
	}
	else if(MT_SPINDLE_Z_03 == nMtNo) //21
	{
	}
	else if(MT_SPINDLE_Z_04 == nMtNo) //22
	{
	}
	else if(MT_OUTPNP_Y == nMtNo) //23
	{
		if(!g_mt[MT_OUTPNP_Z].IsRdy(COutPnp::PZ_READY))
		{
			g_wr.Save(WR_MT_OUTPNP_Z_NOT_READY_POS);
			return (FALSE);
		}

		if (!g_mt[MT_OUTPNP_X].IsRdy(COutPnp::PZ_READY))
		{
			g_wr.Save(WR_MT_OUTPNP_X_NOT_READY_POS);
			return (FALSE);
		}
	}
	else if(MT_OUTPNP_Z == nMtNo) //24
	{
	}
	else if(MT_ADC_Z == nMtNo) //25
	{
	}
	else if(MT_ADC_X == nMtNo) //26
	{
	}
	else if(MT_OUTPNP_X == nMtNo) //27
	{
		if(!g_mt[MT_OUTPNP_Z].IsRdy(COutPnp::PZ_READY))
		{
			g_wr.Save(WR_MT_OUTPNP_Z_NOT_READY_POS);
			return (FALSE);
		}
	}

	return (TRUE);
}