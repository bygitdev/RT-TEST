#include "..\DEF\Includes.h"


//////////////////////////////////////////////////////////////////////////
CIndex g_index01;
CIndex g_index02;
CIndex g_index03;
CIndex g_index04;
//////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
inline BOOL IsSkip(int nIdx)
{
	BOOL bSkip = !g_pNV->UseSkip(usIndex01 + nIdx);
	return (bSkip);
}


//-------------------------------------------------------------------
inline void IndexFree(int nIndex)
{
	int nRouterIndexNo = nIndex % 2; // 나머지 0, 1, 0, 1
	int nRouterPart	   = nIndex / 2; // 몫     0, 0, 1, 1

	if(ROUTER_PART_F == nRouterPart)
	{
		if(nRouterIndexNo == g_routerF.m_nCurIndex)
			g_routerF.m_nCurIndex = INDEX_FR_IDLE;
	}
	else
	{
		if(nRouterIndexNo == g_routerR.m_nCurIndex)
			g_routerR.m_nCurIndex = INDEX_FR_IDLE;
	}

	if(nIndex == g_outPnp.m_nCurIndex)
		g_outPnp.m_nCurIndex = INDEX_IDLE;
}


//-------------------------------------------------------------------
CIndex::CIndex()
{
	m_bRun = FALSE;

	m_bReqBitSupplyPos			= FALSE;
	m_bReqBitEjectPos			= FALSE;
	m_bReqBitAlignFPos			= FALSE;
	m_bReqBitAlignRPos			= FALSE;
	m_bReqBitSpdClampFPos		= FALSE;
	m_bReqBitSpdClampRPos		= FALSE;

	m_bRdyInPnp					= FALSE;
	m_bRdyRouterRun				= FALSE;
	m_bRdyRouterPrs				= FALSE;
	m_bRdyLoadCheck				= FALSE;
	m_bRdyRouterLiveVi			= FALSE;
	m_bRdyRouterCylBitClamp		= FALSE;
	m_bRdyRouterCylBitAlignF	= FALSE;
	m_bRdyRouterCylBitAlignR	= FALSE;
	m_bRdyRouterSpdBitClampF	= FALSE;
	m_bRdyRouterSpdBitClampR	= FALSE;
	m_bRdyRouterSpdBitEject		= FALSE;
	m_bRdyOutPnp				= FALSE;

	m_bRdyAdcInPnpPickUpPicker	= FALSE;
	m_bRdyAdcInPnpPickUpStage	= FALSE;
	m_bRdyAdcInPnpPickUpMask	= FALSE;
	m_bRdyAdcInPnpPutDnPicker	= FALSE;
	m_bRdyAdcInPnpPutDnStage	= FALSE;
	m_bRdyAdcInPnpPutDnMask		= FALSE;

	m_bRdyAdcOutPnpPickUpPicker	= FALSE;
	m_bRdyAdcOutPnpPutDnPicker	= FALSE;

}

	
//-------------------------------------------------------------------
void CIndex::AutoRun()
{
	m_bRdyInPnp					= FALSE;
	m_bRdyRouterRun				= FALSE;
	m_bRdyRouterPrs				= FALSE;
	m_bRdyLoadCheck				= FALSE;
	m_bRdyRouterLiveVi			= FALSE;
	m_bRdyRouterCylBitClamp		= FALSE;
	m_bRdyRouterCylBitAlignF	= FALSE;
	m_bRdyRouterCylBitAlignR	= FALSE;
	m_bRdyRouterSpdBitClampF	= FALSE;
	m_bRdyRouterSpdBitClampR	= FALSE;
	m_bRdyRouterSpdBitEject		= FALSE;
	m_bRdyOutPnp				= FALSE;

	m_bRdyAdcInPnpPickUpPicker	= FALSE;
	m_bRdyAdcInPnpPickUpStage	= FALSE;
	m_bRdyAdcInPnpPickUpMask	= FALSE;
	m_bRdyAdcInPnpPutDnPicker	= FALSE;
	m_bRdyAdcInPnpPutDnStage	= FALSE;
	m_bRdyAdcInPnpPutDnMask		= FALSE;

	m_bRdyAdcOutPnpPickUpPicker	= FALSE;
	m_bRdyAdcOutPnpPutDnPicker	= FALSE;

	if(!ExistPcb() && !ExistScrap() && !m_fsm.IsRun())
	{
		m_pMem->state				= 0;
		m_pMem->routerCmdCnt		= 0;
		m_pMem->routerCurCnt		= -1;
		m_pMem->compMaskClamp		= 0;
		m_pMem->compLoadCheck		= 0;
		m_pMem->compRouterPrs		= 0;
		m_pMem->compRouterLiveVi	= 0;
		m_pMem->compRouterRun		= 0;
		m_pMem->compMaskUnClamp		= 0;
		m_pMem->compPRSFail			= 0;
		m_pMem->compOutPnp			= 0;
	}

	if(!m_bRun)
		return;

	m_bRun = FALSE;

	if(m_fsm.IsRun())
		return;

	int nErrCode = g_err.GetNo();
	if(0 < nErrCode)
		return;

	// Cycle & Pos 확인
	if(!CanMove())
		return;
	
	// Kit 유무 확인
	if(!ExistErr())
		return;

	// Cylinder 초기 동작
	if(!InitCyl())
		return;

	if(ExistPcb())
	{
		m_pMem->state = PS_IN_PNP;

		if(IsSkip(m_nId))
			IndexFree(m_nId);

		if(m_pMem->compOutPnp)
		{
		}
		else if(m_pMem->compPRSFail)
		{
			m_pMem->state = PS_MASK_UNCLAMP;
		}
		else if(m_pMem->compMaskUnClamp)
		{
			m_pMem->state = PS_OUTPNP;
		}
		else if(m_pMem->compRouterRun)
		{
			if(INDEX_01 == m_nId)
			{
				if(INDEX_F == g_routerF.m_nCurIndex)
					g_routerF.m_nCurIndex = INDEX_IDLE;
			}
			else if(INDEX_02 == m_nId)
			{
				if(INDEX_R == g_routerF.m_nCurIndex)
					g_routerF.m_nCurIndex = INDEX_IDLE;
			}
			else if(INDEX_03 == m_nId)
			{
				if(INDEX_F == g_routerR.m_nCurIndex)
					g_routerR.m_nCurIndex = INDEX_IDLE;
			}
			else if(INDEX_04 == m_nId)
			{
				if(INDEX_R == g_routerR.m_nCurIndex)
					g_routerR.m_nCurIndex = INDEX_IDLE;
			}

			m_pMem->state = PS_MASK_UNCLAMP;
		}
		else if(m_pMem->compRouterLiveVi)
			m_pMem->state = PS_ROUTER_RUN;
		else if(m_pMem->compRouterPrs)
			m_pMem->state = PS_ROUTER_LIVE_VI;
		else if(m_pMem->compMaskClamp)
			m_pMem->state = PS_ROUTER_PRS;
		else if(m_pMem->compLoadCheck)
			m_pMem->state = PS_MASK_CLAMP;
		else 
			m_pMem->state = PS_LOAD_CHECK;
	}
	else
	{
		m_pMem->state = PS_IN_PNP;
		IndexFree(m_nId);
	}

	if(!m_pMtX->IsRdy() || !m_pMtT->IsRdy())
		return;

	if(INDEX_04 == m_nId)
	{
		if(g_err.m_bScrapSafetyBeam)
			return;
	}

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if(ExistPcb())
	{
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode) + 1);
		_sprintf(cMaterialType, L"PCB");
	}
	else
	{
		_sprintf(cMaterialId, L"$");
		_sprintf(cMaterialType, L"$");
	}

	_char cIndexId[_MAX_CHAR_SIZE_], cEventIdX[_MAX_CHAR_SIZE_], cEventIdT[_MAX_CHAR_SIZE_];
	_char cXPos[_MAX_CHAR_SIZE_], cTPos[_MAX_CHAR_SIZE_];

	_sprintf(cIndexId, L"INDEX%d", (m_nId + 1));

	switch(GetState())
	{
	case S_IDLE:
		if(!IsReadyMtIndexXInPnp() || !m_pMtT->InPos(PT_IN_PNP))
		{
			if(CanMove(PX_IN_PNP))
			{
				MoveMtIndexXInPnp();
				m_pMtT->Move(PT_IN_PNP);
			}
		}
		break;
	case S_IN_PNP:
		// mask가 있으면 Picker로 이동
		if(ExistKitMask())
		{
			_sprintf(cEventIdX, L"MT_UNCLAMP_INDEX_%02d_X_MASK_PICKER", (m_nId + 1));
			_sprintf(cEventIdT, L"MT_UNCLAMP_INDEX_%02d_T_MASK_PICKER", (m_nId + 1));

			if(!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
			{
				if(CanMove(PX_MASK_PICKER))
				{
					m_pMtX->Move(PX_MASK_PICKER);
					m_pMtT->Move(PT_MASK_PICKER);

					if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
					}

					if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
					}
				}
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
				}

				if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
				{
					g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
				}

				m_fsm.Set(C_MASK_PICKUP_START);	
			}
		}
		else
		{
			_sprintf(cEventIdX, L"MT_INDEX_%02d_X_INPNP", (m_nId + 1));
			_sprintf(cEventIdT, L"MT_INDEX_%02d_T_INPNP", (m_nId + 1));

			if(!IsReadyMtIndexXInPnp() || !m_pMtT->InPos(PT_IN_PNP))
			{
				if(CanMove(PX_IN_PNP))
				{
					MoveMtIndexXInPnp();
					m_pMtT->Move(PT_IN_PNP);

					if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_IN_PNP])
					{
						g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_IN_PNP] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
															g_data2c.cIndex[m_nId].T[PT_IN_PNP][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_IN_PNP][_POS_], 
															g_data2c.cIndex[m_nId].T[PT_IN_PNP][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_IN_PNP][_SPD_], 
															g_data2c.cIndex[m_nId].T[PT_IN_PNP][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_IN_PNP][_ACC_]))
					}
				}
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_IN_PNP])
				{
					g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_IN_PNP] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
														g_data2c.cIndex[m_nId].T[PT_IN_PNP][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_IN_PNP][_POS_], 
														g_data2c.cIndex[m_nId].T[PT_IN_PNP][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_IN_PNP][_SPD_], 
														g_data2c.cIndex[m_nId].T[PT_IN_PNP][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_IN_PNP][_ACC_]))
				}

				m_bRdyInPnp = TRUE;
			}
		}
		break;
	case S_LOAD_CHECK:
		if(!g_pNV->UseSkip(usLoadCheck) || PrsOnceSkip())
		{
			m_pMem->compLoadCheck = TRUE;
			break;
		}

		SetRouterCurIndex(m_nId);
		if(g_pNV->UseSkip(usLoadCheck))
		{
			POINT2D ptPos = GetRouterPrsPos(0);

			if(!m_pMtX->InPos(PX_ROUTER_PRS, ptPos.dX, 5) || !m_pMtT->InPos(PT_ROUTER_PRS))
			{
				if(CanMove(PX_ROUTER_PRS))
				{
					m_pMtX->PMove(PX_ROUTER_PRS, ptPos.dX);
					m_pMtT->Move(PT_ROUTER_PRS);
				}
			}
			else
			{
				m_bRdyLoadCheck = TRUE;
			}
		}
		break;
	case S_MASK_CLAMP:
		// Mask Picker Put Down
		if(ExistKitMask())
		{
			m_pMem->compMaskClamp = TRUE;
		}
		else
		{
			_sprintf(cEventIdX, L"MT_INDEX_%02d_X_MASK_PICKER", (m_nId + 1));
			_sprintf(cEventIdT, L"MT_INDEX_%02d_T_MASK_PICKER", (m_nId + 1));

			if(!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
			{
				if(CanMove(PX_MASK_PICKER))
				{
					m_pMtX->Move(PX_MASK_PICKER);
					m_pMtT->Move(PT_MASK_PICKER);

					if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
					}

					if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
					}
				}
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
				}

				if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
				{
					g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
				}

				m_fsm.Set(C_MASK_PUTDN_START);
			}
		}
		break;
	case S_ROUTER_PRS:
		if(INDEX_01 == m_nId || INDEX_02 == m_nId)
		{
			if(!g_pNV->UseSkip(usRouterPartF))
			{
				m_pMem->compRouterPrs		= TRUE;
				m_pMem->compRouterLiveVi	= TRUE;
				m_pMem->compRouterRun		= TRUE;
				break;
			}
		}
		else
		{
			if(!g_pNV->UseSkip(usRouterPartR))
			{
				m_pMem->compRouterPrs		= TRUE;
				m_pMem->compRouterLiveVi	= TRUE;
				m_pMem->compRouterRun		= TRUE;
				break;
			}
		}

		if((!g_pNV->UseSkip(usRouterPrs)) || PrsOnceSkip())
		{
			// PRS 1회성 Skip 기능으로 Router 진행 전에 PRS Data 초기화 해주도록 한다.
			if(PrsOnceSkip())
			{
				PrsOnceSkip() = 0;

				for(int n = 0; n < UNIT_MAX; n++)
				{
					m_pPrsResult->unit[n].dX = 0;
					m_pPrsResult->unit[n].dY = 0;
					m_pPrsResult->unit[n].dT = 0;
				}

				m_pPrsResult->block.dX = 0.0;
				m_pPrsResult->block.dY = 0.0;
				m_pPrsResult->block.dT = 0.0;
			}
			
			m_pMem->compRouterPrs = TRUE;
			break;
		}

		if(!ExistKitMask())
		{
			_sprintf(cEventIdX, L"MT_INDEX_%02d_X_MASK_PICKER", (m_nId + 1));
			_sprintf(cEventIdT, L"MT_INDEX_%02d_T_MASK_PICKER", (m_nId + 1));
			if(!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
			{
				if(CanMove(PX_MASK_PICKER))
				{
					m_pMtX->Move(PX_MASK_PICKER);
					m_pMtT->Move(PT_MASK_PICKER);

					if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
					}

					if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
					}
				}
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
				}

				if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
				{
					g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
				}

				m_fsm.Set(C_MASK_PUTDN_START);	
			}
		}
		else
		{
			SetRouterCurIndex(m_nId);

			if(g_pNV->UseSkip(usRouterPrs))
			{
				POINT2D ptPos = GetRouterPrsPos(0);

				_sprintf(cXPos, L"%03f", ptPos.dX);
				_sprintf(cEventIdX, L"MT_INDEX_%02d_X_PRS", (m_nId + 1));
				_sprintf(cEventIdT, L"MT_INDEX_%02d_T_PRS", (m_nId + 1));
				
				if(!m_pMtX->InPos(PX_ROUTER_PRS, ptPos.dX, 5) || !m_pMtT->InPos(PT_ROUTER_PRS))
				{
					if(CanMove(PX_ROUTER_PRS))
					{
						m_pMtX->PMove(PX_ROUTER_PRS, ptPos.dX);
						m_pMtT->Move(PT_ROUTER_PRS);

						if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_ROUTER_PRS])
						{
							g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_ROUTER_PRS] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start, 
																cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
																g_data2c.cIndex[m_nId].X[PX_ROUTER_PRS][_POSIDX_], cXPos, 
																g_data2c.cIndex[m_nId].X[PX_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_ROUTER_PRS][_SPD_], 
																g_data2c.cIndex[m_nId].X[PX_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_ROUTER_PRS][_ACC_]))
						}

						if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_ROUTER_PRS])
						{
							g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_ROUTER_PRS] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
											g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_POS_], 
											g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_SPD_], 
											g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_ACC_]))
						}
					}
				}
				else
				{
					if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_ROUTER_PRS])
					{
						g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_ROUTER_PRS] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
															g_data2c.cIndex[m_nId].X[PX_ROUTER_PRS][_POSIDX_], cXPos, 
															g_data2c.cIndex[m_nId].X[PX_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_ROUTER_PRS][_SPD_], 
															g_data2c.cIndex[m_nId].X[PX_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_ROUTER_PRS][_ACC_]))
					}

					if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_ROUTER_PRS])
					{
						g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_ROUTER_PRS] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
															g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_POS_], 
															g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_SPD_], 
															g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_ROUTER_PRS][_ACC_]))
					}

					m_bRdyRouterPrs = TRUE;
				}
			}
		}
		break;
	case S_ROUTER_LIVE_VI:
		{
			if(INDEX_01 == m_nId || INDEX_02 == m_nId)
			{
				if(!g_pNV->UseSkip(usRouterPartF))
				{
					m_pMem->compRouterLiveVi = TRUE;
					m_pMem->compRouterRun = TRUE;
					break;
				}
			}
			else
			{
				if(!g_pNV->UseSkip(usRouterPartR))
				{
					m_pMem->compRouterLiveVi = TRUE;
					m_pMem->compRouterRun = TRUE;
					break;
				}
			}

			if(!g_pNV->UseSkip(usRouterLiveVision))
			{
				m_pMem->compRouterLiveVi = TRUE;
				break;
			}

			if(!ExistKitMask())
			{
				if(!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
				{
					if(CanMove(PX_MASK_PICKER))
					{
						m_pMtX->Move(PX_MASK_PICKER);
						m_pMtT->Move(PT_MASK_PICKER);
					}
				}
				else
				{
					m_fsm.Set(C_MASK_PUTDN_START);	
				}
			}
			else
			{
				SetRouterCurIndex(m_nId);

				XYT ptXYT = GetRouterLiveViPos(0, POS_START);

				if(!m_pMtX->InPos(PX_ROUTER_LIVE_VI, ptXYT.dX, 5) || 
					!m_pMtT->InPos(PT_ROUTER_LIVE_VI, ptXYT.dT, 5))
				{
					if(CanMove(PX_ROUTER_LIVE_VI))
					{
						m_pMtX->PMove(PX_ROUTER_LIVE_VI, ptXYT.dX);
						m_pMtT->PMove(PT_ROUTER_LIVE_VI, ptXYT.dT);
					}
				}
				else
					m_bRdyRouterLiveVi = TRUE;
			}
		}
		break;
	case S_ROUTER_RUN:
		{
			if(INDEX_01 == m_nId || INDEX_02 == m_nId)
			{
				if(!g_pNV->UseSkip(usRouterPartF))
				{
					m_pMem->compRouterRun = TRUE;
					break;
				}
			}
			else
			{
				if(!g_pNV->UseSkip(usRouterPartR))
				{
					m_pMem->compRouterRun = TRUE;
					break;
				}
			}

			if(!ExistKitMask())
			{
				_sprintf(cEventIdX, L"MT_INDEX_%02d_X_MASK_PICKER", (m_nId + 1));
				_sprintf(cEventIdT, L"MT_INDEX_%02d_T_MASK_PICKER", (m_nId + 1));

				if(!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
				{
					if(CanMove(PX_MASK_PICKER))
					{
						m_pMtX->Move(PX_MASK_PICKER);
						m_pMtT->Move(PT_MASK_PICKER);

						if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
						{
							g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start, 
																cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
																g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
																g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
																g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
						}

						if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
						{
							g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = TRUE;
							NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start, 
																cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
																g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
																g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
																g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
						}
					}
				}
				else
				{
					if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
					}

					if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
					}

					m_fsm.Set(C_MASK_PUTDN_START);	
				}
			}
			else
			{
				SetRouterCurIndex(m_nId);

				XYT ptXYT = GetRouterPos(m_pMem->routerCmdCnt, POS_START);

				_sprintf(cXPos, L"%03f", ptXYT.dX);
				_sprintf(cTPos, L"%03f", ptXYT.dT);
				
				if(!m_pMtX->InPos(PX_ROUTER_RUN, ptXYT.dX, 5) || !m_pMtT->InPos(PT_ROUTER_RUN, ptXYT.dT, 5))
				{
					if(CanMove(PX_ROUTER_RUN))
					{
						m_pMtX->PMove(PX_ROUTER_RUN, ptXYT.dX);
						m_pMtT->PMove(PT_ROUTER_RUN, ptXYT.dT);

						if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_ROUTER_RUN])
						{
							g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_ROUTER_RUN] = TRUE;
							_sprintf(cEventIdX, L"MT_INDEX_%02d_X_CUT_01_START", (m_nId + 1));
							NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start, 
																cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
																g_data2c.cIndex[m_nId].X[PX_ROUTER_RUN][_POSIDX_], cXPos, 
																g_data2c.cIndex[m_nId].X[PX_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_ROUTER_RUN][_SPD_], 
																g_data2c.cIndex[m_nId].X[PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_ROUTER_RUN][_ACC_]))
						}

						if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_ROUTER_RUN])
						{
							g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_ROUTER_RUN] = TRUE;
							_sprintf(cEventIdT, L"MT_INDEX_%02d_T_CUT_01_START", (m_nId + 1));
							NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start, 
																cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
																g_data2c.cIndex[m_nId].T[PT_ROUTER_RUN][_POSIDX_], cTPos, 
																g_data2c.cIndex[m_nId].T[PT_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_ROUTER_RUN][_SPD_], 
																g_data2c.cIndex[m_nId].T[PT_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_ROUTER_RUN][_ACC_]))
						}
					}
				}
				else
				{
					m_bRdyRouterRun = TRUE;

					if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_ROUTER_RUN] = FALSE;
						_sprintf(cEventIdX, L"MT_INDEX_%02d_X_CUT_01_START", (m_nId + 1));
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
															g_data2c.cIndex[m_nId].X[PX_ROUTER_RUN][_POSIDX_], cXPos, 
															g_data2c.cIndex[m_nId].X[PX_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_ROUTER_RUN][_SPD_], 
															g_data2c.cIndex[m_nId].X[PX_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_ROUTER_RUN][_ACC_]))
					}
					if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_ROUTER_RUN])
					{
						g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_ROUTER_RUN] = FALSE;
						_sprintf(cEventIdT, L"MT_INDEX_%02d_T_CUT_01_START", (m_nId + 1));
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
															g_data2c.cIndex[m_nId].T[PT_ROUTER_RUN][_POSIDX_], cTPos, 
															g_data2c.cIndex[m_nId].T[PT_ROUTER_RUN][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_ROUTER_RUN][_SPD_], 
															g_data2c.cIndex[m_nId].T[PT_ROUTER_RUN][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_ROUTER_RUN][_ACC_]))					}
				}
			}
		}
		break;
	case S_MASK_UNCLAMP:
		// Mask Picker Pickup
		if(!ExistKitMask())
		{
			if(m_pMem->compPRSFail)
			{
				if(CanMove(PX_IN_PNP))
				{
					MoveMtIndexXInPnp();
					m_pMtT->Move(PT_IN_PNP);
					g_err.Save(ER_VI_INDEX01_BLOCK_PRS_RESULT_LIMIT+m_nId);
					m_pMem->compPRSFail = FALSE;
				}
			}
			else
			{
				m_pMem->compMaskUnClamp = TRUE;
			}
		}
		else
		{
			_sprintf(cEventIdX, L"MT_UNCLAMP_INDEX_%02d_X_MASK_PICKER", (m_nId + 1));
			_sprintf(cEventIdT, L"MT_UNCLAMP_INDEX_%02d_T_MASK_PICKER", (m_nId + 1));

			if (!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
			{
				if (CanMove(PX_MASK_PICKER))
				{
					m_pMtX->Move(PX_MASK_PICKER);
					m_pMtT->Move(PT_MASK_PICKER);

					if (!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start,
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo],
							g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_],
							g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_],
							g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
					}

					if (!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start,
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo],
							g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_],
							g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_],
							g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
					}
				}
			}
			else
			{
				if (g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end,
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo],
						g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_],
						g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_],
						g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
				}

				if (g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
				{
					g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end,
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo],
						g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_],
						g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_],
						g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
				}
				m_fsm.Set(C_MASK_PICKUP_START);
			}
		}
		break;
	case S_OUTPNP:
		if(ExistKitMask())
		{
			_sprintf(cEventIdX, L"MT_UNCLAMP_INDEX_%02d_X_MASK_PICKER", (m_nId + 1));
			_sprintf(cEventIdT, L"MT_UNCLAMP_INDEX_%02d_T_MASK_PICKER", (m_nId + 1));
			
			if(!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
			{
				if(CanMove(PX_MASK_PICKER))
				{
					m_pMtX->Move(PX_MASK_PICKER);
					m_pMtT->Move(PT_MASK_PICKER);

					if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
															g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
					}

					if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
					{
						g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
															g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
					}
				}
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
														g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
				}

				if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
				{
					g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
														g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
				}

				m_fsm.Set(C_MASK_PICKUP_START);
			}
		}
		else
		{
			_sprintf(cEventIdX, L"MT_INDEX_%02d_X_OUTPNP", (m_nId + 1));
			_sprintf(cEventIdT, L"MT_INDEX_%02d_T_OUTPNP", (m_nId + 1));

			if(!m_pMtX->InPos(PX_OUT_PNP_WAIT, 1000) || !m_pMtT->InPos(PT_OUT_PNP_WAIT, 1000))
			{
				if(CanMove(PX_OUT_PNP_WAIT))
				{
					m_pMtX->Move(PX_OUT_PNP_WAIT);
					m_pMtT->Move(PT_OUT_PNP_WAIT);

					if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_OUT_PNP_WAIT])
					{
						g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_OUT_PNP_WAIT] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
															g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_POS_], 
															g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_SPD_], 
															g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_ACC_]))
					}

					if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_OUT_PNP_WAIT])
					{
						g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_OUT_PNP_WAIT] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start,
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
															g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_POS_], 
															g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_SPD_], 
															g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_ACC_]))
					}
				}
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_OUT_PNP_WAIT])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_OUT_PNP_WAIT] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
														g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_POS_], 
														g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_SPD_], 
														g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_OUT_PNP_WAIT][_ACC_]))
				}
				if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_OUT_PNP_WAIT])
				{
					g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_OUT_PNP_WAIT] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end,
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
														g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_POS_], 
														g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_SPD_], 
														g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_OUT_PNP_WAIT][_ACC_]))
				}

				if(g_pNV->UseSkip(usTcServer))
				{
					if((NULL == g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].mergeLotID[0]))
					{
						g_err.Save(ER_LOT_INFO_NOT_EXIST_INDEX01 + m_nId);
						break;
					}

					BOOL bDiff  = (0 != _stricmp(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].mergeLotID));
						 bDiff &= (NULL != g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].mergeLotID[0]);
					
					if(bDiff)
					{
						// MGZ을 제외한 All Lot 정보 비교 - Merge Lot 정보가 같은 것이 없으면 flow
						BOOL bRun = TRUE;

						if((NULL != g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID[0]) && g_rail.Exist())
							bRun &= (0 != _stricmp(g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID));
						if((NULL != g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].mergeLotID[0]) && g_inPnp.ExistPcb())
							bRun &= (0 != _stricmp(g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].mergeLotID));
						if((NULL != g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01].mergeLotID[0]) && g_index01.ExistPcb())
							bRun &= (0 != _stricmp(g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01].mergeLotID));
						if((NULL != g_pNV->m_pLotInfo->data[LOT_INFO_INDEX02].mergeLotID[0]) && g_index02.ExistPcb())
							bRun &= (0 != _stricmp(g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX02].mergeLotID));
						if((NULL != g_pNV->m_pLotInfo->data[LOT_INFO_INDEX03].mergeLotID[0]) && g_index03.ExistPcb())
							bRun &= (0 != _stricmp(g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX03].mergeLotID));
						if((NULL != g_pNV->m_pLotInfo->data[LOT_INFO_INDEX04].mergeLotID[0]) && g_index04.ExistPcb())
							bRun &= (0 != _stricmp(g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX04].mergeLotID));

						if(bRun)
						{
							if(INDEX_IDLE == g_outPnp.m_nCurIndex)
								g_outPnp.m_nCurIndex = m_nId;
					
							m_bRdyOutPnp = TRUE;
						}
						else if(g_outPnp.m_nCurIndex == m_nId)
						{
							g_outPnp.m_nCurIndex = INDEX_IDLE;
						}
					}
					else
					{
						if(INDEX_IDLE == g_outPnp.m_nCurIndex)
							g_outPnp.m_nCurIndex = m_nId;
					
						m_bRdyOutPnp = TRUE;
					}
				}
				else
				{
					if(INDEX_IDLE == g_outPnp.m_nCurIndex)
						g_outPnp.m_nCurIndex = m_nId;
					
					m_bRdyOutPnp = TRUE;
				}
			}
		}
		break;
	case S_BIT_SUPPLY_BOX:
		{
			int nBitCurCnt = 0;
			
			if(INDEX_01 == m_nId)
			{
				nBitCurCnt = g_pNV->NDm(RedindexBitBoxCurCnt01);
				if(nBitCurCnt < 0)
				{
					g_err.Save(ER_INDEX01_BIT_BOX_EMPTY);
					break;
				}
			}
			else if(INDEX_04 == m_nId)
			{
				nBitCurCnt = g_pNV->NDm(RedindexBitBoxCurCnt04);
				if(nBitCurCnt < 0)
				{
					g_err.Save(ER_INDEX04_BIT_BOX_EMPTY);
					break;
				}
			}
			else if (INDEX_02 == m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				nBitCurCnt = g_pNV->NDm(BlueindexBitBoxCurCnt02);
				if (nBitCurCnt < 0)
				{
					g_err.Save(ER_INDEX02_BIT_BOX_EMPTY);
					break;
				}
			}
			else if (INDEX_03 == m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			{
				nBitCurCnt = g_pNV->NDm(BlueindexBitBoxCurCnt03);
				if (nBitCurCnt < 0)
				{
					g_err.Save(ER_INDEX03_BIT_BOX_EMPTY);
					break;
				}
			}
			/*else
			{
				break;
			}*/

			m_bRdyRouterCylBitClamp = TRUE;
		}
		break;
	case S_BIT_EJECT_BOX:
		if(INDEX_01 != m_nId && INDEX_04 != m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
			break;

		m_bRdyRouterSpdBitEject = TRUE;
		break;
	case S_BIT_ALIGN_F:
		if(INDEX_01 != m_nId && INDEX_04 != m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
			break;

		m_bRdyRouterCylBitAlignF = TRUE;
		break;
	case S_BIT_ALIGN_R:
		if(INDEX_01 != m_nId && INDEX_04 != m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
			break;

		m_bRdyRouterCylBitAlignR = TRUE;
		break;
	case S_BIT_SPINDLE_CLAMP_F:
		if(INDEX_01 != m_nId && INDEX_04 != m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
			break;

		m_bRdyRouterSpdBitClampF = TRUE;
		break;
	case S_BIT_SPINDLE_CLAMP_R:
		if(INDEX_01 != m_nId && INDEX_04 != m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
			break;

		m_bRdyRouterSpdBitClampR = TRUE;
		break;
	case S_ADC_WAIT_IN_PNP_PUTDN:
		if(!m_pMtX->InPos(PX_ADC_WAIT, 1000) || !m_pMtT->InPos(PT_ADC_WAIT, 1000))
		{
			if(CanMove(PX_ADC_WAIT))
			{
				m_pMtX->Move(PX_ADC_WAIT);
				m_pMtT->Move(PT_ADC_WAIT);
			}
		}
		else
		{
			// Mask 가 있는 상태에서 Stage를 안착하거나 OutPnp Kit을 받으면 안됨
			if(g_inPnp.ExistKit() && (ADC_KIT_PICKER == g_inPnp.KitInfo()))
				m_bRdyAdcInPnpPutDnPicker = TRUE;
			else if(!ExistKitStage())
				m_bRdyAdcInPnpPutDnStage = TRUE;
			else if(!ExistKitMask())
				m_bRdyAdcInPnpPutDnMask = TRUE;
		}
		break;
	case S_ADC_WAIT_IN_PNP_PICKUP:
		if(!m_pMtX->InPos(PX_ADC_WAIT, 1000) || !m_pMtT->InPos(PT_ADC_WAIT, 1000))
		{
			if(CanMove(PX_ADC_WAIT))
			{
				m_pMtX->Move(PX_ADC_WAIT);
				m_pMtT->Move(PT_ADC_WAIT);
			}
		}
		else
		{
			int nReturnNo = g_adc.GetAdcIndexReturnNo();

			if(INDEX_01 == m_nId)
			{
				if(ExistKitMovePicker()) //Mask 위에 Picker가 있는 상태
				{
					if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndexMovePickerJobType)))
						m_bRdyAdcInPnpPickUpPicker = TRUE;
				}
				else
				{
					if(ADC_KIT_STAGE_01 == nReturnNo)
						m_bRdyAdcInPnpPickUpStage = TRUE;
					else if(ADC_KIT_MASK_01 == nReturnNo)
						m_bRdyAdcInPnpPickUpMask = TRUE;
				}
			}
			else if(INDEX_02 == m_nId)
			{
				if(ADC_KIT_STAGE_02 == nReturnNo)
					m_bRdyAdcInPnpPickUpStage = TRUE;
				else if(ADC_KIT_MASK_02 == nReturnNo)
					m_bRdyAdcInPnpPickUpMask = TRUE;
			}
			else if(INDEX_03 == m_nId)
			{
				if(ADC_KIT_STAGE_03 == nReturnNo)
					m_bRdyAdcInPnpPickUpStage = TRUE;
				else if(ADC_KIT_MASK_03 == nReturnNo)
					m_bRdyAdcInPnpPickUpMask = TRUE;
			}
			else if(INDEX_04 == m_nId)
			{
				if(ADC_KIT_STAGE_04 == nReturnNo)
					m_bRdyAdcInPnpPickUpStage = TRUE;
				else if(ADC_KIT_MASK_04 == nReturnNo)
					m_bRdyAdcInPnpPickUpMask = TRUE;
			}
		}
		break;
	case S_ADC_WAIT_OUT_PNP_PUTDN:
		if(!m_pMtX->InPos(PX_ADC_WAIT, 1000) || !m_pMtT->InPos(PT_ADC_WAIT, 1000))
		{
			if(CanMove(PX_ADC_WAIT))
			{
				m_pMtX->Move(PX_ADC_WAIT);
				m_pMtT->Move(PT_ADC_WAIT);
			}
		}
		else
		{
			if(INDEX_01 != m_nId)
				break;

			BOOL bInPnpBusy  = g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_MASK_01);
			     bInPnpBusy |= g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_STAGE_01);
			     bInPnpBusy |= g_inPnp.m_pMtY->ComparePos(CInPnp::PY_PUTDN_01);

			if(bInPnpBusy)
			{
				if(g_inPnp.ExistKit())
					break;
			}
 			m_bRdyAdcOutPnpPutDnPicker = TRUE;
		}
		break;
	case S_ADC_WAIT_OUT_PNP_PICKUP:
		if(!m_pMtX->InPos(PX_ADC_WAIT, 1000) || !m_pMtT->InPos(PT_ADC_WAIT, 1000))
		{
			if(CanMove(PX_ADC_WAIT))
			{
				m_pMtX->Move(PX_ADC_WAIT);
				m_pMtT->Move(PT_ADC_WAIT);
			}
		}
		else
		{
			if(INDEX_01 != m_nId)
				break;

			BOOL bInPnpBusy  = g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_MASK_01);
			     bInPnpBusy |= g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_STAGE_01);
			     bInPnpBusy |= g_inPnp.m_pMtY->ComparePos(CInPnp::PY_PUTDN_01);

			if(bInPnpBusy)
			{
				if(g_inPnp.ExistKit())
					break;
			}

			if(!g_outPnp.ExistKit())
				m_bRdyAdcOutPnpPickUpPicker = TRUE;
			else
				g_err.Save(ER_ADC_KIT_EXIST_OUTPNP);
		}		
		break;
	case S_ADC_MASK_PICKER_PICKUP:
		if(!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
		{
			if(CanMove(PX_MASK_PICKER))
			{
				m_pMtX->Move(PX_MASK_PICKER);
				m_pMtT->Move(PT_MASK_PICKER);
			}
		}
		else
		{
			m_fsm.Set(C_MASK_PICKUP_START);
		}
		break;
	case S_ADC_MASK_PICKER_PUTDN:
		if(!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
		{
			if(CanMove(PX_MASK_PICKER))
			{
				m_pMtX->Move(PX_MASK_PICKER);
				m_pMtT->Move(PT_MASK_PICKER);
			}
		}
		else
		{
			m_fsm.Set(C_MASK_PUTDN_START);
		}
		break;
	case S_ADC_IDLE:
		if(!m_pMtX->InPos(PX_ADC_WAIT) || !m_pMtT->InPos(PT_ADC_WAIT))
		{
			if(CanMove(PX_ADC_WAIT))
			{
				m_pMtX->Move(PX_ADC_WAIT);
				m_pMtT->Move(PT_ADC_WAIT);
			}
		}
		break;
	}
}


