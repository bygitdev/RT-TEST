#include "..\def\Includes.h"


//////////////////////////////////////////////////////////////////////////
CMMI g_mmi;
//////////////////////////////////////////////////////////////////////////



//-------------------------------------------------------------------
BOOL CMMI::Init(void)
{
	int bufferSize = sizeof(COMM_BUFFER);
	if(FALSE == m_kamelas.Init(bufferSize))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
void CMMI::Run(void)
{
	static COMM_BUFFER mmiToSeq;

	if(FALSE == m_kamelas.Recv((PBYTE)&mmiToSeq))
		return;

	switch(mmiToSeq.command)
	{
	case CMD_RD_IO:
		{
			static PWORD pTarget, pOrigin;
			int nSize;

			nSize = DI_CH_CNT * sizeof(WORD);
			pTarget = &mmiToSeq.buffer.rdIO.inCH[0];
			pOrigin = &CDIn::m_ch[0];
			memcpy(pTarget, pOrigin, nSize);


			nSize = DO_CH_CNT * sizeof(WORD);
			pTarget = &mmiToSeq.buffer.rdIO.outCH[0];
			pOrigin = &CDOut::m_ch[0];
			memcpy(pTarget, pOrigin, nSize);
		}
		break;
	case CMD_WR_IO:
		{
			if(!g_opr.isStop || g_opr.isPausedStop || g_opr.isEmg)
				break;

			int nIONo = mmiToSeq.buffer.wrIO.nIONo;
			g_dOut.Set(nIONo, mmiToSeq.buffer.wrIO.bOn);
		}
		break;
	case CMD_RD_PKG:
		{
			int nSize = sizeof(double) * 1000;
			double* pOrigin = &g_pNV->m_pData->pkg[0];
			double* pTarget = &mmiToSeq.buffer.pkg.dVal[0];
			memcpy(pTarget, pOrigin, nSize);
		}
		break;
	case CMD_WR_PKG:
		{
			int nStart = mmiToSeq.buffer.pkg.nStart;
			int nEnd = mmiToSeq.buffer.pkg.nEnd;

			if(999 < nEnd)
				nEnd = 999;

			BOOL bErr = (nStart < 0) || (nEnd < nStart) || (999 < nEnd);
			if(bErr)
			{
				SeqLog(L"Err : CMD_WR_PKG");
				break;
			}

			for(int nNo = nStart; nNo <= nEnd; nNo++)
			{
				g_pNV->Pkg(nNo) = mmiToSeq.buffer.pkg.dVal[nNo];
			}
		}
		break;
	case CMD_RD_NDM:
		{
			int nSize = sizeof(int) * 1000;
			int* pOrigin = &g_pNV->m_pData->ndm[0];
			int* pTarget = &mmiToSeq.buffer.ndm.nVal[0];
			memcpy(pTarget, pOrigin, nSize);
		}
		break;
	case CMD_WR_NDM:
		{
			int nStart = mmiToSeq.buffer.ndm.nStart;
			int nEnd = mmiToSeq.buffer.ndm.nEnd;

			if(999 < nEnd)
				nEnd = 999;

			BOOL bErr = (nStart < 0) || (nEnd < nStart) || (999 < nEnd);
			if(bErr)
			{
				SeqLog(L"Err : CMD_WR_NDM");
				break;
			}
			for(int nNo = nStart; nNo <= nEnd; nNo++)
			{
				g_pNV->NDm(nNo) = mmiToSeq.buffer.ndm.nVal[nNo];
			}
		}
		break;
	case CMD_RD_DDM:
		{
			int nSize = sizeof(double) * 1000;
			double* pOrigin = &g_pNV->m_pData->ddm[0];
			double* pTarget = &mmiToSeq.buffer.ddm.dVal[0];
			memcpy(pTarget, pOrigin, nSize);
		}
		break;
	case CMD_WR_DDM:
		{
			int nStart = mmiToSeq.buffer.ddm.nStart;
			int nEnd = mmiToSeq.buffer.ddm.nEnd;

			if(999 < nEnd)
				nEnd = 999;

			BOOL bErr = (nStart < 0) || (nEnd < nStart) || (999 < nEnd);
			if(bErr)
			{
				SeqLog(L"Err : CMD_WR_DDM");
				break;
			}

			for(int nNo = nStart; nNo <= nEnd; nNo++)
			{
				g_pNV->DDm(nNo) = mmiToSeq.buffer.ddm.dVal[nNo];
			}
		}
		break;
	case CMD_RD_USESKIP:
		{
			int nSize = sizeof(int) * 500;
			int* pOrigin = &g_pNV->m_pData->useSkip[0];
			int* pTarget = &mmiToSeq.buffer.useSkip.nVal[0];
			memcpy(pTarget, pOrigin, nSize);
		}
		break;
	case CMD_WR_USESKIP:
		{
			int nStart = mmiToSeq.buffer.useSkip.nStart;
			int nEnd = mmiToSeq.buffer.useSkip.nEnd;

			if(499 < nEnd)
				nEnd = 499;

			BOOL bErr = (nStart < 0) || (nEnd < nStart) || (499 < nEnd);
			if(bErr)
			{
				SeqLog(L"Err : CMD_WR_USESKIP");
				break;
			}
			for(int nNo = nStart; nNo <= nEnd; nNo++)
			{
				g_pNV->UseSkip(nNo) = mmiToSeq.buffer.useSkip.nVal[nNo];
			}
		}
		break;
	case CMD_RD_MT_STATUS:
		{
			int nMtNo = mmiToSeq.buffer.motStatus.nMtNo;

			if(!Between(nMtNo, 0, (MAX_MT_NO - 1)))
				break;

			mmiToSeq.buffer.motStatus.bServoOn	= g_mt[nMtNo].m_state.isServoOn;
			mmiToSeq.buffer.motStatus.bAlarm	= g_mt[nMtNo].m_state.isAlarm;
			mmiToSeq.buffer.motStatus.bDriving	= g_mt[nMtNo].m_state.isDriving;
			mmiToSeq.buffer.motStatus.bPaused	= g_mt[nMtNo].m_state.isPaused;
			mmiToSeq.buffer.motStatus.bHome		= g_mt[nMtNo].m_state.isHome;
			mmiToSeq.buffer.motStatus.bHoming	= g_mt[nMtNo].m_state.isHoming;
			mmiToSeq.buffer.motStatus.bCw		= g_mt[nMtNo].m_state.isCw;
			mmiToSeq.buffer.motStatus.bCCw		= g_mt[nMtNo].m_state.isCCw;
			mmiToSeq.buffer.motStatus.bOrg		= g_mt[nMtNo].m_state.isOrg;
			mmiToSeq.buffer.motStatus.nCurIndex = g_mt[nMtNo].m_profile.curIndex;
			mmiToSeq.buffer.motStatus.dRealCnt	= g_mt[nMtNo].m_state.realCnt;
		}
		break;
	case CMD_RD_MT_TABLE:
		{
			int nMtNo = mmiToSeq.buffer.motTable.nMtNo;

			if(!Between(nMtNo, 0, (MAX_MT_NO - 1)))
				break;

			for(int nIndex = 0; nIndex < 50; nIndex++)
			{
				mmiToSeq.buffer.motTable.dPos[nIndex] = g_pNV->m_pMotTable[nMtNo].pos[nIndex];
				mmiToSeq.buffer.motTable.dVel[nIndex] = g_pNV->m_pMotTable[nMtNo].vel[nIndex];
				mmiToSeq.buffer.motTable.dAcc[nIndex] = g_pNV->m_pMotTable[nMtNo].acc[nIndex];
			}
		}
		break;
	case CMD_WR_MT_TABLE:
		{
			int nMtNo = mmiToSeq.buffer.motTable.nMtNo;

			if(!Between(nMtNo, 0, (MAX_MT_NO - 1)))
				break;

			for(int nIndex = 0; nIndex < 50; nIndex++)
			{
				g_pNV->m_pMotTable[nMtNo].pos[nIndex] = mmiToSeq.buffer.motTable.dPos[nIndex];
				g_pNV->m_pMotTable[nMtNo].vel[nIndex] = mmiToSeq.buffer.motTable.dVel[nIndex];
				g_pNV->m_pMotTable[nMtNo].acc[nIndex] = mmiToSeq.buffer.motTable.dAcc[nIndex];
			}

			copy2Mtd();
		}
		break;
	case CMD_WR_MT_CMD:
		{
			if(!g_opr.isStop || g_opr.isPausedStop || g_opr.isEmg)
				break;
			MtControl(mmiToSeq.buffer.motControl);
		}
		break;
	case CMD_RD_ERR_WR:
		{
			for(int i = 0; i < 10; i++)
			{
				mmiToSeq.buffer.errWr.err[i] = g_err.m_err[i];
				mmiToSeq.buffer.errWr.wr[i]  = g_wr.m_err[i];
			}
		}
		break;
	case CMD_WR_INPNP_PRSVI:
		break;
	case CMD_WR_ROUTER_F_PRSVI:
		g_routerF.m_viPrsData = mmiToSeq.buffer.prsRouterResultF;
		break;
	case CMD_WR_ROUTER_R_PRSVI:
		g_routerR.m_viPrsData = mmiToSeq.buffer.prsRouterResultR;
		break;
	case CMD_WR_QC_VI:
		break;
	case CMD_WR_TOP_VI:
		break;
	case CMD_WR_BTM_VI:
		break;

	case CMD_RD_INDEX01_PCB:
		break;
	case CMD_WR_INDEX01_PCB:
		break;
	case CMD_RD_INDEX02_PCB:
		break;
	case CMD_WR_INDEX02_PCB:
		break;
	case CMD_RD_INDEX03_PCB:
		break;
	case CMD_WR_INDEX03_PCB:
		break;
	case CMD_RD_INDEX04_PCB:
		break;
	case CMD_WR_INDEX04_PCB:
		break;
	case CMD_RD_GERBER_PARA:
		{
			int nSize = sizeof(double) * 1000;
			double* pOrigin = &g_pNV->m_pGerber->para[0];
			double* pTarget = &mmiToSeq.buffer.gerberPara.dVal[0];
			memcpy(pTarget, pOrigin, nSize);
		}
		break;
	case CMD_WR_GERBER_PARA:
		{
			int nStart = mmiToSeq.buffer.gerberPara.nStart;
			int nEnd = mmiToSeq.buffer.gerberPara.nEnd;

			if(999 < nEnd)
				nEnd = 999;

			BOOL bErr = (nStart < 0) || (nEnd < nStart) || (999 < nEnd);
			if(bErr)
			{
				SeqLog(L"Err : CMD_WR_GERBER_PARA");
				break;
			}
			// 저장만 하고 잘 Download 되었는지만 확인
			for(int nNo = nStart; nNo <= nEnd; nNo++)
			{
				g_pNV->gerberPara(nNo) = mmiToSeq.buffer.gerberPara.dVal[nNo];
			}
		}
		break;

	case CMD_RD_GERBER_DATA:
		{
			// 모두다 Upload
			int nSize = sizeof(GERBER) * 500;
			GERBER* pOrigin = (GERBER*)&g_pNV->m_pGerber->data[0];
			GERBER* pTarget = &mmiToSeq.buffer.gerberData.data[0];
			memcpy(pTarget, pOrigin, nSize);
		}
		break;

	case CMD_WR_GERBER_DATA:
		{
			int nStart = mmiToSeq.buffer.gerberData.nStart; // GERBER 단위
			int nEnd = mmiToSeq.buffer.gerberData.nEnd;

			if(499 < nEnd)
				nEnd = 499;

			BOOL bErr = (nStart < 0) || (nEnd < nStart) || (499 < nEnd);
			if(bErr)
			{
				SeqLog(L"Err : CMD_WR_GERBER_DATA");
				break;
			}
			
			// 저장만 하고 잘 Download 되었는지만 확인
			for(int nNo = nStart; nNo <= nEnd; nNo++)
			{
				g_pGerberPath->data[nNo] = mmiToSeq.buffer.gerberData.data[nNo];
			}
		}
		break;

	case CMD_RD_LOT_INFO:
		{
			int nNo = mmiToSeq.buffer.lotInfo.part;
			int nSize = sizeof(LotInfo);
			LotInfo* pOrigin = (LotInfo*)&g_pNV->m_pLotInfo->data[nNo];
			LotInfo* pTarget = &mmiToSeq.buffer.lotInfo.data;
			memcpy(pTarget, pOrigin, nSize);
		}
		break;

	case CMD_WR_LOT_INFO:
		{
			int nNo = mmiToSeq.buffer.lotInfo.part;
			
			BOOL bErr = (nNo < LOT_INFO_MGZ) || (LOT_INFO_OUTPNP < nNo);
			if(bErr)
			{
				SeqLog(L"Err : CMD_WR_LOT_INFO");
				break;
			}
			
			g_pNV->m_pLotInfo->data[nNo] = (LotInfo)mmiToSeq.buffer.lotInfo.data;
		}
		break;
	
	case CMD_RD_LOT_HISTORY:
		{
			int nNo = mmiToSeq.buffer.lotHistory.part;
			int nSize = sizeof(LotHistory);
			LotHistory* pOrigin = (LotHistory*)&g_pNV->m_pLotInfo->history[nNo];
			LotHistory* pTarget = &mmiToSeq.buffer.lotHistory.history;
			memcpy(pTarget, pOrigin, nSize);
		}
		break;

	case CMD_WR_LOT_HISTORY:
		{
			int nNo = mmiToSeq.buffer.lotHistory.part;
			
			BOOL bErr = (nNo < 0) || ((LOT_INFO_MAX_CNT-1) < nNo);
			if(bErr)
			{
				SeqLog(L"Err : CMD_WR_LOT_HISTORY");
				break;
			}
			
			g_pNV->m_pLotInfo->history[nNo] = (LotHistory)mmiToSeq.buffer.lotHistory.history;
		}
		break;
	case CMD_RD_LOT_SPLIT:
		{
			int nSize = sizeof(LotSplitInfo);
			LotSplitInfo* pOrigin = (LotSplitInfo*)&g_pNV->m_pLotInfo->split;
			LotSplitInfo* pTarget = &mmiToSeq.buffer.lotSplitInfo.split;
			memcpy(pTarget, pOrigin, nSize);
		}
		break;
	case CMD_WR_LOT_SPLIT:
		{
			g_pNV->m_pLotInfo->split = (LotSplitInfo)mmiToSeq.buffer.lotSplitInfo.split;
		}
		break;

	// ETC
	case CMD_WR_REGULATOR:
		{
			g_ao.m_dVolt[aoOutPnpVacVolt] = g_pNV->Pkg(OutPnpVacSetValue);
		}
		break;
	}

	if(FALSE == m_kamelas.Send((PBYTE)&mmiToSeq))
	{
		SeqLog(L"Failed to send data");
	}
}


//-------------------------------------------------------------------
// Motor control 
void CMMI::MtControl(CommMtControl motCommand)
{
	int nMtNo = motCommand.nMtNo;
	if(!Between(nMtNo, 0, (MAX_MT_NO - 1)))
		return;

	int nJogVel = g_pNV->NDm(jogSpeed) * 1000;

	switch(motCommand.nCmd)
	{
	case MTCMD_SERVO_ON:
		g_mt[nMtNo].ServoOn();
		break;
	case MTCMD_SERVO_OFF:
		g_mt[nMtNo].ServoOff();
		break;
	case MTCMD_INDEX_MOVE:
		{
			BOOL bMtOffsetMove  = (MT_PUSHER_X == nMtNo); 
				 bMtOffsetMove |= (MT_RAIL_GRIPPER_X == nMtNo);
				 bMtOffsetMove |= (MT_INPNP_Y == nMtNo);
				 bMtOffsetMove |= (MT_INPNP_Z == nMtNo);
				 bMtOffsetMove |= (MT_INDEX_X_01 == nMtNo);
				 bMtOffsetMove |= (MT_INDEX_X_02 == nMtNo);
				 bMtOffsetMove |= (MT_INDEX_X_03 == nMtNo);
				 bMtOffsetMove |= (MT_INDEX_X_04 == nMtNo);
				 bMtOffsetMove |= (MT_OUTPNP_Y == nMtNo);
				 bMtOffsetMove |= (MT_OUTPNP_Z == nMtNo);
				 bMtOffsetMove |= (MT_OUTPNP_X == nMtNo);
				 bMtOffsetMove &= (g_pNV->NDm(flagMtIndexOffsetMove));

			g_pNV->NDm(flagMtIndexOffsetMove) = 0; // Clear

			if(g_opr.isDoorOpen)
			{
				g_err.Door(FALSE);
				break;
			}

			if(!g_err.ChkMtSafety(nMtNo))
				break;

			if (MT_LD_Y == nMtNo) // Loader MGZ Y축이 개별 홈 잡을때,
			{
				if (!g_mt[MT_PUSHER_X].InPos(CLdMz::PX_BWD)) // Pusher X 축이 BWD 상태가 아니라면,
				{
					g_err.Save(ER_MT_INDEX_MOVE_LD_Y); // 에러띄움
					break;
				}
			}
			else if (MT_LD_Z == nMtNo) // Loader MGZ Z축이 개별 홈 잡을때,
			{
				if (!g_mt[MT_PUSHER_X].InPos(CLdMz::PX_BWD)) // Pusher X 축이 BWD 상태가 아니라면,
				{
					g_err.Save(ER_MT_INDEX_MOVE_LD_Z); // 에러띄움
					break;
				}
			}

			// Index Move시에 안전 조건 처리
			if(!g_err.ChkMtIndexMove(nMtNo, motCommand.nCmdIndexNo))
				break;

			if(bMtOffsetMove)
				MtIndexOffsetMove(motCommand);
			else
				g_mt[nMtNo].Move(motCommand.nCmdIndexNo);
		}
		break;
	case MTCMD_JOG_MOVE:
		if(g_opr.isDoorOpen)
		{
			g_err.Door(FALSE);
			break;
		}

		if(!g_err.ChkMtSafety(nMtNo))
			break;

		if(motCommand.nDir)
			g_mt[nMtNo].RMove(1500000, nJogVel); //1회 최대 이동량은 200mm
		else
			g_mt[nMtNo].RMove(-1500000, nJogVel); //1회 최대 이동량은 200mm
		break;
	case MTCMD_JOG_STOP:
		g_mt[nMtNo].Stop(TRUE);
		break;
	case MTCMD_R_MOVE:
		if(g_opr.isDoorOpen)
		{
			g_err.Door(FALSE);
			break;
		}

		if(!g_err.ChkMtSafety(nMtNo))
			break;

		if(motCommand.nDir)
			g_mt[nMtNo].RMove((int)motCommand.dPulse, nJogVel); //1회 최대 이동량은 200mm
		else
			g_mt[nMtNo].RMove((int)(motCommand.dPulse * (-1.0)), nJogVel); //1회 최대 이동량은 200mm
		break;
	case MTCMD_A_MOVE:
		{
			if(g_opr.isDoorOpen)
			{
				g_err.Door(FALSE);
				break;
			}

			if(!g_err.ChkMtSafety(nMtNo))
				break;

			int nAMovePos = (int)(motCommand.dPulse - g_mt[nMtNo].m_state.cmdCnt);
			g_mt[nMtNo].RMove(nAMovePos, nJogVel);
		}
		break;
	case MTCMD_ALL_HOME:
		break;
	case MTCMD_HOME:
		if(g_opr.isDoorOpen)
		{
			g_err.Door(FALSE);
			break;
		}

		if(!g_err.ChkMtSafety(nMtNo))
			break;

		if (MT_LD_Y == nMtNo) // Loader MGZ Y축이 개별 홈 잡을때,
		{
			if (!g_mt[MT_PUSHER_X].InPos(CLdMz::PX_BWD)) // Pusher X 축이 BWD 상태가 아니라면,
			{
				g_err.Save(ER_MT_INDIVIDUAL_HOME_LD_Y); // 에러띄움
				break;
			}
		}
		else if (MT_LD_Z == nMtNo) // Loader MGZ Z축이 개별 홈 잡을때,
		{
			if (!g_mt[MT_PUSHER_X].InPos(CLdMz::PX_BWD)) // Pusher X 축이 BWD 상태가 아니라면,
			{
				g_err.Save(ER_MT_INDIVIDUAL_HOME_LD_Z); // 에러띄움
				break;
			}
		}
		else if (MT_INDEX_X_01 == nMtNo) // Index X1 축이 개별 홈 잡을때,
		{
			if (!g_mt[MT_SPINDLE_Z_01].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_01);
				break;
			}
			if (!g_mt[MT_SPINDLE_Z_02].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_02);
				break;
			}
			if (pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].GetPos(300))	// Bit Clamp 실린더가 Up 상태가 아니라면,
			{
				g_err.Save(ER_CYL_ROUTER_BIT_CHANGE_CLAMP_NOT_UP_01);
				break;
			}
			if (!g_mt[MT_OUTPNP_Z].InPos(COutPnp::PZ_READY)) // Outpnp Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_MT_OUTPNP_Z_NOT_READY_POS);
				break;
			}
		}
		else if (MT_INDEX_X_02 == nMtNo) // Index X2 축이 개별 홈 잡을때,
		{
			if (!g_mt[MT_SPINDLE_Z_01].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_01);
				break;
			}
			if (!g_mt[MT_SPINDLE_Z_02].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_02);
				break;
			}
			if (pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].GetPos(300))	// Bit Clamp 실린더가 Up 상태가 아니라면,
			{
				g_err.Save(ER_CYL_ROUTER_BIT_CHANGE_CLAMP_NOT_UP_01);
				break;
			}
			if (!g_mt[MT_OUTPNP_Z].InPos(COutPnp::PZ_READY)) // Outpnp Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_MT_OUTPNP_Z_NOT_READY_POS);
				break;
			}
		}
		else if (MT_INDEX_X_03 == nMtNo) // Index X3 축이 개별 홈 잡을때,
		{
			if (!g_mt[MT_SPINDLE_Z_03].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_03);
				break;
			}
			if (!g_mt[MT_SPINDLE_Z_04].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_04);
				break;
			}
			if (pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].GetPos(300))	// Bit Clamp 실린더가 Up 상태가 아니라면,
			{
				g_err.Save(ER_CYL_ROUTER_BIT_CHANGE_CLAMP_NOT_UP_02);
				break;
			}
			if (!g_mt[MT_OUTPNP_Z].InPos(COutPnp::PZ_READY)) // Outpnp Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_MT_OUTPNP_Z_NOT_READY_POS);
				break;
			}
		}
		else if (MT_INDEX_X_04 == nMtNo) // Index X4 축이 개별 홈 잡을때,
		{
			if (!g_mt[MT_SPINDLE_Z_03].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_03);
				break;
			}
			if (!g_mt[MT_SPINDLE_Z_04].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_04);
				break;
			}
			if (pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].GetPos(300))	// Bit Clamp 실린더가 Up 상태가 아니라면,
			{
				g_err.Save(ER_CYL_ROUTER_BIT_CHANGE_CLAMP_NOT_UP_02);
				break;
			}
			if (!g_mt[MT_OUTPNP_Z].InPos(COutPnp::PZ_READY)) // Outpnp Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_MT_OUTPNP_Z_NOT_READY_POS);
				break;
			}
		}
		else if (MT_INPNP_Y == nMtNo) // Inpnp Y 축이 개별 홈 잡을때,
		{
			if (!g_mt[MT_INPNP_Z].InPos(CInPnp::PZ_READY)) // Inpnp Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(WR_MT_INPNP_Z_NOT_READY_POS); // 에러띄움
				break;
			}
		}
		else if (MT_ROUTER_Y_01 == nMtNo) // Router Y1 축이 개별 홈 잡을때,
		{
			if (!g_mt[MT_SPINDLE_Z_01].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_01);
				break;
			}
			if (!g_mt[MT_SPINDLE_Z_02].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_02);
				break;
			}
			if (pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].GetPos(300))	// Bit Clamp 실린더가 Up 상태가 아니라면,
			{
				g_err.Save(ER_CYL_ROUTER_BIT_CHANGE_CLAMP_NOT_UP_01);
				break;
			}
		}
		else if (MT_ROUTER_Y_02 == nMtNo) // Router Y2 축이 개별 홈 잡을때,
		{
			if (!g_mt[MT_SPINDLE_Z_03].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_03);
				break;
			}
			if (!g_mt[MT_SPINDLE_Z_04].IsRdy(CRouter::PZ_READY))	// Spindle Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_ROUTER_Z_NOT_READY_POS_04);
				break;
			}
			if (pmUP != g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].GetPos(300))	// Bit Clamp 실린더가 Up 상태가 아니라면,
			{
				g_err.Save(ER_CYL_ROUTER_BIT_CHANGE_CLAMP_NOT_UP_02);
				break;
			}
		}
		else if (MT_OUTPNP_Y == nMtNo) // Outpnp Y 축이 개별 홈 잡을때,
		{
			if (!g_mt[MT_OUTPNP_Z].InPos(COutPnp::PZ_READY)) // Outpnp Z 축이 Ready 상태가 아니라면,
			{
				g_err.Save(ER_MT_OUTPNP_Z_NOT_READY_POS);
				break;
			}
		}

		g_mt[nMtNo].CancelHomeSearch();
		g_mt[nMtNo].StartHomeSearch();
		break;
	case MTCMD_RESET:
		g_mt[nMtNo].AlarmClear();
		break;
	case MTCMD_STOP:
		g_mt[nMtNo].Stop(FALSE);
		break;
	}
}


