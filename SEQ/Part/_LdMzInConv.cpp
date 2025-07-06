#include "..\DEF\Includes.h"

/////////////////////////////////////////////////////////////////////
CLdMzInConv g_ldMzInConv;
/////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
CLdMzInConv::CLdMzInConv()
{
	m_bRun		= FALSE;
	m_bRdyMzIn  = FALSE;
	m_bRdyLoadInCall = FALSE;
}


//-------------------------------------------------------------------
void CLdMzInConv::AutoRun(void)
{
	m_bRdyMzIn = FALSE;
	m_bRdyLoadInCall = FALSE;

	if(g_pNV->UseSkip(usInCylinderSkip))
	{
		if(g_dIn.AOn(iMzInStopperExist01) || g_opr.isDryRun)
			ExistStopper01() = TRUE;
		else
			ExistStopper01() = FALSE;

		if(g_dIn.AOn(iMzInStopperExist02) || g_opr.isDryRun)
			ExistStopper02() = TRUE;
		else
			ExistStopper02() = FALSE;

		if(g_dIn.AOn(iMzInStopperExist03) || g_opr.isDryRun)
			ExistStopper03() = TRUE;
		else
			ExistStopper03() = FALSE;
	}

	int nStopper1ExistErr = GetStopper1ExistErr();
	int nStopper2ExistErr = GetStopper2ExistErr();
	int nStopper3ExistErr = GetStopper3ExistErr();

	if(!m_bRun)
		return;

	m_bRun = FALSE;

	if(m_fsm.IsRun())
		return;

	if(g_ldMz.m_fsm.Between(CLdMz::C_LOADING_START, CLdMz::C_LOADING_END))
		return;
//	if(g_MgzLoadZ.m_fsm.Between(CMgzLoadZ::C_OHT_IN_START, CMgzLoadZ::C_OHT_IN_END))  // Loading시
//		return;
//	if(g_MgzLoadZ.m_fsm.Between(CMgzLoadZ::C_OHT_IN_START, CMgzLoadZ::C_OHT_IN_END))  // Unloading시
//		return;

	BeltRun(FALSE);

	if(!g_pNV->UseSkip(usInCylinderSkip))
	{
		if(EXIST_UNCERTAIN == nStopper1ExistErr)
			return;
		if(EXIST_UNCERTAIN == nStopper2ExistErr)
			return;
		if(EXIST_UNCERTAIN == nStopper3ExistErr)
			return;

		if(EXIST_ERR == nStopper1ExistErr)
		{
			g_err.Save(ER_MZ_IN_STOPPER_1_EXIST);
			return;
		}
		if(EXIST_ERR == nStopper2ExistErr)
		{
			g_err.Save(ER_MZ_IN_STOPPER_2_EXIST);
			return;
		}
		if(EXIST_ERR == nStopper3ExistErr)
		{
			g_err.Save(ER_MZ_IN_STOPPER_3_EXIST);
			return;
		}

		int nExistCnt = 0;

		if(ExistStopper01())
			nExistCnt++;
		if(ExistStopper02())
			nExistCnt++;
		if(ExistStopper03())
			nExistCnt++;
		if(nExistCnt < 3)
		{
			m_bRdyLoadInCall = TRUE;
		}

		if(!ExistStopper01())
		{
			m_pCylStopper01UD->Actuate(pmDOWN);
			if(pmDOWN != m_pCylStopper01UD->GetPos(300))
				return;
		}
		else if(g_ldMz.Exist())
		{
			m_pCylStopper01UD->Actuate(pmUP);
			if(pmUP != m_pCylStopper01UD->GetPos(300))
				return;
		}
		if(!ExistStopper02())
		{
			m_pCylStopper02UD->Actuate(pmDOWN);
			if(pmDOWN != m_pCylStopper02UD->GetPos(300))
				return;
		}
		else
		{
			m_pCylStopper02UD->Actuate(pmUP);
			if(pmUP != m_pCylStopper02UD->GetPos(300))
				return;
		}
		if(!ExistStopper03())
		{
			m_pCylStopper03UD->Actuate(pmDOWN);
			if(pmDOWN != m_pCylStopper03UD->GetPos(300))
				return;
		}
		else
		{
			m_pCylStopper03UD->Actuate(pmUP);
			if(pmUP != m_pCylStopper03UD->GetPos(300))
				return;
		}
	}
	else
	{
		m_pCylStopper01UD->Actuate(pmDOWN);
		if(pmDOWN != m_pCylStopper01UD->GetPos(300))
			return;
		m_pCylStopper02UD->Actuate(pmDOWN);
		if(pmDOWN != m_pCylStopper02UD->GetPos(300))
			return;
		m_pCylStopper03UD->Actuate(pmDOWN);
		if(pmDOWN != m_pCylStopper03UD->GetPos(300))
			return;

		if(!g_dIn.AOn(iMzInStopperExist01))
			m_bRdyLoadInCall = TRUE;
	}
	
	if(!g_ldMz.m_pMtY->IsRdy())
		return;
	if(g_ldMz.m_pMtY->ComparePos(CLdMz::PY_RCV))
		return;

	switch(GetState())
	{
	case S_IDLE:
		break;
	case S_EMPTY_STOPPER_1:
		if(ExistStopper02())
			m_fsm.Set(C_STOPPER_STEP_START, msgStopperStep2MoveStep1);
		else if(ExistStopper03())
			m_fsm.Set(C_STOPPER_STEP_START, msgStopperStep3MoveStep1);
		else
		{
			if(g_MgzLoadZ.IsReadyLoadIn())
			{
				m_fsm.Set(C_LOADER_TO_STOPPER_START, msgLoaderMoveStopper1);
				break;
			}

//			m_bRdyMzIn = TRUE;
		}
		break;
	case S_EMPTY_STOPPER_2:
		if(ExistStopper03())
			m_fsm.Set(C_STOPPER_STEP_START, msgStopperStep3MoveStep2);
		else
		{
			if(g_MgzLoadZ.IsReadyLoadIn())
			{
				m_fsm.Set(C_LOADER_TO_STOPPER_START, msgLoaderMoveStopper2);
				break;
			}

			m_bRdyMzIn = TRUE;
		}
		break;
	case S_EMPTY_STOPPER_3:
		if(g_MgzLoadZ.IsReadyLoadIn())
		{
			m_fsm.Set(C_LOADER_TO_STOPPER_START, msgLoaderMoveStopper3);
			break;
		}

		m_bRdyMzIn = TRUE;
		break;
	case S_ALL_EXIST:
		m_bRdyMzIn = TRUE;
		break;
	case S_ARRIVAL:
		// 자재가 있으면 MGZ 동작
		// oht mode 아닐시에만 사용
		m_bRdyMzIn = TRUE;
		break;
	case S_CONV :
		m_fsm.Set(C_START);
		break;	

	}
}


