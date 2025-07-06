#include "..\DEF\Includes.h"


/////////////////////////////////////////////////////////////////////
CLdMz g_ldMz;
/////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
CLdMz::CLdMz()
{
	m_bRun				  = FALSE;
	m_bReworkMz			  = FALSE;
	m_bCompAlign		  = FALSE;
	m_bCompRfidRead		  = FALSE;
	m_bCompPartNoCompare  = FALSE;
	m_bCompAutoRecipeChg  = FALSE;
	m_bCompCarrierIdRead  = FALSE;
	m_bCompMergeInfo	  = FALSE;
	m_bCompTrayInfo		  = FALSE;
	m_bCompWork			  = FALSE;
	m_bCompRfidWrite	  = FALSE;
	m_bCompRfidWriteCheck = FALSE;

	//CurSlotNo()		= 0;
	//CmdSlotNo()		= 1;
	m_nRfidRetryCnt	= 0;
	m_nTcRetryCnt	= 0;
	m_bManualOut	= FALSE;
}


//-------------------------------------------------------------------
void CLdMz::AutoRun(void)
{
	if(g_dIn.BOn(iMzClampPcbJam) || g_dIn.AOn(iRailExistStart))
	{
		m_tmMouthSen.Reset();

		if(g_opr.isStop)
			m_tmMouthSenWarn.Reset();
	}
	else
	{
		m_tmMouthSenWarn.Reset();
	}

	if(!m_pMtX->m_state.isOrg)
	{
		if(m_tmPusherWarn.TmOver(60000))
			m_fsm.Set(C_ERROR, ER_PUSHER_X_NOT_BWD_POS);

		if(g_opr.isStop)
			m_tmPusherWarn.Reset();
	}
	else
	{
		m_tmPusherWarn.Reset();
	}

	BOOL bCycleOff  = !g_rail.m_fsm.Between(CRail::C_RCV_START, CRail::C_RCV_END);
		 bCycleOff &= !m_fsm.IsRun();

	if(!Exist() && bCycleOff)
	{
		m_bReworkMz				= FALSE;

		m_bCompAlign		  = FALSE;
		m_bCompRfidRead		  = FALSE;
		m_bCompPartNoCompare  = FALSE;
		m_bCompAutoRecipeChg  = FALSE;
		m_bCompCarrierIdRead  = FALSE;
		m_bCompMergeInfo	  = FALSE;
		m_bCompTrayInfo		  = FALSE;
		m_bCompWork			  = FALSE;
		m_bCompRfidWrite	  = FALSE;
		m_bCompRfidWriteCheck = FALSE;
		m_bNewMz			  = TRUE;
		m_bNewRailInfo		  = TRUE;

		CurSlotNo()				= 0;
		CmdSlotNo()				= 1;
		m_nRfidRetryCnt			= 0;
	}

	// Cycle 동작중임을 Main 화면 LED로 표시
	if(m_fsm.Between(C_EJECT_START, C_EJECT_END))
		g_pNV->NDm(mmiBtnEjectLdMz) = TRUE;
	else
		g_pNV->NDm(mmiBtnEjectLdMz) = FALSE;

	if(!m_bRun)
		return;

	m_bRun = FALSE;

	// Auto 시에는 항상 Off
	g_ldMz.m_bManualOut = TRUE;

	if(m_fsm.IsRun())
		return;

	if(g_err.m_bLdSafetyBeam)
		return;

	if(g_rail.m_fsm.Between(CRail::C_RCV_START, CRail::C_RCV_END))
		return;

	int existErrVal = GetExistErr();

	if(EXIST_UNCERTAIN == existErrVal)
		return;
	if(EXIST_ERR == existErrVal)
	{
		g_err.Save(ER_LD_MZ_EXIST);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;
	
	if(!m_pMtX->InPos(PX_BWD) || !m_pMtX->m_state.isOrg)
	{
		m_pMtX->Move(PX_BWD);
		return;
	}

	//if(g_pNV->Pkg(PcbLengthOptionUse) == 1)
	//{
		if(pmBWD != g_rail.m_pCylGripFB->GetPos(300))
		{
			g_rail.m_pCylGripFB->Actuate(pmBWD); 
		}
	//}

	if(Exist())
	{
		m_pCylClampOC->Actuate(pmCLOSE);
		if(pmCLOSE != m_pCylClampOC->GetPos(300)) 
			return;
	}

	m_pCylAlignFB->Actuate(pmBWD);
	if(pmBWD != m_pCylAlignFB->GetPos(50))
		return;

	if(m_tmMouthSenWarn.TmOver(60000))
		m_fsm.Set(C_ERROR, ER_LOADER_PCB_JAM);

	if(g_dIn.BOn(iMzClampPcbJam) || g_dIn.AOn(iRailExistStart) || !m_tmMouthSen.TmOver(3000))
		return;

	int ZSlotNum = (CmdSlotNo()-1)%(int)g_pNV->Pkg(mzSlotZCnt);
	int YSlotNum = (CmdSlotNo()-1)/(int)g_pNV->Pkg(mzSlotZCnt);

	double slotPosZ = GetMzZSlotPos(ZSlotNum);
	double slotPosY = GetMzYSlotPos(YSlotNum);
	int Total_SlotNum = (int)g_pNV->Pkg(mzSlotYCnt)*(int)g_pNV->Pkg(mzSlotZCnt);
	switch(GetState())
	{
	case S_IDLE:
		break;
	case S_LOADING_EXCEPTION:
		m_fsm.Set(C_LOADING_START);
		break;
	case S_EJECT_EXCEPTION:
		m_fsm.Set(C_EJECT_START);
		break;
	case S_LOADING:
		if(m_pMtZ->InPos(PZ_RCV))
		{
			if(g_ldMzInConv.IsReadyMzIn())
				m_fsm.Set(C_LOADING_START);
		}
		else
		{
			m_pMtZ->Move(PZ_RCV);

			_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
			_sprintf(cMaterialId, L"$");
			_sprintf(cMaterialType, L"MZ");	

			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RCV])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RCV] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_RCV_LOADING", g_data2c.cEtc.start, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
					g_data2c.cLdmz.Z[PZ_RCV][_POSIDX_], g_data2c.cLdmz.Z[PZ_RCV][_POS_], 
					g_data2c.cLdmz.Z[PZ_RCV][_SPDIDX_], g_data2c.cLdmz.Z[PZ_RCV][_SPD_], 
					g_data2c.cLdmz.Z[PZ_RCV][_ACCIDX_], g_data2c.cLdmz.Z[PZ_RCV][_ACC_]))
			}
		}
		break;
	case S_ALIGN:
		if(!g_pNV->UseSkip(usMzClampAlign))
		{
			m_bCompAlign = TRUE;
			break;
		}		

		_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
		_sprintf(cMaterialId, L"$");
		_sprintf(cMaterialType, L"MZ");	

		if(m_pMtZ->InPos(PZ_ALIGN))
		{
			if(m_pMtY->InPos(PY_ALIGN))
				m_fsm.Set(C_ALIGN_START);
			else
			{
				m_pMtY->Move(PY_ALIGN);

				if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ALIGN])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ALIGN] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_ALIGN", g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
						g_data2c.cLdmz.Y[PY_ALIGN][_POSIDX_], g_data2c.cLdmz.Y[PY_ALIGN][_POS_], 
						g_data2c.cLdmz.Y[PY_ALIGN][_SPDIDX_], g_data2c.cLdmz.Y[PY_ALIGN][_SPD_], 
						g_data2c.cLdmz.Y[PY_ALIGN][_ACCIDX_], g_data2c.cLdmz.Y[PY_ALIGN][_ACC_]))
				}
			}
		}
		else
		{
			m_pMtZ->Move(PZ_ALIGN);

			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_ALIGN])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_ALIGN] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_ALIGN", g_data2c.cEtc.start, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
					g_data2c.cLdmz.Z[PZ_ALIGN][_POSIDX_], g_data2c.cLdmz.Z[PZ_ALIGN][_POS_], 
					g_data2c.cLdmz.Z[PZ_ALIGN][_SPDIDX_], g_data2c.cLdmz.Z[PZ_ALIGN][_SPD_], 
					g_data2c.cLdmz.Z[PZ_ALIGN][_ACCIDX_], g_data2c.cLdmz.Z[PZ_ALIGN][_ACC_]))
			}
		}
		break;
	case S_RFID_READ:
		if(g_opr.isDryRun)
		{
			m_bCompRfidRead = TRUE;
			break;
		}

		if(!g_pNV->UseSkip(usRfid))
		{
			m_bCompRfidRead = TRUE;
			break;
		}

		if(m_pMtZ->InPos(PZ_RFID))
		{
			if(m_pMtY->InPos(PY_RFID))
				m_fsm.Set(C_RFID_READ_START);
			else
				m_pMtY->Move(PY_RFID);
		}
		else
			m_pMtZ->Move(PZ_RFID);
		break;
	case S_PART_NO_COMPARE:
		if(g_opr.isDryRun)
		{
			m_bCompPartNoCompare = TRUE;
			break;
		}

		if(!g_pNV->UseSkip(usRfid) || !g_pNV->UseSkip(usRfidPartNoCompare))
		{
			m_bCompPartNoCompare = TRUE;
			break;
		}

		m_fsm.Set(C_PART_NO_COMPARE_START);
		break;
	case S_AUTO_RECIPE_CHG:
		if(g_opr.isDryRun)
		{
			m_bCompAutoRecipeChg = TRUE;
			break;
		}

		if(!g_pNV->UseSkip(usRfid) || !g_pNV->UseSkip(usRfidPartNoCompare) || !g_pNV->UseSkip(usAutoRecipeChg))
		{
			m_bCompAutoRecipeChg = TRUE;
			g_pNV->NDm(needAutoRecipeChg) = FALSE; // Recipe Chg가 필요한 상황이어도 Clear 함
			break;
		}

		if(!g_pNV->NDm(needAutoRecipeChg))
		{
			m_bCompAutoRecipeChg = TRUE;
			break;
		}

		if(g_dIn.AOn(iSorterStageAllEmpty)) // Sorter Empty 확인 후에 Cycle 갈 수 있도록 수정
		{
			if(g_dIn.AOn(iSorterLotEndComp)) // Lot End 및 Motor All Stop 상태 확인 후에 Auto Change
			{
				m_fsm.Set(C_AUTO_RECIPE_CHG_START);
			}
		}
		break;
	case S_CARRIER_ID_READ:
		if(g_opr.isDryRun)
		{
			m_bCompCarrierIdRead = TRUE;
			break;
		}

//		if(!g_pNV->UseSkip(usRfid))
//		{
			m_bCompCarrierIdRead = TRUE;
			break;
//		}

		m_fsm.Set(C_CARRIER_ID_READ_START);
		break;
	case S_MERGE_INFO:
		if(g_opr.isDryRun)
		{
			m_bCompMergeInfo = TRUE;
			break;
		}

//		if(!g_pNV->UseSkip(usRfid)) // Oht Mode 사용시 사용 안함
//		{
			m_bCompMergeInfo = TRUE;
			break;
