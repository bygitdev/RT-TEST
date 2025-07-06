
#include "..\DEF\Includes.h"

//////////////////////////////////////////////////////////////////////////
CAllHome g_allHome;
//////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
// Home Run
void CAllHome::Run(void)
{
	g_pNV->NDm(mmiAllHomeComp) = g_opr.isAllHome;

	if(!m_fsm.IsRun())
		return;

	if(IsError())
	{
		Cancel();
		return;
	}

	if(m_fsm.TimeLimit(300000))
	{
		g_err.Save(ER_TIME_OVER_HOME_CYCLE);
		Cancel();
		return;
	}

	int nMsg = m_fsm.GetMsg();

	BOOL bComp = TRUE;

	if(nMsg & hmLoader)
		bComp &= PartLoader();
	if(nMsg & hmRail)
		bComp &= PartInRail();
	if(nMsg & hmInPnp)
		bComp &= PartInPnp();
	if(nMsg & hmIndex01)
		bComp &= PartIndex01();
	if(nMsg & hmIndex02)
		bComp &= PartIndex02();
	if(nMsg & hmIndex03)
		bComp &= PartIndex03();
	if(nMsg & hmIndex04)
		bComp &= PartIndex04();
	if(nMsg & hmRouterF)
		bComp &= PartRouterF();
	if(nMsg & hmRouterR)
		bComp &= PartRouterR();
	if(nMsg & hmOutPnp)
		bComp &= PartOutPnp();
	if(nMsg & hmADC)
		bComp &= PartADC();
	if(nMsg & hmMGZLoadZ)
		bComp &= PartMGZLoadZ();

	if(TRUE == bComp)
	{
		m_fsm.Set(C_IDLE);

		if(hmAllHome == nMsg)
		{
			g_opr.isAllHome = TRUE;
			g_err.Clear();
			g_wr.Clear();
		}
	}
}


//-------------------------------------------------------------------
// Home Start
void CAllHome::Start(int nMsg)
{
	if(m_fsm.IsRun())
	{
		int nNewMsg = (m_fsm.GetMsg() | nMsg);
		m_fsm.Set(100, nNewMsg);
	}
	else
	{
		m_fsm.Set(100, nMsg);
	}
}


//-------------------------------------------------------------------
// Home Cancel
void CAllHome::Cancel(void)
{
	int nMsg = m_fsm.GetMsg();
	int nMtNo = 0;

	for(nMtNo = 0; nMtNo < MAX_MT_NO; nMtNo++)
	{
		if(g_mt[nMtNo].m_state.isHoming)
			g_mt[nMtNo].CancelHomeSearch();
	}

	m_fsm.Set(C_IDLE);
}