//-------------------------------------------------------------------
void CIndex::CycleRun(void)
{
	if(!m_fsm.IsRun())
		return;

	if(m_fsm.IsStop())
		return;

	if(C_ERROR == m_fsm.Get())
	{
		if(0 < m_fsm.GetMsg())
			g_err.Save(m_fsm.GetMsg());

		m_fsm.Set(C_IDLE);
		return;
	}
	else if(IsErr())
	{
		m_fsm.Set(C_ERROR);
		return;
	}

	CycleRunMaskPickerPickUp();
	CycleRunMaskPickerPutDn();
}


//-------------------------------------------------------------------
void CIndex::Init(int nNo, INDEX_SYS_TEACH* pSysTeach, INDEX_MEMORY* pIndexMemory, POINT2D* pRouterOrgPos)
{
	m_nId			= nNo;
	m_pSysTeach		= pSysTeach;
	m_pMem			= pIndexMemory;
	m_pRouterOrgPos	= pRouterOrgPos;

	m_pGbPrsBlockPos = (GERBER_BLOCK_PRS_POS*)&g_pNV->gerberPara(prsBlockPos);
	m_pGbPrsUnitPos  = (GERBER_PRS_UNIT_POS*)&g_pNV->gerberPara(prsUnitPos);
}


//-------------------------------------------------------------------
void CIndex::Init2(PRS_RESULT* pPrsResult)
{
	m_pPrsResult = pPrsResult;
}