void CMMI::MtIndexOffsetMove(CommMtControl motCommand)
{
	int nMtNo = motCommand.nMtNo;
	int nMtIdxNo = motCommand.nCmdIndexNo;
	if(!Between(nMtNo, 0, (MAX_MT_NO - 1)))
		return;

	double dOffset = 0.0;	
	double dPos	   = 0.0;

	switch(nMtNo)
	{
	case MT_PUSHER_X:
		if(CLdMz::PX_FWD == nMtIdxNo)
		{
			dOffset  = g_pNV->Pkg(offsetLdMzPusherXFwd) * 1000.0;
			dOffset += (g_pNV->DDm(commonPcbLength) - g_pNV->Pkg(pcbXLength)) * 1000.0;
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else if(CLdMz::PX_2D == nMtIdxNo)
		{
			dOffset  = g_pNV->Pkg(offsetLdMzPusherX2D) * 1000.0;	
			dOffset += (g_pNV->DDm(commonPcbLength) - g_pNV->Pkg(pcbXLength)) * 1000.0;
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else
			g_mt[nMtNo].Move(nMtIdxNo);
		break;
	case MT_RAIL_GRIPPER_X:
		if(CRail::PX_RCV_END == nMtIdxNo) 
		{
			dOffset  = g_pNV->Pkg(offsetRailXRcvEnd) * 1000.0;	
			dOffset += (g_pNV->DDm(commonPcbUnitCenterToEdgeXLength) - g_pNV->Pkg(pcbUnitCenterToEdgeXLength)) * 1000.0;	
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else if(CRail::PX_ALIGN == nMtIdxNo) 
		{
			dOffset  = g_pNV->Pkg(offsetRailXAlign) * 1000.0;	
			dOffset += (g_pNV->DDm(commonPcbUnitCenterToEdgeXLength) - g_pNV->Pkg(pcbUnitCenterToEdgeXLength)) * 1000.0;	
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else if (CRail::PX_2D == nMtIdxNo)
		{
			dOffset = g_pNV->Pkg(offsetRailX2D) * 1000.0;
			dPos = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] - dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else
			g_mt[nMtNo].Move(nMtIdxNo); 
		break;
	case MT_INPNP_Y:
		if(Between(nMtIdxNo, CInPnp::PY_PUTDN_01, CInPnp::PY_PUTDN_04))
		{
			int nIndex = nMtIdxNo - CInPnp::PY_PUTDN_01; // 0, 1, 2, 3
			dOffset = g_pNV->Pkg(offsetInPnpYPutDn1 + nIndex) * 1000.0;	
			dPos	= g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else
			g_mt[nMtNo].Move(nMtIdxNo);
		break;
	case MT_INPNP_Z:
		if(Between(nMtIdxNo, CInPnp::PZ_PCB_PUTDN_01, CInPnp::PZ_PCB_PUTDN_04))
		{
			int nIndex = nMtIdxNo - CInPnp::PZ_PCB_PUTDN_01; // 0, 1, 2, 3
			dOffset = g_pNV->Pkg(offsetInPnpZPutDn1 + nIndex) * 1000.0;
			dPos	= g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else
			g_mt[nMtNo].Move(nMtIdxNo);
		break;
	case MT_INDEX_X_01:
		if(CIndex::PX_IN_PNP == nMtIdxNo)
		{
			dOffset  = g_pNV->Pkg(offsetIndexXInPnp1) * 1000.0;	
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else if(CIndex::PX_OUT_PNP == nMtIdxNo)
		{
			dOffset  = g_pNV->Pkg(offsetIndexXOutPnp1) * 1000.0;	
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else
			g_mt[nMtNo].Move(nMtIdxNo);
		break;
	case MT_INDEX_X_02:
		if(CIndex::PX_IN_PNP == nMtIdxNo)
		{
			dOffset  = g_pNV->Pkg(offsetIndexXInPnp2) * 1000.0;	
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else if(CIndex::PX_OUT_PNP == nMtIdxNo)
		{
			dOffset  = g_pNV->Pkg(offsetIndexXOutPnp2) * 1000.0;	
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else
			g_mt[nMtNo].Move(nMtIdxNo);
		break;
	case MT_INDEX_X_03:
		if(CIndex::PX_IN_PNP == nMtIdxNo)
		{
			dOffset  = g_pNV->Pkg(offsetIndexXInPnp3) * 1000.0;	
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else if(CIndex::PX_OUT_PNP == nMtIdxNo)
		{
			dOffset  = g_pNV->Pkg(offsetIndexXOutPnp3) * 1000.0;	
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else
			g_mt[nMtNo].Move(nMtIdxNo);		
		break;
	case MT_INDEX_X_04:
		if(CIndex::PX_IN_PNP == nMtIdxNo)
		{
			dOffset  = g_pNV->Pkg(offsetIndexXInPnp4) * 1000.0;	
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else if(CIndex::PX_OUT_PNP == nMtIdxNo)
		{
			dOffset  = g_pNV->Pkg(offsetIndexXOutPnp4) * 1000.0;	
			dPos	 = g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else
			g_mt[nMtNo].Move(nMtIdxNo);
		break;
	case MT_OUTPNP_Y:
		if(Between(nMtIdxNo, COutPnp::PY_PICKUP_01, COutPnp::PY_PICKUP_04))
		{
			int nIndex = nMtIdxNo - COutPnp::PY_PICKUP_01; // 0, 1, 2, 3
			dOffset = g_pNV->Pkg(offsetOutPnpYPickUp1 + nIndex) * 1000.0;	
			dPos	= g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else if(Between(nMtIdxNo, COutPnp::PY_PUTDN_01, COutPnp::PY_PUTDN_04))
		{
			int nIndex = nMtIdxNo - COutPnp::PY_PUTDN_01; // 0, 1, 2, 3
			dOffset = g_pNV->Pkg(offsetOutPnpYPutDn1 + nIndex) * 1000.0;	
			dPos	= g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else 
			g_mt[nMtNo].Move(nMtIdxNo);
		break;
	case MT_OUTPNP_Z:
		if(Between(nMtIdxNo, COutPnp::PZ_PICKUP_01, COutPnp::PZ_PICKUP_04))
		{
			int nIndex = nMtIdxNo - COutPnp::PZ_PICKUP_01; // 0, 1, 2, 3
			dOffset = g_pNV->Pkg(offsetOutPnpZPickUp1 + nIndex) * 1000.0;	
			dPos	= g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else if(Between(nMtIdxNo, COutPnp::PZ_PUTDN_01, COutPnp::PZ_PUTDN_04))
		{
			int nIndex = nMtIdxNo - COutPnp::PZ_PUTDN_01; // 0, 1, 2, 3
			dOffset = g_pNV->Pkg(offsetOutPnpZPutDn1 + nIndex) * 1000.0;	
			dPos	= g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else 
			g_mt[nMtNo].Move(nMtIdxNo);
		break;
	case MT_OUTPNP_X:
		if(Between(nMtIdxNo, COutPnp::PX_SORTER_01, COutPnp::PX_SORTER_04))
		{
			int nIndex = nMtIdxNo - COutPnp::PX_SORTER_01; // 0, 1, 2, 3
			dOffset = g_pNV->Pkg(offsetOutPnpXSorter1 + nIndex) * 1000.0;	
			dPos	= g_mt[nMtNo].m_pTable->pos[nMtIdxNo] + dOffset;
			g_mt[nMtNo].PMove(nMtIdxNo, dPos);
		}
		else 
			g_mt[nMtNo].Move(nMtIdxNo);
		break;
	}
}
