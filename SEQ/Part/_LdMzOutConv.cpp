#include "..\DEF\Includes.h"


//////////////////////////////////////////////////////////////////////////
CLdMzOutConv g_ldMzOutConv;
//////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
CLdMzOutConv::CLdMzOutConv()
{
	m_bRun		= FALSE;
	m_bRdyMzOut = FALSE;
}


//-------------------------------------------------------------------
void CLdMzOutConv::AutoRun()
{
	m_bRdyMzOut = FALSE;

	if(!g_dIn.AOn(iMzOutExist02))
	{
		m_tmOutMzFull.Reset();
		if(g_wr.Find(WR_LOADER_MGZ_FULL))
			g_wr.Del(WR_LOADER_MGZ_FULL);
	}


	if(!m_bRun)
		return;

	m_bRun = FALSE;

	if(m_fsm.IsRun())
		return;

	if(g_ldMz.m_fsm.Between(CLdMz::C_EJECT_START, CLdMz::C_EJECT_END))
		return;
//	if(g_oht.m_fsm.Between(COht::C_OHT_OUT_START, COht::C_OHT_OUT_END))
//		return;

	BeltRun(FALSE);

//	if(g_pNV->UseSkip(usOhtMode))
		AutoMove();
//	else
//	ManualMove();
}


//-------------------------------------------------------------------
void CLdMzOutConv::AutoMove(void)
{
	int existErrArrival = GetExistErrArrival();
	int existErrBuffer1 = GetExistErrBuffer1();
	int existErrBuffer2 = GetExistErrBuffer2();

	if(EXIST_UNCERTAIN == existErrArrival)
		return;
	if(EXIST_UNCERTAIN == existErrBuffer1)
		return;
	if(EXIST_UNCERTAIN == existErrBuffer2)
		return;

	if(EXIST_ERR == existErrArrival)
	{
		g_err.Save(ER_MZ_OUT_ARRIVAL_EXIST);
		return;
	}
	if(EXIST_ERR == existErrBuffer1)
	{
		g_err.Save(ER_MZ_OUT_BUFFER_EXIST1);
		return;
	}
	if(EXIST_ERR == existErrBuffer2)
	{
		g_err.Save(ER_MZ_OUT_BUFFER_EXIST2);
		return;
	}


	if(ExistBuffer2())
	{
		if(m_tmOutMzFull.TmOver(20000))
		{
			g_wr.Save(WR_LOADER_MGZ_FULL);
		}
	}

	if(!g_ldMz.m_pMtY->IsRdy())
		return;
	if(CLdMz::PY_EJECT == g_ldMz.m_pMtY->m_profile.cmdIndex)
		return;

	switch(GetState())
	{
	case S_IDLE:
		break;
	case S_ARRIVAL:

		if(g_pNV->NDm(ldMzOutSwOn))
		{
			if(!ExistBuffer1() && !ExistBuffer2())
			{
				g_pNV->NDm(ldMzOutSwOn) =0;
			}
			else if(!ExistBuffer1() && ExistArrival())
			{
				m_fsm.Set(C_START, msgBuffer1Empty);
			}
			else if(!ExistBuffer2() && ExistBuffer1())
			{
				m_fsm.Set(C_START, msgBuffer2Empty);
			}
			else if(ExistBuffer2() && !g_dIn.AOn(iMzLoadZMid))
			{
				m_fsm.Set(C_START, msgBufferOut);
			}
		}
		else if(!ExistBuffer1() && !ExistBuffer2())
		{
			m_fsm.Set(C_START, msgBuffer1Empty);
		}
		else if(!ExistBuffer2())
		{
			m_fsm.Set(C_START, msgBuffer2Empty);
		}
		break;
	case S_ALL_EMPTY:
		m_bRdyMzOut = TRUE;
		break;
	}
}