//-------------------------------------------------------------------
void CLdMzInConv::CycleRun(void)
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

	CycleLoaderToStopper();
	CycleStopperStep();
	CycleConvMove();
}


//-------------------------------------------------------------------
void CLdMzInConv::Init(void)
{
	m_pCylStopper01UD	= &g_pm[CYL_MGZ_IN_STOPPER_UD_01];
	m_pCylStopper02UD	= &g_pm[CYL_MGZ_IN_STOPPER_UD_02];
	m_pCylStopper03UD	= &g_pm[CYL_MGZ_IN_STOPPER_UD_03];
}


//-------------------------------------------------------------------
int& CLdMzInConv::ExistStopper01(void)
{
	return (g_pNV->m_pData->ndm[existMzInStopper01]);
}


//-------------------------------------------------------------------
int& CLdMzInConv::ExistStopper02(void)
{
	return (g_pNV->m_pData->ndm[existMzInStopper02]);
}


//-------------------------------------------------------------------
int& CLdMzInConv::ExistStopper03(void)
{
	return (g_pNV->m_pData->ndm[existMzInStopper03]);
}


//-------------------------------------------------------------------
BOOL CLdMzInConv::IsReadyMzIn(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyMzIn);
	}
	else
	{
		BOOL isRdy  = !g_dOut.IsOn(oAcMgzInRun);
			 isRdy &= (g_dIn.AOn(iMzInStopperExist01) && ExistStopper01()) || g_opr.isDryRun;
			 isRdy &= !m_fsm.IsRun();
			 
// 			 if(g_pNV->UseSkip(usOhtMode) && !g_pNV->UseSkip(usOhtInCylinderSkip))
// 				isRdy &= (pmUP == m_pCylOhtUD->GetPos(300));
// 			 else
 			 	isRdy &= !m_fsm.Between(C_LOADER_TO_STOPPER_START,C_END);

		return (isRdy);
	}
}