//-------------------------------------------------------------------
void CIndex::SetHW(CMtAXL* pMtX, CMtAXL* pMtT, CPneumatic* pPmMaskFBL, CPneumatic* pPmMaskFBR, CPneumatic* pPmMaskUD)
{
	m_pMtX			= pMtX;
	m_pMtT			= pMtT;
	m_pCylMaskFB_L	= pPmMaskFBL;
	m_pCylMaskFB_R	= pPmMaskFBR;
	m_pCylMaskUD	= pPmMaskUD;
}


//-------------------------------------------------------------------
void CIndex::SetHW2(CPneumatic* pCylMaskPickerUD, CPneumatic* pCylMaskPickerOC)
{
	m_pCylMaskPickerUD = pCylMaskPickerUD;
	m_pCylMaskPickerOC = pCylMaskPickerOC;
}


//-------------------------------------------------------------------
void CIndex::SetHW3(CPneumatic* pSolStageKitOC, CPneumatic* pCylDustShutterOC)
{
	m_pSolStageKitOC = pSolStageKitOC;
	m_pCylDustShutterOC = pCylDustShutterOC;
}


void CIndex::SetIndexPosReq(int nReqNo)
{
	m_bReqBitSupplyPos		= FALSE;
	m_bReqBitEjectPos		= FALSE;
	m_bReqBitAlignFPos		= FALSE;
	m_bReqBitAlignRPos		= FALSE;
	m_bReqBitSpdClampFPos	= FALSE;
	m_bReqBitSpdClampRPos	= FALSE;

	if(REQ_BIT_SUPPLY_BOX == nReqNo)
		m_bReqBitSupplyPos = TRUE;
	else if(REQ_BIT_EJECT_BOX == nReqNo)
		m_bReqBitEjectPos = TRUE;
	else if(REQ_BIT_ALIGN_F == nReqNo)
		m_bReqBitAlignFPos = TRUE;
	else if(REQ_BIT_ALIGN_R == nReqNo)
		m_bReqBitAlignRPos = TRUE;
	else if(REQ_BIT_SPINDLE_CLAMP_F == nReqNo)
		m_bReqBitSpdClampFPos = TRUE;
	else if(REQ_BIT_SPINDLE_CLAMP_R == nReqNo)
		m_bReqBitSpdClampRPos = TRUE;
}


//-------------------------------------------------------------------
int& CIndex::ExistPcb(void)
{
	return (g_pNV->m_pData->ndm[(existIndex01 + m_nId)]);
}


//-------------------------------------------------------------------
int& CIndex::ExistScrap(void)
{
	return (g_pNV->m_pData->ndm[(existIndexScrap01 + m_nId)]);
}


//-------------------------------------------------------------------
int& CIndex::ExistKitStage(void)
{
	return (g_pNV->m_pData->ndm[(existKitStage01 + m_nId)]);
}


//-------------------------------------------------------------------
int& CIndex::ExistKitMask(void)
{
	return (g_pNV->m_pData->ndm[(existKitMask01 + m_nId)]);
}


//-------------------------------------------------------------------
int& CIndex::ExistKitMovePicker(void)
{
	return (g_pNV->m_pData->ndm[existKitMovePicker]);
}


//-------------------------------------------------------------------
int& CIndex::ExistKitMaskPicker(void)
{
	return (g_pNV->m_pData->ndm[(existKitMaskPicker01 + m_nId)]);
}


//-------------------------------------------------------------------
int& CIndex::PrsOnceSkip(void)
{
	return (g_pNV->m_pData->ndm[(mmiIndex01PrsOnceSkip + m_nId)]);
}


