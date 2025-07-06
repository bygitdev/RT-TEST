#include "..\DEF\Includes.h"


//////////////////////////////////////////////////////////////////////////
CRouter g_routerF;
CRouter g_routerR;
//////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
inline BOOL IsSpindleRearSkip(int nPart)
{
	BOOL bSkip = FALSE;

	if(ROUTER_PART_F == nPart)
		bSkip = g_pNV->UseSkip(usSpindle2Skip);
	else if(ROUTER_PART_R == nPart)
		bSkip = g_pNV->UseSkip(usSpindle4Skip);

	return (bSkip);
}

CRouter::CRouter()
{
	m_nCurIndex  = INDEX_FR_IDLE;
	m_nNextIndex = INDEX_F;

	m_nBitBrokenErrCntF = 0;
	m_nBitBrokenErrCntR = 0;
	m_nBitDownErrCntF = 0;
	m_nBitDownErrCntR = 0;
	
	m_nBitCurIdx = 0;
	m_nBitChangeIdx = 0;
	m_nBitWireIdx	= 0;
	m_nBitVisionIdx = 0;
	m_nBitBrokenIdx = 0;

	m_bNeedESDCheckF = FALSE;
	m_bNeedESDCheckR = FALSE;
	m_bNeedBitChangeF = FALSE;
	m_bNeedBitChangeR = FALSE;
	m_bNeedBitColorF = FALSE;
	m_bNeedBitColorR = FALSE;
	m_bNeedBitVisionF = FALSE;
	m_bNeedBitVisionR = FALSE;
	m_bNeedBitBrokenCheckF = FALSE;
	m_bNeedBitBrokenCheckR = FALSE;

	m_bReqReadyPos = FALSE;
	m_bReqRouterBitReadyPos = FALSE;

	m_viPrsData.dX = m_viPrsData.dY = 0; 

	m_nLiveViPos = 0;
	m_bReadyPos = FALSE;
	m_nPrsRetry = 0;
	m_nPrsArrayYRetry = 0;

	m_nBitAlignRetry = 0;
}

//-------------------------------------------------------------------
void CRouter::AutoRun()
{
	m_bReadyPos = FALSE;
	m_bReqReadyPos = FALSE;
	m_bReqRouterBitReadyPos = FALSE;

	if(!m_pInfoBitF->nExist && !m_fsm.IsRun())
	{
		m_pInfoBitF->nLength   = 0;
		m_pInfoBitF->nZStep    = 0;
		m_bNeedESDCheckF	   = FALSE;
		m_bNeedBitChangeF	   = FALSE;
		//m_bNeedBitColorF	   = FALSE;
		m_bNeedBitVisionF	   = FALSE;
		m_bNeedBitBrokenCheckF = FALSE;
		flagSpidleBitChangeF() = FALSE;
	}
	if(!m_pInfoBitR->nExist && !m_fsm.IsRun())
	{
		m_pInfoBitR->nLength   = 0;
		m_pInfoBitR->nZStep    = 0;
		m_bNeedESDCheckR	   = FALSE;
		m_bNeedBitChangeR	   = FALSE;
		//m_bNeedBitColorR	   = FALSE;
		m_bNeedBitVisionR	   = FALSE;
		m_bNeedBitBrokenCheckR = FALSE;
		flagSpidleBitChangeR() = FALSE;
	}

	if(!m_bRun)
		return;

	m_bRun = FALSE;

	m_nBitWireIdx   = GetBitWireIndex();
	m_nBitChangeIdx = GetBitChangeIndex();
	m_nBitVisionIdx = GetBitVisionIndex();
	m_nBitBrokenIdx = GetBitBrokenIndex();

	if(m_fsm.IsRun())
		return;

	if(!IsSpindle2PinPitch())
	{
		g_err.Save(ER_ROUTER_Y_SPINDLE_PITCH_ERR);
		return;
	}

	// Bit Change시 Bit Align Exist 확인
	int existErrValF = GetExistBitAlignFErr();
	if(EXIST_UNCERTAIN == existErrValF)
		return;
	if(EXIST_ERR == existErrValF)
	{
		g_err.Save(ER_INDEX_BIT_ALIGN_INDEX_01_EXIST_01 + (m_nId*2));
		return;
	}

	// Bit Change시 Bit Align Exist 확인
	int existErrValR = GetExistBitAlignRErr();
	if(EXIST_UNCERTAIN == existErrValR)
		return;
	if(EXIST_ERR == existErrValR)
	{
		g_err.Save(ER_INDEX_BIT_ALIGN_INDEX_01_EXIST_02 + (m_nId*2));
		return;
	}

	int nCurIndexNo = (((m_nId % 2) * 2) + m_nCurIndex);

	if(ROUTER_PART_F == m_nId)
	{
		if ((g_routerR.m_nCurIndex == INDEX_F && g_routerF.m_nCurIndex == INDEX_R) || (g_routerR.m_nCurIndex == INDEX_F && g_routerF.m_nBitChangeIdx == INDEX_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE))
		{
			if (g_routerR.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
				g_routerR.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
				g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
				g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
				g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
				g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
				g_routerR.m_fsm.Between(C_LDC_START, C_LDC_END) ||
				g_routerR.m_fsm.Between(C_PRS_START, C_PRS_END) ||
				g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
			{
				return;
			}
		}

		if (g_routerR.m_fsm.Between(C_SPD_ESD_CHECK_START, C_SPD_ESD_CHECK_END))
			return;
		if (g_routerR.m_fsm.Between(C_SPD_BIT_VI_START, C_SPD_BIT_VI_END))
			return;

		// 간섭 위치
		BOOL bRun  = g_routerR.m_pMtY->ComparePos(PY_SPD_BIT_VI_F);
		bRun |= g_routerR.m_pMtY->ComparePos(PY_SPD_BIT_VI_R);
		bRun |= g_routerR.m_pMtY->ComparePos(PY_SPD_WIRE_CHECK_F);
		bRun |= g_routerR.m_pMtY->ComparePos(PY_SPD_WIRE_CHECK_R);

		if (g_routerR.m_nBitChangeIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
		{
			bRun |= g_routerR.m_pMtY->ComparePos(PY_SPD_BIT_EJECT_BLUE_03);
			bRun |= g_routerR.m_pMtY->ComparePos(PY_SPD_BIT_CLAMP_BLUE_03_R);
			bRun |= g_routerR.m_pMtY->ComparePos(PY_CYL_BIT_CLAMP_BLUE);
			bRun |= g_routerR.m_pMtY->ComparePos(PY_CYL_BIT_ALIGN_BLUE_F);
			bRun |= g_routerR.m_pMtY->ComparePos(PY_CYL_BIT_ALIGN_BLUE_R);
		}

		if(bRun)
		{
			if(IsGentryMtYWRdy(PY_READY, PW_READY))
				m_bReadyPos = TRUE;
			return;
		}
	}
	else // Rear Part
	{
		if ((g_routerF.m_nCurIndex == INDEX_R && g_routerR.m_nCurIndex == INDEX_F) || (g_routerF.m_nCurIndex == INDEX_R && g_routerR.m_nBitChangeIdx == INDEX_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE))
		{
			if (g_routerF.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
				g_routerF.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
				g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
				g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
				g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
				g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
				g_routerF.m_fsm.Between(C_LDC_START, C_LDC_END) ||
				g_routerF.m_fsm.Between(C_PRS_START, C_PRS_END) ||
				g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
			{
				return;
			}
		}

		if (g_routerF.m_fsm.Between(C_SPD_ESD_CHECK_START, C_SPD_ESD_CHECK_END))
			return;
		if (g_routerF.m_fsm.Between(C_SPD_BIT_VI_START, C_SPD_BIT_VI_END))
			return;

		// 간섭 위치
		BOOL bRun  = g_routerF.m_pMtY->ComparePos(PY_SPD_BIT_VI_F);
		bRun |= g_routerF.m_pMtY->ComparePos(PY_SPD_BIT_VI_R);
		bRun |= g_routerF.m_pMtY->ComparePos(PY_SPD_WIRE_CHECK_F);
		bRun |= g_routerF.m_pMtY->ComparePos(PY_SPD_WIRE_CHECK_R);

		if (g_routerF.m_nBitChangeIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
		{
			bRun |= g_routerF.m_pMtY->ComparePos(PY_SPD_BIT_EJECT_BLUE_02);
			bRun |= g_routerF.m_pMtY->ComparePos(PY_SPD_BIT_CLAMP_BLUE_02_R);
			bRun |= g_routerF.m_pMtY->ComparePos(PY_CYL_BIT_CLAMP_BLUE);
			bRun |= g_routerF.m_pMtY->ComparePos(PY_CYL_BIT_ALIGN_BLUE_F);
			bRun |= g_routerF.m_pMtY->ComparePos(PY_CYL_BIT_ALIGN_BLUE_R);
		}

		if(bRun)
		{
			if (IsGentryMtYWRdy(PY_READY, PW_READY))
				m_bReadyPos = TRUE;
			return;
		}
	}

	if(g_opr.isDryRun)
	{
		m_pInfoBitF->nExist  = TRUE;
		m_pInfoBitF->nLength = 0;
		m_pInfoBitF->nZStep  = 0;
		m_pInfoBitR->nExist  = TRUE;
		m_pInfoBitR->nLength = 0;
		m_pInfoBitR->nZStep  = 0;
	}

	if(!IsMtRdy())
		return;

	if(!m_pMtZ_F->InPos(PZ_READY) || !m_pMtZ_R->InPos(PZ_READY))
	{
		m_pMtZ_F->Move(PZ_READY);
		m_pMtZ_R->Move(PZ_READY);
		return;
	}
	if(pmUP != m_pCylBitClampUD->GetPos(300))
	{
		m_pCylBitClampUD->Actuate(pmUP);
		return;
	}
	if(m_pInfoBitF->nExist)
	{
		if(pmCLOSE != m_pSolSpdChuckOC_F->GetPos(300))
		{
			m_pSolSpdChuckOC_F->Actuate(pmCLOSE);
			return;
		}
	}
	if(m_pInfoBitR->nExist)
	{
		if(pmCLOSE != m_pSolSpdChuckOC_R->GetPos(300))
		{
			m_pSolSpdChuckOC_R->Actuate(pmCLOSE);
			return;
		}
	}

	if(pmOFF != m_pSolSpindleBlow->GetPos(300))
	{
		m_pSolSpindleBlow->Actuate(pmOFF);
		return;
	}
	if(pmOFF != m_pSolRouterIonizerF->GetPos(300))
	{
		m_pSolRouterIonizerF->Actuate(pmOFF);
		return;
	}
	if(pmOFF != m_pSolRouterIonizerR->GetPos(300))
	{
		m_pSolRouterIonizerR->Actuate(pmOFF);
		return;
	}

	if(!g_pNV->UseSkip(usBitChange))
	{
		if(ROUTER_F == m_nBitChangeIdx)
		{
			if(ROUTER_PART_F == m_nId)
				g_err.Save(ER_NEED_BIT_CHANGE_01);
			else
				g_err.Save(ER_NEED_BIT_CHANGE_02);

			m_pSpindleF->Actuate(pmOFF);
			m_pSpindleR->Actuate(pmOFF);
			return;
		}
		else if(ROUTER_R == m_nBitChangeIdx)
		{
			if(ROUTER_PART_F == m_nId)
				g_err.Save(ER_NEED_BIT_CHANGE_03);
			else
				g_err.Save(ER_NEED_BIT_CHANGE_04);

			m_pSpindleF->Actuate(pmOFF);
			m_pSpindleR->Actuate(pmOFF);
			return;
		}
	}

	if(INDEX_F == m_nCurIndex)
	{
		if(IsIndexRUseSkip())
			m_nNextIndex = INDEX_R;
		else
			m_nNextIndex = INDEX_F;
	}
	else if(INDEX_R == m_nCurIndex)
	{
		if(IsIndexFUseSkip())
			m_nNextIndex = INDEX_F;
		else
			m_nNextIndex = INDEX_R;
	}

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");	

	int nCurrentIndexNo = (((m_nId %2) * 2) + m_nCurIndex);
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + nCurrentIndexNo].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + nCurrentIndexNo].pcbBarcode) + 1);

	_char cYMoveEventId[_MAX_CHAR_SIZE_], cYPos[_MAX_CHAR_SIZE_], cWPos[_MAX_CHAR_SIZE_];
	_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_", (nCurrentIndexNo + 1));
	_char cWMoveEventIdX[_MAX_CHAR_SIZE_];
	_sprintf(cWMoveEventIdX, L"MT_ROUTER_W_%02d_", (nCurrentIndexNo + 1));

	switch(GetState())
	{
	case S_IDLE:
		m_pIndexF->SetIndexPosReq(INDEX_IDLE);
		m_pIndexR->SetIndexPosReq(INDEX_IDLE);

		if(!IsGentryMtYWRdy(PY_READY, PW_READY))
		{
			GentryMtYWMove(PY_READY, PW_READY);
		}
		else
			m_bReadyPos = TRUE;
		break;
	case S_READY:
		m_pIndexF->SetIndexPosReq(INDEX_IDLE);
		m_pIndexR->SetIndexPosReq(INDEX_IDLE);

		if(!IsGentryMtYWRdy(PY_READY, PW_READY))
		{
			GentryMtYWMove(PY_READY, PW_READY);
		}
		else
			m_bReadyPos = TRUE;
		break;
	case S_ROUTER: 
		m_pIndexF->SetIndexPosReq(INDEX_IDLE);
		m_pIndexR->SetIndexPosReq(INDEX_IDLE);

		// 여기서 Live Vision 까지 동작 할 수 있도록 한다.
		if(INDEX_F == m_nCurIndex)
		{
			if (ROUTER_PART_R == m_nId && g_routerF.m_nCurIndex == INDEX_R)
			{
				if (g_routerF.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
					g_routerF.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
					g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
					g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
					g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
					g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
					g_routerF.m_fsm.Between(C_LDC_START, C_LDC_END) ||
					g_routerF.m_fsm.Between(C_PRS_START, C_PRS_END) ||
					g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
				{
					break;
				}
			}

			if(m_pIndexF->IsReadyLoadCheck())
			{
				if(g_pNV->UseSkip(usLoadCheck))
				{
					POINT2D ptPos = m_pIndexF->GetRouterPrsPos(0);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

					if(!IsGentryMtYWPRdy(PY_VI_PRS_F, PW_READY, ptPos.dY, dPosW))
					{
						GentryMtYWPMove(PY_VI_PRS_F, PW_READY, ptPos.dY, dPosW);
						break;
					}
					
					m_fsm.Set(C_LDC_START, INDEX_F);
				}
			}
			else if(m_pIndexF->IsReadyRouterPrs())
			{
				_strcat(cYMoveEventId, L"PRS");
				_strcat(cWMoveEventIdX, L"PRS");
				if(g_pNV->UseSkip(usRouterPrs))
				{
					POINT2D ptPos = m_pIndexF->GetRouterPrsPos(0);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

					_sprintf(cYPos, L"%03f", ptPos.dY);
					_sprintf(cWPos, L"%03f", dPosW);

					if(!IsGentryMtYWPRdy(PY_VI_PRS_F, PW_READY, ptPos.dY, dPosW))
					{
						GentryMtYWPMove(PY_VI_PRS_F, PW_READY, ptPos.dY, dPosW);
						if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_F]) 
						{
							g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_F] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_POSIDX_], cYPos, 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_SPD_], 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_ACC_]))
						}
						if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
						{
							g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventIdX, g_data2c.cEtc.start, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
								g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], cWPos, 
								g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
								g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
						}
						break;
					}
					else
					{
						if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_F]) 
						{
							g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_F] = FALSE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_POSIDX_], cYPos, 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_SPD_], 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_ACC_]))
						}
						if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
						{
							g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = FALSE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventIdX, g_data2c.cEtc.end, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
								g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], cWPos, 
								g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
								g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
						}
					}

					m_fsm.Set(C_PRS_START, INDEX_F);
				}
			}
			else if(m_pIndexF->IsReadyLiveVision())
			{
				XYT xytPos = m_pIndexF->GetRouterLiveViPos(0, POS_START);
				double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

				if(!IsGentryMtYWPRdy(PY_VI_LIVE_F, PW_READY, xytPos.dY, dPosW))
				{
					GentryMtYWPMove(PY_VI_LIVE_F, PW_READY, xytPos.dY, dPosW);
					break;
				}

				m_fsm.Set(C_LIVE_VI_START, INDEX_F);
			}
			else if(m_pIndexF->IsReadyRouter())
			{
				_strcat(cYMoveEventId, L"CUT_01_START");
				_strcat(cWMoveEventIdX, L"CUT_01_START");

				XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
				POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
				POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
				double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
				double dOffset = GetBitYOffset();
				double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

				_sprintf(cYPos, L"%03f", xytPos.dY);
				_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);

				if(!IsGentryMtYWPRdy(PY_ROUTER_F, PW_READY, xytPos.dY, dPosW + dOffset + dSubOffset))
				{
					GentryMtYWPMove(PY_ROUTER_F, PW_READY, xytPos.dY, dPosW + dOffset + dSubOffset);
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F]) 
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventIdX, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
					}
					break;
				}
				else
				{
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F]) 
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventIdX, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
					}
				}

				double dPosF = GetZStepPos(ROUTER_F);
				double dPosR = GetZStepPos(ROUTER_R);

				if(ROUTER_PART_F == m_nId)
				{
					double dPosLimitF = g_pNV->DDm(spindle1ZLimitPos) * 1000.0;
					double dPosLimitR = g_pNV->DDm(spindle2ZLimitPos) * 1000.0;

					if(dPosLimitF <= dPosF)
					{
						g_err.Save(ER_SPINDLE_01_Z_ROUTER_POS_LIMIT);
						break;
					}
					if(dPosLimitR <= dPosR)
					{
						g_err.Save(ER_SPINDLE_02_Z_ROUTER_POS_LIMIT);
						break;
					}
				}
				else
				{
					double dPosLimitF = g_pNV->DDm(spindle3ZLimitPos) * 1000.0;
					double dPosLimitR = g_pNV->DDm(spindle4ZLimitPos) * 1000.0;

					if(dPosLimitF < dPosF)
					{
						g_err.Save(ER_SPINDLE_03_Z_ROUTER_POS_LIMIT);
						break;
					}
					if(dPosLimitR < dPosR)
					{
						g_err.Save(ER_SPINDLE_04_Z_ROUTER_POS_LIMIT);
						break;
					}
				}

				m_fsm.Set(C_ROUTER_START, INDEX_F);
			}
		}
		else if(INDEX_R == m_nCurIndex)
		{
			if (ROUTER_PART_F == m_nId && g_routerR.m_nCurIndex == INDEX_F)
			{
				if (g_routerR.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
					g_routerR.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
					g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
					g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
					g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
					g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
					g_routerR.m_fsm.Between(C_LDC_START, C_LDC_END) ||
					g_routerR.m_fsm.Between(C_PRS_START, C_PRS_END) ||
					g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
				{
					break;
				}
			}

			if(m_pIndexR->IsReadyLoadCheck())
			{
				if(g_pNV->UseSkip(usLoadCheck))
				{
					POINT2D ptPos = m_pIndexR->GetRouterPrsPos(0);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

					if(!IsGentryMtYWPRdy(PY_VI_PRS_R, PW_READY, ptPos.dY, dPosW))
					{
						GentryMtYWPMove(PY_VI_PRS_R, PW_READY, ptPos.dY, dPosW);
						break;
					}
					
					m_fsm.Set(C_LDC_START, INDEX_R);
				}
			}
			else if(m_pIndexR->IsReadyRouterPrs())
			{
				_strcat(cYMoveEventId, L"PRS");
				_strcat(cWMoveEventIdX, L"PRS");
				if(g_pNV->UseSkip(usRouterPrs))
				{
					POINT2D ptPos = m_pIndexR->GetRouterPrsPos(0);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

					_sprintf(cYPos, L"%03f", ptPos.dY);
					_sprintf(cWPos, L"%03f", dPosW);

					if(!IsGentryMtYWPRdy(PY_VI_PRS_R, PW_READY, ptPos.dY, dPosW))
					{
						GentryMtYWPMove(PY_VI_PRS_R, PW_READY, ptPos.dY, dPosW);
						if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_R]) 
						{
							g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_R] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_POSIDX_], cYPos, 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_SPD_], 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_ACC_]))
						}
						if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
						{
							g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventIdX, g_data2c.cEtc.start, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
								g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], cWPos, 
								g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
								g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
						}
						break;
					}
					else
					{
						if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_R]) 
						{
							g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_R] = FALSE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_POSIDX_], cYPos, 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_SPD_], 
								g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_ACC_]))
						}
						if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
						{
							g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = FALSE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventIdX, g_data2c.cEtc.end, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
								g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], cWPos, 
								g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
								g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
						}
					}
					m_fsm.Set(C_PRS_START, INDEX_R);
				}
			}
			else if(m_pIndexR->IsReadyLiveVision())
			{
				XYT xytPos = m_pIndexR->GetRouterLiveViPos(0, POS_START);
				double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

				if(!IsGentryMtYWPRdy(PY_VI_LIVE_R, PW_READY, xytPos.dY, dPosW))
				{
					GentryMtYWPMove(PY_VI_LIVE_R, PW_READY, xytPos.dY, dPosW);
					break;
				}
				
				m_fsm.Set(C_LIVE_VI_START, INDEX_R);
			}
			else if(m_pIndexR->IsReadyRouter())
			{
				_strcat(cYMoveEventId, L"CUT_02_START");
				_strcat(cWMoveEventIdX, L"CUT_02_START");
				XYT xytPos = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
				POINT2D ptMainOffset = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
				POINT2D ptSubOffset = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
				double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
				double dOffset = GetBitYOffset();
				double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

				_sprintf(cYPos, L"%03f", xytPos.dY);
				_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);

				if(!IsGentryMtYWPRdy(PY_ROUTER_R, PW_READY, xytPos.dY, dPosW + dOffset + dSubOffset))
				{
					GentryMtYWPMove(PY_ROUTER_R, PW_READY, xytPos.dY, dPosW + dOffset + dSubOffset);
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_R]) 
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_R] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventIdX, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
					}
					break;
				}
				else
				{
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_R]) 
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_R] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventIdX, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
					}
				}

				double dPosF = GetZStepPos(ROUTER_F);
				double dPosR = GetZStepPos(ROUTER_R);

				if(ROUTER_PART_F == m_nId)
				{
					double dPosLimitF = g_pNV->DDm(spindle1ZLimitPos) * 1000.0;
					double dPosLimitR = g_pNV->DDm(spindle2ZLimitPos) * 1000.0;

					if(dPosLimitF <= dPosF)
					{
						g_err.Save(ER_SPINDLE_01_Z_ROUTER_POS_LIMIT);
						break;
					}
					if(dPosLimitR <= dPosR)
					{
						g_err.Save(ER_SPINDLE_02_Z_ROUTER_POS_LIMIT);
						break;
					}
				}
				else
				{
					double dPosLimitF = g_pNV->DDm(spindle3ZLimitPos) * 1000.0;
					double dPosLimitR = g_pNV->DDm(spindle4ZLimitPos) * 1000.0;

					if(dPosLimitF <= dPosF)
					{
						g_err.Save(ER_SPINDLE_03_Z_ROUTER_POS_LIMIT);
						break;
					}
					if(dPosLimitR <= dPosR)
					{
						g_err.Save(ER_SPINDLE_04_Z_ROUTER_POS_LIMIT);
						break;
					}
				}

				m_fsm.Set(C_ROUTER_START, INDEX_R);
			}
		}
		else if(INDEX_F == m_nNextIndex)
		{
			POINT2D ptPos = m_pIndexF->GetRouterPrsPos(0);
			double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

			if (!IsGentryMtYWRdy(PY_READY, PW_READY))
			{
				GentryMtYWMove(PY_READY, PW_READY);
			}
		}
		else if(INDEX_R == m_nNextIndex)
		{
			POINT2D ptPos = m_pIndexR->GetRouterPrsPos(0);
			double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

			if (!IsGentryMtYWRdy(PY_READY, PW_READY))
			{
				GentryMtYWMove(PY_READY, PW_READY);
			}
		}
		break;
	case S_SPD_BIT_EJECT:
	{
		// Bit Change Part 는 Red bit 1번 4번 Index에 붙어 있음
		// Bit Change Part 는 Blue bit 2번 3번 Index에 붙어 있음 (추가)
		if (ROUTER_F == m_nBitChangeIdx)
		{
			if (ROUTER_PART_F == m_nId)
			{
				//@ 1. PCB가 있고
				//@ 2. 컷팅은 완료된 상태
				//@ 3. OUT PNP WAIT 포지션 또는 OUT PNP에 위치해 있으면 진행 불가

				//@ 1. PCB가 있고
				//@ 2. 컷팅은 완료된 상태라면 진행 불가로 하면.. 계속 상태값은 S_SPD_BIT_EJECT 상태임
				//@    에초에 BIT EJECT 상태를 타면 안될 듯...

				//@ PCB가 있고 컷팅이 완료된 상태가 아니라면 EJECT 진행해야하는데
				//@ 정면은 어떻게 진행 된거지?? 정면은 우선적으로 PCB가 빠져서 진행된건가???
				//@ compRouterRun / compRouterRun / compRouterRun
				BOOL bIndexNotRdy = m_pIndexF->ExistPcb();
				bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexF->SetIndexPosReq(REQ_BIT_EJECT_BOX);

				if (!IsGentryMtYWRdy(PY_SPD_BIT_EJECT_01_03_RED_F, PW_READY))
				{
					GentryMtYWMove(PY_SPD_BIT_EJECT_01_03_RED_F, PW_READY);
					break;
				}

				if (!g_dIn.AOn(iIndexBitEjectBoxExist01) && !g_opr.isDryRun)
				{
					g_err.Save(ER_INDEX01_BIT_EJECT_BOX_NOT_EXIST);
					break;
				}

				if (!g_pNV->UseSkip(usIndex01))
				{
					g_err.Save(ER_INDEX01_NOT_IN_USE);
					break;
				}

				if (m_pIndexF->IsReadySpdBitEject())
					m_fsm.Set(C_SPD_BIT_EJECT_START, ROUTER_F);
			}
			else
			{
				BOOL bIndexNotRdy = m_pIndexR->ExistPcb();
				bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexR->SetIndexPosReq(REQ_BIT_EJECT_BOX);

				if (!IsGentryMtYWRdy(PY_SPD_BIT_EJECT_01_03_RED_F, PW_READY))
				{
					GentryMtYWMove(PY_SPD_BIT_EJECT_01_03_RED_F, PW_READY);
					break;
				}

				if (!g_dIn.AOn(iIndexBitEjectBoxExist04) && !g_opr.isDryRun)
				{
					g_err.Save(ER_INDEX04_BIT_EJECT_BOX_NOT_EXIST);
					break;
				}

				if (!g_pNV->UseSkip(usIndex04))
				{
					g_err.Save(ER_INDEX04_NOT_IN_USE);
					break;
				}

				if (m_pIndexR->IsReadySpdBitEject())
					m_fsm.Set(C_SPD_BIT_EJECT_START, ROUTER_F);
			}
		}
		else if (ROUTER_R == m_nBitChangeIdx)
		{
			if (ROUTER_PART_F == m_nId)
			{
				if (g_routerR.m_nCurIndex == INDEX_F && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					if (g_routerR.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
						g_routerR.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
						g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
						g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
						g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
						g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
						g_routerR.m_fsm.Between(C_LDC_START, C_LDC_END) ||
						g_routerR.m_fsm.Between(C_PRS_START, C_PRS_END) ||
						g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
					{
						break;
					}
				}

				if (g_routerR.m_nCurIndex == INDEX_F && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					break;
				}

				bool bPosMoveStatus = false;
				bPosMoveStatus = g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_BLUE);
				bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_F);
				bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_R);
				bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_BLUE_03);
				bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_BLUE_03_R);
				bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_COLOR_R);
				if (g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
					g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
					g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
					g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
					g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
				{
					bPosMoveStatus = true;
				}

				if (g_routerF.m_nBitChangeIdx == ROUTER_R && g_routerR.m_bReqRouterBitReadyPos == FALSE && !bPosMoveStatus)
				{
					g_routerF.m_bReqRouterBitReadyPos = TRUE;
				}

				if (g_routerF.m_bReqRouterBitReadyPos == FALSE && g_routerF.m_nBitChangeIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					break;
				}

				if (C_ERROR == g_routerR.m_fsm.Get())
				{
					break;
				}

				BOOL bIndexNotRdy = false;

				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					bIndexNotRdy = m_pIndexR->ExistPcb();
					bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
					bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
						m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
					if (bIndexNotRdy)
					{
						m_pIndexF->SetIndexPosReq(INDEX_IDLE);
						m_pIndexR->SetIndexPosReq(INDEX_IDLE);
						break;
					}

					m_pIndexR->SetIndexPosReq(REQ_BIT_EJECT_BOX); 
				}
				else
				{
					bIndexNotRdy = m_pIndexF->ExistPcb();
					bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
					bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
						m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
					if (bIndexNotRdy)
					{
						m_pIndexF->SetIndexPosReq(INDEX_IDLE);
						m_pIndexR->SetIndexPosReq(INDEX_IDLE);
						break;
					}

					m_pIndexF->SetIndexPosReq(REQ_BIT_EJECT_BOX);
				}

				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					if (!IsGentryMtYWRdy(PY_SPD_BIT_EJECT_BLUE_02, PW_READY))
					{
						GentryMtYWMove(PY_SPD_BIT_EJECT_BLUE_02, PW_READY);
						break;
					}

					if (!g_dIn.AOn(iIndexBitEjectBoxExist02) && !g_opr.isDryRun)
					{
						g_err.Save(ER_INDEX02_BIT_EJECT_BOX_NOT_EXIST);
						break;
					}

					if (!g_pNV->UseSkip(usIndex02))
					{
						g_err.Save(ER_INDEX02_NOT_IN_USE);
						break;
					}
				}
				else
				{
					if (!IsGentryMtYWRdy(PY_SPD_BIT_EJECT_02_04_RED_R, PW_READY))
					{
						GentryMtYWMove(PY_SPD_BIT_EJECT_02_04_RED_R, PW_READY);
						break;
					}

					if (!g_dIn.AOn(iIndexBitEjectBoxExist01) && !g_opr.isDryRun)
					{
						g_err.Save(ER_INDEX01_BIT_EJECT_BOX_NOT_EXIST);
						break;
					}

					if (!g_pNV->UseSkip(usIndex01))
					{
						g_err.Save(ER_INDEX01_NOT_IN_USE);
						break;
					}
				}

				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					if (m_pIndexR->IsReadySpdBitEject())
						m_fsm.Set(C_SPD_BIT_EJECT_START, ROUTER_R);
				}
				else
				{
					if (m_pIndexF->IsReadySpdBitEject())
						m_fsm.Set(C_SPD_BIT_EJECT_START, ROUTER_R);
				}
			}
			else
			{
				if (g_routerF.m_nCurIndex == INDEX_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					if (g_routerF.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
						g_routerF.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
						g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
						g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
						g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
						g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
						g_routerF.m_fsm.Between(C_LDC_START, C_LDC_END) ||
						g_routerF.m_fsm.Between(C_PRS_START, C_PRS_END) ||
						g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
					{
						break;
					}
				}
				if (g_routerF.m_nCurIndex == INDEX_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					break;
				}

				bool bPosMoveStatus = false;
				bPosMoveStatus = g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_BLUE);
				bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_F);
				bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_R);
				bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_BLUE_02);
				bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_BLUE_02_R);
				bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_COLOR_F);
				if (g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
					g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
					g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
					g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
					g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
				{
					bPosMoveStatus = true;
				}

				if (g_routerR.m_nBitChangeIdx == ROUTER_R && g_routerF.m_bReqRouterBitReadyPos == FALSE && !bPosMoveStatus)
				{
					g_routerR.m_bReqRouterBitReadyPos = TRUE;
				}

				if (g_routerR.m_bReqRouterBitReadyPos == FALSE && g_routerR.m_nBitChangeIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					break;
				}

				if (C_ERROR == g_routerF.m_fsm.Get())
				{
					break;
				}

				BOOL bIndexNotRdy = false;

				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					bIndexNotRdy = m_pIndexF->ExistPcb();
					bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
					bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
						m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
					if (bIndexNotRdy)
					{
						m_pIndexF->SetIndexPosReq(INDEX_IDLE);
						m_pIndexR->SetIndexPosReq(INDEX_IDLE);
						break;
					}

					m_pIndexF->SetIndexPosReq(REQ_BIT_EJECT_BOX);
				}
				else
				{
					bIndexNotRdy = m_pIndexR->ExistPcb();
					bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
					bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
						m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
					if (bIndexNotRdy)
					{
						m_pIndexF->SetIndexPosReq(INDEX_IDLE);
						m_pIndexR->SetIndexPosReq(INDEX_IDLE);
						break;
					}

					m_pIndexR->SetIndexPosReq(REQ_BIT_EJECT_BOX);
				}

				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					if (!IsGentryMtYWRdy(PY_SPD_BIT_EJECT_BLUE_03, PW_READY))
					{
						GentryMtYWMove(PY_SPD_BIT_EJECT_BLUE_03, PW_READY);
						break;
					}

					if (!g_dIn.AOn(iIndexBitEjectBoxExist03) && !g_opr.isDryRun)
					{
						g_err.Save(ER_INDEX03_BIT_EJECT_BOX_NOT_EXIST);
						break;
					}

					if (!g_pNV->UseSkip(usIndex03))
					{
						g_err.Save(ER_INDEX03_NOT_IN_USE);
						break;
					}
				}
				else
				{
					if (!IsGentryMtYWRdy(PY_SPD_BIT_EJECT_02_04_RED_R, PW_READY))
					{
						GentryMtYWMove(PY_SPD_BIT_EJECT_02_04_RED_R, PW_READY);
						break;
					}

					if (!g_dIn.AOn(iIndexBitEjectBoxExist04) && !g_opr.isDryRun)
					{
						g_err.Save(ER_INDEX04_BIT_EJECT_BOX_NOT_EXIST);
						break;
					}

					if (!g_pNV->UseSkip(usIndex04))
					{
						g_err.Save(ER_INDEX04_NOT_IN_USE);
						break;
					}
				}

				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					if (m_pIndexF->IsReadySpdBitEject())
						m_fsm.Set(C_SPD_BIT_EJECT_START, ROUTER_R);
				}
				else
				{
					if (m_pIndexR->IsReadySpdBitEject())
						m_fsm.Set(C_SPD_BIT_EJECT_START, ROUTER_R);
				}
			}
		}
	}
		break;
	case S_SPD_BIT_CLAMP_F:
		if (ROUTER_PART_F == m_nId)
		{
			BOOL bIndexNotRdy = m_pIndexF->ExistPcb();
			bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
			bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
				m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
			if (bIndexNotRdy)
			{
				m_pIndexF->SetIndexPosReq(INDEX_IDLE);
				m_pIndexR->SetIndexPosReq(INDEX_IDLE);
				break;
			}

			m_pIndexF->SetIndexPosReq(REQ_BIT_SPINDLE_CLAMP_F);

			if (!IsGentryMtYWRdy(PY_SPD_BIT_CLAMP_01_03_RED_F, PW_READY))
			{
				GentryMtYWMove(PY_SPD_BIT_CLAMP_01_03_RED_F, PW_READY);
				break;
			}

			if (m_pIndexF->IsReadySpdBitClampF())
				m_fsm.Set(C_SPD_BIT_CLAMP_START, ROUTER_F);
		}
		else
		{
			BOOL bIndexNotRdy = m_pIndexR->ExistPcb();
			bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
			bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
				m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
			if (bIndexNotRdy)
			{
				m_pIndexF->SetIndexPosReq(INDEX_IDLE);
				m_pIndexR->SetIndexPosReq(INDEX_IDLE);
				break;
			}

			m_pIndexR->SetIndexPosReq(REQ_BIT_SPINDLE_CLAMP_F);

			if (!IsGentryMtYWRdy(PY_SPD_BIT_CLAMP_01_03_RED_F, PW_READY))
			{
				GentryMtYWMove(PY_SPD_BIT_CLAMP_01_03_RED_F, PW_READY);
				break;
			}

			if (m_pIndexR->IsReadySpdBitClampF())
				m_fsm.Set(C_SPD_BIT_CLAMP_START, ROUTER_F);
		}
		break;
	case S_SPD_BIT_CLAMP_R:
	{
		if (ROUTER_PART_F == m_nId)
		{
			if (g_routerR.m_nCurIndex == INDEX_F && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (g_routerR.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
					g_routerR.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
					g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
					g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
					g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
					g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
					g_routerR.m_fsm.Between(C_LDC_START, C_LDC_END) ||
					g_routerR.m_fsm.Between(C_PRS_START, C_PRS_END) ||
					g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
				{
					break;
				}
			}

			//@ 여기서 움직이지 말았어야 하는데 움직였음..
			//@ 둘 다 막아버리면 둘 다 안움직일 수 있음.. 항상 우선순위가 있어야함
			//@ FRONT 우선 순위...
			//@ 1. 그래서 안전위치를 체크하기전에 2번 INDEX에서 BIT 체인지 중이였음
			//@ 2. 이외 두번째 라우터 위치를 체크하지 않은 거임
			//@ 3. 라우터 위치를 체크하게 되면 1번 라우터도 정지 상태이기 때문에 행업이 발생할 수 있음
			//@ 4. 그렇다면 REAR ROUTER 가 레디상태로 대기하고 있었어야함..
			//@ 5. REAR 라우터는 지속적으로 BREAK 를 타고 있었음.. 여기서 대기 위치로 가지 못하는 상태임!!!!!
			//     !!!!!!! 둘 중 동시 BIT CHANGE 중이라면 한쪽은 무조건 READY 상태로 가야함..
			//     !!!!!!! 역으로 리어쪽 먼저 체인지 하고 정면 쪽 체인지 할 때도 동일하게 수정해야함..
			//	   !!!!!!! EXIST 가 있고 라우터가 완료된 상태라면 EJECT를 안했으면 되었나???
			//             그렇다면 PCB를 배출하고 EJECT 및 진행 처리??
			//             IN PNP 투입 순이 우선 순위 인가????
			//     !!!!!!! 앞 뒤 서로 FLOW 상태가 지속적으로 일치해야함..
			//     ******* 5번 항목 ***********************
			//     서로 멈춰 있고 PCB가 없는 상태에서 동시에 BIT CHANGE를 하면은 문제는 없지만 위 상태가 문제임...
			//@ m_pIndexF->SetIndexPosReq(INDEX_IDLE); INDEX_IDLE(-1)
			//@ m_pIndexR->SetIndexPosReq(INDEX_IDLE);

			if (g_routerR.m_nCurIndex == INDEX_F && g_routerF.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			bool bPosMoveStatus = false;
			bPosMoveStatus = g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_BLUE);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_F);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_R);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_BLUE_03);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_BLUE_03_R);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_COLOR_R);

			if (g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
				g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
				g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
				g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
				g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
			{
				bPosMoveStatus = true;
			}

			if (g_routerF.m_nBitCurIdx == ROUTER_R && g_routerR.m_bReqRouterBitReadyPos == FALSE && !bPosMoveStatus)
			{
				g_routerF.m_bReqRouterBitReadyPos = TRUE;
			}

			if (g_routerF.m_bReqRouterBitReadyPos == FALSE && g_routerF.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			if (C_ERROR == g_routerR.m_fsm.Get())
			{
				break;
			}

			BOOL bIndexNotRdy = false;
			int mtRednBlueY = PY_SPD_BIT_CLAMP_02_04_RED_R;
			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				mtRednBlueY = PY_SPD_BIT_CLAMP_BLUE_02_R;

				bIndexNotRdy = m_pIndexR->ExistPcb();
				bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexR->SetIndexPosReq(REQ_BIT_SPINDLE_CLAMP_R);
			}
			else
			{
				bIndexNotRdy = m_pIndexF->ExistPcb();
				bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexF->SetIndexPosReq(REQ_BIT_SPINDLE_CLAMP_R);
			}

			if (!IsGentryMtYWRdy(mtRednBlueY, PW_READY))
			{
				GentryMtYWMove(mtRednBlueY, PW_READY);
				break;
			}

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (m_pIndexR->IsReadySpdBitClampR())
					m_fsm.Set(C_SPD_BIT_CLAMP_START, ROUTER_R);
			}
			else
			{
				if (m_pIndexF->IsReadySpdBitClampR())
					m_fsm.Set(C_SPD_BIT_CLAMP_START, ROUTER_R);
			}
		}
		else
		{
			if (g_routerF.m_nCurIndex == INDEX_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (g_routerF.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
					g_routerF.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
					g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
					g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
					g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
					g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
					g_routerF.m_fsm.Between(C_LDC_START, C_LDC_END) ||
					g_routerF.m_fsm.Between(C_PRS_START, C_PRS_END) ||
					g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
				{
					break;
				}
			}

			if (g_routerF.m_nCurIndex == INDEX_R && g_routerR.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			bool bPosMoveStatus = false;
			bPosMoveStatus = g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_BLUE);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_F);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_R);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_BLUE_02);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_BLUE_02_R);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_COLOR_F);

			if (g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
				g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
				g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
				g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
				g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
			{
				bPosMoveStatus = true;
			}

			if (g_routerR.m_nBitCurIdx == ROUTER_R && g_routerF.m_bReqRouterBitReadyPos == FALSE && !bPosMoveStatus)
			{
				g_routerR.m_bReqRouterBitReadyPos = TRUE;
			}

			if (g_routerR.m_bReqRouterBitReadyPos == FALSE && g_routerR.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			if (C_ERROR == g_routerF.m_fsm.Get())
			{
				break;
			}

			BOOL bIndexNotRdy = false;
			int mtRednBlueY = PY_SPD_BIT_CLAMP_02_04_RED_R;
			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				mtRednBlueY = PY_SPD_BIT_CLAMP_BLUE_03_R;

				bIndexNotRdy = m_pIndexF->ExistPcb();
				bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexF->SetIndexPosReq(REQ_BIT_SPINDLE_CLAMP_R);
			}
			else
			{
				bIndexNotRdy = m_pIndexR->ExistPcb();
				bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexR->SetIndexPosReq(REQ_BIT_SPINDLE_CLAMP_R);
			}

			if (!IsGentryMtYWRdy(mtRednBlueY, PW_READY))
			{
				GentryMtYWMove(mtRednBlueY, PW_READY);
				break;
			}

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (m_pIndexF->IsReadySpdBitClampR())
					m_fsm.Set(C_SPD_BIT_CLAMP_START, ROUTER_R);
			}
			else
			{
				if (m_pIndexR->IsReadySpdBitClampR())
					m_fsm.Set(C_SPD_BIT_CLAMP_START, ROUTER_R);
			}
		}
	}
		break;
	case S_SPD_WIRE_CHECK:
		m_pIndexF->SetIndexPosReq(INDEX_IDLE);
		m_pIndexR->SetIndexPosReq(INDEX_IDLE);

		m_bReqReadyPos = TRUE;

		if(ROUTER_PART_F == m_nId)
		{
			if(!g_routerR.IsReadyReadyPos())
				break;
		}
		else // Rear Part
		{
			if(!g_routerF.IsReadyReadyPos())
				break;
		}
		if(ROUTER_F == m_nBitWireIdx)
		{
			if(!IsGentryMtYWRdy(PY_SPD_WIRE_CHECK_F, PW_READY))
			{
				GentryMtYWMove(PY_SPD_WIRE_CHECK_F, PW_READY);
				break;
			}
			m_fsm.Set(C_SPD_ESD_CHECK_START, ROUTER_F);
		}
		else if(ROUTER_R == m_nBitWireIdx)
		{
			if(!IsGentryMtYWRdy(PY_SPD_WIRE_CHECK_R, PW_READY))
			{
				GentryMtYWMove(PY_SPD_WIRE_CHECK_R, PW_READY);
				break;
			}
			m_fsm.Set(C_SPD_ESD_CHECK_START, ROUTER_R);
		}
		break;

	case S_CYL_BIT_CLAMP:
	{
		if (ROUTER_PART_F == m_nId)
		{
			if (g_routerR.m_nCurIndex == INDEX_F && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (g_routerR.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
					g_routerR.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
					g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
					g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
					g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
					g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
					g_routerR.m_fsm.Between(C_LDC_START, C_LDC_END) ||
					g_routerR.m_fsm.Between(C_PRS_START, C_PRS_END) ||
					g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
				{
					break;
				}
			}

			if (g_routerR.m_nCurIndex == INDEX_F && g_routerF.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			bool bPosMoveStatus = false;
			bPosMoveStatus = g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_BLUE);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_F);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_R);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_BLUE_03);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_BLUE_03_R);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_COLOR_R);
			if (g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
				g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
				g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
				g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
				g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
			{
				bPosMoveStatus = true;
			}

			if (g_routerF.m_nBitCurIdx == ROUTER_R && g_routerR.m_bReqRouterBitReadyPos == FALSE && !bPosMoveStatus)
			{
				g_routerF.m_bReqRouterBitReadyPos = TRUE;
			}

			if (g_routerF.m_bReqRouterBitReadyPos == FALSE && g_routerF.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			if (C_ERROR == g_routerR.m_fsm.Get())
			{
				break;
			}

			int iSupplyBoxExist = iIndexBitSupplyBoxExist01;
			int ErrSupply = ER_INDEX01_BIT_SUPPLY_BOX_NOT_EXIST;
			int BitCnt = RedindexBitBoxCurCnt01;
			int ErrBoxEmpty = ER_INDEX01_BIT_BOX_EMPTY;
			int mtRednBlueY = PY_CYL_BIT_CLAMP_RED;

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				iSupplyBoxExist = iIndexBitSupplyBoxExist02;
				ErrSupply = ER_INDEX02_BIT_SUPPLY_BOX_NOT_EXIST;
				BitCnt = BlueindexBitBoxCurCnt02;
				ErrBoxEmpty = ER_INDEX02_BIT_BOX_EMPTY;
				mtRednBlueY = PY_CYL_BIT_CLAMP_BLUE;
			}

			if (!g_dIn.AOn(iSupplyBoxExist) && !g_opr.isDryRun)
			{
				g_err.Save(ErrSupply);
				break;
			}

			BOOL bIndexNotRdy = false;
			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				bIndexNotRdy = m_pIndexR->ExistPcb();
				bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexR->SetIndexPosReq(REQ_BIT_SUPPLY_BOX);
			}
			else
			{
				bIndexNotRdy = m_pIndexF->ExistPcb();
				bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexF->SetIndexPosReq(REQ_BIT_SUPPLY_BOX);
			}

			int nBitCurCnt = g_pNV->NDm(BitCnt);

			if (nBitCurCnt < 0)
			{
				g_err.Save(ErrBoxEmpty);
				break;
			}

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				POINT2D ptXY = m_pIndexR->GetBitBoxSupplyPos(nBitCurCnt);
				double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

				if (!IsGentryMtYWPRdy(mtRednBlueY, PW_READY, ptXY.dY, dPosW))
				{
					GentryMtYWPMove(mtRednBlueY, PW_READY, ptXY.dY, dPosW);
					break;
				}
			}
			else
			{
				POINT2D ptXY = m_pIndexF->GetBitBoxSupplyPos(nBitCurCnt);
				double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

				if (!IsGentryMtYWPRdy(mtRednBlueY, PW_READY, ptXY.dY, dPosW))
				{
					GentryMtYWPMove(mtRednBlueY, PW_READY, ptXY.dY, dPosW);
					break;
				}
			}

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (m_pIndexR->IsReadyBitClamp())
				{
					m_fsm.Set(C_CYL_BIT_CLAMP_START, m_nBitCurIdx);
				}
			}
			else
			{
				if (m_pIndexF->IsReadyBitClamp())
				{
					m_fsm.Set(C_CYL_BIT_CLAMP_START, m_nBitCurIdx);
				}
			}
		}
		else
		{
			if (g_routerF.m_nCurIndex == INDEX_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (g_routerF.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
					g_routerF.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
					g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
					g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
					g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
					g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
					g_routerF.m_fsm.Between(C_LDC_START, C_LDC_END) ||
					g_routerF.m_fsm.Between(C_PRS_START, C_PRS_END) ||
					g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
				{
					break;
				}
			}

			if (g_routerF.m_nCurIndex == INDEX_R && g_routerR.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			bool bPosMoveStatus = false;
			bPosMoveStatus = g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_BLUE);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_F);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_R);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_BLUE_02);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_BLUE_02_R);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_COLOR_F);
			if (g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
				g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
				g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
				g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
				g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
			{
				bPosMoveStatus = true;
			}

			if (g_routerR.m_nBitCurIdx == ROUTER_R && g_routerF.m_bReqRouterBitReadyPos == FALSE && !bPosMoveStatus)
			{
				g_routerR.m_bReqRouterBitReadyPos = TRUE;
			}

			if (g_routerR.m_bReqRouterBitReadyPos == FALSE && g_routerR.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			if (C_ERROR == g_routerF.m_fsm.Get())
			{
				break;
			}

			int iSupplyBoxExist = iIndexBitSupplyBoxExist04;
			int ErrSupply = ER_INDEX04_BIT_SUPPLY_BOX_NOT_EXIST;
			int BitCnt = RedindexBitBoxCurCnt04;
			int ErrBoxEmpty = ER_INDEX04_BIT_BOX_EMPTY;
			int mtRednBlueY = PY_CYL_BIT_CLAMP_RED;

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				iSupplyBoxExist = iIndexBitSupplyBoxExist03;
				ErrSupply = ER_INDEX03_BIT_SUPPLY_BOX_NOT_EXIST;
				BitCnt = BlueindexBitBoxCurCnt03;
				ErrBoxEmpty = ER_INDEX03_BIT_BOX_EMPTY;
				mtRednBlueY = PY_CYL_BIT_CLAMP_BLUE;
			}

			if (!g_dIn.AOn(iSupplyBoxExist) && !g_opr.isDryRun)
			{
				g_err.Save(ErrSupply);
				break;
			}

			BOOL bIndexNotRdy = false;
			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				bIndexNotRdy = m_pIndexF->ExistPcb();
				bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexF->SetIndexPosReq(REQ_BIT_SUPPLY_BOX);
			}
			else
			{
				bIndexNotRdy = m_pIndexR->ExistPcb();
				bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexR->SetIndexPosReq(REQ_BIT_SUPPLY_BOX);
			}


			int nBitCurCnt = g_pNV->NDm(BitCnt);
			if (nBitCurCnt < 0)
			{
				g_err.Save(ErrBoxEmpty);
				break;
			}

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				POINT2D ptXY = m_pIndexF->GetBitBoxSupplyPos(nBitCurCnt);
				double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

				if (!IsGentryMtYWPRdy(mtRednBlueY, PW_READY, ptXY.dY, dPosW))
				{
					GentryMtYWPMove(mtRednBlueY, PW_READY, ptXY.dY, dPosW);
					break;
				}
			}
			else
			{
				POINT2D ptXY = m_pIndexR->GetBitBoxSupplyPos(nBitCurCnt);
				double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];

				if (!IsGentryMtYWPRdy(mtRednBlueY, PW_READY, ptXY.dY, dPosW))
				{
					GentryMtYWPMove(mtRednBlueY, PW_READY, ptXY.dY, dPosW);
					break;
				}
			}

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (m_pIndexF->IsReadyBitClamp())
					m_fsm.Set(C_CYL_BIT_CLAMP_START, m_nBitCurIdx);
			}
			else
			{
				if (m_pIndexR->IsReadyBitClamp())
					m_fsm.Set(C_CYL_BIT_CLAMP_START, m_nBitCurIdx);
			}
		}
	}
	break;
	case S_CYL_BIT_ALIGN_F:
		if(ROUTER_PART_F == m_nId)
		{
			BOOL bIndexNotRdy  = m_pIndexF->ExistPcb();
			bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
			bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) || 
				m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
			if(bIndexNotRdy)
			{
				m_pIndexF->SetIndexPosReq(INDEX_IDLE);
				m_pIndexR->SetIndexPosReq(INDEX_IDLE);
				break;
			}

			m_pIndexF->SetIndexPosReq(REQ_BIT_ALIGN_F);

			if(!IsGentryMtYWRdy(PY_CYL_BIT_ALIGN_RED_F, PW_READY))
			{
				GentryMtYWMove(PY_CYL_BIT_ALIGN_RED_F, PW_READY);
				break;
			}

			if(m_pIndexF->IsReadyBitAlignF())
				m_fsm.Set(C_CYL_BIT_ALIGN_START, ROUTER_F);
		}
		else
		{
			BOOL bIndexNotRdy  = m_pIndexR->ExistPcb();
			bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
			bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) || 
				m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
			if(bIndexNotRdy)
			{
				m_pIndexF->SetIndexPosReq(INDEX_IDLE);
				m_pIndexR->SetIndexPosReq(INDEX_IDLE);
				break;
			}

			m_pIndexR->SetIndexPosReq(REQ_BIT_ALIGN_F);

			if(!IsGentryMtYWRdy(PY_CYL_BIT_ALIGN_RED_F, PW_READY))
			{
				GentryMtYWMove(PY_CYL_BIT_ALIGN_RED_F, PW_READY);
				break;
			}

			if(m_pIndexR->IsReadyBitAlignF())
				m_fsm.Set(C_CYL_BIT_ALIGN_START, ROUTER_F);
		}
		break;
	case S_CYL_BIT_ALIGN_R:
	{
		if (ROUTER_PART_F == m_nId)
		{
			if (g_routerR.m_nCurIndex == INDEX_F && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (g_routerR.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
					g_routerR.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
					g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
					g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
					g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
					g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
					g_routerR.m_fsm.Between(C_LDC_START, C_LDC_END) ||
					g_routerR.m_fsm.Between(C_PRS_START, C_PRS_END) ||
					g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
				{
					break;
				}
			}

			if (g_routerR.m_nCurIndex == INDEX_F && g_routerF.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			bool bPosMoveStatus = false;
			bPosMoveStatus = g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_BLUE);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_F);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_R);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_BLUE_03);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_BLUE_03_R);
			bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_COLOR_R);
			if (g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
				g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
				g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
				g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
				g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
			{
				bPosMoveStatus = true;
			}

			if (g_routerF.m_nBitCurIdx == ROUTER_R && g_routerR.m_bReqRouterBitReadyPos == FALSE && !bPosMoveStatus)
			{
				g_routerF.m_bReqRouterBitReadyPos = TRUE;
			}

			if (g_routerF.m_bReqRouterBitReadyPos == FALSE && g_routerF.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			if (C_ERROR == g_routerR.m_fsm.Get())
			{
				break;
			}

			BOOL bIndexNotRdy = false;
			int mtRednBlueAlignY = PY_CYL_BIT_ALIGN_RED_R;

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				mtRednBlueAlignY = PY_CYL_BIT_ALIGN_BLUE_R;

				bIndexNotRdy = m_pIndexR->ExistPcb();
				bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexR->SetIndexPosReq(REQ_BIT_ALIGN_R);
			}
			else
			{
				bIndexNotRdy = m_pIndexF->ExistPcb();
				bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexF->SetIndexPosReq(REQ_BIT_ALIGN_R);
			}

			if (!IsGentryMtYWRdy(mtRednBlueAlignY, PW_READY))
			{
				GentryMtYWMove(mtRednBlueAlignY, PW_READY);
				break;
			}

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (m_pIndexR->IsReadyBitAlignR())
					m_fsm.Set(C_CYL_BIT_ALIGN_START, ROUTER_R);
			}
			else
			{
				if (m_pIndexF->IsReadyBitAlignR())
					m_fsm.Set(C_CYL_BIT_ALIGN_START, ROUTER_R);
			}
		}
		else
		{
			if (g_routerF.m_nCurIndex == INDEX_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (g_routerF.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
					g_routerF.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
					g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
					g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
					g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
					g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
					g_routerF.m_fsm.Between(C_LDC_START, C_LDC_END) ||
					g_routerF.m_fsm.Between(C_PRS_START, C_PRS_END) ||
					g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
				{
					break;
				}
			}

			if (g_routerF.m_nCurIndex == INDEX_R && g_routerR.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			bool bPosMoveStatus = false;
			bPosMoveStatus = g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_BLUE);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_F);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_R);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_BLUE_02);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_BLUE_02_R);
			bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_COLOR_F);

			if (g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
				g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
				g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
				g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
				g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
			{
				bPosMoveStatus = true;
			}

			if (g_routerR.m_nBitCurIdx == ROUTER_R && g_routerF.m_bReqRouterBitReadyPos == FALSE && !bPosMoveStatus)
			{
				g_routerR.m_bReqRouterBitReadyPos = TRUE;
			}

			if (g_routerR.m_bReqRouterBitReadyPos == FALSE && g_routerR.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				break;
			}

			if (C_ERROR == g_routerF.m_fsm.Get())
			{
				break;
			}

			BOOL bIndexNotRdy = false;
			int mtRednBlueAlignY = PY_CYL_BIT_ALIGN_RED_R;

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				mtRednBlueAlignY = PY_CYL_BIT_ALIGN_BLUE_R;

				bIndexNotRdy = m_pIndexF->ExistPcb();
				bIndexNotRdy &= m_pIndexF->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexF->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexF->SetIndexPosReq(REQ_BIT_ALIGN_R);
			}
			else
			{
				bIndexNotRdy = m_pIndexR->ExistPcb();
				bIndexNotRdy &= m_pIndexR->m_pMem->compRouterRun;
				bIndexNotRdy &= m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP_WAIT) ||
					m_pIndexR->m_pMtX->ComparePos(CIndex::PX_OUT_PNP);
				if (bIndexNotRdy)
				{
					m_pIndexF->SetIndexPosReq(INDEX_IDLE);
					m_pIndexR->SetIndexPosReq(INDEX_IDLE);
					break;
				}

				m_pIndexR->SetIndexPosReq(REQ_BIT_ALIGN_R);
			}

			if (!IsGentryMtYWRdy(mtRednBlueAlignY, PW_READY))
			{
				GentryMtYWMove(mtRednBlueAlignY, PW_READY);
				break;
			}

			if (m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				if (m_pIndexF->IsReadyBitAlignR())
					m_fsm.Set(C_CYL_BIT_ALIGN_START, ROUTER_R);
			}
			else
			{
				if (m_pIndexR->IsReadyBitAlignR())
					m_fsm.Set(C_CYL_BIT_ALIGN_START, ROUTER_R);
			}
		}
	}
	break;
	case S_SPD_BIT_COLOR:
			if (ROUTER_F == m_nBitCurIdx)
			{
				m_fsm.Set(C_CYL_COLOR_CLAMP_START, ROUTER_F); 

			}
			else
			{
				if (ROUTER_PART_F == m_nId)
				{
					if (g_routerR.m_nCurIndex == INDEX_F && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
					{
						if (g_routerR.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
							g_routerR.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
							g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
							g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
							g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
							g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
							g_routerR.m_fsm.Between(C_LDC_START, C_LDC_END) ||
							g_routerR.m_fsm.Between(C_PRS_START, C_PRS_END) ||
							g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
						{
							break;
						}
					}

					if (g_routerR.m_nCurIndex == INDEX_F && g_routerF.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
					{
						break;
					}

					bool bPosMoveStatus = false;
					bPosMoveStatus = g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_BLUE);
					bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_F);
					bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_R);
					bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_BLUE_03);
					bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_BLUE_03_R);
					bPosMoveStatus |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_COLOR_R);
					if (g_routerR.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
						g_routerR.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
						g_routerR.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
						g_routerR.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
						g_routerR.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
					{
						bPosMoveStatus = true;
					}

					if (g_routerF.m_nBitCurIdx == ROUTER_R && g_routerR.m_bReqRouterBitReadyPos == FALSE && !bPosMoveStatus)
					{
						g_routerF.m_bReqRouterBitReadyPos = TRUE;
					}

					if (g_routerF.m_bReqRouterBitReadyPos == FALSE && g_routerF.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
					{
						break;
					}

					if (C_ERROR == g_routerR.m_fsm.Get())
					{
						break;
					}
				}
				else
				{
					if (g_routerF.m_nCurIndex == INDEX_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
					{
						if (g_routerF.m_fsm.Between(C_ROUTER_START, C_ROUTER_END) ||
							g_routerF.m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END) ||
							g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
							g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
							g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
							g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
							g_routerF.m_fsm.Between(C_LDC_START, C_LDC_END) ||
							g_routerF.m_fsm.Between(C_PRS_START, C_PRS_END) ||
							g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
						{
							break;
						}
					}

					if (g_routerF.m_nCurIndex == INDEX_R && g_routerR.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
					{
						break;
					}

					bool bPosMoveStatus = false;
					bPosMoveStatus = g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_BLUE);
					bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_F);
					bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_BLUE_R);
					bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_BLUE_02);
					bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_BLUE_02_R);
					bPosMoveStatus |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_COLOR_F);

					if (g_routerF.m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END) ||
						g_routerF.m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END) ||
						g_routerF.m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END) ||
						g_routerF.m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END) ||
						g_routerF.m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
					{
						bPosMoveStatus = true;
					}

					if (g_routerR.m_nBitCurIdx == ROUTER_R && g_routerF.m_bReqRouterBitReadyPos == FALSE && !bPosMoveStatus)
					{
						g_routerR.m_bReqRouterBitReadyPos = TRUE;
					}

					if (g_routerR.m_bReqRouterBitReadyPos == FALSE && g_routerR.m_nBitCurIdx == ROUTER_R && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
					{
						break;
					}

					if (C_ERROR == g_routerF.m_fsm.Get())
					{
						break;
					}
				}

				m_fsm.Set(C_CYL_COLOR_CLAMP_START, ROUTER_R);
			}
		break;
	case S_SPD_BIT_VI:
		m_pIndexF->SetIndexPosReq(INDEX_IDLE);
		m_pIndexR->SetIndexPosReq(INDEX_IDLE);

		m_bReqReadyPos = TRUE;

		if(ROUTER_PART_F == m_nId)
		{
			if(!g_routerR.IsReadyReadyPos())
				break;
		}
		else
		{
			if(!g_routerF.IsReadyReadyPos())
				break;
		}
		if(ROUTER_F == m_nBitVisionIdx)
		{
			if(!IsGentryMtYWRdy(PY_SPD_BIT_VI_F, PW_READY))
			{
				GentryMtYWMove(PY_SPD_BIT_VI_F, PW_READY);
				break;
			}
			m_fsm.Set(C_SPD_BIT_VI_START, ROUTER_F);
		}
		else if(ROUTER_R == m_nBitVisionIdx)
		{
			if(!IsGentryMtYWRdy(PY_SPD_BIT_VI_R, PW_READY))
			{
				GentryMtYWMove(PY_SPD_BIT_VI_R, PW_READY);
				break;
			}
			m_fsm.Set(C_SPD_BIT_VI_START, ROUTER_R);
		}
		break;
	case S_SPD_BIT_BROKEN_CHECK:
		m_pIndexF->SetIndexPosReq(INDEX_IDLE);
		m_pIndexR->SetIndexPosReq(INDEX_IDLE);

		_sprintf(cMaterialId, L"$");
		_sprintf(cMaterialType, L"BIT");

		_strcat(cYMoveEventId, L"SPD_BIT_VERIFY_F");

		if(ROUTER_F == m_nBitBrokenIdx)
		{
			if(!IsGentryMtYWRdy(PY_SPD_BIT_VERIFY_F, PW_READY))
			{
				GentryMtYWMove(PY_SPD_BIT_VERIFY_F, PW_READY);
				if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_F]) 
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_F] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_POSIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_POS_],
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_SPD_], 
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_F])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_F] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_POSIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_POS_], 
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_SPD_], 
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_ACC_]))
				}
			}
			m_fsm.Set(C_SPD_BIT_VERIFY_START, ROUTER_F);
		}
		else if(ROUTER_R == m_nBitBrokenIdx)
		{
			if(!IsGentryMtYWRdy(PY_SPD_BIT_VERIFY_R, PW_READY))
			{
				GentryMtYWMove(PY_SPD_BIT_VERIFY_R, PW_READY);
				if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_R]) 
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_R] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_POSIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_POS_],
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_SPD_], 
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_R])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_R] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_POSIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_POS_], 
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_SPD_],
						g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_ACC_]))
				}
			}
			m_fsm.Set(C_SPD_BIT_VERIFY_START, ROUTER_R);
		}
		break;
	}
}


