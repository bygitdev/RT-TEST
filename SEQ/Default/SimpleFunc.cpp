#include "..\def\Includes.h"



//-------------------------------------------------------------------
void SimpleFunc(void)
{
	UpdateForMMI();
	OptionCheck();

	EctRst();

	// Index Safety Output
	InterfaceAllOff(FALSE);

	// Index Safety Output
	for(int nCnt = INDEX_01; nCnt <= INDEX_04; nCnt++)
		SetSorterSefetyIndex(nCnt);

	SetSorterSefetyPnp();
}


//------------------------------------------------------------------
void UpdateForMMI(void)
{
	static CTimer tmRunDown;

	if(!g_opr.isAuto)
		tmRunDown.Reset();

	if(g_opr.isStop)
	{
		if(g_opr.isCycleRun)
			g_pNV->NDm(stateMachine) = MC_STATE_CYCLE;
		else if(0 < g_err.GetNo())
			g_pNV->NDm(stateMachine) = MC_STATE_ERROR;
		else
			g_pNV->NDm(stateMachine) = MC_STATE_STOP;
	}
	else if(g_opr.isAuto)
	{
		if (!g_opr.isCycleRun)
		{
			if (tmRunDown.TmOver(60000))
			{
				g_pNV->NDm(stateMachine) = MC_STATE_RUN_DOWN;

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
			}
			//
		}
		else
		{
			tmRunDown.Reset();
			g_pNV->NDm(stateMachine) = MC_STATE_AUTO;
		}
	}
	else if(g_opr.isEmg)
		g_pNV->NDm(stateMachine) = MC_STATE_EMG;

	// 시간당 생산량 
	g_pNV->DDm(uph) = g_pNV->Pkg(unitCnt) / g_pNV->DDm(cycleTmOutPnp) * 3600.0;
	
	// pio sensor mmi display (설비별로 io no가 틀리므로 ndm으로 구성)
// 	g_pNV->NDm(pioDisplayiOhtValId)	= g_dIn.AOn(iOhtValId);
// 	g_pNV->NDm(pioDisplayiOhtCs0)	= g_dIn.AOn(iOhtCs0);
// 	g_pNV->NDm(pioDisplayiOhtCs1)	= g_dIn.AOn(iOhtCs1);
// 	g_pNV->NDm(pioDisplayiOhtCs2)	= g_dIn.AOn(iOhtCs2);
// 	g_pNV->NDm(pioDisplayiOhtCs3)	= g_dIn.AOn(iOhtCs3);
// 	g_pNV->NDm(pioDisplayiOhtTrReq)	= g_dIn.AOn(iOhtTrReq);
// 	g_pNV->NDm(pioDisplayiOhtBusy)	= g_dIn.AOn(iOhtBusy);
// 	g_pNV->NDm(pioDisplayiOhtCompt)	= g_dIn.AOn(iOhtCompt);
// 
// 	g_pNV->NDm(pioDisplayoOhtLdReq)	= g_dOut.IsOn(oOhtLdReq);
// 	g_pNV->NDm(pioDisplayoOhtUldReq)= g_dOut.IsOn(oOhtUldReq);
// 	g_pNV->NDm(pioDisplayoOhtAbort)	= g_dOut.IsOn(oOhtAbort);
// 	g_pNV->NDm(pioDisplayoOhtReady)	= g_dOut.IsOn(oOhtReady);
// 
// 	if(g_opr.isStop)
// 	{
// 		if(g_pNV->NDm(pioOutputoOhtLdReq))
// 		{
// 			g_pNV->NDm(pioOutputoOhtLdReq) = 0;
// 			g_dOut.Set(oOhtLdReq, !g_dOut.IsOn(oOhtLdReq));
// 		}
// 		if(g_pNV->NDm(pioOutputoOhtUldReq))
// 		{
// 			g_pNV->NDm(pioOutputoOhtUldReq) = 0;
// 			g_dOut.Set(oOhtUldReq, !g_dOut.IsOn(oOhtUldReq));
// 		}
// 		if(g_pNV->NDm(pioOutputoOhtAbort))
// 		{
// 			g_pNV->NDm(pioOutputoOhtAbort) = 0;
// 			g_dOut.Set(oOhtAbort, !g_dOut.IsOn(oOhtAbort));
// 		}
// 		if(g_pNV->NDm(pioOutputoOhtReady))
// 		{
// 			g_pNV->NDm(pioOutputoOhtReady) = 0;
// 			g_dOut.Set(oOhtReady, !g_dOut.IsOn(oOhtReady));
// 		}
// 
// 		if(g_pNV->NDm(pioOutputReset))
// 		{
// 			g_pNV->NDm(pioOutputReset) = 0;
// 			g_dOut.Off(oOhtLdReq);
// 			g_dOut.Off(oOhtUldReq);
// 			g_dOut.Off(oOhtAbort);
// 			g_dOut.Off(oOhtReady);
// 		}
// 	}

}