//		}

		m_fsm.Set(C_MERGE_INFO_START);
		break;
	case S_TRAY_INFO:
		if(g_opr.isDryRun)
		{
			m_bCompTrayInfo = TRUE;
			break;
		}

//		if(!g_pNV->UseSkip(usRfid))
//		{
			m_bCompTrayInfo = TRUE;
			break;
//		}

		m_fsm.Set(C_TRAY_INFO_START);
		break;

	case S_WORK: 

		if(g_rail.Exist() && !g_rail.m_bCompRcv)
			break;

		if(m_pMtY->InPos(PY_RAIL,slotPosY,5))
		{
			if(CmdSlotNo() != CurSlotNo())
			{
				int ZSlotNum = (CmdSlotNo()-1)%(int)g_pNV->Pkg(mzSlotZCnt);
				int YSlotNum = (CmdSlotNo()-1)/(int)g_pNV->Pkg(mzSlotZCnt);

				double slotPosZ = GetMzZSlotPos(ZSlotNum);
				m_pMtZ->PMove(PZ_RAIL, slotPosZ);

				double slotPosY = GetMzYSlotPos(YSlotNum);
				m_pMtY->PMove(PY_RAIL, slotPosY);
				CurSlotNo() = CmdSlotNo();

				// 테스트
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RAIL_TEST", g_data2c.cEtc.start, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
					g_data2c.cLdmz.Y[PY_RAIL][_POSIDX_], g_data2c.cLdmz.Y[PY_RAIL][_POS_], 
					g_data2c.cLdmz.Y[PY_RAIL][_SPDIDX_], g_data2c.cLdmz.Y[PY_RAIL][_SPD_], 
					g_data2c.cLdmz.Y[PY_RAIL][_ACCIDX_], g_data2c.cLdmz.Y[PY_RAIL][_ACC_]))

					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_RAIL_TEST", g_data2c.cEtc.start, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
					g_data2c.cLdmz.Z[PZ_RAIL][_POSIDX_], g_data2c.cLdmz.Z[PZ_RAIL][_POS_], 
					g_data2c.cLdmz.Z[PZ_RAIL][_SPDIDX_], g_data2c.cLdmz.Z[PZ_RAIL][_SPD_], 
					g_data2c.cLdmz.Z[PZ_RAIL][_ACCIDX_], g_data2c.cLdmz.Z[PZ_RAIL][_ACC_]))


			}
			else
			{
				if(g_pNV->NDm(mmiBtnElevLock))
					break;

				// 2022.07 MGZ 연속 랏 중복 방지
				BOOL EmptyChk = false;
				EmptyChk |= g_pNV->NDm(existRail);
				EmptyChk |= g_pNV->NDm(existInPnp);
				EmptyChk |= g_pNV->NDm(existIndex01);
				EmptyChk |= g_pNV->NDm(existIndex02);
				EmptyChk |= g_pNV->NDm(existIndex03);
				EmptyChk |= g_pNV->NDm(existIndex04);
				EmptyChk |= g_pNV->NDm(existIndexScrap01);
				EmptyChk |= g_pNV->NDm(existIndexScrap02);
				EmptyChk |= g_pNV->NDm(existIndexScrap03);
				EmptyChk |= g_pNV->NDm(existIndexScrap04);
				EmptyChk |= g_pNV->NDm(existOutPnpScrap);
				EmptyChk |= g_pNV->NDm(existOutPnpPcb);

				// 새로운 MGZ 이고 & 작업중인 자재가 있으면 자재를 푸쉬 하지 않는다
				if(m_bNewMz && EmptyChk)
					break;

				if(g_rail.IsReadyLdMz())
				{
					m_fsm.Set(C_PUSHER_START); // 위에서 start log, 이부분에서 end log.

					// 테스트
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RAIL_TEST", g_data2c.cEtc.end, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
						g_data2c.cLdmz.Y[PY_RAIL][_POSIDX_], g_data2c.cLdmz.Y[PY_RAIL][_POS_], 
						g_data2c.cLdmz.Y[PY_RAIL][_SPDIDX_], g_data2c.cLdmz.Y[PY_RAIL][_SPD_], 
						g_data2c.cLdmz.Y[PY_RAIL][_ACCIDX_], g_data2c.cLdmz.Y[PY_RAIL][_ACC_]))

						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_RAIL_TEST", g_data2c.cEtc.end, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
						g_data2c.cLdmz.Z[PZ_RAIL][_POSIDX_], g_data2c.cLdmz.Z[PZ_RAIL][_POS_], 
						g_data2c.cLdmz.Z[PZ_RAIL][_SPDIDX_], g_data2c.cLdmz.Z[PZ_RAIL][_SPD_], 
						g_data2c.cLdmz.Z[PZ_RAIL][_ACCIDX_], g_data2c.cLdmz.Z[PZ_RAIL][_ACC_]))
				}					
			}
		}
		else{
			if(Total_SlotNum < CmdSlotNo())
			{
				int QtyTime = (int)g_pNV->DDm(lotQtyCheckTime) * 1000;
				int LotQtyCheck = (int)g_pNV->DDm(lotCurQty);
				
				if(m_fsm.TimeLimit(QtyTime))
				{
					m_fsm.Set(C_ERROR, ER_LD_MZ_LOT_CUR_QTY_DIFFERENT);
					break;
				}

				if(LotQtyCheck == g_pNV->m_pLotInfo->history[0].lotMergeCurCnt || !g_pNV->UseSkip(usTcServer) || !g_pNV->UseSkip(usArts))
				{
					g_pNV->DDm(lotCurQty) = 0;
					m_bCompWork = TRUE;
				}
			}
			else
			{
				m_pMtY->PMove(PY_RAIL,slotPosY);
			}
		}
		break;

	case S_RFID_WRITE:
		if(g_opr.isDryRun)
		{
			m_bCompRfidWrite = TRUE;
			break;
		}

		if(!g_pNV->UseSkip(usRfid))
		{
			m_bCompRfidWrite = TRUE;
			break;
		}
		
		if(m_pMtZ->InPos(PZ_RFID))
		{
			if(m_pMtY->InPos(PY_RFID))
				m_fsm.Set(C_RFID_WRITE_START);
			else
				m_pMtY->Move(PY_RFID);
		}
		else
			m_pMtZ->Move(PZ_RFID);
		break;
	case S_RFID_WRITE_CHECK: // Write 정보 재확인
		if(g_opr.isDryRun)
		{
			m_bCompRfidWriteCheck = TRUE;
			break;
		}
		
		if(!g_pNV->UseSkip(usRfid))
		{
			m_bCompRfidWriteCheck = TRUE;
			break;
		}
		
		if(m_pMtZ->InPos(PZ_RFID))
		{
			if(m_pMtY->InPos(PY_RFID))
				m_fsm.Set(C_RFID_WRITE_CHECK_START);
			else
				m_pMtY->Move(PY_RFID);
		}
		else
			m_pMtZ->Move(PZ_RFID);
		break;
	case S_EJECT:
		if(m_pMtZ->InPos(PZ_EJECT))
		{
			if(g_ldMzOutConv.IsReadyMzOut())
				m_fsm.Set(C_EJECT_START);
		}
		else
			m_pMtZ->Move(PZ_EJECT);

		break;
	}

}


//-------------------------------------------------------------------
void CLdMz::CycleRun(void)
{
	if(!m_fsm.IsRun())
		return;

	if(m_fsm.IsStop())
		return;

	if(C_ERROR == m_fsm.Get())
	{
		if(0 < m_fsm.GetMsg())
			g_err.Save(m_fsm.GetMsg());

		m_pMtX->Move(PX_BWD);
		g_rail.m_pCylGripFB->Actuate(pmBWD);
		g_pNV->NDm(stateRfidRead)		= STATE_IDLE;
		g_pNV->NDm(stateRfidWrite)		= STATE_IDLE;
		g_pNV->NDm(stateRfidWriteCheck)	= STATE_IDLE;
		g_pNV->NDm(stateLotMergeInfo)	= STATE_IDLE;
		g_pNV->NDm(stateCarrierIdRead)	= STATE_IDLE;
		g_pNV->NDm(stateTrayInfoReq)	= STATE_IDLE;

		g_ldMz.m_bManualOut = FALSE;
		m_fsm.Set(C_IDLE);
		return;
	}
	else if(IsErr())
	{
		m_fsm.Set(C_ERROR);
		return;
	}

	CycleLoading();
	CycleMzAlign();
	CycleReadRfid();
	CycleCarrierIdRead();
	CyclePartNoCompare();
	CycleMergeInfo();
	CycleTrayInfo();
	CyclePusher();
	CycleRfidWrite();
	CycleRfidWriteCheck();
	CycleEject();
}


//-------------------------------------------------------------------
void CLdMz::Init(void)
{
	m_pMtX = &g_mt[MT_PUSHER_X];
	m_pMtY = &g_mt[MT_LD_Y];
	m_pMtZ = &g_mt[MT_LD_Z];

	m_pCylAlignFB	= &g_pm[CYL_MGZ_CLAMP_ALIGN_FB];
	m_pCylClampOC	= &g_pm[CYL_MGZ_CLAMP_OC];
}


//-------------------------------------------------------------------
int& CLdMz::Exist(void)
{
	return (g_pNV->m_pData->ndm[existLdMz]);
}


//-------------------------------------------------------------------
int& CLdMz::CmdSlotNo(void)
{
	return (g_pNV->m_pData->ndm[ldMzCmdSlotNo]);
}


//-------------------------------------------------------------------
int& CLdMz::CurSlotNo(void)
{
	return (g_pNV->m_pData->ndm[ldMzCurSlotNo]);
}