//-------------------------------------------------------------------
void CRouter::CycleRun(void)
{
	if(!m_fsm.IsRun())
		return;

	if(m_fsm.IsStop())
		return;

	if(C_ERROR == m_fsm.Get())
	{
		if(0 < m_fsm.GetMsg())
			g_err.Save(m_fsm.GetMsg());

		g_pNV->NDm(stateSpindleESDCheck) = STATE_IDLE;
		g_pNV->NDm(spindleESDNo) = 0;

		if(ROUTER_PART_F == m_nId)
		{
			g_dOut.Off(oViRouterPrsTrigF);
			g_dOut.Off(oViRouterPrsRstF);
		}
		else
		{
			g_dOut.Off(oViRouterPrsTrigR);
			g_dOut.Off(oViRouterPrsRstR);
		}

		g_dOut.Off(oViSpindleBtmTrig);
		g_dOut.Off(oViSpindleBtmRst);

		m_pSpindleF->Actuate(pmOFF);
		m_pSpindleR->Actuate(pmOFF);

		m_fsm.Set(C_IDLE);
		return;
	}
	else if(IsErr())
	{
		m_fsm.Set(C_ERROR);
		return;
	}

	CycleRunRouter();
	CycleRunLiveVision();
	CycleRunCylColorClamp();
	CycleRunSpdBitEject();
	CycleRunSpdBitClamp();
	CycleRunSpdESDCheck();
	CycleRunCylBitClamp();
	CycleRunCylBitAlign();
	CycleRunSpdBitVision();
	CycleRunSpdBitVerify();
	CycleRunLoadCheck();
	CycleRunPrs();
}


//-------------------------------------------------------------------
void CRouter::InitNomal(int nNo, CIndex* pIndexF, CIndex* pIndexR)
{
	m_nId		= nNo;
	m_pIndexF	= pIndexF;
	m_pIndexR	= pIndexR;
}


//-------------------------------------------------------------------
void CRouter::InitNv(BIT_INFO* pInfoBitF, BIT_INFO* pInfoBitR)
{
	m_pInfoBitF	= pInfoBitF;
	m_pInfoBitR	= pInfoBitR;
}


//-------------------------------------------------------------------
void CRouter::InitMt(CMtAXL* pMtY, CMtAXL* pMtW, CMtAXL* pMtZ_F, CMtAXL* pMtZ_R)
{
	m_pMtY	 = pMtY;
	m_pMtW	 = pMtW;
	m_pMtZ_F = pMtZ_F;
	m_pMtZ_R = pMtZ_R;
}


//-------------------------------------------------------------------
void CRouter::InitPm1(CPneumatic* pPmBitClampUD, CPneumatic* pPmBitClampOC)
{
	m_pCylBitClampUD	= pPmBitClampUD;
	m_pCylBitClampOC	= pPmBitClampOC;
}


//-------------------------------------------------------------------
void CRouter::InitPm3(CPneumatic* pPmSolSpinldeChuckOC_F, CPneumatic* pPmSolSpinldeChuckOC_R)
{
	m_pSolSpdChuckOC_F	= pPmSolSpinldeChuckOC_F;
	m_pSolSpdChuckOC_R	= pPmSolSpinldeChuckOC_R;
}


void CRouter::InitPm4(CPneumatic* pSolSpindleBlow)
{
	m_pSolSpindleBlow = pSolSpindleBlow;
}


void CRouter::InitPm5(CPneumatic* pSolRouterIonizerF, CPneumatic* pSolRouterIonizerR)
{
	m_pSolRouterIonizerF = pSolRouterIonizerF;
	m_pSolRouterIonizerR = pSolRouterIonizerR;
}


void CRouter::InitPm6(CPneumatic* pSpindleF, CPneumatic* pSpindleR)
{
	m_pSpindleF = pSpindleF;
	m_pSpindleR = pSpindleR; 
}

//-------------------------------------------------------------------
int& CRouter::IsPartUseSkip(void)
{
	return (g_pNV->m_pData->useSkip[(usRouterPartF + m_nId)]);
}


//-------------------------------------------------------------------
int& CRouter::IsIndexFUseSkip(void)
{
	return (g_pNV->m_pData->useSkip[(usIndex01 + (m_nId*2))]);
}


//-------------------------------------------------------------------
int& CRouter::IsIndexRUseSkip(void)
{
	return (g_pNV->m_pData->useSkip[(usIndex03 + (m_nId*2))]);
}


//-------------------------------------------------------------------
int& CRouter::ExistCylBitClamp(void)
{
	return (g_pNV->m_pData->ndm[(existRouterCylBitF + m_nId)]);
}


//-------------------------------------------------------------------
int& CRouter::ExistCylBitAlignRedF(void)
{
	return (g_pNV->m_pData->ndm[(existRedIndexBitAlign01 + (m_nId*2))]);
}


//-------------------------------------------------------------------
int& CRouter::ExistCylBitAlignRedR(void)
{
	return (g_pNV->m_pData->ndm[(existRedIndexBitAlign02 + (m_nId*2))]);
}

//-------------------------------------------------------------------
int& CRouter::ExistCylBitAlignBlueF(void)
{
	return (g_pNV->m_pData->ndm[(existBlueIndexBitAlign01 + (m_nId * 2))]);
}


//-------------------------------------------------------------------
int& CRouter::ExistCylBitAlignBlueR(void)
{
	return (g_pNV->m_pData->ndm[(existBlueIndexBitAlign02 + (m_nId * 2))]);
}