//-------------------------------------------------------------------
BOOL CLdMzInConv::IsReadyLoadInCall(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyLoadInCall);
	}
	else
	{
		BOOL isRdy = (g_dIn.AOn(iMzInStopperExist01) == ExistStopper01()) || g_opr.isDryRun;
			 isRdy &= (g_dIn.AOn(iMzInStopperExist02) == ExistStopper02()) || g_opr.isDryRun;
			 isRdy &= (g_dIn.AOn(iMzInStopperExist03) == ExistStopper03()) || g_opr.isDryRun;
			 isRdy &= !m_fsm.IsRun();

		int nExistCnt = 0;

		if(ExistStopper01())
			nExistCnt++;
		if(ExistStopper02())
			nExistCnt++;
		if(ExistStopper03())
			nExistCnt++;
		
		isRdy &= (nExistCnt < 3);

// 		if(g_pNV->UseSkip(usInCylinderSkip))
// 		{
// 			isRdy  = ((!g_dIn.AOn(iMzInOhtExist) && !g_dIn.AOn(iMzInOhtExistF)) == g_oht.ExistOhtIn()) || g_opr.isDryRun;
// 			isRdy &= !m_fsm.IsRun();
// 		}
		
		return (isRdy);
	}
}


//-------------------------------------------------------------------
void CLdMzInConv::BeltRun(BOOL Run, BOOL Ccw)
{
	if(Run)
	{
		g_dOut.On(oAcMgzInRun);  // Pwr
//		if(Ccw)
//			g_dOut.Off(oAcMgzInDir); // 정방향
//		else
//			g_dOut.On(oAcMgzInDir); // 역방향
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
			g_dOut.Off(oAcMgzInRun);
			//		g_dOut.Off(oAcMgzInDir);
		}

	}
}


//-------------------------------------------------------------------
int  CLdMzInConv::GetState(void)
{
	int nState = S_IDLE;

	if(!g_pNV->UseSkip(usInCylinderSkip))
	{
		if(!ExistStopper01())
			nState = S_EMPTY_STOPPER_1;
		else if(!ExistStopper02())
			nState = S_EMPTY_STOPPER_2;
		else if(!ExistStopper03())
			nState = S_EMPTY_STOPPER_3;
		else
			nState = S_ALL_EXIST;
	}
	else
	{
		if(ExistStopper01())
			return (S_ARRIVAL);
		else if(g_dIn.AOn(iMzInConvExist))
			return (S_CONV);
	}

	return (nState);
}


//-------------------------------------------------------------------
BOOL CLdMzInConv::IsErr(void)
{
	if(0 < m_pCylStopper01UD->GetErr())
		return (TRUE);
	if(0 < m_pCylStopper02UD->GetErr())
		return (TRUE);
	if(0 < m_pCylStopper03UD->GetErr())
		return (TRUE);
		
	return (FALSE);
}