//-------------------------------------------------------------------
void EctRst(void)
{
	static CTimer tmrSpindleRunF;
	static CTimer tmrSpindleRunR;
	if(g_dOut.IsOn(oSpindleRunF_01))
	{
		if(tmrSpindleRunF.TmOver(60000)){
			tmrSpindleRunF.Reset();
			g_pNV->DDm(SpindelTimeCountF)++;
		}
	}
	else
	{
		if(tmrSpindleRunF.TmOver(30000)){ //반올림.. ^^
			g_pNV->DDm(SpindelTimeCountF)++;
		}
		tmrSpindleRunF.Reset();
	}

	if(g_dOut.IsOn(oSpindleRunR_01))
	{
		if(tmrSpindleRunR.TmOver(60000)){
			tmrSpindleRunR.Reset();
			g_pNV->DDm(SpindelTimeCountR)++;
		}
	}
	else
	{
		if(tmrSpindleRunR.TmOver(30000)){ //반올림.. ^^
			g_pNV->DDm(SpindelTimeCountR)++;
		}
		tmrSpindleRunR.Reset();
	}

	if(g_pNV->DDm(SpindelTimeCountF)>60)
	{
		g_pNV->DDm(SpindelTimeCountF) = 0;
		g_pNV->DDm(ActBrush01)++;
	}
	if(g_pNV->DDm(SpindelTimeCountR)>60)
	{
		g_pNV->DDm(SpindelTimeCountR) = 0;
		g_pNV->DDm(ActBrush02)++;
	}

	if(g_pNV->DDm(ActBrush01) > g_pNV->DDm(SetBlush01)){
		g_wr.Save(WR_SPINDLE_TIME_OVER_F);
	}
	if(g_pNV->DDm(ActBrush02) > g_pNV->DDm(SetBlush02)){
		g_wr.Save(WR_SPINDLE_TIME_OVER_R);
	}

	if(g_pNV->DDm(ActOutPnpPadLifeCount) > g_pNV->DDm(SetOutPnpPadLifeCount)) 
	{
		g_wr.Save(WR_OUTPNP_PAD_LIFE_COUNT_OVER);
	}

	static CTimer tmrViTcpReconnect;
	//CHECK PARA NO
	if(1 > g_pNV->NDm(jogSpeed))
		g_pNV->NDm(jogSpeed) = 1;
	
	if(g_pNV->gerberPara(arrayPathCnt) < 1)
		g_pNV->gerberPara(arrayPathCnt) = 1;
	if(g_pNV->gerberPara(arrayXCnt) < 1)
		g_pNV->gerberPara(arrayXCnt) = 1;
	if(g_pNV->gerberPara(arrayYCnt) < 1)
		g_pNV->gerberPara(arrayYCnt) = 1;
	
	// Cutting Velocity + Live Vision
	if(g_pNV->DDm(routerVelStart) < 1)
		g_pNV->DDm(routerVelStart) = 1;
	if(g_pNV->DDm(routerVelEnd) < 1)
		g_pNV->DDm(routerVelEnd) = 1;
	if(g_pNV->DDm(routerVelViStart) < 1)
		g_pNV->DDm(routerVelViStart) = 1;
	if(g_pNV->DDm(routerVelViEnd) < 1)
		g_pNV->DDm(routerVelViEnd) = 1;
	
	if(g_pNV->DDm(lotMergeLimitQty) < 1)
		g_pNV->DDm(lotMergeLimitQty) = 1200;

	if(g_pNV->Pkg(bitZStepCnt) < 1)
		g_pNV->Pkg(bitZStepCnt) = 1;

	// Bit Box Off -> On 시에 Bit Supply Box = 49 로 셋팅
	if(!g_opr.isDryRun)
	{
		if(g_dIn.AOn(iIndexBitSupplyBoxExist01))
		{
			if(g_pNV->NDm(RedindexBitSupplyBoxClear01))
			{
				g_pNV->NDm(RedindexBitSupplyBoxClear01) = FALSE;
				g_pNV->NDm(RedindexBitBoxCurCnt01) = (int)(g_pNV->DDm(indexBitBoxXCnt) * g_pNV->DDm(indexBitBoxYCnt)) - 1; //49
			}
		}
		else
		{
			g_pNV->NDm(RedindexBitSupplyBoxClear01) = TRUE;
		}

		if (g_dIn.AOn(iIndexBitSupplyBoxExist02))
		{
			if (g_pNV->NDm(BlueindexBitSupplyBoxClear02))
			{
				g_pNV->NDm(BlueindexBitSupplyBoxClear02) = FALSE;
				g_pNV->NDm(BlueindexBitBoxCurCnt02) = (int)(g_pNV->DDm(indexBitBoxXCnt) * g_pNV->DDm(indexBitBoxYCnt)) - 1; //49
			}
		}
		else
		{
			g_pNV->NDm(BlueindexBitSupplyBoxClear02) = TRUE;
		}

		if (g_dIn.AOn(iIndexBitSupplyBoxExist03))
		{
			if (g_pNV->NDm(BlueindexBitSupplyBoxClear03))
			{
				g_pNV->NDm(BlueindexBitSupplyBoxClear03) = FALSE;
				g_pNV->NDm(BlueindexBitBoxCurCnt03) = (int)(g_pNV->DDm(indexBitBoxXCnt) * g_pNV->DDm(indexBitBoxYCnt)) - 1; //49
			}
		}
		else
		{
			g_pNV->NDm(BlueindexBitSupplyBoxClear03) = TRUE;
		}

		if(g_dIn.AOn(iIndexBitSupplyBoxExist04))
		{
			if(g_pNV->NDm(RedindexBitSupplyBoxClear04))
			{
				g_pNV->NDm(RedindexBitSupplyBoxClear04) = FALSE;
				g_pNV->NDm(RedindexBitBoxCurCnt04) = (int)(g_pNV->DDm(indexBitBoxXCnt) * g_pNV->DDm(indexBitBoxYCnt)) - 1; //49
			}
		}
		else
		{
			g_pNV->NDm(RedindexBitSupplyBoxClear04) = TRUE;
		}
	}

	if(!g_opr.isAuto)
	{
		if(g_pNV->NDm(flagAllLotClear))
		{
			SeqLog(L"Event : LotHistoryAllClear (ndm330) !!!");
		
			g_pNV->NDm(flagAllLotClear) = 0;
			g_pNV->NDm(flagSplitIDLotStart) = FALSE;
			g_pNV->NDm(flagSplitInfo) = FALSE;
			g_pNV->NDm(flagLotMergeComp) = FALSE;
			g_pNV->NDm(lotSplitCount) = 0;

			g_lotInfo.LotInfoAllClear();
			g_lotInfo.LotHistoryAllClear();
			g_lotInfo.LotSplitAllClear();
		}
	}
	else
	{
		g_pNV->NDm(flagAllLotClear) = 0;
	}

	// Vision TCP Reconnect
	if(g_pNV->NDm(flagViTcpReconnect))
	{
		g_pNV->NDm(flagViTcpReconnect) = 0;
		g_dOut.On(oViTcpReconnect);
		tmrViTcpReconnect.Reset();
	}

	if(tmrViTcpReconnect.TmOver(200))
	{
		g_dOut.Off(oViTcpReconnect);
	}



	/////////////////////////////////////////////////////////////////////
	// 설비 Stop 중에 Door Open 이면 Spindle Stop
	// Stop 중에도 Cycle 중이면 Spindle이 부러지므로 컨셉 수정
	// Router 중에는 Router에서 Stop
	if(g_opr.isDoorOpen && !g_routerF.m_fsm.Between(CRouter::C_ROUTER_START, CRouter::C_ROUTER_END))
	{
		g_routerF.m_pSpindleF->Actuate(pmOFF);
		g_routerF.m_pSpindleR->Actuate(pmOFF);
	}
	if(g_opr.isDoorOpen && !g_routerR.m_fsm.Between(CRouter::C_ROUTER_START, CRouter::C_ROUTER_END))
	{
		g_routerR.m_pSpindleF->Actuate(pmOFF);
		g_routerR.m_pSpindleR->Actuate(pmOFF);
	}

}