//-------------------------------------------------------------------
void CLdMz::CycleLoading(void)
{
	if(!m_fsm.Between(C_LOADING_START, C_LOADING_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_LD_MZ_LOADING_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialId, L"$");
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_LOADING_START:
		if(m_fsm.Once())
		{
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LD_LOADING_MGZ", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"MGZ_IN_CONV", g_data2c.cLdmz.deviceId))				

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemMzLoadingStart) = STATE_REQ;

			g_ldMzInConv.m_pCylStopper01UD->Actuate(pmDOWN);

			//if(!g_logChk.bFunction[g_ldMzInConv.m_pCylStopper01UD->GetNo()])
			//{
			//	g_logChk.bFunction[g_ldMzInConv.m_pCylStopper01UD->GetNo()] = TRUE;
			//	NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_MGZ_IN_STOPPER_DOWN", g_data2c.cEtc.start, 
			//										cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
			//										g_data2c.cPmName[g_ldMzInConv.m_pCylStopper01UD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
			//										g_data2c.cPmIO[g_ldMzInConv.m_pCylStopper01UD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.on, 
			//										g_data2c.cPmIO[g_ldMzInConv.m_pCylStopper01UD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.off))
			//}


			g_ldMzInConv.BeltRun(TRUE);
		}
		else
		{
			//if(g_pNV->Pkg(PcbLengthOptionUse) == 1)
			//{
				if(pmBWD != g_rail.m_pCylGripFB->GetPos(300))
				{
					g_rail.m_pCylGripFB->Actuate(pmBWD); 
					break;
				}
			//}

 			if(g_dIn.BOn(iMzClampPcbJam) || g_dIn.AOn(iRailExistStart))
			{
				if(g_dIn.BOn(iMzClampPcbJam))
					m_fsm.Set(C_ERROR, ER_LD_MZ_CLAMP_PCB_JAM_NOT_OFF);
				if(g_dIn.AOn(iRailExistStart))
					m_fsm.Set(C_ERROR, ER_RAIL_EXIST_START_NOT_OFF);
				break;
			}
			else if(!m_pMtX->InPos(PX_BWD, 100))
				m_pMtX->Move(PX_BWD);
			else if(pmDOWN != g_ldMzInConv.m_pCylStopper01UD->GetPos(300))
				g_ldMzInConv.m_pCylStopper01UD->Actuate(pmDOWN);
			else
			{
				//if(g_logChk.bFunction[g_ldMzInConv.m_pCylStopper01UD->GetNo()])
				//{
				//	g_logChk.bFunction[g_ldMzInConv.m_pCylStopper01UD->GetNo()] = FALSE;
				//	NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_MGZ_IN_STOPPER_DOWN", g_data2c.cEtc.end, 
				//										cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
				//										g_data2c.cPmName[g_ldMzInConv.m_pCylStopper01UD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
				//										g_data2c.cPmIO[g_ldMzInConv.m_pCylStopper01UD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
				//										g_data2c.cPmIO[g_ldMzInConv.m_pCylStopper01UD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
				//}

				m_fsm.Set(C_LOADING_01);
			}
		}
		break;
	case C_LOADING_01:
		if(m_pMtY->InPos(PY_RAIL, 100))
		{
			if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RAIL_LOADING", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
													g_data2c.cLdmz.Y[PY_RAIL][_POSIDX_], g_data2c.cLdmz.Y[PY_RAIL][_POS_], 
													g_data2c.cLdmz.Y[PY_RAIL][_SPDIDX_], g_data2c.cLdmz.Y[PY_RAIL][_SPD_], 
													g_data2c.cLdmz.Y[PY_RAIL][_ACCIDX_], g_data2c.cLdmz.Y[PY_RAIL][_ACC_]))
			}

			if(Exist())
			{
				if(pmCLOSE != m_pCylClampOC->GetPos(1500))
					m_pCylClampOC->Actuate(pmCLOSE);
				else
					m_fsm.Set(C_LOADING_END);
			}
			else
			{
				if(m_pMtZ->InPos(PZ_RCV, 100))
				{
					if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RCV])
					{
						g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RCV] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_RCV_LOADING", g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
															g_data2c.cLdmz.Z[PZ_RCV][_POSIDX_], g_data2c.cLdmz.Z[PZ_RCV][_POS_], 
															g_data2c.cLdmz.Z[PZ_RCV][_SPDIDX_], g_data2c.cLdmz.Z[PZ_RCV][_SPD_], 
															g_data2c.cLdmz.Z[PZ_RCV][_ACCIDX_], g_data2c.cLdmz.Z[PZ_RCV][_ACC_]))
					}

					if(pmOPEN != m_pCylClampOC->GetPos(500))
					{
						m_pCylClampOC->Actuate(pmOPEN); // OPEN LOADING 체크

						if(!g_logChk.bFunction[m_pCylClampOC->GetNo()])
						{
							g_logChk.bFunction[m_pCylClampOC->GetNo()] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_OPEN_LOADING", g_data2c.cEtc.start, 
																cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
																g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
																g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.off, 
																g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.on))
						}
					}
					else
					{
						if(g_logChk.bFunction[m_pCylClampOC->GetNo()])
						{
							g_logChk.bFunction[m_pCylClampOC->GetNo()] = FALSE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_OPEN_LOADING", g_data2c.cEtc.end, 
																cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
																g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
																g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.on, 
																g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.off))
						}
						m_fsm.Set(C_LOADING_MOVE_FWD);
					}
				}
				else
				{
					m_pMtZ->Move(PZ_RCV);

					if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RCV])
					{
						g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RCV] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_RCV_LOADING", g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
															g_data2c.cLdmz.Z[PZ_RCV][_POSIDX_], g_data2c.cLdmz.Z[PZ_RCV][_POS_], 
															g_data2c.cLdmz.Z[PZ_RCV][_SPDIDX_], g_data2c.cLdmz.Z[PZ_RCV][_SPD_], 
															g_data2c.cLdmz.Z[PZ_RCV][_ACCIDX_], g_data2c.cLdmz.Z[PZ_RCV][_ACC_]))
					}
				}
			}
		}
		else if(m_pMtY->InPos(PY_RCV, 100))
			m_fsm.Set(C_LOADING_MOVE_FWD);
		else
		{
			if(CanMtYMove())
			{
				m_pMtY->Move(PY_RAIL);

				if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RAIL_LOADING", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
														g_data2c.cLdmz.Y[PY_RAIL][_POSIDX_], g_data2c.cLdmz.Y[PY_RAIL][_POS_], 
														g_data2c.cLdmz.Y[PY_RAIL][_SPDIDX_], g_data2c.cLdmz.Y[PY_RAIL][_SPD_], 
														g_data2c.cLdmz.Y[PY_RAIL][_ACCIDX_], g_data2c.cLdmz.Y[PY_RAIL][_ACC_]))
				}
			}
			else
				m_fsm.Set(C_ERROR, ER_LD_MZ_MTY_CANNOT_MOVE);
		}
		break;
	case C_LOADING_MOVE_FWD:
		if(m_pMtY->InPos(PY_RCV, 100))
		{
			if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RCV])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RCV] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RCV_LOADING", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
													g_data2c.cLdmz.Y[PY_RCV][_POSIDX_], g_data2c.cLdmz.Y[PY_RCV][_POS_], 
													g_data2c.cLdmz.Y[PY_RCV][_SPDIDX_], g_data2c.cLdmz.Y[PY_RCV][_SPD_], 
													g_data2c.cLdmz.Y[PY_RCV][_ACCIDX_], g_data2c.cLdmz.Y[PY_RCV][_ACC_]))
			}

			if(m_pMtZ->InPos(PZ_RCV, 100))
			{
				m_pMtZ->Move(PZ_CLAMP_DN);

				if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_CLAMP_DN])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_CLAMP_DN] = TRUE; 
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_CLAMP_DOWN_LOADING", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cLdmz.Z[PZ_CLAMP_DN][_POSIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_DN][_POS_], 
														g_data2c.cLdmz.Z[PZ_CLAMP_DN][_SPDIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_DN][_SPD_], 
														g_data2c.cLdmz.Z[PZ_CLAMP_DN][_ACCIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_DN][_ACC_]))
				}
			}
			else if(m_pMtZ->InPos(PZ_CLAMP_DN))
			{
				if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_CLAMP_DN])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_CLAMP_DN] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_CLAMP_DOWN_LOADING", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cLdmz.Z[PZ_CLAMP_DN][_POSIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_DN][_POS_], 
														g_data2c.cLdmz.Z[PZ_CLAMP_DN][_SPDIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_DN][_SPD_], 
														g_data2c.cLdmz.Z[PZ_CLAMP_DN][_ACCIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_DN][_ACC_]))
				}

				if(pmCLOSE != m_pCylClampOC->GetPos(1500))
				{
					m_pCylClampOC->Actuate(pmCLOSE);

					if(!g_logChk.bFunction[m_pCylClampOC->GetNo()])
					{
						g_logChk.bFunction[m_pCylClampOC->GetNo()] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_CLOSE_LOADING", g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
															g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.on, 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.off))
					}
					break;
				}
				else
				{
					if(g_logChk.bFunction[m_pCylClampOC->GetNo()])
					{
						g_logChk.bFunction[m_pCylClampOC->GetNo()] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_CLOSE_LOADING", g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
															g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.off, 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.on))
															//g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.on))
					}
				}

				if((g_dIn.AOn(iMzClampLExist) && g_dIn.AOn(iMzClampRExist)) || g_opr.isDryRun)
				{
					m_fsm.Set(C_LOADING_MOVE_BWD);
					g_ldMzInConv.BeltRun(FALSE);
				}
				else
				{
					if(pmOPEN != m_pCylClampOC->GetPos(500))
					{
						m_pCylClampOC->Actuate(pmOPEN);
						m_pMtZ->Move(PZ_RCV);

						m_fsm.Set(C_ERROR, ER_LD_MZ_EXIST_SEN_NOT_ON);
					}
				}
			}
			else
			{
			}
		}
		else
		{
			m_pMtY->Move(PY_RCV);
			if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RCV])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RCV] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RCV_LOADING", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
													g_data2c.cLdmz.Y[PY_RCV][_POSIDX_], g_data2c.cLdmz.Y[PY_RCV][_POS_], 
													g_data2c.cLdmz.Y[PY_RCV][_SPDIDX_], g_data2c.cLdmz.Y[PY_RCV][_SPD_], 
													g_data2c.cLdmz.Y[PY_RCV][_ACCIDX_], g_data2c.cLdmz.Y[PY_RCV][_ACC_]))
			}
		}
		break;
	case C_LOADING_MOVE_BWD:
		// 있던 없던 무조건 잡고 뒤로 이동.
		if(pmCLOSE != m_pCylClampOC->GetPos(1500)) 
			m_pCylClampOC->Actuate(pmCLOSE);
		else if(m_pMtZ->InPos(PZ_RCV_BWD, 100))
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RCV_BWD])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RCV_BWD] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_RCV_BWD", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cLdmz.Z[PZ_RCV_BWD][_POSIDX_], g_data2c.cLdmz.Z[PZ_RCV_BWD][_POS_], 
													g_data2c.cLdmz.Z[PZ_RCV_BWD][_SPDIDX_], g_data2c.cLdmz.Z[PZ_RCV_BWD][_SPD_], 
													g_data2c.cLdmz.Z[PZ_RCV_BWD][_ACCIDX_], g_data2c.cLdmz.Z[PZ_RCV_BWD][_ACC_]))
			}

			if(!m_pMtY->InPos(PY_RAIL, 100))
			{
				m_pMtY->Move(PY_RAIL);

				if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RAIL_LOADING_END", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
														g_data2c.cLdmz.Y[PY_RAIL][_POSIDX_], g_data2c.cLdmz.Y[PY_RAIL][_POS_], 
														g_data2c.cLdmz.Y[PY_RAIL][_SPDIDX_], g_data2c.cLdmz.Y[PY_RAIL][_SPD_], 
														g_data2c.cLdmz.Y[PY_RAIL][_ACCIDX_], g_data2c.cLdmz.Y[PY_RAIL][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RAIL_LOADING_END", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
														g_data2c.cLdmz.Y[PY_RAIL][_POSIDX_], g_data2c.cLdmz.Y[PY_RAIL][_POS_], 
														g_data2c.cLdmz.Y[PY_RAIL][_SPDIDX_], g_data2c.cLdmz.Y[PY_RAIL][_SPD_], 
														g_data2c.cLdmz.Y[PY_RAIL][_ACCIDX_], g_data2c.cLdmz.Y[PY_RAIL][_ACC_]))
				}
				m_fsm.Set(C_LOADING_END);
			}
		}
		else
		{
			m_pMtZ->Move(PZ_RCV_BWD);

			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RCV_BWD])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RCV_BWD] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_RCV_BWD", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cLdmz.Z[PZ_RCV_BWD][_POSIDX_], g_data2c.cLdmz.Z[PZ_RCV_BWD][_POS_], 
													g_data2c.cLdmz.Z[PZ_RCV_BWD][_SPDIDX_], g_data2c.cLdmz.Z[PZ_RCV_BWD][_SPD_], 
													g_data2c.cLdmz.Z[PZ_RCV_BWD][_ACCIDX_], g_data2c.cLdmz.Z[PZ_RCV_BWD][_ACC_]))
			}
		}
		break;
	case C_LOADING_END:
		Exist() = TRUE;
		m_bReworkMz = g_pNV->NDm(mmiBtnRework);
		g_pNV->NDm(mmiBtnRework) = FALSE;
		g_ldMzInConv.ExistStopper01() = FALSE;

		CurSlotNo()				= 0;
		CmdSlotNo()				= 1;
		m_bCompAlign			= FALSE;
		m_bCompRfidRead			= FALSE;
		m_bCompMergeInfo		= FALSE;
		m_bCompCarrierIdRead	= FALSE;
		m_bCompWork				= FALSE;
		m_bCompRfidWrite		= FALSE;
		m_bCompRfidWriteCheck	= FALSE;
		m_bNewMz				= TRUE;
		m_bNewRailInfo		  = TRUE;
		m_nRfidRetryCnt			= 0;

		if(g_pNV->UseSkip(usSecsGem))	
			g_pNV->NDm(gemMzLoadingEnd) = STATE_REQ;

		g_ldMzInConv.BeltRun(FALSE);

		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LD_LOADING_MGZ", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"MGZ_IN_CONV", g_data2c.cLdmz.deviceId))				
		m_fsm.Set(C_IDLE);
		break;
	}

}