//-------------------------------------------------------------------
void CLdMzOutConv::ManualMove(void)
{
	if(g_dIn.AOn(iMzOutExist02))
	{
		if(m_tmOutMzFull.TmOver(20000))
			g_wr.Save(WR_LOADER_MGZ_FULL);
	}

	if(!g_ldMz.m_pMtY->IsRdy())
		return;
	if(CLdMz::PY_EJECT == g_ldMz.m_pMtY->m_profile.cmdIndex)
		return;

	if(g_dIn.AOn(iMzOutArrival))
		ExistArrival() = TRUE;
	else
		ExistArrival() = FALSE;

	if(g_dIn.AOn(iMzOutExist01))
		ExistBuffer1() = TRUE;
	else
		ExistBuffer1() = FALSE;

	if(g_dIn.AOn(iMzOutExist02))
		ExistBuffer2() = TRUE;
	else
		ExistBuffer2() = FALSE;

	if(g_dIn.AOn(iMzOutArrival))
		m_fsm.Set(C_MANUAL_START);
	else
		m_bRdyMzOut = TRUE;
}


//-------------------------------------------------------------------
void CLdMzOutConv::Init(void)
{
	m_pCylStopperL	= &g_pm[CYL_MGZ_OUT_STOPPER_FB_01];
	m_pCylStopperR	= &g_pm[CYL_MGZ_OUT_STOPPER_FB_02];
}


//-------------------------------------------------------------------
int& CLdMzOutConv::ExistBuffer1(void)
{
	return (g_pNV->m_pData->ndm[existMzOutBuffer1]);
}

//-------------------------------------------------------------------
int& CLdMzOutConv::ExistBuffer2(void)
{
	return (g_pNV->m_pData->ndm[existMzOutBuffer2]);
}

//-------------------------------------------------------------------
int& CLdMzOutConv::ExistArrival(void)
{
	return (g_pNV->m_pData->ndm[existMzOutArrival]);
}


//-------------------------------------------------------------------
BOOL CLdMzOutConv::IsReadyMzOut(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyMzOut);
	}
	else
	{
		BOOL isRdy  = FALSE;
		 isRdy  = !g_dIn.AOn(iMzOutArrival) || g_opr.isDryRun;
			
		return (isRdy);
	}
}


//-------------------------------------------------------------------
void CLdMzOutConv::BeltRun(BOOL Run, BOOL Ccw)
{
	if(Run)
	{
		g_dOut.On(oAcMgzOutRun);  // Pwr
// 		if(Ccw)
// 			g_dOut.Off(oAcMgzOutDir); // 정방향
// 		else
// 			g_dOut.On(oAcMgzOutDir); // 역방향
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
			g_dOut.Off(oAcMgzOutRun);
			//		g_dOut.Off(oAcMgzOutDir);
		}
	}
}


//-------------------------------------------------------------------
void CLdMzOutConv::CycleRun(void)
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

	CycleConvMoveAuto();
	CycleConvMoveManual();
}


//-------------------------------------------------------------------
int  CLdMzOutConv::GetState(void)
{
	int nState = S_IDLE;

	if(ExistArrival() || !!g_pNV->NDm(ldMzOutSwOn))
		nState = S_ARRIVAL;
	else
		nState = S_ALL_EMPTY;

	return (nState);
}


//-------------------------------------------------------------------
BOOL CLdMzOutConv::IsErr(void)
{
	return (FALSE);
}