//-------------------------------------------------------------------
void OptionCheck(void)
{
	//////////////////////////////////////////////////////////////////
	// Light
	g_dOut.Off(oLight);
	if(g_pNV->UseSkip(usLight))
		g_dOut.On(oLight);
	else if(g_opr.isStop)
		g_dOut.On(oLight);

	//////////////////////////////////////////////////////////////////
	// Ionizer
	if(g_opr.isAuto && !g_opr.isDryRun)
	{
		if(g_pNV->UseSkip(usIonizer))
		{
			if(g_dIn.BOn(iIonizerAlarm01))
				g_err.Save(ER_ROUTER_IONIZER_01);
			else
				g_err.Del(ER_ROUTER_IONIZER_01);

			if(g_dIn.BOn(iIonizerAlarm02))
				g_err.Save(ER_ROUTER_IONIZER_02);
			else
				g_err.Del(ER_ROUTER_IONIZER_02);

			if(g_dIn.BOn(iIonizerAlarm03))
				g_err.Save(ER_ROUTER_IONIZER_03);
			else
				g_err.Del(ER_ROUTER_IONIZER_03);

			if(g_dIn.BOn(iIonizerAlarm04))
				g_err.Save(ER_ROUTER_IONIZER_04);
			else
				g_err.Del(ER_ROUTER_IONIZER_04);
		}
	}

	if(g_pNV->Pkg(bitZStepCnt) < 1.0)
		g_pNV->Pkg(bitZStepCnt) = 1;

	//////////////////////////////////////////////////////////////////
	// Tc Server를 사용시에 Sorter에 자동으로 IO 전송한다.
	if(g_pNV->UseSkip(usTcServer))
		g_dOut.On(oSorterTcServer);
	else
		g_dOut.Off(oSorterTcServer);

	//////////////////////////////////////////////////////////////////
	// Auto Kit Change 신호
	// OutPnp Kit 진입시 안전 조건처리를 위해 IO 추가
	if(g_pNV->NDm(mmiBtnAdcMode))
		g_dOut.On(oSorterRouterAdcMode);
	else
		g_dOut.Off(oSorterRouterAdcMode);


	//////////////////////////////////////////////////////////////////
	// ESD Check를 1일 1회 (오전 6:00 Reset) 진행하라는 mmi의 신호
	if(g_pNV->NDm(mmiNeedSpindleESD))
	{
		g_pNV->NDm(mmiNeedSpindleESD) = 0;

		g_pNV->NDm(flagSpindleESDCheck01) = 1;
		g_pNV->NDm(flagSpindleESDCheck02) = 1;
		g_pNV->NDm(flagSpindleESDCheck03) = 1;
		g_pNV->NDm(flagSpindleESDCheck04) = 1;
	}

	//////////////////////////////////////////////////////////////////
	// FDC Drop시에 EQP Stop 기능 (mmi->seq)
	if(g_pNV->UseSkip(usSecsGem))
	{
		if(g_pNV->NDm(gemRemoteStop))
		{
			g_pNV->NDm(gemRemoteStop) = STATE_IDLE;
			g_err.Save(ER_FDC_REMOTE_STOP);
		}
	}


}