int& CRouter::flagSpindleESDCheckF(void)
{
	return (g_pNV->m_pData->ndm[(flagSpindleESDCheck01 + (m_nId*2))]);
}


int& CRouter::flagSpindleESDCheckR(void)
{
	return (g_pNV->m_pData->ndm[(flagSpindleESDCheck02 + (m_nId*2))]);
}


int& CRouter::flagSpidleBitChangeF(void)
{
	return (g_pNV->m_pData->ndm[(mmiSpindle01BitChange + (m_nId*2))]);
}


int& CRouter::flagSpidleBitChangeR(void)
{
	return (g_pNV->m_pData->ndm[(mmiSpindle02BitChange + (m_nId*2))]);
}


//-------------------------------------------------------------------
BOOL CRouter::IsReadyReadyPos(void)
{
	if(g_opr.isAuto)
	{
		return (m_bReadyPos);
	}
	else
	{
		BOOL isRdy  = !m_fsm.IsRun();
		isRdy &= IsMtRdy();
		isRdy &= m_pMtY->InPos(PY_READY);

		return (isRdy);
	}
}


//-------------------------------------------------------------------
BOOL CRouter::GentryMtYWMove(int nIndexY, int nIndexW)
{
	m_pMtY->Move(nIndexY);
	m_pMtW->Move(nIndexW);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRouter::GentryMtYWPMove(int nIndexY, int nIndexW, double dPosY, double dPosW, double dVel)
{
	// m_pMtW 상태를 보아야 함
	if(0 == (int)dVel)
		m_pMtY->PMove(nIndexY, dPosY);
	else
		m_pMtY->PMove(nIndexY, dPosY, dVel);

	m_pMtW->PMove(nIndexW, dPosW);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRouter::IsGentryMtYWRdy(int nIndexY, int nIndexW)
{
	if(!m_pMtY->IsRdy())
		return (FALSE);

	if(!m_pMtW->IsRdy())
		return (FALSE);

	if(!m_pMtY->InPos(nIndexY))
		return (FALSE);

	if(!m_pMtW->InPos(nIndexW))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRouter::IsGentryMtYWPRdy(int nIndexY, int nIndexW, double dPosY, double dPosW)
{
	if(!m_pMtY->IsRdy())
		return (FALSE);

	if(!m_pMtW->IsRdy())
		return (FALSE);

	if(!m_pMtY->InPos(nIndexY, dPosY, 5))
		return (FALSE);

	if(!m_pMtW->InPos(nIndexW, dPosW, 5))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
double CRouter::GetBitYOffset()
{
	// Router Front Part & Router Rear Part는 모두 기준이 앞단
	// 연산 기준 포지션도 앞단으로 함으로 Rear의 Motor를 뒤로 보낼 수 있도록한다.
	// 일단 rear Y의 Pos는 (-) 값을 줘야 함
	// 추후 확인 필요함.
	double dResult = 0.0;

	int nYLineMoveCnt = (int)g_pNV->gerberPara(arrayYCnt) / 2;

	// Spindle 2개가 동시에 작업하지 않는 경우, Pitch 값이 0인경우
	// Array가 1이면 Pitch 는 무조건 0 이어야 함
	// Array Count 2 이상일 때만 하위 연산 가능
	if((0 >= nYLineMoveCnt) || (0 >= g_pNV->gerberPara(arrayYPitch)))
		return (dResult);

	int    nArraySpindleYCnt = ((int)g_pNV->gerberPara(arrayYCnt) + 1) / 2; 
	double routerYPitch = g_pNV->gerberPara(arrayYPitch) * nArraySpindleYCnt;
	double dOffsetF = routerYPitch - g_pNV->DDm(gentryYDisF);
	double dOffsetR = routerYPitch - g_pNV->DDm(gentryYDisR);

	/* 1호기 대비 Axis 컨셉 변경으로 부호 수정
	if(ROUTER_PART_F == m_nId)
	dResult = dOffsetF * 1000.0;
	else
	dResult = -dOffsetR * 1000.0; 
	*/

	if(ROUTER_PART_F == m_nId)
		dResult = dOffsetF * 1000.0;
	else
		dResult = dOffsetR * 1000.0;

	return (dResult);
}


//-------------------------------------------------------------------
int	 CRouter::GetBitChangeIndex(void)
{
	// 부러짐 또는 동심이 맞지 않을때 해당 변수 On..
	int nResult = ROUTER_FR_IDLE;

	BOOL bChangeF  = (int)g_pNV->Pkg(bitZStepCnt) <= m_pInfoBitF->nZStep;
	bChangeF &= m_pInfoBitF->nExist;

	BOOL bChangeR  = (int)g_pNV->Pkg(bitZStepCnt) <= m_pInfoBitR->nZStep;
	bChangeR &= m_pInfoBitR->nExist;

	if(bChangeF)
		m_bNeedBitChangeF = TRUE;
	else if(bChangeR)
		m_bNeedBitChangeR = TRUE;

	// Auto or Manual 일 때만 해당 기능 동작
	if(flagSpidleBitChangeF() && m_pInfoBitF->nExist)
		m_bNeedBitChangeF = TRUE;
	else if(flagSpidleBitChangeR() && m_pInfoBitR->nExist)
		m_bNeedBitChangeR = TRUE;

	if(m_bNeedBitChangeF)
	{
		flagSpidleBitChangeF() = FALSE;
		nResult = ROUTER_F;
	}
	else if(m_bNeedBitChangeR)
	{	
		flagSpidleBitChangeR() = FALSE;
		nResult = ROUTER_R;
	}

	return (nResult);
}


//-------------------------------------------------------------------
int	 CRouter::GetBitWireIndex(void)
{
	// Bit Change 및 LotEnd 후 에 주기별로 전류량 확인
	int nResult = ROUTER_FR_IDLE;

	if(!g_pNV->UseSkip(usSpindleESDCheck))
		return (ROUTER_FR_IDLE);

	if(m_bNeedESDCheckF && m_pInfoBitF->nExist)
		nResult = ROUTER_F;
	else if(m_bNeedESDCheckR && m_pInfoBitR->nExist)
		nResult = ROUTER_R;

	return (nResult);
}

//-------------------------------------------------------------------
int	 CRouter::GetBitVisionIndex(void)
{
	int nResult = ROUTER_FR_IDLE;

	if(!g_pNV->UseSkip(usBitVision))
		return (ROUTER_FR_IDLE);

	if(m_bNeedBitVisionF && m_pInfoBitF->nExist)
		nResult = ROUTER_F;
	else if(m_bNeedBitVisionR && m_pInfoBitR->nExist)
		nResult = ROUTER_R;

	return (nResult);
}


//-------------------------------------------------------------------
int	 CRouter::GetBitBrokenIndex(void)
{
	int nResult = ROUTER_FR_IDLE;

	if(!g_pNV->UseSkip(usBitBroken) && !g_pNV->UseSkip(usBitHeight))
		return (ROUTER_FR_IDLE);

	if(m_bNeedBitBrokenCheckF && m_pInfoBitF->nExist)
		nResult = ROUTER_F;
	else if(m_bNeedBitBrokenCheckR && m_pInfoBitR->nExist)
		nResult = ROUTER_R;

	return (nResult);
}


//-------------------------------------------------------------------
double CRouter::GetZStepPos(int nSpindle)
{
	double dPos  = 0.0;
	int nStep = 0;

	if(ROUTER_F == nSpindle)
	{
		if((int)g_pNV->Pkg(bitZStepCnt) <= m_pInfoBitF->nZStep)
			nStep = (int)g_pNV->Pkg(bitZStepCnt) - 1; // 0 부터 시작하므로 셋팅 수량 -1이 Max
		else
			nStep = m_pInfoBitF->nZStep;

		if(nStep < 0)
			nStep = 0;

		dPos  = m_pMtZ_F->m_pTable->pos[PZ_MOVE_DW];
		dPos += (g_pNV->Pkg(bitZStepPitch) * nStep) * 1000.0;
	}
	else // REAR
	{
		if((int)g_pNV->Pkg(bitZStepCnt) <= m_pInfoBitR->nZStep)
			nStep = (int)g_pNV->Pkg(bitZStepCnt) - 1; // 0 부터 시작하므로 셋팅 수량 -1이 Max
		else
			nStep = m_pInfoBitR->nZStep; // 0 부터 시작하므로 셋팅 수량 -1이 Max

		if(nStep < 0)
			nStep = 0;

		dPos  = m_pMtZ_R->m_pTable->pos[PZ_MOVE_DW];
		dPos += (g_pNV->Pkg(bitZStepPitch) * nStep) * 1000.0;
	}

	/*
	// LLimit 값 설정 필요
	if(Limit)
	{
	Max 값으로 설정
	}
	*/

	return (dPos);
}


//-------------------------------------------------------------------
BOOL CRouter::IsSpindle2PinPitch()
{
	int nYLineMoveCnt = (int)g_pNV->gerberPara(arrayYCnt) / 2;

	// Spindle 2개가 동시에 작업하지 않는 경우, Pitch 값이 0인경우
	// Array가 1이면 Pitch 는 무조건 0 이어야 함
	if((0 >= nYLineMoveCnt) || (0 >= g_pNV->gerberPara(arrayYPitch)))
		return (TRUE);

	// 세로열단위를 기준으로 보았을 때 +1을 해주어야 전체 Y열에서 중간 Y열 위치에 두번째 Spindle이 위치하게 된다.
	// 해당 수치는 Y 홀수열에 대한 예외 처리 임
	int    nArraySpindleYCnt = ((int)g_pNV->gerberPara(arrayYCnt) + 1) / 2;
	double routerYPitch = g_pNV->gerberPara(arrayYPitch) * nArraySpindleYCnt;
	double dRouterYOffsetF = routerYPitch - g_pNV->DDm(gentryYDisF);
	double dRouterYOffsetR = routerYPitch - g_pNV->DDm(gentryYDisR);

	// Spindle Y Pitch Offset 값은 0으로 연산함
	if(ROUTER_PART_F == m_nId)
	{
		//if(g_pNV->UseSkip(usSpindle2Skip))
		//	return (TRUE);
		if(0 > dRouterYOffsetF)
			return (FALSE);
	}
	else
	{
		//if(g_pNV->UseSkip(usSpindle4Skip))
		//	return (TRUE);
		if(0 > dRouterYOffsetR)
			return (FALSE);
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRouter::IsReadyMtSpindleZOverrideF(int nMtIdx)
{
	int    nIndex = nMtIdx;
	double dPos	  = m_pMtZ_F->m_pTable->pos[nIndex];
	double dOffset = 0.0;	

	if(PZ_MOVE_DW == nMtIdx)
	{
		// Step Pos 연산 적용


		dPos += dOffset;
	}

	if(!m_pMtZ_F->IsRdy())
		return (FALSE);

	if(!m_pMtZ_F->InPos(nIndex, dPos, 50))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRouter::MoveMtSpindleZOverrideF(int nMtIdx)
{
	int	   nIndex = nMtIdx;
	double dPos	  = m_pMtZ_F->m_pTable->pos[nIndex];
	double dOffset = 0.0;	

	if(PZ_MOVE_DW == nMtIdx)
	{
		// Step Pos 연산 적용


		dPos += dOffset;
	}

	double endPos   = dPos;
	double slowPos  = dPos - m_pMtZ_F->m_pTable->pos[PZ_SLOW_DN_OFFSET];
	double startVel = m_pMtZ_F->m_pTable->vel[nIndex];
	double slowVel  = m_pMtZ_F->m_pTable->vel[PZ_SLOW_DN_OFFSET];

	m_pMtZ_F->Move2(nIndex, endPos, slowPos, slowVel, startVel);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRouter::IsReadyMtSpindleZOverrideR(int nMtIdx)
{
	int    nIndex = nMtIdx;
	double dPos	  = m_pMtZ_R->m_pTable->pos[nIndex];
	double dOffset = 0.0;	

	if(PZ_MOVE_DW == nMtIdx)
	{
		// Step Pos 연산 적용


		dPos += dOffset;
	}

	if(!m_pMtZ_R->IsRdy())
		return (FALSE);

	if(!m_pMtZ_R->InPos(nIndex, dPos, 50))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRouter::MoveMtSpindleZOverrideR(int nMtIdx)
{
	int	   nIndex = nMtIdx;
	double dPos	  = m_pMtZ_R->m_pTable->pos[nIndex];
	double dOffset = 0.0;	

	if(PZ_MOVE_DW == nMtIdx)
	{
		// Step Pos 연산 적용


		dPos += dOffset;
	}

	double endPos   = dPos;
	double slowPos  = dPos - m_pMtZ_R->m_pTable->pos[PZ_SLOW_DN_OFFSET];
	double startVel = m_pMtZ_R->m_pTable->vel[nIndex];
	double slowVel  = m_pMtZ_R->m_pTable->vel[PZ_SLOW_DN_OFFSET];

	m_pMtZ_R->Move2(nIndex, endPos, slowPos, slowVel, startVel);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRouter::IsMtRdy(void)
{
	if(!m_pMtY->IsRdy() || !m_pMtW->IsRdy() || !m_pMtZ_F->IsRdy() || !m_pMtZ_R->IsRdy())
		return (FALSE);

	return (TRUE);
}


//////////////////////////////////////////////////////////////////////////
//               <MARK 위치>
//			3rd mark	2nd mark
//						1st mark
// 1st, 2nd : y shrinkage
// 2nd, 3rd : x shrinkage, theta
// 2nd mark : 수축율 및 옵셋 연산 기준점.
BOOL CRouter::CreatePrsData(int nIdx, int nArrayYCnt)
{
	// 첫번째 Point는 PRS Y의 SHRINKAGE를 연산하여 사용하려 하였으나
	// 컨셉상 사용하지 않음. PRS를 아예 찍지 않음
	int nBlockCnt = 1; // 무조건 1

	for(int nCnt = 0; nCnt < nBlockCnt; nCnt++)
	{
		int n1st = nCnt * 3;
		int n2nd = n1st + 1;
		int n3rd = n2nd + 1;

		// 무조건 1 Block
		n1st = 0;
		n2nd = 1;
		n3rd = 2;

		int nMax = (int)g_pNV->gerberPara(arrayYCnt) - 1; // 값이 4 일 때 0, 1, 2, 3 이여야 함
		BOOL bCanMoveY  = Between(nArrayYCnt, 1, nMax);

		POINT2D ptArrayYPitch;
		ptArrayYPitch.dX = ptArrayYPitch.dY = 0.0; 

		if(bCanMoveY)
		{
			ptArrayYPitch.dY = g_pNV->gerberPara(arrayYPitch) * nArrayYCnt; // 설비 전면으로 이동
		}

		POINT2D ptOrg1st = g_pIndex[nIdx]->GetMarkCoord(n1st); // 첫번째는 사용 안함으로 YPitch 이동시에 제외시켜야 함
		POINT2D ptOrg2nd = g_pIndex[nIdx]->GetMarkCoord(n2nd) - ptArrayYPitch;
		POINT2D ptOrg3rd = g_pIndex[nIdx]->GetMarkCoord(n3rd) - ptArrayYPitch;

		POINT2D ptReal1st = ptOrg1st + m_viPrsBlock[n1st];
		POINT2D ptReal2nd = ptOrg2nd + m_viPrsBlock[n2nd];
		POINT2D ptReal3rd = ptOrg3rd + m_viPrsBlock[n3rd];

		double dOrgTAdjacent = ptOrg2nd.dX - ptOrg3rd.dX;
		double dOrgTOppsite  = ptOrg2nd.dY - ptOrg3rd.dY;
		double dOrgT		 = GetTheta(dOrgTAdjacent, dOrgTOppsite);	

		SeqLog(L"\n");
		SeqLog(L"\n");
		SeqLog(L"Router PRS Start");
		SeqLog(L"Index[%d] ROUTER PRS ORG T = %3f", nIdx, dOrgT);
		SeqLog(L"Index[%d] Router Vision Offset 1st X = %3f, Y = %3f", nIdx, m_viPrsBlock[n1st].dX, m_viPrsBlock[n1st].dY);
		SeqLog(L"Index[%d] Router Vision Offset 2nd X = %3f, Y = %3f", nIdx, m_viPrsBlock[n2nd].dX, m_viPrsBlock[n2nd].dY);
		SeqLog(L"Index[%d] Router Vision Offset 3rd X = %3f, Y = %3f", nIdx, m_viPrsBlock[n3rd].dX, m_viPrsBlock[n3rd].dY);

		double dAdjacent = ptReal2nd.dX - ptReal3rd.dX;
		double dOppsite  = ptReal2nd.dY - ptReal3rd.dY;
		double dTargetT = GetTheta(dAdjacent, dOppsite);

		g_pIndex[nIdx]->m_pPrsResult->block.dT = dTargetT - dOrgT;

		// theta 보상 이후 옵셋. 기준점은 본래의 센터 기준 위치
		POINT2D ptRotate =  Rotate(ptReal2nd, -g_pIndex[nIdx]->m_pPrsResult->block.dT);
		g_pIndex[nIdx]->m_pPrsResult->block.dX = ptRotate.dX - ptOrg2nd.dX;
		g_pIndex[nIdx]->m_pPrsResult->block.dY = ptRotate.dY - ptOrg2nd.dY;
		g_pIndex[nIdx]->m_pPrsResult->shrinkage.dX = GetDist(ptReal2nd, ptReal3rd) / GetDist(ptOrg2nd, ptOrg3rd); // Real Data 를 실측 Data로 입력
		g_pIndex[nIdx]->m_pPrsResult->shrinkage.dY = GetDist(ptReal2nd, ptReal1st) / GetDist(ptOrg2nd, ptOrg1st); // Real Data 를 실측 Data로 입력

		SeqLog(L"\n");
		SeqLog(L"Index[%d] ROUTER PRS TARGET T = %3f", nIdx, dTargetT);
		SeqLog(L"Index[%d] ROUTER PRS X = %3f", nIdx, g_pIndex[nIdx]->m_pPrsResult->block.dX);
		SeqLog(L"Index[%d] ROUTER PRS Y = %3f", nIdx, g_pIndex[nIdx]->m_pPrsResult->block.dY);
		SeqLog(L"Index[%d] ROUTER PRS T = %3f", nIdx, g_pIndex[nIdx]->m_pPrsResult->block.dT);
		SeqLog(L"Index[%d] ROUTER SHRINKAGE X = %3f", nIdx, g_pIndex[nIdx]->m_pPrsResult->shrinkage.dX);
		SeqLog(L"Index[%d] ROUTER SHRINKAGE Y = %3f", nIdx, g_pIndex[nIdx]->m_pPrsResult->shrinkage.dY);
	}

	SeqLog(L"\n");
	SeqLog(L"\n");

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRouter::CreatePrsDataVerify(int nIdx)
{
	int nBlockCnt = 1; // 무조건 1

	for(int nCnt = 0; nCnt < nBlockCnt; nCnt++)
	{
		int n1st = nCnt * 3;
		int n2nd = n1st + 1;
		int n3rd = n2nd + 1;

		// 무조건 1 Block
		n1st = 0;
		n2nd = 1;
		n3rd = 2;

		POINT2D ptOrg1st = g_pIndex[nIdx]->GetMarkCoord(n1st);
		POINT2D ptOrg2nd = g_pIndex[nIdx]->GetMarkCoord(n2nd);
		POINT2D ptOrg3rd = g_pIndex[nIdx]->GetMarkCoord(n3rd);

		POINT2D ptReal1st = ptOrg1st + m_viPrsBlock[n1st];
		POINT2D ptReal2nd = ptOrg2nd + m_viPrsBlock[n2nd];
		POINT2D ptReal3rd = ptOrg3rd + m_viPrsBlock[n3rd];

		double dOrgTAdjacent = ptOrg2nd.dX - ptOrg3rd.dX;
		double dOrgTOppsite  = ptOrg2nd.dY - ptOrg3rd.dY;
		double dOrgT		 = GetTheta(dOrgTAdjacent, dOrgTOppsite);	

		SeqLog(L"\n");
		SeqLog(L"\n");
		SeqLog(L"Router PRS Start");
		SeqLog(L"Index[%d] ROUTER PRS ORG T = %3f", nIdx, dOrgT);
		SeqLog(L"Index[%d] Router Vision Offset 1st X = %3f, Y = %3f", nIdx, m_viPrsBlock[n1st].dX, m_viPrsBlock[n1st].dY);
		SeqLog(L"Index[%d] Router Vision Offset 2nd X = %3f, Y = %3f", nIdx, m_viPrsBlock[n2nd].dX, m_viPrsBlock[n2nd].dY);
		SeqLog(L"Index[%d] Router Vision Offset 3rd X = %3f, Y = %3f", nIdx, m_viPrsBlock[n3rd].dX, m_viPrsBlock[n3rd].dY);

		double dAdjacent = ptReal2nd.dX - ptReal3rd.dX;
		double dOppsite  = ptReal2nd.dY - ptReal3rd.dY;
		double dTargetT = GetTheta(dAdjacent, dOppsite);

		XYT xytResult;
		xytResult.dX = xytResult.dY = xytResult.dT = 0;
		xytResult.dT = dTargetT - dOrgT;

		// theta 보상 이후 옵셋. 기준점은 본래의 센터 기준 위치
		POINT2D ptRotate =  Rotate(ptReal2nd, -xytResult.dT);
		xytResult.dX = ptRotate.dX - ptOrg2nd.dX;
		xytResult.dY = ptRotate.dY - ptOrg2nd.dY;

		POINT2D ptShrinkage;
		ptShrinkage.dX = GetDist(ptReal2nd, ptReal3rd) / GetDist(ptOrg2nd, ptOrg3rd); 
		ptShrinkage.dY = GetDist(ptReal2nd, ptReal1st) / GetDist(ptOrg2nd, ptOrg1st); 

		SeqLog(L"\n");
		SeqLog(L"Index[%d] ROUTER PRS TARGET T = %3f", nIdx, dTargetT);
		SeqLog(L"Index[%d] PRS X = %3f", nIdx, xytResult.dX);
		SeqLog(L"Index[%d] PRS Y = %3f", nIdx, xytResult.dY);
		SeqLog(L"Index[%d] PRS T = %3f", nIdx, xytResult.dT);
		SeqLog(L"Index[%d] SHRINKAGE X = %3f", nIdx, ptShrinkage.dX);
		SeqLog(L"Index[%d] SHRINKAGE Y = %3f", nIdx, ptShrinkage.dY);
	}

	SeqLog(L"\n");
	SeqLog(L"\n");

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRouter::GetPrsDataLimit(void)
{
	if(g_pNV->DDm(prsVisionOffsetLimitX) < fabs(m_viPrsData.dX))
	{
		SeqLog(L"Router PRS X Limit Recive = %3f", m_viPrsData.dX);
		return (TRUE);
	}

	if(g_pNV->DDm(prsVisionOffsetLimitY) < fabs(m_viPrsData.dY))
	{
		SeqLog(L"Router PRS Y Limit Recive = %3f", m_viPrsData.dY);
		return (TRUE);
	}

	return (FALSE);
}



//-------------------------------------------------------------------
BOOL CRouter::GetLdcDataLimit(void)
{
	if(g_pNV->DDm(ldcVisionOffsetLimitX) < fabs(m_viPrsData.dX))
	{
		SeqLog(L"Pcb Load Check X Limit Recive = %3f", m_viPrsData.dX);
		return (TRUE);
	}

	if(g_pNV->DDm(ldcVisionOffsetLimitY) < fabs(m_viPrsData.dY))
	{
		SeqLog(L"Pcb Load Check Y Limit Recive = %3f", m_viPrsData.dY);
		return (TRUE);
	}

	return (FALSE);
}


//-------------------------------------------------------------------
int CRouter::GetState(void)
{
	int nState = S_IDLE;

	if(!IsPartUseSkip())
		return (S_IDLE);

	if(ROUTER_PART_F == m_nId)
	{
		BOOL bReqReadyPos  = ROUTER_FR_IDLE < g_routerR.m_nBitWireIdx;
		bReqReadyPos |= ROUTER_FR_IDLE < g_routerR.m_nBitVisionIdx;
		bReqReadyPos &= g_routerR.m_bReqReadyPos;

		if(bReqReadyPos)
			return (S_READY);
	}
	else
	{
		BOOL bReqReadyPos  = ROUTER_FR_IDLE < g_routerF.m_nBitWireIdx;
		bReqReadyPos |= ROUTER_FR_IDLE < g_routerF.m_nBitVisionIdx;
		bReqReadyPos &= g_routerF.m_bReqReadyPos;

		if(bReqReadyPos)
			return (S_READY);
	}


	//@ 1. EJECT 되고
	//@ 2. CLAMP 까지 된 상태
	//@ ----> EJECT를 못하게 하면 CLAMP는 자동으로 하지 않음
	//@ ----> 
	if(m_pInfoBitF->nExist && m_pInfoBitR->nExist)
	{
		if(ROUTER_FR_IDLE < m_nBitChangeIdx)
			nState = S_SPD_BIT_EJECT;
		else if(ROUTER_FR_IDLE < m_nBitVisionIdx)
			nState = S_SPD_BIT_VI;
		else if(ROUTER_FR_IDLE < m_nBitWireIdx)
			nState = S_SPD_WIRE_CHECK;
		else if(ROUTER_FR_IDLE < m_nBitBrokenIdx)
			nState = S_SPD_BIT_BROKEN_CHECK;
		else
			nState = S_ROUTER; 
	}
	else
	{
		if(!m_pInfoBitF->nExist)
		{
			if(!ExistCylBitAlignRedF())
			{
				m_nBitCurIdx = ROUTER_F;
				if (!ExistCylBitClamp())
					nState = S_CYL_BIT_CLAMP;
				else if(m_bNeedBitColorF && g_pNV->UseSkip(usBitColor))
					nState = S_SPD_BIT_COLOR;
				else
					nState = S_CYL_BIT_ALIGN_F;
			}
			else
			{
				m_nBitCurIdx = ROUTER_F;
				nState = S_SPD_BIT_CLAMP_F;
			}
		}
		else if(!m_pInfoBitR->nExist)
		{
			if(!ExistCylBitAlignRedR() && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
			{
				m_nBitCurIdx = ROUTER_R;
				if (!ExistCylBitClamp())
					nState = S_CYL_BIT_CLAMP;
				else if(m_bNeedBitColorR && g_pNV->UseSkip(usBitColor))
					nState = S_SPD_BIT_COLOR;
				else
					nState = S_CYL_BIT_ALIGN_R;
			}
			else if (!ExistCylBitAlignBlueR() && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				m_nBitCurIdx = ROUTER_R;

				if (!ExistCylBitClamp())
					nState = S_CYL_BIT_CLAMP;
				else if (m_bNeedBitColorR && g_pNV->UseSkip(usBitColor))
					nState = S_SPD_BIT_COLOR;
				else
					nState = S_CYL_BIT_ALIGN_R;
			}
			else
			{
				m_nBitCurIdx = ROUTER_R;
				nState = S_SPD_BIT_CLAMP_R;
			}
		}
	}

	return (nState);
}


//-------------------------------------------------------------------
BOOL CRouter::IsErr(void)
{
	if(!m_pMtY->m_state.isHome)
		return (TRUE);
	if(!m_pMtW->m_state.isHome)
		return (TRUE);
	if(!m_pMtZ_F->m_state.isHome)
		return (TRUE);
	if(!m_pMtZ_R->m_state.isHome)
		return (TRUE);

	// Index01, Index02, Index03, Index04  
	BOOL bUseIndexCycle  = m_fsm.Between(C_ROUTER_START, C_ROUTER_END);
	 	bUseIndexCycle |= m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END);
	  	bUseIndexCycle |= m_fsm.Between(C_LDC_START, C_LDC_END);
	 	bUseIndexCycle |= m_fsm.Between(C_PRS_START, C_PRS_END);

	if(bUseIndexCycle)
	{
		if(INDEX_F == m_fsm.GetMsg())
		{
			if(!m_pIndexF->m_pMtX->m_state.isHome)
				return (TRUE);
			if(!m_pIndexF->m_pMtT->m_state.isHome)
				return (TRUE);
			if(0 < m_pIndexF->m_pCylMaskFB_L->GetErr())
				return (TRUE);
			if(0 < m_pIndexF->m_pCylMaskFB_R->GetErr())
				return (TRUE);
			if(0 < m_pIndexF->m_pCylMaskUD->GetErr())
				return (TRUE);
		}
		else
		{
			if(!m_pIndexR->m_pMtX->m_state.isHome)
				return (TRUE);
			if(!m_pIndexR->m_pMtT->m_state.isHome)
				return (TRUE);
			if(0 < m_pIndexR->m_pCylMaskFB_L->GetErr())
				return (TRUE);
			if(0 < m_pIndexR->m_pCylMaskFB_R->GetErr())
				return (TRUE);
			if(0 < m_pIndexR->m_pCylMaskUD->GetErr())
				return (TRUE);
		}
	}

	// Router Front Part -> Index01, Router Rear Part -> Index04 
	BOOL bUseIndexCycle2  = m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END);
	bUseIndexCycle2 |= m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END);
	//bUseIndexCycle2 |= m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END);
	bUseIndexCycle2 |= m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END);
	bUseIndexCycle2 |= m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END);

	if(bUseIndexCycle2)
	{
		if(ROUTER_PART_F == m_nId)
		{
			if(!m_pIndexF->m_pMtX->m_state.isHome)
				return (TRUE);
			if(!m_pIndexF->m_pMtT->m_state.isHome)
				return (TRUE);
			if(0 < m_pIndexF->m_pCylMaskFB_L->GetErr())
				return (TRUE);
			if(0 < m_pIndexF->m_pCylMaskFB_R->GetErr())
				return (TRUE);
			if(0 < m_pIndexF->m_pCylMaskUD->GetErr())
				return (TRUE);
		}
		else
		{
			if(!m_pIndexR->m_pMtX->m_state.isHome)
				return (TRUE);
			if(!m_pIndexR->m_pMtT->m_state.isHome)
				return (TRUE);
			if(0 < m_pIndexR->m_pCylMaskFB_L->GetErr())
				return (TRUE);
			if(0 < m_pIndexR->m_pCylMaskFB_R->GetErr())
				return (TRUE);
			if(0 < m_pIndexR->m_pCylMaskUD->GetErr())
				return (TRUE);
		}
	}

	if(0 < m_pCylBitClampUD->GetErr())
		return (TRUE);
	if(0 < m_pCylBitClampOC->GetErr())
		return (TRUE);
	if(0 < m_pSolSpdChuckOC_F->GetErr())
		return (TRUE);
	if(0 < m_pSolSpdChuckOC_R->GetErr())
		return (TRUE);

	if(0 < m_pSolSpindleBlow->GetErr())
		return (TRUE);
	if(0 < m_pSolRouterIonizerF->GetErr())
		return (TRUE);
	if(0 < m_pSolRouterIonizerR->GetErr())
		return (TRUE);
	if(0 < m_pSpindleF->GetErr())
		return (TRUE);
	if (0 < m_pSpindleR->GetErr())
		return (TRUE);

	return (FALSE);
}


//-------------------------------------------------------------------
int  CRouter::GetExistBitAlignFErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL bSenOn = FALSE; 
	if(ROUTER_PART_F == m_nId)
		bSenOn = g_dIn.BOn(iIndex01BitAlignExist01); 
	else
		bSenOn = g_dIn.BOn(iIndex04BitAlignExist01);

	if(ExistCylBitAlignRedF() == bSenOn)
	{
		m_tmExistBitAlignFErr.Reset();
	}
	else
	{
		if(m_tmExistBitAlignFErr.TmOver(1))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
int  CRouter::GetExistBitAlignRErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL bSenOn = FALSE; 
	if(ROUTER_PART_F == m_nId)
		bSenOn = g_dIn.BOn(iIndex01BitAlignExist02); 
	else
		bSenOn = g_dIn.BOn(iIndex04BitAlignExist02);

	if(ExistCylBitAlignRedR() == bSenOn)
	{
		m_tmExistBitAlignRErr.Reset();
	}
	else
	{
		if(m_tmExistBitAlignRErr.TmOver(1))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
void CRouter::CycleRunRouter(void)
{
	if(!m_fsm.Between(C_ROUTER_START, C_ROUTER_END))
		return;

	if(m_fsm.TimeLimit(1000000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_ROUTER_CYCLE_TM_OVER_01 + (m_nId*2) + m_fsm.GetMsg());
		return;
	}

	if(!IsMtRdy())
		return;

	if(INDEX_F == m_fsm.GetMsg())
	{
		if(!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
			return;
	}
	else
	{
		if(!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
			return;
	}

	_char cXPos[_MAX_CHAR_SIZE_], cYPos[_MAX_CHAR_SIZE_], cTPos[_MAX_CHAR_SIZE_], cWPos[_MAX_CHAR_SIZE_], cZmainPos[_MAX_CHAR_SIZE_], cZsubPos[_MAX_CHAR_SIZE_];
	_char cXMoveEventId[_MAX_CHAR_SIZE_], cYMoveEventId[_MAX_CHAR_SIZE_], cTMoveEventId[_MAX_CHAR_SIZE_], cWMoveEventId[_MAX_CHAR_SIZE_], cZMainMoveEventId[_MAX_CHAR_SIZE_], cZSubMoveEventId[_MAX_CHAR_SIZE_];
	_char cSpdEventId[_MAX_CHAR_SIZE_], cSpdBlowEventId[_MAX_CHAR_SIZE_];
	_char cDustShutterId[_MAX_CHAR_SIZE_], cTopHouseVacId[_MAX_CHAR_SIZE_];
	_char cIonizerId[2][_MAX_CHAR_SIZE_];
	_char cRoutingSpd[_MAX_CHAR_SIZE_];

	int nIdx = INDEX_IDLE;

	if(ROUTER_PART_F == m_nId)
	{
		_sprintf(cSpdEventId, L"SPD1_RUN");
		_sprintf(cSpdBlowEventId, L"SOL_SPD_AIR_BLOW_0102_ON");
		_sprintf(cIonizerId[0], L"SOL_ROUTER_IONIZER_01_ON");
		_sprintf(cIonizerId[1], L"SOL_ROUTER_IONIZER_02_ON");

		if(INDEX_F == m_fsm.GetMsg())
			nIdx = INDEX_01;
		else 
			nIdx = INDEX_02;
	}
	else
	{
		_sprintf(cSpdEventId, L"SPD2_RUN");
		_sprintf(cSpdBlowEventId, L"SOL_SPD_AIR_BLOW_0304_ON");
		_sprintf(cIonizerId[0], L"SOL_ROUTER_IONIZER_03_ON");
		_sprintf(cIonizerId[1], L"SOL_ROUTER_IONIZER_04_ON");

		if(INDEX_F == m_fsm.GetMsg())
			nIdx = INDEX_03;
		else 
			nIdx = INDEX_04;
	}

	_sprintf(cDustShutterId, L"CYL_DUST_SHUTTER_%02d_OPEN", (nIdx + 1));
	_sprintf(cTopHouseVacId, L"CYL_TOP_HOUSE_VAC_%02d_DOWN", (nIdx + 1));

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");	
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + nIdx].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + nIdx].pcbBarcode) + 1);

	_char cLotId[_MAX_CHAR_SIZE_];
	mbstowcs(cLotId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + nIdx].lotID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + nIdx].lotID) + 1);

	/////////////////////////////////////////////////////////////////////
	// Spindle Rear Skip 일 경우 앞단 Spindle로 모든 Path 이동
	BOOL bSpindleRearSkip = IsSpindleRearSkip(m_nId);
	double dVelStart = g_pNV->DDm(routerVelStart) * 1000.0;
	double dVelEnd = g_pNV->DDm(routerVelEnd) * 1000.0;

	switch(m_fsm.Get())
	{
	case C_ROUTER_START:
		if(m_fsm.Once())
		{
			NEGRETE_WRITE(g_TpBase.logProcess(g_data2c.cRouter[m_nId].deviceId, L"ROUTER_CUTTING", g_data2c.cEtc.start, cMaterialId, cLotId, _wcsupr(g_cRecipeId)));
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRouter[m_nId].deviceId, L"ROUTER_CUTTING", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"ROUTER_CUTTING_START", L"ROUTER_CUTTING_END"))

				if(TOP_BLOW)
				{
					// 일단 무조건 On 하도록 한다.
					//g_pm[SOL_INDEX_STAGE_AIR_BLOW_0102].Actuate(pmON);
					//g_pm[SOL_INDEX_STAGE_AIR_BLOW_0304].Actuate(pmON);
				}

				if(g_pNV->UseSkip(usSecsGem))
					g_pNV->NDm(gemStageRouter01Start + nIdx) = STATE_REQ;

				if(!IsSpindle2PinPitch())
				{
					m_fsm.Set(C_ERROR, ER_ROUTER_Y_SPINDLE_PITCH_ERR);
					break;
				}			
				else
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
				}

				if (fabs(g_pNV->DDm(indexAirFlowMinValue1 + nIdx)) > fabs(g_pNV->DDm(srcIndexFlow01 + nIdx))	// 위치변경
					&& pmOPEN == g_pm[CYL_INDEX_DUST_SHUTTER_OC_01 + nIdx].GetPos(200) && g_pIndex[nIdx]->ExistPcb() 
					&& !g_opr.isDryRun && !g_pNV->NDm(mmiBtnAdcMode))
				{
					m_fsm.Set(C_ERROR, ER_INDEX1_AIR_FLOW_IS_LOW + nIdx);
				}

		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
			{
				m_pCylBitClampUD->Actuate(pmUP);
				break;
			}			

			if(!g_opr.isDryRun)
			{
				//g_dOut.On(oSpindleVel01F);
				//g_dOut.On(oSpindleVel02F);
				//g_dOut.On(oSpindleVel01R);
				//g_dOut.On(oSpindleVel02R);

				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
				{
					if (ROUTER_PART_F == m_nId)
					{
						if (!g_dOut.IsOn(oSpindleReverseRun02))
						{
							g_dOut.On(oSpindleReverseRun02);
							break;
						}
					}
					else
					{
						if (!g_dOut.IsOn(oSpindleReverseRun04))
						{
							g_dOut.On(oSpindleReverseRun04);
							break;
						}
					}
				}
				else
				{
					g_dOut.Off(oSpindleReverseRun02);
					g_dOut.Off(oSpindleReverseRun04);
				}

				if(pmON != m_pSpindleF->GetPos(2000) || pmON != m_pSpindleR->GetPos(2000))
				{
					m_pSpindleF->Actuate(pmON); 
					m_pSpindleR->Actuate(pmON);
					if (!g_logChk.bFunction[m_pSpindleF->GetNo()])
					{
						g_logChk.bFunction[m_pSpindleF->GetNo()] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cSpdEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.spindle, g_data2c.cEtc.actName, g_data2c.cPmName[m_pSpindleF->GetNo()], g_data2c.cEtc.delayTime, L"2000", 
							g_data2c.cPmIO[m_pSpindleF->GetNo()].In[pmON][pmON], g_data2c.cEtc.off, 
							g_data2c.cPmIO[m_pSpindleF->GetNo()].In[pmON][pmOFF], g_data2c.cEtc.on))
					}
					break;
				}
				else
				{
					if(g_logChk.bFunction[m_pSpindleF->GetNo()])
					{
						g_logChk.bFunction[m_pSpindleF->GetNo()] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cSpdEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.spindle, g_data2c.cEtc.actName, g_data2c.cPmName[m_pSpindleF->GetNo()], g_data2c.cEtc.delayTime, L"2000", 
							g_data2c.cPmIO[m_pSpindleF->GetNo()].In[pmON][pmON], g_data2c.cEtc.on, 
							g_data2c.cPmIO[m_pSpindleF->GetNo()].In[pmON][pmOFF], g_data2c.cEtc.off))
					}
				}
			}

			if(pmON != m_pSolSpindleBlow->GetPos(300))
			{
				m_pSolSpindleBlow->Actuate(pmON);

				if(!g_logChk.bFunction[m_pSolSpindleBlow->GetNo()])
				{
					g_logChk.bFunction[m_pSolSpindleBlow->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cSpdBlowEventId, g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sol, g_data2c.cEtc.actName, g_data2c.cPmName[m_pSolSpindleBlow->GetNo()], L"'BLOW'", L"'ON'"))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pSolSpindleBlow->GetNo()])
				{
					g_logChk.bFunction[m_pSolSpindleBlow->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cSpdBlowEventId, g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sol, g_data2c.cEtc.actName, g_data2c.cPmName[m_pSolSpindleBlow->GetNo()], L"'BLOW'", L"'OFF'"))
				}
			}

			if(INDEX_F == m_fsm.GetMsg())
			{
				if(!m_pIndexF->CylIndexMaskFixAct(pmCLOSE))
					break;
				if(!g_opr.isDryRun)
				{
					if(pmON != m_pSolRouterIonizerF->GetPos(300) || pmON != m_pSolRouterIonizerR->GetPos(300))
					{
						m_pSolRouterIonizerF->Actuate(pmON);
						m_pSolRouterIonizerR->Actuate(pmON);

						if(!g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()])
						{
							g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[0], g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerF->GetNo()], L"'BLOW'", L"'ON'"))
						}

						if(!g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()])
						{
							g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[1], g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerR->GetNo()], L"'BLOW'", L"'ON'"))
						}
						break;
					}
					else
					{
						if(g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()])
						{
							g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()] = FALSE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[0], g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerF->GetNo()], L"'BLOW'", L"'OFF'"))
						}

						if(g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()])
						{
							g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()] = FALSE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[1], g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerR->GetNo()], L"'BLOW'", L"'OFF'"))
						}					
					}
				}

				m_fsm.Set(C_ROUTER_FRONT);
			}
			else
			{
				if(!m_pIndexR->CylIndexMaskFixAct(pmCLOSE))
					break;

				if(!g_opr.isDryRun)
				{
					if(pmON != m_pSolRouterIonizerF->GetPos(300) || pmON != m_pSolRouterIonizerR->GetPos(300))
					{
						m_pSolRouterIonizerF->Actuate(pmON);
						m_pSolRouterIonizerR->Actuate(pmON);

						if(!g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()])
						{
							g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[0], g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerF->GetNo()], L"'BLOW'", L"'ON'"))
						}

						if(!g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()])
						{
							g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[1], g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerR->GetNo()], L"'BLOW'", L"'ON'"))
						}
						break;
					}
					else
					{
						if(g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()])
						{
							g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()] = FALSE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[0], g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerF->GetNo()], L"'BLOW'", L"'OFF'"))
						}

						if(g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()])
						{
							g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()] = FALSE;
							NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[1], g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerR->GetNo()], L"'BLOW'", L"'OFF'"))
						}	
					}
				}
				m_fsm.Set(C_ROUTER_REAR);
			}
		}
		break;
	case C_ROUTER_FRONT:
		if(m_fsm.Once())
		{
			if(ROUTER_PART_F == m_nId)
				g_pNV->NDm(spindleSpeedUpload0102) = 1; 
			else 
				g_pNV->NDm(spindleSpeedUpload0304) = 1;

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemTraceInfoStage01 + nIdx) = STATE_REQ;

			if(!g_pNV->UseSkip(usRouterPrs))
			{
				for(int n = 0; n < UNIT_MAX; n++)
				{
					m_pIndexF->m_pPrsResult->unit[n].dX = 0;
					m_pIndexF->m_pPrsResult->unit[n].dY = 0;
					m_pIndexF->m_pPrsResult->unit[n].dT = 0;
				}

				m_pIndexF->m_pPrsResult->block.dX = 0.0;
				m_pIndexF->m_pPrsResult->block.dY = 0.0;
				m_pIndexF->m_pPrsResult->block.dT = 0.0;
			}

			m_tmRouterCycle.Reset();
		}
		else
		{
			/////////////////////////////////////////////////////////////////////
			// Spindle 2개 사용기준 Cutting Move Cnt
			// 0 = Spindle Front만 사용하여 Max Path 만큼 이동
			// 0 < 듀얼로 동작 가능한 상태
			int nYLineMoveCnt = (int)g_pNV->gerberPara(arrayYCnt) / 2;

			/////////////////////////////////////////////////////////////////////
			// Spindle 2개 사용시 마지막열은 Spindle 1개로만 사용
			// 0 = 듀얼 동작 가능
			// 1 = 마지막 동작은 Spindle Front 만 사용하여 1라인 Path 이동
			int nYRearSkip		= (int)g_pNV->gerberPara(arrayYCnt) % 2; 
			int nMaxPathCnt		= (int)g_pNV->gerberPara(arrayPathCnt) * (int)g_pNV->gerberPara(arrayXCnt) * (int)g_pNV->gerberPara(arrayYCnt);
			int nArrayPathCnt	= (int)g_pNV->gerberPara(arrayPathCnt);
			int nYLineCurCnt	= m_pIndexF->m_pMem->routerCmdCnt / (int)g_pNV->gerberPara(arrayPathCnt);

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

			int nLineType = m_pIndexF->GetGerberLineType(m_pIndexF->m_pMem->routerCmdCnt);
			if(-1 == nLineType)
			{
				m_fsm.Set(C_ERROR, ER_GERBER_LINE_TYPE_ERR);
				break;
			}

			if(0 == nLineType)
			{
				_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_START", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
				_sprintf(cTMoveEventId, L"MT_INDEX_T_%02d_ROUTER", (nIdx + 1));
				_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_START", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
				_sprintf(cWMoveEventId, L"MT_ROUTER_W_%02d_ROUTER", (nIdx + 1));
				_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_DOWN", (((m_nId % 2) * 2) + 1));
				_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_DOWN", (((m_nId % 2) * 2) + 2));
				_sprintf(cRoutingSpd, L"%03f", dVelStart);

				if(0 == m_fsm.GetStep())
				{
					// Start Pos 이동
					XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelStart);
					m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					double dSubOffset = (ptSubOffset.dY - ptMainOffset.dY) * 1000;														  
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_F, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelStart);

					if(!g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN] = TRUE;
						_sprintf(cTPos, L"%03f", xytPos.dT);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_POSIDX_], cTPos, 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(1);
				}
				else if(1 == m_fsm.GetStep())
				{
					// log만을 위한 연산
					XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					double dSubOffset = (ptSubOffset.dY - ptMainOffset.dY) * 1000;														  
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					if(g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN] = FALSE;
						_sprintf(cTPos, L"%03f", xytPos.dT);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_POSIDX_], cTPos, 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					double dPosF = GetZStepPos(ROUTER_F);
					m_pMtZ_F->PMove(PZ_MOVE_DW, dPosF);

					if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW] = TRUE;
						_sprintf(cZmainPos, L"%03f", dPosF);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_POSIDX_], cZmainPos, 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACC_]))
					}

					m_pInfoBitF->nLength += (int)g_pNV->Pkg(bit1CutLength);

					if(!bSpindleRearSkip)
					{
						if((0 < nYLineMoveCnt) && (0 == nYRearSkip)) // 듀얼 짝수
						{
							double dPosR = GetZStepPos(ROUTER_R);
							m_pMtZ_R->PMove(PZ_MOVE_DW, dPosR);

							if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
							{
								g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = TRUE;
								_sprintf(cZsubPos, L"%03f", dPosR);
								NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
							}

							m_pInfoBitR->nLength += (int)g_pNV->Pkg(bit1CutLength);
						}
						else if((0 < nYLineMoveCnt) && (1 == nYRearSkip))  // 듀얼 홀수
						{
							if((nYLineCurCnt < nYLineMoveCnt)) // 값이 같거나 현재 카운트가 크면 Spindle Front만 사용하여 홀수열 Path이동
							{
								double dPosR = GetZStepPos(ROUTER_R);
								m_pMtZ_R->PMove(PZ_MOVE_DW, dPosR);

								if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
								{
									g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = TRUE;
									_sprintf(cZsubPos, L"%03f", dPosR);
									NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
										cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
								}

								m_pInfoBitR->nLength += (int)g_pNV->Pkg(bit1CutLength);
							}
						}
					}
					m_fsm.SetStep(2);
				}
				else if(2 == m_fsm.GetStep())
				{
					double dPosF = GetZStepPos(ROUTER_F);
					if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW] = FALSE;
						_sprintf(cZmainPos, L"%03f", dPosF);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_POSIDX_], cZmainPos, 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACC_]))
					}

					double dPosR = GetZStepPos(ROUTER_R);
					if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = FALSE;
						_sprintf(cZsubPos, L"%03f", dPosR);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
					}

					// End Pos 이동 (T 오동작으로 위험 할 수 있으므로 T 동작하지 않음)
					XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelEnd);
					//m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;	
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_F, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelEnd);

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(!g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(3);
				}
				else if(3 == m_fsm.GetStep())
				{
					XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;	
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 1));
					_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 2));

					m_pMtZ_F->Move(PZ_MOVE_UP);
					if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], //cMtName까지만
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACC_]))
					}

					if(!bSpindleRearSkip)
					{
						if((0 < nYLineMoveCnt) && (0 == nYRearSkip))
						{
							m_pMtZ_R->Move(PZ_MOVE_UP);
							if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
							{
								g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = TRUE;
								NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
							}
						}
						else if((0 < nYLineMoveCnt) && (1 == nYRearSkip))
						{
							if((nYLineCurCnt < nYLineMoveCnt)) // 값이 같거나 현재 카운트가 크면 Spindle Front만 사용하여 홀수열 Path이동
							{
								m_pMtZ_R->Move(PZ_MOVE_UP);
								if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
								{
									g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = TRUE;
									NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
										cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
								}
							}
						}
					}
					m_fsm.SetStep(4);
				}
				else if(4 == m_fsm.GetStep())
				{
					_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 1));
					_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 2));

					if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
					}

					m_pIndexF->m_pMem->routerCmdCnt++;

					if(m_pIndexF->m_pMem->routerCmdCnt < nMaxPathCnt)
						m_fsm.SetStep(0);
					else
						m_fsm.Set(C_ROUTER_03);
				}
			}
			else if(1 == nLineType)
			{
				_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_START", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
				_sprintf(cTMoveEventId, L"MT_INDEX_T_%02d_ROUTER", (nIdx + 1));
				_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_START", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
				_sprintf(cWMoveEventId, L"MT_ROUTER_W_%02d_ROUTER", (nIdx + 1));
				_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_DOWN", (((m_nId % 2) * 2) + 1));
				_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_DOWN", (((m_nId % 2) * 2) + 2));
				_sprintf(cRoutingSpd, L"%03f", dVelStart);

				if(0 == m_fsm.GetStep())
				{
					// Start Pos 이동
					XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelStart);
					m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_F, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelStart);

					if(!g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN] = TRUE;
						_sprintf(cTPos, L"%03f", xytPos.dT);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_POSIDX_], cTPos, 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(1);
				}
				else if(1 == m_fsm.GetStep())
				{
					// log만을 위한 연산
					XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_START);
					double dSubOffset = (ptSubOffset.dY - ptMainOffset.dY) * 1000;														  
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					if(g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN] = FALSE;
						_sprintf(cTPos, L"%03f", xytPos.dT);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_POSIDX_], cTPos, 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					double dPosF = GetZStepPos(ROUTER_F);
					m_pMtZ_F->PMove(PZ_MOVE_DW, dPosF);

					if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW] = TRUE;
						_sprintf(cZmainPos, L"%03f", dPosF);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_POSIDX_], cZmainPos, 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACC_]))
					}

					m_pInfoBitF->nLength += (int)g_pNV->Pkg(bit1CutLength);

					if(!bSpindleRearSkip)
					{
						if((0 < nYLineMoveCnt) && (0 == nYRearSkip))
						{
							double dPosR = GetZStepPos(ROUTER_R);
							m_pMtZ_R->PMove(PZ_MOVE_DW, dPosR);

							if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
							{
								g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = TRUE;
								_sprintf(cZsubPos, L"%03f", dPosR);
								NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
							}
							m_pInfoBitR->nLength += (int)g_pNV->Pkg(bit1CutLength);
						}
						else if((0 < nYLineMoveCnt) && (1 == nYRearSkip))
						{
							if((nYLineCurCnt < nYLineMoveCnt)) // 값이 같거나 현재 카운트가 크면 Spindle Front만 사용하여 홀수열 Path이동
							{
								double dPosR = GetZStepPos(ROUTER_R);
								m_pMtZ_R->PMove(PZ_MOVE_DW, dPosR);
								if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
								{
									g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = TRUE;
									_sprintf(cZsubPos, L"%03f", dPosR);
									NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
										cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
								}
								m_pInfoBitR->nLength += (int)g_pNV->Pkg(bit1CutLength);
							}
						}
					}
					m_fsm.SetStep(2);
				}
				else if(2 == m_fsm.GetStep())
				{
					double dPosF = GetZStepPos(ROUTER_F);
					if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW] = FALSE;
						_sprintf(cZmainPos, L"%03f", dPosF);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_POSIDX_], cZmainPos, 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACC_]))
					}

					double dPosR = GetZStepPos(ROUTER_R);
					if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = FALSE;
						_sprintf(cZsubPos, L"%03f", dPosR);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
					}

					// (T 오동작으로 위험 할 수 있으므로 T 동작하지 않음)
					XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_01);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelEnd);
					//m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_01);
					POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_01);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_F, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelEnd);

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_MID_01", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_MID_01", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(!g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(3);
				}
				else if(3 == m_fsm.GetStep())
				{
					XYT xytPosLog = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_01);
					POINT2D ptMainOffsetLog = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_01);
					POINT2D ptSubOffsetLog = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_01);
					double dSubOffsetLog = (ptSubOffsetLog.dY - ptMainOffsetLog.dY) * 1000;
					double dPosWLog = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffsetLog = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_MID_01", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_MID_01", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPosLog.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPosLog.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosWLog + dOffsetLog + dSubOffsetLog);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					// (T 오동작으로 위험 할 수 있으므로 T 동작하지 않음)
					XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_02);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelEnd);
					//m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_02);
					POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_02);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_F, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelEnd);

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_MID_02", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_MID_02", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(!g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(4);
				}
				else if(4 == m_fsm.GetStep())
				{
					XYT xytPosLog = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_02);
					POINT2D ptMainOffsetLog = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_02);
					POINT2D ptSubOffsetLog = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_MID_02);
					double dSubOffsetLog = (ptSubOffsetLog.dY - ptMainOffsetLog.dY) * 1000;
					double dPosWLog = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffsetLog = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_MID_02", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_MID_02", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPosLog.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPosLog.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosWLog + dOffsetLog + dSubOffsetLog);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					// End Pos 이동 (T 오동작으로 위험 할 수 있으므로 T 동작하지 않음)
					XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelEnd);
					//m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;	
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_F, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelEnd);

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(!g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(5);
				}
				else if(5 == m_fsm.GetStep())
				{
					XYT xytPos = m_pIndexF->GetRouterPos(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptMainOffset = m_pIndexF->GetGerberOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptSubOffset = m_pIndexF->GetGerberSubOffset(m_pIndexF->m_pMem->routerCmdCnt, POS_END);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;	
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 1));
					_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 2));

					m_pMtZ_F->Move(PZ_MOVE_UP);
					if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[nIdx].Z1[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[nIdx].Z1[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[nIdx].Z1[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[nIdx].Z1[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[nIdx].Z1[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[nIdx].Z1[PZ_MOVE_UP][_ACC_]))
					}

					if(!bSpindleRearSkip)
					{
						if((0 < nYLineMoveCnt) && (0 == nYRearSkip))
						{
							m_pMtZ_R->Move(PZ_MOVE_UP);
							if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
							{
								g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = TRUE;
								NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
							}
						}
						else if((0 < nYLineMoveCnt) && (1 == nYRearSkip))
						{
							if((nYLineCurCnt < nYLineMoveCnt)) // 값이 같거나 현재 카운트가 크면 Spindle Front만 사용하여 홀수열 Path이동
							{
								m_pMtZ_R->Move(PZ_MOVE_UP);
								if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
								{
									g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = TRUE;
									NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
										cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
								}
							}
						}
					}
					m_fsm.SetStep(6);
				}
				else if(6 == m_fsm.GetStep())
				{
					_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 1));
					_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 2));
					if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
					}

					m_pIndexF->m_pMem->routerCmdCnt++;

					if(m_pIndexF->m_pMem->routerCmdCnt < nMaxPathCnt)
						m_fsm.SetStep(0);
					else
						m_fsm.Set(C_ROUTER_03);
				}
			}
			else if(3 == nLineType)
			{
				// 원호 타입은 어떻게 구동 ??
			}
		}
		break;

	case C_ROUTER_REAR:
		if(m_fsm.Once())
		{
			if(ROUTER_PART_F == m_nId)
				g_pNV->NDm(spindleSpeedUpload0102) = 1; 
			else 
				g_pNV->NDm(spindleSpeedUpload0304) = 1;

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemTraceInfoStage01 + nIdx) = STATE_REQ;

			if(!g_pNV->UseSkip(usRouterPrs))
			{
				for(int n = 0; n < UNIT_MAX; n++)
				{
					m_pIndexR->m_pPrsResult->unit[n].dX = 0;
					m_pIndexR->m_pPrsResult->unit[n].dY = 0;
					m_pIndexR->m_pPrsResult->unit[n].dT = 0;
				}

				m_pIndexR->m_pPrsResult->block.dX = 0.0;
				m_pIndexR->m_pPrsResult->block.dY = 0.0;
				m_pIndexR->m_pPrsResult->block.dT = 0.0;
			}

			m_tmRouterCycle.Reset();
		}
		else
		{
			/////////////////////////////////////////////////////////////////////
			// Spindle 2개 사용기준 Cutting Move Cnt
			// 0 = Spindle Front만 사용하여 Max Path 만큼 이동
			// 0 < 듀얼로 동작 가능한 상태
			int nYLineMoveCnt = (int)g_pNV->gerberPara(arrayYCnt) / 2; 

			/////////////////////////////////////////////////////////////////////
			// Spindle 2개 사용시 마지막열은 Spindle 1개로만 사용
			// 0 = 듀얼 동작 가능
			// 1 = 마지막 동작은 Spindle Front 만 사용하여 1라인 Path 이동
			int nYRearSkip  = (int)g_pNV->gerberPara(arrayYCnt) % 2; 
			int nMaxPathCnt = (int)g_pNV->gerberPara(arrayPathCnt) * (int)g_pNV->gerberPara(arrayXCnt) * (int)g_pNV->gerberPara(arrayYCnt);
			int nArrayPathCnt   = (int)g_pNV->gerberPara(arrayPathCnt);
			int nYLineCurCnt = m_pIndexR->m_pMem->routerCmdCnt / (int)g_pNV->gerberPara(arrayPathCnt);

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

			int nLineType = m_pIndexR->GetGerberLineType(m_pIndexR->m_pMem->routerCmdCnt);
			if(-1 == nLineType)
			{
				m_fsm.Set(C_ERROR, ER_GERBER_LINE_TYPE_ERR);
				break;
			}

			if(0 == nLineType)
			{
				_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_START", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
				_sprintf(cTMoveEventId, L"MT_INDEX_T_%02d_ROUTER", (nIdx + 1));
				_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_START", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
				_sprintf(cWMoveEventId, L"MT_ROUTER_W_%02d_ROUTER", (nIdx + 1));
				_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_DOWN", (((m_nId % 2) * 2) + 1));
				_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_DOWN", (((m_nId % 2) * 2) + 2));
				_sprintf(cRoutingSpd, L"%03f", dVelStart);

				if(0 == m_fsm.GetStep())
				{
					// Start Pos 이동
					XYT xytPos = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelStart);
					m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptSubOffset = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;	
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_R, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelStart);

					if(!g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pIndexR->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN] = TRUE;
						_sprintf(cTPos, L"%03f", xytPos.dT);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_POSIDX_], cTPos, 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(1);
				}
				else if(1 == m_fsm.GetStep())
				{
					XYT xytPosLog = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptMainOffsetLog = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptSubOffsetLog = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					double dSubOffsetLog = (ptSubOffsetLog.dY - ptMainOffsetLog.dY) * 1000;	
					double dPosWLog = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffsetLog = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					if(g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPosLog.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPosLog.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pIndexR->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN] = FALSE;
						_sprintf(cTPos, L"%03f", xytPosLog.dT);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_POSIDX_], cTPos, 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosWLog + dOffsetLog + dSubOffsetLog);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					double dPosF = GetZStepPos(ROUTER_F);
					m_pMtZ_F->PMove(PZ_MOVE_DW, dPosF);
					if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW] = TRUE;
						_sprintf(cZmainPos, L"%03f", dPosF);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_POSIDX_], cZmainPos, 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACC_]))
					}

					m_pInfoBitF->nLength += (int)g_pNV->Pkg(bit1CutLength);

					if(!bSpindleRearSkip)
					{
						if((0 < nYLineMoveCnt) && (0 == nYRearSkip))
						{
							double dPosR = GetZStepPos(ROUTER_R);
							m_pMtZ_R->PMove(PZ_MOVE_DW, dPosR);
							if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
							{
								g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = TRUE;
								_sprintf(cZsubPos, L"%03f", dPosR);
								NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
							}
							m_pInfoBitR->nLength += (int)g_pNV->Pkg(bit1CutLength);
						}
						else if((0 < nYLineMoveCnt) && (1 == nYRearSkip))
						{
							if((nYLineCurCnt < nYLineMoveCnt)) // 값이 같거나 현재 카운트가 크면 Spindle Front만 사용하여 홀수열 Path이동
							{
								double dPosR = GetZStepPos(ROUTER_R);
								m_pMtZ_R->PMove(PZ_MOVE_DW, dPosR);
								if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
								{
									g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = TRUE;
									_sprintf(cZsubPos, L"%03f", dPosR);
									NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
										cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
								}
								m_pInfoBitR->nLength += (int)g_pNV->Pkg(bit1CutLength);
							}
						}
					}
					m_fsm.SetStep(2);
				}
				else if(2 == m_fsm.GetStep())
				{
					double dPosF = GetZStepPos(ROUTER_F);
					if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW] = FALSE;
						_sprintf(cZmainPos, L"%03f", dPosF);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_POSIDX_], cZmainPos, 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACC_]))
					}

					double dPosR = GetZStepPos(ROUTER_R);
					if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = FALSE;
						_sprintf(cZsubPos, L"%03f", dPosR);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
					}

					// End Pos 이동	(T 오동작으로 위험 할 수 있으므로 T 동작하지 않음)
					XYT xytPos = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelEnd);
					//m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptSubOffset = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;	
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_R, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelEnd);

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(!g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(3);
				}
				else if(3 == m_fsm.GetStep())
				{
					XYT xytPos = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptMainOffset = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptSubOffset = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;	
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_R][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 1));
					_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 2));

					m_pMtZ_F->Move(PZ_MOVE_UP);
					if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP] = TRUE; 
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACC_]))
					}

					if(!bSpindleRearSkip)
					{
						if((0 < nYLineMoveCnt) && (0 == nYRearSkip))
						{
							m_pMtZ_R->Move(PZ_MOVE_UP);
							if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
							{
								g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = TRUE;
								NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
							}
						}
						else if((0 < nYLineMoveCnt) && (1 == nYRearSkip))
						{
							if((nYLineCurCnt < nYLineMoveCnt)) // 값이 같거나 현재 카운트가 크면 Spindle Front만 사용하여 홀수열 Path이동
							{
								m_pMtZ_R->Move(PZ_MOVE_UP);
								if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
								{
									g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = TRUE;
									NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
										cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
								}
							}
						}
					}
					m_fsm.SetStep(4);
				}
				else if(4 == m_fsm.GetStep())
				{
					_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 1));
					_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 2));

					if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP] = FALSE; 
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
					}

					m_pIndexR->m_pMem->routerCmdCnt++;

					if(m_pIndexR->m_pMem->routerCmdCnt < nMaxPathCnt)
						m_fsm.SetStep(0);
					else
						m_fsm.Set(C_ROUTER_03);
				}
			}
			else if(1 == nLineType)
			{
				_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_START", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
				_sprintf(cTMoveEventId, L"MT_INDEX_T_%02d_ROUTER", (nIdx + 1));
				_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_START", (nIdx + 1), m_pIndexF->m_pMem->routerCmdCnt);
				_sprintf(cWMoveEventId, L"MT_ROUTER_W_%02d_ROUTER", (nIdx + 1));
				_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_DOWN", (((m_nId % 2) * 2) + 1));
				_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_DOWN", (((m_nId % 2) * 2) + 2));
				_sprintf(cRoutingSpd, L"%03f", dVelStart);

				if(0 == m_fsm.GetStep())
				{
					// Start Pos 이동
					XYT xytPos = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelStart);
					m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptSubOffset = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_R, PW_ROUTER, xytPos.dY, dPosW + dOffset +dSubOffset, dVelStart);

					if(!g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN] = TRUE;
						_sprintf(cTPos, L"%03f", xytPos.dT);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_POSIDX_], cTPos, 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(1);
				}
				else if(1 == m_fsm.GetStep())
				{
					XYT xytPosLog = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptMainOffsetLog = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					POINT2D ptSubOffsetLog = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_START);
					double dSubOffsetLog = (ptSubOffsetLog.dY - ptMainOffsetLog.dY) * 1000;
					double dPosWLog = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffsetLog = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					if(g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPosLog.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPosLog.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_RUN] = FALSE;
						_sprintf(cTPos, L"%03f", xytPosLog.dT);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_POSIDX_], cTPos, 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosWLog + dOffsetLog + dSubOffsetLog);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					double dPosF = GetZStepPos(ROUTER_F);
					m_pMtZ_F->PMove(PZ_MOVE_DW, dPosF);

					if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW] = TRUE;
						_sprintf(cZmainPos, L"%03f", dPosF);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_POSIDX_], cZmainPos, 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACC_]))
					}

					m_pInfoBitF->nLength += (int)g_pNV->Pkg(bit1CutLength);
					if(!bSpindleRearSkip)
					{
						if((0 < nYLineMoveCnt) && (0 == nYRearSkip))
						{
							double dPosR = GetZStepPos(ROUTER_R);
							m_pMtZ_R->PMove(PZ_MOVE_DW, dPosR);
							if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
							{
								g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = TRUE;
								_sprintf(cZsubPos, L"%03f", dPosR);
								NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
							}
							m_pInfoBitR->nLength += (int)g_pNV->Pkg(bit1CutLength);
						}
						else if((0 < nYLineMoveCnt) && (1 == nYRearSkip))
						{
							if((nYLineCurCnt < nYLineMoveCnt)) // 값이 같거나 현재 카운트가 크면 Spindle Front만 사용하여 홀수열 Path이동
							{
								double dPosR = GetZStepPos(ROUTER_R);
								m_pMtZ_R->PMove(PZ_MOVE_DW, dPosR);
								if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
								{
									g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = TRUE;
									_sprintf(cZsubPos, L"%03f", dPosR);
									NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
										cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
								}
								m_pInfoBitR->nLength += (int)g_pNV->Pkg(bit1CutLength);
							}
						}
					}
					m_fsm.SetStep(2);
				}
				else if(2 == m_fsm.GetStep())
				{
					double dPosF = GetZStepPos(ROUTER_F);
					if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_DW] = FALSE;
						_sprintf(cZmainPos, L"%03f", dPosF);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_POSIDX_], cZmainPos, 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_DW][_ACC_]))
					}

					double dPosR = GetZStepPos(ROUTER_R);
					if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW])
					{
						g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_DW] = FALSE;
						_sprintf(cZsubPos, L"%03f", dPosR);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_POSIDX_], cZsubPos, 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_SPD_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_DW][_ACC_]))
					}

					// (T 오동작으로 위험 할 수 있으므로 T 동작하지 않음)
					XYT xytPos = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_01);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelEnd);
					//m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_01);
					POINT2D ptSubOffset = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_01);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_R, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelEnd);

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_MID_01", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_MID_01", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(!g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(3);
				}
				else if(3 == m_fsm.GetStep())
				{
					XYT xytPosLog = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_01);
					POINT2D ptMainOffsetLog = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_01);
					POINT2D ptSubOffsetLog = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_01);
					double dSubOffsetLog = (ptSubOffsetLog.dY - ptMainOffsetLog.dY) * 1000;
					double dPosWLog = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffsetLog = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_MID_01", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_MID_01", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPosLog.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPosLog.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosWLog + dOffsetLog + dSubOffsetLog);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					// (T 오동작으로 위험 할 수 있으므로 T 동작하지 않음)
					XYT xytPos = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_02);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelEnd);
					//m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_02);
					POINT2D ptSubOffset = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_02);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_R, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelEnd);

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_MID_02", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_MID_02", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(!g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(4);
				}
				else if(4 == m_fsm.GetStep())
				{
					XYT xytPosLog = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_02);
					POINT2D ptMainOffsetLog = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_02);
					POINT2D ptSubOffsetLog = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_MID_02);
					double dSubOffsetLog = (ptSubOffsetLog.dY - ptMainOffsetLog.dY) * 1000;
					double dPosWLog = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffsetLog = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_MID_02", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_MID_02", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPosLog.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPosLog.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosWLog + dOffsetLog + dSubOffsetLog);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					// End Pos 이동 (T 오동작으로 위험 할 수 있으므로 T 동작하지 않음)
					XYT xytPos = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_RUN, xytPos.dX, dVelEnd);
					//m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_RUN, xytPos.dT);
					POINT2D ptMainOffset = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptSubOffset = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					double dSubOffset = (ptSubOffset.dY-ptMainOffset.dY) * 1000;	
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffset = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting
					GentryMtYWPMove(PY_ROUTER_R, PW_ROUTER, xytPos.dY, dPosW + dOffset + dSubOffset, dVelEnd);

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(!g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = TRUE;
						_sprintf(cXPos, L"%03f", xytPos.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = TRUE;
						_sprintf(cYPos, L"%03f", xytPos.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = TRUE;
						_sprintf(cWPos, L"%03f", dPosW + dOffset + dSubOffset);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					m_fsm.SetStep(5);
				}
				else if(5 == m_fsm.GetStep())
				{
					XYT xytPosLog = m_pIndexR->GetRouterPos(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptMainOffsetLog = m_pIndexR->GetGerberOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					POINT2D ptSubOffsetLog = m_pIndexR->GetGerberSubOffset(m_pIndexR->m_pMem->routerCmdCnt, POS_END);
					double dSubOffsetLog = (ptSubOffsetLog.dY - ptMainOffsetLog.dY) * 1000;	
					double dPosWLog = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					double dOffsetLog = GetBitYOffset(); // Spindle이 2개의 가변 Pitch Setting

					_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_CUT_%02d_END", (nIdx + 1), m_pIndexR->m_pMem->routerCmdCnt);
					_sprintf(cRoutingSpd, L"%03f", dVelEnd);

					if(g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_RUN] = FALSE;
						_sprintf(cXPos, L"%03f", xytPosLog.dX);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_SPDIDX_], cRoutingSpd, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_ROUTER_F] = FALSE;
						_sprintf(cYPos, L"%03f", xytPosLog.dY);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_SPDIDX_], cRoutingSpd, 
							g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_ROUTER_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER])
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_ROUTER] = FALSE;
						_sprintf(cWPos, L"%03f", dPosWLog + dOffsetLog + dSubOffsetLog);
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cWMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_POSIDX_], cWPos, 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_ROUTER][_ACC_]))
					}

					_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 1));
					_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 2));

					m_pMtZ_F->Move(PZ_MOVE_UP);

					if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACC_]))
					}

					if(!bSpindleRearSkip)
					{
						if((0 < nYLineMoveCnt) && (0 == nYRearSkip))
						{
							m_pMtZ_R->Move(PZ_MOVE_UP);

							if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
							{
								g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = TRUE;
								NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
									g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
							}
						}
						else if((0 < nYLineMoveCnt) && (1 == nYRearSkip))
						{
							if((nYLineCurCnt < nYLineMoveCnt)) // 값이 같거나 현재 카운트가 크면 Spindle Front만 사용하여 홀수열 Path이동
							{
								m_pMtZ_R->Move(PZ_MOVE_UP);

								if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
								{
									g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = TRUE;
									NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
										cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
										g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
								}
							}
						}
					}
					m_fsm.SetStep(6);
				}
				else if(6 == m_fsm.GetStep())
				{
					_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 1));
					_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_MOVE_UP", (((m_nId % 2) * 2) + 2));
					if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_MOVE_UP] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_MOVE_UP][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP])
					{
						g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_MOVE_UP] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_POS_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_SPD_], 
							g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_MOVE_UP][_ACC_]))
					}

					m_pIndexR->m_pMem->routerCmdCnt++;

					if(m_pIndexR->m_pMem->routerCmdCnt < nMaxPathCnt)
						m_fsm.SetStep(0);
					else
						m_fsm.Set(C_ROUTER_03);
				}
			}
			else if(3 == nLineType)
			{
				// 원호 타입은 어떻게 구동 ??
			}
		}
		break;
	case C_ROUTER_03:
		{
			double tm = m_tmRouterCycle.Elapsed()/1000.0;

			if(INDEX_F == m_fsm.GetMsg())
			{
				if(ROUTER_PART_F == m_nId)
				{
					g_pNV->DDm(cycleTmRouterCutting1) = tm;
					SeqLog(L"[Router Cycle Time] INDEX[1] Router Cutting Z Down -> Z Up Cycle [%3f] (sec)", tm);
				}
				else
				{
					g_pNV->DDm(cycleTmRouterCutting3) = tm;
					SeqLog(L"[Router Cycle Time] INDEX[3] Router Cutting Z Down -> Z Up Cycle [%3f] (sec)", tm);
				}
			}
			else
			{
				if(ROUTER_PART_F == m_nId)
				{
					g_pNV->DDm(cycleTmRouterCutting2) = tm;
					SeqLog(L"[Router Cycle Time] INDEX[2] Router Cutting Z Down -> Z Up Cycle [%3f] (sec)", tm);
				}
				else
				{
					g_pNV->DDm(cycleTmRouterCutting4) = tm;
					SeqLog(L"[Router Cycle Time] INDEX[4] Router Cutting Z Down -> Z Up Cycle [%3f] (sec)", tm);
				}
			}

			m_fsm.Set(C_ROUTER_04);
		}
		break;
	case C_ROUTER_04:

		_sprintf(cIonizerId[0], L"SOL_ROUTER_IONIZER_01_OFF");
		_sprintf(cIonizerId[1], L"SOL_ROUTER_IONIZER_02_OFF");

		if(INDEX_F == m_fsm.GetMsg())
		{
			if(pmOFF != m_pSolRouterIonizerF->GetPos(10) || pmOFF != m_pSolRouterIonizerR->GetPos(10))
			{
				m_pSolRouterIonizerF->Actuate(pmOFF);
				m_pSolRouterIonizerR->Actuate(pmOFF);

				if(!g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()])
				{
					g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[0], g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerF->GetNo()], L"'BLOW'", L"'ON'"))
				}
				if(!g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()])
				{
					g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[1], g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerR->GetNo()], L"'BLOW'", L"'ON'"))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()])
				{
					g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()] = FALSE; 
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[0], g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerF->GetNo()], L"'BLOW'", L"'OFF'"))
				}
				if(g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()])
				{
					g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[1], g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerR->GetNo()], L"'BLOW'", L"'OFF'"))
				}
			}
		}
		else  
		{
			if(pmOFF != m_pSolRouterIonizerF->GetPos(10) || pmOFF != m_pSolRouterIonizerR->GetPos(10))
			{
				m_pSolRouterIonizerF->Actuate(pmOFF);
				m_pSolRouterIonizerR->Actuate(pmOFF);

				if(!g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()])
				{
					g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[0], g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerF->GetNo()], L"'BLOW'", L"'ON'"))
				}
				if(!g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()])
				{
					g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[1], g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerR->GetNo()], L"'BLOW'", L"'ON'"))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()])
				{
					g_logChk.bFunction[m_pSolRouterIonizerF->GetNo()] = FALSE; 
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[0], g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerF->GetNo()], L"'BLOW'", L"'OFF'"))
				}
				if(g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()])
				{
					g_logChk.bFunction[m_pSolRouterIonizerR->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cIonizerId[1], g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'SOL'", L"'ACT_NAME'", g_data2c.cPmName[m_pSolRouterIonizerR->GetNo()], L"'BLOW'", L"'OFF'"))
				}
			}
		}
		m_fsm.Set(C_ROUTER_END);
		break;
	case C_ROUTER_END:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);

			_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_READY", (((m_nId % 2) * 2) + 1));
			_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_READY", (((m_nId % 2) * 2) + 2));

			if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_POS_], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_SPD_], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_ACC_]))
			}
			if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_POS_], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_SPD_], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_ACC_]))
			}
		}
		else
		{
			_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_READY", (((m_nId % 2) * 2) + 1));
			_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_READY", (((m_nId % 2) * 2) + 2));

			if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_POS_], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_SPD_], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_ACC_]))
			}
			if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.end, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_POS_], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_SPD_], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_ACC_]))
			}

			_sprintf(cSpdBlowEventId, L"SOL_SPD_AIR_BLOW_0102_OFF");			

			if(pmOFF != m_pSolSpindleBlow->GetPos(10))
			{
				m_pSolSpindleBlow->Actuate(pmOFF);

				if(!g_logChk.bFunction[m_pSolSpindleBlow->GetNo()])
				{
					g_logChk.bFunction[m_pSolSpindleBlow->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cSpdBlowEventId, g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sol, g_data2c.cEtc.actName, g_data2c.cPmName[m_pSolSpindleBlow->GetNo()], L"'BLOW'", L"'ON'"))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pSolSpindleBlow->GetNo()])
				{
					g_logChk.bFunction[m_pSolSpindleBlow->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cSpdBlowEventId, g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sol, g_data2c.cEtc.actName, g_data2c.cPmName[m_pSolSpindleBlow->GetNo()], L"'BLOW'", L"'OFF'"))
				}
			}

			if(INDEX_F == m_fsm.GetMsg())
			{
				m_pIndexF->m_pMem->routerCmdCnt  = 0; 
				m_pIndexF->m_pMem->compRouterRun = TRUE; 
			}
			else
			{
				m_pIndexR->m_pMem->routerCmdCnt  = 0; 
				m_pIndexR->m_pMem->compRouterRun = TRUE; 
			}

			if(g_pNV->UseSkip(usBitBroken) || g_pNV->UseSkip(usBitHeight))
			{
				m_bNeedBitBrokenCheckF = TRUE; 
				m_bNeedBitBrokenCheckR = TRUE;
			}

			if(g_pNV->DDm(bitMaxLifeLength) <= m_pInfoBitF->nLength)
			{
				m_pInfoBitF->nLength = 0;
				m_pInfoBitF->nZStep++;
			}

			if(g_pNV->DDm(bitMaxLifeLength) <= m_pInfoBitR->nLength)
			{
				m_pInfoBitR->nLength = 0;
				m_pInfoBitR->nZStep++;
			}

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemStageRouter01End + nIdx) = STATE_REQ;

			NEGRETE_WRITE(g_TpBase.logProcess(g_data2c.cRouter[m_nId].deviceId, L"ROUTER_CUTTING", g_data2c.cEtc.end, cMaterialId, cLotId, _wcsupr(g_cRecipeId)));
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRouter[m_nId].deviceId, L"ROUTER_CUTTING", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"ROUTER_CUTTING_START", L"ROUTER_CUTTING_END"))

			m_fsm.Set(C_IDLE);
		}
		break;
	}
}