//-------------------------------------------------------------------
// Err 발생 확인..
BOOL CAllHome::IsError(void)
{
	int nErrMsg = g_err.GetNo();
	if(1 <= nErrMsg && nErrMsg < ER_ALL_HOME)
	{
		return (TRUE);
	}

	// Device & Safety Check
	if(m_fsm.GetMsg() & hmLoader)
	{
		BOOL bErr = g_dIn.BOn(iMzClampPcbJam) && g_dIn.AOn(iRailExistStart) && pmBWD != g_pm[CYL_RAIL_GRIPPER_FB].GetPos(300);

		if(bErr)
		{
			if(g_dIn.BOn(iMzClampPcbJam))
				g_err.Save(ER_LD_MZ_CLAMP_PCB_JAM_NOT_OFF);
			if(g_dIn.AOn(iRailExistStart))
				g_err.Save(ER_RAIL_EXIST_START_NOT_OFF);
			if(pmBWD != g_pm[CYL_RAIL_GRIPPER_FB].GetPos(300))
				g_err.Save(ER_CYL_RAIL_GRIPPER_FB);

			return (TRUE);
		}
	}

	if(m_fsm.GetMsg() & hmRail)
	{
		BOOL bErr  = g_dIn.AOn(iRailExistStart) || g_dIn.AOn(iRailExistMid1) || g_dIn.AOn(iRailExistMid2);
			 //bErr |= g_dIn.AOn(iRailExistEnd); // End 센서 감지되어 All Home시에는 보지 않음
			 bErr |= g_dIn.AOn(iRailGripperExist);
			 bErr |= g_pNV->NDm(existRail);

		if(bErr)
		{
			if(g_dIn.AOn(iRailExistStart))
				g_err.Save(ER_RAIL_EXIST_START_NOT_OFF);
			if(g_dIn.AOn(iRailExistMid1))
				g_err.Save(ER_RAIL_EXIST_MID_NOT_OFF);
			if(g_dIn.AOn(iRailExistMid2))
				g_err.Save(ER_RAIL_EXIST_MID_NOT_OFF);
			//if(g_dIn.AOn(iRailExistEnd))
			//	g_err.Save(ER_RAIL_EXIST_END_NOT_OFF); // End 센서 감지되어 All Home시에는 보지 않음
			if(g_dIn.AOn(iRailGripperExist))
				g_err.Save(ER_RAIL_GRIPPER_EXIST_NOT_OFF);
			if(g_pNV->NDm(existRail))
				g_err.Save(ER_RAIL_EXIST_MEMORY_NOT_OFF);
			return (TRUE);
		}

		if(hmRail == m_fsm.GetMsg())
		{
			if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY)) 
			{
				g_err.Save(ER_IN_PNP_Z_NOT_READY_POS);
				return (TRUE);
			}
		}
	}

	if(m_fsm.GetMsg() & hmInPnp)
	{
		// 1회만 Check 하고 빠져나가도록 수정 (자재감지센서가 Station에 감지됨)
		if(m_bInPnpError)
		{
			m_bInPnpError = FALSE;

			BOOL bErr  = g_dIn.AOn(iInPnpExist01);
				 bErr |= g_dIn.AOn(iInPnpExist02);
				 bErr |= g_pNV->NDm(existInPnp);
				 bErr |= g_pNV->NDm(existInPnpClampKit);

			if(bErr)
			{
				if(g_dIn.AOn(iInPnpExist01))
					g_err.Save(ER_IN_PNP_EXIST_NOT_OFF);
				if(g_dIn.AOn(iInPnpExist02))
					g_err.Save(ER_IN_PNP_EXIST_NOT_OFF);
				if(g_pNV->NDm(existInPnp))
					g_err.Save(ER_IN_PNP_EXIST_MEMORY_NOT_OFF);
				if(g_pNV->NDm(existInPnpClampKit))
					g_err.Save(ER_ADC_IN_PNP_EXIST_MEMORY_NOT_OFF);

				return (TRUE);
			}
		}
	}

	if(m_fsm.GetMsg() & hmIndex01)
	{
		if(hmIndex01 == m_fsm.GetMsg())
		{
			if(!g_mt[MT_SPINDLE_Z_01].IsRdy(CRouter::PZ_READY)) 
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_01);
				return (TRUE);
			}
			if(!g_mt[MT_SPINDLE_Z_02].IsRdy(CRouter::PZ_READY)) 
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_02);
				return (TRUE);
			}
			if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].GetPos(300))
			{
				g_err.Save(ER_CYL_ROUTER_BIT_CHANGE_CLAMP_NOT_UP_01);
				return (TRUE);
			}
		}
	}

	if(m_fsm.GetMsg() & hmIndex02)
	{
		if(hmIndex02 == m_fsm.GetMsg())
		{
			if(!g_mt[MT_SPINDLE_Z_01].IsRdy(CRouter::PZ_READY)) 
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_01);
				return (TRUE);
			}
			if(!g_mt[MT_SPINDLE_Z_02].IsRdy(CRouter::PZ_READY)) 
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_02);
				return (TRUE);
			}
			if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].GetPos(300))
			{
				g_err.Save(ER_CYL_ROUTER_BIT_CHANGE_CLAMP_NOT_UP_01);
				return (TRUE);
			}
		}
	}

	if(m_fsm.GetMsg() & hmIndex03)
	{
		if(hmIndex03 == m_fsm.GetMsg())
		{
			if(!g_mt[MT_SPINDLE_Z_03].IsRdy(CRouter::PZ_READY)) 
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_03);
				return (TRUE);
			}
			if(!g_mt[MT_SPINDLE_Z_04].IsRdy(CRouter::PZ_READY)) 
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_04);
				return (TRUE);
			}
			if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].GetPos(300))
			{
				g_err.Save(ER_CYL_ROUTER_BIT_CHANGE_CLAMP_NOT_UP_02);
				return (TRUE);
			}
		}
	}

	if(m_fsm.GetMsg() & hmIndex04)
	{
		if(hmIndex04 == m_fsm.GetMsg())
		{
			if(!g_mt[MT_SPINDLE_Z_03].IsRdy(CRouter::PZ_READY)) 
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_03);
				return (TRUE);
			}
			if(!g_mt[MT_SPINDLE_Z_04].IsRdy(CRouter::PZ_READY)) 
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_04);
				return (TRUE);
			}
			if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].GetPos(300))
			{
				g_err.Save(ER_CYL_ROUTER_BIT_CHANGE_CLAMP_NOT_UP_02);
				return (TRUE);
			}
		}
	}

	if(m_fsm.GetMsg() & hmRouterF)
	{
	}

	if(m_fsm.GetMsg() & hmRouterR)
	{
	}

	if(m_fsm.GetMsg() & hmOutPnp)
	{
	}

	if (m_fsm.GetMsg() & hmADC)
	{
		BOOL bErr = g_dIn.AOn(iAdcKitJutExist);
	
		if(bErr)
		{
			g_err.Save(ER_ADC_KIT_JUT);
		}
	}
	if(m_fsm.GetMsg() & hmMGZLoadZ)
	{
	}


	return (FALSE);
}