//-------------------------------------------------------------------
void CLdMz::CycleMzAlign(void)
{
	if(!m_fsm.Between(C_ALIGN_START, C_ALIGN_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_LD_MZ_ALIGN_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialId, L"$");
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_ALIGN_START:
		if(m_fsm.Once())
		{
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LD_MGZ_ALIGN", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"LD_MGZ_ALIGN_START", L"LD_MGZ_ALIGN_END"))				
		}
		else
		{
			if(pmBWD != g_rail.m_pCylGripFB->GetPos(300))
			{
				g_rail.m_pCylGripFB->Actuate(pmBWD); 
				break;
			}

			if(!m_pMtZ->InPos(PZ_ALIGN))
			{
				m_pMtZ->Move(PZ_ALIGN);

				if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_ALIGN])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_ALIGN] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_ALIGN", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cLdmz.Z[PZ_ALIGN][_POSIDX_], g_data2c.cLdmz.Z[PZ_ALIGN][_POS_], 
														g_data2c.cLdmz.Z[PZ_ALIGN][_SPDIDX_], g_data2c.cLdmz.Z[PZ_ALIGN][_SPD_], 
														g_data2c.cLdmz.Z[PZ_ALIGN][_ACCIDX_], g_data2c.cLdmz.Z[PZ_ALIGN][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_ALIGN])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_ALIGN] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_ALIGN", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cLdmz.Z[PZ_ALIGN][_POSIDX_], g_data2c.cLdmz.Z[PZ_ALIGN][_POS_], 
														g_data2c.cLdmz.Z[PZ_ALIGN][_SPDIDX_], g_data2c.cLdmz.Z[PZ_ALIGN][_SPD_], 
														g_data2c.cLdmz.Z[PZ_ALIGN][_ACCIDX_], g_data2c.cLdmz.Z[PZ_ALIGN][_ACC_]))
				}
			}

			if(!m_pMtY->InPos(PY_ALIGN))
			{
				m_pMtY->Move(PY_ALIGN);

				if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ALIGN])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ALIGN] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_ALIGN", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
														g_data2c.cLdmz.Y[PY_ALIGN][_POSIDX_], g_data2c.cLdmz.Y[PY_ALIGN][_POS_], 
														g_data2c.cLdmz.Y[PY_ALIGN][_SPDIDX_], g_data2c.cLdmz.Y[PY_ALIGN][_SPD_], 
														g_data2c.cLdmz.Y[PY_ALIGN][_ACCIDX_], g_data2c.cLdmz.Y[PY_ALIGN][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ALIGN])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ALIGN] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_ALIGN", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
														g_data2c.cLdmz.Y[PY_ALIGN][_POSIDX_], g_data2c.cLdmz.Y[PY_ALIGN][_POS_], 
														g_data2c.cLdmz.Y[PY_ALIGN][_SPDIDX_], g_data2c.cLdmz.Y[PY_ALIGN][_SPD_], 
														g_data2c.cLdmz.Y[PY_ALIGN][_ACCIDX_], g_data2c.cLdmz.Y[PY_ALIGN][_ACC_]))
				}
			}

			if(pmFWD != m_pCylAlignFB->GetPos(500))
			{
				m_pCylAlignFB->Actuate(pmFWD);

				if(!g_logChk.bFunction[m_pCylAlignFB->GetNo()])
				{
					g_logChk.bFunction[m_pCylAlignFB->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_ALIGN_FWD", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylAlignFB->GetNo()], g_data2c.cEtc.delayTime, L"500", 
														g_data2c.cPmIO[m_pCylAlignFB->GetNo()].In[pmFWD][pmON], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylAlignFB->GetNo()].In[pmFWD][pmOFF], g_data2c.cEtc.on))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylAlignFB->GetNo()])
				{
					g_logChk.bFunction[m_pCylAlignFB->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_ALIGN_FWD", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylAlignFB->GetNo()], g_data2c.cEtc.delayTime, L"500", 
														g_data2c.cPmIO[m_pCylAlignFB->GetNo()].In[pmFWD][pmON], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylAlignFB->GetNo()].In[pmFWD][pmOFF], g_data2c.cEtc.off))
				}
			}

			if(pmOPEN != m_pCylClampOC->GetPos(1000))
			{
				m_pCylClampOC->Actuate(pmOPEN);

				if(!g_logChk.bFunction[m_pCylClampOC->GetNo()])
				{
					g_logChk.bFunction[m_pCylClampOC->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_OPEN_ALIGN", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"1000", 
														g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.on))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylClampOC->GetNo()])
				{
					g_logChk.bFunction[m_pCylClampOC->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_OPEN_ALIGN", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"1000", 
														g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.off))
				}
			}
			m_fsm.Set(C_ALIGN_01);
		}
		break;
	case C_ALIGN_01:
		if(pmCLOSE != m_pCylClampOC->GetPos(1000))
		{
			m_pCylClampOC->Actuate(pmCLOSE);

			if(!g_logChk.bFunction[m_pCylClampOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylClampOC->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_CLOSE_ALIGN", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName,
													g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"1000",
													g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.on,
													g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.off))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylClampOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylClampOC->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_CLOSE_ALIGN", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName,
													g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"1000",
													g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.off,
													g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.on))
			}
		}

		if(pmBWD != m_pCylAlignFB->GetPos(500))
		{
			m_pCylAlignFB->Actuate(pmBWD);
			if(!g_logChk.bFunction[m_pCylAlignFB->GetNo()])
			{
				g_logChk.bFunction[m_pCylAlignFB->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_ALIGN_BWD", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylAlignFB->GetNo()], g_data2c.cEtc.delayTime, L"500", 
													g_data2c.cPmIO[m_pCylAlignFB->GetNo()].In[pmBWD][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylAlignFB->GetNo()].In[pmBWD][pmON], g_data2c.cEtc.off))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylAlignFB->GetNo()])
			{
				g_logChk.bFunction[m_pCylAlignFB->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_ALIGN_BWD", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylAlignFB->GetNo()], g_data2c.cEtc.delayTime, L"500", 
													g_data2c.cPmIO[m_pCylAlignFB->GetNo()].In[pmBWD][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylAlignFB->GetNo()].In[pmBWD][pmON], g_data2c.cEtc.on))
			}
		}
		m_fsm.Set(C_ALIGN_END);
		break;
	case C_ALIGN_END:
		m_pMtY->Move(PY_RAIL);
		m_bCompAlign = TRUE;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LD_MGZ_ALIGN", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"LD_MGZ_ALIGN_START", L"LD_MGZ_ALIGN_END"))				
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CLdMz::CycleReadRfid(void)
{
	if(!m_fsm.Between(C_RFID_READ_START, C_RFID_READ_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_LD_MZ_READ_RFID_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialId, L"$");
	_sprintf(cMaterialType, L"MZ");	

	_char cLotId[_MAX_CHAR_SIZE_], cPartId[_MAX_CHAR_SIZE_];
	_sprintf(cLotId, L"$");
	_sprintf(cPartId, L"$");	

	switch(m_fsm.Get())
	{
	case C_RFID_READ_START:
		if(m_fsm.Once())
		{
			m_nRfidRetryCnt = 0;
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LD_MGZ_RFID_READ", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"LD_MGZ_RFID_READ_START", L"LD_MGZ_RFID_READ_END"))
		}
		else
		{
			if(pmBWD != g_rail.m_pCylGripFB->GetPos(300))
			{
				g_rail.m_pCylGripFB->Actuate(pmBWD); 
				break;
			}

			if(m_pMtZ->InPos(PZ_RFID))
			{
				if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RFID])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RFID] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_RFID", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cLdmz.Z[PZ_RFID][_POSIDX_], g_data2c.cLdmz.Z[PZ_RFID][_POS_], 
														g_data2c.cLdmz.Z[PZ_RFID][_SPDIDX_], g_data2c.cLdmz.Z[PZ_RFID][_SPD_], 
														g_data2c.cLdmz.Z[PZ_RFID][_ACCIDX_], g_data2c.cLdmz.Z[PZ_RFID][_ACC_]))
				}

				if(m_pMtY->InPos(PY_RFID))
				{
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RFID])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RFID] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RFID", g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
															g_data2c.cLdmz.Y[PY_RFID][_POSIDX_], g_data2c.cLdmz.Y[PY_RFID][_POS_], 
															g_data2c.cLdmz.Y[PY_RFID][_SPDIDX_], g_data2c.cLdmz.Y[PY_RFID][_SPD_], 
															g_data2c.cLdmz.Y[PY_RFID][_ACCIDX_], g_data2c.cLdmz.Y[PY_RFID][_ACC_]))
					}
					m_fsm.Set(C_RFID_READ_01);
				}
				else
				{
					m_pMtY->Move(PY_RFID);

					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RFID])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RFID] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RFID", g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
															g_data2c.cLdmz.Y[PY_RFID][_POSIDX_], g_data2c.cLdmz.Y[PY_RFID][_POS_], 
															g_data2c.cLdmz.Y[PY_RFID][_SPDIDX_], g_data2c.cLdmz.Y[PY_RFID][_SPD_], 
															g_data2c.cLdmz.Y[PY_RFID][_ACCIDX_], g_data2c.cLdmz.Y[PY_RFID][_ACC_]))
					}
				}
			}
			else
			{
				m_pMtZ->Move(PZ_RFID);

				if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RFID])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_RFID] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_RFID", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cLdmz.Z[PZ_RFID][_POSIDX_], g_data2c.cLdmz.Z[PZ_RFID][_POS_], 
														g_data2c.cLdmz.Z[PZ_RFID][_SPDIDX_], g_data2c.cLdmz.Z[PZ_RFID][_SPD_], 
														g_data2c.cLdmz.Z[PZ_RFID][_ACCIDX_], g_data2c.cLdmz.Z[PZ_RFID][_ACC_]))
				}
			}
		}
		break;
	case C_RFID_READ_01:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateRfidRead) = STATE_REQ;
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"LD_RFID_READ", g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'RS232'", L"'LOT_ID'", cLotId, L"'PART_ID'", cPartId))
		}
		else
		{
			if(m_fsm.TimeLimit(7000))
			{
				m_fsm.Set(C_RFID_READ_02);
				break;
			}

			switch(g_pNV->NDm(stateRfidRead))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				if (g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID[0] == L'\0') 
					_sprintf(cMaterialId, L"$");
				else
					mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID) + 1);
				mbstowcs(cLotId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].lotID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].lotID) + 1);
				mbstowcs(cPartId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].partID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].partID) + 1);
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"LD_RFID_READ", g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'RS232'", L"'LOT_ID'", cLotId, L"'PART_ID'", cPartId))
				m_fsm.Set(C_RFID_READ_END);
				break;
			case STATE_ERR:
				m_fsm.Set(C_RFID_READ_02);
				break;
			}
		}
		break;
	case C_RFID_READ_02:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateRfidRead) = STATE_IDLE;

			m_nRfidRetryCnt++;

			if(RFID_FAIL_MAX < m_nRfidRetryCnt)
			{
				m_fsm.Set(C_ERROR, ER_LD_MZ_READ_RFID_FAIL);
			}
		}
		else
		{
			if(!m_fsm.Delay(1000))
				break;
			m_fsm.Set(C_RFID_READ_01);
		}
		break;
	case C_RFID_READ_END:
		m_pMtY->Move(PY_RAIL);
		m_bCompRfidRead = TRUE;
		g_pNV->NDm(stateRfidRead) = STATE_IDLE;

		if (g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID[0] == L'\0') 
			_sprintf(cMaterialId, L"$");
		else
			mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID) + 1);
		mbstowcs(cLotId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].lotID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].lotID) + 1);
		mbstowcs(cPartId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].partID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].partID) + 1);
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LD_MGZ_RFID_READ", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"LD_MGZ_RFID_READ_START", L"LD_MGZ_RFID_READ_END"))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CLdMz::CyclePartNoCompare(void)
{
	if(!m_fsm.Between(C_PART_NO_COMPARE_START, C_PART_NO_COMPARE_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_PART_NO_COMPARE_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if (g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID) + 1);
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_PART_NO_COMPARE_START:
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LOT_PART_NO_COMP", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"LOT_PART_NO_COMP_START", L"LOT_PART_NO_COMP_END"))
		m_fsm.Set(C_PART_NO_COMPARE_01);
		break;
	case C_PART_NO_COMPARE_01:
		if(m_fsm.Once())
		{
			g_pNV->NDm(needAutoRecipeChg) = FALSE;
			g_pNV->NDm(statePartNoCompare) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(7000))
			{
				m_fsm.Set(C_ERROR, ER_PART_NO_COMPARE_TM_OVER);
				break;
			}

			switch(g_pNV->NDm(statePartNoCompare))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				// 전 Lot, 현 Lot Part No 동일하거나 작업 가능한 Lot
				m_fsm.Set(C_PART_NO_COMPARE_END);
				break;
			case STATE_ERR:
				// 전 Lot, 현 Lot Part No 가 다르고 설비에 Setup 되어 있지 않음
				m_fsm.Set(C_ERROR, ER_PART_NO_DIFFERENT);
				break;
			case 5: // 현 Step만 예외적으로 추가
				// 동일 Part No가 현재 Setup되어 있는 그룹에 존재함
				// ndm autoRecipeChgJobNo = 10, autoRecipeChgGroupNo = 11 에 mmi에서 저장 한 후 
				// CycleAutoRecipeChg 에서 해당 Recipe로 mmi가 Auto Change 함
				if(!g_pNV->UseSkip(usAutoRecipeChg))
				{
					m_fsm.Set(C_ERROR, ER_PART_NO_DIFFERENT);
					break;
				}
				
				g_pNV->NDm(needAutoRecipeChg) = TRUE;
				m_fsm.Set(C_PART_NO_COMPARE_END);
				break;
			}
		}
		break;
	case C_PART_NO_COMPARE_END:
		m_bCompPartNoCompare = TRUE;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LOT_PART_NO_COMP", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"LOT_PART_NO_COMP_START", L"LOT_PART_NO_COMP_END"))
		g_pNV->NDm(statePartNoCompare) = STATE_IDLE;
		m_fsm.Set(C_IDLE);
		break;
	}
}

