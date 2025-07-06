#include "..\def\Includes.h"

/////////////////////////////////////////////////////////////////////
CTenkeyOpr g_tenkeyOpr;
/////////////////////////////////////////////////////////////////////


enum enJogCmd
{
	C_JOGWAIT	= 100,
	C_JOGMOVE_N	= 101,
	C_JOGMOVE_P	= 102,
	C_JOGSTOP	= 103,
};

//-------------------------------------------------------------------
void CTenkeyOpr::Run(void)
{
	Cycle();
	Auto();
}


//-------------------------------------------------------------------
BOOL CTenkeyOpr::DoorSafety(int nVal)
{
//#if(DOOR_BYPASS)
//	return (TRUE);
//#endif//DOOR_BYPASS

	// Door Open 시에 Cylinder 동작은 가능 하도록 함
	if(g_opr.isDoorOpen)
	{
		if(nVal < 0)
			return (TRUE);

		switch(nVal)
		{
			case HOME(2):  // LdMz Pusher 동작
			case STEP(2):  // LdMz Pusher 동작
			case HOME(3):  // LdMz Clamp Aligner 동작 
			case STEP(3):  // LdMz Clamp Aligner 동작
			case HOME(4):  // LdMz Clamp OpenClose 동작
			case STEP(4):  // LdMz Clamp OpenClose 동작

			case HOME(21): // Index 01 Mask Kit Up/Down
			case STEP(21): // Index 01 Mask Kit Up/Down
			case HOME(22): // Index 01 Mask Kit Fwd/Bwd
			case STEP(22): // Index 01 Mask Kit Fwd/Bwd
			case HOME(23): // Index 01 Stage Kit Open/Close
			case STEP(23): // Index 01 Stage Kit Open/Close
			case HOME(26):
			case STEP(26):


			case HOME(31): // Index 02 Mask Kit Up/Down
			case STEP(31): // Index 02 Mask Kit Up/Down
			case HOME(32): // Index 02 Mask Kit Fwd/Bwd
			case STEP(32): // Index 02 Mask Kit Fwd/Bwd
			case HOME(33): // Index 02 Stage Kit Open/Close
			case STEP(33): // Index 02 Stage Kit Open/Close
			case HOME(36):
			case STEP(36):

			case HOME(41): // Index 03 Mask Kit Up/Down
			case STEP(41): // Index 03 Mask Kit Up/Down
			case HOME(42): // Index 03 Mask Kit Fwd/Bwd
			case STEP(42): // Index 03 Mask Kit Fwd/Bwd
			case HOME(43): // Index 03 Stage Kit Open/Close
			case STEP(43): // Index 03 Stage Kit Open/Close
			case HOME(46):
			case STEP(46):

			case HOME(51): // Index 04 Mask Kit Up/Down
			case STEP(51): // Index 04 Mask Kit Up/Down
			case HOME(52): // Index 04 Mask Kit Fwd/Bwd
			case STEP(52): // Index 04 Mask Kit Fwd/Bwd
			case HOME(53): // Index 04 Stage Kit Open/Close
			case STEP(53): // Index 04 Stage Kit Open/Close
			case HOME(56):
			case STEP(56):

			case HOME(61): // Spindle 01 Bit Clamp Front
			case STEP(61): // Spindle 01 Bit Clamp Front
			case HOME(62): // Spindle 02 Bit Clamp Rear
			case STEP(62): // Spindle 02 Bit Clamp Rear

			case HOME(71): // Spindle 03 Bit Clamp Front
			case STEP(71): // Spindle 03 Bit Clamp Front
			case HOME(72): // Spindle 04 Bit Clamp Rear
			case STEP(72): // Spindle 04 Bit Clamp Rear

			case HOME(81): // Outpnp Vac On/Off
			case STEP(81): // Outpnp Vac On/Off
			case HOME(82): // Outpnp Kit Open/Close
			case STEP(82): // Outpnp Kit Open/Close
			
				return (TRUE);

			break;
		}

		g_err.Door(FALSE);
		return (FALSE);
	}
	else
	{
		return (TRUE);
	}

	return (FALSE);
}