//-------------------------------------------------------------------
BOOL CIndex::CanMove(int nTargetIndex)
{
	/////////////////////////////////////////////////////////////////////
	// InPnp 안전관련
	BOOL bInPnpRun  = g_inPnp.m_pMtY->ComparePos(CInPnp::PY_PUTDN_01 + m_nId);
		 bInPnpRun |= g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_MASK_01 + m_nId);
		 bInPnpRun |= g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_STAGE_01 + m_nId);

	if(bInPnpRun)
	{
		if(!g_inPnp.m_pMtZ->InPos(CInPnp::PZ_READY))
			return (FALSE);
		if(g_inPnp.m_fsm.Between(CInPnp::C_PCB_PUTDN_START, CInPnp::C_PCB_PUTDN_END))
			return (FALSE);
		if(g_inPnp.m_fsm.Between(CInPnp::C_ADC_INDEX_PICKUP_START, CInPnp::C_ADC_INDEX_PICKUP_END))
			return (FALSE);
		if(g_inPnp.m_fsm.Between(CInPnp::C_ADC_INDEX_PUTDN_START, CInPnp::C_ADC_INDEX_PUTDN_END))
			return (FALSE);
	}

	/////////////////////////////////////////////////////////////////////
	// Router 안전관련 
	int nRouterIndexNo = m_nId % 2; // 나머지
	int nRouterPart	   = m_nId / 2; // 몫

	if(ROUTER_PART_F == nRouterPart)
	{
		if(INDEX_F == nRouterIndexNo)
		{
			// Index 01 에서 Router Y 01 의 간섭 조건
			BOOL bMtYPos  = g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_RED);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_RED_F);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_RED_R);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_VI_PRS_F);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_VI_LIVE_F);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_ROUTER_F);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_01_03_RED_F);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_01_03_RED_F);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_02_04_RED_R);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_02_04_RED_R);

			if(bMtYPos)
			{
				if(pmUP != g_routerF.m_pCylBitClampUD->GetPos(300))
					return (FALSE);
				if(!g_routerF.m_pMtZ_F->IsRdy(CRouter::PZ_READY) || !g_routerF.m_pMtZ_F->InPos(CRouter::PZ_READY))
					return (FALSE);
				if(!g_routerF.m_pMtZ_R->IsRdy(CRouter::PZ_READY) || !g_routerF.m_pMtZ_R->InPos(CRouter::PZ_READY))
					return (FALSE);
				if(g_routerF.m_fsm.IsRun())
					return (FALSE);
			}
		}
		else // Index Rear Part
		{
			// Index 02 에서 Router Y 01 의 간섭 조건
			BOOL bMtYPos  = g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_VI_PRS_R);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_VI_LIVE_R);
				 bMtYPos |= g_routerF.m_pMtY->m_profile.cmdIndex == (CRouter::PY_ROUTER_R);

			if(bMtYPos)
			{
				if(pmUP != g_routerF.m_pCylBitClampUD->GetPos(300))
					return (FALSE);
				if(!g_routerF.m_pMtZ_F->IsRdy(CRouter::PZ_READY) || !g_routerF.m_pMtZ_F->InPos(CRouter::PZ_READY))
					return (FALSE);
				if(!g_routerF.m_pMtZ_R->IsRdy(CRouter::PZ_READY) || !g_routerF.m_pMtZ_R->InPos(CRouter::PZ_READY))
					return (FALSE);	
				if(g_routerF.m_fsm.IsRun())
					return (FALSE);
			}
		}
	}
	else // Router Rear Part
	{		
		if(INDEX_F == nRouterIndexNo)
		{
			// Index 03 에서 Router Y 02 의 간섭 조건
			BOOL bMtYPos  = g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_VI_PRS_F);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_VI_LIVE_F);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_ROUTER_F);

			if(bMtYPos)
			{
				if(pmUP != g_routerR.m_pCylBitClampUD->GetPos(300))
					return (FALSE);
				if(!g_routerR.m_pMtZ_F->IsRdy(CRouter::PZ_READY) || !g_routerR.m_pMtZ_F->InPos(CRouter::PZ_READY))
					return (FALSE);
				if(!g_routerR.m_pMtZ_R->IsRdy(CRouter::PZ_READY) || !g_routerR.m_pMtZ_R->InPos(CRouter::PZ_READY))
					return (FALSE);
				if(g_routerR.m_fsm.IsRun())
					return (FALSE);
			}
		}
		else
		{
			// Index 04 에서 Router Y 02 의 간섭 조건
			BOOL bMtYPos  = g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_CLAMP_RED);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_RED_F);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_CYL_BIT_ALIGN_RED_R);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_VI_PRS_R);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_VI_LIVE_R);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_ROUTER_R);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_01_03_RED_F);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_01_03_RED_F);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_EJECT_02_04_RED_R);
				 bMtYPos |= g_routerR.m_pMtY->m_profile.cmdIndex == (CRouter::PY_SPD_BIT_CLAMP_02_04_RED_R);

			if(bMtYPos)
			{
				if(pmUP != g_routerR.m_pCylBitClampUD->GetPos(300))
					return (FALSE);
				if(!g_routerR.m_pMtZ_F->IsRdy(CRouter::PZ_READY) || !g_routerR.m_pMtZ_F->InPos(CRouter::PZ_READY))
					return (FALSE);
				if(!g_routerR.m_pMtZ_R->IsRdy(CRouter::PZ_READY) || !g_routerR.m_pMtZ_R->InPos(CRouter::PZ_READY))
					return (FALSE);	
				if(g_routerR.m_fsm.IsRun())
					return (FALSE);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////
	// OutPnp 안전관련
	BOOL bOutPnpRun = g_outPnp.m_pMtY->ComparePos(COutPnp::PY_PICKUP_01 + m_nId);

	if(bOutPnpRun)
	{
		if(!g_outPnp.m_pMtZ->IsRdy(COutPnp::PZ_READY))
			return (FALSE);
		if(g_outPnp.m_fsm.Between(COutPnp::C_PICKUP_START, COutPnp::C_PICKUP_END))
			return (FALSE);
	}

	bOutPnpRun = g_outPnp.m_pMtY->ComparePos(COutPnp::PY_ADC_KIT_CLAMP);

	if(m_nId == INDEX_01)
	{
		if(bOutPnpRun)
		{
			if(!g_outPnp.m_pMtZ->IsRdy(COutPnp::PZ_READY))
				return (FALSE);
			if(g_outPnp.m_fsm.Between(COutPnp::C_ADC_PICKUP_START, COutPnp::C_ADC_PICKUP_END))
				return (FALSE);
			if(g_outPnp.m_fsm.Between(COutPnp::C_ADC_PUTDN_START, COutPnp::C_ADC_PUTDN_END))
				return (FALSE);
		}
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyInPnp(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyInPnp);
	}
	else
	{
		BOOL bRdy = !m_fsm.IsRun();
			 bRdy &= IsReadyMtIndexXInPnp();
			 bRdy &= m_pMtT->IsRdy(PT_IN_PNP) && m_pMtT->InPos(PT_IN_PNP);
			 bRdy &= IsCylMaskFixUD(pmUP, 300);
			 bRdy &= IsCylMaskFixFB(pmBWD, 300);
			 bRdy &= !ExistPcb() && !ExistScrap();
			 bRdy &= g_pNV->UseSkip(usIndex01 + m_nId);
			 bRdy &= g_pNV->UseSkip(usRouterPartF + (m_nId/2));

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyRouter(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyRouterRun);		
	}
	else
	{
		BOOL bRdy = !m_fsm.IsRun();
			 bRdy &= m_pMtX->IsRdy(PX_ROUTER_RUN) && m_pMtX->InPos(PX_ROUTER_RUN);
			 bRdy &= m_pMtT->IsRdy(PT_ROUTER_RUN) && m_pMtT->InPos(PT_ROUTER_RUN);
			 bRdy &= IsCylMaskFixUD(pmDOWN, 300);
			 bRdy &= IsCylMaskFixFB(pmFWD, 300);
			 bRdy &= ExistPcb();
			 bRdy &= g_pNV->UseSkip(usIndex01 + m_nId);
			 bRdy &= g_pNV->UseSkip(usRouterPartF + (m_nId/2));

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyRouterPrs(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyRouterPrs);
	}
	else
	{
		BOOL bRdy = !m_fsm.IsRun();
			 bRdy &= m_pMtX->IsRdy(PX_ROUTER_PRS) && m_pMtX->InPos(PX_ROUTER_PRS);
			 bRdy &= m_pMtT->IsRdy(PT_ROUTER_PRS) && m_pMtT->InPos(PT_ROUTER_PRS);
			 bRdy &= IsCylMaskFixUD(pmDOWN, 300);
			 bRdy &= IsCylMaskFixFB(pmFWD, 300);
			 bRdy &= ExistPcb();
			 bRdy &= g_pNV->UseSkip(usRouterPrs);
			 bRdy &= g_pNV->UseSkip(usIndex01 + m_nId);

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyLoadCheck(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyLoadCheck);
	}
	else
	{
		BOOL bRdy = !m_fsm.IsRun();
		bRdy &= m_pMtX->IsRdy(PX_ROUTER_PRS) && m_pMtX->InPos(PX_ROUTER_PRS);
		bRdy &= m_pMtT->IsRdy(PT_ROUTER_PRS) && m_pMtT->InPos(PT_ROUTER_PRS);
		bRdy &= ExistPcb();
		bRdy &= g_pNV->UseSkip(usLoadCheck);
		bRdy &= g_pNV->UseSkip(usIndex01 + m_nId);

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyLiveVision(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyRouterLiveVi);
	}
	else
	{
		// PRS가 완료된 후 검사위치로 이동시에 연산된 Pos로 이동 될 수 있도록 연산식 추가
		BOOL bRdy = !m_fsm.IsRun();
			 bRdy &= m_pMtX->IsRdy(PX_ROUTER_LIVE_VI) && m_pMtX->InPos(PX_ROUTER_LIVE_VI);
			 bRdy &= m_pMtT->IsRdy(PT_ROUTER_LIVE_VI) && m_pMtT->InPos(PT_ROUTER_LIVE_VI);
			 bRdy &= IsCylMaskFixUD(pmDOWN, 300);
			 bRdy &= IsCylMaskFixFB(pmFWD, 300);
			 bRdy &= ExistPcb();
			 bRdy &= g_pNV->UseSkip(usRouterLiveVision);
			 bRdy &= g_pNV->UseSkip(usIndex01 + m_nId);

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyBitClamp(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyRouterCylBitClamp);
	}
	else
	{
		if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
		{
			if (INDEX_02 == m_nId || INDEX_03 == m_nId)
				return (FALSE);
		}

		// Bit clamp 연산된 Pos로 이동 될 수 있도록 연산식 추가
		BOOL bRdy = !m_fsm.IsRun();
			 //bRdy &= m_pMtX->IsRdy(PX_CYL_BIT_SUPPLY_BOX) && m_pMtX->InPos(PX_CYL_BIT_SUPPLY_BOX);
			 //bRdy &= m_pMtT->IsRdy(PT_CYL_BIT_SUPPLY_BOX) && m_pMtT->InPos(PT_CYL_BIT_SUPPLY_BOX);
	
		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyBitAlignF(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyRouterCylBitAlignF);
	}
	else
	{
		if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
		{
			if (INDEX_02 == m_nId || INDEX_03 == m_nId)
				return (FALSE);
		}

		BOOL bRdy = !m_fsm.IsRun();
			 //bRdy &= m_pMtX->IsRdy(PX_CYL_BIT_ALIGN_F) && m_pMtX->InPos(PX_CYL_BIT_ALIGN_F);
			 //bRdy &= m_pMtT->IsRdy(PT_CYL_BIT_ALIGN_F) && m_pMtT->InPos(PT_CYL_BIT_ALIGN_F);
	
		if(INDEX_01 == m_nId)
		{
			BOOL bExist = !g_routerF.ExistCylBitAlignRedF();
			bRdy &= bExist == g_dIn.BOn(iIndex01BitAlignExist01);
			bRdy &= bExist == FALSE;
		}
		else if (INDEX_02 == m_nId)
		{
			BOOL bExist = !g_routerR.ExistCylBitAlignBlueF();
			bRdy &= bExist == g_dIn.BOn(iIndex02BitAlignExist01);
			bRdy &= bExist == FALSE;
		}
		else if (INDEX_03 == m_nId)
		{
			BOOL bExist = !g_routerF.ExistCylBitAlignBlueF();
			bRdy &= bExist == g_dIn.BOn(iIndex03BitAlignExist01);
			bRdy &= bExist == FALSE;
		}
		else if(INDEX_04 == m_nId)
		{
			BOOL bExist = !g_routerR.ExistCylBitAlignRedF();
			bRdy &= bExist == g_dIn.BOn(iIndex04BitAlignExist01);
			bRdy &= bExist == FALSE;
		}

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyBitAlignR(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyRouterCylBitAlignR);
	}
	else
	{
		if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
		{
			if (INDEX_02 == m_nId || INDEX_03 == m_nId)
				return (FALSE);
		}

		BOOL bRdy = !m_fsm.IsRun();
			 //bRdy &= m_pMtX->IsRdy(PX_CYL_BIT_ALIGN_R) && m_pMtX->InPos(PX_CYL_BIT_ALIGN_R);
			 //bRdy &= m_pMtT->IsRdy(PT_CYL_BIT_ALIGN_R) && m_pMtT->InPos(PT_CYL_BIT_ALIGN_R);
	
		if(INDEX_01 == m_nId)
		{
			BOOL bExist = !g_routerF.ExistCylBitAlignRedR();
			bRdy &= bExist == g_dIn.BOn(iIndex01BitAlignExist02);
			bRdy &= bExist == FALSE;
		}
		else if (INDEX_02 == m_nId)
		{
			BOOL bExist = !g_routerR.ExistCylBitAlignBlueR();
			bRdy &= bExist == g_dIn.BOn(iIndex02BitAlignExist02);
			bRdy &= bExist == FALSE;
		}
		else if (INDEX_03 == m_nId)
		{
			BOOL bExist = !g_routerF.ExistCylBitAlignBlueR();
			bRdy &= bExist == g_dIn.BOn(iIndex03BitAlignExist02);
			bRdy &= bExist == FALSE;
		}
		else if (INDEX_04 == m_nId)
		{
			BOOL bExist = !g_routerR.ExistCylBitAlignRedR();
			bRdy &= bExist == g_dIn.BOn(iIndex04BitAlignExist02);
			bRdy &= bExist == FALSE;
		}

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadySpdBitClampF(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyRouterSpdBitClampF);
	}
	else
	{
		if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
		{
			if (INDEX_02 == m_nId || INDEX_03 == m_nId)
				return (FALSE);
		}

		BOOL bRdy = !m_fsm.IsRun();
			 //bRdy &= m_pMtX->IsRdy(PX_SPD_BIT_CLAMP_F) && m_pMtX->InPos(PX_SPD_BIT_CLAMP_F);
			 //bRdy &= m_pMtT->IsRdy(PT_SPD_BIT_CLAMP_F) && m_pMtT->InPos(PT_SPD_BIT_CLAMP_F);
	
		if(INDEX_01 == m_nId)
		{
			BOOL bExist = g_routerF.ExistCylBitAlignRedF();
			bRdy &= bExist == g_dIn.BOn(iIndex01BitAlignExist01);
			bRdy &= bExist == TRUE;
		}
		else if (INDEX_02 == m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
		{
			BOOL bExist = g_routerR.ExistCylBitAlignBlueF();
			bRdy &= bExist == g_dIn.BOn(iIndex02BitAlignExist01);
			bRdy &= bExist == TRUE;
		}
		else if (INDEX_03 == m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
		{
			BOOL bExist = g_routerF.ExistCylBitAlignBlueF();
			bRdy &= bExist == g_dIn.BOn(iIndex03BitAlignExist01);
			bRdy &= bExist == TRUE;
		}
		else if (INDEX_04 == m_nId)
		{
			BOOL bExist = g_routerR.ExistCylBitAlignRedF();
			bRdy &= bExist == g_dIn.BOn(iIndex04BitAlignExist01);
			bRdy &= bExist == TRUE;
		}

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadySpdBitClampR(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyRouterSpdBitClampR);
	}
	else
	{
		if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
		{
			if (INDEX_02 == m_nId || INDEX_03 == m_nId)
				return (FALSE);
		}

		BOOL bRdy = !m_fsm.IsRun();
			 //bRdy &= m_pMtX->IsRdy(PX_SPD_BIT_CLAMP_R) && m_pMtX->InPos(PX_SPD_BIT_CLAMP_R);
			 //bRdy &= m_pMtT->IsRdy(PT_SPD_BIT_CLAMP_R) && m_pMtT->InPos(PT_SPD_BIT_CLAMP_R);
	
		if (INDEX_01 == m_nId)
		{
			BOOL bExist = g_routerF.ExistCylBitAlignRedR();
			bRdy &= bExist == g_dIn.BOn(iIndex01BitAlignExist02);
			bRdy &= bExist == TRUE;
		}
		else if (INDEX_02 == m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
		{
			BOOL bExist = g_routerR.ExistCylBitAlignBlueR();
			bRdy &= bExist == g_dIn.BOn(iIndex02BitAlignExist02);
			bRdy &= bExist == TRUE;
		}
		else if (INDEX_03 == m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
		{
			BOOL bExist = g_routerF.ExistCylBitAlignBlueR();
			bRdy &= bExist == g_dIn.BOn(iIndex03BitAlignExist02);
			bRdy &= bExist == TRUE;
		}
		else if (INDEX_04 == m_nId)
		{
			BOOL bExist = g_routerR.ExistCylBitAlignRedR();
			bRdy &= bExist == g_dIn.BOn(iIndex04BitAlignExist02);
			bRdy &= bExist == TRUE;
		}

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadySpdBitEject(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyRouterSpdBitEject);
	}
	else
	{
		if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_SKIP)
		{
			if (INDEX_02 == m_nId || INDEX_03 == m_nId)
				return (FALSE);
		}

		BOOL bRdy = !m_fsm.IsRun();
			 //bRdy &= m_pMtX->IsRdy(PX_SPD_BIT_EJECT) && m_pMtX->InPos(PX_SPD_BIT_EJECT);
			 //bRdy &= m_pMtT->IsRdy(PT_SPD_BIT_EJECT) && m_pMtT->InPos(PT_SPD_BIT_EJECT);

		if (INDEX_01 == m_nId)
			bRdy &= g_dIn.AOn(iIndexBitEjectBoxExist01);
		else if (INDEX_02 == m_nId)
			bRdy &= g_dIn.AOn(iIndexBitEjectBoxExist02);
		else if (INDEX_03 == m_nId)
			bRdy &= g_dIn.AOn(iIndexBitEjectBoxExist03);
		else if (INDEX_04 == m_nId)
			bRdy &= g_dIn.AOn(iIndexBitEjectBoxExist04);

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyOutPnp(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyOutPnp);
	}
	else
	{
		BOOL bRdy = !m_fsm.IsRun();
			 bRdy &= m_pMtX->IsRdy(PX_OUT_PNP_WAIT) && m_pMtX->InPos(PX_OUT_PNP_WAIT);
			 bRdy &= m_pMtT->IsRdy(PT_OUT_PNP_WAIT) && m_pMtT->InPos(PT_OUT_PNP_WAIT);
			 bRdy &= IsCylMaskFixUD(pmUP, 300);
			 bRdy &= IsCylMaskFixFB(pmBWD, 300);
			 bRdy &= ExistPcb();
			 bRdy &= g_pNV->UseSkip(usIndex01 + m_nId);

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyAdcInPnpPickUpPicker(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyAdcInPnpPickUpPicker);
	}
	else
	{
		BOOL bRdy = !m_fsm.IsRun();
			 bRdy &= m_pMtX->IsRdy(PX_ADC_WAIT) && m_pMtX->InPos(PX_ADC_WAIT);
			 bRdy &= m_pMtT->IsRdy(PT_ADC_WAIT) && m_pMtT->InPos(PT_ADC_WAIT);
			 bRdy &= IsCylMaskFixUD(pmDOWN, 300);
			 bRdy &= IsCylMaskFixFB(pmFWD, 300);
			 bRdy &= ExistKitMovePicker() && !ExistKitMask();
			 bRdy &= EXIST_NORMAL == GetKitMovePickerExistErr();
			 bRdy &= (INDEX_01 == m_nId);

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyAdcInPnpPickUpStage(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyAdcInPnpPickUpStage);
	}
	else
	{
		BOOL bRdy  = !m_fsm.IsRun();
		     bRdy &= m_pMtX->IsRdy(PX_ADC_WAIT) && m_pMtX->InPos(PX_ADC_WAIT);
			 bRdy &= m_pMtT->IsRdy(PT_ADC_WAIT) && m_pMtT->InPos(PT_ADC_WAIT);
			 bRdy &= IsCylMaskFixUD(pmUP, 300);
			 bRdy &= IsCylMaskFixFB(pmBWD, 300);
			 bRdy &= EXIST_NORMAL == GetKitStageExistErr();
			 bRdy &= ExistKitStage();

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyAdcInPnpPickUpMask(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyAdcInPnpPickUpMask);
	}
	else
	{
		BOOL bRdy  = !m_fsm.IsRun();
		     bRdy &= m_pMtX->IsRdy(PX_ADC_WAIT) && m_pMtX->InPos(PX_ADC_WAIT);
			 bRdy &= m_pMtT->IsRdy(PT_ADC_WAIT) && m_pMtT->InPos(PT_ADC_WAIT);
			 bRdy &= IsCylMaskFixUD(pmDOWN, 300);
			 bRdy &= IsCylMaskFixFB(pmFWD, 300);
			 bRdy &= EXIST_NORMAL == GetKitMaskExistErr();
			 bRdy &= ExistKitMask();

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyAdcInPnpPutDnPicker(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyAdcInPnpPutDnPicker);
	}
	else
	{
		BOOL bRdy  = !m_fsm.IsRun();
		     bRdy &= m_pMtX->IsRdy(PX_ADC_WAIT) && m_pMtX->InPos(PX_ADC_WAIT);
			 bRdy &= m_pMtT->IsRdy(PT_ADC_WAIT) && m_pMtT->InPos(PT_ADC_WAIT);
			 bRdy &= IsCylMaskFixUD(pmUP, 300);
			 bRdy &= IsCylMaskFixFB(pmBWD, 300);
			 bRdy &= EXIST_NORMAL == GetKitMovePickerExistErr();
			 bRdy &= !ExistKitMask() && !ExistKitMovePicker();
			 bRdy &= (INDEX_01 == m_nId);

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyAdcInPnpPutDnStage(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyAdcInPnpPutDnStage);
	}
	else
	{
		BOOL bRdy  = !m_fsm.IsRun();
		     bRdy &= m_pMtX->IsRdy(PX_ADC_WAIT) && m_pMtX->InPos(PX_ADC_WAIT);
			 bRdy &= m_pMtT->IsRdy(PT_ADC_WAIT) && m_pMtT->InPos(PT_ADC_WAIT);
			 bRdy &= IsCylMaskFixUD(pmUP, 300);
			 bRdy &= IsCylMaskFixFB(pmBWD, 300);
			 bRdy &= EXIST_NORMAL == GetKitStageExistErr();
			 bRdy &= !ExistKitStage();

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyAdcInPnpPutDnMask(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyAdcInPnpPutDnMask);
	}
	else
	{
		BOOL bRdy  = !m_fsm.IsRun();
		     bRdy &= m_pMtX->IsRdy(PX_ADC_WAIT) && m_pMtX->InPos(PX_ADC_WAIT);
			 bRdy &= m_pMtT->IsRdy(PT_ADC_WAIT) && m_pMtT->InPos(PT_ADC_WAIT);
			 bRdy &= IsCylMaskFixUD(pmUP, 300);
			 bRdy &= IsCylMaskFixFB(pmBWD, 300);
			 bRdy &= EXIST_NORMAL == GetKitMaskExistErr();
			 bRdy &= !ExistKitMask();

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyAdcOutPnpPickUpPicker(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyAdcOutPnpPickUpPicker);
	}
	else
	{
		BOOL bRdy  = !m_fsm.IsRun();
			 bRdy &= m_pMtX->IsRdy(PX_ADC_WAIT) && m_pMtX->InPos(PX_ADC_WAIT);
			 bRdy &= m_pMtT->IsRdy(PT_ADC_WAIT) && m_pMtT->InPos(PT_ADC_WAIT);
			 bRdy &= IsCylMaskFixUD(pmDOWN, 300);
			 bRdy &= IsCylMaskFixFB(pmFWD, 300);
			 bRdy &= EXIST_NORMAL == GetKitMovePickerExistErr();
			 bRdy &= EXIST_NORMAL == GetKitMaskExistErr();
			 bRdy &= EXIST_NORMAL == GetKitStageExistErr();
			 bRdy &= !ExistKitMask();
			 bRdy &= ExistKitMovePicker();
			 bRdy &= g_pNV->Pkg(adcKitJobType) == g_pNV->NDm(adcIndexMovePickerJobType);
			 bRdy &= (INDEX_01 == m_nId);

		return (bRdy);
	}
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyAdcOutPnpPutDnPicker(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyAdcOutPnpPutDnPicker);
	}
	else
	{
		BOOL bRdy  = !m_fsm.IsRun();
			 bRdy &= m_pMtX->IsRdy(PX_ADC_WAIT) && m_pMtX->InPos(PX_ADC_WAIT);
			 bRdy &= m_pMtT->IsRdy(PT_ADC_WAIT) && m_pMtT->InPos(PT_ADC_WAIT);
			 bRdy &= IsCylMaskFixUD(pmUP, 300);
			 bRdy &= IsCylMaskFixFB(pmBWD, 300);
			 bRdy &= EXIST_NORMAL == GetKitMovePickerExistErr();
			 bRdy &= EXIST_NORMAL == GetKitMaskExistErr();
			 bRdy &= EXIST_NORMAL == GetKitStageExistErr();
			 bRdy &= !ExistKitMask() && !ExistKitMovePicker();
			 bRdy &= !ExistKitStage();
			 bRdy &= (INDEX_01 == m_nId);
			 bRdy &= g_pNV->NDm(existKitOutPnp);
			 bRdy &= (ADC_KIT_PICKER == g_adc.GetAdcIndexReturnNo());

		return (bRdy);
	}
}


//-------------------------------------------------------------------
POINT2D CIndex::GetRouterPrsPos(int nCnt, int nArrayYCnt)
{
	POINT2D ptMark = GetMarkCoord(nCnt) * 1000.0;
		
	POINT2D ptViCen;
	ptViCen.dX = ((m_pSysTeach->router.dViX1 + m_pSysTeach->router.dViX2) / 2.0) * 1000.0;
	ptViCen.dY = ((m_pSysTeach->router.dViY1 + m_pSysTeach->router.dViY2) / 2.0) * 1000.0;

	POINT2D ptRet;
	ptRet.dX = ptViCen.dX - ptMark.dX;

	int nMax = (int)g_pNV->gerberPara(arrayYCnt) - 1; // 값이 4 일 때 0, 1, 2, 3 이여야 함
	BOOL bCanMoveY  = Between(nArrayYCnt, 1, nMax);
		 bCanMoveY &= (0 < nCnt); // PRS 3Point 중에 첫번째 위치가 아니어야 함

	double dArrayYPitch = 0.0; 
	if(bCanMoveY)
	{
		dArrayYPitch = (g_pNV->gerberPara(arrayYPitch) * nArrayYCnt) * 1000.0; // 설비 전면으로 이동
	}

	if(INDEX_01 == m_nId || INDEX_02 == m_nId)
		ptRet.dY = ptViCen.dY + (ptMark.dY - dArrayYPitch);
	else
		// Y 값은 반대로 연산
		ptRet.dY = ptViCen.dY - (ptMark.dY - dArrayYPitch);

	return (ptRet);
}


//-------------------------------------------------------------------
XYT CIndex::GetRouterPrsVerifyPos(int nPos) // 완료
{
	// PRS Result 까지 포함된 Data
	if(!g_pNV->UseSkip(usRouterPrs))
	{
		m_pPrsResult->block.dX = m_pPrsResult->block.dY = m_pPrsResult->block.dT = 0.0;

		for(int n = 0; n < UNIT_MAX; n++)
		{
			m_pPrsResult->unit[n].dX = m_pPrsResult->unit[n].dY = m_pPrsResult->unit[n].dT = 0.0;
		}
	}

	POINT2D ptMark = GetMarkCoord(nPos) * 1000.0; // 검증하고자 하는 Point를 nPos에 입력
		
	POINT2D ptViCen;
	ptViCen.dX = ((m_pSysTeach->router.dViX1 + m_pSysTeach->router.dViX2) / 2.0) * 1000.0;
	ptViCen.dY = ((m_pSysTeach->router.dViY1 + m_pSysTeach->router.dViY2) / 2.0) * 1000.0;

	XYT xytPrs;
	xytPrs.dX = m_pPrsResult->block.dX * 1000.0;
	xytPrs.dY = m_pPrsResult->block.dY * 1000.0;
	xytPrs.dT = m_pPrsResult->block.dT * 1000.0;

	XYT xytRet;
	xytRet.dX  = ptViCen.dX - (ptMark.dX + xytPrs.dX);
	
	if(INDEX_01 == m_nId || INDEX_02 == m_nId)
		xytRet.dY = ptViCen.dY + (ptMark.dY + xytPrs.dY); // 기준
	else
		// Y 값은 반대로 연산
		xytRet.dY = ptViCen.dY - (ptMark.dY + xytPrs.dY);

	xytRet.dT = g_mt[MT_INDEX_T_01 + m_nId].m_pTable->pos[PT_ROUTER_PRS];
	xytRet.dT = xytRet.dT - xytPrs.dT;

	return (xytRet);
}


//-------------------------------------------------------------------
XYT CIndex::GetRouterPos(int nCnt, int nPos) //완료
{
	// PRS Result 까지 포함된 Data
	if(!g_pNV->UseSkip(usRouterPrs))
	{
		m_pPrsResult->block.dX = m_pPrsResult->block.dY = m_pPrsResult->block.dT = 0;

		for(int n = 0; n < UNIT_MAX; n++)
		{
			m_pPrsResult->unit[n].dX = m_pPrsResult->unit[n].dY = m_pPrsResult->unit[n].dT = 0;
		}
	}

	// Offset 적용 필요
	POINT2D ptGbOffet = GetGerberOffset(nCnt, nPos);
	ptGbOffet.dX = ptGbOffet.dX * 1000.0;
	ptGbOffet.dY = ptGbOffet.dY * 1000.0; 

	POINT2D ptGbPos = GetGerberPos(nCnt, nPos);
	ptGbPos.dX = ptGbPos.dX * 1000.0;
	ptGbPos.dY = ptGbPos.dY * 1000.0; 

	POINT2D ptRtOrgPos;
	ptRtOrgPos.dX = m_pRouterOrgPos->dX * 1000.0;
	ptRtOrgPos.dY = m_pRouterOrgPos->dY * 1000.0;
	
	XYT xytPrsBlock;
	xytPrsBlock.dX = m_pPrsResult->block.dX * 1000.0;
	xytPrsBlock.dY = m_pPrsResult->block.dY * 1000.0;
	xytPrsBlock.dT = m_pPrsResult->block.dT * 1000.0;
	
	XYT xytRet;
	xytRet.dX  = ptRtOrgPos.dX - (ptGbPos.dX + ptGbOffet.dX + xytPrsBlock.dX);

	if(INDEX_01 == m_nId || INDEX_02 == m_nId)
		xytRet.dY = ptRtOrgPos.dY + (ptGbPos.dY + xytPrsBlock.dY + ptGbOffet.dY); // 기준
	else
		xytRet.dY = ptRtOrgPos.dY - (ptGbPos.dY + xytPrsBlock.dY + ptGbOffet.dY);

	// T 방향이 기존 설비 전면이 Home 방향 -> 2번 4번 인덱스는 설비 후면이 Home 방향임
	// 수정 필요
	//xytRet.dT = g_mt[MT_INDEX_T_01 + m_nId].m_pTable->pos[PT_ROUTER_RUN] + xytPrsBlock.dT;
	
	if(INDEX_01 == m_nId || INDEX_03 == m_nId)
		xytRet.dT = g_mt[MT_INDEX_T_01 + m_nId].m_pTable->pos[PT_ROUTER_RUN] + xytPrsBlock.dT;
	else
		xytRet.dT = g_mt[MT_INDEX_T_01 + m_nId].m_pTable->pos[PT_ROUTER_RUN] - xytPrsBlock.dT;

// 	if(INDEX_03 == m_nId)
// 	{
// 		SeqLog(L"\n");
// 		SeqLog(L"\n");
// 		SeqLog(L"Router Run !!!");
// 		SeqLog(L"Index[%d] Router Run nCnt = %d, nPos = %d", m_nId, nCnt, nPos);
// 		SeqLog(L"Index[%d] Gerber Pos X = %3f, Y = %3f, T = %3f", m_nId, ptRtOrgPos.dX + ptGbPos.dX, ptRtOrgPos.dY + ptGbPos.dY, g_mt[MT_INDEX_T_01 + m_nId].m_pTable->pos[PT_ROUTER_RUN]);
// 		SeqLog(L"Index[%d] PRS    Pos X = %3f, Y = %3f, T = %3f", m_nId, xytPrsBlock.dX, xytPrsBlock.dY, xytPrsBlock.dT);
// 		SeqLog(L"Index[%d] Result Pos X = %3f, Y = %3f, T = %3f", m_nId, xytRet.dX, xytRet.dY, xytRet.dT);
// 	}

	return (xytRet);
}


//-------------------------------------------------------------------
XYT CIndex::GetRouterLiveViPos(int nCnt, int nPos) // 완료
{
	// PRS Result 까지 포함된 Data
	if(!g_pNV->UseSkip(usRouterPrs))
	{
		m_pPrsResult->block.dX = m_pPrsResult->block.dY = m_pPrsResult->block.dT = 0;

		for(int n = 0; n < UNIT_MAX; n++)
		{
			m_pPrsResult->unit[n].dX = m_pPrsResult->unit[n].dY = m_pPrsResult->unit[n].dT = 0;
		}
	}

	// Offset 적용 필요 
	POINT2D ptGbOffet = GetGerberOffset(nCnt, nPos);
	ptGbOffet.dX = ptGbOffet.dX * 1000.0;
	ptGbOffet.dY = ptGbOffet.dY * 1000.0; 

	POINT2D ptGbPos = GetGerberPos(nCnt, nPos);
	ptGbPos.dX = ptGbPos.dX * 1000.0;
	ptGbPos.dY = ptGbPos.dY * 1000.0;

	POINT2D ptViCen;
	ptViCen.dX = ((m_pSysTeach->router.dViX1 + m_pSysTeach->router.dViX2) / 2.0) * 1000.0;
	ptViCen.dY = ((m_pSysTeach->router.dViY1 + m_pSysTeach->router.dViY2) / 2.0) * 1000.0;
	
	XYT xytPrsBlock;
	xytPrsBlock.dX = m_pPrsResult->block.dX * 1000.0;
	xytPrsBlock.dY = m_pPrsResult->block.dY * 1000.0;
	xytPrsBlock.dT = m_pPrsResult->block.dT * 1000.0;
	
	XYT xytRet;
	xytRet.dX  = ptViCen.dX - (ptGbPos.dX + xytPrsBlock.dX + xytPrsBlock.dX);

	if(INDEX_01 == m_nId || INDEX_02 == m_nId)
		xytRet.dY  = ptViCen.dY + (ptGbPos.dY + ptGbOffet.dY + xytPrsBlock.dY);
	else
		xytRet.dY  = ptViCen.dY - (ptGbPos.dY + ptGbOffet.dY + xytPrsBlock.dY);

	// T 방향이 기존 설비 전면이 Home 방향 -> 2번 4번 인덱스는 설비 후면이 Home 방향임
	// 수정 필요
	//xytRet.dT  = g_mt[MT_INDEX_T_01 + m_nId].m_pTable->pos[PT_ROUTER_LIVE_VI] + xytPrsBlock.dT;

	if(INDEX_01 == m_nId || INDEX_03 == m_nId)
		xytRet.dT = g_mt[MT_INDEX_T_01 + m_nId].m_pTable->pos[PT_ROUTER_LIVE_VI] + xytPrsBlock.dT;
	else
		xytRet.dT = g_mt[MT_INDEX_T_01 + m_nId].m_pTable->pos[PT_ROUTER_LIVE_VI] - xytPrsBlock.dT;

// 	SeqLog(L"\n");
// 	SeqLog(L"\n");
// 	SeqLog(L"Router Live Vision !!!");
// 	SeqLog(L"Index[%d] Live Vi nCnt = %d, nPos = %d", m_nId, nCnt, nPos);
// 	SeqLog(L"Index[%d] Pos X = %3f, Y = %3f, T = %3f", m_nId, xytRet.dX, xytRet.dY, xytRet.dT);
		
	return (xytRet);
}


//-------------------------------------------------------------------
POINT2D CIndex::GetBitBoxSupplyPos(int nCnt)
{
	// 첫번재 Pos는 setup Max : 49, Empty : -1
	int nCol = nCnt / 5; // 몫 행
	int nRow = nCnt % 5; // 나머지 열

	POINT2D ptOffset;

	ptOffset.dX = (4 - nRow) * g_pNV->DDm(indexBitBoxXPitch) * (1000.0);	
	ptOffset.dY = (9 - nCol) * g_pNV->DDm(indexBitBoxYPitch) * (1000.0);

	POINT2D ptRet;
	ptRet.dX = ptRet.dY = 0;
	
	if(INDEX_01 == m_nId)
	{
		ptRet.dX = g_mt[MT_INDEX_X_01].m_pTable->pos[PX_CYL_BIT_SUPPLY_BOX] - ptOffset.dX;
		ptRet.dY = g_mt[MT_ROUTER_Y_01].m_pTable->pos[CRouter::PY_CYL_BIT_CLAMP_RED] + ptOffset.dY;
	}
	else if (INDEX_02 == m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
	{
		ptRet.dX = g_mt[MT_INDEX_X_02].m_pTable->pos[PX_CYL_BIT_SUPPLY_BOX] - ptOffset.dX;
		ptRet.dY = g_mt[MT_ROUTER_Y_01].m_pTable->pos[CRouter::PY_CYL_BIT_CLAMP_BLUE] + ptOffset.dY;
	}
	else if (INDEX_03 == m_nId && (int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
	{
		ptRet.dX = g_mt[MT_INDEX_X_03].m_pTable->pos[PX_CYL_BIT_SUPPLY_BOX] - ptOffset.dX;
		ptRet.dY = g_mt[MT_ROUTER_Y_02].m_pTable->pos[CRouter::PY_CYL_BIT_CLAMP_BLUE] - ptOffset.dY;
	}
	else if(INDEX_04 == m_nId)
	{
		ptRet.dX = g_mt[MT_INDEX_X_04].m_pTable->pos[PX_CYL_BIT_SUPPLY_BOX] - ptOffset.dX;
		ptRet.dY = g_mt[MT_ROUTER_Y_02].m_pTable->pos[CRouter::PY_CYL_BIT_CLAMP_RED] - ptOffset.dY;
	}
	// 그 외에는 0 값 입력
	return (ptRet);
}


//-------------------------------------------------------------------
int CIndex::GetGerberLineType(int nCnt)
{
	int nResult = (int)g_pRouterData->rtData[nCnt].dLineType;

	if(nResult < 0 || nResult > 1)
		nResult = -1;

	return (nResult);
}


//-------------------------------------------------------------------
POINT2D	CIndex::GetGerberPos(int nCnt, int nPos)
{
	POINT2D ptRet;
	ptRet.dX = ptRet.dY = 0;

	if(POS_START == nPos)
	{
		ptRet.dX = g_pRouterData->rtData[nCnt].dX_Start;
		ptRet.dY = g_pRouterData->rtData[nCnt].dY_Start;
	}
	else if(POS_END == nPos)
	{
		ptRet.dX = g_pRouterData->rtData[nCnt].dX_End;
		ptRet.dY = g_pRouterData->rtData[nCnt].dY_End;
	}
	else if(POS_MID_01 == nPos)
	{
		ptRet.dX = g_pRouterData->rtData[nCnt].dX_Mid_01;
		ptRet.dY = g_pRouterData->rtData[nCnt].dY_Mid_01;
	}
	else if(POS_MID_02 == nPos)
	{
		ptRet.dX = g_pRouterData->rtData[nCnt].dX_Mid_02;
		ptRet.dY = g_pRouterData->rtData[nCnt].dY_Mid_02;
	}
	else if(POS_MID_03 == nPos)
	{
		ptRet.dX = g_pRouterData->rtData[nCnt].dX_Mid_03;
		ptRet.dY = g_pRouterData->rtData[nCnt].dY_Mid_03;
	}
	else if(POS_MID_04 == nPos)
	{
		ptRet.dX = g_pRouterData->rtData[nCnt].dX_Mid_04;
		ptRet.dY = g_pRouterData->rtData[nCnt].dY_Mid_04;
	}

	//SeqLog(L"\n");
	//SeqLog(L"\n");
	//SeqLog(L"Gerber Pos !!!");
	//SeqLog(L"Index[%d] Router Run nCnt = %d, nPos = %d", m_nId, nCnt, nPos);
	//SeqLog(L"Index[%d] Pos X = %3f, Y = %3f", m_nId, ptRet.dX, ptRet.dY);

	return (ptRet);
}


//-------------------------------------------------------------------
POINT2D	CIndex::GetGerberOffset(int nCnt, int nPos)
{
	POINT2D ptRet;
	ptRet.dX = ptRet.dY = 0;

	if(POS_START == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_Start;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_Start;
	}
	else if(POS_END == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_Start;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_Start;
	}
	else if(POS_MID_01 == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_Mid_01;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_Mid_01;
	}
	else if(POS_MID_02 == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_Mid_01;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_Mid_01;
	}
	else if(POS_MID_03 == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_Mid_03;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_Mid_03;
	}
	else if(POS_MID_04 == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_Mid_03;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_Mid_03;
	}

	//SeqLog(L"\n");
	//SeqLog(L"\n");
	//SeqLog(L"Gerber Offset !!!");
	//SeqLog(L"Index[%d] Router Run nCnt = %d, nPos = %d", m_nId, nCnt, nPos);
	//SeqLog(L"Index[%d] Pos X = %3f, Y = %3f", m_nId, ptRet.dX, ptRet.dY);

	return (ptRet);
}

//-------------------------------------------------------------------
POINT2D	CIndex::GetGerberSubOffset(int nCnt, int nPos)
{
	POINT2D ptRet;
	ptRet.dX = ptRet.dY = 0;

	if(POS_START == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_End;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_End;
	}
	else if(POS_END == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_End;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_End;
	}
	else if(POS_MID_01 == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_Mid_02;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_Mid_02;
	}
	else if(POS_MID_02 == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_Mid_02;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_Mid_02;
	}
	else if(POS_MID_03 == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_Mid_04;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_Mid_04;
	}
	else if(POS_MID_04 == nPos)
	{
		ptRet.dX = g_pOffsetIndex[m_nId]->rtData[nCnt].dX_Mid_04;
		ptRet.dY = g_pOffsetIndex[m_nId]->rtData[nCnt].dY_Mid_04;
	}

	//SeqLog(L"\n");
	//SeqLog(L"\n");
	//SeqLog(L"Gerber Offset !!!");
	//SeqLog(L"Index[%d] Router Run nCnt = %d, nPos = %d", m_nId, nCnt, nPos);
	//SeqLog(L"Index[%d] Pos X = %3f, Y = %3f", m_nId, ptRet.dX, ptRet.dY);

	return (ptRet);
}

//-------------------------------------------------------------------
int CIndex::GetState(void)
{
	int nState = S_IDLE;

	if(g_pNV->NDm(mmiBtnAdcMode))
	{
		nState = S_ADC_IDLE;
		int nReturnNo = g_adc.GetAdcIndexReturnNo(); // Kit 여부 및 Job 정보 확인

		if(INDEX_01 == m_nId)
		{
			if(ExistKitMovePicker()) //Mask 위에 Picker가 있는 상태
			{
				if(g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndexMovePickerJobType))
					nState = S_ADC_WAIT_IN_PNP_PICKUP;
				else 
					nState = S_ADC_WAIT_OUT_PNP_PICKUP;
			}
			else
			{
				if(ADC_KIT_MASK_01 == nReturnNo)
				{
					if(ExistKitMask())
						nState = S_ADC_WAIT_IN_PNP_PICKUP;
					else // Picker에 있음
						nState = S_ADC_MASK_PICKER_PUTDN;
				}
				else if(ADC_KIT_STAGE_01 == nReturnNo)
					nState = S_ADC_WAIT_IN_PNP_PICKUP;
				else if(ADC_KIT_PICKER == nReturnNo) 
					nState = S_ADC_WAIT_OUT_PNP_PUTDN;
				else
				{
					if(ExistKitMask())
						nState = S_ADC_MASK_PICKER_PICKUP;
					else
						nState = S_ADC_WAIT_IN_PNP_PUTDN;
				}
			}
		}
		else if(INDEX_02 == m_nId)
		{
			if(ADC_KIT_MASK_02 == nReturnNo)
			{
				if(ExistKitMask())
					nState = S_ADC_WAIT_IN_PNP_PICKUP;
				else // Picker에 있음
					nState = S_ADC_MASK_PICKER_PUTDN;
			}
			else if(ADC_KIT_STAGE_02 == nReturnNo)
				nState = S_ADC_WAIT_IN_PNP_PICKUP;
			else
			{
				if(ExistKitMask())
					nState = S_ADC_MASK_PICKER_PICKUP;
				else
					nState = S_ADC_WAIT_IN_PNP_PUTDN;
			}
		}
		else if(INDEX_03 == m_nId)
		{
			if(ADC_KIT_MASK_03 == nReturnNo)
			{
				if(ExistKitMask())
					nState = S_ADC_WAIT_IN_PNP_PICKUP;
				else // Picker에 있음
					nState = S_ADC_MASK_PICKER_PUTDN;
			}
			else if(ADC_KIT_STAGE_03 == nReturnNo)
				nState = S_ADC_WAIT_IN_PNP_PICKUP;
			else
			{
				if(ExistKitMask())
					nState = S_ADC_MASK_PICKER_PICKUP;
				else
					nState = S_ADC_WAIT_IN_PNP_PUTDN;
			}
		}
		else if(INDEX_04 == m_nId)
		{
			if(ADC_KIT_MASK_04 == nReturnNo)
			{
				if(ExistKitMask())
					nState = S_ADC_WAIT_IN_PNP_PICKUP;
				else // Picker에 있음
					nState = S_ADC_MASK_PICKER_PUTDN;
			}
			else if(ADC_KIT_STAGE_04 == nReturnNo)
				nState = S_ADC_WAIT_IN_PNP_PICKUP;
			else
			{
				if(ExistKitMask())
					nState = S_ADC_MASK_PICKER_PICKUP;
				else
					nState = S_ADC_WAIT_IN_PNP_PUTDN;
			}
		}
	}
	else
	{
		if(IsSkip(m_nId))
			return (S_IDLE);

		bool bBlueBit = false;
		if ((int)g_pNV->Pkg(optionSocammUseBlueBit) == S_USE)
			bBlueBit = true;
		
		if(INDEX_01 == m_nId || INDEX_04 == m_nId || bBlueBit)
		{
			// Router에서 요청
			if(m_bReqBitSupplyPos)
				return (S_BIT_SUPPLY_BOX);
			else if(m_bReqBitEjectPos)
				return (S_BIT_EJECT_BOX);
			else if(m_bReqBitAlignFPos)
				return (S_BIT_ALIGN_F);
			else if(m_bReqBitAlignRPos)
				return (S_BIT_ALIGN_R);
			else if(m_bReqBitSpdClampFPos)
				return (S_BIT_SPINDLE_CLAMP_F);
			else if(m_bReqBitSpdClampRPos)
				return (S_BIT_SPINDLE_CLAMP_R);
		}

		if(ExistPcb())
		{
			if(m_pMem->compPRSFail)
				nState = S_MASK_UNCLAMP;
			else if(!m_pMem->compLoadCheck)
				nState = S_LOAD_CHECK;
			else if(!m_pMem->compMaskClamp)
				nState = S_MASK_CLAMP;
			else if(!m_pMem->compRouterPrs)
				nState = S_ROUTER_PRS;
			else if(!m_pMem->compRouterLiveVi)
				nState = S_ROUTER_LIVE_VI;
			else if(!m_pMem->compRouterRun)
				nState = S_ROUTER_RUN;
			else if(!m_pMem->compMaskUnClamp)
				nState = S_MASK_UNCLAMP;
			else if(!m_pMem->compOutPnp)
				nState = S_OUTPNP;
		}
		else
		{
			nState = S_IN_PNP;
		}
	}

	return (nState);
}


//-------------------------------------------------------------------
BOOL CIndex::IsErr(void)
{
	if(!m_pMtX->m_state.isHome)
		return (TRUE);
	if(!m_pMtT->m_state.isHome)
		return (TRUE);

	if(0 < m_pCylMaskFB_L->GetErr())
		return (TRUE);
	if(0 < m_pCylMaskFB_R->GetErr())
		return (TRUE);
	if(0 < m_pCylMaskUD->GetErr())
		return (TRUE);
	if(0 < m_pCylMaskPickerUD->GetErr())
		return (TRUE);
	if(0 < m_pCylMaskPickerOC->GetErr())
		return (TRUE);
	if(0 < m_pSolStageKitOC->GetErr())
		return (TRUE);
	if(0 < m_pCylDustShutterOC->GetErr())
		return (TRUE);

	return (FALSE);
}//--------------------------------------------------------------


int CIndex::GetKitStageExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);
	
	BOOL isSenOn = FALSE;
	
	if(INDEX_01 == m_nId)
		isSenOn = g_dIn.AOn(iIndexStageKitExistL01) && g_dIn.AOn(iIndexStageKitExistR01);
	else if(INDEX_02 == m_nId)
		isSenOn = g_dIn.AOn(iIndexStageKitExistL02) && g_dIn.AOn(iIndexStageKitExistR02);
	else if(INDEX_03 == m_nId)
		isSenOn = g_dIn.AOn(iIndexStageKitExistL03) && g_dIn.AOn(iIndexStageKitExistR03);
	else if(INDEX_04 == m_nId)
		isSenOn = g_dIn.AOn(iIndexStageKitExistL04) && g_dIn.AOn(iIndexStageKitExistR04);

	if(ExistKitStage() == isSenOn)
	{
		m_tmExistKitStageErr.Reset();
	}
	else
	{
		if(m_tmExistKitStageErr.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


int CIndex::GetKitMaskExistErr(void)
{
	if(g_pNV->UseSkip(usMaskKitSensorSkip) || g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL isSenOn = FALSE;

	if(INDEX_01 == m_nId)
	{
		if(g_pNV->NDm(mmiBtnAdcMode))
		{
			// 다른 쪽에서 확인
			if(ExistKitMovePicker())
				return (EXIST_NORMAL);
		}

		isSenOn = g_dIn.AOn(iIndexMaskKitExist01);
	}
	else if(INDEX_02 == m_nId)
		isSenOn = g_dIn.AOn(iIndexMaskKitExist02);
	else if(INDEX_03 == m_nId)
		isSenOn = g_dIn.AOn(iIndexMaskKitExist03);
	else if(INDEX_04 == m_nId)
		isSenOn = g_dIn.AOn(iIndexMaskKitExist04);
	
	if(ExistKitMask() == isSenOn)
	{
		m_tmExistKitMaskErr.Reset();
	}
	else
	{
		if(m_tmExistKitMaskErr.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


int CIndex::GetKitMovePickerExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL isSenOn = g_dIn.AOn(iIndex01OutPnpKitExist01) && g_dIn.AOn(iIndex01OutPnpKitExist02);
	//BOOL isSenOn = g_dIn.AOn(iIndex01OutPnpKitExist01);

	if(INDEX_01 == m_nId)
	{
		if(ExistKitMovePicker())
		{
			if(isSenOn)
			{
				m_tmExistKitMovePickerErr.Reset();
			}
			else
			{
				if(m_tmExistKitMovePickerErr.TmOver(1000))
					return (EXIST_ERR);
				else
					return (EXIST_UNCERTAIN);
			}
		}
	}

	return (EXIST_NORMAL);
}


int CIndex::GetKitMaskPickerExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL isSenOn = FALSE;

	if(INDEX_01 == m_nId)
		isSenOn = g_dIn.AOn(iMaskPickerKitExist01);
	else if(INDEX_02 == m_nId)
		isSenOn = g_dIn.AOn(iMaskPickerKitExist02);
	else if(INDEX_03 == m_nId)
		isSenOn = g_dIn.AOn(iMaskPickerKitExist03);
	else if(INDEX_04 == m_nId)
		isSenOn = g_dIn.AOn(iMaskPickerKitExist04);
	
	if(ExistKitMaskPicker() == isSenOn)
	{
		m_tmExistKitMaskPickerErr.Reset();
	}
	else
	{
		if(m_tmExistKitMaskPickerErr.TmOver(1000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}

	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
void CIndex::SetRouterCurIndex(int nIdx)
{
	if(INDEX_01 == nIdx)
	{
		if (INDEX_FR_IDLE == g_routerF.m_nCurIndex)
		{
			g_routerF.m_nCurIndex = INDEX_F;
		}

	}
	else if (INDEX_03 == nIdx && g_routerF.m_nCurIndex != INDEX_R)
	{
		if (INDEX_FR_IDLE == g_routerR.m_nCurIndex)
		{
			g_routerR.m_nCurIndex = INDEX_F;
		}
	}
	else if (INDEX_02 == nIdx && g_routerR.m_nCurIndex != INDEX_F)
	{
		if (INDEX_FR_IDLE == g_routerF.m_nCurIndex)
		{
			g_routerF.m_nCurIndex = INDEX_R;
		}
	}
	else if (INDEX_04 == nIdx)
	{
		if (INDEX_FR_IDLE == g_routerR.m_nCurIndex)
		{
			g_routerR.m_nCurIndex = INDEX_R;
		}
	}
}


//-------------------------------------------------------------------
POINT2D CIndex::GetBlockCenCoord(int nBlockNo) //nBlockNo : 0~
{
	// 항상 0, 0
	POINT2D ptCoord;
	ptCoord.dX = ptCoord.dY = 0;
	/*
	int nMaxBlockCnt = (int)g_nv.Pkg(blockCnt);

	ptCoord.x = ptCoord.y = 0;

	double dHalfPitch = (g_nv.Pkg(blockPitch) * (nMaxBlockCnt - 1)) / 2.0;
	ptCoord.x = dHalfPitch - (g_nv.Pkg(blockPitch) * nBlockNo);

	*/
	return (ptCoord);
}


//-------------------------------------------------------------------
POINT2D CIndex::GetMarkCoord(int nCnt)
{
	POINT2D ptRet = m_pGbPrsBlockPos->ptXY[nCnt];
	
	return (ptRet);
}


//-------------------------------------------------------------------
BOOL CIndex::GetBlockPrsResultErr()
{
	BOOL bResult  = m_pPrsResult->block.dX > g_pNV->DDm(prsResultLimitX);
		 bResult |= m_pPrsResult->block.dY > g_pNV->DDm(prsResultLimitY);
		 bResult |= m_pPrsResult->block.dT > g_pNV->DDm(prsResultLimitT);
		 
		 bResult |= m_pPrsResult->block.dX < -g_pNV->DDm(prsResultLimitX);
		 bResult |= m_pPrsResult->block.dY < -g_pNV->DDm(prsResultLimitY);
		 bResult |= m_pPrsResult->block.dT < -g_pNV->DDm(prsResultLimitT);

	return (bResult);
}


//-------------------------------------------------------------------
void CIndex::SetCylMaskFixUD(BOOL bAct)
{
	m_pCylMaskUD->Actuate(bAct);
}


//-------------------------------------------------------------------
BOOL CIndex::IsCylMaskFixUD(BOOL bAct, int nDelay)
{
	if(bAct != m_pCylMaskUD->GetPos(nDelay))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
void CIndex::SetCylMaskFixFB(BOOL bAct)
{
	m_pCylMaskFB_L->Actuate(bAct);
	m_pCylMaskFB_R->Actuate(bAct);
}


//-------------------------------------------------------------------
BOOL CIndex::IsCylMaskFixFB(BOOL bAct, int nDelay)
{
	if(bAct != m_pCylMaskFB_L->GetPos(nDelay))
		return (FALSE);
	if(bAct != m_pCylMaskFB_R->GetPos(nDelay))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CIndex::CylIndexMaskFixAct(BOOL bAct) //pmOPEN, pmCLOSE
{
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if(ExistPcb())
	{
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode) + 1);
		_sprintf(cMaterialType, L"PCB");
	}
	else
	{
		_sprintf(cMaterialId, L"$");
		_sprintf(cMaterialType, L"$");
	}

	_char cIndexId[_MAX_CHAR_SIZE_], cEventIdCyl[_MAX_CHAR_SIZE_];
	_sprintf(cIndexId, L"INDEX%d", (m_nId + 1));

	if(pmOPEN == bAct)
	{
		if(!IsCylMaskFixUD(pmUP, 300))
		{
			SetCylMaskFixUD(pmUP);

			if(!g_logChk.bFunction[m_pCylMaskUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskUD->GetNo()] = TRUE;
				_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_%02d_UP", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
			}
			return (FALSE);
		}
		else
		{
			if(g_logChk.bFunction[m_pCylMaskUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskUD->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_%02d_UP", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.off))
			}
		}

		if(!IsCylMaskFixFB(pmBWD, 300))
		{
			SetCylMaskFixFB(pmBWD);

			if(!g_logChk.bFunction[m_pCylMaskFB_L->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskFB_L->GetNo()] = TRUE;
				_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_L_%02d_BWD", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskFB_L->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmBWD][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmBWD][pmON], g_data2c.cEtc.off))

				_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_R_%02d_BWD", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskFB_L->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmBWD][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmBWD][pmON], g_data2c.cEtc.off))
			}
		}
		else
		{
			if(g_logChk.bFunction[m_pCylMaskFB_L->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskFB_L->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_L_%02d_BWD", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskFB_L->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmBWD][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmBWD][pmON], g_data2c.cEtc.on))

				_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_R_%02d_BWD", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskFB_L->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmBWD][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmBWD][pmON], g_data2c.cEtc.on))
			}
		}
		return (TRUE);
	}
	else //close
	{
		if(IsCylMaskFixUD(pmUP, 300))
		{
			if(!IsCylMaskFixFB(pmFWD, 300))
			{
				SetCylMaskFixFB(pmFWD);
				if(!g_logChk.bFunction[m_pCylMaskFB_L->GetNo()])
				{
					g_logChk.bFunction[m_pCylMaskFB_L->GetNo()] = TRUE;
					_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_L_%02d_FWD", (m_nId + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylMaskFB_L->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmFWD][pmON], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmFWD][pmOFF], g_data2c.cEtc.on))

					_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_R_%02d_FWD", (m_nId + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylMaskFB_L->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmFWD][pmON], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmFWD][pmOFF], g_data2c.cEtc.on))
				}
			}
			else
			{
				if(g_logChk.bFunction[m_pCylMaskFB_L->GetNo()])
				{
					g_logChk.bFunction[m_pCylMaskFB_L->GetNo()] = FALSE;
					_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_L_%02d_FWD", (m_nId + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylMaskFB_L->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmFWD][pmON], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmFWD][pmOFF], g_data2c.cEtc.off))

					_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_R_%02d_FWD", (m_nId + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylMaskFB_L->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmFWD][pmON], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylMaskFB_L->GetNo()].In[pmFWD][pmOFF], g_data2c.cEtc.off))
				}

				SetCylMaskFixUD(pmDOWN);

				if(!g_logChk.bFunction[m_pCylMaskUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylMaskUD->GetNo()] = TRUE;
					_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_%02d_DOWN", (m_nId + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylMaskUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylMaskUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylMaskUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.off))
				}
			}
			return (FALSE);
		}
		else if(IsCylMaskFixUD(pmDOWN, 300))
		{
			if(g_logChk.bFunction[m_pCylMaskUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskUD->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_INDEX_MASK_KIT_FIX_%02d_DOWN", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
			}

			// 예외처리 이므로 코딩하지 않음
			if(!IsCylMaskFixFB(pmFWD, 300))
			{
				SetCylMaskFixUD(pmUP);
				return (FALSE);
			}

			return (TRUE);
		}
	}

	return (FALSE);
}


//-------------------------------------------------------------------
BOOL CIndex::IsInterfaceSorterIndex(int nIdx)
{
	if(!g_dIn.AOn(iSorterAutoRun))
		return (FALSE);
	if(g_dIn.AOn(iSorterError))
		return (FALSE);

	if(nIdx < 0 || 4 < nIdx)
		return (FALSE);

	if(INDEX_01 == nIdx)
	{
		if(!g_dIn.AOn(iSorterStageSafety01)) 
			return (FALSE);

		g_dOut.On(oSorterStageReq01);

		if(!g_dIn.AOn(iSorterPickerSafety0102)) 
			return (FALSE);
		if(!g_dIn.AOn(iSorterPickerAllow0102)) 
			return (FALSE);
	}
	else if(INDEX_02 == nIdx)
	{
		if(!g_dIn.AOn(iSorterStageSafety02))
			return (FALSE);

		g_dOut.On(oSorterStageReq02);

		if(!g_dIn.AOn(iSorterPickerSafety0102)) 
			return (FALSE);
		if(!g_dIn.AOn(iSorterPickerAllow0102)) 
			return (FALSE);
	}
	else if(INDEX_03 == nIdx)
	{
		if(!g_dIn.AOn(iSorterStageSafety03))
			return (FALSE);

		g_dOut.On(oSorterStageReq03);

		if(!g_dIn.AOn(iSorterPickerSafety0304)) 
			return (FALSE);
		if(!g_dIn.AOn(iSorterPickerAllow0304)) 
			return (FALSE);
	}
	else if(INDEX_04 == nIdx)
	{
		if(!g_dIn.AOn(iSorterStageSafety04))
			return (FALSE);

		g_dOut.On(oSorterStageReq04);

		if(!g_dIn.AOn(iSorterPickerSafety0304))
			return (FALSE);
		if(!g_dIn.AOn(iSorterPickerAllow0304))
			return (FALSE);
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyMtIndexXInPnp()
{
	double dOffset = g_pNV->Pkg(offsetIndexXInPnp1 + m_nId) * 1000.0;	
	double dPos	   = m_pMtX->m_pTable->pos[PX_IN_PNP] + dOffset;

	if(!m_pMtX->IsRdy())
		return (FALSE);

	if(!m_pMtX->InPos(PX_IN_PNP, dPos, 50))
		return (FALSE);

	_char cIndexId[_MAX_CHAR_SIZE_], cEventId[_MAX_CHAR_SIZE_];
	_sprintf(cIndexId, L"INDEX%d", (m_nId + 1));
	_sprintf(cEventId, L"MT_INDEX_%02d_X_INPNP", (m_nId + 1));
	
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if(ExistPcb())
	{
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode) + 1);
		_sprintf(cMaterialType, L"PCB");
	}
	else
	{
		_sprintf(cMaterialId, L"$");
		_sprintf(cMaterialType, L"$");
	}

	_char cPos[_MAX_CHAR_SIZE_];
	_sprintf(cPos, L"%03f", dPos);

	if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_IN_PNP])
	{
		g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_IN_PNP] = FALSE;
		NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventId, g_data2c.cEtc.end, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
											g_data2c.cIndex[m_nId].X[PX_IN_PNP][_POSIDX_], cPos, 
											g_data2c.cIndex[m_nId].X[PX_IN_PNP][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_IN_PNP][_SPD_], 
											g_data2c.cIndex[m_nId].X[PX_IN_PNP][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_IN_PNP][_ACC_]))
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CIndex::MoveMtIndexXInPnp()
{
	double dOffset = g_pNV->Pkg(offsetIndexXInPnp1 + m_nId) * 1000.0;	
	double dPos	   = m_pMtX->m_pTable->pos[PX_IN_PNP] + dOffset;

	m_pMtX->PMove(PX_IN_PNP, dPos);

	_char cIndexId[_MAX_CHAR_SIZE_], cEventId[_MAX_CHAR_SIZE_];
	_sprintf(cIndexId, L"INDEX%d", (m_nId + 1));
	_sprintf(cEventId, L"MT_INDEX_%02d_X_INPNP", (m_nId + 1));
	
	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if(ExistPcb())
	{
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode) + 1);
		_sprintf(cMaterialType, L"PCB");
	}
	else
	{
		_sprintf(cMaterialId, L"$");
		_sprintf(cMaterialType, L"$");
	}

	_char cPos[_MAX_CHAR_SIZE_];
	_sprintf(cPos, L"%03f", dPos);

	if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_IN_PNP])
	{
		g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_IN_PNP] = TRUE;
		NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventId, g_data2c.cEtc.start, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
											g_data2c.cIndex[m_nId].X[PX_IN_PNP][_POSIDX_], cPos, 
											g_data2c.cIndex[m_nId].X[PX_IN_PNP][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_IN_PNP][_SPD_], 
											g_data2c.cIndex[m_nId].X[PX_IN_PNP][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_IN_PNP][_ACC_]))
	}
	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CIndex::IsReadyMtIndexXOutPnp()
{
	double dOffset = g_pNV->Pkg(offsetIndexXOutPnp1 + m_nId) * 1000.0;	
	double dPos	   = m_pMtX->m_pTable->pos[PX_OUT_PNP] + dOffset;

	if(!m_pMtX->IsRdy())
		return (FALSE);

	if(!m_pMtX->InPos(PX_OUT_PNP, dPos, 1000))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CIndex::MoveMtIndexXOutPnp()
{
	double dOffset = g_pNV->Pkg(offsetIndexXOutPnp1 + m_nId) * 1000.0;	
	double dPos	   = m_pMtX->m_pTable->pos[PX_OUT_PNP] + dOffset;

	m_pMtX->PMove(PX_OUT_PNP, dPos);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CIndex::ExistErr(void)
{
	int existStageErrVal = GetKitStageExistErr();
	int existMaskErrVal  = GetKitMaskExistErr();
	int existMaskPickerErrVal  = GetKitMaskPickerExistErr();

	if(EXIST_UNCERTAIN == existStageErrVal)
		return (FALSE);
	if(EXIST_UNCERTAIN == existMaskErrVal)
		return (FALSE);
	if(EXIST_UNCERTAIN == existMaskPickerErrVal)
		return (FALSE);
	if(EXIST_ERR == existStageErrVal)
	{
		g_err.Save(ER_ADC_KIT_EXIST_STAGE_01 + m_nId);
		return (FALSE);
	}
	if(EXIST_ERR == existMaskErrVal)
	{
		g_err.Save(ER_ADC_KIT_EXIST_MASK_01 + m_nId);
		return (FALSE);
	}
	if(EXIST_ERR == existMaskPickerErrVal)
	{
		g_err.Save(ER_ADC_KIT_EXIST_MASK_PICKER_01 + m_nId);
		return (FALSE);
	}
	
	// Index 1번에 Picker가 있을 때는 따로 확인
	if(g_pNV->NDm(mmiBtnAdcMode))
	{
		int existMovePickerErrVal = GetKitMovePickerExistErr();
		if(EXIST_UNCERTAIN == existMovePickerErrVal)
			return (FALSE);
		if(EXIST_ERR == existMovePickerErrVal)
		{
			g_err.Save(ER_ADC_INDEX_01_PICKER_EXIST);
			return (FALSE);
		}
	}

	// Mask와 Move Picker Exist가 동시에 존재하면 Error
	if(INDEX_01 == m_nId)
	{
		if(ExistKitMask() && ExistKitMovePicker())
		{
			g_err.Save(ER_ADC_INDEX_01_MASK_N_PICKER_EXIST);
			return (FALSE);
		}
	}
	
	// Mask와 Mask Picker Exist가 동시에 존재하면 Error
	if(ExistKitMask() && ExistKitMaskPicker())
	{
		g_err.Save(ER_ADC_INDEX_01_MASK_N_MASK_PICKER_EXIST + m_nId);
		return (FALSE);
	}

	if(!g_pNV->NDm(mmiBtnAdcMode) && !g_opr.isDryRun)
	{
		if(!ExistKitStage() && g_pNV->UseSkip(usIndex01 + m_nId))
		{
			g_err.Save(ER_ADC_KIT_NOT_EXIST_STAGE_01 + m_nId);
			return (FALSE);
		}

		// Auto 중에 둘다 없으면 에러
		if((!ExistKitMask() && !ExistKitMaskPicker()) && g_pNV->UseSkip(usIndex01 + m_nId))
		{
			g_err.Save(ER_ADC_KIT_NOT_EXIST_MASK_01 + m_nId);
			return (FALSE);
		}
	}
	
	// Index에 PCB가 없고 Scrap만 있으면 Error
	if(!ExistPcb())
	{
		if(ExistScrap())
		{
			g_err.Save(ER_INDEX01_SCRAP_MEMORY_REMOVE + m_nId);
			return (FALSE);
		}
	}	

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CIndex::InitCyl(void)
{
	if(ExistKitStage())
	{
		if(pmCLOSE != m_pSolStageKitOC->GetPos(300))
		{
			m_pSolStageKitOC->Actuate(pmCLOSE);
			return (FALSE);
		}
	}

	m_pCylMaskPickerUD->Actuate(pmUP);
	if(pmUP != m_pCylMaskPickerUD->GetPos(300))
		return (FALSE);

	if(ExistKitMaskPicker())
	{
		m_pCylMaskPickerOC->Actuate(pmCLOSE);
		if(pmCLOSE != m_pCylMaskPickerOC->GetPos(300))
			return (FALSE);
	}
	else
	{
		m_pCylMaskPickerOC->Actuate(pmOPEN);
		if(pmOPEN != m_pCylMaskPickerOC->GetPos(300))
			return (FALSE);
	}

	if(ExistKitMovePicker())
	{
		// Index 1번에 Picker가 있으면 Close 상태이어야 함
		if(!CylIndexMaskFixAct(pmCLOSE))
			return (FALSE);
	}
	else if(ExistKitMask())
	{
		if(!CylIndexMaskFixAct(pmCLOSE))
			return (FALSE);
	}
	else
	{
		if(!CylIndexMaskFixAct(pmOPEN))
			return (FALSE);
	}

	if(g_pNV->NDm(mmiBtnAdcMode))
	{
		m_pCylDustShutterOC->Actuate(pmCLOSE);
		if(pmCLOSE != m_pCylDustShutterOC->GetPos(200))
			return (FALSE);
	}
	else
	{
		if( !ExistPcb() && m_pMtX->IsRdy(PX_IN_PNP) )
		{
			m_pCylDustShutterOC->Actuate(pmCLOSE);
			if(pmCLOSE != m_pCylDustShutterOC->GetPos(200))
				return (FALSE);
		}
		else 
		{
			m_pCylDustShutterOC->Actuate(pmOPEN);
			if(pmOPEN != m_pCylDustShutterOC->GetPos(200))
				return (FALSE);
		}
	}
	
	return (TRUE);
}


//-------------------------------------------------------------------
void CIndex::CycleRunMaskPickerPickUp(void)
{
	if(!m_fsm.Between(C_MASK_PICKUP_START, C_MASK_PICKUP_END))
		return;

	if(!m_pMtX->IsRdy() || !m_pMtT->IsRdy())
		return;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if(ExistPcb())
	{
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode) + 1);
		_sprintf(cMaterialType, L"PCB");
	}
	else
	{
		_sprintf(cMaterialId, L"$");
		_sprintf(cMaterialType, L"$");
	}

	_char cIndexId[_MAX_CHAR_SIZE_], cEventIdX[_MAX_CHAR_SIZE_], cEventIdT[_MAX_CHAR_SIZE_], cEventIdCyl[_MAX_CHAR_SIZE_];
	_char cTo[_MAX_CHAR_SIZE_];

	_sprintf(cIndexId, L"INDEX%d", (m_nId + 1));
	_sprintf(cTo, L"MASK_PICKER%d", (m_nId + 1));

	// unclamp
	switch(m_fsm.Get())
	{
	case C_MASK_PICKUP_START:
		if(m_fsm.Once())
			NEGRETE_WRITE(g_TpBase.logTransfer(cIndexId, L"MOVE_MASK", g_data2c.cEtc.start, cMaterialId, cMaterialType, cIndexId, cTo))		
			
		if(pmUP != m_pCylMaskPickerUD->GetPos(300))
		{
			m_pCylMaskPickerUD->Actuate(pmUP);

			if(!g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = TRUE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_UP_UNCLAMP_INIT", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_UP_UNCLAMP_INIT", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.off))
			}
		}

		if(pmOPEN != m_pCylMaskPickerOC->GetPos(300))
		{
			m_pCylMaskPickerOC->Actuate(pmOPEN);

			if(!g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()] = TRUE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_OPEN_UNCLAMP_INIT", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.on))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_OPEN_UNCLAMP_INIT", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.off))
			}
		}

		_sprintf(cEventIdX, L"MT_UNCLAMP_INDEX_%02d_X_MASK_PICKER", (m_nId + 1));
		_sprintf(cEventIdT, L"MT_UNCLAMP_INDEX_%02d_T_MASK_PICKER", (m_nId + 1));

		if(!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
		{
			m_pMtX->Move(PX_MASK_PICKER);
			m_pMtT->Move(PT_MASK_PICKER);

			if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
			{
				g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
			}

			if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
			{
				g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
			{
				g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
			}

			if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
			{
				g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
			}
		}

		if(CylIndexMaskFixAct(pmCLOSE))
		{
			m_fsm.Set(C_MASK_PICKUP_01);
		}
		break;
	case C_MASK_PICKUP_01:
		if(CylIndexMaskFixAct(pmOPEN))
		{
			if(pmDOWN != m_pCylMaskPickerUD->GetPos(300))
			{
				m_pCylMaskPickerUD->Actuate(pmDOWN);

				if(!g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = TRUE;
					_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_DOWN_UNCLAMP", (m_nId + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.off))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = FALSE;
					_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_DOWN_UNCLAMP", (m_nId + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
				}
			}

			if(pmCLOSE != m_pCylMaskPickerOC->GetPos(300))
			{
				m_pCylMaskPickerOC->Actuate(pmCLOSE);

				if(!g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()])
				{
					g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()] = TRUE;
					_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_CLOSE", (m_nId + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylMaskPickerOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.off))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()])
				{
					g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()] = FALSE;
					_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_CLOSE", (m_nId + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
														g_data2c.cPmName[m_pCylMaskPickerOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.on))
				}
			}

			m_fsm.Set(C_MASK_PICKUP_02);
		}
		break;
	case C_MASK_PICKUP_02:
		if(pmUP != m_pCylMaskPickerUD->GetPos(300))
		{
			m_pCylMaskPickerUD->Actuate(pmUP);

			if(!g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = TRUE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_UP_UNCLAMP", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_UP_UNCLAMP", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.off))
			}
		}

		m_fsm.Set(C_MASK_PICKUP_END);
		break;
	case C_MASK_PICKUP_END:
		ExistKitMaskPicker() = TRUE;
		ExistKitMask() = FALSE;