//-------------------------------------------------------------------
void CLdMz::CycleAutoRecipeChg(void)
{
	if(!m_fsm.Between(C_AUTO_RECIPE_CHG_START, C_AUTO_RECIPE_CHG_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_AUTO_RECIPE_CHG_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	switch(m_fsm.Get())
	{
	case C_AUTO_RECIPE_CHG_START:
		m_fsm.Set(C_AUTO_RECIPE_CHG_01);
		break;
	case C_AUTO_RECIPE_CHG_01:
		// MMI에 Sorter Data 전송 및 Router Device Chg 성공 확인
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateAutoRecipeChg) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(7000))
			{
				m_fsm.Set(C_ERROR, ER_AUTO_RECIPE_CHG_TM_OVER);
				break;
			}

			switch(g_pNV->NDm(stateAutoRecipeChg))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				m_fsm.Set(C_AUTO_RECIPE_CHG_02);
				break;
			case STATE_ERR:
				m_fsm.Set(C_ERROR, ER_AUTO_RECIPE_CHG_FAIL);
				break;
			}
		}
		break;
	case C_AUTO_RECIPE_CHG_02:
		if(!m_fsm.Delay(1000))
			break;

		// 아래값 수정하여 Seq()에서 All Home 깨지지 않도록 수정
		g_err.nOldJobNo   = g_pNV->NDm(jobNo);
		g_err.nOldGroupNo = g_pNV->NDm(groupNo);

		m_fsm.Set(C_AUTO_RECIPE_CHG_03);
		break;
	case C_AUTO_RECIPE_CHG_03:
		if(m_fsm.Once())
		{
			g_dOut.On(oSorterAutoRecipeChg);
		}
		else
		{
			if(m_fsm.TimeLimit(20000))
			{
				m_fsm.Set(C_ERROR, ER_SORTER_AUTO_RECIPE_CHG_TM_OVER);
				return;
			}

			if(g_dIn.AOn(iSorterAutoRecipeChgComp))
			{
				m_fsm.Set(C_AUTO_RECIPE_CHG_END);
			}
		}
		break;
	case C_AUTO_RECIPE_CHG_END:
		g_dOut.Off(oSorterAutoRecipeChg);
		m_bCompAutoRecipeChg = TRUE;
		g_pNV->NDm(stateAutoRecipeChg) = STATE_IDLE;
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CLdMz::CycleCarrierIdRead(void)
{
	if(!m_fsm.Between(C_CARRIER_ID_READ_START, C_CARRIER_ID_READ_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_LD_MZ_CARRIER_ID_READ_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if (g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID) + 1);
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_CARRIER_ID_READ_START:
		g_pNV->NDm(stateCarrierIdRead) = STATE_IDLE;
		m_nTcRetryCnt = 0;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LOT_CARRIER_ID_READ", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"LOT_CARRIER_ID_READ_START", L"LOT_CARRIER_ID_READ_END"))
		m_fsm.Set(C_CARRIER_ID_READ_01);
		break;
	case C_CARRIER_ID_READ_01:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateCarrierIdRead) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_fsm.Set(C_CARRIER_ID_READ_02);
				break;
			}

			switch(g_pNV->NDm(stateCarrierIdRead))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				m_fsm.Set(C_CARRIER_ID_READ_END);
				break;
			case STATE_ERR:
				// OHT 관련 명령어 이므로 OHT 사용 안할시에는 Error 발생하지 않는다.
// 				if(g_pNV->UseSkip(usOhtMode))
// 					m_fsm.Set(C_CARRIER_ID_READ_02);
// 				else
					m_fsm.Set(C_CARRIER_ID_READ_END);
				break;
			}
		}
		break;
	case C_CARRIER_ID_READ_02:
		if(m_fsm.Once())
		{
			m_nTcRetryCnt++;
		}
		else
		{
			if(3 <= m_nTcRetryCnt)
			{
				if(STATE_ERR == g_pNV->NDm(stateCarrierIdRead))
					m_fsm.Set(C_ERROR, ER_LD_MZ_CARRIER_ID_READ_FAIL);
				else 
					m_fsm.Set(C_ERROR, ER_LD_MZ_CARRIER_ID_READ_TM_OVER);
			}
			else
			{
				if(m_fsm.Delay(10000))
				{
					m_fsm.Set(C_CARRIER_ID_READ_01);
				}
			}
		}
		break;
	case C_CARRIER_ID_READ_END:
		g_pNV->NDm(stateCarrierIdRead) = STATE_IDLE;
		m_bCompCarrierIdRead = TRUE;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LOT_CARRIER_ID_READ", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"LOT_CARRIER_ID_READ_START", L"LOT_CARRIER_ID_READ_END"))
		m_fsm.Set(C_IDLE);
		break;
	}
}

//-------------------------------------------------------------------
void CLdMz::CycleMergeInfo(void)
{
	if(!m_fsm.Between(C_MERGE_INFO_START, C_MERGE_INFO_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_LD_MZ_MERGE_INFO_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if (g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID) + 1);
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_MERGE_INFO_START:
		g_pNV->NDm(stateLotMergeInfo) = STATE_IDLE;
		m_nTcRetryCnt = 0;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LOT_MERGE_INFO", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"LOT_MERGE_INFO_START", L"LOT_MERGE_INFO_END"))
		m_fsm.Set(C_MERGE_INFO_01);
		break;
	case C_MERGE_INFO_01:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateLotMergeInfo) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_fsm.Set(C_MERGE_INFO_02);
				break;
			}

			switch(g_pNV->NDm(stateLotMergeInfo))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				m_fsm.Set(C_MERGE_INFO_END);
				break;
			case STATE_ERR:
				// OHT 관련 명령어 이므로 OHT 사용 안할시에는 Error 발생하지 않는다.