//-------------------------------------------------------------------
void CTenkeyOpr::Auto(void)
{
	BOOL shouldTenkeyOff  = (!g_opr.isStop || g_opr.isEmg || g_opr.isCycleRun);
	     shouldTenkeyOff |= g_pNV->NDm(tenKeyJog);
	     shouldTenkeyOff |= (g_allHome.m_fsm.IsRun() && (CAllHome::hmAllHome == g_allHome.m_fsm.GetMsg()));
		
	if(shouldTenkeyOff)
	{
		m_bDisable = TRUE;
		m_tenkey.Disable();
	}
	else
	{
		if(TRUE == g_pNV->NDm(tenKeyJog))
		{
			m_bDisable = TRUE;
			m_tenkey.Disable();
		}
		else if(TRUE == m_bDisable)
		{
			m_bDisable = FALSE;
			m_tenkey.Enable();
		}
	}


	int nTenkeyNo = m_tenkey.GetTenkeyNo();

	//mmi tenkey 사용
	if(nTenkeyNo == NO_KEY && !shouldTenkeyOff)
	{
		if(0 < g_pNV->NDm(screenTenkey))
			nTenkeyNo = g_pNV->NDm(screenTenkey);
	}
	g_pNV->NDm(screenTenkey) = NO_KEY;

	if(m_fsm.IsRun() || shouldTenkeyOff || g_opr.isPausedStop)
		return;

	if(FALSE == DoorSafety(nTenkeyNo))
	{
		nTenkeyNo = NO_KEY;
	}

	switch(nTenkeyNo)
	{
		 // Machine Reset
		case HOME(0):
			break;
		case STEP(0):
			break;

		/////////////////////////////////////////////////////////////////////
		// Ld Mz    01: 
		/////////////////////////////////////////////////////////////////////
		
		// Home, Step
		case HOME(1):
			g_pm[CYL_RAIL_GRIPPER_FB].Actuate(pmBWD); 
			if(pmBWD != g_pm[CYL_RAIL_GRIPPER_FB].GetPos(300))
				break;

			g_ldMz.m_pMtX->CancelHomeSearch();
			g_ldMz.m_pMtY->CancelHomeSearch();
			g_ldMz.m_pMtZ->CancelHomeSearch();

			g_allHome.Start(CAllHome::hmLoader);
			break;
		case STEP(1):
			g_ldMz.m_bRun = TRUE;
			break;

		// LdMz Pusher 동작
		case HOME(2):
			g_ldMz.m_pMtX->Move(CLdMz::PX_BWD);
			break;
		case STEP(2):
			g_ldMz.MoveMtPusherXFwd();
			break;
			
		// LdMz Clamp Aligner 동작
		case HOME(3):
			g_ldMz.m_pCylAlignFB->Actuate(pmOFF);
			break;
		case STEP(3):
			g_ldMz.m_pCylAlignFB->Actuate(pmON);
			break;

		// LdMz Clamp OpenClose 동작
		case HOME(4):
			g_ldMz.m_pCylClampOC->Actuate(pmOPEN);
			break;
		case STEP(4):
			g_ldMz.m_pCylClampOC->Actuate(pmCLOSE);
			break;

		/////////////////////////////////////////////////////////////////////
		// In Rail		5
		/////////////////////////////////////////////////////////////////////

		// Home, Step
		case HOME(5):
			g_rail.m_pMtGrip->CancelHomeSearch();
			g_allHome.Start(CAllHome::hmRail);
			break;
		case STEP(5):
			g_rail.m_bRun = TRUE;
			break;
		case HOME(6):
			g_rail.m_pCylGripOC->Actuate(pmOPEN);
			break;
		case STEP(6):
			g_rail.m_pCylGripOC->Actuate(pmCLOSE);
			break;


		// MGZ 강제 배출 // 텐키 자리가 없음
		case HOME(8):
			g_ldMzInConv.m_bRun = TRUE;
			g_ldMz.m_bRun = TRUE;
			g_ldMzOutConv.m_bRun = TRUE;
			g_MgzLoadZ.m_bRun = TRUE;
			break;
		case STEP(8):
			if(!g_ldMz.Exist() || !g_dIn.AOn(iMzClampLExist) || !g_dIn.AOn(iMzClampRExist))
				break;
			if(g_rail.Exist() || g_dIn.AOn(iRailExistStart) || g_dIn.BOn(iMzClampPcbJam) || g_dIn.AOn(iRailGripperExist))
				break;
			if(pmCLOSE != g_ldMz.m_pCylClampOC->GetPos(300)) 
				break;
			if(!g_ldMz.m_pMtX->InPos(CLdMz::PX_BWD))
				break;
			if(g_ldMz.m_pMtY->InPos(CLdMz::PY_RCV))
				break;
			if(g_ldMz.m_pMtY->InPos(CLdMz::PY_EJECT))
				break;
			if(!g_ldMzOutConv.IsReadyMzOut())
				break;

			g_ldMz.m_bManualOut = TRUE;
			g_ldMz.m_fsm.Set(CLdMz::C_EJECT_START);
			break;
			
		/////////////////////////////////////////////////////////////////////
		// In Pnp		10 
		/////////////////////////////////////////////////////////////////////

		// Home, Step
		case HOME(10):
			g_inPnp.m_pMtY->CancelHomeSearch();
			g_inPnp.m_pMtZ->CancelHomeSearch();
			g_inPnp.m_pMtW->CancelHomeSearch();

			g_allHome.m_bInPnpError = TRUE;
			g_allHome.Start(CAllHome::hmInPnp);
			break;
		case STEP(10):
			g_inPnp.m_bRun = TRUE;
			break;
			
		/////////////////////////////////////////////////////////////////////
		// MGZ Load Z		15 
		/////////////////////////////////////////////////////////////////////

		case HOME(15):
			if(pmUP != g_ldMzInConv.m_pCylStopper01UD->GetPos(100))
			{
				g_ldMzInConv.m_pCylStopper01UD->Actuate(pmUP);
			}
			else
			{
				g_ldMzInConv.m_pCylStopper01UD->Actuate(pmDOWN);
			}
			break;
		case STEP(15):
			if(pmUP != g_ldMzInConv.m_pCylStopper02UD->GetPos(100))
			{
				g_ldMzInConv.m_pCylStopper02UD->Actuate(pmUP);
			}
			else
			{
				g_ldMzInConv.m_pCylStopper02UD->Actuate(pmDOWN);
			}
			break;

		case HOME(16):
			if(pmUP != g_ldMzInConv.m_pCylStopper03UD->GetPos(100))
			{
				g_ldMzInConv.m_pCylStopper03UD->Actuate(pmUP);
			}
			else
			{
				g_ldMzInConv.m_pCylStopper03UD->Actuate(pmDOWN);
			}
			break;
		case STEP(16):
			break;

		/////////////////////////////////////////////////////////////////////
		// Index01		20 
		/////////////////////////////////////////////////////////////////////
		case HOME(20):
			g_pIndex[INDEX_01]->m_pMtX->CancelHomeSearch();
			g_pIndex[INDEX_01]->m_pMtT->CancelHomeSearch();

			g_allHome.Start(CAllHome::hmIndex01);
			break;
		case STEP(20):
			g_pIndex[INDEX_01]->m_bRun = TRUE;
			break;

		// Index Mask Fix Up/Down
		case HOME(21):
			g_pIndex[INDEX_01]->SetCylMaskFixUD(pmUP);
			break;
		case STEP(21):
			g_pIndex[INDEX_01]->SetCylMaskFixUD(pmDOWN);
			break;

		// Index Mask Fix Fwd/Bwd
		case HOME(22):
			if(pmUP == g_pIndex[INDEX_01]->IsCylMaskFixUD(pmUP, 1000))
				g_pIndex[INDEX_01]->SetCylMaskFixFB(pmBWD);
			break;
		case STEP(22):
			if(pmUP == g_pIndex[INDEX_01]->IsCylMaskFixUD(pmUP, 1000))
				g_pIndex[INDEX_01]->SetCylMaskFixFB(pmFWD);
			break;

		// Index Stage Kit Open/Close
		case HOME(23):
			g_pIndex[INDEX_01]->m_pSolStageKitOC->Actuate(pmCLOSE);
			break;
		case STEP(23):
			g_pIndex[INDEX_01]->m_pSolStageKitOC->Actuate(pmOPEN);
			break;

		// Index Mask Picker Up/Down
		case HOME(24):
			g_pIndex[INDEX_01]->m_pCylMaskPickerUD->Actuate(pmUP);
			break;
		case STEP(24):
			if(g_pIndex[INDEX_01]->m_pMtX->IsRdy(CIndex::PX_MASK_PICKER)&&g_pIndex[INDEX_01]->m_pMtT->IsRdy(CIndex::PT_MASK_PICKER))
			{
				g_pIndex[INDEX_01]->m_pCylMaskPickerUD->Actuate(pmDOWN);
			}
			break;			
		// Index Mask Picker Open/Close
		case HOME(25):
			g_pIndex[INDEX_01]->m_pCylMaskPickerOC->Actuate(pmOPEN);
			break;
		case STEP(25):
			g_pIndex[INDEX_01]->m_pCylMaskPickerOC->Actuate(pmCLOSE);
			break;
			
		case HOME(26):
			g_pIndex[INDEX_01]->m_pCylDustShutterOC->Actuate(pmOPEN);
			break;

		case STEP(26):
			g_pIndex[INDEX_01]->m_pCylDustShutterOC->Actuate(pmCLOSE);
			break;
			
		// Index & Router Prs Pos Move
		case HOME(28):
			break;
		case STEP(28):
			{
				if(!g_routerF.m_pMtZ_F->IsRdy(CRouter::PZ_READY) && !g_routerF.m_pMtZ_R->IsRdy(CRouter::PZ_READY))
				{
					g_routerF.m_pMtZ_F->Move(CRouter::PZ_READY);
					g_routerF.m_pMtZ_R->Move(CRouter::PZ_READY);
					break;
				}

				if(!g_pIndex[INDEX_01]->CanMove(CIndex::PX_ROUTER_PRS))
					break;

				if(!g_pIndex[INDEX_01]->CylIndexMaskFixAct(pmCLOSE))
					break;

				static int nPrsBlockCnt_1 = 0;

				if(2 < nPrsBlockCnt_1)
					nPrsBlockCnt_1 = 0;

				g_routerF.m_pMtZ_F->Move(CRouter::PZ_PRS);
				POINT2D ptPos = g_pIndex[INDEX_01]->GetRouterPrsPos(nPrsBlockCnt_1);
				g_pIndex[INDEX_01]->m_pMtX->PMove(CIndex::PX_ROUTER_PRS, ptPos.dX);
				g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ROUTER_PRS);
				double dPosW = g_routerF.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				g_routerF.GentryMtYWPMove(CRouter::PY_VI_PRS_F, CRouter::PW_READY, ptPos.dY, dPosW);

				nPrsBlockCnt_1++;
			}
			break;

		/////////////////////////////////////////////////////////////////////
		// Index02		30 
		/////////////////////////////////////////////////////////////////////
		case HOME(30):
			g_pIndex[INDEX_02]->m_pMtX->CancelHomeSearch();
			g_pIndex[INDEX_02]->m_pMtT->CancelHomeSearch();

			g_allHome.Start(CAllHome::hmIndex01);
			break;
		case STEP(30):
			g_pIndex[INDEX_02]->m_bRun = TRUE;
			break;

		// Index Mask Fix Up/Down
		case HOME(31):
			g_pIndex[INDEX_02]->SetCylMaskFixUD(pmUP);
			break;
		case STEP(31):
			g_pIndex[INDEX_02]->SetCylMaskFixUD(pmDOWN);
			break;

		// Index Mask Fix Fwd/Bwd
		case HOME(32):
			if(pmUP == g_pIndex[INDEX_02]->IsCylMaskFixUD(pmUP, 1000))
				g_pIndex[INDEX_02]->SetCylMaskFixFB(pmBWD);
			break;
		case STEP(32):
			if(pmUP == g_pIndex[INDEX_02]->IsCylMaskFixUD(pmUP, 1000))
				g_pIndex[INDEX_02]->SetCylMaskFixFB(pmFWD);
			break;

		// Index Stage Kit Open/Close
		case HOME(33):
			g_pIndex[INDEX_02]->m_pSolStageKitOC->Actuate(pmCLOSE);
			break;
		case STEP(33):
			g_pIndex[INDEX_02]->m_pSolStageKitOC->Actuate(pmOPEN);
			break;

		// Index Mask Picker Up/Down
		case HOME(34):
			g_pIndex[INDEX_02]->m_pCylMaskPickerUD->Actuate(pmUP);
			break;
		case STEP(34):
			if(g_pIndex[INDEX_02]->m_pMtX->IsRdy(CIndex::PX_MASK_PICKER)&&g_pIndex[INDEX_02]->m_pMtT->IsRdy(CIndex::PT_MASK_PICKER))
			{
				g_pIndex[INDEX_02]->m_pCylMaskPickerUD->Actuate(pmDOWN);
			}
			
		// Index Mask Picker Open/Close
		case HOME(35):
			g_pIndex[INDEX_02]->m_pCylMaskPickerOC->Actuate(pmOPEN);
			break;
		case STEP(35):
			g_pIndex[INDEX_02]->m_pCylMaskPickerOC->Actuate(pmCLOSE);
			break;

		case HOME(36):
			g_pIndex[INDEX_02]->m_pCylDustShutterOC->Actuate(pmOPEN);
			break;

		case STEP(36):
			g_pIndex[INDEX_02]->m_pCylDustShutterOC->Actuate(pmCLOSE);
			break;

		// Index & Router Prs Pos Move
		case HOME(38):
			break;
		case STEP(38):
			{
				if(!g_routerF.m_pMtZ_F->IsRdy(CRouter::PZ_READY) && !g_routerF.m_pMtZ_R->IsRdy(CRouter::PZ_READY))
				{
					g_routerF.m_pMtZ_F->Move(CRouter::PZ_READY);
					g_routerF.m_pMtZ_R->Move(CRouter::PZ_READY);
					break;
				}

				if(!g_pIndex[INDEX_02]->CanMove(CIndex::PX_ROUTER_PRS))
					break;

				if(!g_pIndex[INDEX_02]->CylIndexMaskFixAct(pmCLOSE))
					break;

				static int nPrsBlockCnt_2 = 0;

				if(2 < nPrsBlockCnt_2)
					nPrsBlockCnt_2 = 0;

				g_routerF.m_pMtZ_F->Move(CRouter::PZ_PRS);
				POINT2D ptPos = g_pIndex[INDEX_02]->GetRouterPrsPos(nPrsBlockCnt_2);
				g_pIndex[INDEX_02]->m_pMtX->PMove(CIndex::PX_ROUTER_PRS, ptPos.dX);
				g_pIndex[INDEX_02]->m_pMtT->Move(CIndex::PT_ROUTER_PRS);
				double dPosW = g_routerF.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				g_routerF.GentryMtYWPMove(CRouter::PY_VI_PRS_R, CRouter::PW_READY, ptPos.dY, dPosW);

				nPrsBlockCnt_2++;
			}
			break;

		/////////////////////////////////////////////////////////////////////
		// Index03		40 
		/////////////////////////////////////////////////////////////////////
		case HOME(40):
			g_pIndex[INDEX_03]->m_pMtX->CancelHomeSearch();
			g_pIndex[INDEX_03]->m_pMtT->CancelHomeSearch();

			g_allHome.Start(CAllHome::hmIndex01);
			break;
		case STEP(40):
			g_pIndex[INDEX_03]->m_bRun = TRUE;
			break;

		// Index Mask Fix Up/Down
		case HOME(41):
			g_pIndex[INDEX_03]->SetCylMaskFixUD(pmUP);
			break;
		case STEP(41):
			g_pIndex[INDEX_03]->SetCylMaskFixUD(pmDOWN);
			break;

		// Index Mask Fix Fwd/Bwd
		case HOME(42):
			if(pmUP == g_pIndex[INDEX_03]->IsCylMaskFixUD(pmUP, 1000))
				g_pIndex[INDEX_03]->SetCylMaskFixFB(pmBWD);
			break;
		case STEP(42):
			if(pmUP == g_pIndex[INDEX_03]->IsCylMaskFixUD(pmUP, 1000))
				g_pIndex[INDEX_03]->SetCylMaskFixFB(pmFWD);
			break;

		// Index Stage Kit Open/Close
		case HOME(43):
			g_pIndex[INDEX_03]->m_pSolStageKitOC->Actuate(pmCLOSE);
			break;
		case STEP(43):
			g_pIndex[INDEX_03]->m_pSolStageKitOC->Actuate(pmOPEN);
			break;

		// Index Mask Picker Up/Down
		case HOME(44):
			g_pIndex[INDEX_03]->m_pCylMaskPickerUD->Actuate(pmUP);
			break;
		case STEP(44):
			if(g_pIndex[INDEX_03]->m_pMtX->IsRdy(CIndex::PX_MASK_PICKER)&&g_pIndex[INDEX_03]->m_pMtT->IsRdy(CIndex::PT_MASK_PICKER))
			{
				g_pIndex[INDEX_03]->m_pCylMaskPickerUD->Actuate(pmDOWN);
			}
			
		// Index Mask Picker Open/Close
		case HOME(45):
			g_pIndex[INDEX_03]->m_pCylMaskPickerOC->Actuate(pmOPEN);
			break;
		case STEP(45):
			g_pIndex[INDEX_03]->m_pCylMaskPickerOC->Actuate(pmCLOSE);
			break;

		case HOME(46):
			g_pIndex[INDEX_03]->m_pCylDustShutterOC->Actuate(pmOPEN);
			break;

		case STEP(46):
			g_pIndex[INDEX_03]->m_pCylDustShutterOC->Actuate(pmCLOSE);
			break;

		// Index & Router Prs Pos Move
		case HOME(48):
			break;
		case STEP(48):
			{
				if(!g_routerR.m_pMtZ_F->IsRdy(CRouter::PZ_READY) && !g_routerR.m_pMtZ_R->IsRdy(CRouter::PZ_READY))
				{
					g_routerR.m_pMtZ_F->Move(CRouter::PZ_READY);
					g_routerR.m_pMtZ_R->Move(CRouter::PZ_READY);
					break;
				}

				if(!g_pIndex[INDEX_03]->CanMove(CIndex::PX_ROUTER_PRS))
					break;

				if(!g_pIndex[INDEX_03]->CylIndexMaskFixAct(pmCLOSE))
					break;

				static int nPrsBlockCnt_3 = 0;

				if(2 < nPrsBlockCnt_3)
					nPrsBlockCnt_3 = 0;

				g_routerR.m_pMtZ_F->Move(CRouter::PZ_PRS);
				POINT2D ptPos = g_pIndex[INDEX_03]->GetRouterPrsPos(nPrsBlockCnt_3);
				g_pIndex[INDEX_03]->m_pMtX->PMove(CIndex::PX_ROUTER_PRS, ptPos.dX);
				g_pIndex[INDEX_03]->m_pMtT->Move(CIndex::PT_ROUTER_PRS);
				double dPosW = g_routerR.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				g_routerR.GentryMtYWPMove(CRouter::PY_VI_PRS_F, CRouter::PW_READY, ptPos.dY, dPosW);

				nPrsBlockCnt_3++;
			}
			break;

		/////////////////////////////////////////////////////////////////////
		// Index04		50 
		/////////////////////////////////////////////////////////////////////
		case HOME(50):
			g_pIndex[INDEX_04]->m_pMtX->CancelHomeSearch();
			g_pIndex[INDEX_04]->m_pMtT->CancelHomeSearch();

			g_allHome.Start(CAllHome::hmIndex01);
			break;
		case STEP(50):
			g_pIndex[INDEX_04]->m_bRun = TRUE;
			break;

		// Index Mask Fix Up/Down
		case HOME(51):
			g_pIndex[INDEX_04]->SetCylMaskFixUD(pmUP);
			break;
		case STEP(51):
			g_pIndex[INDEX_04]->SetCylMaskFixUD(pmDOWN);
			break;

		// Index Mask Fix Fwd/Bwd
		case HOME(52):
			if(pmUP == g_pIndex[INDEX_04]->IsCylMaskFixUD(pmUP, 1000))
				g_pIndex[INDEX_04]->SetCylMaskFixFB(pmBWD);
			break;
		case STEP(52):
			if(pmUP == g_pIndex[INDEX_04]->IsCylMaskFixUD(pmUP, 1000))
				g_pIndex[INDEX_04]->SetCylMaskFixFB(pmFWD);
			break;

		// Index Stage Kit Open/Close
		case HOME(53):
			g_pIndex[INDEX_04]->m_pSolStageKitOC->Actuate(pmCLOSE);
			break;
		case STEP(53):
			g_pIndex[INDEX_04]->m_pSolStageKitOC->Actuate(pmOPEN);
			break;

		// Index Mask Picker Up/Down
		case HOME(54):
			g_pIndex[INDEX_04]->m_pCylMaskPickerUD->Actuate(pmUP);
			break;
		case STEP(54):
			if(g_pIndex[INDEX_04]->m_pMtX->IsRdy(CIndex::PX_MASK_PICKER)&&g_pIndex[INDEX_04]->m_pMtT->IsRdy(CIndex::PT_MASK_PICKER))
			{
				g_pIndex[INDEX_04]->m_pCylMaskPickerUD->Actuate(pmDOWN);
			}
			
		// Index Mask Picker Open/Close
		case HOME(55):
			g_pIndex[INDEX_04]->m_pCylMaskPickerOC->Actuate(pmOPEN);
			break;
		case STEP(55):
			g_pIndex[INDEX_04]->m_pCylMaskPickerOC->Actuate(pmCLOSE);
			break;

		case HOME(56):
			g_pIndex[INDEX_04]->m_pCylDustShutterOC->Actuate(pmOPEN);
			break;

		case STEP(56):
			g_pIndex[INDEX_04]->m_pCylDustShutterOC->Actuate(pmCLOSE);
			break;

		// Index & Router Prs Pos Move
		case HOME(58):
			break;
		case STEP(58):
			{
				if(!g_routerR.m_pMtZ_F->IsRdy(CRouter::PZ_READY) && !g_routerR.m_pMtZ_R->IsRdy(CRouter::PZ_READY))
				{
					g_routerR.m_pMtZ_F->Move(CRouter::PZ_READY);
					g_routerR.m_pMtZ_R->Move(CRouter::PZ_READY);
					break;
				}

				if(!g_pIndex[INDEX_04]->CanMove(CIndex::PX_ROUTER_PRS))
					break;

				if(!g_pIndex[INDEX_04]->CylIndexMaskFixAct(pmCLOSE))
					break;

				static int nPrsBlockCnt_4 = 0;

				if(2 < nPrsBlockCnt_4)
					nPrsBlockCnt_4 = 0;

				g_routerR.m_pMtZ_F->Move(CRouter::PZ_PRS);
				POINT2D ptPos = g_pIndex[INDEX_04]->GetRouterPrsPos(nPrsBlockCnt_4);
				g_pIndex[INDEX_04]->m_pMtX->PMove(CIndex::PX_ROUTER_PRS, ptPos.dX);
				g_pIndex[INDEX_04]->m_pMtT->Move(CIndex::PT_ROUTER_PRS);
				double dPosW = g_routerR.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				g_routerR.GentryMtYWPMove(CRouter::PY_VI_PRS_R, CRouter::PW_READY, ptPos.dY, dPosW);

				nPrsBlockCnt_4++;
			}
			break;

		/////////////////////////////////////////////////////////////////////
		// Router0102	60
		/////////////////////////////////////////////////////////////////////

		// Home, Step
		case HOME(60):
			g_routerF.m_pMtY->CancelHomeSearch();
			g_routerF.m_pMtW->CancelHomeSearch();
			g_routerF.m_pMtZ_F->CancelHomeSearch();
			g_routerF.m_pMtZ_R->CancelHomeSearch();
			
			g_allHome.Start(CAllHome::hmRouterF);
			break;
		case STEP(60):
			if (!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			g_routerF.m_bRun = TRUE;
			break;

		// Spindle Bit Clamp Front
		case HOME(61):
			g_routerF.m_pSolSpdChuckOC_F->Actuate(pmCLOSE);
			break;
		case STEP(61):
			g_routerF.m_pSolSpdChuckOC_F->Actuate(pmOPEN);
			break;

		// Spindle Bit Clamp Rear
		case HOME(62):
			g_routerF.m_pSolSpdChuckOC_R->Actuate(pmCLOSE);
			break;
		case STEP(62):
			g_routerF.m_pSolSpdChuckOC_R->Actuate(pmOPEN);
			break;

		// Bit WireCheck Rear Cycle
		case HOME(63):
			if(!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			g_routerF.m_fsm.Set(CRouter::C_SPD_ESD_CHECK_START, ROUTER_R);
			break;

		// Bit WireCheck Front Cycle
		case STEP(63):
			if(!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			g_routerF.m_fsm.Set(CRouter::C_SPD_ESD_CHECK_START, ROUTER_F);
			break;

		// Bit Vision Rear Cycle
		case HOME(64):
			if(!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			g_routerF.m_fsm.Set(CRouter::C_SPD_BIT_VI_START, ROUTER_R);
			break;

		// Bit Vision Front Cycle
		case STEP(64):
			if(!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			g_routerF.m_fsm.Set(CRouter::C_SPD_BIT_VI_START, ROUTER_F);
			break;
			
		// Cyl Bit Clamp Cycle
		case HOME(65):
			break;
		case STEP(65):
			{
				if (!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
					break;

				if(g_pNV->NDm(existRouterCylBitF))
					break;
			
				if(!g_pIndex[INDEX_01]->CanMove(CIndex::PX_CYL_BIT_SUPPLY_BOX))
					break;

				int nBitCurCnt = g_pNV->NDm(RedindexBitBoxCurCnt01);

				if(nBitCurCnt < 0)
				{
					g_err.Save(ER_INDEX01_BIT_BOX_EMPTY);
					break;
				}

				g_routerF.m_fsm.Set(CRouter::C_CYL_BIT_CLAMP_START);
			}
			break;

		// Cyl Bit Align Rear Put Down Cycle
		case HOME(66):
			if (!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_pNV->NDm(existRouterCylBitF))
				break;
			if(g_pNV->NDm(existRedIndexBitAlign02))
				break;
			if(g_dIn.BOn(iIndex01BitAlignExist02))
				break;
			if(g_routerF.m_pInfoBitR->nExist)
				break;
			if(!g_pIndex[INDEX_01]->CanMove(CIndex::PX_CYL_BIT_ALIGN_R))
				break;

			g_routerF.m_fsm.Set(CRouter::C_CYL_BIT_ALIGN_START, ROUTER_R);
			break;
		// Cyl Bit Align Front Put Down Cycle
		case STEP(66):
			if (!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_pNV->NDm(existRouterCylBitF))
				break;
			if(g_pNV->NDm(existRedIndexBitAlign01))
				break;
			if(g_dIn.BOn(iIndex01BitAlignExist01))
				break;
			if(g_routerF.m_pInfoBitF->nExist)
				break;
			if(!g_pIndex[INDEX_01]->CanMove(CIndex::PX_CYL_BIT_ALIGN_F))
				break;

			g_routerF.m_fsm.Set(CRouter::C_CYL_BIT_ALIGN_START, ROUTER_F);
			break;

		// Spindle Rear Eject Cycle
		case HOME(67):
			if (!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_routerF.m_pInfoBitR->nExist)
				break;
			if(!g_dIn.AOn(iIndexBitEjectBoxExist01))
				break;
			if(!g_pIndex[INDEX_01]->CanMove(CIndex::PX_SPD_BIT_EJECT))
				break;

			g_routerF.m_fsm.Set(CRouter::C_SPD_BIT_EJECT_START, ROUTER_R);
			break;

		// Spindle Front Eject Cycle
		case STEP(67):
			if (!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_routerF.m_pInfoBitF->nExist)
				break;
			if(!g_dIn.AOn(iIndexBitEjectBoxExist01))
				break;
			if(!g_pIndex[INDEX_01]->CanMove(CIndex::PX_SPD_BIT_EJECT))
				break;

			g_routerF.m_fsm.Set(CRouter::C_SPD_BIT_EJECT_START, ROUTER_F);
			break;

		// Spindle Rear Bit PickUp Cycle
		case HOME(68):
			if (!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(g_routerF.m_pInfoBitR->nExist)
				break;
			if(!g_pNV->NDm(existRedIndexBitAlign02))
				break;
			if(!g_dIn.BOn(iIndex01BitAlignExist02))
				break;
			if(!g_pIndex[INDEX_01]->CanMove(CIndex::PX_SPD_BIT_CLAMP_R))
				break;

			g_routerF.m_fsm.Set(CRouter::C_SPD_BIT_CLAMP_START, ROUTER_R);
			break;

		// Spindle Front Bit PickUp Cycle
		case STEP(68):
			if (!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(g_routerF.m_pInfoBitF->nExist)
				break;
			if(!g_pNV->NDm(existRedIndexBitAlign01))
				break;
			if(!g_dIn.BOn(iIndex01BitAlignExist01))
				break;
			if(!g_pIndex[INDEX_01]->CanMove(CIndex::PX_SPD_BIT_CLAMP_F))
				break;

			g_routerF.m_fsm.Set(CRouter::C_SPD_BIT_CLAMP_START, ROUTER_F);
			break;

		// Spindle Rear Bit Broken/Flow Down Cycle
		case HOME(69):
			if (!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_routerF.m_pInfoBitF->nExist)
				break;
			if (!g_routerF.m_pMtZ_F->InPos((CRouter::PZ_READY)) || !g_routerF.m_pMtZ_R->InPos((CRouter::PZ_READY)))
				break;
			if(!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;
			if (pmUP != g_routerF.m_pCylBitClampUD->GetPos())
				break;
			if(g_routerF.m_fsm.IsRun())
				break;

			// g_routerF 라우터 Front 2개중 ROUTER_R rear 쪽
			g_routerF.m_fsm.Set(CRouter::C_SPD_BIT_VERIFY_START, ROUTER_R);
			break;

			// Spindle Front Broken/Flow Down Cycle
		case STEP(69):
			if (!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_routerF.m_pInfoBitF->nExist)
				break;
			if (!g_routerF.m_pMtZ_F->InPos((CRouter::PZ_READY)) || !g_routerF.m_pMtZ_R->InPos((CRouter::PZ_READY)))
				break;
			if(!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;
			if (pmUP != g_routerF.m_pCylBitClampUD->GetPos())
				break;
			if(g_routerF.m_fsm.IsRun())
				break;

			g_routerF.m_fsm.Set(CRouter::C_SPD_BIT_VERIFY_START, ROUTER_F);
			break;


		/////////////////////////////////////////////////////////////////////
		// Router0304	70
		/////////////////////////////////////////////////////////////////////

		// Home, Step
		case HOME(70):
			g_routerR.m_pMtY->CancelHomeSearch();
			g_routerR.m_pMtW->CancelHomeSearch();
			g_routerR.m_pMtZ_F->CancelHomeSearch();
			g_routerR.m_pMtZ_R->CancelHomeSearch();
			
			g_allHome.Start(CAllHome::hmRouterR);
			break;
		case STEP(70):
			if (!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			g_routerR.m_bRun = TRUE;
			break;

		// Spindle Bit Clamp Front
		case HOME(71):
			g_routerR.m_pSolSpdChuckOC_F->Actuate(pmCLOSE);
			break;
		case STEP(71):
			g_routerR.m_pSolSpdChuckOC_F->Actuate(pmOPEN);
			break;

		// Spindle Bit Clamp Rear
		case HOME(72):
			g_routerR.m_pSolSpdChuckOC_R->Actuate(pmCLOSE);
			break;
		case STEP(72):
			g_routerR.m_pSolSpdChuckOC_R->Actuate(pmOPEN);
			break;

		// Bit WireCheck Rear Cycle
		case HOME(73):
			if(!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			g_routerR.m_fsm.Set(CRouter::C_SPD_ESD_CHECK_START, ROUTER_R);
			break;

		// Bit WireCheck Front Cycle
		case STEP(73):
			if(!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			g_routerR.m_fsm.Set(CRouter::C_SPD_ESD_CHECK_START, ROUTER_F);
			break;

		// Bit Vision Rear Cycle
		case HOME(74):
			if(!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			g_routerR.m_fsm.Set(CRouter::C_SPD_BIT_VI_START, ROUTER_R);
			break;

		// Bit Vision Front Cycle
		case STEP(74):
			if(!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			g_routerR.m_fsm.Set(CRouter::C_SPD_BIT_VI_START, ROUTER_F);
			break;
			
		// Cyl Bit Clamp Cycle
		case HOME(75):
			break;
		case STEP(75):
			{
				if (!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
					break;

				if(g_pNV->NDm(existRouterCylBitR))
					break;
			
				if(!g_pIndex[INDEX_04]->CanMove(CIndex::PX_CYL_BIT_SUPPLY_BOX))
					break;

				int nBitCurCnt = g_pNV->NDm(RedindexBitBoxCurCnt04);

				if(nBitCurCnt < 0)
				{
					g_err.Save(ER_INDEX04_BIT_BOX_EMPTY);
					break;
				}

				g_routerR.m_fsm.Set(CRouter::C_CYL_BIT_CLAMP_START);
			}
			break;

		// Cyl Bit Align Rear Put Down Cycle
		case HOME(76):
			if (!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_pNV->NDm(existRouterCylBitR))
				break;
			if(g_pNV->NDm(existRedIndexBitAlign04))
				break;
			if(g_dIn.BOn(iIndex04BitAlignExist02))
				break;
			if(g_routerR.m_pInfoBitR->nExist)
				break;
			if(!g_pIndex[INDEX_04]->CanMove(CIndex::PX_CYL_BIT_ALIGN_R))
				break;

			g_routerR.m_fsm.Set(CRouter::C_CYL_BIT_ALIGN_START, ROUTER_R);
			break;
		// Cyl Bit Align Front Put Down Cycle
		case STEP(76):
			if (!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_pNV->NDm(existRouterCylBitR))
				break;
			if(g_pNV->NDm(existRedIndexBitAlign03))
				break;
			if(g_dIn.BOn(iIndex04BitAlignExist01))
				break;
			if(g_routerR.m_pInfoBitF->nExist)
				break;
			if(!g_pIndex[INDEX_04]->CanMove(CIndex::PX_CYL_BIT_ALIGN_F))
				break;

			g_routerR.m_fsm.Set(CRouter::C_CYL_BIT_ALIGN_START, ROUTER_F);
			break;

		// Spindle Rear Eject Cycle
		case HOME(77):
			if (!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_routerR.m_pInfoBitR->nExist)
				break;
			if(!g_dIn.AOn(iIndexBitEjectBoxExist04))
				break;
			if(!g_pIndex[INDEX_04]->CanMove(CIndex::PX_SPD_BIT_EJECT))
				break;

			g_routerR.m_fsm.Set(CRouter::C_SPD_BIT_EJECT_START, ROUTER_R);
			break;

		// Spindle Front Eject Cycle
		case STEP(77):
			if (!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_routerR.m_pInfoBitF->nExist)
				break;
			if(!g_dIn.AOn(iIndexBitEjectBoxExist04))
				break;
			if(!g_pIndex[INDEX_04]->CanMove(CIndex::PX_SPD_BIT_EJECT))
				break;

			g_routerR.m_fsm.Set(CRouter::C_SPD_BIT_EJECT_START, ROUTER_F);
			break;

		// Spindle Rear Bit PickUp Cycle
		case HOME(78):
			if (!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(g_routerR.m_pInfoBitR->nExist)
				break;
			if(!g_pNV->NDm(existRedIndexBitAlign04))
				break;
			if(!g_dIn.BOn(iIndex04BitAlignExist02))
				break;
			if(!g_pIndex[INDEX_04]->CanMove(CIndex::PX_SPD_BIT_CLAMP_R))
				break;

			g_routerR.m_fsm.Set(CRouter::C_SPD_BIT_CLAMP_START, ROUTER_R);
			break;

		// Spindle Front Bit PickUp Cycle
		case STEP(78):
			if (!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(g_routerR.m_pInfoBitF->nExist)
				break;
			if(!g_pNV->NDm(existRedIndexBitAlign03))
				break;
			if(!g_dIn.BOn(iIndex04BitAlignExist01))
				break;
			if(!g_pIndex[INDEX_04]->CanMove(CIndex::PX_SPD_BIT_CLAMP_F))
				break;

			g_routerR.m_fsm.Set(CRouter::C_SPD_BIT_CLAMP_START, ROUTER_F);
			break;

		// Spindle Rear Bit Broken/Flow Down Cycle
		case HOME(79):
			if (!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_routerR.m_pInfoBitF->nExist)
				break;
			if (!g_routerR.m_pMtZ_F->InPos((CRouter::PZ_READY)) || !g_routerR.m_pMtZ_R->InPos((CRouter::PZ_READY)))
				break;
			if(!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;
			if (pmUP != g_routerR.m_pCylBitClampUD->GetPos())
				break;
			if(g_routerR.m_fsm.IsRun())
				break;

			// g_routerF 라우터 Front 2개중 ROUTER_R rear 쪽
			g_routerR.m_fsm.Set(CRouter::C_SPD_BIT_VERIFY_START, ROUTER_R);
			break;

		// Spindle Front Broken/Flow Down Cycle
		case STEP(79):
			if (!g_routerF.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;

			if(!g_routerR.m_pInfoBitF->nExist)
				break;
			if (!g_routerR.m_pMtZ_F->InPos((CRouter::PZ_READY)) || !g_routerR.m_pMtZ_R->InPos((CRouter::PZ_READY)))
				break;
			if(!g_routerR.IsGentryMtYWRdy(CRouter::PY_READY, CRouter::PW_READY))
				break;
			if (pmUP != g_routerR.m_pCylBitClampUD->GetPos())
				break;
			if(g_routerR.m_fsm.IsRun())
				break;

			g_routerR.m_fsm.Set(CRouter::C_SPD_BIT_VERIFY_START, ROUTER_F);
			break;

		/////////////////////////////////////////////////////////////////////
		// Out Pnp		80
		/////////////////////////////////////////////////////////////////////

		// Home, Step
		case HOME(80):
			g_outPnp.m_pMtX->CancelHomeSearch();
			g_outPnp.m_pMtY->CancelHomeSearch();
			g_outPnp.m_pMtZ->CancelHomeSearch();

			g_allHome.Start(CAllHome::hmOutPnp);
			break;
		case STEP(80):
			g_outPnp.m_bRun = TRUE;
			break;

		// Outpnp Vac On/Off
		case HOME(81):
			g_outPnp.m_pVac->Actuate(pmOFF);
			break;
		case STEP(81):
			g_outPnp.m_pVac->Actuate(pmON);
			break;

		// Outpnp Kit Open/Close
		case HOME(82):
			g_outPnp.m_pSolKitClampOC->Actuate(pmCLOSE);
			break;
		case STEP(82):
			g_outPnp.m_pSolKitClampOC->Actuate(pmOPEN);
			break;

		// Outpnp Scrap Up/Down
		case HOME(83):
			g_outPnp.m_pCylScrapUD->Actuate(pmUP);
			break;
		case STEP(83):
			g_outPnp.m_pCylScrapUD->Actuate(pmDOWN);
			break;

		// Outpnp Scrap Fix Up/Down
		case HOME(84):
			g_outPnp.SetCylScrapFixUD(pmUP);
			break;
		case STEP(84):
			g_outPnp.SetCylScrapFixUD(pmDOWN);
			break;

		// Outpnp Scrap Open/Close
		case HOME(85):
			g_outPnp.SetCylScrapOC(pmOPEN);
			break;
		case STEP(85):
			g_outPnp.SetCylScrapOC(pmCLOSE);
			break;


		/////////////////////////////////////////////////////////////////////
		// ADC		90
		/////////////////////////////////////////////////////////////////////

		// Home, Step
		case HOME(90):
			g_adc.m_pMtX->CancelHomeSearch();
			g_adc.m_pMtZ->CancelHomeSearch();

			g_allHome.Start(CAllHome::hmADC);
			break;
		case STEP(90):
			g_adc.m_bRun = TRUE;
			break;
		
		// ADC에 해당 하는 모든 축을 동작 시킨다.
		case HOME(91):
			break;
		case STEP(91):
			// 다른 쪽 Part는 해당 Part에서 모든 에로조건 확인
			if(g_pNV->NDm(mmiBtnAdcMode))
			{
				g_adc.m_bRun     = TRUE;
				g_inPnp.m_bRun   = TRUE;
				g_index01.m_bRun = TRUE;
				g_index02.m_bRun = TRUE;
				g_index03.m_bRun = TRUE;
				g_index04.m_bRun = TRUE;
				g_outPnp.m_bRun  = TRUE;
			}
			break;

			
		/////////////////////////////////////////////////////////////////////
		// Manual Router Pos Move 97: 
		/////////////////////////////////////////////////////////////////////
		case HOME(97):
			m_fsm.Set(C_POS_ROUTER_START);
			break;

		/////////////////////////////////////////////////////////////////////
		// Manual (Vision) Pos Move 97: 
		/////////////////////////////////////////////////////////////////////
		case STEP(97):
			m_fsm.Set(C_POS_VISION_START);
			break;

		/////////////////////////////////////////////////////////////////////
		// DRY RUNNING	98: 
		/////////////////////////////////////////////////////////////////////
		case HOME(98):
			{
				// Dry Running Home
				if(g_opr.isDoorOpen)
				{
					g_err.Door(FALSE);
					break;
				}

				if(!g_opr.isStop || g_allHome.m_fsm.IsRun() || g_opr.isDoorOpen)
					break;

				g_ldMzInConv.ExistStopper01() = FALSE;
				g_ldMzInConv.ExistStopper02() = FALSE;
				g_ldMzInConv.ExistStopper03() = FALSE;
				g_ldMzOutConv.ExistBuffer1() = FALSE;
				g_ldMzOutConv.ExistBuffer2() = FALSE;
				g_ldMzOutConv.ExistArrival() = FALSE;
				g_ldMz.Exist() = FALSE;
				g_rail.Exist() = FALSE;
				g_inPnp.ExistPcb() = FALSE;
				g_index01.ExistPcb() = FALSE;
				g_index01.ExistScrap() = FALSE;
				g_index02.ExistPcb() = FALSE;
				g_index02.ExistScrap() = FALSE;
				g_index03.ExistPcb() = FALSE;
				g_index03.ExistScrap() = FALSE;
				g_index04.ExistPcb() = FALSE;
				g_index04.ExistScrap() = FALSE;
				g_outPnp.ExistScrap() = FALSE;
				g_outPnp.ExistPcb() = FALSE;
				//g_routerF.m_pInfoBitF->nExist = TRUE;
				//g_routerF.m_pInfoBitF->nLength = 0;
				//g_routerF.m_pInfoBitR->nExist = TRUE;
				//g_routerF.m_pInfoBitR->nLength = 0;
				//g_routerR.m_pInfoBitF->nExist = TRUE;
				//g_routerR.m_pInfoBitF->nLength = 0;
				//g_routerR.m_pInfoBitR->nExist = TRUE;
				//g_routerR.m_pInfoBitR->nLength = 0;

				g_pNV->NDm(existRouterCylBitF) = FALSE;
				g_pNV->NDm(existRouterCylBitR) = FALSE;
				g_pNV->NDm(existRedIndexBitAlign01) = FALSE;
				g_pNV->NDm(existRedIndexBitAlign02) = FALSE;
				g_pNV->NDm(existRedIndexBitAlign03) = FALSE;
				g_pNV->NDm(existRedIndexBitAlign04) = FALSE;

				g_pNV->NDm(RedindexBitBoxCurCnt01) = (int)(g_pNV->DDm(indexBitBoxXCnt) * g_pNV->DDm(indexBitBoxYCnt)) - 1; // Max
				g_pNV->NDm(RedindexBitBoxCurCnt04) = (int)(g_pNV->DDm(indexBitBoxXCnt) * g_pNV->DDm(indexBitBoxYCnt)) - 1; // Max

				g_opr.isDryRun = FALSE;

			}
			break;

		case STEP(98): 
			{
				// Dry Running Run
				if(g_opr.isDoorOpen)
				{
					g_err.Door(FALSE);
					break;
				}

				if(!g_opr.isStop || g_allHome.m_fsm.IsRun() || g_opr.isDoorOpen)
					break;

				g_ldMzInConv.ExistStopper01() = FALSE;
				g_ldMzInConv.ExistStopper02() = FALSE;
				g_ldMzInConv.ExistStopper03() = FALSE;
				g_ldMzOutConv.ExistBuffer1() = FALSE;
				g_ldMzOutConv.ExistBuffer2() = FALSE;
				g_ldMzOutConv.ExistArrival() = FALSE;
				g_ldMz.Exist() = FALSE;
				g_rail.Exist() = FALSE;
				g_inPnp.ExistPcb() = FALSE;
				g_index01.ExistPcb() = FALSE;
				g_index01.ExistScrap() = FALSE;
				g_index02.ExistPcb() = FALSE;
				g_index02.ExistScrap() = FALSE;
				g_index03.ExistPcb() = FALSE;
				g_index03.ExistScrap() = FALSE;
				g_index04.ExistPcb() = FALSE;
				g_index04.ExistScrap() = FALSE;
				g_outPnp.ExistScrap() = FALSE;
				g_outPnp.ExistPcb() = FALSE;
			
				// Bit Clamp 시퀀스 검증을 위해서 아래 기능 Skip hkkim
				g_routerF.m_pInfoBitF->nExist  = TRUE;
				g_routerF.m_pInfoBitF->nLength = 0;
				g_routerF.m_pInfoBitF->nZStep  = 0;
				g_routerF.m_pInfoBitR->nExist  = TRUE;
				g_routerF.m_pInfoBitR->nLength = 0;
				g_routerF.m_pInfoBitR->nZStep  = 0;
				g_routerR.m_pInfoBitF->nExist  = TRUE;
				g_routerR.m_pInfoBitF->nLength = 0;
				g_routerR.m_pInfoBitF->nZStep  = 0;
				g_routerR.m_pInfoBitR->nExist  = TRUE;
				g_routerR.m_pInfoBitR->nLength = 0;
				g_routerR.m_pInfoBitR->nZStep  = 0;

				g_pNV->NDm(existRouterCylBitF)   = FALSE;
				g_pNV->NDm(existRouterCylBitR)   = FALSE;
				g_pNV->NDm(existRedIndexBitAlign01) = FALSE;
				g_pNV->NDm(existRedIndexBitAlign02) = FALSE;
				g_pNV->NDm(existRedIndexBitAlign03) = FALSE;
				g_pNV->NDm(existRedIndexBitAlign04) = FALSE;

				g_pNV->NDm(RedindexBitBoxCurCnt01) = (int)(g_pNV->DDm(indexBitBoxXCnt) * g_pNV->DDm(indexBitBoxYCnt)) - 1; // Max
				g_pNV->NDm(RedindexBitBoxCurCnt04) = (int)(g_pNV->DDm(indexBitBoxXCnt) * g_pNV->DDm(indexBitBoxYCnt)) - 1; // Max

				int nSize = sizeof(INDEX_MEMORY);
				ZeroMemory(g_index01.m_pMem, nSize);
				ZeroMemory(g_index02.m_pMem, nSize);
				ZeroMemory(g_index03.m_pMem, nSize);
				ZeroMemory(g_index04.m_pMem, nSize);


				g_opr.isDryRun = TRUE;
			}
			break;

		/////////////////////////////////////////////////////////////////////
		// ALL HOME    99: 
		/////////////////////////////////////////////////////////////////////
		case HOME(99):
			{
				if(g_opr.isDoorOpen)
				{
					g_err.Door(FALSE);
					break;
				}

				if(!g_opr.isStop || g_allHome.m_fsm.IsRun() || g_opr.isDoorOpen)
					break;

				int mtNo = 0;

				for(mtNo = 0; mtNo < MAX_MT_NO; mtNo++)
				{
					g_mt[mtNo].CancelHomeSearch();
				}

				g_opr.isAllHome = FALSE;
				g_pNV->NDm(initViewChange) = TRUE;
				g_allHome.m_bInPnpError = TRUE;
				g_allHome.Start(CAllHome::hmAllHome);
			}
			break;
	}
}

//-------------------------------------------------------------------
void CTenkeyOpr::Init(void)
{
	m_tenkey.SetIO(&g_dIn.m_ch[0], &g_dOut.m_ch[0]);
	m_tenkey.Enable();
}


//-------------------------------------------------------------------
void CTenkeyOpr::Cycle(void)
{
	Jog();
	CyclePosRouter();
	CyclePosVision();
}


//---------------------------------------------------------------
void CTenkeyOpr::Jog(void)
{
	if(!Between(g_pNV->NDm(jogSpeed), 1, 100))
		g_pNV->NDm(jogSpeed) = 5;

	int nAxisNo   = g_pNV->NDm(jogAxisNo);
	int nJogSpeed = (g_pNV->NDm(jogSpeed) * 1000);

	BOOL bLpressed = (CLR_KEY == m_tenkey.GetKeyPadNo());
	BOOL bRpressed = (SET_KEY == m_tenkey.GetKeyPadNo());

	BOOL bJogStop   = (TRUE != g_pNV->NDm(tenKeyJog));
		 bJogStop  |= (g_opr.isCycleRun || !g_opr.isStop || g_opr.isEmg || g_allHome.m_fsm.IsRun());

	if(bJogStop)
	{
		if((C_JOGSTOP > m_fsmJog.Get()) && (C_IDLE != m_fsmJog.Get()))
			m_fsmJog.Set(C_JOGSTOP);
	}

	switch(m_fsmJog.Get())
	{
	case C_JOGWAIT:
		if(g_mt[nAxisNo].m_state.isDriving)
			break;

		if(bLpressed || bRpressed)
		{
			if(g_mt[nAxisNo].m_profile.cmdIndex < MIDX_JOG)
			{
				g_mt[nAxisNo].m_profile.nextIndex = g_mt[nAxisNo].m_profile.cmdIndex;
				g_mt[nAxisNo].m_profile.cmdIndex  = g_mt[nAxisNo].m_profile.curIndex = MIDX_JOG;
			}
		}

		if(bRpressed)
		{
			g_mt[nAxisNo].RMove(-200000, nJogSpeed); //1회 최대 이동량은 200mm
			m_fsmJog.Set(C_JOGMOVE_N);
		}
		else if(bLpressed)
		{
			g_mt[nAxisNo].RMove(200000, nJogSpeed);
			m_fsmJog.Set(C_JOGMOVE_P);
		}
		break;
	case C_JOGMOVE_N:
		if(!bRpressed)
		{
			m_fsmJog.Set(C_JOGSTOP);
		}
		else if(!g_mt[nAxisNo].m_state.isDriving)
		{
			if(m_fsmJog.Delay(100))
				m_fsmJog.Set(C_IDLE);
		}
		break;
	case C_JOGMOVE_P:
		if(!bLpressed)
		{
			m_fsmJog.Set(C_JOGSTOP);
		}
		else if(!g_mt[nAxisNo].m_state.isDriving)
		{
			if(m_fsmJog.Delay(100))
				m_fsmJog.Set(C_IDLE);
		}
		break;
	case C_JOGSTOP:
		g_mt[nAxisNo].Stop(TRUE);
		m_fsmJog.Set(C_IDLE);
		break;
	case C_IDLE:
		if(TRUE == bJogStop)
			break;
		if(FALSE == g_opr.isStop)
			break;

		if(NO_KEY == m_tenkey.GetKeyPadNo())
			m_fsmJog.Set(C_JOGWAIT);
		break;
	default:
		m_fsmJog.Set(C_IDLE);
		break;
	}
}


void CTenkeyOpr::CyclePosRouter(void)
{
	// 조건들만 Check 하므로 Error시에 초기화 루틴이 없음
	if(!m_fsm.Between(C_POS_ROUTER_START, C_POS_ROUTER_END))
		return;

	if(m_fsm.TimeLimit(600000))
	{
		g_err.Save(ER_TENKEY_POS_ROUTER_CYCLE_TM_OVER);
		m_fsm.Set(C_IDLE);
		return;
	}

	int nIdx = g_pNV->NDm(mmiRouterIndexNo);

	switch(m_fsm.Get())
	{
	case C_POS_ROUTER_START:
		{
			if(!Between(nIdx, INDEX_01, INDEX_04))
			{
				m_fsm.Set(C_IDLE);
				break;
			}

			if(INDEX_01 == nIdx || INDEX_02 == nIdx)
			{
				if(!g_routerF.m_pMtZ_F->IsRdy(CRouter::PZ_READY) || 
				   !g_routerF.m_pMtZ_R->IsRdy(CRouter::PZ_READY))
				{
					g_routerF.m_pMtZ_F->Move(CRouter::PZ_READY);
					g_routerF.m_pMtZ_R->Move(CRouter::PZ_READY);
				}
			}
			else
			{
				if(!g_routerR.m_pMtZ_F->IsRdy(CRouter::PZ_READY) || 
				   !g_routerR.m_pMtZ_R->IsRdy(CRouter::PZ_READY))
				{
					g_routerR.m_pMtZ_F->Move(CRouter::PZ_READY);
					g_routerR.m_pMtZ_R->Move(CRouter::PZ_READY);
				}
			}
			m_fsm.Set(C_POS_ROUTER_01);
		}
		break;
	case C_POS_ROUTER_01:
		{
			if(INDEX_01 == nIdx || INDEX_02 == nIdx)
			{
				if(!g_routerF.m_pMtZ_F->InPos(CRouter::PZ_READY))
					break;
				if(!g_routerF.m_pMtZ_R->InPos(CRouter::PZ_READY))
					break;
			}
			else
			{
				if(!g_routerR.m_pMtZ_F->InPos(CRouter::PZ_READY))
					break;
				if(!g_routerR.m_pMtZ_R->InPos(CRouter::PZ_READY))
					break;
			}

			if(!g_pIndex[nIdx]->CanMove())
			{
				m_fsm.Set(C_IDLE);
				break;
			}

			m_fsm.Set(C_POS_ROUTER_02);
		}
		break;
	case C_POS_ROUTER_02:
		{
			int pathNo  = g_pNV->NDm(mmiRouterPathNo);
			int pathPos = g_pNV->NDm(mmiRouterPathPos);

			if(INDEX_01 == nIdx || INDEX_02 == nIdx)
			{
				if(!g_routerF.IsSpindle2PinPitch())
				{
					g_err.Save(ER_ROUTER_Y_SPINDLE_PITCH_ERR);
					m_fsm.Set(C_IDLE);
					break;
				}
			}
			else
			{
				if(!g_routerR.IsSpindle2PinPitch())
				{
					g_err.Save(ER_ROUTER_Y_SPINDLE_PITCH_ERR);
					m_fsm.Set(C_IDLE);
					break;
				}
			}

			int nYLineMoveCnt = (int)g_pNV->gerberPara(arrayYCnt) / 2;

			int nYRearSkip    = (int)g_pNV->gerberPara(arrayYCnt) % 2; 
			int nMaxPathCnt   = (int)g_pNV->gerberPara(arrayPathCnt) * (int)g_pNV->gerberPara(arrayXCnt) * (int)g_pNV->gerberPara(arrayYCnt);
			int nArrayPathCnt = (int)g_pNV->gerberPara(arrayPathCnt);
			int nYLineCurCnt  = g_pIndex[nIdx]->m_pMem->routerCmdCnt / (int)g_pNV->gerberPara(arrayPathCnt);

			BOOL bSpindleRearSkip = FALSE;

			if(INDEX_01 == nIdx || INDEX_02 == nIdx)
				bSpindleRearSkip = g_pNV->UseSkip(usSpindle2Skip);
			else 
				bSpindleRearSkip = g_pNV->UseSkip(usSpindle4Skip);

			if(!bSpindleRearSkip)
			{
				// Max Path Cnt 연산
				if((0 == nYLineMoveCnt)) // 동일한 배열의 Path가 없는 것이므로 Spindle Front로 Max Path 이동
				{}
				else if((0 < nYLineMoveCnt) && (0 == nYRearSkip)) // 짝수 : Spindle 두개 모두 사용 (Path 절반만 이동.)
					nMaxPathCnt = nMaxPathCnt/2;
				else if((0 < nYLineMoveCnt) && (1 == nYRearSkip)) // 홀수 : 막지막 열만 Spindle 한개 사용(1개 사용시의 Path를 더함)
					nMaxPathCnt = (nArrayPathCnt * nYLineMoveCnt) + nArrayPathCnt; 
			}
			if(nMaxPathCnt < pathNo)
			{
				m_fsm.Set(C_IDLE);
				break;
			}

			int nLineType = g_pIndex[nIdx]->GetGerberLineType(pathNo);
			if(-1 == nLineType)
			{
				g_err.Save(ER_GERBER_LINE_TYPE_ERR);
				m_fsm.Set(C_IDLE);
				break;
			}

			XYT xytPos = g_pIndex[nIdx]->GetRouterPos(pathNo, pathPos);
			g_pIndex[nIdx]->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX);
			g_pIndex[nIdx]->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);

			if(INDEX_01 == nIdx)
			{
				double dPosW = g_routerF.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				double dOffset = g_routerF.GetBitYOffset();
				POINT2D ptMainOffset = g_pIndex[nIdx]->GetGerberOffset(pathNo, pathPos);
				POINT2D ptSubOffset = g_pIndex[nIdx]->GetGerberSubOffset(pathNo, pathPos);
				double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
				//g_routerF.GentryMtYWPMove(CRouter::PY_ROUTER_F, xytPos.dY, xytPos.dY + dOffset);
				g_routerF.GentryMtYWPMove(CRouter::PY_ROUTER_F, CRouter::PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset);
			}
			else if(INDEX_02 == nIdx)
			{
				double dPosW = g_routerF.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				double dOffset = g_routerF.GetBitYOffset();
				POINT2D ptMainOffset = g_pIndex[nIdx]->GetGerberOffset(pathNo, pathPos);
				POINT2D ptSubOffset = g_pIndex[nIdx]->GetGerberSubOffset(pathNo, pathPos);
				double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
				g_routerF.GentryMtYWPMove(CRouter::PY_ROUTER_R, CRouter::PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset);
			}
			else if(INDEX_03 == nIdx)
			{
				double dPosW = g_routerR.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				double dOffset = g_routerR.GetBitYOffset();
				POINT2D ptMainOffset = g_pIndex[nIdx]->GetGerberOffset(pathNo, pathPos);
				POINT2D ptSubOffset = g_pIndex[nIdx]->GetGerberSubOffset(pathNo, pathPos);
				double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
				g_routerR.GentryMtYWPMove(CRouter::PY_ROUTER_F, CRouter::PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset);
			}
			else if(INDEX_04 == nIdx)
			{
				double dPosW = g_routerR.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				double dOffset = g_routerR.GetBitYOffset();
				POINT2D ptMainOffset = g_pIndex[nIdx]->GetGerberOffset(pathNo, pathPos);
				POINT2D ptSubOffset = g_pIndex[nIdx]->GetGerberSubOffset(pathNo, pathPos);
				double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
				g_routerR.GentryMtYWPMove(CRouter::PY_ROUTER_R, CRouter::PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset);
			}
			m_fsm.Set(C_POS_ROUTER_END);
		}
		break;
	case C_POS_ROUTER_END:
		if(!g_routerF.m_pMtW->IsRdy())
			break;
		if(!g_routerF.m_pMtY->IsRdy())
			break;
		if(!g_routerR.m_pMtW->IsRdy())
			break;
		if(!g_routerR.m_pMtY->IsRdy())
			break;

		m_fsm.Set(C_IDLE);
		break;
	}
}


void CTenkeyOpr::CyclePosVision(void)
{
	// 조건들만 Check 하므로 Error시에 초기화 루틴이 없음
	if(!m_fsm.Between(C_POS_VISION_START, C_POS_VISION_END))
		return;

	if(m_fsm.TimeLimit(600000))
	{
		g_err.Save(ER_TENKEY_POS_VISION_CYCLE_TM_OVER);
		m_fsm.Set(C_IDLE);
		return;
	}

	int nIdx = g_pNV->NDm(mmiRouterIndexNo);

	switch(m_fsm.Get())
	{
	case C_POS_VISION_START:
		{
			if(!Between(nIdx, INDEX_01, INDEX_04))
			{
				m_fsm.Set(C_IDLE);
				break;
			}

			// 확인 후 Z Vision Pos 이동되도록 수정
			if(INDEX_01 == nIdx || INDEX_02 == nIdx)
			{
				if(!g_routerF.m_pMtZ_F->IsRdy(CRouter::PZ_READY) || 
				   !g_routerF.m_pMtZ_R->IsRdy(CRouter::PZ_READY))
				{
					g_routerF.m_pMtZ_F->Move(CRouter::PZ_READY);
					g_routerF.m_pMtZ_R->Move(CRouter::PZ_READY);
				}
			}
			else
			{
				if(!g_routerR.m_pMtZ_F->IsRdy(CRouter::PZ_READY) || 
				   !g_routerR.m_pMtZ_R->IsRdy(CRouter::PZ_READY))
				{
					g_routerR.m_pMtZ_F->Move(CRouter::PZ_READY);
					g_routerR.m_pMtZ_R->Move(CRouter::PZ_READY);
				}
			}
			m_fsm.Set(C_POS_VISION_01);
		}
		break;
	case C_POS_VISION_01:
		{
			if(INDEX_01 == nIdx || INDEX_02 == nIdx)
			{
				if(!g_routerF.m_pMtZ_F->InPos(CRouter::PZ_READY))
					break;
				if(!g_routerF.m_pMtZ_R->InPos(CRouter::PZ_READY))
					break;
			}
			else
			{
				if(!g_routerR.m_pMtZ_F->InPos(CRouter::PZ_READY))
					break;
				if(!g_routerR.m_pMtZ_R->InPos(CRouter::PZ_READY))
					break;
			}

			if(!g_pIndex[nIdx]->CanMove())
			{
				m_fsm.Set(C_IDLE);
				break;
			}

			m_fsm.Set(C_POS_VISION_02);
		}
		break;
	case C_POS_VISION_02:
		{
			int pathNo  = g_pNV->NDm(mmiRouterPathNo);
			int pathPos = g_pNV->NDm(mmiRouterPathPos);

			int nMaxPathCnt = (int)g_pNV->gerberPara(arrayPathCnt) * (int)g_pNV->gerberPara(arrayXCnt) * (int)g_pNV->gerberPara(arrayYCnt);
			if(nMaxPathCnt < pathNo)
			{
				m_fsm.Set(C_IDLE);
				break;
			}

			int nLineType = g_pIndex[nIdx]->GetGerberLineType(pathNo);
			if(-1 == nLineType)
			{
				g_err.Save(ER_GERBER_LINE_TYPE_ERR);
				m_fsm.Set(C_IDLE);
				break;
			}

			XYT xytPos = g_pIndex[nIdx]->GetRouterLiveViPos(pathNo, pathPos);
			g_pIndex[nIdx]->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX);
			g_pIndex[nIdx]->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
			
			if(INDEX_01 == nIdx)
			{
				double dPosW = g_routerF.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				g_routerF.GentryMtYWPMove(CRouter::PY_VI_LIVE_F, CRouter::PW_READY, xytPos.dY, dPosW);
			}
			else if(INDEX_02 == nIdx)
			{
				double dPosW = g_routerF.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				g_routerF.GentryMtYWPMove(CRouter::PY_VI_LIVE_R, CRouter::PW_READY, xytPos.dY, dPosW);
			}
			else if(INDEX_03 == nIdx)
			{
				double dPosW = g_routerR.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				g_routerR.GentryMtYWPMove(CRouter::PY_VI_LIVE_F, CRouter::PW_READY, xytPos.dY, dPosW);
			}
			else if(INDEX_04 == nIdx)
			{
				double dPosW = g_routerR.m_pMtW->m_pTable->pos[CRouter::PW_READY];
				g_routerR.GentryMtYWPMove(CRouter::PY_VI_LIVE_R, CRouter::PW_READY, xytPos.dY, dPosW);
			}

			if(INDEX_01 == nIdx || INDEX_02 == nIdx)
			{
				g_routerF.m_pMtZ_F->Move(CRouter::PZ_PRS);
				g_dOut.On(oViLiveModeRouterF);
			}
			else 
			{
				g_routerR.m_pMtZ_F->Move(CRouter::PZ_PRS);
				g_dOut.On(oViLiveModeRouterR);
			}

			m_fsm.Set(C_POS_VISION_END);
		}
		break;
	case C_POS_VISION_END:
		if(!g_routerF.m_pMtW->IsRdy())
			break;
		if(!g_routerF.m_pMtY->IsRdy())
			break;
		if(!g_routerR.m_pMtW->IsRdy())
			break;
		if(!g_routerR.m_pMtY->IsRdy())
			break;
		m_fsm.Set(C_IDLE);
		break;
	}
}


