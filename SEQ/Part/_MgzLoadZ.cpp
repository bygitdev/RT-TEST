#include "..\DEF\Includes.h"

/////////////////////////////////////////////////////////////////////
CMgzLoadZ g_MgzLoadZ;
/////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
CMgzLoadZ::CMgzLoadZ()
{
	m_bRun		= FALSE;
	m_bRdyMzIn  = FALSE;
	m_bRdyMzOut  = FALSE;
}


//-------------------------------------------------------------------
void CMgzLoadZ::AutoRun(void)
{
	m_bRdyMzIn = FALSE;
	m_bRdyMzOut = FALSE;


	if(g_pNV->NDm(ldMzInSwOn) && g_dIn.AOn(iMzLoadZArrival))
		ExistLoadArrival() = TRUE;
	else
		ExistLoadArrival() = FALSE;

	if(g_pNV->NDm(ldMzInSwOn) && g_dIn.AOn(iMzLoadZExist))
		ExistLoadExist() = TRUE;

	if(!g_pNV->NDm(ldMzOutSwOn) && !g_dIn.AOn(iMzLoadZExist))
		ExistLoadExist() = FALSE;
//	else if
//		ExistLoadExist() = FALSE;

	int nLoadZArrivalExistErr  = GetLoadZArrivalErr();
	int nLoadZExisttErr  = GetLoadZExistErr();

	if( !m_pMtZ->IsRdy() ){
		if(g_err.m_bLdSafetyBeam || g_opr.isDoorOpen)
		{
			m_pMtZ->Paused();
			return;
		}
		else
		{
			if(m_pMtZ->m_state.isPaused)
			{
				m_pMtZ->Resume();
			}
		}
		return;
	}

	if(!m_bRun)
		return;

	m_bRun = FALSE;

	if(m_fsm.IsRun())
		return;

	if(g_err.m_bLdSafetyBeam)
		return;

	if(EXIST_UNCERTAIN == nLoadZArrivalExistErr)
		return;
	if(EXIST_UNCERTAIN == nLoadZExisttErr)
		return;

	if(EXIST_ERR == nLoadZArrivalExistErr)
	{
		g_pNV->NDm(ldMzInSwOn) = 0;
		g_err.Save(ER_MZ_LOADER_EXIST);
		return;
	}
	if(EXIST_ERR == nLoadZExisttErr)
	{
		g_pNV->NDm(ldMzInSwOn) = 0;
		g_err.Save(ER_MZ_LOADER_ARRIVAL);
		return;
	}


 	if(g_ldMzInConv.m_fsm.Between(CLdMzInConv::C_LOADER_TO_STOPPER_START, CLdMzInConv::C_END))
 		return;
//	if(g_ldMzOutConv.m_fsm.Between(CLdMzOutConv::C_START, CLdMzOutConv::C_MANUAL_END))
//		return;
	if(!g_pNV->NDm(ldMzInSwOn) && !g_pNV->NDm(ldMzOutSwOn) ) BeltRun(FALSE);

// 	m_pCylStopperUD->Actuate(pmUP);
// 	if(pmUP != m_pCylStopperUD->GetPos(300))
// 		return;

	if(!m_pMtZ->IsRdy())
		return;

	switch(GetState())
	{
		case S_IDLE:
			if(!m_pMtZ->IsRdy(PZ_DOWN_POS))
				m_pMtZ->Move(PZ_DOWN_POS);
			if(pmUP!=m_pCylStopperUD->GetPos(300))
			{
				m_pCylStopperUD->Actuate(pmUP);

				if(!g_logChk.bFunction[m_pCylStopperUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperUD->GetNo()] = TRUE; 
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"CYL_MGZ_LOAD_STOPPER_UP", g_data2c.cEtc.start, 
						L"$", L"MZ", g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
						g_data2c.cPmName[m_pCylStopperUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
						g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off,
						g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
				}
			}
			break;
		case S_LOADING:
			m_fsm.Set(C_LOADING_START);
			break;
		case S_LOAD_WAIT:
			m_bRdyMzIn=TRUE;
			break;
		case S_UNLOAD_WAIT:
			m_bRdyMzOut=TRUE;
			break;
		case S_UNLOADING:
			m_fsm.Set(C_UNLOADING_START);
			break;
	}
}


//-------------------------------------------------------------------
void CMgzLoadZ::CycleRun(void)
{
	if(!m_fsm.IsRun())
		return;

	if(m_fsm.IsStop())
		return;

	if(C_ERROR == m_fsm.Get())
	{
		if(0 < m_fsm.GetMsg())
			g_err.Save(m_fsm.GetMsg());

		BeltRun(FALSE);
		m_fsm.Set(C_IDLE);
		return;
	}
	else if(IsErr())
	{
		m_fsm.Set(C_ERROR);
		return;
	}

	CycleLoadingStep();
	CycleUnLoadingStep();
}


//-------------------------------------------------------------------
void CMgzLoadZ::Init(void)
{
	m_pMtZ = &g_mt[MT_MGZ_LOAD_Z];
	m_pCylStopperUD	= &g_pm[CYL_MGZ_IN_LOAD_STOPPER_UD];
}


//-------------------------------------------------------------------
int& CMgzLoadZ::ExistLoadExist(void)
{
	return (g_pNV->m_pData->ndm[existMzLoadZExist]);
}


//-------------------------------------------------------------------
int& CMgzLoadZ::ExistLoadArrival(void)
{
	return (g_pNV->m_pData->ndm[existMzLoadZArrival]);
}


//-------------------------------------------------------------------
BOOL CMgzLoadZ::IsReadyLoadIn(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyMzIn);
	}
	else
	{
		BOOL isRdy  = !g_dOut.IsOn(oAcMgzLoadRun);
			 isRdy &= (g_dIn.AOn(iMzLoadZArrival) && ExistLoadArrival()) || g_opr.isDryRun;
			 isRdy &= (m_pMtZ->IsRdy(PZ_LOAD_POS));
			 isRdy &= !m_fsm.IsRun();
			 
		return (isRdy);
	}
}