//-------------------------------------------------------------------
void CRouter::CycleRunLiveVision(void)
{
	if(!m_fsm.Between(C_LIVE_VI_START, C_LIVE_VI_END))
		return;

	if(m_fsm.TimeLimit(1000000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_LIVE_VI_CYCLE_TM_OVER_01 + (m_nId*2) + m_fsm.GetMsg());
		return;
	}

	if(!IsMtRdy())
		return;

	if(INDEX_F == m_fsm.GetMsg())
	{
		if(!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy() || !m_pMtY->IsRdy())
			return;
	}
	else
	{
		if(!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy()|| !m_pMtY->IsRdy())
			return;
	}

	double dVelStart = g_pNV->DDm(routerVelViStart) * 1000.0;
	double dVelEnd   = g_pNV->DDm(routerVelViEnd) * 1000.0;

	switch(m_fsm.Get())
	{
	case C_LIVE_VI_START:
		if(m_fsm.Once())
		{
			m_nLiveViPos = 0;
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
				m_pCylBitClampUD->Actuate(pmUP);
			else if(pmOFF != m_pSpindleF->GetPos(300) || pmOFF != m_pSpindleR->GetPos(300))
			{
				m_pSpindleF->Actuate(pmOFF);
				m_pSpindleR->Actuate(pmOFF);
			}
			else
			{
				if(INDEX_F == m_fsm.GetMsg())
				{
					if(!m_pIndexF->CylIndexMaskFixAct(pmCLOSE))
						break;
					m_fsm.Set(C_LIVE_VI_RST);
				}
				else
				{
					if(!m_pIndexR->CylIndexMaskFixAct(pmCLOSE))
						break;
					m_fsm.Set(C_LIVE_VI_RST);
				}
			}
		}
		break;

	case C_LIVE_VI_RST:
		if(m_fsm.Once())
		{
			// Z 같이 동작
			m_pMtZ_F->Move(PZ_PRS);

			if(ROUTER_PART_F == m_nId)
				g_dOut.On(oViRouterPrsRstF);
			else
				g_dOut.On(oViRouterPrsRstR);
		}
		else
		{
			if(ROUTER_PART_F == m_nId)
			{
				if(m_fsm.TimeLimit(5000))
				{
					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_F_PRS_NOT_READY);
					break;
				}

				if((g_dIn.AOn(iViRouterPrsReadyF) && !g_dIn.AOn(iViRouterPrsBusyF)) || g_opr.isDryRun)
				{
					g_dOut.Off(oViRouterPrsRstF);
				}
			}
			else
			{
				if(m_fsm.TimeLimit(5000))
				{
					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_R_PRS_NOT_READY);
					break;
				}

				if((g_dIn.AOn(iViRouterPrsReadyR) && !g_dIn.AOn(iViRouterPrsBusyR)) || g_opr.isDryRun)
				{
					g_dOut.Off(oViRouterPrsRstR);
				}
			}

			if(INDEX_F == m_fsm.GetMsg())
				m_fsm.Set(C_LIVE_VI_FRONT);
			else
				m_fsm.Set(C_LIVE_VI_REAR);
		}
		break;
	case C_LIVE_VI_FRONT:
		if(m_fsm.Once())
		{
			if(ROUTER_PART_F == m_nId)
				g_dOut.On(oViLiveModeRouterF);
			else
				g_dOut.On(oViLiveModeRouterR);

			if(!g_pNV->UseSkip(usRouterPrs))
			{
				for(int n = 0; n < UNIT_MAX; n++)
				{
					m_pIndexF->m_pPrsResult->unit[n].dX = 0;
					m_pIndexF->m_pPrsResult->unit[n].dY = 0;
					m_pIndexF->m_pPrsResult->unit[n].dT = 0;
				}

				m_pIndexF->m_pPrsResult->block.dX = 0.0;
				m_pIndexF->m_pPrsResult->block.dY = 0.0;
				m_pIndexF->m_pPrsResult->block.dT = 0.0;
			}
		}
		else
		{

			/////////////////////////////////////////////////////////////////////
			// Live Vision은 모든 위치 전부 Scan
			int nMaxPathCnt = (int)g_pNV->gerberPara(arrayPathCnt) * (int)g_pNV->gerberPara(arrayXCnt) * (int)g_pNV->gerberPara(arrayYCnt);
			int nLineType = m_pIndexF->GetGerberLineType(m_nLiveViPos);
			if(-1 == nLineType)
			{
				m_fsm.Set(C_ERROR, ER_GERBER_LINE_TYPE_ERR);
				break;
			}

			if(0 == nLineType)
			{
				if(0 == m_fsm.GetStep())
				{
					// Start Pos 이동
					XYT xytPos = m_pIndexF->GetRouterLiveViPos(m_nLiveViPos, POS_START);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelStart);
					m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_F, PW_READY, xytPos.dY, dPosW, dVelStart);
					m_fsm.SetStep(1);
				}
				else if(1 == m_fsm.GetStep())
				{
					// End Pos 이동
					XYT xytPos = m_pIndexF->GetRouterLiveViPos(m_nLiveViPos, POS_END);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelEnd);
					//m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_F, PW_READY, xytPos.dY, dPosW, dVelEnd);
					m_fsm.SetStep(2);
				}
				else if(2 == m_fsm.GetStep())
				{
					m_nLiveViPos++;
					if(m_nLiveViPos < nMaxPathCnt)
						m_fsm.SetStep(0);
					else
						m_fsm.Set(C_LIVE_VI_END);
				}
			}
			else if(1 == nLineType)
			{
				if(0 == m_fsm.GetStep())
				{
					// Start Pos 이동
					XYT xytPos = m_pIndexF->GetRouterLiveViPos(m_nLiveViPos, POS_START);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelStart);
					m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_F, PW_READY, xytPos.dY, dPosW, dVelStart);
					m_fsm.SetStep(1);
				}
				else if(1 == m_fsm.GetStep())
				{
					Sleep(300);
					XYT xytPos = m_pIndexF->GetRouterLiveViPos(m_nLiveViPos, POS_MID_01);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelEnd);
					//m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_F, PW_READY, xytPos.dY, dPosW, dVelEnd);
					m_fsm.SetStep(2);
				}
				else if(2 == m_fsm.GetStep())
				{
					XYT xytPos = m_pIndexF->GetRouterLiveViPos(m_nLiveViPos, POS_MID_02);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelEnd);
					//m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_F, PW_READY, xytPos.dY, dPosW, dVelEnd);
					m_fsm.SetStep(3);
				}
				else if(3 == m_fsm.GetStep())
				{
					XYT xytPos = m_pIndexF->GetRouterLiveViPos(m_nLiveViPos, POS_END);
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelEnd);
					//m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_F, PW_READY, xytPos.dY, dPosW, dVelEnd);
					m_fsm.SetStep(4);
				}
				else if(4 == m_fsm.GetStep())
				{
					Sleep(300);
					m_nLiveViPos++;
					if(m_nLiveViPos < nMaxPathCnt)
						m_fsm.SetStep(0);
					else
						m_fsm.Set(C_LIVE_VI_END);
				}
			}
			else if(3 == nLineType)
			{
				// 원호 타입은 어떻게 구동 ??
			}
		}
		break;

	case C_LIVE_VI_REAR:
		if(m_fsm.Once())
		{
			if(ROUTER_PART_F == m_nId)
				g_dOut.On(oViLiveModeRouterF);
			else
				g_dOut.On(oViLiveModeRouterR);

			if(!g_pNV->UseSkip(usRouterPrs))
			{
				for(int n = 0; n < UNIT_MAX; n++)
				{
					m_pIndexR->m_pPrsResult->unit[n].dX = 0;
					m_pIndexR->m_pPrsResult->unit[n].dY = 0;
					m_pIndexR->m_pPrsResult->unit[n].dT = 0;
				}

				m_pIndexR->m_pPrsResult->block.dX = 0.0;
				m_pIndexR->m_pPrsResult->block.dY = 0.0;
				m_pIndexR->m_pPrsResult->block.dT = 0.0;
			}
		}
		else
		{
			/////////////////////////////////////////////////////////////////////
			// Live Vision은 모든 위치 전부 Scan
			int nMaxPathCnt = (int)g_pNV->gerberPara(arrayPathCnt) * (int)g_pNV->gerberPara(arrayXCnt) * (int)g_pNV->gerberPara(arrayYCnt);
			int nLineType = m_pIndexR->GetGerberLineType(m_nLiveViPos);
			if(-1 == nLineType)
			{
				m_fsm.Set(C_ERROR, ER_GERBER_LINE_TYPE_ERR);
				break;
			}

			if(0 == nLineType)
			{
				if(0 == m_fsm.GetStep())
				{
					// Start Pos 이동
					XYT xytPos = m_pIndexR->GetRouterLiveViPos(m_nLiveViPos, POS_START);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelStart);
					m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_R, PW_READY, xytPos.dY, dPosW, dVelStart);
					m_fsm.SetStep(1);
				}
				else if(1 == m_fsm.GetStep())
				{
					// End Pos 이동
					XYT xytPos = m_pIndexR->GetRouterLiveViPos(m_nLiveViPos, POS_END);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelEnd);
					//m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_R, PW_READY, xytPos.dY, dPosW, dVelEnd);
					m_fsm.SetStep(2);
				}
				else if(2 == m_fsm.GetStep())
				{
					m_nLiveViPos++;
					if(m_nLiveViPos < nMaxPathCnt)
						m_fsm.SetStep(0);
					else
						m_fsm.Set(C_LIVE_VI_END);
				}
			}
			else if(1 == nLineType)
			{
				if(0 == m_fsm.GetStep())
				{
					// Start Pos 이동
					XYT xytPos = m_pIndexR->GetRouterLiveViPos(m_nLiveViPos, POS_START);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelStart);
					m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_R, PW_READY, xytPos.dY, dPosW, dVelStart);
					m_fsm.SetStep(1);
				}
				else if(1 == m_fsm.GetStep())
				{
					Sleep(300);
					XYT xytPos = m_pIndexR->GetRouterLiveViPos(m_nLiveViPos, POS_MID_01);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelEnd);
					//m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_R, PW_READY, xytPos.dY, dPosW, dVelEnd);
					m_fsm.SetStep(2);
				}
				else if(2 == m_fsm.GetStep())
				{
					XYT xytPos = m_pIndexR->GetRouterLiveViPos(m_nLiveViPos, POS_MID_02);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelEnd);
					//m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_R, PW_READY, xytPos.dY, dPosW, dVelEnd);
					m_fsm.SetStep(3);
				}
				else if(3 == m_fsm.GetStep())
				{
					XYT xytPos = m_pIndexR->GetRouterLiveViPos(m_nLiveViPos, POS_END);
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_LIVE_VI, xytPos.dX, dVelEnd);
					//m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_LIVE_VI, xytPos.dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_LIVE_R, PW_READY, xytPos.dY, dPosW, dVelEnd);
					m_fsm.SetStep(4);
				}
				else if(4 == m_fsm.GetStep())
				{
					Sleep(300);
					m_nLiveViPos++;
					if(m_nLiveViPos < nMaxPathCnt)
						m_fsm.SetStep(0);
					else
						m_fsm.Set(C_LIVE_VI_END);
				}
			}
			else if(3 == nLineType)
			{
				// 원호 타입은 어떻게 구동 ??
			}
		}
		break;

	case C_LIVE_VI_END:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(ROUTER_PART_F == m_nId)
				g_dOut.Off(oViLiveModeRouterF);
			else
				g_dOut.Off(oViLiveModeRouterR);

			if(INDEX_F == m_fsm.GetMsg())
				m_pIndexF->m_pMem->compRouterLiveVi = TRUE;
			else
				m_pIndexR->m_pMem->compRouterLiveVi = TRUE;

			m_fsm.Set(C_IDLE);
		}
		break;
	}
}