//-------------------------------------------------------------------
void InterfaceAllOff(BOOL isRealTime)
{
	g_dOut.Off(oSorterAutoRun, isRealTime);
	//g_dOut.Off(oSorterError, isRealTime); 삭제
	g_dOut.Off(oSorterStageReq01, isRealTime);
	g_dOut.Off(oSorterStageBusy01, isRealTime);
	g_dOut.Off(oSorterStageSafety01, isRealTime);
	g_dOut.Off(oSorterStageReq02, isRealTime);
	g_dOut.Off(oSorterStageBusy02, isRealTime);
	g_dOut.Off(oSorterStageSafety02, isRealTime);
	g_dOut.Off(oSorterStageReq03, isRealTime);
	g_dOut.Off(oSorterStageBusy03, isRealTime);
	g_dOut.Off(oSorterStageSafety03, isRealTime);
	g_dOut.Off(oSorterStageReq04, isRealTime);
	g_dOut.Off(oSorterStageBusy04, isRealTime);
	g_dOut.Off(oSorterStageSafety04, isRealTime);
	g_dOut.Off(oSorterPickerReq, isRealTime);
	g_dOut.Off(oSorterPickerBusy, isRealTime);
	g_dOut.Off(oSorterPickerSafety0102, isRealTime);
	g_dOut.Off(oSorterPickerSafety0304, isRealTime);
	g_dOut.Off(oSorterStageNoL, isRealTime);		
	g_dOut.Off(oSorterStageNoH, isRealTime);	
	g_dOut.Off(oSorterRouterAllEmpty, isRealTime);
}