// 		if(m_pMem->compPRSFail)
// 		{
// 			m_pMem->compPRSFail = FALSE;
// 		}
// 		else
// 		{
// 			m_pMem->compMaskUnClamp = TRUE;
// 		}
		NEGRETE_WRITE(g_TpBase.logTransfer(cIndexId, L"MOVE_MASK", g_data2c.cEtc.end, cMaterialId, cMaterialType, cIndexId, cTo))		
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CIndex::CycleRunMaskPickerPutDn(void)
{
	if(!m_fsm.Between(C_MASK_PUTDN_START, C_MASK_PUTDN_END))
		return;

	if(!m_pMtX->IsRdy() || !m_pMtT->IsRdy())
		return;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	if(ExistPcb())
	{
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + m_nId].pcbBarcode) + 1);
		_sprintf(cMaterialType, L"PCB");
	}
	else
	{
		_sprintf(cMaterialId, L"$");
		_sprintf(cMaterialType, L"$");
	}

	_char cIndexId[_MAX_CHAR_SIZE_], cEventIdX[_MAX_CHAR_SIZE_], cEventIdT[_MAX_CHAR_SIZE_], cEventIdCyl[_MAX_CHAR_SIZE_];
	_char cTo[_MAX_CHAR_SIZE_];

	_sprintf(cIndexId, L"INDEX%d", (m_nId + 1));
	_sprintf(cTo, L"MASK_PICKER%d", (m_nId + 1));

	switch(m_fsm.Get())
	{
	case C_MASK_PUTDN_START:
		if(m_fsm.Once())
			NEGRETE_WRITE(g_TpBase.logTransfer(cIndexId, L"MOVE_MASK", g_data2c.cEtc.start, cMaterialId, cMaterialType, cTo, cIndexId))		

		if(pmUP != m_pCylMaskPickerUD->GetPos(300))
		{
			m_pCylMaskPickerUD->Actuate(pmUP);

			if(!g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = TRUE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_UP_CLAMP_INIT", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_UP_CLAMP_INIT", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.off))
			}
		}

		if(pmCLOSE != m_pCylMaskPickerOC->GetPos(300))
		{
			m_pCylMaskPickerOC->Actuate(pmCLOSE);

			if(!g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()] = TRUE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_CLOSE_CLAMP_INIT", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.off))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_CLOSE_CLAMP_INIT", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.on))
			}
		}
		
		_sprintf(cEventIdX, L"MT_CLAMP_INDEX_%02d_X_MASK_PICKER", (m_nId + 1));
		_sprintf(cEventIdT, L"MT_CLAMP_INDEX_%02d_T_MASK_PICKER", (m_nId + 1));

		if(!m_pMtX->InPos(PX_MASK_PICKER) || !m_pMtT->InPos(PT_MASK_PICKER))
		{
			m_pMtX->Move(PX_MASK_PICKER);
			m_pMtT->Move(PT_MASK_PICKER);

			if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
			{
				g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
			}

			if(!g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
			{
				g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER])
			{
				g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_MASK_PICKER] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdX, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_POS_], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_SPD_], 
													g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].X[PX_MASK_PICKER][_ACC_]))
			}

			if(g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER])
			{
				g_logChk.bTransfer[m_pMtT->m_config.axisNo][PT_MASK_PICKER] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdT, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtT->m_config.axisNo], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POSIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_POS_], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPDIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_SPD_], 
													g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACCIDX_], g_data2c.cIndex[m_nId].T[PT_MASK_PICKER][_ACC_]))
			}
		}
		
		if(CylIndexMaskFixAct(pmOPEN))
		{
			m_fsm.Set(C_MASK_PUTDN_01);
		}
		break;
	case C_MASK_PUTDN_01:
		if(pmDOWN != m_pCylMaskPickerUD->GetPos(300))
		{
			m_pCylMaskPickerUD->Actuate(pmDOWN);

			if(!g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = TRUE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_DOWN_CLAMP", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.off))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_DOWN_CLAMP", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
			}
		}
		
		if(pmOPEN != m_pCylMaskPickerOC->GetPos(300))
		{
			m_pCylMaskPickerOC->Actuate(pmOPEN);

			if(!g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()] = TRUE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_OPEN_CLAMP", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.on))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerOC->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_OPEN_CLAMP", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskPickerOC->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.off))
			}
		}

		m_fsm.Set(C_MASK_PUTDN_02);
		break;
	case C_MASK_PUTDN_02:
		if(pmUP != m_pCylMaskPickerUD->GetPos(300))
		{
			m_pCylMaskPickerUD->Actuate(pmUP);

			if(!g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = TRUE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_UP_CLAMP", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylMaskPickerUD->GetNo()] = FALSE;
				_sprintf(cEventIdCyl, L"CYL_MASK_KIT_PICKER_%02d_UP_CLAMP", (m_nId + 1));
				NEGRETE_WRITE(g_TpBase.logFunction(cIndexId, cEventIdCyl, g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylMaskPickerUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylMaskPickerUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.off))
			}
		}
		
		if(CylIndexMaskFixAct(pmCLOSE))
		{
			m_fsm.Set(C_MASK_PUTDN_END);
		}
		break;
	case C_MASK_PUTDN_END:
		ExistKitMaskPicker() = FALSE;
		ExistKitMask() = TRUE;

		m_pMem->compMaskClamp = TRUE;
		NEGRETE_WRITE(g_TpBase.logTransfer(cIndexId, L"MOVE_MASK", g_data2c.cEtc.end, cMaterialId, cMaterialType, cTo, cIndexId))		
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void IndexInit(void)
{
	g_index01.Init(INDEX_01, (INDEX_SYS_TEACH*)&g_pNV->DDm(sysTeach01), (INDEX_MEMORY*)&g_pNV->NDm(indexMemory01), (POINT2D*)&g_pNV->DDm(routerOrgPos01));
	g_index02.Init(INDEX_02, (INDEX_SYS_TEACH*)&g_pNV->DDm(sysTeach02), (INDEX_MEMORY*)&g_pNV->NDm(indexMemory02), (POINT2D*)&g_pNV->DDm(routerOrgPos02));
	g_index03.Init(INDEX_03, (INDEX_SYS_TEACH*)&g_pNV->DDm(sysTeach03), (INDEX_MEMORY*)&g_pNV->NDm(indexMemory03), (POINT2D*)&g_pNV->DDm(routerOrgPos03));
	g_index04.Init(INDEX_04, (INDEX_SYS_TEACH*)&g_pNV->DDm(sysTeach04), (INDEX_MEMORY*)&g_pNV->NDm(indexMemory04), (POINT2D*)&g_pNV->DDm(routerOrgPos04));

	g_index01.Init2((PRS_RESULT*)&g_pNV->DDm(prsResult01));
	g_index02.Init2((PRS_RESULT*)&g_pNV->DDm(prsResult02));
	g_index03.Init2((PRS_RESULT*)&g_pNV->DDm(prsResult03));
	g_index04.Init2((PRS_RESULT*)&g_pNV->DDm(prsResult04));

	g_index01.SetHW(&g_mt[MT_INDEX_X_01], &g_mt[MT_INDEX_T_01], &g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_01], 
		            &g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_01], &g_pm[CYL_INDEX_MASK_KIT_FIX_UD_01]);
	g_index02.SetHW(&g_mt[MT_INDEX_X_02], &g_mt[MT_INDEX_T_02], &g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_02], 
		            &g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_02], &g_pm[CYL_INDEX_MASK_KIT_FIX_UD_02]);
	g_index03.SetHW(&g_mt[MT_INDEX_X_03], &g_mt[MT_INDEX_T_03], &g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_03], 
		            &g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_03], &g_pm[CYL_INDEX_MASK_KIT_FIX_UD_03]);
	g_index04.SetHW(&g_mt[MT_INDEX_X_04], &g_mt[MT_INDEX_T_04], &g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_04], 
		            &g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_04], &g_pm[CYL_INDEX_MASK_KIT_FIX_UD_04]);

	g_index01.SetHW2(&g_pm[CYL_MASK_KIT_PICKER_UD_01], &g_pm[CYL_MASK_KIT_PICKER_OC_01]);
	g_index02.SetHW2(&g_pm[CYL_MASK_KIT_PICKER_UD_02], &g_pm[CYL_MASK_KIT_PICKER_OC_02]);
	g_index03.SetHW2(&g_pm[CYL_MASK_KIT_PICKER_UD_03], &g_pm[CYL_MASK_KIT_PICKER_OC_03]);
	g_index04.SetHW2(&g_pm[CYL_MASK_KIT_PICKER_UD_04], &g_pm[CYL_MASK_KIT_PICKER_OC_04]);
	
	g_index01.SetHW3(&g_pm[SOL_INDEX_STAGE_KIT_OC_01], &g_pm[CYL_INDEX_DUST_SHUTTER_OC_01]);
	g_index02.SetHW3(&g_pm[SOL_INDEX_STAGE_KIT_OC_02], &g_pm[CYL_INDEX_DUST_SHUTTER_OC_02]);
	g_index03.SetHW3(&g_pm[SOL_INDEX_STAGE_KIT_OC_03], &g_pm[CYL_INDEX_DUST_SHUTTER_OC_03]);
	g_index04.SetHW3(&g_pm[SOL_INDEX_STAGE_KIT_OC_04], &g_pm[CYL_INDEX_DUST_SHUTTER_OC_04]);

}