/////////////////////////////////////////////////////////////////////
// Ld Mz Home
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartLoader(void)
{
	// MGZ Clamp는 동작하지 않음.

//	g_dOut.Off(oAcMgzInDir);
	g_dOut.Off(oAcMgzInRun);
//	g_dOut.Off(oAcMgzOutDir);
	g_dOut.Off(oAcMgzOutRun);

	g_pm[CYL_RAIL_GRIPPER_FB].Actuate(pmBWD); 
	if(pmBWD != g_pm[CYL_RAIL_GRIPPER_FB].GetPos(300))
		return (FALSE);

	g_pm[CYL_MGZ_CLAMP_ALIGN_FB].Actuate(pmBWD);
	if(pmBWD != g_pm[CYL_MGZ_CLAMP_ALIGN_FB].GetPos(300))
		return (FALSE);

	g_pm[CYL_MGZ_IN_STOPPER_UD_01].Actuate(pmDOWN);
	g_pm[CYL_MGZ_IN_STOPPER_UD_02].Actuate(pmDOWN);
	g_pm[CYL_MGZ_IN_STOPPER_UD_03].Actuate(pmDOWN);
//	g_pm[CYL_MGZ_IN_LOAD_STOPPER_UD].Actuate(pmDOWN);

	g_pm[CYL_MGZ_OUT_STOPPER_FB_01].Actuate(pmBWD);
	g_pm[CYL_MGZ_OUT_STOPPER_FB_02].Actuate(pmBWD);

	if(!g_mt[MT_PUSHER_X].m_state.isHome)
		g_mt[MT_PUSHER_X].StartHomeSearch();
	if(!g_mt[MT_PUSHER_X].IsRdy(g_mt[MT_PUSHER_X].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_LD_Y].m_state.isHome)
		g_mt[MT_LD_Y].StartHomeSearch();
	if(!g_mt[MT_LD_Y].IsRdy(g_mt[MT_LD_Y].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_LD_Z].m_state.isHome)
		g_mt[MT_LD_Z].StartHomeSearch();
	if(!g_mt[MT_LD_Z].IsRdy(g_mt[MT_LD_Z].m_config.homeIdx))
		return (FALSE);
	
	if(pmDOWN != g_pm[CYL_MGZ_IN_STOPPER_UD_01].GetPos(300))
		return (FALSE);
	if(pmDOWN != g_pm[CYL_MGZ_IN_STOPPER_UD_02].GetPos(300))
		return (FALSE);
	if(pmDOWN != g_pm[CYL_MGZ_IN_STOPPER_UD_03].GetPos(300))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// In Rail Home
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartInRail(void)
{
	if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY)) 
		return (FALSE);

	g_pm[CYL_RAIL_GRIPPER_OC].Actuate(pmOPEN);
	if(pmOPEN != g_pm[CYL_RAIL_GRIPPER_OC].GetPos(300))
		return (FALSE);

	g_pm[CYL_RAIL_GRIPPER_FB].Actuate(pmBWD); 
	if(pmBWD != g_pm[CYL_RAIL_GRIPPER_FB].GetPos(300))
		return (FALSE);

	if(!g_mt[MT_RAIL_GRIPPER_X].m_state.isHome)
		g_mt[MT_RAIL_GRIPPER_X].StartHomeSearch();
	if(!g_mt[MT_RAIL_GRIPPER_X].IsRdy(g_mt[MT_RAIL_GRIPPER_X].m_config.homeIdx))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// In Pnp Home
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartInPnp(void)
{
	if(!g_mt[MT_INPNP_Z].m_state.isHome)
		g_mt[MT_INPNP_Z].StartHomeSearch();
	if(!g_mt[MT_INPNP_Z].IsRdy(g_mt[MT_INPNP_Z].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_INPNP_Y].m_state.isHome)
		g_mt[MT_INPNP_Y].StartHomeSearch();
	if(!g_mt[MT_INPNP_CLAMP_Y].m_state.isHome)
		g_mt[MT_INPNP_CLAMP_Y].StartHomeSearch();
	if(!g_mt[MT_INPNP_Y].IsRdy(g_mt[MT_INPNP_Y].m_config.homeIdx))
		return (FALSE);
	if(!g_mt[MT_INPNP_CLAMP_Y].IsRdy(g_mt[MT_INPNP_CLAMP_Y].m_config.homeIdx))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// Index01 Home 
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartIndex01(void)
{
//	if(TOP_BLOW)	
//		g_dOut.Off(oSolIndexStageAirBlow0102);

	if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY)) 
		return (FALSE);
	if(!g_mt[MT_OUTPNP_Z].IsRdy(COutPnp::PZ_READY)) 
		return (FALSE);
	if(!g_mt[MT_SPINDLE_Z_01].IsRdy(CRouter::PZ_READY))
		return (FALSE);
	if(!g_mt[MT_SPINDLE_Z_02].IsRdy(CRouter::PZ_READY))
		return (FALSE);
	if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].GetPos(300))
		return (FALSE);

	if(pmUP != g_pm[CYL_MASK_KIT_PICKER_UD_01].GetPos(300))
	{
		g_pm[CYL_MASK_KIT_PICKER_UD_01].Actuate(pmUP);
		return (FALSE);
	}

	if(g_pNV->NDm(existKitMask01) || g_pNV->NDm(existKitMovePicker))
	{
		if(pmUP == g_pm[CYL_INDEX_MASK_KIT_FIX_UD_01].GetPos(300))
		{
			if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_01].GetPos(300) || 
			   pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_01].GetPos(300))
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_01].Actuate(pmFWD);
				g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_01].Actuate(pmFWD);
			}
			else
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_UD_01].Actuate(pmDOWN);
				return (FALSE);
			}
		}
		else if(pmDOWN == g_pm[CYL_INDEX_MASK_KIT_FIX_UD_01].GetPos(300))
		{
			if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_01].GetPos(300) || 
			   pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_01].GetPos(300))
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_UD_01].Actuate(pmUP);
				return (FALSE);
			}
		}		
		
		if(pmDOWN != g_pm[CYL_INDEX_MASK_KIT_FIX_UD_01].GetPos(300))
			return (FALSE);
		if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_01].GetPos(300))
			return (FALSE);
		if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_01].GetPos(300))
			return (FALSE);
	}
	else
	{
		if(pmUP != g_pm[CYL_INDEX_MASK_KIT_FIX_UD_01].GetPos(300))
		{
			g_pm[CYL_INDEX_MASK_KIT_FIX_UD_01].Actuate(pmUP);
			return (FALSE);
		}
		if(pmBWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_01].GetPos(300) || 
		   pmBWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_01].GetPos(300))
		{
			g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_01].Actuate(pmBWD);
			g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_01].Actuate(pmBWD);
			return (FALSE);
		}
	}


	if(g_pNV->NDm(existKitStage01))
	{
		if(pmCLOSE != g_pm[SOL_INDEX_STAGE_KIT_OC_01].GetPos(300))
		{
			g_pm[SOL_INDEX_STAGE_KIT_OC_01].Actuate(pmCLOSE);
			return (FALSE);
		}		
	}

	if(!g_mt[MT_INDEX_X_01].m_state.isHome)
		g_mt[MT_INDEX_X_01].StartHomeSearch();
	if(!g_mt[MT_INDEX_X_01].IsRdy(g_mt[MT_INDEX_X_01].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_INDEX_T_01].m_state.isHome)
		g_mt[MT_INDEX_T_01].StartHomeSearch();
	if(!g_mt[MT_INDEX_T_01].IsRdy(g_mt[MT_INDEX_T_01].m_config.homeIdx))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// Index02 Home 
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartIndex02(void)
{
//	if(TOP_BLOW)	
//		g_dOut.Off(oSolIndexStageAirBlow0102);

	if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY)) 
		return (FALSE);
	if(!g_mt[MT_OUTPNP_Z].IsRdy(COutPnp::PZ_READY)) 
		return (FALSE);
	if(!g_mt[MT_SPINDLE_Z_01].IsRdy(CRouter::PZ_READY))
		return (FALSE);
	if(!g_mt[MT_SPINDLE_Z_02].IsRdy(CRouter::PZ_READY))
		return (FALSE);
	if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].GetPos(300))
		return (FALSE);

	if(pmUP != g_pm[CYL_MASK_KIT_PICKER_UD_02].GetPos(300))
	{
		g_pm[CYL_MASK_KIT_PICKER_UD_02].Actuate(pmUP);
		return (FALSE);
	}

	if(g_pNV->NDm(existKitMask02))
	{
		if(pmUP == g_pm[CYL_INDEX_MASK_KIT_FIX_UD_02].GetPos(300))
		{
			if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_02].GetPos(300) || 
			   pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_02].GetPos(300))
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_02].Actuate(pmFWD);
				g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_02].Actuate(pmFWD);
			}
			else
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_UD_02].Actuate(pmDOWN);
				return (FALSE);
			}
		}
		else if(pmDOWN == g_pm[CYL_INDEX_MASK_KIT_FIX_UD_02].GetPos(300))
		{
			if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_02].GetPos(300) || 
			   pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_02].GetPos(300))
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_UD_02].Actuate(pmUP);
				return (FALSE);
			}
		}		
		
		if(pmDOWN != g_pm[CYL_INDEX_MASK_KIT_FIX_UD_02].GetPos(300))
			return (FALSE);
		if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_02].GetPos(300))
			return (FALSE);
		if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_02].GetPos(300))
			return (FALSE);
	}
	else
	{
		if(pmUP != g_pm[CYL_INDEX_MASK_KIT_FIX_UD_02].GetPos(300))
		{
			g_pm[CYL_INDEX_MASK_KIT_FIX_UD_02].Actuate(pmUP);
			return (FALSE);
		}
		if(pmBWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_02].GetPos(300) || 
		   pmBWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_02].GetPos(300))
		{
			g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_02].Actuate(pmBWD);
			g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_02].Actuate(pmBWD);
			return (FALSE);
		}
	}


	if(g_pNV->NDm(existKitStage02))
	{
		if(pmCLOSE != g_pm[SOL_INDEX_STAGE_KIT_OC_02].GetPos(300))
		{
			g_pm[SOL_INDEX_STAGE_KIT_OC_02].Actuate(pmCLOSE);
			return (FALSE);
		}		
	}

	if(!g_mt[MT_INDEX_X_02].m_state.isHome)
		g_mt[MT_INDEX_X_02].StartHomeSearch();
	if(!g_mt[MT_INDEX_X_02].IsRdy(g_mt[MT_INDEX_X_02].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_INDEX_T_02].m_state.isHome)
		g_mt[MT_INDEX_T_02].StartHomeSearch();
	if(!g_mt[MT_INDEX_T_02].IsRdy(g_mt[MT_INDEX_T_02].m_config.homeIdx))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// Index03 Home 
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartIndex03(void)
{
//	if(TOP_BLOW)	
//		g_dOut.Off(oSolIndexStageAirBlow0304);

	if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY)) 
		return (FALSE);
	if(!g_mt[MT_OUTPNP_Z].IsRdy(COutPnp::PZ_READY)) 
		return (FALSE);
	if(!g_mt[MT_SPINDLE_Z_03].IsRdy(CRouter::PZ_READY))
		return (FALSE);
	if(!g_mt[MT_SPINDLE_Z_04].IsRdy(CRouter::PZ_READY))
		return (FALSE);
	if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].GetPos(300))
		return (FALSE);

	if(pmUP != g_pm[CYL_MASK_KIT_PICKER_UD_03].GetPos(300))
	{
		g_pm[CYL_MASK_KIT_PICKER_UD_03].Actuate(pmUP);
		return (FALSE);
	}

	if(g_pNV->NDm(existKitMask03))
	{
		if(pmUP == g_pm[CYL_INDEX_MASK_KIT_FIX_UD_03].GetPos(300))
		{
			if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_03].GetPos(300) || 
			   pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_03].GetPos(300))
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_03].Actuate(pmFWD);
				g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_03].Actuate(pmFWD);
			}
			else
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_UD_03].Actuate(pmDOWN);
				return (FALSE);
			}
		}
		else if(pmDOWN == g_pm[CYL_INDEX_MASK_KIT_FIX_UD_03].GetPos(300))
		{
			if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_03].GetPos(300) || 
			   pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_03].GetPos(300))
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_UD_03].Actuate(pmUP);
				return (FALSE);
			}
		}		
		
		if(pmDOWN != g_pm[CYL_INDEX_MASK_KIT_FIX_UD_03].GetPos(300))
			return (FALSE);
		if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_03].GetPos(300))
			return (FALSE);
		if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_03].GetPos(300))
			return (FALSE);
	}
	else
	{
		if(pmUP != g_pm[CYL_INDEX_MASK_KIT_FIX_UD_03].GetPos(300))
		{
			g_pm[CYL_INDEX_MASK_KIT_FIX_UD_03].Actuate(pmUP);
			return (FALSE);
		}
		if(pmBWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_03].GetPos(300) || 
		   pmBWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_03].GetPos(300))
		{
			g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_03].Actuate(pmBWD);
			g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_03].Actuate(pmBWD);
			return (FALSE);
		}
	}


	if(g_pNV->NDm(existKitStage03))
	{
		if(pmCLOSE != g_pm[SOL_INDEX_STAGE_KIT_OC_03].GetPos(300))
		{
			g_pm[SOL_INDEX_STAGE_KIT_OC_03].Actuate(pmCLOSE);
			return (FALSE);
		}		
	}

	if(!g_mt[MT_INDEX_X_03].m_state.isHome)
		g_mt[MT_INDEX_X_03].StartHomeSearch();
	if(!g_mt[MT_INDEX_X_03].IsRdy(g_mt[MT_INDEX_X_03].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_INDEX_T_03].m_state.isHome)
		g_mt[MT_INDEX_T_03].StartHomeSearch();
	if(!g_mt[MT_INDEX_T_03].IsRdy(g_mt[MT_INDEX_T_03].m_config.homeIdx))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// Index04 Home 
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartIndex04(void)
{
//	if(TOP_BLOW)	
//		g_dOut.Off(oSolIndexStageAirBlow0304);

	if(!g_mt[MT_INPNP_Z].IsRdy(CInPnp::PZ_READY)) 
		return (FALSE);
	if(!g_mt[MT_OUTPNP_Z].IsRdy(COutPnp::PZ_READY)) 
		return (FALSE);
	if(!g_mt[MT_SPINDLE_Z_03].IsRdy(CRouter::PZ_READY))
		return (FALSE);
	if(!g_mt[MT_SPINDLE_Z_04].IsRdy(CRouter::PZ_READY))
		return (FALSE);
	if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].GetPos(300))
		return (FALSE);

	if(pmUP != g_pm[CYL_MASK_KIT_PICKER_UD_04].GetPos(300))
	{
		g_pm[CYL_MASK_KIT_PICKER_UD_04].Actuate(pmUP);
		return (FALSE);
	}

	if(g_pNV->NDm(existKitMask04))
	{
		if(pmUP == g_pm[CYL_INDEX_MASK_KIT_FIX_UD_04].GetPos(300))
		{
			if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_04].GetPos(300) || 
			   pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_04].GetPos(300))
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_04].Actuate(pmFWD);
				g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_04].Actuate(pmFWD);
			}
			else
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_UD_04].Actuate(pmDOWN);
				return (FALSE);
			}
		}
		else if(pmDOWN == g_pm[CYL_INDEX_MASK_KIT_FIX_UD_04].GetPos(300))
		{
			if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_04].GetPos(300) || 
			   pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_04].GetPos(300))
			{
				g_pm[CYL_INDEX_MASK_KIT_FIX_UD_04].Actuate(pmUP);
				return (FALSE);
			}
		}		
		
		if(pmDOWN != g_pm[CYL_INDEX_MASK_KIT_FIX_UD_04].GetPos(300))
			return (FALSE);
		if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_04].GetPos(300))
			return (FALSE);
		if(pmFWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_04].GetPos(300))
			return (FALSE);
	}
	else
	{
		if(pmUP != g_pm[CYL_INDEX_MASK_KIT_FIX_UD_04].GetPos(300))
		{
			g_pm[CYL_INDEX_MASK_KIT_FIX_UD_04].Actuate(pmUP);
			return (FALSE);
		}
		if(pmBWD != g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_04].GetPos(300) || 
		   pmBWD != g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_04].GetPos(300))
		{
			g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_04].Actuate(pmBWD);
			g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_04].Actuate(pmBWD);
			return (FALSE);
		}
	}


	if(g_pNV->NDm(existKitStage04))
	{
		if(pmCLOSE != g_pm[SOL_INDEX_STAGE_KIT_OC_04].GetPos(300))
		{
			g_pm[SOL_INDEX_STAGE_KIT_OC_04].Actuate(pmCLOSE);
			return (FALSE);
		}		
	}

	if(!g_mt[MT_INDEX_X_04].m_state.isHome)
		g_mt[MT_INDEX_X_04].StartHomeSearch();
	if(!g_mt[MT_INDEX_X_04].IsRdy(g_mt[MT_INDEX_X_04].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_INDEX_T_04].m_state.isHome)
		g_mt[MT_INDEX_T_04].StartHomeSearch();
	if(!g_mt[MT_INDEX_T_04].IsRdy(g_mt[MT_INDEX_T_04].m_config.homeIdx))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// Router Front Home 
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartRouterF(void)
{
	g_dOut.Off(oSolSpindleAirBlow0102);
	g_dOut.Off(oSolRouterIonizer01);
	g_dOut.Off(oSolRouterIonizer02);

	// Open / Close는 하지 않음
	if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].GetPos(300))
	{
		g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].Actuate(pmUP);
		return (FALSE);
	}

	if(!g_mt[MT_SPINDLE_Z_01].m_state.isHome)
		g_mt[MT_SPINDLE_Z_01].StartHomeSearch();
	if(!g_mt[MT_SPINDLE_Z_02].m_state.isHome)
		g_mt[MT_SPINDLE_Z_02].StartHomeSearch();
	if(!g_mt[MT_SPINDLE_Z_01].IsRdy(g_mt[MT_SPINDLE_Z_01].m_config.homeIdx))
		return (FALSE);
	if(!g_mt[MT_SPINDLE_Z_02].IsRdy(g_mt[MT_SPINDLE_Z_02].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_ROUTER_W_01].m_state.isHome)
		g_mt[MT_ROUTER_W_01].StartHomeSearch();
	if(!g_mt[MT_ROUTER_W_01].IsRdy(g_mt[MT_ROUTER_W_01].m_config.homeIdx))
		return (FALSE);
	if(!g_mt[MT_ROUTER_Y_01].m_state.isHome)
		g_mt[MT_ROUTER_Y_01].StartHomeSearch();
	if(!g_mt[MT_ROUTER_Y_01].IsRdy(g_mt[MT_ROUTER_Y_01].m_config.homeIdx))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// Router Rear Home 
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartRouterR(void)
{
	g_dOut.Off(oSolSpindleAirBlow0304);
	g_dOut.Off(oSolRouterIonizer03);
	g_dOut.Off(oSolRouterIonizer04);

	if(pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].GetPos(300))
	{
		g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].Actuate(pmUP);
		return (FALSE);
	}

	if(!g_mt[MT_SPINDLE_Z_03].m_state.isHome)
		g_mt[MT_SPINDLE_Z_03].StartHomeSearch();
	if(!g_mt[MT_SPINDLE_Z_04].m_state.isHome)
		g_mt[MT_SPINDLE_Z_04].StartHomeSearch();
	if(!g_mt[MT_SPINDLE_Z_03].IsRdy(g_mt[MT_SPINDLE_Z_03].m_config.homeIdx))
		return (FALSE);
	if(!g_mt[MT_SPINDLE_Z_04].IsRdy(g_mt[MT_SPINDLE_Z_04].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_ROUTER_W_02].m_state.isHome)
		g_mt[MT_ROUTER_W_02].StartHomeSearch();
	if(!g_mt[MT_ROUTER_W_02].IsRdy(g_mt[MT_ROUTER_W_02].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_ROUTER_Y_02].m_state.isHome)
		g_mt[MT_ROUTER_Y_02].StartHomeSearch();
	if(!g_mt[MT_ROUTER_Y_02].IsRdy(g_mt[MT_ROUTER_Y_02].m_config.homeIdx))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// Out Pnp Home
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartOutPnp(void)
{
	// Cylinder는 초기화 하지 않음
	// 현상태 유지

	if(!g_mt[MT_OUTPNP_Z].m_state.isHome)
		g_mt[MT_OUTPNP_Z].StartHomeSearch();
	if(!g_mt[MT_OUTPNP_Z].IsRdy(g_mt[MT_OUTPNP_Z].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_OUTPNP_X].m_state.isHome)
		g_mt[MT_OUTPNP_X].StartHomeSearch();
	if(!g_mt[MT_OUTPNP_X].IsRdy(g_mt[MT_OUTPNP_X].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_OUTPNP_Y].m_state.isHome)
		g_mt[MT_OUTPNP_Y].StartHomeSearch();
	if(!g_mt[MT_OUTPNP_Y].IsRdy(g_mt[MT_OUTPNP_Y].m_config.homeIdx))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// ADC Home
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartADC(void)
{
	if(!g_mt[MT_ADC_X].m_state.isHome)
		g_mt[MT_ADC_X].StartHomeSearch();
	if(!g_mt[MT_ADC_X].IsRdy(g_mt[MT_ADC_X].m_config.homeIdx))
		return (FALSE);

	if(!g_mt[MT_ADC_Z].m_state.isHome)
		g_mt[MT_ADC_Z].StartHomeSearch();
	if(!g_mt[MT_ADC_Z].IsRdy(g_mt[MT_ADC_Z].m_config.homeIdx))
		return (FALSE);

	return (TRUE);
}


/////////////////////////////////////////////////////////////////////
// MGZ Load Z Home
/////////////////////////////////////////////////////////////////////
BOOL CAllHome::PartMGZLoadZ(void)
{
	if(g_err.m_bLdSafetyBeam)
	{
		g_mt[MT_MGZ_LOAD_Z].CancelHomeSearch();
		return (FALSE);
	}

	if(!g_mt[MT_MGZ_LOAD_Z].m_state.isHome)
		g_mt[MT_MGZ_LOAD_Z].StartHomeSearch();
	if(!g_mt[MT_MGZ_LOAD_Z].IsRdy(g_mt[MT_MGZ_LOAD_Z].m_config.homeIdx))
		return (FALSE);
	g_pm[CYL_MGZ_IN_LOAD_STOPPER_UD].Actuate(pmUP);
	return (TRUE);
}