//-------------------------------------------------------------------
void CRouter::CycleRunSpdBitEject(void)
{
	if(!m_fsm.Between(C_SPD_BIT_EJECT_START, C_SPD_BIT_EJECT_END))
		return;

	if(m_fsm.TimeLimit(180000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_SPD_BIT_EJECT_CYCLE_TM_OVER_01 + (m_nId*2) + m_fsm.GetMsg());
		return;
	}

	if(!IsMtRdy())
		return;

	int nIdx = SPINDLE_IDLE;

	if (ROUTER_PART_F == m_nId)
	{
		if (ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_01;
		else
			nIdx = SPINDLE_02;
	}
	else
	{
		if (ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_03;
		else
			nIdx = SPINDLE_04;
	}

	if (ROUTER_PART_F == m_nId)
	{
		if (ROUTER_F == m_fsm.GetMsg())
		{
			// Index01
			if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
				return;
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
			{
				if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
					return;
			}
			else
			{
				if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
					return;
			}
		}
	}
	else
	{
		if (ROUTER_F == m_fsm.GetMsg())
		{
			// Index04
			if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
				return;
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
			{
				if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
					return;
			}
			else
			{
				if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
					return;
			}
		}
	}
	
	int nIdxCheck = (m_nId * 2) + m_fsm.GetMsg(); // 0, 1, 2, 3

	switch(m_fsm.Get())
	{
	case C_SPD_BIT_EJECT_START:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
			{
				m_pCylBitClampUD->Actuate(pmUP);
				break;
			}

			if(pmOFF != m_pSpindleF->GetPos(1000) || pmOFF != m_pSpindleR->GetPos(1000))
			{
				m_pSpindleF->Actuate(pmOFF);
				m_pSpindleR->Actuate(pmOFF);
				break;
			}

			if (ROUTER_PART_F == m_nId)
			{
				if (ROUTER_F == m_fsm.GetMsg())
				{
					if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT) ||
						!m_pIndexF->m_pMtT->InPos(CIndex::PT_SPD_BIT_EJECT))
					{
						m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
						m_pIndexF->m_pMtT->Move(CIndex::PT_SPD_BIT_EJECT);
						break;
					}
				}
				else
				{
					if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
					{
						if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT) ||
							!m_pIndexR->m_pMtT->InPos(CIndex::PT_SPD_BIT_EJECT))
						{
							m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
							m_pIndexR->m_pMtT->Move(CIndex::PT_SPD_BIT_EJECT);
							break;
						}
					}
					else
					{
						if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT) ||
							!m_pIndexF->m_pMtT->InPos(CIndex::PT_SPD_BIT_EJECT))
						{
							m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
							m_pIndexF->m_pMtT->Move(CIndex::PT_SPD_BIT_EJECT);
							break;
						}
					}
				}
			}
			else
			{
				if (ROUTER_F == m_fsm.GetMsg())
				{
					if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT) ||
						!m_pIndexR->m_pMtT->InPos(CIndex::PT_SPD_BIT_EJECT))
					{
						m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
						m_pIndexR->m_pMtT->Move(CIndex::PT_SPD_BIT_EJECT);
						break;
					}
				}
				else
				{
					if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
					{
						if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT) ||
							!m_pIndexF->m_pMtT->InPos(CIndex::PT_SPD_BIT_EJECT))
						{
							m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
							m_pIndexF->m_pMtT->Move(CIndex::PT_SPD_BIT_EJECT);
							break;
						}
					}
					else
					{
						if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT) ||
							!m_pIndexR->m_pMtT->InPos(CIndex::PT_SPD_BIT_EJECT))
						{
							m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
							m_pIndexR->m_pMtT->Move(CIndex::PT_SPD_BIT_EJECT);
							break;
						}
					}
				}
			}

			if (ROUTER_F == m_fsm.GetMsg())
			{
				if (!IsGentryMtYWRdy(PY_SPD_BIT_EJECT_01_03_RED_F, PW_READY))
					GentryMtYWMove(PY_SPD_BIT_EJECT_01_03_RED_F, PW_READY);
				else if (!IsReadyMtSpindleZOverrideF(PZ_SPD_BIT_EJECT_RED))
					MoveMtSpindleZOverrideF(PZ_SPD_BIT_EJECT_RED);
				else
					m_fsm.Set(C_SPD_BIT_EJECT_01);
			}
			else
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
				{
						if (!IsGentryMtYWRdy(PY_SPD_BIT_EJECT_BLUE_02, PW_READY))
							GentryMtYWMove(PY_SPD_BIT_EJECT_BLUE_02, PW_READY);
						else if (!IsReadyMtSpindleZOverrideR(PZ_SPD_BIT_EJECT_BLUE))
						{
							MoveMtSpindleZOverrideR(PZ_SPD_BIT_EJECT_BLUE);
						}
						else
							m_fsm.Set(C_SPD_BIT_EJECT_01);
				}
				else if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
				{
					if (!IsGentryMtYWRdy(PY_SPD_BIT_EJECT_BLUE_03, PW_READY))
						GentryMtYWMove(PY_SPD_BIT_EJECT_BLUE_03, PW_READY);
					else if (!IsReadyMtSpindleZOverrideR(PZ_SPD_BIT_EJECT_BLUE))
					{
						MoveMtSpindleZOverrideR(PZ_SPD_BIT_EJECT_BLUE);
					}
					else
						m_fsm.Set(C_SPD_BIT_EJECT_01);
				}
				else
				{
					if (!IsGentryMtYWRdy(PY_SPD_BIT_EJECT_02_04_RED_R, PW_READY))
						GentryMtYWMove(PY_SPD_BIT_EJECT_02_04_RED_R, PW_READY);
					else if (!IsReadyMtSpindleZOverrideR(PZ_SPD_BIT_EJECT_RED))
						MoveMtSpindleZOverrideR(PZ_SPD_BIT_EJECT_RED);
					else
						m_fsm.Set(C_SPD_BIT_EJECT_01);
				}
			}
		}
		break;
	case C_SPD_BIT_EJECT_01:
		if(ROUTER_F == m_fsm.GetMsg())
		{
			if(pmOPEN != m_pSolSpdChuckOC_F->GetPos(500))
			{
				m_pSolSpdChuckOC_F->Actuate(pmOPEN);
				break;
			}
		}
		else
		{
			if(pmOPEN != m_pSolSpdChuckOC_R->GetPos(500))
			{
				m_pSolSpdChuckOC_R->Actuate(pmOPEN);
				break;
			}
		}
		m_fsm.Set(C_SPD_BIT_EJECT_02);
		break;

	case C_SPD_BIT_EJECT_02:
		if(ROUTER_PART_F == m_nId)
		{
			if (ROUTER_F == m_fsm.GetMsg())
			{
				if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT_FORK))
				{
					m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT_FORK);
					break;
				}
			}
			else
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
				{
					if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT_FORK))
					{
						m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT_FORK);
						break;
					}
				}
				else
				{
					if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT_FORK))
					{
						m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT_FORK);
						break;
					}
				}
			}
		}
		else
		{
			if (ROUTER_F == m_fsm.GetMsg())
			{
				if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT_FORK))
				{
					m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT_FORK);
					break;
				}
			}
			else
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
				{
					if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT_FORK))
					{
						m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT_FORK);
						break;
					}
				}
				else
				{
					if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT_FORK))
					{
						m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT_FORK);
						break;
					}
				}
			}
		}
		m_fsm.Set(C_SPD_BIT_EJECT_03);
		break;
	case C_SPD_BIT_EJECT_03:
		if(m_fsm.Once())
		{
			if(ROUTER_F == m_fsm.GetMsg())
			{
				m_pMtZ_F->Move(PZ_SPD_BIT_EJECT_MID_UP_RED);
			}
			else
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
					m_pMtZ_R->Move(PZ_SPD_BIT_EJECT_MID_UP_BLUE);
				else if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
					m_pMtZ_R->Move(PZ_SPD_BIT_EJECT_MID_UP_BLUE);
				else
					m_pMtZ_R->Move(PZ_SPD_BIT_EJECT_MID_UP_RED);
			}
		}
		else
		{
			if(!m_fsm.Delay(1000))
				break;

			if (ROUTER_PART_F == m_nId)
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
				{
					if (g_dIn.AOn(iIndexBitEjectCheck02))
					{
						m_fsm.Set(C_SPD_BIT_EJECT_ERR);
						break;
					}
				}
				else
				{
					if (g_dIn.AOn(iIndexBitEjectCheck01))
					{
						m_fsm.Set(C_SPD_BIT_EJECT_ERR);
						break;
					}
				}
			}
			else
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
				{
					if (g_dIn.AOn(iIndexBitEjectCheck03))
					{
						m_fsm.Set(C_SPD_BIT_EJECT_ERR);
						break;
					}
				}
				else
				{
					if (g_dIn.AOn(iIndexBitEjectCheck04))
					{
						m_fsm.Set(C_SPD_BIT_EJECT_ERR);
						break;
					}
				}
			}

			m_fsm.Set(C_SPD_BIT_EJECT_04);
		}
		break;
	case C_SPD_BIT_EJECT_04:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(ROUTER_F == m_fsm.GetMsg())
				m_pInfoBitF->nExist = FALSE;
			else
				m_pInfoBitR->nExist = FALSE;

			m_fsm.Set(C_SPD_BIT_EJECT_END);
		}
		break;
	case C_SPD_BIT_EJECT_ERR:
		if (ROUTER_PART_F == m_nId)
		{
			if (ROUTER_F == m_fsm.GetMsg())
			{
				if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT))
				{
					m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
					break;
				}

				if (pmCLOSE != m_pSolSpdChuckOC_F->GetPos(500))
					m_pSolSpdChuckOC_F->Actuate(pmCLOSE);
				else
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_SPINDLE_01_BIT_EJECT_FAIL);
				}
			}
			else
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
				{
					if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT))
					{
						m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
						break;
					}
				}
				else
				{
					if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT))
					{
						m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
						break;
					}
				}

				if (pmCLOSE != m_pSolSpdChuckOC_R->GetPos(500))
					m_pSolSpdChuckOC_R->Actuate(pmCLOSE);
				else
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_SPINDLE_02_BIT_EJECT_FAIL);
				}
			}
		}
		else
		{
			if (ROUTER_F == m_fsm.GetMsg())
			{
				if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT))
				{
					m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
					break;
				}

				if (pmCLOSE != m_pSolSpdChuckOC_F->GetPos(500))
					m_pSolSpdChuckOC_F->Actuate(pmCLOSE);
				else
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_SPINDLE_03_BIT_EJECT_FAIL);
				}
			}
			else
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
				{
					if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT))
					{
						m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
						break;
					}
				}
				else
				{
					if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_EJECT))
					{
						m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_EJECT);
						break;
					}
				}

				if (pmCLOSE != m_pSolSpdChuckOC_R->GetPos(500))
					m_pSolSpdChuckOC_R->Actuate(pmCLOSE);
				else
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_SPINDLE_04_BIT_EJECT_FAIL);
				}
			}
		}
		break;
	case C_SPD_BIT_EJECT_END:
		m_nBitAlignRetry = 0;
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CRouter::CycleRunSpdBitClamp(void)
{
	if(!m_fsm.Between(C_SPD_BIT_CLAMP_START, C_SPD_BIT_CLAMP_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_SPD_BIT_CLAMP_CYCLE_TM_OVER_01 + (m_nId*2) + m_fsm.GetMsg());
		return;
	}

	int nIdx = SPINDLE_IDLE;

	if (ROUTER_PART_F == m_nId)
	{
		if (ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_01;
		else
			nIdx = SPINDLE_02;
	}
	else
	{
		if (ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_03;
		else
			nIdx = SPINDLE_04;
	}

	if(C_SPD_BIT_CLAMP_03 == m_fsm.Get())
	{
		BOOL isBitPressErr = FALSE;

		if(ROUTER_PART_F == m_nId)
		{
			if(ROUTER_F == m_fsm.GetMsg())
				isBitPressErr = g_dIn.AOn(iIndex01BitPressCheck01);
			else
				isBitPressErr = g_dIn.AOn(iIndex01BitPressCheck02);

			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
			{
				if (ROUTER_F == m_fsm.GetMsg())
					isBitPressErr = g_dIn.AOn(iIndex02BitPressCheck01);
				else
					isBitPressErr = g_dIn.AOn(iIndex02BitPressCheck02);
			}
		}
		else
		{
			if(ROUTER_F == m_fsm.GetMsg())
				isBitPressErr = g_dIn.AOn(iIndex04BitPressCheck01);
			else
				isBitPressErr = g_dIn.AOn(iIndex04BitPressCheck02);

			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
			{
				if (ROUTER_F == m_fsm.GetMsg())
					isBitPressErr = g_dIn.AOn(iIndex03BitPressCheck01);
				else
					isBitPressErr = g_dIn.AOn(iIndex03BitPressCheck02);
			}
		}

		if(isBitPressErr)
		{
			m_pMtZ_F->Stop(FALSE);
			m_pMtZ_R->Stop(FALSE);
			m_fsm.Set(C_SPD_BIT_CLAMP_ERR);
			return;
		}
	}

	if(!IsMtRdy())
		return;

	if(ROUTER_PART_F == m_nId)
	{
		if (ROUTER_F == m_fsm.GetMsg())
		{
			if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
				return;
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
			{
				if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
					return;
			}
			else
			{
				if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
					return;
			}
		}
	}
	else
	{
		if (ROUTER_F == m_fsm.GetMsg())
		{
			// Index04
			if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
				return;
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
			{
				if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
					return;
			}
			else
			{
				if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
					return;
			}
		}
	}

	switch(m_fsm.Get())
	{
	case C_SPD_BIT_CLAMP_START:
		if(m_fsm.Once())
		{
			/*if(g_pNV->UseSkip(usSecsGem))
			g_pNV->NDm(gemBitChange01Start + nIdx) = STATE_REQ;*/

			g_pNV->NDm(gemBitChange01Start + nIdx) = STATE_REQ;

			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
			{
				m_pCylBitClampUD->Actuate(pmUP);
				break;
			}

			if(pmOFF != m_pSpindleF->GetPos(1000) || pmOFF != m_pSpindleR->GetPos(1000))
			{
				m_pSpindleF->Actuate(pmOFF);
				m_pSpindleR->Actuate(pmOFF);
				break;
			}

			if(ROUTER_F == m_fsm.GetMsg())
			{
				if(!IsGentryMtYWRdy(PY_SPD_BIT_VERIFY_F, PW_READY))
				{
					GentryMtYWMove(PY_SPD_BIT_VERIFY_F, PW_READY);
					break;
				}
				if(!m_pMtZ_F->InPos(PZ_SPD_BIT_VERIFY))
				{
					m_pMtZ_F->Move(PZ_SPD_BIT_VERIFY);
					break;
				}
			}
			else
			{
				if(!IsGentryMtYWRdy(PY_SPD_BIT_VERIFY_R, PW_READY))
				{
					GentryMtYWMove(PY_SPD_BIT_VERIFY_R, PW_READY);
					break;
				}
				if(!m_pMtZ_R->InPos(PZ_SPD_BIT_VERIFY))
				{
					m_pMtZ_R->Move(PZ_SPD_BIT_VERIFY);
					break;
				}
			}

			m_fsm.Set(C_SPD_BIT_CLAMP_01);
		}
		break;
	case C_SPD_BIT_CLAMP_01:
		if(!m_fsm.Delay(300))
			break;

		if(g_opr.isDryRun)
		{
			m_fsm.Set(C_SPD_BIT_CLAMP_02);
			break;
		}

		if(ROUTER_PART_F == m_nId)
		{
			if(g_dIn.AOn(iRouterBitBrokenCheck01))
			{
				m_pMtZ_F->Move(PZ_READY);
				m_pMtZ_R->Move(PZ_READY);

				if(ROUTER_F == m_fsm.GetMsg())
					m_fsm.Set(C_ERROR, ER_SPINDLE_01_BIT_CHANGE_BUT_BIT_NOT_EMPTY);
				else
					m_fsm.Set(C_ERROR, ER_SPINDLE_02_BIT_CHANGE_BUT_BIT_NOT_EMPTY);

				break;
			}
		}
		else
		{
			if(g_dIn.AOn(iRouterBitBrokenCheck02))
			{
				m_pMtZ_F->Move(PZ_READY);
				m_pMtZ_R->Move(PZ_READY);

				if(ROUTER_F == m_fsm.GetMsg())
					m_fsm.Set(C_ERROR, ER_SPINDLE_03_BIT_CHANGE_BUT_BIT_NOT_EMPTY);
				else
					m_fsm.Set(C_ERROR, ER_SPINDLE_04_BIT_CHANGE_BUT_BIT_NOT_EMPTY);

				break;
			}
		}

		m_fsm.Set(C_SPD_BIT_CLAMP_02);
		break;
	case C_SPD_BIT_CLAMP_02:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
			{
				m_pCylBitClampUD->Actuate(pmUP);
				break;
			}

			if(pmOFF != m_pSpindleF->GetPos(1000) || pmOFF != m_pSpindleR->GetPos(1000))
			{
				m_pSpindleF->Actuate(pmOFF);
				m_pSpindleR->Actuate(pmOFF);
				break;
			}
			// Index에 Bit가 몇개 남았는지 확인 한 후에 Clamp 할 수 있도록 한다.
			if(ROUTER_PART_F == m_nId)
			{
				// 확인 후 삭제
				//if(!m_pIndexF->CylIndexMaskFixAct(pmCLOSE))
				//	break;

				if(ROUTER_F == m_fsm.GetMsg())
				{
					if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_CLAMP_F) ||
						!m_pIndexF->m_pMtT->InPos(CIndex::PT_SPD_BIT_CLAMP_F)) 
					{	
						m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_CLAMP_F);
						m_pIndexF->m_pMtT->Move(CIndex::PT_SPD_BIT_CLAMP_F);
						break;
					}

					if(!IsGentryMtYWRdy(PY_SPD_BIT_CLAMP_01_03_RED_F, PW_READY))
					{
						GentryMtYWMove(PY_SPD_BIT_CLAMP_01_03_RED_F, PW_READY);
						break;
					}

					if(pmOPEN != m_pSolSpdChuckOC_F->GetPos(300))
					{
						m_pSolSpdChuckOC_F->Actuate(pmOPEN);
						break;
					}
				}
				else
				{
					if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
					{
						if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_CLAMP_R) ||
							!m_pIndexR->m_pMtT->InPos(CIndex::PT_SPD_BIT_CLAMP_R))
						{
							m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_CLAMP_R);
							m_pIndexR->m_pMtT->Move(CIndex::PT_SPD_BIT_CLAMP_R);
							break;
						}
					}
					else
					{
						if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_CLAMP_R) ||
							!m_pIndexF->m_pMtT->InPos(CIndex::PT_SPD_BIT_CLAMP_R))
						{
							m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_CLAMP_R);
							m_pIndexF->m_pMtT->Move(CIndex::PT_SPD_BIT_CLAMP_R);
							break;
						}
					}

					if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
					{
						if (!IsGentryMtYWRdy(PY_SPD_BIT_CLAMP_BLUE_02_R, PW_READY))
						{
							GentryMtYWMove(PY_SPD_BIT_CLAMP_BLUE_02_R, PW_READY);
							break;
						}
					}
					else
					{
						if (!IsGentryMtYWRdy(PY_SPD_BIT_CLAMP_02_04_RED_R, PW_READY))
						{
							GentryMtYWMove(PY_SPD_BIT_CLAMP_02_04_RED_R, PW_READY);
							break;
						}
					}

					if(pmOPEN != m_pSolSpdChuckOC_R->GetPos(300))
					{
						m_pSolSpdChuckOC_R->Actuate(pmOPEN);
						break;
					}
				}
			}
			else
			{
				// 확인 후 삭제
				//if(!m_pIndexR->CylIndexMaskFixAct(pmCLOSE))
				//	break;

				if(ROUTER_F == m_fsm.GetMsg())
				{
					if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_CLAMP_F) ||
						!m_pIndexR->m_pMtT->InPos(CIndex::PT_SPD_BIT_CLAMP_F)) 
					{	
						m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_CLAMP_F);
						m_pIndexR->m_pMtT->Move(CIndex::PT_SPD_BIT_CLAMP_F);
						break;
					}

					if (!IsGentryMtYWRdy(PY_SPD_BIT_CLAMP_01_03_RED_F, PW_READY))
					{
						GentryMtYWMove(PY_SPD_BIT_CLAMP_01_03_RED_F, PW_READY);
						break;
					}

					if (pmOPEN != m_pSolSpdChuckOC_F->GetPos(300))
					{
						m_pSolSpdChuckOC_F->Actuate(pmOPEN);
						break;
					}
				}
				else
				{
					if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
					{
						if (!m_pIndexF->m_pMtX->InPos(CIndex::PX_SPD_BIT_CLAMP_R) ||
							!m_pIndexF->m_pMtT->InPos(CIndex::PT_SPD_BIT_CLAMP_R))
						{
							m_pIndexF->m_pMtX->Move(CIndex::PX_SPD_BIT_CLAMP_R);
							m_pIndexF->m_pMtT->Move(CIndex::PT_SPD_BIT_CLAMP_R);
							break;
						}
					}
					else
					{
						if (!m_pIndexR->m_pMtX->InPos(CIndex::PX_SPD_BIT_CLAMP_R) ||
							!m_pIndexR->m_pMtT->InPos(CIndex::PT_SPD_BIT_CLAMP_R))
						{
							m_pIndexR->m_pMtX->Move(CIndex::PX_SPD_BIT_CLAMP_R);
							m_pIndexR->m_pMtT->Move(CIndex::PT_SPD_BIT_CLAMP_R);
							break;
						}
					}

					if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
					{
						if (!IsGentryMtYWRdy(PY_SPD_BIT_CLAMP_BLUE_03_R, PW_READY))
						{
							GentryMtYWMove(PY_SPD_BIT_CLAMP_BLUE_03_R, PW_READY);
							break;
						}
					}
					else
					{
						if (!IsGentryMtYWRdy(PY_SPD_BIT_CLAMP_02_04_RED_R, PW_READY))
						{
							GentryMtYWMove(PY_SPD_BIT_CLAMP_02_04_RED_R, PW_READY);
							break;
						}
					}

					if(pmOPEN != m_pSolSpdChuckOC_R->GetPos(300))
					{
						m_pSolSpdChuckOC_R->Actuate(pmOPEN);
						break;
					}
				}
			}

			m_fsm.Set(C_SPD_BIT_CLAMP_03);
		}
		break;
	case C_SPD_BIT_CLAMP_03:
		if(m_fsm.Once())
		{
			if (ROUTER_F == m_fsm.GetMsg())
			{
				if (m_pInfoBitF->nExist && nIdx == SPINDLE_01)
				{
					m_fsm.Set(C_ERROR, ER_SPINDLE_01_BIT_CHANGE_BUT_BIT_NOT_EMPTY);
					break;
				}
				else if (m_pInfoBitF->nExist && nIdx == SPINDLE_03)
				{
					m_fsm.Set(C_ERROR, ER_SPINDLE_02_BIT_CHANGE_BUT_BIT_NOT_EMPTY);
					break;
				}

				MoveMtSpindleZOverrideF(PZ_SPD_BIT_CLAMP_RED);

			}
			else
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
				{
					if (m_pInfoBitR->nExist)
					{
						m_fsm.Set(C_ERROR, ER_SPINDLE_02_BIT_CHANGE_BUT_BIT_NOT_EMPTY);
						break;
					}

					MoveMtSpindleZOverrideR(PZ_SPD_BIT_CLAMP_BLUE);
				}

				else if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
				{
					if (m_pInfoBitR->nExist)
					{
						m_fsm.Set(C_ERROR, ER_SPINDLE_04_BIT_CHANGE_BUT_BIT_NOT_EMPTY);
						break;
					}

					MoveMtSpindleZOverrideR(PZ_SPD_BIT_CLAMP_BLUE);
				}
				else
				{
					if (m_pInfoBitR->nExist && nIdx == SPINDLE_02)
					{
						m_fsm.Set(C_ERROR, ER_SPINDLE_02_BIT_CHANGE_BUT_BIT_NOT_EMPTY);
						break;
					}
					else if (m_pInfoBitR->nExist && nIdx == SPINDLE_04)
					{
						m_fsm.Set(C_ERROR, ER_SPINDLE_04_BIT_CHANGE_BUT_BIT_NOT_EMPTY);
						break;
					}

					MoveMtSpindleZOverrideR(PZ_SPD_BIT_CLAMP_RED);
				}
			}
		}
		else
		{
			if(ROUTER_F == m_fsm.GetMsg())
			{
				if(pmCLOSE != m_pSolSpdChuckOC_F->GetPos(1500))
				{
					m_pSolSpdChuckOC_F->Actuate(pmCLOSE);
					break;
				}
			}
			else
			{
				if(pmCLOSE != m_pSolSpdChuckOC_R->GetPos(1500))
				{
					m_pSolSpdChuckOC_R->Actuate(pmCLOSE);
					break;
				}
			}

			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);

			m_fsm.Set(C_SPD_BIT_CLAMP_04);
		}
		break;
	case C_SPD_BIT_CLAMP_04:
		if(ROUTER_PART_F == m_nId)
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
			{
				if (ROUTER_F == m_fsm.GetMsg())
				{
					if (g_dIn.BOn(iIndex02BitAlignExist01))
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_02_EXIST_01);
						break;
					}
				}
				else
				{
					if (g_dIn.BOn(iIndex02BitAlignExist02))
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_02_EXIST_02);
						break;
					}
				}
			}

			if(ROUTER_F == m_fsm.GetMsg())
			{
				if(g_dIn.BOn(iIndex01BitAlignExist01))
				{
					m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_01_EXIST_01);
					break;
				}
			}
			else
			{
				if(g_dIn.BOn(iIndex01BitAlignExist02))
				{
					m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_01_EXIST_02);
					break;
				}
			}
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
			{
				if (ROUTER_F == m_fsm.GetMsg())
				{
					if (g_dIn.BOn(iIndex03BitAlignExist01))
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_03_EXIST_01);
						break;
					}
				}
				else
				{
					if (g_dIn.BOn(iIndex03BitAlignExist02))
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_03_EXIST_02);
						break;
					}
				}
			}

			if(ROUTER_F == m_fsm.GetMsg())
			{
				if(g_dIn.BOn(iIndex04BitAlignExist01))
				{
					m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_04_EXIST_01);
					break;
				}
			}
			else
			{
				if(g_dIn.BOn(iIndex04BitAlignExist02))
				{
					m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_04_EXIST_02);
					break;
				}
			}
		}
		m_fsm.Set(C_SPD_BIT_CLAMP_END);
		break;
	case C_SPD_BIT_CLAMP_ERR:
		if(!m_fsm.Delay(1000))
			break;

		if(ROUTER_PART_F == m_nId)
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
			{
				if (ROUTER_F == m_fsm.GetMsg())
					m_fsm.Set(C_ERROR, ER_SPINDLE_BIT_PRESS_DETECT_INDEX_02_BLUE_01);
				else
					m_fsm.Set(C_ERROR, ER_SPINDLE_BIT_PRESS_DETECT_INDEX_02_BLUE_02);
			}

			if(ROUTER_F == m_fsm.GetMsg())
				m_fsm.Set(C_ERROR, ER_SPINDLE_BIT_PRESS_DETECT_INDEX_01_RED_01);
			else
				m_fsm.Set(C_ERROR, ER_SPINDLE_BIT_PRESS_DETECT_INDEX_01_RED_02);
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
			{
				if (ROUTER_F == m_fsm.GetMsg())
					m_fsm.Set(C_ERROR, ER_SPINDLE_BIT_PRESS_DETECT_INDEX_03_BLUE_01);
				else
					m_fsm.Set(C_ERROR, ER_SPINDLE_BIT_PRESS_DETECT_INDEX_03_BLUE_02);
			}

			if(ROUTER_F == m_fsm.GetMsg())
				m_fsm.Set(C_ERROR, ER_SPINDLE_BIT_PRESS_DETECT_INDEX_04_RED_01);
			else
				m_fsm.Set(C_ERROR, ER_SPINDLE_BIT_PRESS_DETECT_INDEX_04_RED_02);
		}

		m_pMtZ_F->Move(PZ_READY);
		m_pMtZ_R->Move(PZ_READY);
		m_fsm.Set(C_ERROR);
		break;
	case C_SPD_BIT_CLAMP_END:
		if(ROUTER_F == m_fsm.GetMsg())
		{
			m_pInfoBitF->nExist	 = TRUE;
			ExistCylBitAlignRedF() = FALSE;
			m_pInfoBitF->nLength = 0;
			m_pInfoBitF->nZStep	 = 0;

			if(g_pNV->UseSkip(usBitVision))
				m_bNeedBitVisionF = TRUE;
			if(g_pNV->UseSkip(usSpindleESDCheck))
			{
				if(0 < flagSpindleESDCheckF())
				{
					flagSpindleESDCheckF() = 0;
					m_bNeedESDCheckF = TRUE;
				}
			}
		}
		else
		{
			if(g_pNV->UseSkip(usBitVision))
				m_bNeedBitVisionR = TRUE;
			if(g_pNV->UseSkip(usSpindleESDCheck))
			{
				if(0 < flagSpindleESDCheckR())
				{
					flagSpindleESDCheckR() = 0;
					m_bNeedESDCheckR = TRUE;
				}
			}

			m_pInfoBitR->nExist	= TRUE;
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
				ExistCylBitAlignBlueR() = FALSE;
			else if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
				ExistCylBitAlignBlueR() = FALSE;
			else
				ExistCylBitAlignRedR() = FALSE;

			m_pInfoBitR->nLength = 0;
			m_pInfoBitR->nZStep	 = 0;
		}

		if(g_pNV->UseSkip(usSecsGem))
			g_pNV->NDm(gemBitChange01End + nIdx) = STATE_REQ;

		m_fsm.Set(C_IDLE);
		break;

	}
}