//-------------------------------------------------------------------
int CLdMzOutConv::GetExistErrBuffer1(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL bSenOn = g_dIn.AOn(iMzOutExist01);

	if(bSenOn == ExistBuffer1())
		m_tmExistErrBuffer1.Reset();
	else
	{
		if(m_tmExistErrBuffer1.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}

//-------------------------------------------------------------------
int CLdMzOutConv::GetExistErrBuffer2(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL bSenOn = g_dIn.AOn(iMzOutExist02);

	if(bSenOn == ExistBuffer2())
		m_tmExistErrBuffer2.Reset();
	else
	{
		if(m_tmExistErrBuffer2.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}

//-------------------------------------------------------------------
int  CLdMzOutConv::GetExistErrArrival(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL bSenOn = g_dIn.AOn(iMzOutArrival);

	if(bSenOn == ExistArrival())
		m_tmExistErrArrival.Reset();
	else
	{
		if(m_tmExistErrArrival.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
void CLdMzOutConv::CycleConvMoveAuto(void)
{
	if(!m_fsm.Between(C_START, C_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_MZ_OUT_CYCLE_TM_OVER);
		return;
	}

	int nMsg = m_fsm.GetMsg();

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if(g_pNV->UseSkip(usRfid))
		_sprintf(cMaterialId, L"MGZID-%s", g_pNV->m_pLotInfo->data[LOT_INFO_CONV_ARRIVAL].carrierID);
	else
		_sprintf(cMaterialId, L"MGZID-SKIP");
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_START:
		if(m_fsm.Once())
		{
			// 시퀀스 확인 후 아래 case의 경우 로그 남길 수 있도록 확인 필요
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cMzOutConv.deviceId, L"MOVE_MGZ", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"MGZ_OUT_ARRIVAL", L"MGZ_LOAD"))
		}
		else
		{
	// 		if(g_err.m_bLdSafetyBeam || g_opr.isDoorOpen)
	// 		{
	// 			BeltRun(FALSE);
	// 			m_fsm.Set(C_PAUSED_STOP);
	// 			break;
	// 		}

	//		BeltRun(TRUE);
			m_fsm.Set(C_01);
		}
		break;
	case C_PAUSED_STOP:
		if(!g_opr.isDoorOpen)
			m_fsm.Set(C_START);
		break;
	case C_01:
// 		if(g_err.m_bLdSafetyBeam || g_opr.isDoorOpen)
// 		{
// 			BeltRun(FALSE);
// 			m_fsm.Set(C_PAUSED_STOP);
// 			break;
// 		}

		if(m_fsm.TimeLimit(60000))
		{
			m_fsm.Set(C_ERROR, ER_MZ_OUT_JAM);
			break;
		}

//		if(!m_fsm.Delay(5000))
//			break;

		if(msgBuffer1Empty == nMsg)
		{
			BeltRun(TRUE);
			if((!g_dIn.AOn(iMzOutArrival) && g_dIn.AOn(iMzOutExist01)) || g_opr.isDryRun)
			{
				ExistArrival() = FALSE;
				ExistBuffer1() = TRUE;

				g_lotInfo.LotInfoCopy(LOT_INFO_CONV_ARRIVAL, LOT_INFO_CONV_BUFFER1);
				g_lotInfo.LotInfoClear(LOT_INFO_CONV_ARRIVAL);

				m_fsm.Set(C_END);
			}
		}
		else if(msgBuffer2Empty == nMsg)
		{
			if(pmFWD == m_pCylStopperL->GetPos(100) && pmFWD == m_pCylStopperR->GetPos(100))
			{
				BeltRun(TRUE);
				if(((!g_dIn.AOn(iMzOutExist01) || ExistArrival()) && g_dIn.AOn(iMzOutExist02)) || g_opr.isDryRun)
				{
					ExistBuffer1() = FALSE;
					ExistBuffer2() = TRUE;
					g_lotInfo.LotInfoCopy(LOT_INFO_CONV_BUFFER1, LOT_INFO_CONV_BUFFER2);
					if(ExistArrival())
					{
						if((!g_dIn.AOn(iMzOutArrival) && g_dIn.AOn(iMzOutExist01)) || g_opr.isDryRun)
						{
							ExistArrival() = FALSE;
							ExistBuffer1() = TRUE;
							g_lotInfo.LotInfoCopy(LOT_INFO_CONV_ARRIVAL, LOT_INFO_CONV_BUFFER1);
							g_lotInfo.LotInfoClear(LOT_INFO_CONV_ARRIVAL);
							m_fsm.Set(C_END);
						}
					}
					else
					{
						m_fsm.Set(C_END);
					}
				}
			}
			else
			{
				m_pCylStopperL->Actuate(pmFWD);
				m_pCylStopperR->Actuate(pmFWD);
			}
		}
		else if(msgBufferOut == nMsg)
		{
			// 마지막의 경우에만 로그 생성
			if(!g_MgzLoadZ.IsReadyLoadOut())
			{
				BeltRun(FALSE);
				g_MgzLoadZ.BeltRun(FALSE);
				break;
			}

			if(pmDOWN != g_MgzLoadZ.m_pCylStopperUD->GetPos(300))
			{
				g_MgzLoadZ.m_pCylStopperUD->Actuate(pmDOWN);

				if(!g_logChk.bFunction[g_MgzLoadZ.m_pCylStopperUD->GetNo()])
				{
					g_logChk.bFunction[g_MgzLoadZ.m_pCylStopperUD->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"CYL_MGZ_LOAD_STOPPER_DOWN_OUT_CONV", g_data2c.cEtc.start, 
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
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"CYL_MGZ_LOAD_STOPPER_DOWN_OUT_CONV", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[g_MgzLoadZ.m_pCylStopperUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[g_MgzLoadZ.m_pCylStopperUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
														g_data2c.cPmIO[g_MgzLoadZ.m_pCylStopperUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
				}
			}

			if(pmBWD != m_pCylStopperL->GetPos(300) || pmBWD != m_pCylStopperR->GetPos(300))
			{
				m_pCylStopperL->Actuate(pmBWD);
				m_pCylStopperR->Actuate(pmBWD);

				if(!g_logChk.bFunction[m_pCylStopperL->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperL->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"CYL_MGZ_OUT_STOPPER_LEFT_BWD", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperL->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperL->GetNo()].In[pmBWD][pmOFF], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylStopperL->GetNo()].In[pmBWD][pmON], g_data2c.cEtc.off))
				}
				if(!g_logChk.bFunction[m_pCylStopperR->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperR->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"CYL_MGZ_OUT_STOPPER_RIGHT_BWD", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperR->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperR->GetNo()].In[pmBWD][pmOFF], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylStopperR->GetNo()].In[pmBWD][pmON], g_data2c.cEtc.off))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylStopperL->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperL->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"CYL_MGZ_OUT_STOPPER_LEFT_BWD", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperL->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperL->GetNo()].In[pmBWD][pmOFF], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylStopperL->GetNo()].In[pmBWD][pmON], g_data2c.cEtc.on))
				}
				if(g_logChk.bFunction[m_pCylStopperR->GetNo()])
				{
					g_logChk.bFunction[m_pCylStopperR->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"CYL_MGZ_OUT_STOPPER_RIGHT_BWD", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperR->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperR->GetNo()].In[pmBWD][pmOFF], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylStopperR->GetNo()].In[pmBWD][pmON], g_data2c.cEtc.on))
				}
			}

			if(m_fsm.Once())
			{
				BeltRun(TRUE);
				g_MgzLoadZ.BeltRun(TRUE,TRUE);

				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"AC_MGZ_OUT_BELT_RUN", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.acMotor, g_data2c.cEtc.actName, 
													g_data2c.cAcMtName[1], g_data2c.cAcIO[1], g_data2c.cEtc.on))
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"AC_MGZ_LOAD_BELT_RUN_CCW", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.acMotor, g_data2c.cEtc.actName,
													g_data2c.cAcMtName[2], g_data2c.cAcIO[2], g_data2c.cEtc.on))
				m_bMzOutSenOn = FALSE;
			}
			else
			{
				if((g_dIn.AOn(iMzLoadZMid) && !m_bMzOutSenOn) || g_opr.isDryRun)
				{
					m_bMzOutSenOn = TRUE;
					BeltRun(FALSE);
				}
				else if(!g_dIn.AOn(iMzOutExist03) && g_dIn.AOn(iMzLoadZExist) && m_bMzOutSenOn)
				{
					g_MgzLoadZ.BeltRun(FALSE,FALSE);
					g_MgzLoadZ.ExistLoadExist() = TRUE;

					m_pCylStopperL->Actuate(pmFWD);
					m_pCylStopperR->Actuate(pmFWD);
					ExistBuffer2() = FALSE;
					g_lotInfo.LotInfoClear(LOT_INFO_CONV_BUFFER2);

					BeltRun(FALSE);
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"AC_MGZ_OUT_BELT_RUN", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.acMotor, g_data2c.cEtc.actName, 
														g_data2c.cAcMtName[1], g_data2c.cAcIO[1], g_data2c.cEtc.off))
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"AC_MGZ_LOAD_BELT_RUN_CCW", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.acMotor, g_data2c.cEtc.actName,
														g_data2c.cAcMtName[2], g_data2c.cAcIO[2], g_data2c.cEtc.off))

					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"CYL_MGZ_OUT_STOPPER_LEFT_FWD", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperL->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperL->GetNo()].In[pmFWD][pmON], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylStopperL->GetNo()].In[pmFWD][pmOFF], g_data2c.cEtc.on))
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"CYL_MGZ_OUT_STOPPER_RIGHT_FWD", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperR->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperR->GetNo()].In[pmFWD][pmON], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylStopperR->GetNo()].In[pmFWD][pmOFF], g_data2c.cEtc.on))
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"CYL_MGZ_OUT_STOPPER_LEFT_FWD", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperL->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperL->GetNo()].In[pmFWD][pmON], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylStopperL->GetNo()].In[pmFWD][pmOFF], g_data2c.cEtc.off))
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cMzOutConv.deviceId, L"CYL_MGZ_OUT_STOPPER_RIGHT_FWD", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylStopperR->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylStopperR->GetNo()].In[pmFWD][pmON], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylStopperR->GetNo()].In[pmFWD][pmOFF], g_data2c.cEtc.off))

					if(ExistBuffer1())
					{
						m_fsm.Set(C_START, msgBuffer2Empty);
					}
					else if(ExistArrival())
					{
						m_fsm.Set(C_START, msgBuffer1Empty);
					}
					else
					{
						m_fsm.Set(C_END);
					}

				}
			}
		}
		break;
	case C_END:
		BeltRun(FALSE);
		g_MgzLoadZ.BeltRun(FALSE);

		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cMzOutConv.deviceId, L"MOVE_MGZ", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"MGZ_OUT_ARRIVAL", L"MGZ_LOAD"))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CLdMzOutConv::CycleConvMoveManual(void)
{
	if(!m_fsm.Between(C_MANUAL_START, C_MANUAL_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_MZ_OUT_MANUAL_CYCLE_TM_OVER);
		return;
	}

	switch(m_fsm.Get())
	{
	case C_MANUAL_START:
		if(g_opr.isDoorOpen)
		{
			BeltRun(FALSE);
			m_fsm.Set(C_MANUAL_PAUSED_STOP);
			break;
		}
		BeltRun(TRUE, TRUE);
		m_fsm.Set(C_MANUAL_01);
		break;
	case C_MANUAL_PAUSED_STOP:
		if(!g_opr.isDoorOpen)
			m_fsm.Set(C_MANUAL_START);
		break;
	case C_MANUAL_01:
		if(g_opr.isDoorOpen)
		{
			BeltRun(FALSE);
			m_fsm.Set(C_MANUAL_PAUSED_STOP);
			break;
		}

		// 무조건 10초 Running
		if(!m_fsm.Delay(15000))
			break;
		m_fsm.Set(C_MANUAL_END);
		break;
	case C_MANUAL_END:
		BeltRun(FALSE);
		m_fsm.Set(C_IDLE);
		break;
	}
}