//-------------------------------------------------------------------
int  CLdMzInConv::GetStopper1ExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	if(ExistStopper01() == (g_dIn.AOn(iMzInStopperExist01)))
	{
		m_tmStopper1ExistErr.Reset();
	}
	else
	{
		if(m_tmStopper1ExistErr.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
int  CLdMzInConv::GetStopper2ExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	if(ExistStopper02() == (g_dIn.AOn(iMzInStopperExist02)))
	{
		m_tmStopper2ExistErr.Reset();
	}
	else
	{
		if(m_tmStopper2ExistErr.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
int  CLdMzInConv::GetStopper3ExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	if(ExistStopper03() == (g_dIn.AOn(iMzInStopperExist03)))
	{
		m_tmStopper3ExistErr.Reset();
	}
	else
	{
		if(m_tmStopper3ExistErr.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
void CLdMzInConv::CycleLoaderToStopper(void)
{
	if(!m_fsm.Between(C_LOADER_TO_STOPPER_START, C_LOADER_TO_STOPPER_END))
		return;

	if(m_fsm.TimeLimit(600000))
	{
		m_fsm.Set(C_ERROR, ER_MZ_IN_STOPPER_CYCLE_TM_OVER);
		return;
	}

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialId, L"$");
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_LOADER_TO_STOPPER_START:
		if(m_fsm.Once())
		{
			if(msgLoaderMoveStopper1 == m_fsm.GetMsg())
				NEGRETE_WRITE(g_TpBase.logTransfer(L"MGZ_IN_CONV", L"MOVE_MGZ", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"MGZ_LOAD", L"BUFFER_1"))
			else if(msgLoaderMoveStopper2 == m_fsm.GetMsg())
				NEGRETE_WRITE(g_TpBase.logTransfer(L"MGZ_IN_CONV", L"MOVE_MGZ", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"MGZ_LOAD", L"BUFFER_2"))
			else if(msgLoaderMoveStopper3 == m_fsm.GetMsg())
				NEGRETE_WRITE(g_TpBase.logTransfer(L"MGZ_IN_CONV", L"MOVE_MGZ", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"MGZ_LOAD", L"BUFFER_3"))
		}
		else
		{
			if(msgLoaderMoveStopper1 == m_fsm.GetMsg())
			{
				m_pCylStopper01UD->Actuate(pmDOWN);
				m_pCylStopper02UD->Actuate(pmDOWN);
				m_pCylStopper03UD->Actuate(pmDOWN);
				if(pmDOWN != m_pCylStopper01UD->GetPos(300))
					break;
				if(pmDOWN != m_pCylStopper02UD->GetPos(300))
					break;
				if(pmDOWN != m_pCylStopper03UD->GetPos(300))
					break;
			}
			else if(msgLoaderMoveStopper2 == m_fsm.GetMsg())
			{
				m_pCylStopper02UD->Actuate(pmDOWN);
				m_pCylStopper03UD->Actuate(pmDOWN);
				if(pmDOWN != m_pCylStopper02UD->GetPos(300))
					break;
				if(pmDOWN != m_pCylStopper03UD->GetPos(300))
					break;
			}
			else if(msgLoaderMoveStopper3 == m_fsm.GetMsg())
			{
				m_pCylStopper03UD->Actuate(pmDOWN);
				if(pmDOWN != m_pCylStopper03UD->GetPos(300))
					break;
			}
			m_fsm.Set(C_LOADER_TO_STOPPER_01);
		}
		break;
	case C_LOADER_TO_STOPPER_01:
		if(pmDOWN != g_MgzLoadZ.m_pCylStopperUD->GetPos(300))
		{
			g_MgzLoadZ.m_pCylStopperUD->Actuate(pmDOWN); 

			if(!g_logChk.bFunction[g_MgzLoadZ.m_pCylStopperUD->GetNo()])
			{
				g_logChk.bFunction[g_MgzLoadZ.m_pCylStopperUD->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzInConv.deviceId, L"CYL_MGZ_LOAD_STOPPER_DOWN_IN_CONV", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[g_MgzLoadZ.m_pCylStopperUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[g_MgzLoadZ.m_pCylStopperUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[g_MgzLoadZ.m_pCylStopperUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.off))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[g_MgzLoadZ.m_pCylStopperUD->GetNo()])
			{
				g_logChk.bFunction[g_MgzLoadZ.m_pCylStopperUD->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzInConv.deviceId, L"CYL_MGZ_LOAD_STOPPER_DOWN_IN_CONV", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[g_MgzLoadZ.m_pCylStopperUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[g_MgzLoadZ.m_pCylStopperUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[g_MgzLoadZ.m_pCylStopperUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
			}
		}
		m_fsm.Set(C_LOADER_TO_STOPPER_02);
		break;

	case C_LOADER_TO_STOPPER_02:
		if(m_fsm.Once())                    
		{
			BeltRun(TRUE);
			g_MgzLoadZ.BeltRun(TRUE);

			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzInConv.deviceId, L"AC_MGZ_LOAD_BELT_RUN_CW_IN_CONV", g_data2c.cEtc.start, 
												cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.acMotor, g_data2c.cEtc.actName, 
												g_data2c.cAcMtName[2], g_data2c.cAcIO[2], g_data2c.cEtc.on))

			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzInConv.deviceId, L"AC_MGZ_IN_BELT_RUN", g_data2c.cEtc.start, 
												cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.acMotor, g_data2c.cEtc.actName, 
												g_data2c.cAcMtName[0], g_data2c.cAcIO[0], g_data2c.cEtc.on))
		}
		else
		{
			if(msgLoaderMoveStopper1 == m_fsm.GetMsg())
			{
				if(m_fsm.TimeLimit(30000))
				{
					if(!g_dIn.AOn(iMzInStopperExist01))
						g_err.Save(ER_MZ_IN_STOPPER_01_EXIST_NOT_ON);
					if(g_dIn.AOn(iMzLoadZExist))
						g_err.Save(ER_MZ_IN_LOADER_EXIST_NOT_OFF);
					g_MgzLoadZ.BeltRun(FALSE);
					g_err.Save(ER_MZ_IN_JAM);
					m_fsm.Set(C_ERROR);
					break;
				}

				if(g_opr.isDryRun)
				{
					if(!m_fsm.Delay(5000))
						break;

					ExistStopper01() = TRUE;
					g_MgzLoadZ.ExistLoadArrival() = FALSE;
					m_fsm.Set(C_LOADER_TO_STOPPER_03);
					break;
				}

				if(g_dIn.AOn(iMzInStopperExist01) && !g_dIn.AOn(iMzLoadZArrival))
				{
//					if(m_fsm.Delay(2000))
//					{
						ExistStopper01() = TRUE;
						g_MgzLoadZ.ExistLoadArrival() = FALSE;
						m_fsm.Set(C_LOADER_TO_STOPPER_03);
//					}
				}
				else
					m_fsm.RstDelay();
			}
			else if(msgLoaderMoveStopper2 == m_fsm.GetMsg())
			{
				if(m_fsm.TimeLimit(30000))
				{
					if(!g_dIn.AOn(iMzInStopperExist02))
						g_err.Save(ER_MZ_IN_STOPPER_02_EXIST_NOT_ON);
					if(g_dIn.AOn(iMzLoadZExist))
						g_err.Save(ER_MZ_IN_LOADER_EXIST_NOT_OFF);
					g_MgzLoadZ.BeltRun(FALSE);
					g_err.Save(ER_MZ_IN_JAM);
					m_fsm.Set(C_ERROR);
					break;
				}

				if(g_opr.isDryRun)
				{
					if(!m_fsm.Delay(5000))
						break;

					ExistStopper02() = TRUE;
					g_MgzLoadZ.ExistLoadArrival() = FALSE;
					m_fsm.Set(C_LOADER_TO_STOPPER_03);
					break;
				}

				if(g_dIn.AOn(iMzInStopperExist02) && !g_dIn.AOn(iMzLoadZArrival))
				{
//					if(m_fsm.Delay(2000))
//					{
						ExistStopper02() = TRUE;
						g_MgzLoadZ.ExistLoadArrival() = FALSE;
						m_fsm.Set(C_LOADER_TO_STOPPER_03);
//					}
				}
				else
					m_fsm.RstDelay();
			}
			else if(msgLoaderMoveStopper3 == m_fsm.GetMsg())
			{
				if(m_fsm.TimeLimit(30000))
				{
					if(!g_dIn.AOn(iMzInStopperExist03))
						g_err.Save(ER_MZ_IN_STOPPER_03_EXIST_NOT_ON);
					if(g_dIn.AOn(iMzLoadZExist))
						g_err.Save(ER_MZ_IN_LOADER_EXIST_NOT_OFF);
					g_MgzLoadZ.BeltRun(FALSE);
					g_err.Save(ER_MZ_IN_JAM);
					m_fsm.Set(C_ERROR);
					break;
				}

				if(g_opr.isDryRun)
				{
					if(!m_fsm.Delay(5000))
						break;

					ExistStopper03() = TRUE;
					g_MgzLoadZ.ExistLoadArrival() = FALSE;
					m_fsm.Set(C_LOADER_TO_STOPPER_03);
					break;
				}

				if(g_dIn.AOn(iMzInStopperExist03) && !g_dIn.AOn(iMzLoadZArrival))
				{
//					if(m_fsm.Delay(2000))
//					{
						ExistStopper03() = TRUE;
						g_MgzLoadZ.ExistLoadArrival() = FALSE;
						m_fsm.Set(C_LOADER_TO_STOPPER_03);
//					}
				}
				else
					m_fsm.RstDelay();
			}
		}
		break;

	case C_LOADER_TO_STOPPER_03:
		if(m_fsm.Once())
		{
			BeltRun(FALSE);
			g_MgzLoadZ.BeltRun(FALSE);
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzInConv.deviceId, L"AC_MGZ_LOAD_BELT_RUN_CW_IN_CONV", g_data2c.cEtc.end, 
												cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.acMotor, g_data2c.cEtc.actName, 
												g_data2c.cAcMtName[2], g_data2c.cAcIO[2], g_data2c.cEtc.off))
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzInConv.deviceId, L"AC_MGZ_IN_BELT_RUN", g_data2c.cEtc.end, 
												cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.acMotor, g_data2c.cEtc.actName, 
												g_data2c.cAcMtName[0], g_data2c.cAcIO[0], g_data2c.cEtc.off))
		}
		else
		{
			if(!m_fsm.Delay(300))
				break;
// 			m_pCylStopper01UD->Actuate(pmUP);
// 			m_pCylStopper02UD->Actuate(pmUP);
// 			m_pCylStopper03UD->Actuate(pmUP);
// 			g_MgzLoadZ.m_pCylStopperUD->Actuate(pmUP);
// 			if(pmUP != m_pCylStopper01UD->GetPos(300))
// 				break;
// 			if(pmUP != m_pCylStopper02UD->GetPos(300))
// 				break;
// 			if(pmUP != m_pCylStopper03UD->GetPos(300))
// 				break;
// 			if(pmUP != g_MgzLoadZ.m_pCylStopperUD->GetPos(300))
// 				break;
			m_fsm.Set(C_LOADER_TO_STOPPER_END);
		}
		break;
	case C_LOADER_TO_STOPPER_END:
		if(msgLoaderMoveStopper1 == m_fsm.GetMsg())
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cMzInConv.deviceId, L"MOVE_MGZ", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"MGZ_LOAD", L"BUFFER_1"))			
		else if(msgLoaderMoveStopper2 == m_fsm.GetMsg())
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cMzInConv.deviceId, L"MOVE_MGZ", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"MGZ_LOAD", L"BUFFER_2"))			
		else if(msgLoaderMoveStopper3 == m_fsm.GetMsg())
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cMzInConv.deviceId, L"MOVE_MGZ", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"MGZ_LOAD", L"BUFFER_3"))			

		m_fsm.Set(C_IDLE);
		break;
	}
}