//-------------------------------------------------------------------
void CRouter::CycleRunSpdESDCheck(void)
{
	if(!m_fsm.Between(C_SPD_ESD_CHECK_START, C_SPD_ESD_CHECK_END))
		return;

	int nSpindleNo = (m_nId*2) + m_fsm.GetMsg(); // 0, 1, 2, 3

	if(m_fsm.TimeLimit(180000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_SPD_ESD_CHECK_CYCLE_TM_OVER_01 + nSpindleNo);
		return;
	}

	if(!IsMtRdy())
		return;

	switch(m_fsm.Get())
	{
	case C_SPD_ESD_CHECK_START:
		if(m_fsm.Once())
		{
			m_nESDRetry = 0;
			m_nErrESD = 0;
			g_pNV->NDm(spindleESDNo) = nSpindleNo + 1; // 1, 2, 3, 4
			g_pNV->NDm(stateSpindleESDCheck) = STATE_IDLE;

			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
			{
				m_pCylBitClampUD->Actuate(pmUP);
				break;
			}
			else if(pmOFF != m_pSpindleF->GetPos(7000) || pmOFF != m_pSpindleR->GetPos(7000))
			{
				m_pSpindleF->Actuate(pmOFF);
				m_pSpindleR->Actuate(pmOFF);
				break;
			}
			else
			{
				if(ROUTER_F == m_fsm.GetMsg())
				{
					if(!IsGentryMtYWRdy(PY_SPD_WIRE_CHECK_F, PW_READY))
					{
						GentryMtYWMove(PY_SPD_WIRE_CHECK_F, PW_READY);
						break;
					}
				}
				else
				{
					if(!IsGentryMtYWRdy(PY_SPD_WIRE_CHECK_R, PW_READY))
					{
						GentryMtYWMove(PY_SPD_WIRE_CHECK_R, PW_READY);
						break;
					}
				}
				m_fsm.Set(C_SPD_ESD_CHECK_01);
			}
		}
		break;
	case C_SPD_ESD_CHECK_01:
		if(m_fsm.Once())
		{
			// Retray를 위한 Ready Pos이동
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(ROUTER_F == m_fsm.GetMsg())
			{
				if(!IsReadyMtSpindleZOverrideF(PZ_SPD_EDS_CHECK))
				{
					MoveMtSpindleZOverrideF(PZ_SPD_EDS_CHECK);
					break;
				}
			}
			else
			{
				if(!IsReadyMtSpindleZOverrideR(PZ_SPD_EDS_CHECK))
				{
					MoveMtSpindleZOverrideR(PZ_SPD_EDS_CHECK);
					break;
				}
			}
			m_fsm.Set(C_SPD_ESD_CHECK_02);
		}
		break;
	case C_SPD_ESD_CHECK_02:
		if(g_opr.isDryRun)
		{
			m_fsm.Set(C_SPD_ESD_CHECK_03);
			break;
		}

		if(!m_fsm.Delay(500))
			break;

		if(m_fsm.Once())
		{
			g_pNV->NDm(spindleESDNo) = nSpindleNo + 1; // 1, 2, 3, 4
			g_pNV->NDm(stateSpindleESDCheck) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(7000))
			{
				m_pMtZ_F->Move(PZ_READY);
				m_pMtZ_R->Move(PZ_READY);
				m_fsm.Set(C_ERROR, ER_ESD_CHECK_READ_TM_OVER);
				break;
			}

			switch(g_pNV->NDm(stateSpindleESDCheck))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				g_pNV->NDm(stateSpindleESDCheck) = STATE_IDLE;
				g_pNV->NDm(spindleESDNo) = 0;
				m_fsm.Set(C_SPD_ESD_CHECK_03);
				break;
			case STATE_ERR:
				m_nErrESD = ER_ESD_CHECK_READ_FAIL_01 + nSpindleNo;
				m_fsm.Set(C_SPD_ESD_CHECK_RETRY);
				break;
			}
		}
		break;
	case C_SPD_ESD_CHECK_03:
		if(g_opr.isDryRun)
		{
			if(ROUTER_F == m_fsm.GetMsg())
			{
				m_bNeedESDCheckF = FALSE;
				flagSpindleESDCheckF() = 0;
			}
			else
			{
				m_bNeedESDCheckR = FALSE;
				flagSpindleESDCheckR() = 0;
			}
			m_fsm.Set(C_SPD_ESD_CHECK_END);
			break;
		}

		if(g_pNV->DDm(mmiReadSpindleResistance) > g_pNV->DDm(spindleResistanceLimit))
		{
			m_nErrESD = ER_ESD_CHECK_VALUE_LIMIT_01 + nSpindleNo;
			m_fsm.Set(C_SPD_ESD_CHECK_RETRY);
			break;
		}

		if(g_pNV->DDm(mmiReadSpindleResistance) < g_pNV->DDm(spindleResistanceLowLimit))
		{
			m_nErrESD = ER_ESD_CHECK_VALUE_LIMIT_01 + nSpindleNo;
			m_fsm.Set(C_SPD_ESD_CHECK_RETRY);
			break;
		}

		if(ROUTER_F == m_fsm.GetMsg())
		{
			m_bNeedESDCheckF = FALSE;
			flagSpindleESDCheckF()  = 0;
		}
		else
		{
			m_bNeedESDCheckR = FALSE;
			flagSpindleESDCheckR() = 0;
		}
		m_fsm.Set(C_SPD_ESD_CHECK_END);
		break;
	case C_SPD_ESD_CHECK_RETRY:
		m_nESDRetry++;
		if(3 < m_nESDRetry)
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
			m_fsm.Set(C_ERROR, m_nErrESD);
			break;
		}
		else
		{
			m_fsm.Set(C_SPD_ESD_CHECK_01);
		}
		break;

	case C_SPD_ESD_CHECK_END:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			m_fsm.Set(C_IDLE);
		}
		break;
	}
}


//-------------------------------------------------------------------
void CRouter::CycleRunCylBitClamp(void)
{
	if(!m_fsm.Between(C_CYL_BIT_CLAMP_START, C_CYL_BIT_CLAMP_END))
		return;

	if(m_fsm.TimeLimit(180000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_CYL_BIT_CLAMP_CYCLE_TM_OVER_F + m_fsm.GetMsg());
		return;
	}

	if(!IsMtRdy())
		return;

	int nIdx = SPINDLE_IDLE;

	if (ROUTER_PART_F == m_nId)
	{
		if (ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_01;
		else
			nIdx = SPINDLE_02;
	}
	else
	{
		if (ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_03;
		else
			nIdx = SPINDLE_04;
	}

	if(ROUTER_PART_F == m_nId)
	{
		if (ROUTER_F == m_fsm.GetMsg())
		{
			// Index01
			if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
				return;
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
			{
				if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
					return;
			}
			else
			{
				if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
					return;
			}
		}
	}
	else
	{
		if (ROUTER_F == m_fsm.GetMsg())
		{
			// Index04
			if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
				return;
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
			{
				if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
					return;
			}
			else
			{
				if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
					return;
			}
		}
	}

	switch(m_fsm.Get())
	{
	case C_CYL_BIT_CLAMP_START:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
			{
				m_pCylBitClampUD->Actuate(pmUP);
				break;
			}
			if(pmOPEN != m_pCylBitClampOC->GetPos(300))
			{
				m_pCylBitClampOC->Actuate(pmOPEN);
				break;
			}

			if(pmOFF != m_pSpindleF->GetPos(1000) || pmOFF != m_pSpindleR->GetPos(1000))
			{
				m_pSpindleF->Actuate(pmOFF);
				m_pSpindleR->Actuate(pmOFF);
				break;
			}
			// Index에 Bit가 몇개 남았는지 확인 한 후에 Clamp 할 수 있도록 한다.
			// 추후에 배열로 수정
			if(ROUTER_PART_F == m_nId)
			{
				// 확인 후 삭제
				//if(!m_pIndexF->CylIndexMaskFixAct(pmCLOSE))
				//	break;

				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
				{
					int nBitCurCnt = g_pNV->NDm(BlueindexBitBoxCurCnt02);
					POINT2D ptXY = m_pIndexR->GetBitBoxSupplyPos(nBitCurCnt);

					m_pIndexR->m_pMtX->PMove(CIndex::PX_CYL_BIT_SUPPLY_BOX, ptXY.dX);
					m_pIndexR->m_pMtT->Move(CIndex::PT_CYL_BIT_SUPPLY_BOX);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_CYL_BIT_CLAMP_BLUE, PW_READY, ptXY.dY, dPosW);
				}
				else
				{
					int nBitCurCnt = g_pNV->NDm(RedindexBitBoxCurCnt01);
					POINT2D ptXY = m_pIndexF->GetBitBoxSupplyPos(nBitCurCnt);

					m_pIndexF->m_pMtX->PMove(CIndex::PX_CYL_BIT_SUPPLY_BOX, ptXY.dX);
					m_pIndexF->m_pMtT->Move(CIndex::PT_CYL_BIT_SUPPLY_BOX);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_CYL_BIT_CLAMP_RED, PW_READY, ptXY.dY, dPosW);
				}
			}
			else
			{
				// 확인 후 삭제
				//if(!m_pIndexR->CylIndexMaskFixAct(pmCLOSE))
				//	break;

				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
				{
					int nBitCurCnt = g_pNV->NDm(BlueindexBitBoxCurCnt03);
					POINT2D ptXY = m_pIndexF->GetBitBoxSupplyPos(nBitCurCnt);

					m_pIndexF->m_pMtX->PMove(CIndex::PX_CYL_BIT_SUPPLY_BOX, ptXY.dX);
					m_pIndexF->m_pMtT->Move(CIndex::PT_CYL_BIT_SUPPLY_BOX);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_CYL_BIT_CLAMP_BLUE, PW_READY, ptXY.dY, dPosW);
				}
				else
				{
					int nBitCurCnt = g_pNV->NDm(RedindexBitBoxCurCnt04);
					POINT2D ptXY = m_pIndexR->GetBitBoxSupplyPos(nBitCurCnt);

					m_pIndexR->m_pMtX->PMove(CIndex::PX_CYL_BIT_SUPPLY_BOX, ptXY.dX);
					m_pIndexR->m_pMtT->Move(CIndex::PT_CYL_BIT_SUPPLY_BOX);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_CYL_BIT_CLAMP_RED, PW_READY, ptXY.dY, dPosW);
				}
			}

			m_fsm.Set(C_CYL_BIT_CLAMP_01);
		}
		break;

	case C_CYL_BIT_CLAMP_01:
		if(pmDOWN != m_pCylBitClampUD->GetPos(500))
		{
			m_pCylBitClampUD->Actuate(pmDOWN);
			break;
		}
		if(pmCLOSE != m_pCylBitClampOC->GetPos(500))
		{
			m_pCylBitClampOC->Actuate(pmCLOSE);
			break;
		}

		ExistCylBitClamp() = TRUE;
		if (ROUTER_PART_F == m_nId)
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
				g_pNV->NDm(BlueindexBitBoxCurCnt02)--;
			else
				g_pNV->NDm(RedindexBitBoxCurCnt01)--;
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
				g_pNV->NDm(BlueindexBitBoxCurCnt03)--;
			else
				g_pNV->NDm(RedindexBitBoxCurCnt04)--;
		}

		if (g_pNV->UseSkip(usBitColor))
		{
			if (ROUTER_PART_F == m_nId)
			{
				if (ROUTER_F == m_fsm.GetMsg())
				{
					m_bNeedBitColorF = TRUE;
				}
				else
				{
					m_bNeedBitColorR = TRUE;
				}
			}
			else
			{
				if (ROUTER_F == m_fsm.GetMsg())
				{
					m_bNeedBitColorF = TRUE;
				}
				else
				{
					m_bNeedBitColorR = TRUE;
				}
			}
		}
		m_fsm.Set(C_CYL_BIT_CLAMP_END);
		break;

	case C_CYL_BIT_CLAMP_END:
		if(pmUP != m_pCylBitClampUD->GetPos(300))
		{
			m_pCylBitClampUD->Actuate(pmUP);
			break;
		}
		m_fsm.Set(C_IDLE);
		break;
	}
}