BOOL CMgzLoadZ::IsReadyLoadOut(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyMzOut);
	}
	else
	{
		BOOL isRdy = (m_pMtZ->IsRdy(PZ_UNLOAD_POS));
		isRdy &= !m_fsm.IsRun();

		return (isRdy);
	}
}

//-------------------------------------------------------------------
void CMgzLoadZ::BeltRun(BOOL Run, BOOL Ccw)
{
	if(Run)
	{
		g_dOut.On(oAcMgzLoadRun);
		if(Ccw) g_dOut.On(oAcMgzLoadDir);
		else g_dOut.Off(oAcMgzLoadDir);
	}
	else
	{
		bool m_AutoCon = false;
		if ((g_opBtn.m_AutoConIn > 0) || (g_opBtn.m_AutoConOut > 0)) 
			m_AutoCon = false;
		else
			m_AutoCon = true;

		if(m_AutoCon) 
		{
			g_dOut.Off(oAcMgzLoadRun);
			g_dOut.Off(oAcMgzLoadDir);
		}

	}
}


//-------------------------------------------------------------------
int  CMgzLoadZ::GetState(void)
{
	int nState = S_IDLE;

	if(g_pNV->NDm(ldMzInSwOn))
	{
		if(ExistLoadExist() || ExistLoadArrival())
		{
			if(m_pMtZ->IsRdy() && !m_pMtZ->InPos(PZ_LOAD_POS))
			{
				return (S_LOADING);
			}
			else
			{
				return (S_LOAD_WAIT);
			}
		}
		else
		{
			g_pNV->NDm(ldMzInSwOn)=0;
		}
	}
	else if(g_pNV->NDm(ldMzOutSwOn))
	{
		if(!ExistLoadExist() && !ExistLoadArrival())
		{
			if(m_pMtZ->IsRdy() && !m_pMtZ->InPos(PZ_UNLOAD_POS))
			{
				return (S_UNLOADING);
			}
			else
			{
				return (S_UNLOAD_WAIT);
			}
		}
		else
		{
			g_pNV->NDm(ldMzOutSwOn)=0;
		}
	}
	else
	{
		return (S_IDLE);
	}
	return (nState);
}


//-------------------------------------------------------------------
BOOL CMgzLoadZ::IsErr(void)
{
	if(0 < m_pCylStopperUD->GetErr())
		return (TRUE);
		
	return (FALSE);
}