// Step Move 로그표준화는 추후에 Time 공백 발생시 확인 후 추가 hkkim
//-------------------------------------------------------------------
void CLdMzInConv::CycleStopperStep(void)
{
	if(!m_fsm.Between(C_STOPPER_STEP_START, C_STOPPER_STEP_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_MZ_IN_STOPPER_STEP_CYCLE_TM_OVER);
		return;
	}

	switch(m_fsm.Get())
	{
	case C_STOPPER_STEP_START:
		if(msgStopperStep2MoveStep1 == m_fsm.GetMsg())
		{
			m_pCylStopper01UD->Actuate(pmDOWN);
			m_pCylStopper02UD->Actuate(pmDOWN);
			if(pmDOWN != m_pCylStopper01UD->GetPos(300))
				break;
			if(pmDOWN != m_pCylStopper02UD->GetPos(300))
				break;
			m_pCylStopper03UD->Actuate(pmUP);
			if(pmUP != m_pCylStopper03UD->GetPos(300))
				break;
		}
		else if(msgStopperStep3MoveStep2 == m_fsm.GetMsg())
		{
			m_pCylStopper01UD->Actuate(pmUP);
			if(pmUP != m_pCylStopper01UD->GetPos(300))
			break;

			m_pCylStopper02UD->Actuate(pmDOWN);
			m_pCylStopper03UD->Actuate(pmDOWN);
			if(pmDOWN != m_pCylStopper02UD->GetPos(300))
				break;
			if(pmDOWN != m_pCylStopper03UD->GetPos(300))
				break;
		}
		else if(msgStopperStep3MoveStep1 == m_fsm.GetMsg())
		{
			m_pCylStopper01UD->Actuate(pmDOWN);
			m_pCylStopper02UD->Actuate(pmDOWN);
			m_pCylStopper03UD->Actuate(pmDOWN);
			if(pmDOWN != m_pCylStopper01UD->GetPos(300))
				break;
			if(pmDOWN != m_pCylStopper02UD->GetPos(300))
				break;
			if(pmDOWN != m_pCylStopper03UD->GetPos(300))
				break;
		}
		m_fsm.Set(C_STOPPER_STEP_01);
		break;
	case C_STOPPER_STEP_01:
		if(m_fsm.Once())
		{
			BeltRun(TRUE);
		}
		else
		{
			if(msgStopperStep2MoveStep1 == m_fsm.GetMsg())
			{
				if(m_fsm.TimeLimit(30000))
				{
					if(!g_dIn.AOn(iMzInStopperExist01))
						g_err.Save(ER_MZ_IN_STOPPER_01_EXIST_NOT_ON);
					if(g_dIn.AOn(iMzInStopperExist02))
						g_err.Save(ER_MZ_IN_STOPPER_02_EXIST_NOT_OFF);
					g_MgzLoadZ.BeltRun(FALSE);
					g_err.Save(ER_MZ_IN_JAM);
					m_fsm.Set(C_ERROR);
					break;
				}

				if(g_opr.isDryRun)
				{
					if(!m_fsm.Delay(3000))
						break;

					ExistStopper01() = TRUE;
					ExistStopper02() = FALSE;
					m_fsm.Set(C_STOPPER_STEP_02);
					break;
				}

				if(g_dIn.AOn(iMzInStopperExist01) && !g_dIn.AOn(iMzInStopperExist02))
				{
//					if(m_fsm.Delay(2000))
//					{
						ExistStopper01() = TRUE;
						ExistStopper02() = FALSE;
						m_fsm.Set(C_STOPPER_STEP_02);
//					}
				}
				else
					m_fsm.RstDelay();
			}
			else if(msgStopperStep3MoveStep1 == m_fsm.GetMsg())
			{
				if(m_fsm.TimeLimit(30000))
				{
					if(!g_dIn.AOn(iMzInStopperExist01))
						g_err.Save(ER_MZ_IN_STOPPER_01_EXIST_NOT_ON);
					if(g_dIn.AOn(iMzInStopperExist03))
						g_err.Save(ER_MZ_IN_STOPPER_03_EXIST_NOT_OFF);
					g_MgzLoadZ.BeltRun(FALSE);
					g_err.Save(ER_MZ_IN_JAM);
					m_fsm.Set(C_ERROR);
					break;
				}

				if(g_opr.isDryRun)
				{
					if(!m_fsm.Delay(3000))
						break;

					ExistStopper01() = TRUE;
					ExistStopper03() = FALSE;
					m_fsm.Set(C_STOPPER_STEP_02);
					break;
				}

				if(g_dIn.AOn(iMzInStopperExist01) && !g_dIn.AOn(iMzInStopperExist03))
				{
//					if(m_fsm.Delay(2000))
//					{
						ExistStopper01() = TRUE;
						ExistStopper03() = FALSE;
						m_fsm.Set(C_STOPPER_STEP_02);
//					}
				}
				else
					m_fsm.RstDelay();
			}
			else if(msgStopperStep3MoveStep2 == m_fsm.GetMsg())
			{
				if(m_fsm.TimeLimit(30000))
				{
					if(!g_dIn.AOn(iMzInStopperExist02))
						g_err.Save(ER_MZ_IN_STOPPER_02_EXIST_NOT_ON);
					if(g_dIn.AOn(iMzInStopperExist02))
						g_err.Save(ER_MZ_IN_STOPPER_03_EXIST_NOT_OFF);
					g_MgzLoadZ.BeltRun(FALSE);
					g_err.Save(ER_MZ_IN_JAM);
					m_fsm.Set(C_ERROR);
					break;
				}

				if(g_opr.isDryRun)
				{
					if(!m_fsm.Delay(3000))
						break;

					ExistStopper02() = TRUE;
					ExistStopper03() = FALSE;
					m_fsm.Set(C_STOPPER_STEP_02);
					break;
				}

				if(g_dIn.AOn(iMzInStopperExist02) && !g_dIn.AOn(iMzInStopperExist03))
				{
//					if(m_fsm.Delay(2000))
//					{
						ExistStopper02() = TRUE;
						ExistStopper03() = FALSE;
						m_fsm.Set(C_STOPPER_STEP_02);
//					}
				}
				else
					m_fsm.RstDelay();
			}
		}
		break;
	case C_STOPPER_STEP_02:
		if(m_fsm.Once())
		{
			BeltRun(FALSE);
		}
		else
		{
			if(m_fsm.Delay(300))
				break;
// 
// 			m_pCylStopper01UD->Actuate(pmUP);
// 			m_pCylStopper02UD->Actuate(pmUP);
// 			m_pCylStopper03UD->Actuate(pmUP);
// 			if(pmUP != m_pCylStopper01UD->GetPos(300))
// 				break;
// 			if(pmUP != m_pCylStopper02UD->GetPos(300))
// 				break;
// 			if(pmUP != m_pCylStopper03UD->GetPos(300))
// 				break;

			m_fsm.Set(C_STOPPER_STEP_END);
		}
		break;
	case C_STOPPER_STEP_END:
		m_fsm.Set(C_IDLE);
		break;
	}
}