//-------------------------------------------------------------------
void CRouter::CycleRunCylColorClamp(void)
{
	if(!m_fsm.Between(C_CYL_COLOR_CLAMP_START, C_CYL_COLOR_CLAMP_END))
		return;

	if(m_fsm.TimeLimit(60000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_CYL_BIT_CLAMP_COLOR_CHECK_CYCLE_TM_OVER);
		return;
	}

	//if(!IsMtRdy())
	//	return;

	int nIdx = SPINDLE_IDLE;

	if (ROUTER_PART_F == m_nId)
	{
		if (ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_01;
		else
			nIdx = SPINDLE_02;
	}
	else
	{
		if (ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_03;
		else
			nIdx = SPINDLE_04;
	}

	switch(m_fsm.Get())
	{
	case C_CYL_COLOR_CLAMP_START:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if (ROUTER_PART_F == m_nId)
			{
				if (!m_pMtY->InPos(PY_SPD_BIT_COLOR_F))
				{
					GentryMtYWMove(PY_SPD_BIT_COLOR_F, PW_READY);
					break;
				}
			}
			else
			{
				if (!m_pMtY->InPos(PY_SPD_BIT_COLOR_R))
				{
					GentryMtYWMove(PY_SPD_BIT_COLOR_R, PW_READY);
					break;
				}
			}

			if(pmDOWN != m_pCylBitClampUD->GetPos(500))
			{
				m_pCylBitClampUD->Actuate(pmDOWN);
				break;
			}

			m_fsm.Set(C_CYL_COLOR_CLAMP_01);
		}
		break;

	case C_CYL_COLOR_CLAMP_01:
		{
			if (!m_fsm.Delay(2000))
				break;

			bool bColor = false;

			if (ROUTER_PART_F == m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
			{
				if (g_dIn.AOn(iFrontBitColorBlue, 300))
				{
					bColor = true;
				}
			}
			else if (ROUTER_PART_R == m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
			{
				if (g_dIn.AOn(iRearBitColorBlue, 300))
				{					
					bColor = true;
				}
			}
			else
			{
				if (ROUTER_PART_F == m_nId)
				{
					if (g_dIn.AOn(iFrontBitColorRed, 300))
					{
						bColor = true;
					}
				}
				else
				{
					if (g_dIn.AOn(iRearBitColorRed, 300))
					{
						bColor = true;
					}
				}
			}

			if (!bColor)
			{
				m_pCylBitClampUD->Actuate(pmUP);
				m_fsm.Set(C_ERROR, ER_ROUTER_CYL_BIT_CLAMP_COLOR_CHECK_FAIL_F + m_nId);
				break;
			}

			m_fsm.Set(C_CYL_COLOR_CLAMP_END);
		}
		break;

	case C_CYL_COLOR_CLAMP_END:
		if(pmUP != m_pCylBitClampUD->GetPos(300))
		{
			m_pCylBitClampUD->Actuate(pmUP);
			break;
		}

		if(ROUTER_F == m_fsm.GetMsg())
		{
			m_bNeedBitColorF = FALSE;
		}
		else
		{
			m_bNeedBitColorR = FALSE;
		}

		m_fsm.Set(C_IDLE);
		break;
	}
}

//-------------------------------------------------------------------
void CRouter::CycleRunCylBitAlign(void)
{
	if(!m_fsm.Between(C_CYL_BIT_ALIGN_START, C_CYL_BIT_ALIGN_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_CYL_BIT_ALIGN_CYCLE_TM_OVER_01 + (m_nId*2) + m_fsm.GetMsg());
		return;
	}

	if(!IsMtRdy())
		return;

	int nIdx = SPINDLE_IDLE;

	if (ROUTER_PART_F == m_nId)
	{
		if (ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_01;
		else
			nIdx = SPINDLE_02;
	}
	else
	{
		if (ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_03;
		else
			nIdx = SPINDLE_04;
	}

	if(ROUTER_PART_F == m_nId)
	{
		if (ROUTER_F == m_fsm.GetMsg())
		{
			// Index01
			if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
				return;
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
			{
				if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
					return;
			}
			else
			{
				if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
					return;
			}
		}
	}
	else
	{
		if (ROUTER_F == m_fsm.GetMsg())
		{
			// Index04
			if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
				return;
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
			{
				if (!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
					return;
			}
			else
			{
				if (!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
					return;
			}
		}
	}

	switch(m_fsm.Get())
	{
	case C_CYL_BIT_ALIGN_START:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
			{
				m_pCylBitClampUD->Actuate(pmUP);
				break;
			}

			if(pmOFF != m_pSpindleF->GetPos(1000) || pmOFF != m_pSpindleR->GetPos(1000))
			{
				m_pSpindleF->Actuate(pmOFF);
				m_pSpindleR->Actuate(pmOFF);
				break;
			}

			if(ROUTER_PART_F == m_nId)
			{
				// 확인 후 삭제 삭제해도 될듯.
				//if(!m_pIndexF->CylIndexMaskFixAct(pmCLOSE))
				//	break;

				if(ROUTER_F == m_fsm.GetMsg())
				{
					m_pIndexF->m_pMtX->Move(CIndex::PX_CYL_BIT_ALIGN_F);
					m_pIndexF->m_pMtT->Move(CIndex::PT_CYL_BIT_ALIGN_F);
					GentryMtYWMove(PY_CYL_BIT_ALIGN_RED_F, PW_READY);
				}
				else
				{
					if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
					{
						m_pIndexR->m_pMtX->Move(CIndex::PX_CYL_BIT_ALIGN_R);
						m_pIndexR->m_pMtT->Move(CIndex::PT_CYL_BIT_ALIGN_R);
					}
					else
					{
						m_pIndexF->m_pMtX->Move(CIndex::PX_CYL_BIT_ALIGN_R);
						m_pIndexF->m_pMtT->Move(CIndex::PT_CYL_BIT_ALIGN_R);
					}

					if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
						GentryMtYWMove(PY_CYL_BIT_ALIGN_BLUE_R, PW_READY);
					else
						GentryMtYWMove(PY_CYL_BIT_ALIGN_RED_R, PW_READY);
				}
			}
			else
			{
				// 확인 후 삭제 삭제해도 될듯.
				//if(!m_pIndexR->CylIndexMaskFixAct(pmCLOSE))
				//	break;

				if(ROUTER_F == m_fsm.GetMsg())
				{
					m_pIndexR->m_pMtX->Move(CIndex::PX_CYL_BIT_ALIGN_F);
					m_pIndexR->m_pMtT->Move(CIndex::PT_CYL_BIT_ALIGN_F);
					GentryMtYWMove(PY_CYL_BIT_ALIGN_RED_F, PW_READY);
				}
				else
				{
					if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
					{
						m_pIndexF->m_pMtX->Move(CIndex::PX_CYL_BIT_ALIGN_R);
						m_pIndexF->m_pMtT->Move(CIndex::PT_CYL_BIT_ALIGN_R);
					}
					else
					{
						m_pIndexR->m_pMtX->Move(CIndex::PX_CYL_BIT_ALIGN_R);
						m_pIndexR->m_pMtT->Move(CIndex::PT_CYL_BIT_ALIGN_R);
					}

					if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
						GentryMtYWMove(PY_CYL_BIT_ALIGN_BLUE_R, PW_READY);
					else
						GentryMtYWMove(PY_CYL_BIT_ALIGN_RED_R, PW_READY);
				}
			}
			m_fsm.Set(C_CYL_BIT_ALIGN_01);
		}
		break;
	case C_CYL_BIT_ALIGN_01:
		if(ROUTER_PART_F == m_nId)
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
			{
				if (ROUTER_F == m_fsm.GetMsg())
				{
					if (g_dIn.BOn(iIndex02BitAlignExist01) && !g_opr.isDryRun)
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_02_EXIST_01);
						break;
					}
				}
				else
				{
					if (g_dIn.BOn(iIndex02BitAlignExist02) && !g_opr.isDryRun)
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_02_EXIST_02);
						break;
					}
				}
			}
			else
			{
				if (ROUTER_F == m_fsm.GetMsg())
				{
					if (g_dIn.BOn(iIndex01BitAlignExist01) && !g_opr.isDryRun)
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_01_EXIST_01);
						break;
					}
				}
				else
				{
					if (g_dIn.BOn(iIndex01BitAlignExist02) && !g_opr.isDryRun)
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_01_EXIST_02);
						break;
					}
				}
			}
		}
		else
		{
			if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
			{
				if (ROUTER_F == m_fsm.GetMsg())
				{
					if (g_dIn.BOn(iIndex03BitAlignExist01) && !g_opr.isDryRun)
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_03_EXIST_01);
						break;
					}
				}
				else
				{
					if (g_dIn.BOn(iIndex03BitAlignExist02) && !g_opr.isDryRun)
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_03_EXIST_02);
						break;
					}
				}
			}
			else
			{
				if (ROUTER_F == m_fsm.GetMsg())
				{
					if (g_dIn.BOn(iIndex04BitAlignExist01) && !g_opr.isDryRun)
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_04_EXIST_01);
						break;
					}
				}
				else
				{
					if (g_dIn.BOn(iIndex04BitAlignExist02) && !g_opr.isDryRun)
					{
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_04_EXIST_02);
						break;
					}
				}
			}
		}
		m_fsm.Set(C_CYL_BIT_ALIGN_02);
		break;
	case C_CYL_BIT_ALIGN_02:
		if(pmDOWN != m_pCylBitClampUD->GetPos(300))
		{
			m_pCylBitClampUD->Actuate(pmDOWN);
			break;
		}

		if(pmOPEN != m_pCylBitClampOC->GetPos(300))
		{
			m_pCylBitClampOC->Actuate(pmOPEN);
			break;
		}

		m_fsm.Set(C_CYL_BIT_ALIGN_03);
		break;
	case C_CYL_BIT_ALIGN_03:
		if(pmUP != m_pCylBitClampUD->GetPos(300))
		{
			m_pCylBitClampUD->Actuate(pmUP);
			break;
		}

		m_nBitAlignRetry++;

		if(ROUTER_PART_F == m_nId)
		{
			if(ROUTER_F == m_fsm.GetMsg())
			{
				if(!g_dIn.BOn(iIndex01BitAlignExist01) && !g_opr.isDryRun)
				{
					/*
					if(m_nBitAlignRetry < 3)
					{
					// Retry Pickup
					ExistCylBitClamp() = FALSE;
					m_fsm.Set(C_IDLE);
					}
					else
					{
					m_nBitAlignRetry = 0;
					m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_NOT_EXIST_01);
					}
					*/
					m_nBitAlignRetry = 0;
					m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_01_NOT_EXIST_01);
					break;
				}
				ExistCylBitAlignRedF() = TRUE;
				ExistCylBitClamp() = FALSE;
			}
			else
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_02)
				{
					if (!g_dIn.BOn(iIndex02BitAlignExist02) && !g_opr.isDryRun)
					{
						/*
						if(m_nBitAlignRetry < 3)
						{
						// Retry Pickup
						ExistCylBitClamp() = FALSE;
						m_fsm.Set(C_IDLE);
						}
						else
						{
						m_nBitAlignRetry = 0;
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_NOT_EXIST_02);
						}
						*/
						m_nBitAlignRetry = 0;
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_02_NOT_EXIST_02);
						break;
					}

					ExistCylBitAlignBlueR() = TRUE;
					ExistCylBitClamp() = FALSE;
				}
				else 
				{
					if (!g_dIn.BOn(iIndex01BitAlignExist02) && !g_opr.isDryRun)
					{
						/*
						if(m_nBitAlignRetry < 3)
						{
						// Retry Pickup
						ExistCylBitClamp() = FALSE;
						m_fsm.Set(C_IDLE);
						}
						else
						{
						m_nBitAlignRetry = 0;
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_NOT_EXIST_02);
						}
						*/
						m_nBitAlignRetry = 0;
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_01_NOT_EXIST_02);
						break;
					}

					ExistCylBitAlignRedR() = TRUE;
					ExistCylBitClamp() = FALSE;
				}
			}
		}
		else
		{
			if(ROUTER_F == m_fsm.GetMsg())
			{
				if(!g_dIn.BOn(iIndex04BitAlignExist01) && !g_opr.isDryRun)
				{
					/*
					if(m_nBitAlignRetry < 3)
					{
					// Retry Pickup
					ExistCylBitClamp() = FALSE;
					m_fsm.Set(C_IDLE);
					}
					else
					{
					m_nBitAlignRetry = 0;
					m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_NOT_EXIST_03);
					}
					*/
					m_nBitAlignRetry = 0;
					m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_04_NOT_EXIST_01);
					break;
				}

				ExistCylBitAlignRedF() = TRUE;
				ExistCylBitClamp() = FALSE;
			}
			else
			{
				if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE && nIdx == SPINDLE_04)
				{
					if (!g_dIn.BOn(iIndex03BitAlignExist02) && !g_opr.isDryRun)
					{
						/*
						if(m_nBitAlignRetry < 3)
						{
						// Retry Pickup
						ExistCylBitClamp() = FALSE;
						m_fsm.Set(C_IDLE);
						}
						else
						{
						m_nBitAlignRetry = 0;
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_NOT_EXIST_04);
						}
						*/
						m_nBitAlignRetry = 0;
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_03_NOT_EXIST_02);
						break;
					}

					ExistCylBitAlignBlueR() = TRUE;
					ExistCylBitClamp() = FALSE;
				}
				else 
				{
					if (!g_dIn.BOn(iIndex04BitAlignExist02) && !g_opr.isDryRun)
					{
						/*
						if(m_nBitAlignRetry < 3)
						{
						// Retry Pickup
						ExistCylBitClamp() = FALSE;
						m_fsm.Set(C_IDLE);
						}
						else
						{
						m_nBitAlignRetry = 0;
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_NOT_EXIST_04);
						}
						*/
						m_nBitAlignRetry = 0;
						m_fsm.Set(C_ERROR, ER_INDEX_BIT_ALIGN_INDEX_04_NOT_EXIST_02);
						break;
					}

					ExistCylBitAlignRedR() = TRUE;
					ExistCylBitClamp() = FALSE;
				}
			}
		}

		m_fsm.Set(C_CYL_BIT_ALIGN_END);
		break;
	case C_CYL_BIT_ALIGN_END:
		m_nBitAlignRetry = 0;
		m_fsm.Set(C_IDLE);
		break;

	}
}


//-------------------------------------------------------------------
void CRouter::CycleRunSpdBitVision(void)
{
	if(!m_fsm.Between(C_SPD_BIT_VI_START, C_SPD_BIT_VI_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_CYL_SPD_BIT_VI_CYCLE_TM_OVER_01 + (m_nId*2) + m_fsm.GetMsg());
		return;
	}

	if(!IsMtRdy())
		return;

	int nIdx = SPINDLE_IDLE;

	if(ROUTER_PART_F == m_nId)
	{
		if(ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_01;
		else 
			nIdx = SPINDLE_02;
	}
	else
	{
		if(ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_03;
		else 
			nIdx = SPINDLE_04;
	}

	switch(m_fsm.Get())
	{
	case C_SPD_BIT_VI_START:
		if(m_fsm.Once())
		{
			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemBitVision01Start + nIdx) = STATE_REQ;

			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
			{
				m_pCylBitClampUD->Actuate(pmUP);
				break;
			}

			m_fsm.Set(C_SPD_BIT_VI_01);
		}
		break;
	case C_SPD_BIT_VI_01:
		if(g_opr.isDryRun)
		{
			m_fsm.Set(C_SPD_BIT_VI_02);
			break;
		}

		if(0 == m_fsm.GetStep())
		{
			if(!m_fsm.Delay(300))
				break;
			g_dOut.On(oViSpindleBtmRst);
			m_fsm.SetStep(1, TRUE);
		}
		else if(1 == m_fsm.GetStep())
		{
			if(m_fsm.Delay(5000))
			{
				m_fsm.Set(C_ERROR, ER_VI_ROUTER_BTM_NOT_READY);
				break;
			}

			if(!m_fsm.Delay(300))
				break;

			if(g_dIn.AOn(iViSpindleBtmReady) && !g_dIn.AOn(iViSpindleBtmBusy))
			{
				g_dOut.Off(oViSpindleBtmRst);
			}

			m_fsm.Set(C_SPD_BIT_VI_02);
		}
		break;
	case C_SPD_BIT_VI_02:
		if(ROUTER_F == m_fsm.GetMsg())
		{
			if(!IsGentryMtYWRdy(PY_SPD_BIT_VI_F, PW_READY))
				GentryMtYWMove(PY_SPD_BIT_VI_F, PW_READY);
			else if(!IsReadyMtSpindleZOverrideF(PZ_SPD_BIT_VI))
				MoveMtSpindleZOverrideF(PZ_SPD_BIT_VI);
			else
			{
				if(!g_opr.isDryRun)
				{
					//g_dOut.On(oSpindleVel01F);
					//g_dOut.On(oSpindleVel02F);
					//g_dOut.On(oSpindleVel01R);
					//g_dOut.On(oSpindleVel02R);

					if (g_dOut.IsOn(oSpindleReverseRun02) || g_dOut.IsOn(oSpindleReverseRun04))
					{
						g_dOut.Off(oSpindleReverseRun02);
						g_dOut.Off(oSpindleReverseRun04);
						break;
					}

					if(pmON != m_pSpindleF->GetPos(3000) || pmON != m_pSpindleR->GetPos(3000))
					{
						m_pSpindleF->Actuate(pmON);
						m_pSpindleR->Actuate(pmON);
						break;
					}
				}

				m_fsm.Set(C_SPD_BIT_VI_03);
			}
		}
		else
		{
			if(!IsGentryMtYWRdy(PY_SPD_BIT_VI_R, PW_READY))
				GentryMtYWMove(PY_SPD_BIT_VI_R, PW_READY);
			else if(!IsReadyMtSpindleZOverrideR(PZ_SPD_BIT_VI))
				MoveMtSpindleZOverrideR(PZ_SPD_BIT_VI);
			else
			{
				if(!g_opr.isDryRun)
				{
					//g_dOut.On(oSpindleVel01F);
					//g_dOut.On(oSpindleVel02F);
					//g_dOut.On(oSpindleVel01R);
					//g_dOut.On(oSpindleVel02R);

					if (g_dOut.IsOn(oSpindleReverseRun02) || g_dOut.IsOn(oSpindleReverseRun04))
					{
						g_dOut.Off(oSpindleReverseRun02);
						g_dOut.Off(oSpindleReverseRun04);
						break;
					}

					if(pmON != m_pSpindleF->GetPos(3000) || pmON != m_pSpindleR->GetPos(3000))
					{
						m_pSpindleF->Actuate(pmON);
						m_pSpindleR->Actuate(pmON);
						break;
					}
				}

				m_fsm.Set(C_SPD_BIT_VI_03);
			}
		}
		break;
	case C_SPD_BIT_VI_03:
		if(m_fsm.Once())
		{
			g_pNV->DDm(mmiReadViBtmDis) = -999;
			g_pNV->NDm(mmiBtmViErr) = -999;
			g_dOut.On(oViSpindleBtmTrig);
		}
		else
		{
			// Error 발생시 현 위치 대기 하여 Manual 측정 할수 있도록 함.
			if(m_fsm.TimeLimit(7000))
			{
				m_fsm.Set(C_ERROR, ER_VI_ROUTER_BTM_RESULT_TM_OVER);
				break;
			}

			if(!m_fsm.Delay(500))
				break;

			if(g_opr.isDryRun)
			{
				g_dOut.Off(oViSpindleBtmTrig);
				m_fsm.Set(C_SPD_BIT_VI_04);
				break;
			}

			if(g_dIn.AOn(iViSpindleBtmReady) && !g_dIn.AOn(iViSpindleBtmBusy))
			{
				g_dOut.Off(oViSpindleBtmTrig);
			}

			if((-999) == g_pNV->NDm(mmiBtmViErr))
				break;

			if(1 == g_pNV->NDm(mmiBtmViErr))
				m_fsm.Set(C_ERROR, ER_VI_ROUTER_BTM_FAIL);
			else
				m_fsm.Set(C_SPD_BIT_VI_END);
		}
		break;
	case C_SPD_BIT_VI_END:
		if(m_fsm.Once())
		{
			// Broken, 흘러내림 확인은 하지 않는다.
			if(ROUTER_F == m_fsm.GetMsg())
				m_bNeedBitVisionF = FALSE;
			else
				m_bNeedBitVisionR = FALSE;
		}
		else
		{
			if(pmOFF != m_pSpindleF->GetPos(1000) && pmOFF != m_pSpindleR->GetPos(1000))
			{
				m_pSpindleF->Actuate(pmOFF);
				m_pSpindleR->Actuate(pmOFF);
				break;
			}

			if(!m_pMtZ_F->InPos(PZ_READY) || !m_pMtZ_R->InPos(PZ_READY))
			{
				m_pMtZ_F->Move(PZ_READY);
				m_pMtZ_R->Move(PZ_READY);
				break;
			}

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemBitVision01End + nIdx) = STATE_REQ;

			m_fsm.Set(C_IDLE);
		}
		break;
	}
}


//-------------------------------------------------------------------
void CRouter::CycleRunSpdBitVerify(void)
{
	if(!m_fsm.Between(C_SPD_BIT_VERIFY_START, C_SPD_BIT_VERIFY_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_CYL_SPD_BIT_VERIFY_CYCLE_TM_OVER_01 + (m_nId*2) + m_fsm.GetMsg());
		return;
	}

	if(!IsMtRdy())
		return;

	int nIdx = SPINDLE_IDLE;

	if(ROUTER_PART_F == m_nId)
	{
		if(ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_01;
		else 
			nIdx = SPINDLE_02;
	}
	else
	{
		if(ROUTER_F == m_fsm.GetMsg())
			nIdx = SPINDLE_03;
		else 
			nIdx = SPINDLE_04;
	}

	_char cYMoveEventId[_MAX_CHAR_SIZE_], cZMainMoveEventId[_MAX_CHAR_SIZE_], cZSubMoveEventId[_MAX_CHAR_SIZE_];
	_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_BIT_VERIFY_F", (((m_nId % 2) * 2) + 1));
	if (ROUTER_R == m_fsm.GetMsg())
		_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_BIT_VERIFY_R", (((m_nId % 2) * 2) + 1));

	_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_BIT_VERIFY", (((m_nId % 2) * 2) + 1));
	_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_BIT_VERIFY", (((m_nId % 2) * 2) + 2));

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"BIT");	
	_sprintf(cMaterialId, L"$");

	switch(m_fsm.Get())
	{
	case C_SPD_BIT_VERIFY_START:
		if(m_fsm.Once())
		{
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRouter[m_nId].deviceId, L"BIT_VERIFY", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"BIT_VERIFY_START", L"BIT_VERIFY_END"))

				if(g_pNV->UseSkip(usSecsGem))
					g_pNV->NDm(gemBitCheck01Start + nIdx) = STATE_REQ;

			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
			{
				m_pCylBitClampUD->Actuate(pmUP);
				break;
			}

			if(ROUTER_F == m_fsm.GetMsg())
			{
				if(!IsGentryMtYWRdy(PY_SPD_BIT_VERIFY_F, PW_READY))
				{
					GentryMtYWMove(PY_SPD_BIT_VERIFY_F, PW_READY);
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_F]) 
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_F] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_POSIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_POS_], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_POS_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
					}
					break;
				}
				else
				{
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_F]) 
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_F] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_POSIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_POS_], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_F][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_POS_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
					}
				}

				if(!m_pMtZ_F->InPos(PZ_SPD_BIT_VERIFY))
				{
					m_pMtZ_F->Move(PZ_SPD_BIT_VERIFY);
					if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_SPD_BIT_VERIFY]) 
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_SPD_BIT_VERIFY] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_POS_], 
							g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_ACC_]))
					}
					break;
				}
				else
				{
					if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_SPD_BIT_VERIFY]) 
					{
						g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_SPD_BIT_VERIFY] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end,
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_POS_], 
							g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_SPD_], 
							g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_SPD_BIT_VERIFY][_ACC_]))
					}
				}
			}
			else // 리어도 동일
			{
				if(!IsGentryMtYWRdy(PY_SPD_BIT_VERIFY_R, PW_READY))
				{
					GentryMtYWMove(PY_SPD_BIT_VERIFY_R, PW_READY);
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_R]) 
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_R] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_POSIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_POS_], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_POS_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
					}
					break;
				}
				else
				{
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_R]) 
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SPD_BIT_VERIFY_R] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_POSIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_POS_], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_SPD_BIT_VERIFY_R][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY]) 
					{
						g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_READY] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
							g_data2c.cRouter[m_nId].W[PW_READY][_POSIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_POS_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_SPDIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_SPD_], 
							g_data2c.cRouter[m_nId].W[PW_READY][_ACCIDX_], g_data2c.cRouter[m_nId].W[PW_READY][_ACC_]))
					}
				}

				if(!m_pMtZ_R->InPos(PZ_SPD_BIT_VERIFY))
				{
					m_pMtZ_R->Move(PZ_SPD_BIT_VERIFY);
					if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_SPD_BIT_VERIFY]) 
					{
						g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_SPD_BIT_VERIFY] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_POS_], 
							g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_SPD_], 
							g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_ACC_]))
					}
					break;
				}
				else
				{
					if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_SPD_BIT_VERIFY]) 
					{
						g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_SPD_BIT_VERIFY] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_POS_], 
							g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_SPD_], 
							g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_SPD_BIT_VERIFY][_ACC_]))
					}
				}
			}

			m_fsm.Set(C_SPD_BIT_VERIFY_01);
		}
		break;

	case C_SPD_BIT_VERIFY_01:
		if(!m_fsm.Delay(500))
			break;

		if(g_opr.isDryRun)
		{
			m_fsm.Set(C_SPD_BIT_VERIFY_END);
			break;
		}

		if(0 == m_fsm.GetStep())
		{
			if(ROUTER_F == m_fsm.GetMsg())
			{
				if(!m_pMtZ_F->InPos(PZ_SPD_BIT_VERIFY))
					m_pMtZ_F->Move(PZ_SPD_BIT_VERIFY);
				else
					m_fsm.SetStep(1);
			}
			else
			{
				if(!m_pMtZ_R->InPos(PZ_SPD_BIT_VERIFY))
					m_pMtZ_R->Move(PZ_SPD_BIT_VERIFY);
				else
					m_fsm.SetStep(1);
			}
		} 
		else
		{
			if(ROUTER_PART_F == m_nId)
			{
				if(g_pNV->UseSkip(usBitBroken))
				{
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, L"SENSOR_VERIFY_BIT_BROKEN_F", g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sensor, L"'SEN_X1700'", L"'OFF'"))
				}

				if(!g_dIn.AOn(iRouterBitBrokenCheck01) && g_pNV->UseSkip(usBitBroken))
				{
					if(ROUTER_F == m_fsm.GetMsg())
					{
						m_fsm.Set(C_ERROR, ER_ROUTER_BIT_BROKEN_3_CNT_01);
							break;
						//m_nBitBrokenErrCntF++;
						//if(3 <= m_nBitBrokenErrCntF)
						//{
						//	m_pMtZ_F->Move(PZ_READY);
						//	m_pMtZ_R->Move(PZ_READY);
						//	m_fsm.Set(C_ERROR, ER_ROUTER_BIT_BROKEN_3_CNT_01);
						//	break;
						//}
						//else 
						//{
						//	m_bNeedBitChangeF = TRUE;
						//	m_pInfoBitF->nLength = (int)g_pNV->DDm(bitMaxLifeLength);
						//	m_pInfoBitF->nZStep  = (int)g_pNV->Pkg(bitZStepCnt);

						//	m_fsm.Set(C_SPD_BIT_VERIFY_END);
						//}
					}
					else
					{
						m_fsm.Set(C_ERROR, ER_ROUTER_BIT_BROKEN_3_CNT_02);
							break;

						//m_nBitBrokenErrCntR++; 
						//if(3 <= m_nBitBrokenErrCntR) 
						//{
						//	m_pMtZ_F->Move(PZ_READY);
						//	m_pMtZ_R->Move(PZ_READY);
						//	m_fsm.Set(C_ERROR, ER_ROUTER_BIT_BROKEN_3_CNT_02);
						//	break;
						//}
						//else
						//{
						//	m_bNeedBitChangeR = TRUE;
						//	m_pInfoBitR->nLength = (int)g_pNV->DDm(bitMaxLifeLength);
						//	m_pInfoBitR->nZStep  = (int)g_pNV->Pkg(bitZStepCnt);

						//	m_fsm.Set(C_SPD_BIT_VERIFY_END);
						//}
					}
				}
				else
				{
					if(g_pNV->UseSkip(usBitBroken))
					{
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, L"SENSOR_VERIFY_BIT_BROKEN_F", g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sensor, L"'SEN_X1700'", L"'OFF'"))
					}
					if(ROUTER_F == m_fsm.GetMsg())
						m_nBitBrokenErrCntF = 0;
					else
						m_nBitBrokenErrCntR = 0;

					m_fsm.Set(C_SPD_BIT_VERIFY_02);
				}
			}
			else
			{
				if(g_pNV->UseSkip(usBitBroken))
				{
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, L"SENSOR_VERIFY_BIT_BROKEN_R", g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sensor, L"'SEN_X1700'", L"'OFF'"))
				}

				if(!g_dIn.AOn(iRouterBitBrokenCheck02) && g_pNV->UseSkip(usBitBroken))
				{
					if(ROUTER_F == m_fsm.GetMsg())
					{
						m_fsm.Set(C_ERROR, ER_ROUTER_BIT_BROKEN_3_CNT_03);
							break;
						//m_nBitBrokenErrCntF++;
						//if(3 <= m_nBitBrokenErrCntF)
						//{
						//	m_pMtZ_F->Move(PZ_READY);
						//	m_pMtZ_R->Move(PZ_READY);
						//	m_fsm.Set(C_ERROR, ER_ROUTER_BIT_BROKEN_3_CNT_03);
						//	break;
						//}
						//else
						//{
						//	m_bNeedBitChangeF = TRUE;
						//	m_pInfoBitF->nLength = (int)g_pNV->DDm(bitMaxLifeLength);
						//	m_pInfoBitF->nZStep  = (int)g_pNV->Pkg(bitZStepCnt);

						//	m_fsm.Set(C_SPD_BIT_VERIFY_END);
						//}
					}
					else
					{
						m_fsm.Set(C_ERROR, ER_ROUTER_BIT_BROKEN_3_CNT_04);
							break;
						//m_nBitBrokenErrCntR++;
						//if(3 <= m_nBitBrokenErrCntR)
						//{
						//	m_pMtZ_F->Move(PZ_READY);
						//	m_pMtZ_R->Move(PZ_READY);
						//	m_fsm.Set(C_ERROR, ER_ROUTER_BIT_BROKEN_3_CNT_04);
						//	break;
						//}
						//else
						//{
						//	m_bNeedBitChangeR = TRUE;
						//	m_pInfoBitR->nLength = (int)g_pNV->DDm(bitMaxLifeLength);
						//	m_pInfoBitR->nZStep  = (int)g_pNV->Pkg(bitZStepCnt);

						//	m_fsm.Set(C_SPD_BIT_VERIFY_END);
						//}
					}
				}
				else
				{
					if(g_pNV->UseSkip(usBitBroken))
					{
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, L"SENSOR_VERIFY_BIT_BROKEN_R", g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sensor, L"'SEN_X1700'", L"'OFF'"))
					}
					if(ROUTER_F == m_fsm.GetMsg())
						m_nBitBrokenErrCntF = 0;
					else
						m_nBitBrokenErrCntR = 0;

					m_fsm.Set(C_SPD_BIT_VERIFY_02);
				}
			}
		}
		break;

	case C_SPD_BIT_VERIFY_02:
		if(!m_fsm.Delay(500))
			break;

		if(g_opr.isDryRun)
		{
			m_fsm.Set(C_SPD_BIT_VERIFY_END);
			break;
		}

		if(0 == m_fsm.GetStep())
		{
			if(ROUTER_PART_F == m_nId)
			{
				if(ROUTER_F == m_fsm.GetMsg())
				{
					double dZPos = m_pMtZ_F->m_pTable->pos[PZ_SPD_BIT_VERIFY];; 
					dZPos += (HeightDistBrokenToFlowDown(nIdx) * 1000.0); 

					if(!m_pMtZ_F->InPos(PZ_SPD_BIT_VERIFY, dZPos, 2))
						m_pMtZ_F->PMove(PZ_SPD_BIT_VERIFY, dZPos);
					else
						m_fsm.SetStep(1);
				}
				else
				{
					double dZPos = m_pMtZ_R->m_pTable->pos[PZ_SPD_BIT_VERIFY];;
					dZPos += (HeightDistBrokenToFlowDown(nIdx) * 1000.0); 

					if(!m_pMtZ_R->InPos(PZ_SPD_BIT_VERIFY, dZPos, 2))
						m_pMtZ_R->PMove(PZ_SPD_BIT_VERIFY, dZPos);
					else
						m_fsm.SetStep(1);
				}
			}
			else
			{
				if(ROUTER_F == m_fsm.GetMsg())
				{
					double dZPos = m_pMtZ_F->m_pTable->pos[PZ_SPD_BIT_VERIFY];;
					dZPos += (HeightDistBrokenToFlowDown(nIdx) * 1000.0);

					if(!m_pMtZ_F->InPos(PZ_SPD_BIT_VERIFY, dZPos, 2))
						m_pMtZ_F->PMove(PZ_SPD_BIT_VERIFY, dZPos);
					else 
						m_fsm.SetStep(1);
				}
				else
				{
					double dZPos = m_pMtZ_R->m_pTable->pos[PZ_SPD_BIT_VERIFY];;
					dZPos += (HeightDistBrokenToFlowDown(nIdx) * 1000.0);

					if(!m_pMtZ_R->InPos(PZ_SPD_BIT_VERIFY, dZPos, 2))
						m_pMtZ_R->PMove(PZ_SPD_BIT_VERIFY, dZPos);
					else
						m_fsm.SetStep(1);
				}
			}
		} 
		else
		{
			if(ROUTER_PART_F == m_nId)
			{
				if(g_pNV->UseSkip(usBitHeight))
				{
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, L"SENSOR_VERIFY_BIT_DOWN_F", g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sensor, L"'SEN_X1701'", L"'ON'"))
				}

				if(g_dIn.AOn(iRouterBitBrokenCheck01) && g_pNV->UseSkip(usBitHeight))
				{
					if(ROUTER_F == m_fsm.GetMsg())
					{
						m_fsm.Set(C_ERROR, ER_ROUTER_BIT_DOWN_3_CNT_01);
							break;
						//m_nBitDownErrCntF++;
						//if(3 <= m_nBitDownErrCntF)
						//{
						//	m_pMtZ_F->Move(PZ_READY);
						//	m_pMtZ_R->Move(PZ_READY);
						//	m_fsm.Set(C_ERROR, ER_ROUTER_BIT_DOWN_3_CNT_01);
						//	break;
						//}
						//else
						//{
						//	m_bNeedBitChangeF = TRUE;
						//	m_pInfoBitF->nLength = (int)g_pNV->DDm(bitMaxLifeLength);
						//	m_pInfoBitF->nZStep  = (int)g_pNV->Pkg(bitZStepCnt);

						//	m_fsm.Set(C_SPD_BIT_VERIFY_END);
						//}
					}
					else
					{
						m_fsm.Set(C_ERROR, ER_ROUTER_BIT_DOWN_3_CNT_02);
							break;
						//m_nBitDownErrCntR++;
						//if(3 <= m_nBitDownErrCntR)
						//{
						//	m_pMtZ_F->Move(PZ_READY);
						//	m_pMtZ_R->Move(PZ_READY);
						//	m_fsm.Set(C_ERROR, ER_ROUTER_BIT_DOWN_3_CNT_02);
						//	break;
						//}
						//else
						//{
						//	m_bNeedBitChangeR = TRUE;
						//	m_pInfoBitR->nLength = (int)g_pNV->DDm(bitMaxLifeLength);
						//	m_pInfoBitR->nZStep  = (int)g_pNV->Pkg(bitZStepCnt);

						//	m_fsm.Set(C_SPD_BIT_VERIFY_END);
						//}
					}
				}
				else
				{
					if(g_pNV->UseSkip(usBitHeight))
					{
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, L"SENSOR_VERIFY_BIT_DOWN_F", g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sensor, L"'SEN_X1701'", L"'ON'"))
					}
					if(ROUTER_F == m_fsm.GetMsg())
						m_nBitDownErrCntF = 0;
					else
						m_nBitDownErrCntR = 0;

					m_fsm.Set(C_SPD_BIT_VERIFY_END);
				}
			}
			else
			{
				if(g_pNV->UseSkip(usBitHeight))
				{
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, L"SENSOR_VERIFY_BIT_DOWN_R", g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sensor, L"'SEN_X1701'", L"'ON'"))
				}

				if(g_dIn.AOn(iRouterBitBrokenCheck02) && g_pNV->UseSkip(usBitHeight))
				{
					if(ROUTER_F == m_fsm.GetMsg())
					{
						m_fsm.Set(C_ERROR, ER_ROUTER_BIT_DOWN_3_CNT_03);
							break;
						//m_nBitDownErrCntF++;
						//if(3 <= m_nBitDownErrCntF)
						//{
						//	m_pMtZ_F->Move(PZ_READY);
						//	m_pMtZ_R->Move(PZ_READY);
						//	m_fsm.Set(C_ERROR, ER_ROUTER_BIT_DOWN_3_CNT_03);
						//	break;
						//}
						//else
						//{
						//	m_bNeedBitChangeF = TRUE;
						//	m_pInfoBitF->nLength = (int)g_pNV->DDm(bitMaxLifeLength);
						//	m_pInfoBitF->nZStep  = (int)g_pNV->Pkg(bitZStepCnt);

						//	m_fsm.Set(C_SPD_BIT_VERIFY_END);
						//}
					}
					else
					{
						m_fsm.Set(C_ERROR, ER_ROUTER_BIT_DOWN_3_CNT_04);
							break;
						//m_nBitDownErrCntR++;
						//if(3 <= m_nBitDownErrCntR)
						//{
						//	m_pMtZ_F->Move(PZ_READY);
						//	m_pMtZ_R->Move(PZ_READY);
						//	m_fsm.Set(C_ERROR, ER_ROUTER_BIT_DOWN_3_CNT_04);
						//	break;
						//}
						//else
						//{
						//	m_bNeedBitChangeR = TRUE;
						//	m_pInfoBitR->nLength = (int)g_pNV->DDm(bitMaxLifeLength);
						//	m_pInfoBitR->nZStep  = (int)g_pNV->Pkg(bitZStepCnt);

						//	m_fsm.Set(C_SPD_BIT_VERIFY_END);
						//}
					}
				}
				else
				{
					if(g_pNV->UseSkip(usBitHeight))
					{
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, L"SENSOR_VERIFY_BIT_DOWN_R", g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.sensor, L"'SEN_X1701'", L"'ON'"))
					}
					if(ROUTER_F == m_fsm.GetMsg())
						m_nBitDownErrCntF = 0;
					else
						m_nBitDownErrCntR = 0;

					m_fsm.Set(C_SPD_BIT_VERIFY_END);
				}
			}
		}
		break;

	case C_SPD_BIT_VERIFY_END:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);

			_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_READY_SPD_BIT_VERIFY", (((m_nId % 2) * 2) + 1));
			_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_READY_SPD_BIT_VERIFY", (((m_nId % 2) * 2) + 2));

			if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.start, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_POS_], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_SPD_], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_ACC_]))
			}
			if(!g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.start, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_POS_], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_SPD_], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_ACC_]))
			}
		}
		else
		{
			_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_READY_SPD_BIT_VERIFY", (((m_nId % 2) * 2) + 1));
			_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_READY_SPD_BIT_VERIFY", (((m_nId % 2) * 2) + 2));

			if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZMainMoveEventId, g_data2c.cEtc.end, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_POS_], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_SPD_], 
					g_data2c.cRouter[m_nId].Z1[PZ_READY][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_READY][_ACC_]))
			}
			if(g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ_R->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cZSubMoveEventId, g_data2c.cEtc.end, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_R->m_config.axisNo], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_POSIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_POS_], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_SPDIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_SPD_], 
					g_data2c.cRouter[m_nId].Z2[PZ_READY][_ACCIDX_], g_data2c.cRouter[m_nId].Z2[PZ_READY][_ACC_]))
			}

			if(ROUTER_F == m_fsm.GetMsg())
				m_bNeedBitBrokenCheckF = FALSE;
			else
				m_bNeedBitBrokenCheckR = FALSE;

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemBitCheck01End + nIdx) = STATE_REQ;

			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRouter[m_nId].deviceId, L"BIT_VERIFY", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"BIT_VERIFY_START", L"BIT_VERIFY_END"))
				m_fsm.Set(C_IDLE);
		}
		break;
	}
}