//-------------------------------------------------------------------
void ChkPkgData(PKG pkgNo, double min, double max)
{
	if(min > g_pNV->Pkg(pkgNo))
		g_pNV->Pkg(pkgNo) = min;
	else if(max < g_pNV->Pkg(pkgNo))
		g_pNV->Pkg(pkgNo) = max;
}


//-------------------------------------------------------------------
BOOL SetSorterSefetyIndex(int nIdx)
{
	BOOL bSafety  = g_pIndex[nIdx]->m_pMtX->m_state.isHome;
		 bSafety &= g_pIndex[nIdx]->m_pMtX->m_profile.cmdIndex != (CIndex::PX_OUT_PNP);
		 bSafety &= g_pIndex[nIdx]->m_pMtX->m_profile.curIndex != (CIndex::PX_OUT_PNP);

	if(INDEX_01 == nIdx)
	{
		g_dOut.Off(oSorterStageSafety01);
		g_dOut.On(oSorterStageBusy01);
		if(bSafety)
		{
			g_dOut.On(oSorterStageSafety01);
			g_dOut.Off(oSorterStageBusy01);
		}
		else
		{
			g_dOut.Off(oSorterStageSafety01);
			g_dOut.On(oSorterStageBusy01);
		}
	}
	else if(INDEX_02 == nIdx)
	{
		g_dOut.Off(oSorterStageSafety02);
		g_dOut.On(oSorterStageBusy02);
		if(bSafety)
		{
			g_dOut.On(oSorterStageSafety02);
			g_dOut.Off(oSorterStageBusy02);
		}
		else
		{
			g_dOut.Off(oSorterStageSafety02);
			g_dOut.On(oSorterStageBusy02);
		}
	}
	else if(INDEX_03 == nIdx)
	{
		g_dOut.Off(oSorterStageSafety03);
		g_dOut.On(oSorterStageBusy03);
		if(bSafety)
		{
			g_dOut.On(oSorterStageSafety03);
			g_dOut.Off(oSorterStageBusy03);
		}
		else
		{
			g_dOut.Off(oSorterStageSafety03);
			g_dOut.On(oSorterStageBusy03);
		}

	}
	else if(INDEX_04 == nIdx)
	{
		g_dOut.Off(oSorterStageSafety04);
		g_dOut.On(oSorterStageBusy04);
		if(bSafety)
		{
			g_dOut.On(oSorterStageSafety04);
			g_dOut.Off(oSorterStageBusy04);
		}
		else
		{
			g_dOut.Off(oSorterStageSafety04);
			g_dOut.On(oSorterStageBusy04);
		}
	}

	return (TRUE);
}


BOOL SetSorterSefetyPnp()
{
	BOOL bSafetyPnp  = g_outPnp.m_pMtX->IsRdy();
		 bSafetyPnp &= g_outPnp.m_pMtX->InPos(COutPnp::PX_READY);
		 bSafetyPnp &= !g_outPnp.m_fsm.Between(COutPnp::C_PUTDN_START, COutPnp::C_PUTDN_END);

	g_dOut.On(oSorterPickerBusy);
	g_dOut.Off(oSorterPickerSafety0102);
	g_dOut.Off(oSorterPickerSafety0304);
	
	if(bSafetyPnp)
	{
		g_dOut.Off(oSorterPickerBusy);
		g_dOut.On(oSorterPickerSafety0102);
		g_dOut.On(oSorterPickerSafety0304);
	}
	
	return (TRUE);
}