// 				if(g_pNV->UseSkip(usOhtMode))
// 					m_fsm.Set(C_MERGE_INFO_02);
// 				else
					m_fsm.Set(C_MERGE_INFO_END);
				break;
			}
		}
		break;
	case C_MERGE_INFO_02:
		if(m_fsm.Once())
		{
			m_nTcRetryCnt++;
		}
		else
		{
			if(3 <= m_nTcRetryCnt)
			{
				if(STATE_ERR == g_pNV->NDm(stateLotMergeInfo))
					m_fsm.Set(C_ERROR, ER_LD_MZ_MERGE_INFO_FAIL);
				else 
					m_fsm.Set(C_ERROR, ER_LD_MZ_MERGE_INFO_TM_OVER);
			}
			else
			{
				if(m_fsm.Delay(10000))
				{
					m_fsm.Set(C_MERGE_INFO_01);
				}
			}
		}
		break;
	case C_MERGE_INFO_END:
		g_pNV->NDm(stateLotMergeInfo) = STATE_IDLE;
		m_bCompMergeInfo = TRUE;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LOT_MERGE_INFO", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"LOT_MERGE_INFO_START", L"LOT_MERGE_INFO_END"))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CLdMz::CycleTrayInfo(void)
{
	if(!m_fsm.Between(C_TRAY_INFO_START, C_TRAY_INFO_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_LD_MZ_TRAY_INFO_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if (g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID) + 1);
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_TRAY_INFO_START:
		g_pNV->NDm(stateTrayInfoReq) = STATE_IDLE;
		m_nTcRetryCnt = 0;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LOT_TRAY_INFO", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"LOT_TRAY_INFO_START", L"LOT_TRAY_INFO_END"))
		m_fsm.Set(C_TRAY_INFO_01);
		break;
	case C_TRAY_INFO_01:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateTrayInfoReq) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_fsm.Set(C_TRAY_INFO_02);
				break;
			}

			switch(g_pNV->NDm(stateTrayInfoReq))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				m_fsm.Set(C_TRAY_INFO_END);
				break;
			case STATE_ERR:
				m_fsm.Set(C_TRAY_INFO_02);
				break;
			}
		}
		break;
	case C_TRAY_INFO_02:
		if(m_fsm.Once())
		{
			m_nTcRetryCnt++;
		}
		else
		{
			if(3 <= m_nTcRetryCnt)
			{
				if(STATE_ERR == g_pNV->NDm(stateTrayInfoReq))
					m_fsm.Set(C_ERROR, ER_LD_MZ_TRAY_INFO_FAIL);
				else 
					m_fsm.Set(C_ERROR, ER_LD_MZ_TRAY_INFO_TM_OVER);
			}
			else
			{
				if(m_fsm.Delay(10000))
				{
					m_fsm.Set(C_TRAY_INFO_01);
				}
			}
		}
		break;
	case C_TRAY_INFO_END:
		g_pNV->NDm(stateTrayInfoReq) = STATE_IDLE;
		m_bCompTrayInfo = TRUE;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"LOT_TRAY_INFO", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"LOT_TRAY_INFO_START", L"LOT_TRAY_INFO_END"))
		m_fsm.Set(C_IDLE);
		break;
	}
}