//-------------------------------------------------------------------
int  CMgzLoadZ::GetLoadZArrivalErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	if(!g_pNV->NDm(ldMzInSwOn))
		return (EXIST_NORMAL);

	if(ExistLoadArrival() == (g_dIn.AOn(iMzLoadZArrival)))
	{
		m_tmLoadZArrivalErr.Reset();
	}
	else
	{
		if(m_tmLoadZArrivalErr.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
int  CMgzLoadZ::GetLoadZExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	if(!g_pNV->NDm(ldMzInSwOn))
		return (EXIST_NORMAL);

	if(ExistLoadExist() == (g_dIn.AOn(iMzLoadZExist)))
	{
		m_tmLoadZExistErr.Reset();
	}
	else
	{
		if(m_tmLoadZExistErr.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
void CMgzLoadZ::CycleLoadingStep(void)
{
	if(!m_fsm.Between(C_LOADING_START, C_LOADING_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_MZ_LOAD_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy())
	{
		if(g_err.m_bLdSafetyBeam || g_opr.isDoorOpen)
		{
			m_pMtZ->Paused();
			return;
		}
		else
		{
			if(m_pMtZ->m_state.isPaused)
			{
				m_pMtZ->Resume();
			}
		}
		return;
	}

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialId, L"$");
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_LOADING_START:			
		if(g_err.m_bLdSafetyBeam || g_opr.isDoorOpen)
		{
			BeltRun(FALSE);
			m_fsm.Set(C_LOADING_PAUSE_STOP);
			break;
		}

		if(m_fsm.Once())
		{
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cMzLift.deviceId, L"FULL_MGZ", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"MGZ_DOWN", L"MGZ_LOAD"))				
		}
		else
		{
			if(pmUP != m_pCylStopperUD->GetPos(300))
			{
				m_pCylStopperUD->Actuate(pmUP);

				if(!g_logChk.bFunction[m_pCylStopperUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperUD->GetNo()] = TRUE; 
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"CYL_MGZ_LOAD_STOPPER_UP", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.off,
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.on))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylStopperUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperUD->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"CYL_MGZ_LOAD_STOPPER_UP", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off))
				}
			}

			m_fsm.Set(C_LOADING_01);
		}
		break;
	case C_LOADING_01:
		if(g_err.m_bLdSafetyBeam || g_opr.isDoorOpen)
		{
			BeltRun(FALSE);
			m_fsm.Set(C_LOADING_PAUSE_STOP);
			break;
		}

		if(m_fsm.Once())
		{
			BeltRun(TRUE);
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"AC_MGZ_LOAD_BELT_RUN_CW", g_data2c.cEtc.start, 
												cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.acMotor, g_data2c.cEtc.actName, 
												g_data2c.cAcMtName[2], g_data2c.cAcIO[2], g_data2c.cEtc.on))
		}
		else
		{
			if(g_dIn.AOn(iMzLoadZArrival))
			{
				BeltRun(FALSE);
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"AC_MGZ_LOAD_BELT_RUN_CW", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.acMotor, g_data2c.cEtc.actName, 
													g_data2c.cAcMtName[2], g_data2c.cAcIO[2], g_data2c.cEtc.off))
				m_fsm.Set(C_LOADING_02);
			}
		}
		break;
	case C_LOADING_02:
		if(!m_pMtZ->InPos(PZ_LOAD_POS))
		{
			m_pMtZ->Move(PZ_LOAD_POS);
			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_LOAD_POS])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_LOAD_POS] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"MT_MGZ_LOAD_Z_MOVE_LOAD_POS", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_POSIDX_], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_POS_], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_SPDIDX_], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_SPD_], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_ACCIDX_], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_LOAD_POS])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_LOAD_POS] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"MT_MGZ_LOAD_Z_MOVE_LOAD_POS", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_POSIDX_], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_POS_], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_SPDIDX_], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_SPD_], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_ACCIDX_], 
													g_data2c.cMzLift.Z[PZ_LOAD_POS][_ACC_]))
			}
		}

		m_fsm.Set(C_LOADING_END);
		break;
	case C_LOADING_PAUSE_STOP:
		if(!g_err.m_bLdSafetyBeam && !g_opr.isDoorOpen)
			m_fsm.Set(C_LOADING_START);
		break;
	case C_LOADING_END:	
		if(m_pMtZ->InPos(PZ_LOAD_POS))
		{
			if(pmDOWN != m_pCylStopperUD->GetPos(300))
			{
				m_pCylStopperUD->Actuate(pmDOWN);

				if(!g_logChk.bFunction[m_pCylStopperUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperUD->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"CYL_MGZ_LOAD_STOPPER_DOWN", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.off))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylStopperUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperUD->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"CYL_MGZ_LOAD_STOPPER_DOWN", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName,
														g_data2c.cPmName[m_pCylStopperUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
				}

				m_bRdyMzIn=TRUE;
				//g_pNV->NDm(ldMzInSwOn) = 0;
				NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cMzLift.deviceId, L"FULL_MGZ", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"MGZ_DOWN", L"MGZ_LOAD"))				
				m_fsm.Set(C_IDLE);
			}
		}
		break;
	}
}