// Conv Move 로그표준화는 추후에 Time 공백 발생시 확인 후 추가 hkkim
//-------------------------------------------------------------------
void CLdMzInConv::CycleConvMove(void)
{
	if(!m_fsm.Between(C_START, C_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_MZ_IN_CYCLE_TM_OVER);
		return;
	}

	switch(m_fsm.Get())
	{
	case C_START:
		if(m_fsm.Once())
		{
			BeltRun(TRUE);
		}
		else
		{
			if(m_fsm.TimeLimit(20000))
			{
				g_MgzLoadZ.BeltRun(FALSE);
				m_fsm.Set(C_ERROR, ER_MZ_IN_JAM);
				break;
			}

			if(g_opr.isDoorOpen)
			{
				BeltRun(FALSE);
				m_fsm.Set(C_PAUSED_STOP);
				break;
			}

			if(g_dIn.AOn(iMzInStopperExist01) || g_opr.isDryRun)
				m_fsm.Set(C_END);
		}
		break;

	case C_PAUSED_STOP:
		if(!g_opr.isDoorOpen)
			m_fsm.Set(C_START);
		break;

	case C_END:
		if(!m_fsm.Delay(1000))
			break;

		BeltRun(FALSE);
		m_fsm.Set(C_IDLE);
		break;
	}
}