//-------------------------------------------------------------------
void CLdMz::CyclePusher(void)
{
	if(!m_fsm.Between(C_PUSHER_START, C_PUSHER_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_LD_MZ_PUSHER_CYCLE_TM_OVER);
		return;
	}

	if(g_pNV->UseSkip(usPusherOverload))
	{
		if(C_PUSHER_START == m_fsm.Get())
		{
			if(g_dIn.BOn(iMzPuhserOverload) && !g_opr.isDryRun)
			{
				m_pMtX->Paused();
				g_mt[MT_PUSHER_X].CancelHomeSearch();
				m_fsm.Set(C_ERROR, ER_LD_MZ_PUSHER_OVER_LOAD);
				return;
			}
		}
	}
	
	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];

	if (g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID) + 1);
	_sprintf(cMaterialType, L"PCB");	

	_char cLotId[_MAX_CHAR_SIZE_], cPartId[_MAX_CHAR_SIZE_];
	mbstowcs(cLotId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].lotID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].lotID) + 1);
	mbstowcs(cPartId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].partID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].partID) + 1);

	switch(m_fsm.Get())
	{
	case C_PUSHER_START:
		if(m_fsm.Once())
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"MOVE_PCB", g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cLdmz.deviceId, L"RAIL")) // PUSH 직전 $

		if(IsReadyMtPusherXFwd())
		{
			if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_FWD])
			{
				g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_FWD] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_PUSHER_X_MOVE_FWD", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
													g_data2c.cLdmz.PusherX[PX_FWD][_POSIDX_], g_data2c.cLdmz.PusherX[PX_FWD][_POS_], 
													g_data2c.cLdmz.PusherX[PX_FWD][_SPDIDX_], g_data2c.cLdmz.PusherX[PX_FWD][_SPD_], 
													g_data2c.cLdmz.PusherX[PX_FWD][_ACCIDX_], g_data2c.cLdmz.PusherX[PX_FWD][_ACC_]))
			}

			if(m_bNewMz)
			{
				g_rail.m_bLotMgzFirst = TRUE;
				g_rail.m_bManualLotIn = FALSE;
				g_pNV->NDm(stateManualLotIn) = 0;
				// 무조건 새로운 Lot으로 간주한다.
				// LotSplit가 진행될 수 없는 구조임
				g_lotInfo.LotSplitAllClear();
				g_pNV->NDm(lotSplitCount) = 0;
				g_pNV->NDm(flagSplitInfo) = FALSE;
				g_pNV->NDm(flagSplitIDLotStart) = FALSE;
			}

			if(g_dIn.AOn(iRailExistStart) || g_dIn.AOn(iRailExistMid1) || g_dIn.AOn(iRailExistMid2) ||
				g_dIn.AOn(iRailGripperExist) || g_opr.isDryRun)
			{
				if(m_bNewMz && m_bReworkMz)	// Rework 매거진은 첫슬롯이 비워져 있음..
				{										// 비워져 있지 않을 경우 정상 Lot 매거진이 투입된것임.
					g_rail.m_pCylGripFB->Actuate(pmBWD);
					m_pMtX->Move(PX_BWD);
					m_fsm.Set(C_ERROR, ER_LD_MZ_REWORK_LOT_MIX);
					break;
				}
				m_bNewMz = FALSE;
				g_rail.Exist() = TRUE;

				g_lotInfo.LotInfoCopy(LOT_INFO_MGZ, LOT_INFO_RAIL);
			}
			m_fsm.Set(C_PUSHER_CHK_COMP);
		}
		else
		{
			if(g_pNV->Pkg(PcbLengthOptionUse) == 1)
				g_rail.m_pCylGripFB->Actuate(pmFWD);
			else
				g_rail.m_pCylGripFB->Actuate(pmBWD);

			MoveMtPusherXFwd();

			if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_FWD])
			{
				g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_FWD] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_PUSHER_X_MOVE_FWD", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
													g_data2c.cLdmz.PusherX[PX_FWD][_POSIDX_], g_data2c.cLdmz.PusherX[PX_FWD][_POS_], 
													g_data2c.cLdmz.PusherX[PX_FWD][_SPDIDX_], g_data2c.cLdmz.PusherX[PX_FWD][_SPD_], 
													g_data2c.cLdmz.PusherX[PX_FWD][_ACCIDX_], g_data2c.cLdmz.PusherX[PX_FWD][_ACC_]))
			}
		}
		break;
	case C_PUSHER_CHK_COMP:
		{
			BOOL goNext = m_fsm.Delay(2000) || g_dIn.AOn(iRailExistStart) || 
				          g_dIn.AOn(iRailExistMid1) || g_dIn.AOn(iRailExistMid2) || 
						  g_dIn.AOn(iRailGripperExist) || g_opr.isDryRun;
			 
			if(goNext)
			{
				m_pMtX->Move(PX_BWD);
				if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_BWD])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_BWD] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_PUSHER_X_MOVE_BWD", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
														g_data2c.cLdmz.PusherX[PX_BWD][_POSIDX_], g_data2c.cLdmz.PusherX[PX_BWD][_POS_], 
														g_data2c.cLdmz.PusherX[PX_BWD][_SPDIDX_], g_data2c.cLdmz.PusherX[PX_BWD][_SPD_], 
														g_data2c.cLdmz.PusherX[PX_BWD][_ACCIDX_], g_data2c.cLdmz.PusherX[PX_BWD][_ACC_]))
				}
				m_fsm.Set(C_PUSHER_END);
			}
		}
		break;
	case C_PUSHER_END:
		if(!m_bNewRailInfo && (0 == _stricmp(g_pNV->m_pLotInfo->history[0].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_OLD_RAIL].mergeLotID))&&
			(NULL != g_pNV->m_pLotInfo->history[0].mergeLotID[0]) &&
			(g_pNV->m_pLotInfo->data[LOT_INFO_OLD_RAIL].lotQty<=g_pNV->m_pLotInfo->history[0].lotMergeCurCnt))
		{
			// m_bCompWork = TRUE; // 조건 재검증 필요
		}
		CmdSlotNo()++;

		if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_BWD])
		{
			g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_BWD] = FALSE;
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_PUSHER_X_MOVE_BWD", g_data2c.cEtc.end, 
												cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
												g_data2c.cLdmz.PusherX[PX_BWD][_POSIDX_], g_data2c.cLdmz.PusherX[PX_BWD][_POS_], 
												g_data2c.cLdmz.PusherX[PX_BWD][_SPDIDX_], g_data2c.cLdmz.PusherX[PX_BWD][_SPD_], 
												g_data2c.cLdmz.PusherX[PX_BWD][_ACCIDX_], g_data2c.cLdmz.PusherX[PX_BWD][_ACC_]))
		}

		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"MOVE_PCB", g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cLdmz.deviceId, L"RAIL"))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CLdMz::CycleRfidWrite(void)
{
	if(!m_fsm.Between(C_RFID_WRITE_START, C_RFID_WRITE_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_LD_MZ_RFID_WRITE_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID) + 1);
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_RFID_WRITE_START:
		m_nRfidRetryCnt = 0;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"RFID_WRITE", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"RFID_WRITE_START", L"RFID_WRITE_END"))
		m_fsm.Set(C_RFID_WRITE_01);
		break;
	case C_RFID_WRITE_01:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateRfidWrite) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_fsm.Set(C_RFID_WRITE_02);
				break;
			}

			switch(g_pNV->NDm(stateRfidWrite))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				m_fsm.Set(C_RFID_WRITE_END);
				break;
			case STATE_ERR:
				m_fsm.Set(C_RFID_WRITE_02);
				break;
			}
		}
		break;
	case C_RFID_WRITE_02:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateRfidWrite) = STATE_IDLE;

			m_nRfidRetryCnt++;

			if(RFID_FAIL_MAX < m_nRfidRetryCnt)
			{
				m_fsm.Set(C_ERROR, ER_LD_MZ_FAIL_RFID_WRITE);
			}
		}
		else
		{
			if(!m_fsm.Delay(1000))
				break;
			m_fsm.Set(C_RFID_WRITE_END);
		}
		break;
	case C_RFID_WRITE_END:
		m_pMtY->Move(PY_RAIL);
		g_pNV->NDm(stateRfidWrite) = STATE_IDLE;
		m_bCompRfidWrite = TRUE;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"RFID_WRITE", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"RFID_WRITE_START", L"RFID_WRITE_END"))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CLdMz::CycleRfidWriteCheck(void)
{
	if(!m_fsm.Between(C_RFID_WRITE_CHECK_START, C_RFID_WRITE_CHECK_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_LD_MZ_RFID_WRITE_CHECK_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if (g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID) + 1);
	_sprintf(cMaterialType, L"MZ");	

	switch(m_fsm.Get())
	{
	case C_RFID_WRITE_CHECK_START:
		m_nRfidRetryCnt = 0;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"RFID_DATA_COMP", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"RFID_DATA_COMP_START", L"RFID_DATA_COMP_END"))
		m_fsm.Set(C_RFID_WRITE_CHECK_01);
		break;
	case C_RFID_WRITE_CHECK_01:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateRfidWriteCheck) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_fsm.Set(C_RFID_WRITE_CHECK_02);
				break;
			}

			switch(g_pNV->NDm(stateRfidWriteCheck))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				m_fsm.Set(C_RFID_WRITE_CHECK_END);
				break;
			case STATE_ERR:
				m_fsm.Set(C_RFID_WRITE_CHECK_02);
				break;
			}
		}
		break;
	case C_RFID_WRITE_CHECK_02:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateRfidWriteCheck) = STATE_IDLE;

			m_nRfidRetryCnt++;

			if(RFID_FAIL_MAX < m_nRfidRetryCnt)
			{
				m_fsm.Set(C_ERROR, ER_LD_MZ_FAIL_RFID_WRITE_CHECK);
			}
		}
		else
		{
			if(!m_fsm.Delay(1000))
				break;
			m_fsm.Set(C_RFID_WRITE_CHECK_END);
		}
		break;
	case C_RFID_WRITE_CHECK_END:
		m_pMtY->Move(PY_RAIL);
		g_pNV->NDm(stateRfidWriteCheck) = STATE_IDLE;
		m_bCompRfidWriteCheck = TRUE;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"RFID_DATA_COMP", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"RFID_DATA_COMP_START", L"RFID_DATA_COMP_END"))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CLdMz::CycleEject(void)
{
	if(!m_fsm.Between(C_EJECT_START, C_EJECT_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_LD_MZ_EJECT_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtZ->IsRdy() || !m_pMtY->IsRdy() || !m_pMtX->IsRdy())
		return;

	// negrete
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if (g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].carrierID) + 1);

	_sprintf(cMaterialType, L"MZ");	

	_char cLotId[_MAX_CHAR_SIZE_], cPartId[_MAX_CHAR_SIZE_];
	mbstowcs(cLotId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].lotID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].lotID) + 1);
	mbstowcs(cPartId, g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].partID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_MGZ].partID) + 1);

	switch(m_fsm.Get())
	{
	case C_EJECT_START:
		if(m_fsm.Once())
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"UNLOADING_MGZ", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"LOADER", L"MGZ_OUT_CONV"))

		if(pmBWD != g_rail.m_pCylGripFB->GetPos(300))
		{
			g_rail.m_pCylGripFB->Actuate(pmBWD); 
			break;
		}

		if(m_pMtY->InPos(PY_RAIL))
		{
			if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RAIL_UNLOADING", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
													g_data2c.cLdmz.Y[PY_RAIL][_POSIDX_], g_data2c.cLdmz.Y[PY_RAIL][_POS_], 
													g_data2c.cLdmz.Y[PY_RAIL][_SPDIDX_], g_data2c.cLdmz.Y[PY_RAIL][_SPD_], 
													g_data2c.cLdmz.Y[PY_RAIL][_ACCIDX_], g_data2c.cLdmz.Y[PY_RAIL][_ACC_]))
			}	

			if(Exist())
			{
				if(m_pMtZ->InPos(PZ_EJECT))
				{
					if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_EJECT])
					{
						g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_EJECT] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_EJECT", g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
															g_data2c.cLdmz.Z[PZ_EJECT][_POSIDX_], g_data2c.cLdmz.Z[PZ_EJECT][_POS_], 
															g_data2c.cLdmz.Z[PZ_EJECT][_SPDIDX_], g_data2c.cLdmz.Z[PZ_EJECT][_SPD_], 
															g_data2c.cLdmz.Z[PZ_EJECT][_ACCIDX_], g_data2c.cLdmz.Z[PZ_EJECT][_ACC_]))
					}
					m_fsm.Set(C_EJECT_MOVE_FWD);
				}
				else
				{
					m_pMtZ->Move(PZ_EJECT);

					if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_EJECT])
					{
						g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_EJECT] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_EJECT", g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
															g_data2c.cLdmz.Z[PZ_EJECT][_POSIDX_], g_data2c.cLdmz.Z[PZ_EJECT][_POS_], 
															g_data2c.cLdmz.Z[PZ_EJECT][_SPDIDX_], g_data2c.cLdmz.Z[PZ_EJECT][_SPD_], 
															g_data2c.cLdmz.Z[PZ_EJECT][_ACCIDX_], g_data2c.cLdmz.Z[PZ_EJECT][_ACC_]))
					}
				}
			}
			else
			{
				if(pmOPEN != m_pCylClampOC->GetPos(300))
				{
					m_pCylClampOC->Actuate(pmOPEN);

					if(!g_logChk.bFunction[m_pCylClampOC->GetNo()])
					{
						g_logChk.bFunction[m_pCylClampOC->GetNo()] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_OPEN_UNLOADING", g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmON], L"'OFF'", 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmOFF], L"'ON'"))
					}
					break;
				}
				else
				{
					if(g_logChk.bFunction[m_pCylClampOC->GetNo()])
					{
						g_logChk.bFunction[m_pCylClampOC->GetNo()] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_OPEN_UNLOADING", g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmON], L"'ON'", 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmOFF], L"'OFF'"))
					}
				}

				m_fsm.Set(C_EJECT_END);
			}
		}
		else if(m_pMtY->InPos(PY_EJECT))
		{
			m_fsm.Set(C_EJECT_MOVE_FWD);
		}
		else
		{
			if(CanMtYMove())
			{
				m_pMtY->Move(PY_RAIL);

				if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RAIL_UNLOADING", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
														g_data2c.cLdmz.Y[PY_RAIL][_POSIDX_], g_data2c.cLdmz.Y[PY_RAIL][_POS_], 
														g_data2c.cLdmz.Y[PY_RAIL][_SPDIDX_], g_data2c.cLdmz.Y[PY_RAIL][_SPD_], 
														g_data2c.cLdmz.Y[PY_RAIL][_ACCIDX_], g_data2c.cLdmz.Y[PY_RAIL][_ACC_]))
				}
				break;
			}
			else
				m_fsm.Set(C_ERROR, ER_LD_MZ_MTY_CANNOT_MOVE);
		}
		break;
	case C_EJECT_MOVE_FWD:
		if(m_pMtY->InPos(PY_EJECT))
		{
			if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_EJECT])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_EJECT] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_EJECT_UNLOADING", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
													g_data2c.cLdmz.Y[PY_EJECT][_POSIDX_], g_data2c.cLdmz.Y[PY_EJECT][_POS_], 
													g_data2c.cLdmz.Y[PY_EJECT][_SPDIDX_], g_data2c.cLdmz.Y[PY_EJECT][_SPD_], 
													g_data2c.cLdmz.Y[PY_EJECT][_ACCIDX_], g_data2c.cLdmz.Y[PY_EJECT][_ACC_]))
			}	

			if(m_pMtZ->InPos(PZ_EJECT))
			{
				m_pMtZ->Move(PZ_CLAMP_UP);

				if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_CLAMP_UP])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_CLAMP_UP] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_CLAMP_UP_UNLOADING", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cLdmz.Z[PZ_CLAMP_UP][_POSIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_UP][_POS_], 
														g_data2c.cLdmz.Z[PZ_CLAMP_UP][_SPDIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_UP][_SPD_], 
														g_data2c.cLdmz.Z[PZ_CLAMP_UP][_ACCIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_UP][_ACC_]))
				}
			}
			else if(m_pMtZ->InPos(PZ_CLAMP_UP))
			{
				if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_CLAMP_UP])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_CLAMP_UP] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_CLAMP_UP_UNLOADING", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cLdmz.Z[PZ_CLAMP_UP][_POSIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_UP][_POS_], 
														g_data2c.cLdmz.Z[PZ_CLAMP_UP][_SPDIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_UP][_SPD_], 
														g_data2c.cLdmz.Z[PZ_CLAMP_UP][_ACCIDX_], g_data2c.cLdmz.Z[PZ_CLAMP_UP][_ACC_]))
				}

				if(pmOPEN != m_pCylClampOC->GetPos(300))
				{
					m_pCylClampOC->Actuate(pmOPEN);

					if(!g_logChk.bFunction[m_pCylClampOC->GetNo()])
					{
						g_logChk.bFunction[m_pCylClampOC->GetNo()] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_OPEN_UNLOADING", g_data2c.cEtc.start, 
															cMaterialId, cMaterialType,  g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.off, 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.on))
					}
					break;
				}
				else
				{
					if(g_logChk.bFunction[m_pCylClampOC->GetNo()])
					{
						g_logChk.bFunction[m_pCylClampOC->GetNo()] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"CYL_LD_MGZ_CLAMP_OPEN_UNLOADING", g_data2c.cEtc.end, 
															cMaterialId, cMaterialType,  g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylClampOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.on, 
															g_data2c.cPmIO[m_pCylClampOC->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.off))
					}
				}

				m_pMtZ->Move(PZ_EJECT_BWD);

				if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_EJECT_BWD])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_EJECT_BWD] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_EJECT_BWD", g_data2c.cEtc.start, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
									g_data2c.cLdmz.Z[PZ_EJECT_BWD][_POSIDX_], g_data2c.cLdmz.Z[PZ_EJECT_BWD][_POS_], 						
									g_data2c.cLdmz.Z[PZ_EJECT_BWD][_SPDIDX_], g_data2c.cLdmz.Z[PZ_EJECT_BWD][_SPD_], 
									g_data2c.cLdmz.Z[PZ_EJECT_BWD][_ACCIDX_], g_data2c.cLdmz.Z[PZ_EJECT_BWD][_ACC_]))
				}
			}
			else if(m_pMtZ->InPos(PZ_EJECT_BWD))
			{
				g_ldMzOutConv.ExistArrival() = TRUE; 

				if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_EJECT_BWD])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_EJECT_BWD] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Z_MOVE_EJECT_BWD", g_data2c.cEtc.end, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
									g_data2c.cLdmz.Z[PZ_EJECT_BWD][_POSIDX_], g_data2c.cLdmz.Z[PZ_EJECT_BWD][_POS_], 
									g_data2c.cLdmz.Z[PZ_EJECT_BWD][_SPDIDX_], g_data2c.cLdmz.Z[PZ_EJECT_BWD][_SPD_], 
									g_data2c.cLdmz.Z[PZ_EJECT_BWD][_ACCIDX_], g_data2c.cLdmz.Z[PZ_EJECT_BWD][_ACC_]))
				}
				m_fsm.Set(C_EJECT_END);
			}
			else
			{
				if(!m_fsm.Delay(3000))
					break;
				if(CanMtYMove())
				{
					m_pMtY->Move(PY_RAIL);
					m_fsm.Set(C_EJECT_START);
				}
				else
				{
					m_fsm.Set(C_ERROR, ER_LD_MZ_MTY_CANNOT_MOVE);
				}
			}
		}
		else
		{
			m_pMtY->Move(PY_EJECT);

			if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_EJECT])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_EJECT] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_EJECT_UNLOADING", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
													g_data2c.cLdmz.Y[PY_EJECT][_POSIDX_], g_data2c.cLdmz.Y[PY_EJECT][_POS_], 
													g_data2c.cLdmz.Y[PY_EJECT][_SPDIDX_], g_data2c.cLdmz.Y[PY_EJECT][_SPD_], 
													g_data2c.cLdmz.Y[PY_EJECT][_ACCIDX_], g_data2c.cLdmz.Y[PY_EJECT][_ACC_]))
			}	
		}
		break;
	case C_EJECT_END:
		if(m_fsm.Once())
		{
			m_bReworkMz				= FALSE;
			m_bCompAlign			= FALSE;
			m_bCompRfidRead			= FALSE;
			m_bCompMergeInfo		= FALSE;
			m_bCompCarrierIdRead	= FALSE;
			m_bCompWork				= FALSE;
			m_bCompRfidWrite		= FALSE;
			m_bCompRfidWriteCheck	= FALSE;
			m_bNewMz				= TRUE;
			m_bNewRailInfo		    = TRUE;
			CurSlotNo()				= 0;
			CmdSlotNo()				= 1;
			m_nRfidRetryCnt			= 0;
			Exist()					= FALSE;
			g_pNV->DDm(lotCurQty) = 0; 

			g_lotInfo.LotInfoCopy(LOT_INFO_MGZ, LOT_INFO_OLD_MGZ);
			g_lotInfo.LotInfoCopy(LOT_INFO_MGZ, LOT_INFO_CONV_ARRIVAL);
			g_lotInfo.LotInfoClear(LOT_INFO_MGZ);
		}
		else
		{
			if(!g_dIn.AOn(iMzOutArrival) && !g_opr.isDryRun)
			{
				//MGZ이 감지안되는 상황 Error
				//확인 후 Error 발생 시점 수정
				g_ldMz.m_bManualOut = FALSE;
				
				m_fsm.Set(C_ERROR, ER_MZ_OUT_ARRIVAR_EXIST_NOT_ON);
				break;
			}

			if(!m_pMtY->InPos(PY_RAIL))
			{
				m_pMtY->Move(PY_RAIL);

				if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RAIL_UNLOADING_END", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
														g_data2c.cLdmz.Y[PY_RAIL][_POSIDX_], g_data2c.cLdmz.Y[PY_RAIL][_POS_], 
														g_data2c.cLdmz.Y[PY_RAIL][_SPDIDX_], g_data2c.cLdmz.Y[PY_RAIL][_SPD_], 
														g_data2c.cLdmz.Y[PY_RAIL][_ACCIDX_], g_data2c.cLdmz.Y[PY_RAIL][_ACC_]))	
				}				
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cLdmz.deviceId, L"MT_LD_Y_MOVE_RAIL_UNLOADING_END", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
														g_data2c.cLdmz.Y[PY_RAIL][_POSIDX_], g_data2c.cLdmz.Y[PY_RAIL][_POS_], 
														g_data2c.cLdmz.Y[PY_RAIL][_SPDIDX_], g_data2c.cLdmz.Y[PY_RAIL][_SPD_], 
														g_data2c.cLdmz.Y[PY_RAIL][_ACCIDX_], g_data2c.cLdmz.Y[PY_RAIL][_ACC_]))	
				}	
			}

			if(g_ldMz.m_bManualOut)
			{
				g_ldMz.m_bManualOut = FALSE;
				g_ldMzOutConv.m_bRun = TRUE;
			}

			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cLdmz.deviceId, L"UNLOADING_MGZ", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"LOADER", L"MGZ_OUT_CONV"))
			m_fsm.Set(C_IDLE);
		}
		break;
	}
}