void CMgzLoadZ::CycleUnLoadingStep(void)
{
	if(!m_fsm.Between(C_UNLOADING_START, C_UNLOADING_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_MZ_UNLOAD_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy())
	{
		if(g_err.m_bLdSafetyBeam || g_opr.isDoorOpen)
		{
			m_pMtZ->Paused();
			return;
		}
		else
		{
			if(m_pMtZ->m_state.isPaused)
			{
				m_pMtZ->Resume();
			}
		}
		return;
	}

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialId, L"$");
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_UNLOADING_START:			
		if(g_err.m_bLdSafetyBeam || g_opr.isDoorOpen)
		{
			BeltRun(FALSE);
			m_fsm.Set(C_UNLOADING_PAUSE_STOP);
			break;
		}

		if(m_fsm.Once())
		{
			NEGRETE_WRITE(g_TpBase.logTransfer(L"MGZ_LOAD", L"EMPTY_MGZ", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"MGZ_DOWN", L"MGZ_UNLOAD"))				
		}
		else
		{
			if(pmDOWN != m_pCylStopperUD->GetPos(300))
			{
				m_pCylStopperUD->Actuate(pmDOWN);

				if(!g_logChk.bFunction[m_pCylStopperUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperUD->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"CYL_MGZ_LOAD_STOPPER_DOWN_EMPTY", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.off))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylStopperUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperUD->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"CYL_MGZ_LOAD_STOPPER_DOWN_EMPTY", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylStopperUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
				}

				//BeltRun(TRUE,TRUE);
				if(!g_dIn.AOn(iMzLoadZArrival))
				{
					BeltRun(FALSE);
					m_fsm.Set(C_UNLOADING_01);
				}
			}
		}
		break;
	case C_UNLOADING_01:
		if(!m_pMtZ->InPos(PZ_UNLOAD_POS))
		{
			m_pMtZ->Move(PZ_UNLOAD_POS);
			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_UNLOAD_POS])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_UNLOAD_POS] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"MT_MGZ_LOAD_Z_MOVE_UNLOAD_POS", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_POSIDX_], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_POS_], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_SPDIDX_], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_SPD_], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_ACCIDX_], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_ACC_]))
			}
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_UNLOAD_POS])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_UNLOAD_POS] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzLift.deviceId, L"MT_MGZ_LOAD_Z_MOVE_UNLOAD_POS", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_POSIDX_], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_POS_], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_SPDIDX_], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_SPD_], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_ACCIDX_], 
													g_data2c.cMzLift.Z[PZ_UNLOAD_POS][_ACC_]))
			}
			m_fsm.Set(C_UNLOADING_END);
		}
		break;
	case C_UNLOADING_PAUSE_STOP:
		if(!g_err.m_bLdSafetyBeam && !g_opr.isDoorOpen)
			m_fsm.Set(C_UNLOADING_START);
		break;
	case C_UNLOADING_END:	
		if(m_pMtZ->InPos(PZ_UNLOAD_POS))
		{
			m_bRdyMzOut = TRUE;
			//g_pNV->NDm(ldMzOutSwOn) = 0;
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cMzLift.deviceId, L"EMPTY_MGZ", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"MGZ_DOWN", L"MGZ_UNLOAD"))				
			m_fsm.Set(C_IDLE);
		}
		break;
	}
}