//-------------------------------------------------------------------
void CRouter::CycleRunLoadCheck(void)
{
	if(!m_fsm.Between(C_LDC_START, C_LDC_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_LOAD_CHECK_TM_OVER_01 + (m_nId*2) + m_fsm.GetMsg());
		return;
	}

	if(!IsMtRdy())
		return;

	if(INDEX_F == m_fsm.GetMsg())
	{
		if(!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
			return;
	}
	else
	{
		if(!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
			return;
	}

	int nIdx = INDEX_IDLE;

	if(ROUTER_PART_F == m_nId)
	{
		if(INDEX_F == m_fsm.GetMsg())
			nIdx = INDEX_01;
		else 
			nIdx = INDEX_02;
	}
	else
	{
		if(INDEX_F == m_fsm.GetMsg())
			nIdx = INDEX_03;
		else 
			nIdx = INDEX_04;
	}

	int nMaxLdcCnt = 3;  // 3 point 검사

	switch(m_fsm.Get())
	{
	case C_LDC_START:
		//		if(g_pNV->UseSkip(usSecsGem))
		//			g_pNV->NDm(gemStagePcbPrs01Start + nIdx) = STATE_REQ;

		//		m_nPrsRetry = 0;
		m_nLdcArrayY = 0;
		m_fsm.Set(C_LDC_INIT);
		break;
	case C_LDC_INIT:
		if(m_fsm.Once())
		{
			m_nLdcBlockCnt = 1;

			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
			m_pCylBitClampUD->Actuate(pmUP);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
				m_pCylBitClampUD->Actuate(pmUP);
			else
			{
				if(!m_pMtZ_F->InPos(PZ_PRS))
				{
					m_pMtZ_F->Move(PZ_PRS);
					break;
				}
				if(!m_pMtZ_R->InPos(PZ_READY))
				{
					m_pMtZ_R->Move(PZ_READY);
					break;
				}
				
				m_fsm.Set(C_LDC_RST);
			}

		}
		break;
	case C_LDC_RST:
		if(m_fsm.Once())
		{
			if(ROUTER_PART_F == m_nId)
				g_dOut.On(oViRouterPrsRstF);
			else
				g_dOut.On(oViRouterPrsRstR);
		}
		else
		{
			if(!m_fsm.Delay(300))
				break;

			if(ROUTER_PART_F == m_nId)
			{
				if(m_fsm.TimeLimit(5000))
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_F_PRS_NOT_READY);
					break;
				}

				if(g_opr.isDryRun)
				{
					g_dOut.Off(oViRouterPrsRstF);
					m_fsm.Set(C_LDC_MOVE);
					break;
				}

				if(g_dIn.AOn(iViRouterPrsReadyF) && !g_dIn.AOn(iViRouterPrsBusyF))
				{
					g_dOut.Off(oViRouterPrsRstF);
					m_fsm.Set(C_LDC_MOVE);
				}
			}
			else
			{
				if(m_fsm.TimeLimit(5000))
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_R_PRS_NOT_READY);
					break;
				}

				if(g_opr.isDryRun)
				{
					g_dOut.Off(oViRouterPrsRstR);
					m_fsm.Set(C_LDC_MOVE);
					break;
				}

				if(g_dIn.AOn(iViRouterPrsReadyR) && !g_dIn.AOn(iViRouterPrsBusyR))
				{
					g_dOut.Off(oViRouterPrsRstR);
					m_fsm.Set(C_LDC_MOVE);
				}
			}
		}
		break;
	case C_LDC_MOVE:

		if(m_fsm.Once())
		{
			POINT2D ptPos;
			ptPos.dX = ptPos.dY = 0;

			if(INDEX_F == m_fsm.GetMsg())
			{
				ptPos = m_pIndexF->GetRouterPrsPos(m_nLdcBlockCnt, m_nLdcArrayY);

				m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_PRS, ptPos.dX);
				m_pIndexF->m_pMtT->Move(CIndex::PT_ROUTER_PRS);
				double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
				GentryMtYWPMove(PY_VI_PRS_F, PW_READY, ptPos.dY, dPosW);
			}
			else
			{
				ptPos = m_pIndexR->GetRouterPrsPos(m_nLdcBlockCnt, m_nLdcArrayY);

				m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_PRS, ptPos.dX);
				m_pIndexR->m_pMtT->Move(CIndex::PT_ROUTER_PRS);
				double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
				GentryMtYWPMove(PY_VI_PRS_R, PW_READY, ptPos.dY, dPosW);
			}
		}
		else
		{
			if(!m_pMtZ_F->InPos(PZ_PRS))
			{
				m_pMtZ_F->Move(PZ_PRS);
				break;
			}

			m_fsm.Set(C_LDC_TRIG);
		}
		break;
	case C_LDC_TRIG:

		if(!m_fsm.Delay(300))
			break;

		if(m_fsm.Once())
		{
			m_fsm.RstTimeLimit();
			m_viPrsData.dX = m_viPrsData.dY = DEFAULT_VI_VAL;
			if(ROUTER_PART_F == m_nId)
			{
				g_pNV->NDm(mmiRouterFErr) = 0;
				g_dOut.On(oViRouterPrsTrigF);
			}
			else
			{
				g_pNV->NDm(mmiRouterRErr) = 0;
				g_dOut.On(oViRouterPrsTrigR);
			}
		}
		else
		{
			if(!m_fsm.Delay(300))
				break;

			if(ROUTER_PART_F == m_nId)
			{
				if(m_fsm.TimeLimit(7000))
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_F_PRS_RESULT_TM_OVER);
					break;
				}

				if(1 == g_pNV->NDm(mmiRouterFErr))
				{
					g_pNV->NDm(mmiRouterFErr) = 0;
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					g_dOut.Off(oViRouterPrsTrigF);

					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_F_PRS_FAIL);
					break;
				}

				if(!g_opr.isDryRun)
				{
					if(!g_dIn.AOn(iViRouterPrsReadyF) || g_dIn.AOn(iViRouterPrsBusyF))
						break;

					if(DEFAULT_VI_VAL == (int)m_viPrsData.dX)
						break;

					if(GetLdcDataLimit())
					{
						if(m_pMtZ_F->IsRdy(PZ_READY) && m_pMtZ_R->IsRdy(PZ_READY))
						{
							g_dOut.Off(oViRouterPrsTrigF);
							m_fsm.Set(C_ERROR, ER_VI_INDEX01_IN_PRS_RESULT_LIMIT+nIdx);
							m_pIndexF->m_pCylDustShutterOC->Actuate(pmCLOSE);
						}
						else
						{
							m_pMtZ_F->Move(PZ_READY);
							m_pMtZ_R->Move(PZ_READY);
						}
						break;
					}
				}
				m_nLdcBlockCnt++;

				g_dOut.Off(oViRouterPrsTrigF);
				if(nMaxLdcCnt <= m_nLdcBlockCnt)
					m_fsm.Set(C_LDC_END);
				else
					m_fsm.Set(C_LDC_MOVE);
			}
			else
			{
				if(m_fsm.TimeLimit(7000))
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_R_PRS_RESULT_TM_OVER);
					break;
				}

				if(1 == g_pNV->NDm(mmiRouterRErr))
				{
					g_pNV->NDm(mmiRouterRErr) = 0;
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					g_dOut.Off(oViRouterPrsTrigR);

					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_R_PRS_FAIL);
					break;
				}

				if(!g_opr.isDryRun)
				{
					if(!g_dIn.AOn(iViRouterPrsReadyR) || g_dIn.AOn(iViRouterPrsBusyR))
						break;

					if(DEFAULT_VI_VAL == (int)m_viPrsData.dX)
						break;

					if(GetLdcDataLimit())
					{
						if(m_pMtZ_F->IsRdy(PZ_READY) && m_pMtZ_R->IsRdy(PZ_READY))
						{
							g_dOut.Off(oViRouterPrsTrigR);
							m_fsm.Set(C_ERROR, ER_VI_INDEX01_IN_PRS_RESULT_LIMIT+nIdx);
							m_pIndexR->m_pCylDustShutterOC->Actuate(pmCLOSE);
						}
						else
						{
							m_pMtZ_F->Move(PZ_READY);
							m_pMtZ_R->Move(PZ_READY);
						}
						break;
					}
				}
				m_nLdcBlockCnt++;

				g_dOut.Off(oViRouterPrsTrigR);
				if(nMaxLdcCnt <= m_nLdcBlockCnt)
					m_fsm.Set(C_LDC_END);
				else
					m_fsm.Set(C_LDC_MOVE);
			}
		}
		break;
	case C_LDC_END:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(INDEX_F == m_fsm.GetMsg())
				m_pIndexF->m_pMem->compLoadCheck = TRUE; 
			else
				m_pIndexR->m_pMem->compLoadCheck = TRUE;

			//			if(g_pNV->UseSkip(usSecsGem))
			//				g_pNV->NDm(gemStagePcbPrs01End + nIdx) = STATE_REQ;

			m_fsm.Set(C_IDLE);
		}
		break;
	}
}

//-------------------------------------------------------------------
void CRouter::CycleRunPrs(void)
{
	if(!m_fsm.Between(C_PRS_START, C_PRS_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_ROUTER_PRS_CYCLE_TM_OVER_01 + (m_nId*2) + m_fsm.GetMsg());
		return;
	}

	if(!IsMtRdy())
		return;

	if(INDEX_F == m_fsm.GetMsg())
	{
		if(!m_pIndexF->m_pMtX->IsRdy() || !m_pIndexF->m_pMtT->IsRdy())
			return;
	}
	else
	{
		if(!m_pIndexR->m_pMtX->IsRdy() || !m_pIndexR->m_pMtT->IsRdy())
			return;
	}

	int nIdx = INDEX_IDLE;

	if(ROUTER_PART_F == m_nId)
	{
		if(INDEX_F == m_fsm.GetMsg())
			nIdx = INDEX_01;
		else 
			nIdx = INDEX_02;
	}
	else
	{
		if(INDEX_F == m_fsm.GetMsg())
			nIdx = INDEX_03;
		else 
			nIdx = INDEX_04;
	}

	int nMaxPrsCnt = 3;  // 3 point 검사

	_char cXPos[_MAX_CHAR_SIZE_], cYPos[_MAX_CHAR_SIZE_];
	_char cXMoveEventId[_MAX_CHAR_SIZE_], cYMoveEventId[_MAX_CHAR_SIZE_], cTMoveEventId[_MAX_CHAR_SIZE_], cZMainMoveEventId[_MAX_CHAR_SIZE_], cZSubMoveEventId[_MAX_CHAR_SIZE_];
	_sprintf(cXMoveEventId, L"MT_INDEX_X_%02d_PRS", (nIdx + 1));
	_sprintf(cTMoveEventId, L"MT_INDEX_T_%02d_PRS", (nIdx + 1));
	_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_PRS", (((m_nId % 2) * 2) + 1));
	_sprintf(cZMainMoveEventId, L"MT_SPINDLE_Z_%02d_PRS", (((m_nId % 2) * 2) + 1));
	_sprintf(cZSubMoveEventId, L"MT_SPINDLE_Z_%02d_PRS", (((m_nId % 2) * 2) + 2));

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");	
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + nIdx].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + nIdx].pcbBarcode) + 1);

	switch(m_fsm.Get())
	{
	case C_PRS_START:
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRouter[m_nId].deviceId, L"PRS_VISION", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"PRS_START", L"PRS_END"))

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemStagePcbPrs01Start + nIdx) = STATE_REQ;

		m_nPrsRetry = 0;
		m_nPrsArrayYRetry = 0;
		m_fsm.Set(C_PRS_INIT);
		break;
	case C_PRS_INIT:
		if(m_fsm.Once())
		{
			m_bTestPrs = FALSE;

			// 2Point 만 검사 하도록 하기 위해서 강제로 해당 Step 변경함
			//m_nPrsBlockCnt = 0;
			m_nPrsBlockCnt = 1;
			for(int nCnt = 0; nCnt < BLOCK_PT_MAX; nCnt++)
			{
				m_viPrsBlock[nCnt].dX = m_viPrsBlock[nCnt].dY = 0;
			}
			for(int n = 0; n < UNIT_MAX; n++)
			{
				g_pIndex[nIdx]->m_pPrsResult->unit[n].dX = 0;
				g_pIndex[nIdx]->m_pPrsResult->unit[n].dY = 0;
				g_pIndex[nIdx]->m_pPrsResult->unit[n].dT = 0;
			}
			g_pIndex[nIdx]->m_pPrsResult->block.dX = 0.0;
			g_pIndex[nIdx]->m_pPrsResult->block.dY = 0.0;
			g_pIndex[nIdx]->m_pPrsResult->block.dT = 0.0;
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(pmUP != m_pCylBitClampUD->GetPos(300))
				m_pCylBitClampUD->Actuate(pmUP);
			else
			{
				if(INDEX_F == m_fsm.GetMsg())
				{
					if(!m_pIndexF->CylIndexMaskFixAct(pmCLOSE))
						break;
				}
				else
				{
					if(!m_pIndexR->CylIndexMaskFixAct(pmCLOSE))
						break;
				}

				if(!m_pMtZ_F->InPos(PZ_PRS))
				{
					m_pMtZ_F->Move(PZ_PRS);
					break;
				}

				m_fsm.Set(C_PRS_RST);
			}

		}
		break;
	case C_PRS_RST:
		if(m_fsm.Once())
		{
			if(ROUTER_PART_F == m_nId)
				g_dOut.On(oViRouterPrsRstF);
			else
				g_dOut.On(oViRouterPrsRstR);
		}
		else
		{
			if(!m_fsm.Delay(300))
				break;

			if(ROUTER_PART_F == m_nId)
			{
				if(m_fsm.TimeLimit(5000))
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_F_PRS_NOT_READY);
					break;
				}

				if(g_opr.isDryRun)
				{
					g_dOut.Off(oViRouterPrsRstF);
					m_fsm.Set(C_PRS_MOVE);
					break;
				}

				if(g_dIn.AOn(iViRouterPrsReadyF) && !g_dIn.AOn(iViRouterPrsBusyF))
				{
					g_dOut.Off(oViRouterPrsRstF);
					m_fsm.Set(C_PRS_MOVE);
				}
			}
			else
			{
				if(m_fsm.TimeLimit(5000))
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_R_PRS_NOT_READY);
					break;
				}

				if(g_opr.isDryRun)
				{
					g_dOut.Off(oViRouterPrsRstR);
					m_fsm.Set(C_PRS_MOVE);
					break;
				}

				if(g_dIn.AOn(iViRouterPrsReadyR) && !g_dIn.AOn(iViRouterPrsBusyR))
				{
					g_dOut.Off(oViRouterPrsRstR);
					m_fsm.Set(C_PRS_MOVE);
				}
			}
		}
		break;
	case C_PRS_MOVE:
		if(m_fsm.Once())
		{
			POINT2D ptPos;
			ptPos.dX = ptPos.dY = 0;

			if(INDEX_F == m_fsm.GetMsg())
			{
				// X, T 값은 동일, Y 값은 PRS Retry를 위해 연산 필요
				ptPos = m_pIndexF->GetRouterPrsPos(m_nPrsBlockCnt, m_nPrsArrayYRetry);

				_sprintf(cXPos, L"%03f", ptPos.dX);
				_sprintf(cYPos, L"%03f", ptPos.dY);

				if(!m_bTestPrs)
				{
					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_PRS, ptPos.dX);
					m_pIndexF->m_pMtT->Move(CIndex::PT_ROUTER_PRS);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_PRS_F, PW_READY, ptPos.dY, dPosW);

					if(!g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_PRS])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_PRS] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_SPD_], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_PRS])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_PRS] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_POSIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_POS_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_F])
					{
						_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_PRS", (((m_nId % 2) * 2) + 1));
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_F] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_ACC_]))
					}
				}
				else
				{
					ptPos.dX = ptPos.dX - m_pIndexF->m_pPrsResult->block.dX * (1000.0);
					if(ROUTER_PART_F == m_nId)
						ptPos.dY = ptPos.dY + (m_pIndexF->m_pPrsResult->block.dY * (1000.0));
					else // ROUTER_PART_R
						ptPos.dY = ptPos.dY - (m_pIndexF->m_pPrsResult->block.dY * (1000.0));

					double dT = m_pIndexF->m_pMtT->m_pTable->pos[CIndex::PT_ROUTER_PRS] +
						(m_pIndexF->m_pPrsResult->block.dT * (1000.0));

					m_pIndexF->m_pMtX->PMove(CIndex::PX_ROUTER_PRS, ptPos.dX);
					m_pIndexF->m_pMtT->PMove(CIndex::PT_ROUTER_PRS, dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_PRS_F, PW_READY, ptPos.dY, dPosW);
				}
			}
			else
			{
				ptPos = m_pIndexR->GetRouterPrsPos(m_nPrsBlockCnt, m_nPrsArrayYRetry);

				_sprintf(cXPos, L"%03f", ptPos.dX);
				_sprintf(cYPos, L"%03f", ptPos.dY);

				if(!m_bTestPrs)
				{
					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_PRS, ptPos.dX);
					m_pIndexR->m_pMtT->Move(CIndex::PT_ROUTER_PRS);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_PRS_R, PW_READY, ptPos.dY, dPosW);

					if(!g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_PRS])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_PRS] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_SPD_], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pIndexR->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_PRS])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_PRS] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_POSIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_POS_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_ACC_]))
					}
					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_R])
					{
						_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_PRS", (((m_nId % 2) * 2) + 1));
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_R] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_ACC_]))
					}
				}
				else
				{
					ptPos.dX = ptPos.dX - (m_pIndexR->m_pPrsResult->block.dX * (1000.0));
					if(ROUTER_PART_F == m_nId)
						ptPos.dY = ptPos.dY + (m_pIndexR->m_pPrsResult->block.dY * (1000.0));
					else // ROUTER_PART_R
						ptPos.dY = ptPos.dY - (m_pIndexR->m_pPrsResult->block.dY * (1000.0));
					// Rear Index 2, 4번은 Home 방향이 반대이므로 부호 반대
					//double dT = m_pIndexR->m_pMtT->m_pTable->pos[CIndex::PT_ROUTER_PRS] +
					//	       (m_pIndexR->m_pPrsResult->block.dT * (1000.0));
					double dT = m_pIndexR->m_pMtT->m_pTable->pos[CIndex::PT_ROUTER_PRS] -
						(m_pIndexR->m_pPrsResult->block.dT * (1000.0));

					m_pIndexR->m_pMtX->PMove(CIndex::PX_ROUTER_PRS, ptPos.dX);
					m_pIndexR->m_pMtT->PMove(CIndex::PT_ROUTER_PRS, dT);
					double dPosW = m_pMtW->m_pTable->pos[CRouter::PW_READY];
					GentryMtYWPMove(PY_VI_PRS_R, PW_READY, ptPos.dY, dPosW);
				}
			}
		}
		else
		{
			POINT2D ptPos;
			ptPos.dX = ptPos.dY = 0;

			if(INDEX_F == m_fsm.GetMsg())
			{
				// X, T 값은 동일, Y 값은 PRS Retry를 위해 연산 필요
				ptPos = m_pIndexF->GetRouterPrsPos(m_nPrsBlockCnt, m_nPrsArrayYRetry);

				_sprintf(cXPos, L"%03f", ptPos.dX);
				_sprintf(cYPos, L"%03f", ptPos.dY);

				if(!m_bTestPrs)
				{
					if(g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_PRS])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_PRS] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_SPD_], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_PRS])
					{
						g_logChk.bTransfer[m_pIndexF->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_PRS] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexF->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_POSIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_POS_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_F])
					{
						_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_PRS", (((m_nId % 2) * 2) + 1));
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_F] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_F][_ACC_]))
					}
				}
			}
			else
			{
				ptPos = m_pIndexR->GetRouterPrsPos(m_nPrsBlockCnt, m_nPrsArrayYRetry);

				_sprintf(cXPos, L"%03f", ptPos.dX);
				_sprintf(cYPos, L"%03f", ptPos.dY);

				if(!m_bTestPrs)
				{
					if(g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_PRS])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtX->m_config.axisNo][CIndex::PX_ROUTER_PRS] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cXMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtX->m_config.axisNo], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_POSIDX_], cXPos, 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_SPD_], 
							g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[nIdx].X[CIndex::PX_ROUTER_PRS][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pIndexR->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_PRS])
					{
						g_logChk.bTransfer[m_pIndexR->m_pMtT->m_config.axisNo][CIndex::PT_ROUTER_PRS] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cTMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pIndexR->m_pMtT->m_config.axisNo], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_POSIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_POS_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_SPD_], 
							g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[nIdx].T[CIndex::PT_ROUTER_PRS][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_R])
					{
						_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_PRS", (((m_nId % 2) * 2) + 1));
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_VI_PRS_R] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_POSIDX_], cYPos, 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_SPDIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_SPD_], 
							g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_ACCIDX_], g_data2c.cRouter[m_nId].Y[PY_VI_PRS_R][_ACC_]))
					}				
				}
			}

			if(!m_pMtZ_F->InPos(PZ_PRS))
			{
				m_pMtZ_F->Move(PZ_PRS);

				if(!g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_PRS])
				{
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_PRS", (((m_nId % 2) * 2) + 1)); 
					g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_PRS] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
						g_data2c.cRouter[m_nId].Z1[PZ_PRS][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_PRS][_POS_], 
						g_data2c.cRouter[m_nId].Z1[PZ_PRS][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_PRS][_SPD_], 
						g_data2c.cRouter[m_nId].Z1[PZ_PRS][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_PRS][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_PRS])
				{
					_sprintf(cYMoveEventId, L"MT_ROUTER_Y_%02d_PRS", (((m_nId % 2) * 2) + 1)); 
					g_logChk.bTransfer[m_pMtZ_F->m_config.axisNo][PZ_PRS] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cYMoveEventId, g_data2c.cEtc.end, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ_F->m_config.axisNo], 
						g_data2c.cRouter[m_nId].Z1[PZ_PRS][_POSIDX_], g_data2c.cRouter[m_nId].Z1[PZ_PRS][_POS_], 
						g_data2c.cRouter[m_nId].Z1[PZ_PRS][_SPDIDX_], g_data2c.cRouter[m_nId].Z1[PZ_PRS][_SPD_], 
						g_data2c.cRouter[m_nId].Z1[PZ_PRS][_ACCIDX_], g_data2c.cRouter[m_nId].Z1[PZ_PRS][_ACC_]))
				}
			}

			m_fsm.Set(C_PRS_TRIG);
		}
		break;
	case C_PRS_TRIG:
		if(m_fsm.Once())
		{
			_char cViEventId[_MAX_CHAR_SIZE_], cViActId[_MAX_CHAR_SIZE_];
			_sprintf(cViEventId, L"VISION_%02d_PRS",(nIdx + 1));
			_sprintf(cViActId, L"'VIS%d'", m_nId);
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cViEventId, g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vision, g_data2c.cEtc.actName, cViActId, L"'VALUE_X'", L"'$'", L"'VALUE_Y'", L"'$'"))

			m_fsm.RstTimeLimit();
			m_viPrsData.dX = m_viPrsData.dY = DEFAULT_VI_VAL;
			if(ROUTER_PART_F == m_nId)
			{
				g_pNV->NDm(mmiRouterFErr) = 0;
				g_dOut.On(oViRouterPrsTrigF);
			}
			else
			{
				g_pNV->NDm(mmiRouterRErr) = 0;
				g_dOut.On(oViRouterPrsTrigR);
			}
		}
		else
		{
			if(!m_fsm.Delay(300))
				break;

			if(ROUTER_PART_F == m_nId)
			{
				if(m_fsm.TimeLimit(7000))
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_F_PRS_RESULT_TM_OVER);
					break;
				}

				if(1 == g_pNV->NDm(mmiRouterFErr))
				{
					g_pNV->NDm(mmiRouterFErr) = 0;
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					g_dOut.Off(oViRouterPrsTrigF);

					m_nPrsRetry++;
					if(m_nPrsRetry < 3)
						m_fsm.Set(C_PRS_INIT);
					else
					{
						m_nPrsArrayYRetry++;
						if(m_nPrsArrayYRetry < g_pNV->gerberPara(arrayYCnt))
						{
							m_nPrsRetry = 0;
							m_fsm.Set(C_PRS_INIT);
						}
						else
						{
							m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_F_PRS_FAIL);
						}
					}

					_char cViEventId[_MAX_CHAR_SIZE_], cViActId[_MAX_CHAR_SIZE_], cValueX[_MAX_CHAR_SIZE_], cValueY[_MAX_CHAR_SIZE_];
					_sprintf(cViEventId, L"VISION_%02d_PRS",(nIdx + 1));
					_sprintf(cViActId, L"'VIS%d'", m_nId);
					_sprintf(cValueX, L"%03f", m_viPrsData.dX);
					_sprintf(cValueY, L"%03f", m_viPrsData.dY);
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cViEventId, g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vision, g_data2c.cEtc.actName, cViActId, L"'VALUE_X'", cValueX, L"'VALUE_Y'", cValueY))

						break;
				}

				if(!g_opr.isDryRun)
				{
					if(!g_dIn.AOn(iViRouterPrsReadyF) || g_dIn.AOn(iViRouterPrsBusyF))
						break;

					if(DEFAULT_VI_VAL == (int)m_viPrsData.dX)
						break;

					if(GetPrsDataLimit())
					{
						if(m_pMtZ_F->IsRdy(PZ_READY) && m_pMtZ_R->IsRdy(PZ_READY))
						{
							g_pIndex[nIdx]->m_pMem->compLoadCheck=FALSE;
							g_pIndex[nIdx]->m_pMem->compMaskClamp=FALSE;
							g_pIndex[nIdx]->m_pMem->compPRSFail=TRUE;
							g_dOut.Off(oViRouterPrsTrigF);
							m_fsm.Set(C_IDLE);
						}
						else
						{
							m_pMtZ_F->Move(PZ_READY);
							m_pMtZ_R->Move(PZ_READY);
						}
						break;
					}

					m_viPrsBlock[m_nPrsBlockCnt] = m_viPrsData;

					_char cViEventId[_MAX_CHAR_SIZE_], cViActId[_MAX_CHAR_SIZE_], cValueX[_MAX_CHAR_SIZE_], cValueY[_MAX_CHAR_SIZE_];
					_sprintf(cViEventId, L"VISION_%02d_PRS",(nIdx + 1));
					_sprintf(cViActId, L"'VIS%d'", m_nId);
					_sprintf(cValueX, L"%03f", m_viPrsData.dX);
					_sprintf(cValueY, L"%03f", m_viPrsData.dY);
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cViEventId, g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vision, g_data2c.cEtc.actName, cViActId, L"'VALUE_X'", cValueX, L"'VALUE_Y'", cValueY))

				}
				m_nPrsBlockCnt++;

				g_dOut.Off(oViRouterPrsTrigF);
				if(nMaxPrsCnt <= m_nPrsBlockCnt)
					m_fsm.Set(C_PRS_END);
				else
					m_fsm.Set(C_PRS_MOVE);
			}
			else
			{
				if(m_fsm.TimeLimit(7000))
				{
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_R_PRS_RESULT_TM_OVER);
					break;
				}

				if(1 == g_pNV->NDm(mmiRouterRErr))
				{
					g_pNV->NDm(mmiRouterRErr) = 0;
					m_pMtZ_F->Move(PZ_READY);
					m_pMtZ_R->Move(PZ_READY);
					g_dOut.Off(oViRouterPrsTrigR);

					m_nPrsRetry++;

					if(m_nPrsRetry < 3)
						m_fsm.Set(C_PRS_INIT);
					else
					{
						m_nPrsArrayYRetry++;
						if(m_nPrsArrayYRetry < g_pNV->gerberPara(arrayYCnt))
						{
							m_nPrsRetry = 0;
							m_fsm.Set(C_PRS_INIT);
						}
						else
						{
							m_fsm.Set(C_ERROR, ER_VI_ROUTER_PART_R_PRS_FAIL);
						}
					}

					_char cViEventId[_MAX_CHAR_SIZE_], cViActId[_MAX_CHAR_SIZE_], cValueX[_MAX_CHAR_SIZE_], cValueY[_MAX_CHAR_SIZE_]; 
					_sprintf(cViEventId, L"VISION_%02d_PRS",(nIdx + 1));
					_sprintf(cViActId, L"'VIS%d'", m_nId);
					_sprintf(cValueX, L"%03f", m_viPrsData.dX);
					_sprintf(cValueY, L"%03f", m_viPrsData.dY);
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cViEventId, g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vision, g_data2c.cEtc.actName, cViActId, L"'VALUE_X'", cValueX, L"'VALUE_Y'", cValueY))
						break;
				}

				if(!g_opr.isDryRun)
				{
					if(!g_dIn.AOn(iViRouterPrsReadyR) || g_dIn.AOn(iViRouterPrsBusyR))
						break;

					if(DEFAULT_VI_VAL == (int)m_viPrsData.dX)
						break;

					if(GetPrsDataLimit())
					{
						if(m_pMtZ_F->IsRdy(PZ_READY) && m_pMtZ_R->IsRdy(PZ_READY))
						{
							g_pIndex[nIdx]->m_pMem->compLoadCheck=FALSE;
							g_pIndex[nIdx]->m_pMem->compMaskClamp=FALSE;
							g_pIndex[nIdx]->m_pMem->compPRSFail=TRUE;
							g_dOut.Off(oViRouterPrsTrigR);
							m_fsm.Set(C_IDLE);
						}
						else
						{
							m_pMtZ_F->Move(PZ_READY);
							m_pMtZ_R->Move(PZ_READY);
						}
						break;
					}

					m_viPrsBlock[m_nPrsBlockCnt] = m_viPrsData;

					_char cViEventId[_MAX_CHAR_SIZE_], cViActId[_MAX_CHAR_SIZE_], cValueX[_MAX_CHAR_SIZE_], cValueY[_MAX_CHAR_SIZE_];
					_sprintf(cViEventId, L"VISION_%02d_PRS",(nIdx + 1));
					_sprintf(cViActId, L"'VIS%d'", m_nId);
					_sprintf(cValueX, L"%03f", m_viPrsData.dX);
					_sprintf(cValueY, L"%03f", m_viPrsData.dY);
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRouter[m_nId].deviceId, cViEventId, g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vision, g_data2c.cEtc.actName, cViActId, L"'VALUE_X'", cValueX, L"'VALUE_Y'", cValueY))
				}
				m_nPrsBlockCnt++;

				g_dOut.Off(oViRouterPrsTrigR);
				if(nMaxPrsCnt <= m_nPrsBlockCnt)
					m_fsm.Set(C_PRS_END);
				else
					m_fsm.Set(C_PRS_MOVE);
			}
		}
		break;
	case C_PRS_END:
		if(m_fsm.Once())
		{
			m_pMtZ_F->Move(PZ_READY);
			m_pMtZ_R->Move(PZ_READY);
		}
		else
		{
			if(g_opr.isDryRun)
			{
				for(int nCnt = 0; nCnt < BLOCK_PT_MAX; nCnt++)
				{
					m_viPrsBlock[nCnt].dX = m_viPrsBlock[nCnt].dY = 0;
				}
			}
			else
			{
				if(!m_bTestPrs)
					CreatePrsData(nIdx, m_nPrsArrayYRetry);  // Y Array 이동 하여 연산
				else
					CreatePrsDataVerify(nIdx);

				// PRS가 두개 이므로 나누어서 저장
				if(INDEX_01 == nIdx)
					g_pNV->NDm(prsDataLog12) = 1;
				else if(INDEX_02 == nIdx)
					g_pNV->NDm(prsDataLog12) = 2;
				else if(INDEX_03 == nIdx)
					g_pNV->NDm(prsDataLog34) = 3;
				else if(INDEX_04 == nIdx)
					g_pNV->NDm(prsDataLog34) = 4;
			}

			if(g_pIndex[nIdx]->GetBlockPrsResultErr())
			{
				m_pMtZ_F->Move(PZ_READY);
				m_pMtZ_R->Move(PZ_READY);
				m_fsm.Set(C_ERROR, ER_VI_INDEX01_BLOCK_PRS_RESULT_LIMIT + nIdx);
				break;
			}

			if(g_pNV->UseSkip(usTestRouterPrsVerify) && !m_bTestPrs)
			{
				m_nPrsBlockCnt = 0;
				for(int nCnt = 0; nCnt < BLOCK_PT_MAX; nCnt++)
				{
					m_viPrsBlock[nCnt].dX = m_viPrsBlock[nCnt].dY = 0;
				}

				m_bTestPrs = TRUE;
				m_nPrsBlockCnt = 1;
				m_nPrsArrayYRetry = 0;
				m_fsm.Set(C_PRS_RST);
			}
			else
			{
				if(INDEX_F == m_fsm.GetMsg())
					m_pIndexF->m_pMem->compRouterPrs = TRUE; 
				else
					m_pIndexR->m_pMem->compRouterPrs = TRUE;

				if(g_pNV->UseSkip(usSecsGem))
					g_pNV->NDm(gemStagePcbPrs01End + nIdx) = STATE_REQ;

				NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRouter[m_nId].deviceId, L"PRS_VISION", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"PRS_START", L"PRS_END"))
					m_fsm.Set(C_IDLE);
			}
		}
		break;
	}
}

double & CRouter::HeightDistBrokenToFlowDown(int nSpindleNo)
{
	int nDDmNo = spindle1DistBrokenToFlowdown + nSpindleNo;

	return (g_pNV->DDm(nDDmNo));
}


//-------------------------------------------------------------------
void RouterInit(void)
{
	//////////////////////////////////////////////////////////////////////////
	// Front Router
	g_routerF.InitNomal(ROUTER_PART_F, (CIndex*)&g_index01, (CIndex*)&g_index02);
	g_routerF.InitNv((BIT_INFO*)&g_pNV->NDm(routerBitInfo01), (BIT_INFO*)&g_pNV->NDm(routerBitInfo02));
	g_routerF.InitMt(&g_mt[MT_ROUTER_Y_01], &g_mt[MT_ROUTER_W_01], &g_mt[MT_SPINDLE_Z_01], &g_mt[MT_SPINDLE_Z_02]);

	g_routerF.InitPm1(&g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01], &g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_OC_01]);

	g_routerF.InitPm3(&g_pm[SOL_SPD_CHUCK_OC_01], &g_pm[SOL_SPD_CHUCK_OC_02]);
	g_routerF.InitPm4(&g_pm[SOL_SPD_AIR_BLOW_0102]);
	g_routerF.InitPm5(&g_pm[SOL_ROUTER_IONIZER_01], &g_pm[SOL_ROUTER_IONIZER_02]);
	g_routerF.InitPm6(&g_pm[SPINDLE_F_01], &g_pm[SPINDLE_F_02]);


	//////////////////////////////////////////////////////////////////////////
	// Rear Router
	g_routerR.InitNomal(ROUTER_PART_R, (CIndex*)&g_index03, (CIndex*)&g_index04);
	g_routerR.InitNv((BIT_INFO*)&g_pNV->NDm(routerBitInfo03), (BIT_INFO*)&g_pNV->NDm(routerBitInfo04));
	g_routerR.InitMt(&g_mt[MT_ROUTER_Y_02], &g_mt[MT_ROUTER_W_02], &g_mt[MT_SPINDLE_Z_03], &g_mt[MT_SPINDLE_Z_04]);

	g_routerR.InitPm1(&g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02], &g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_OC_02]);

	g_routerR.InitPm3(&g_pm[SOL_SPD_CHUCK_OC_03], &g_pm[SOL_SPD_CHUCK_OC_04]);
	g_routerR.InitPm4(&g_pm[SOL_SPD_AIR_BLOW_0304]);
	g_routerR.InitPm5(&g_pm[SOL_ROUTER_IONIZER_03], &g_pm[SOL_ROUTER_IONIZER_04]);
	g_routerR.InitPm6(&g_pm[SPINDLE_R_01], &g_pm[SPINDLE_R_02]);
}