//-------------------------------------------------------------------
int CLdMz::GetState(void)
{
	int nState = S_IDLE;

	if(m_pMtY->InPos(PY_RCV))
		nState = S_LOADING_EXCEPTION;
	else if(m_pMtY->InPos(PY_EJECT))
		nState = S_EJECT_EXCEPTION;
	else
	{
		if(Exist())
		{
			if(!m_bCompAlign)
				nState = S_ALIGN;
			else if(!m_bCompRfidRead)
				nState = S_RFID_READ;
			else if(!m_bCompPartNoCompare)
				nState = S_PART_NO_COMPARE;
			else if(!m_bCompAutoRecipeChg)
				nState = S_AUTO_RECIPE_CHG;
			else if(!m_bCompCarrierIdRead)
				nState = S_CARRIER_ID_READ;
			else if(!m_bCompMergeInfo)
				nState = S_MERGE_INFO;
			else if(!m_bCompTrayInfo)
				nState = S_TRAY_INFO;
			else if(!m_bCompWork)
				nState = S_WORK;
			else if(!m_bCompRfidWrite)
				nState = S_RFID_WRITE;
			else if(!m_bCompRfidWriteCheck)
				nState = S_RFID_WRITE_CHECK;
			else
				nState = S_EJECT;
		}
		else
		{
			nState = S_LOADING;
		}
	}
	return (nState);
}


//-------------------------------------------------------------------
BOOL CLdMz::IsErr(void)
{
	if(!m_pMtY->m_state.isHome || !m_pMtZ->m_state.isHome || !m_pMtX->m_state.isHome)
		return (TRUE);
	if(0 < m_pCylAlignFB->GetErr())
		return (TRUE);
	if(0 < m_pCylClampOC->GetErr())
		return (TRUE);

	return (FALSE);
}


//-------------------------------------------------------------------
int CLdMz::GetExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	if(Exist() == (g_dIn.AOn(iMzClampLExist) || g_dIn.AOn(iMzClampRExist)))
	{
		m_tmExistErr.Reset();
	}
	else
	{
		if(m_tmExistErr.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
// Z축의 위치를 확인하여 Y축이 이동 가능한지 여부
BOOL CLdMz::CanMtYMove(void)
{
	if(!Exist())
		return (true);

	int nMtYSafety = (int)(m_pMtY->m_pTable->pos[PY_RAIL] + 30000);
	if(nMtYSafety > m_pMtY->m_state.realCnt)
		return (TRUE);

	int nCurZPos = (int)m_pMtZ->m_state.realCnt;

	int nMin1 = 20000;	// 매거진 로딩할때 간섭위치
	int nMax1 = 50000;

	//int nMin2 = 500000;	// 매거진 언로딩할때 간섭위치
	//int nMax2 = 700000;

	if(Between(nCurZPos, nMin1, nMax1))
		return (FALSE);

	//if(Between(nCurZPos, nMin2, nMax2))
	//	return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
// nNo : 1~ 상단부터 Pusher
double CLdMz::GetMzZSlotPos(int nNo)
{
	double basePosition   = m_pMtZ->m_pTable->pos[PZ_RAIL];
	double railZOffset    = g_pNV->Pkg(LoaderZRailPosOffset) * 1000.0;
	double slotOffset		= g_pNV->Pkg(mzSlotZOffset) * nNo * 1000.0; 
	double targetPosition 	= (basePosition + railZOffset) - slotOffset;
	return (targetPosition);
}
// nNo : 1~ 왼쪽부터 Pusher

double CLdMz::GetMzYSlotPos(int nNo)
{
	double basePosition;  
	double slotOffset;   
	double targetPosition;

	if( g_pNV->UseSkip(usMzRightStart) )
	{
		nNo = ((int)g_pNV->Pkg(mzSlotYCnt)-1) - nNo;
	}
	basePosition   = m_pMtY->m_pTable->pos[PY_RAIL];
	slotOffset     = g_pNV->Pkg(mzSlotYOffset) * nNo * 1000.0; 
	targetPosition = basePosition - slotOffset;

	return (targetPosition);
}


//-------------------------------------------------------------------
BOOL CLdMz::IsReadyMtPusherXFwd() 
{
	// 여기도 연산 포함해야함
	int    nIndex   = PX_FWD;
	double dOffset  = (g_pNV->DDm(commonPcbLength) - g_pNV->Pkg(pcbXLength)) * 1000.0;
		   dOffset -= g_pNV->Pkg(offsetLdMzPusherXFwd) * 1000.0;	
	//double dOffset  = g_pNV->Pkg(offsetLdMzPusherXFwd) * 1000.0; // 기존 옵션
	//       dOffset += (g_pNV->DDm(commonPcbLength) - g_pNV->Pkg(pcbXLength)) * 1000.0;
	double dPos	    = m_pMtX->m_pTable->pos[nIndex] + dOffset;

	if(!m_pMtX->IsRdy())
		return (FALSE);

	if(!m_pMtX->InPos(nIndex, dPos, 50))
		return (FALSE);

	return (TRUE);
}
	
//-------------------------------------------------------------------
BOOL CLdMz::MoveMtPusherXFwd() // 이동
{
	int	   nIndex   = PX_FWD;
	double dOffset = (g_pNV->DDm(commonPcbLength) - g_pNV->Pkg(pcbXLength)) * 1000.0; 	
	       dOffset -= g_pNV->Pkg(offsetLdMzPusherXFwd) * 1000.0;
	//double dOffset  = g_pNV->Pkg(offsetLdMzPusherXFwd) * 1000.0; // 기존 옵션
	//       dOffset += (g_pNV->DDm(commonPcbLength) - g_pNV->Pkg(pcbXLength)) * 1000.0;
	double dPos	    = m_pMtX->m_pTable->pos[nIndex] + dOffset;

	m_pMtX->PMove(nIndex, dPos);

	return (TRUE);
}
	
//-------------------------------------------------------------------
BOOL CLdMz::IsReadyMtPusherX2D() // 사용 안함
{
	int    nIndex   = PX_2D;
	double dOffset  = g_pNV->Pkg(offsetLdMzPusherX2D) * 1000.0;	
	       dOffset += (g_pNV->DDm(commonPcbLength) - g_pNV->Pkg(pcbXLength)) * 1000.0;
	double dPos	    = m_pMtX->m_pTable->pos[nIndex] + dOffset;

	if(!m_pMtX->IsRdy())
		return (FALSE);

	if(!m_pMtX->InPos(nIndex, dPos, 50))
		return (FALSE);

	return (TRUE);
}
	
//-------------------------------------------------------------------
BOOL CLdMz::MoveMtPusherX2D() // 사용 안함
{
	int	   nIndex   = PX_2D;
	double dOffset  = g_pNV->Pkg(offsetLdMzPusherXFwd) * 1000.0;
	       dOffset += (g_pNV->DDm(commonPcbLength) - g_pNV->Pkg(pcbXLength)) * 1000.0;
	double dPos	    = m_pMtX->m_pTable->pos[nIndex] + dOffset;

	m_pMtX->PMove(nIndex, dPos);

	return (TRUE);
}
	
