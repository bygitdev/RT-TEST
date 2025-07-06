#include "..\DEF\Includes.h"


//////////////////////////////////////////////////////////////////////////
COutPnp g_outPnp;
//////////////////////////////////////////////////////////////////////////

COutPnp::COutPnp()
{
	m_bRun = FALSE;
	m_nCurIndex  = INDEX_IDLE;
	m_nNextIndex = INDEX_01;
}


//-------------------------------------------------------------------
void COutPnp::AutoRun()
{
	if(!ExistPcb() && !m_fsm.IsRun())
	{
		g_pNV->NDm(outPnpPcbIndex) = INDEX_IDLE;
	}

	int existErrValScrap  = GetExistErrScrap();
	int existErrValPcb	  = GetExistErrPcb();
	int existErrValKit    = GetExistErrKit();

	if(!m_bRun)
		return;

	m_bRun = FALSE;

	if(m_fsm.IsRun())
		return;

	if(EXIST_UNCERTAIN == existErrValScrap)
		return;
	if(EXIST_UNCERTAIN == existErrValPcb)
		return;
	if(EXIST_UNCERTAIN == existErrValKit)
		return;

	if(EXIST_ERR == existErrValScrap)
	{
		g_err.Save(ER_OUTPNP_SCRAP_EXIST);
		return;
	}
	if(EXIST_ERR == existErrValPcb)
	{
		g_err.Save(ER_OUTPNP_PCB_EXIST);
		return;
	}
	if(EXIST_ERR == existErrValKit)
	{
		g_err.Save(ER_ADC_KIT_EXIST_OUTPNP);
		return;
	}

	if(!g_pNV->NDm(mmiBtnAdcMode))
	{
		if(!g_opr.isDryRun)
		{
			if(!ExistKit())
			{
				g_err.Save(ER_ADC_KIT_NOT_EXIST_OUTPNP);
				return;
			}
		}
		
		if(pmCLOSE != m_pSolKitClampOC->GetPos(300))
		{
			m_pSolKitClampOC->Actuate(pmCLOSE);
			return;
		}
	}
	else
	{
		if(ExistScrap())
		{
			g_err.Save(ER_ADC_OUTPNP_SCRAP_EXIST);
			return;
		}
		if(ExistPcb())
		{
			g_err.Save(ER_ADC_OUTPNP_PCB_EXIST);
			return;
		}
	}
	
	if(!m_pMtY->IsRdy() || !m_pMtZ->IsRdy() || !m_pMtX->IsRdy())
		return;

	if(g_err.m_bScrapSafetyBeam)
		return;

	if(!m_pMtZ->IsRdy(PZ_READY))
	{
		m_pMtZ->Move(PZ_READY);
		return;
	}
	if(!m_pMtX->IsRdy(PX_READY))
	{
		m_pMtX->Move(PX_READY);
		return;
	}
	if(pmUP != m_pCylScrapUD->GetPos(300))
	{
		m_pCylScrapUD->Actuate(pmUP);
		return;
	}

	if(!g_pNV->NDm(mmiBtnAdcMode))
	{
		if(ExistScrap())
		{
			if(!IsCylScrapOC(pmCLOSE))
			{
				SetCylScrapOC(pmCLOSE);
				return;
			}
			if(!IsCylScrapFixUD(pmDOWN))
			{
				SetCylScrapFixUD(pmDOWN);
				return;
			}
		}
		else
		{
			if(!IsCylScrapFixUD(pmUP))
			{
				SetCylScrapFixUD(pmUP);
				return;
			}
			if(!IsCylScrapOC(pmOPEN))
			{
				SetCylScrapOC(pmOPEN);
				return;
			}
		}
	}
	else
	{
		if(!IsCylScrapFixUD(pmUP))
		{
			SetCylScrapFixUD(pmUP);
			return;
		}
		if(!IsCylScrapOC(pmCLOSE))
		{
			SetCylScrapOC(pmCLOSE);
			return;
		}
	}

	// Eject X
	/*	if(pmOFF != m_pVacEject->GetPos(300))
	{
	m_pVacEject->Actuate(pmOFF);
	return;
	}*/

	if(ExistPcb())
	{
		if(!g_opr.isDryRun)
		{
			if(pmON != m_pVac->GetPos(300))
			{
				AnalogVac(TRUE);
				m_pVac->Actuate(pmON);// ON
				return;
			}
		}
	}

	SetNextIndex();

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	if(ExistPcb())
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);
	else
		_sprintf(cMaterialId, L"$");

	switch(GetState())
	{
	case S_IDLE:
		if(g_pNV->NDm(mmiBtnAdcMode))
		{
			if(!m_pMtY->InPos(PY_ADC_KIT_CLAMP))
				m_pMtY->Move(PY_ADC_KIT_CLAMP);

			break;
		}

		if(ExistPcb())
		{
			int nPickUpIdx = g_pNV->NDm(outPnpPcbIndex);

			if(nPickUpIdx < INDEX_01 || nPickUpIdx > INDEX_04)
			{
				g_err.Save(ER_OUTPNP_PCB_INDEX_NO);
				break;
			}

			if(!IsReadyMtOutPnpYPutDn(nPickUpIdx))
			{
				MoveMtOutPnpYPutDn(nPickUpIdx);
				break;
			}

			if(g_pIndex[nPickUpIdx]->m_pMtX->ComparePos(CIndex::PX_OUT_PNP))
				break;

			SetSorterStageNo(nPickUpIdx, REQ_SORTER_PNP);

			if(IsReadySorterPnp(nPickUpIdx))
			{
				m_fsm.Set(C_PUTDN_START, nPickUpIdx); 
			}
		}
		else if(ExistScrap())
		{
			if(!m_pMtY->InPos(PY_SCRAP_EJECT))
			{
				m_pMtY->Move(PY_SCRAP_EJECT);

				if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Y_SCRAP_EJECT", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
														g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POSIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POS_], 
														g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPDIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPD_], 
														g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACCIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT])
				{
					g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Y_SCRAP_EJECT", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
														g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POSIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POS_], 
														g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPDIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPD_], 
														g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACCIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACC_]))
				}
			}

			if(!g_dIn.AOn(iScrapBoxExist) && (!g_opr.isDryRun))
			{
				g_wr.Save(WR_SCRAP_BOX_NOT_EXIST);
			}
			else
			{
				g_wr.Del(WR_SCRAP_BOX_NOT_EXIST);
				m_fsm.Set(C_SCRAP_EJECT_START);
			}
		}
		else
		{
			if(!IsReadyMtOutPnpYPickUp(m_nNextIndex))
				MoveMtOutPnpYPickUp(m_nNextIndex);
		}
		break;

	case S_INDEX:
		{
			int nIdx = m_nCurIndex;

			if(ExistPcb())
			{
				int nPickUpIdx = g_pNV->NDm(outPnpPcbIndex);

				if(nPickUpIdx < INDEX_01 || nPickUpIdx > INDEX_04)
				{
					g_err.Save(ER_OUTPNP_PCB_INDEX_NO);
					break;
				}

				if(!IsReadyMtOutPnpYPutDn(nPickUpIdx))
					MoveMtOutPnpYPutDn(nPickUpIdx);
				else
				{
					if(g_pIndex[nPickUpIdx]->m_pMtX->ComparePos(CIndex::PX_OUT_PNP))
						break;

					SetSorterStageNo(nPickUpIdx, REQ_SORTER_PNP);

					if(IsReadySorterPnp(nPickUpIdx))
					{
						m_fsm.Set(C_PUTDN_START, nPickUpIdx);
					}
				}
			}
			else if(ExistScrap())
			{
				if(!m_pMtY->InPos(PY_SCRAP_EJECT))
				{
					m_pMtY->Move(PY_SCRAP_EJECT);

					if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Y_SCRAP_EJECT", g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
															g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POSIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POS_], 
															g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPDIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPD_], 
															g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACCIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACC_]))
					}
					break;
				}
				else
				{
					if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT])
					{
						g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Y_SCRAP_EJECT", g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
															g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POSIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POS_], 
															g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPDIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPD_], 
															g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACCIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACC_]))
					}
				}
				
				if(!g_dIn.AOn(iScrapBoxExist) && (!g_opr.isDryRun))
					g_wr.Save(WR_SCRAP_BOX_NOT_EXIST);
				else
				{
					g_wr.Del(WR_SCRAP_BOX_NOT_EXIST);
					m_fsm.Set(C_SCRAP_EJECT_START);
				}
			}
			else
			{
				if(!g_pIndex[nIdx]->m_pMem->compOutPnp)
				{
					if(!IsReadyMtOutPnpYPickUp(nIdx))
						MoveMtOutPnpYPickUp(nIdx);
					else 
					{
						SetSorterStageNo(nIdx, REQ_SORTER_STAGE);

						if(IsReadySorterIndex(nIdx) && g_pIndex[nIdx]->IsReadyOutPnp())
						{
							m_fsm.Set(C_PICKUP_START, nIdx);
						}
					}
				}
			}				
		}
		break;
	case S_ADC_PUTDN:
		// InPnp가 Stage Kit를 가지고 있으면 충돌함. 회피해야 함
		if(g_inPnp.m_fsm.Between(CInPnp::C_ADC_INDEX_PICKUP_START, CInPnp::C_ADC_INDEX_PICKUP_END))
		{
			if(ADC_KIT_STAGE_01 == g_inPnp.m_fsm.GetMsg() || ADC_KIT_MASK_01 == g_inPnp.m_fsm.GetMsg() || ADC_KIT_PICKER == g_inPnp.m_fsm.GetMsg())
				break;
		}
		if(g_inPnp.m_fsm.Between(CInPnp::C_ADC_INDEX_PUTDN_START, CInPnp::C_ADC_INDEX_PUTDN_END))
		{
			if(ADC_KIT_STAGE_01 == g_inPnp.m_fsm.GetMsg() || ADC_KIT_MASK_01 == g_inPnp.m_fsm.GetMsg() || ADC_KIT_PICKER == g_inPnp.m_fsm.GetMsg())
				break;
		}

		if(g_inPnp.ExistKit() && (g_inPnp.KitInfo() == ADC_KIT_STAGE_01))
		{
			if(g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_STAGE_01) || g_inPnp.m_pMtY->ComparePos(CInPnp::PY_PUTDN_01))
				break;
			if(g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_MASK_01))
				break;
		}

		// 임시 Sorter의 상태를 보지 않는다
		//if(g_pIndex[INDEX_01]->IsReadyAdcOutPnpPutDnPicker())
		//	m_fsm.Set(C_ADC_PUTDN_START);

		// Sorter에서 Auto Run Mode시에 안전조건 보고 가야 함
		SetSorterStageNo(INDEX_01, REQ_SORTER_STAGE);
		if(IsReadySorterIndex(INDEX_01) && g_pIndex[INDEX_01]->IsReadyAdcOutPnpPutDnPicker())
			m_fsm.Set(C_ADC_PUTDN_START);
		break;
	case S_ADC_PICKUP:
		// InPnp가 Stage Kit를 가지고 있으면 충돌함. 회피해야 함
		if(g_inPnp.m_fsm.Between(CInPnp::C_ADC_INDEX_PICKUP_START, CInPnp::C_ADC_INDEX_PICKUP_END))
		{
			if(ADC_KIT_STAGE_01 == g_inPnp.m_fsm.GetMsg() || ADC_KIT_MASK_01 == g_inPnp.m_fsm.GetMsg() || ADC_KIT_PICKER == g_inPnp.m_fsm.GetMsg())
				break;
		}
		if(g_inPnp.m_fsm.Between(CInPnp::C_ADC_INDEX_PUTDN_START, CInPnp::C_ADC_INDEX_PUTDN_END))
		{
			if(ADC_KIT_STAGE_01 == g_inPnp.m_fsm.GetMsg() || ADC_KIT_MASK_01 == g_inPnp.m_fsm.GetMsg() || ADC_KIT_PICKER == g_inPnp.m_fsm.GetMsg())
				break;
		}

		if(g_inPnp.ExistKit() && (g_inPnp.KitInfo() == ADC_KIT_STAGE_01))
		{
			if(g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_STAGE_01) || g_inPnp.m_pMtY->ComparePos(CInPnp::PY_PUTDN_01))
				break;
			if(g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_MASK_01))
				break;
		}

		// 임시 Sorter의 상태를 보지 않는다
		//if(g_pIndex[INDEX_01]->IsReadyAdcOutPnpPickUpPicker())
		//	m_fsm.Set(C_ADC_PICKUP_START);

		// Sorter에서 Auto Run Mode시에 안전조건 보고 가야 함
		SetSorterStageNo(INDEX_01, REQ_SORTER_STAGE);
		if(IsReadySorterIndex(INDEX_01) && g_pIndex[INDEX_01]->IsReadyAdcOutPnpPickUpPicker())
			m_fsm.Set(C_ADC_PICKUP_START);
		break;
	}
}
	

//-------------------------------------------------------------------
void COutPnp::CycleRun(void)
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
		if(m_fsm.Between(C_PUTDN_START, C_PUTDN_END))
		{
			g_dOut.On(oSorterError);
		}
		
		m_fsm.Set(C_ERROR);
		return;
	}

	CycleRunPickUp();
	CycleRunScrapEject();
	CycleRunPcbPutDn();
	CycleRunAdcPickUp();
	CycleRunAdcPutDn();
}


//-------------------------------------------------------------------
void COutPnp::Init(void)
{
	m_pMtY	= &g_mt[MT_OUTPNP_Y];
	m_pMtZ	= &g_mt[MT_OUTPNP_Z];
	m_pMtX	= &g_mt[MT_OUTPNP_X];

	m_pCylScrapUD		= &g_pm[CYL_OUTPNP_SCRAP_UD];
	m_pCylScrapOC_F		= &g_pm[CYL_OUTPNP_SCRAP_OC_F];
	m_pCylScrapOC_R		= &g_pm[CYL_OUTPNP_SCRAP_OC_R];
	m_pCylScrapFixUD_F	= &g_pm[CYL_OUTPNP_SCRAP_FIX_UD_F];
	m_pCylScrapFixUD_R	= &g_pm[CYL_OUTPNP_SCRAP_FIX_UD_R];

	m_pVac				= &g_pm[VAC_OUTPNP];
	m_pVacEject			= &g_pm[VAC_OUTPNP_EJECT];
	m_pSolKitClampOC	= &g_pm[SOL_OUTPNP_KIT_CLAMP_OC];
}


//-------------------------------------------------------------------
int& COutPnp::ExistScrap(void)
{
	return (g_pNV->m_pData->ndm[existOutPnpScrap]);
}


//-------------------------------------------------------------------
int& COutPnp::ExistPcb(void)
{
	return (g_pNV->m_pData->ndm[existOutPnpPcb]);
}


//-------------------------------------------------------------------
int& COutPnp::ExistKit(void)
{
	return (g_pNV->m_pData->ndm[existKitOutPnp]);
}


//-------------------------------------------------------------------
int& COutPnp::KitJobType(void)
{
	return (g_pNV->m_pData->ndm[adcOutPnpJobType]);
}


//-------------------------------------------------------------------
int COutPnp::GetState(void)
{
	int nState = S_IDLE;

	if(g_pNV->NDm(mmiBtnAdcMode))
	{
		if(ExistKit())
		{
			if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcOutPnpJobType)))
				nState = S_ADC_PUTDN;
		}
		else
		{
			nState = S_ADC_PICKUP;
		}
	}
	else
	{
		if(INDEX_IDLE < m_nCurIndex)
			nState = S_INDEX;
	}
	
	return (nState);
}


//-------------------------------------------------------------------
BOOL COutPnp::IsReadySorterIndex(int nIdx)
{
	// Sorter Index가 자재를 받을 수 있는 조건
	// ADC가 완료되면 Sorter는 정지되므로 ADC 진행 중일 때는 Sorter의 Auto 상태를 보지 않는다.
	if(!g_pNV->NDm(mmiBtnAdcMode))
	{
		if(!g_dIn.AOn(iSorterAutoRun))
			return (FALSE);
	}
	if(g_dIn.AOn(iSorterError))
		return (FALSE);

	if(g_pNV->NDm(mmiBtnAdcMode))
	{
		if(INDEX_01 == nIdx)
		{
			if(!g_dIn.AOn(iSorterStageSafety01))
				return (FALSE);
		}
		else if(INDEX_02 == nIdx)
		{
			if(!g_dIn.AOn(iSorterStageSafety02))
				return (FALSE);
		}
		else if(INDEX_03 == nIdx)
		{
			if(!g_dIn.AOn(iSorterStageSafety03))
				return (FALSE);
		}
		else if(INDEX_04 == nIdx)
		{
			if(!g_dIn.AOn(iSorterStageSafety04))
				return (FALSE);
		}
	}
	else
	{
		if(INDEX_01 == nIdx)
		{
			if(!g_dIn.AOn(iSorterStageAllow01))
				return (FALSE);
		}
		else if(INDEX_02 == nIdx)
		{
			if(!g_dIn.AOn(iSorterStageAllow02))
				return (FALSE);
		}
		else if(INDEX_03 == nIdx)
		{
			if(!g_dIn.AOn(iSorterStageAllow03))
				return (FALSE);
		}
		else if(INDEX_04 == nIdx)
		{
			if(!g_dIn.AOn(iSorterStageAllow04))
				return (FALSE);
		}
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL COutPnp::IsReadySorterPnp(int nIdx)
{
	// Sorter Index가 자재를 받을 수 있는 조건
	if(!g_dIn.AOn(iSorterAutoRun))
		return (FALSE);
	if(g_dIn.AOn(iSorterError))
		return (FALSE);

	if(INDEX_01 == nIdx || INDEX_02 == nIdx)
	{
		if(!g_dIn.AOn(iSorterPickerAllow0102))
			return (FALSE);
		if(!g_dIn.AOn(iSorterPickerSafety0102))
			return (FALSE);
	}
	else
	{
		if(!g_dIn.AOn(iSorterPickerAllow0304))
			return (FALSE);
		if(!g_dIn.AOn(iSorterPickerSafety0304))
			return (FALSE);
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL COutPnp::IsReadyMtOutPnpYPickUp(int nIdx)
{
	int    nIndex  = PY_PICKUP_01 + nIdx;
	double dOffset = g_pNV->Pkg(offsetOutPnpYPickUp1 + nIdx) * 1000.0;	
	double dPos	   = m_pMtY->m_pTable->pos[nIndex] + dOffset;

	if(!m_pMtY->IsRdy())
		return (FALSE);

	if(!m_pMtY->InPos(nIndex, dPos, 50))
		return (FALSE);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	_sprintf(cMaterialId, L"$");

	_char cEventId[_MAX_CHAR_SIZE_];
	_sprintf(cEventId, L"MT_OUT_PNP_Y_PICKUP_%02d", (nIdx + 1));

	_char cPosY[_MAX_CHAR_SIZE_];
	_sprintf(cPosY, L"%03f", dPos);

	if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex] = FALSE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.end, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
											g_data2c.cOutPnp.Y[nIndex][_POSIDX_], cPosY, 
											g_data2c.cOutPnp.Y[nIndex][_SPDIDX_], g_data2c.cOutPnp.Y[nIndex][_SPD_], 
											g_data2c.cOutPnp.Y[nIndex][_ACCIDX_], g_data2c.cOutPnp.Y[nIndex][_ACC_]))
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL COutPnp::MoveMtOutPnpYPickUp(int nIdx)
{
	int	   nIndex  = PY_PICKUP_01 + nIdx;
	double dOffset = g_pNV->Pkg(offsetOutPnpYPickUp1 + nIdx) * 1000.0;	
	double dPos	   = m_pMtY->m_pTable->pos[nIndex] + dOffset;

	m_pMtY->PMove(nIndex, dPos);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	_sprintf(cMaterialId, L"$");

	_char cEventId[_MAX_CHAR_SIZE_];
	_sprintf(cEventId, L"MT_OUT_PNP_Y_PICKUP_%02d", (nIdx + 1));

	_char cPosY[_MAX_CHAR_SIZE_];
	_sprintf(cPosY, L"%03f", dPos);

	if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex] = TRUE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.start, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
											g_data2c.cOutPnp.Y[nIndex][_POSIDX_], cPosY, 
											g_data2c.cOutPnp.Y[nIndex][_SPDIDX_], g_data2c.cOutPnp.Y[nIndex][_SPD_], 
											g_data2c.cOutPnp.Y[nIndex][_ACCIDX_], g_data2c.cOutPnp.Y[nIndex][_ACC_]))
	}

	return (TRUE);
}

//-------------------------------------------------------------------
BOOL COutPnp::IsReadyMtOutPnpYPutDn(int nIdx)
{
	int    nIndex  = PY_PUTDN_01 + nIdx;
	double dOffset = g_pNV->Pkg(offsetOutPnpYPutDn1 + nIdx) * 1000.0;	
	double dPos	   = m_pMtY->m_pTable->pos[nIndex] + dOffset;

	if(!m_pMtY->IsRdy())
		return (FALSE);

	if(!m_pMtY->InPos(nIndex, dPos, 50))
		return (FALSE);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	if (g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);

	_char cEventId[_MAX_CHAR_SIZE_];
	_sprintf(cEventId, L"MT_OUT_PNP_Y_PUTDN_%02d", (nIdx + 1));

	_char cPosY[_MAX_CHAR_SIZE_];
	_sprintf(cPosY, L"%03f", dPos);

	if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex] = FALSE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.end, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
											g_data2c.cOutPnp.Y[nIndex][_POSIDX_], cPosY, 
											g_data2c.cOutPnp.Y[nIndex][_SPDIDX_], g_data2c.cOutPnp.Y[nIndex][_SPD_], 
											g_data2c.cOutPnp.Y[nIndex][_ACCIDX_], g_data2c.cOutPnp.Y[nIndex][_ACC_]))
	}

	return (TRUE);
}

//-------------------------------------------------------------------
BOOL COutPnp::MoveMtOutPnpYPutDn(int nIdx)
{
	int	   nIndex  = PY_PUTDN_01 + nIdx;
	double dOffset = g_pNV->Pkg(offsetOutPnpYPutDn1 + nIdx) * 1000.0;
	double dPos	   = m_pMtY->m_pTable->pos[nIndex] + dOffset;

	m_pMtY->PMove(nIndex, dPos);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	if (g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);

	_char cEventId[_MAX_CHAR_SIZE_];
	_sprintf(cEventId, L"MT_OUT_PNP_Y_PUTDN_%02d", (nIdx + 1));

	_char cPosY[_MAX_CHAR_SIZE_];
	_sprintf(cPosY, L"%03f", dPos);

	if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex] = TRUE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.start, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
											g_data2c.cOutPnp.Y[nIndex][_POSIDX_], cPosY, 
											g_data2c.cOutPnp.Y[nIndex][_SPDIDX_], g_data2c.cOutPnp.Y[nIndex][_SPD_], 
											g_data2c.cOutPnp.Y[nIndex][_ACCIDX_], g_data2c.cOutPnp.Y[nIndex][_ACC_]))
	}

	return (TRUE);
}

//-------------------------------------------------------------------
BOOL COutPnp::IsReadyMtOutPnpZPickUp(int nIdx)
{
	int    nIndex  = PZ_PICKUP_01 + nIdx;
	double dOffset = g_pNV->Pkg(offsetOutPnpZPickUp1 + nIdx) * 1000.0;	
	double dPos	   = m_pMtZ->m_pTable->pos[nIndex] + dOffset;

	if(!m_pMtZ->IsRdy())
		return (FALSE);

	if(!m_pMtZ->InPos(nIndex, dPos, 50))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL COutPnp::MoveMtOutPnpZPickUp(int nIdx)
{
	int	   nIndex  = PZ_PICKUP_01 + nIdx;
	double dOffset = g_pNV->Pkg(offsetOutPnpZPickUp1 + nIdx) * 1000.0;
	double dPos	   = m_pMtZ->m_pTable->pos[nIndex] + dOffset;

	m_pMtZ->PMove(nIndex, dPos);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL COutPnp::IsReadyMtOutPnpXSorter(int nIdx)
{
	int    nIndex  = PX_SORTER_01 + nIdx;
	double dOffset = g_pNV->Pkg(offsetOutPnpXSorter1 + nIdx) * 1000.0;	
	double dPos	   = m_pMtX->m_pTable->pos[nIndex] + dOffset;

	if(!m_pMtX->IsRdy())
		return (FALSE);

	if(!m_pMtX->InPos(nIndex, dPos, 50))
		return (FALSE);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	if (g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);

	_char cEventId[_MAX_CHAR_SIZE_];
	_sprintf(cEventId, L"MT_OUT_PNP_X_PUTDN_%02d", (nIdx + 1));

	_char cPosX[_MAX_CHAR_SIZE_];
	_sprintf(cPosX, L"%03f", dPos);

	if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtX->m_config.axisNo][nIndex] = FALSE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.end, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
											g_data2c.cOutPnp.X[nIndex][_POSIDX_], cPosX, 
											g_data2c.cOutPnp.X[nIndex][_SPDIDX_], g_data2c.cOutPnp.X[nIndex][_SPD_], 
											g_data2c.cOutPnp.X[nIndex][_ACCIDX_], g_data2c.cOutPnp.X[nIndex][_ACC_]))
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL COutPnp::MoveMtOutPnpXSorter(int nIdx)
{
	int	   nIndex  = PX_SORTER_01 + nIdx;
	double dOffset = g_pNV->Pkg(offsetOutPnpXSorter1 + nIdx) * 1000.0;
	double dPos	   = m_pMtX->m_pTable->pos[nIndex] + dOffset;

	m_pMtX->PMove(nIndex, dPos);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	if (g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);

	_char cEventId[_MAX_CHAR_SIZE_];
	_sprintf(cEventId, L"MT_OUT_PNP_X_PUTDN_%02d", (nIdx + 1));

	_char cPosX[_MAX_CHAR_SIZE_];
	_sprintf(cPosX, L"%03f", dPos);

	if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtX->m_config.axisNo][nIndex] = TRUE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.start, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
											g_data2c.cOutPnp.X[nIndex][_POSIDX_], cPosX, 
											g_data2c.cOutPnp.X[nIndex][_SPDIDX_], g_data2c.cOutPnp.X[nIndex][_SPD_], 
											g_data2c.cOutPnp.X[nIndex][_ACCIDX_], g_data2c.cOutPnp.X[nIndex][_ACC_]))
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL COutPnp::IsReadyMtOutPnpZOverride(int nMtIdx)
{
	int    nIndex = nMtIdx;
	double dPos	  = m_pMtZ->m_pTable->pos[nIndex];
	double dOffset = 0.0;	

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");

	_char cEventId[_MAX_CHAR_SIZE_];

	if(Between(nMtIdx, PZ_PICKUP_01, PZ_PICKUP_04))
	{
		_sprintf(cEventId, L"MT_OUT_PNP_Z_PICKUP_%02d", (nIndex + 1));
		_sprintf(cMaterialId, L"$");

		if(PZ_PICKUP_01 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPickUp1) * 1000.0;	
		else if(PZ_PICKUP_02 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPickUp2) * 1000.0;	
		else if(PZ_PICKUP_03 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPickUp3) * 1000.0;	
		else if(PZ_PICKUP_04 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPickUp4) * 1000.0;

		dPos += dOffset;
	}
	else if(Between(nMtIdx, PZ_PUTDN_01, PZ_PUTDN_04))
	{
		_sprintf(cEventId, L"MT_OUT_PNP_Z_PUTDN_%02d", (nIndex + 1));
		if (g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode[0] == L'\0') 
			_sprintf(cMaterialId, L"$");
		else
			mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);

		if(PZ_PUTDN_01 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPutDn1) * 1000.0;	
		else if(PZ_PUTDN_02 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPutDn2) * 1000.0;	
		else if(PZ_PUTDN_03 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPutDn3) * 1000.0;	
		else if(PZ_PUTDN_04 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPutDn4) * 1000.0;

		dPos += dOffset;
	}

	if(!m_pMtZ->IsRdy())
		return (FALSE);

	if(!m_pMtZ->InPos(nIndex, dPos, 50))
		return (FALSE);

	_char cPosZ[_MAX_CHAR_SIZE_];
	_sprintf(cPosZ, L"%03f", dPos);

	if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtZ->m_config.axisNo][nIndex] = FALSE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.end, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
											g_data2c.cOutPnp.Z[nIndex][_POSIDX_], cPosZ, 
											g_data2c.cOutPnp.Z[nIndex][_SPDIDX_], g_data2c.cOutPnp.Z[nIndex][_SPD_], 
											g_data2c.cOutPnp.Z[nIndex][_ACCIDX_], g_data2c.cOutPnp.Z[nIndex][_ACC_]))
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL COutPnp::MoveMtOutPnpZOverride(int nMtIdx)
{
	int	   nIndex = nMtIdx;
	double dPos	  = m_pMtZ->m_pTable->pos[nIndex];
	double dOffset = 0.0;	

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");

	_char cEventId[_MAX_CHAR_SIZE_];

	if(Between(nMtIdx, PZ_PICKUP_01, PZ_PICKUP_04))
	{
		_sprintf(cEventId, L"MT_OUT_PNP_Z_PICKUP_%02d", (nIndex + 1));
		_sprintf(cMaterialId, L"$");

		if(PZ_PICKUP_01 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPickUp1) * 1000.0;	
		else if(PZ_PICKUP_02 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPickUp2) * 1000.0;	
		else if(PZ_PICKUP_03 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPickUp3) * 1000.0;	
		else if(PZ_PICKUP_04 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPickUp4) * 1000.0;

		dPos += dOffset;
	}
	else if(Between(nMtIdx, PZ_PUTDN_01, PZ_PUTDN_04))
	{
		_sprintf(cEventId, L"MT_OUT_PNP_Z_PUTDN_%02d", (nIndex + 1));
		if (g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode[0] == L'\0') 
			_sprintf(cMaterialId, L"$");
		else
			mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);

		if(PZ_PUTDN_01 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPutDn1) * 1000.0;	
		else if(PZ_PUTDN_02 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPutDn2) * 1000.0;	
		else if(PZ_PUTDN_03 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPutDn3) * 1000.0;	
		else if(PZ_PUTDN_04 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetOutPnpZPutDn4) * 1000.0;

		dPos += dOffset;
	}

	double endPos   = dPos;
	double slowPos  = dPos - m_pMtZ->m_pTable->pos[PZ_SLOW_DN_OFFSET];
	double startVel = m_pMtZ->m_pTable->vel[nIndex];
	double slowVel  = m_pMtZ->m_pTable->vel[PZ_SLOW_DN_OFFSET];

	m_pMtZ->Move2(nIndex, endPos, slowPos, slowVel, startVel);

	_char cPosZ[_MAX_CHAR_SIZE_];
	_sprintf(cPosZ, L"%03f", endPos);

	if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtZ->m_config.axisNo][nIndex] = TRUE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.start, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
											g_data2c.cOutPnp.Z[nIndex][_POSIDX_], cPosZ, 
											g_data2c.cOutPnp.Z[nIndex][_SPDIDX_], g_data2c.cOutPnp.Z[nIndex][_SPD_], 
											g_data2c.cOutPnp.Z[nIndex][_ACCIDX_], g_data2c.cOutPnp.Z[nIndex][_ACC_]))
	}

	return (TRUE);
}


//-------------------------------------------------------------------
void COutPnp::SetSorterStageNo(int nIdx, int nReq)
{
	if(REQ_SORTER_PNP == nReq)
	{
		if(INDEX_01 == nIdx)
		{
			g_dOut.Off(oSorterStageNoL);
			g_dOut.Off(oSorterStageNoH);
		}
		else if(INDEX_02 == nIdx)
		{
			g_dOut.On(oSorterStageNoL);
			g_dOut.Off(oSorterStageNoH);
		}
		else if(INDEX_03 == nIdx)
		{
			g_dOut.Off(oSorterStageNoL);
			g_dOut.On(oSorterStageNoH);
		}
		else if(INDEX_04 == nIdx)
		{
			g_dOut.On(oSorterStageNoL);
			g_dOut.On(oSorterStageNoH);
		}
		g_dOut.On(oSorterPickerReq);
	}
	else if(REQ_SORTER_STAGE == nReq)
	{
		if(INDEX_01 == nIdx)
		{
			if(g_dIn.AOn(iSorterStageSafety01))
				g_dOut.On(oSorterStageReq01);
		}
		else if(INDEX_02 == nIdx)
		{
			if(g_dIn.AOn(iSorterStageSafety02))
				g_dOut.On(oSorterStageReq02);
		}
		else if(INDEX_03 == nIdx)
		{
			if(g_dIn.AOn(iSorterStageSafety03))
				g_dOut.On(oSorterStageReq03);
		}
		else if(INDEX_04 == nIdx)
		{
			if(g_dIn.AOn(iSorterStageSafety04))
				g_dOut.On(oSorterStageReq04);
		}
	}
	else
	{
		g_dOut.Off(oSorterPickerReq);
		g_dOut.Off(oSorterStageReq01);
		g_dOut.Off(oSorterStageReq02);
		g_dOut.Off(oSorterStageReq03);
		g_dOut.Off(oSorterStageReq04);
	}
}


//-------------------------------------------------------------------
void COutPnp::SetCylScrapFixUD(BOOL bAct, BOOL bType, BOOL bLog)
{
	m_pCylScrapFixUD_F->Actuate(bAct);
	m_pCylScrapFixUD_R->Actuate(bAct);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");	
	if(PICKUP == bType)
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);

	_sprintf(cMaterialId, L"$");

	if(bLog)
	{
		if(!g_logChk.bFunction[m_pCylScrapFixUD_F->GetNo()])
		{
			g_logChk.bFunction[m_pCylScrapFixUD_F->GetNo()] = TRUE;

			if(pmDOWN == bAct)
			{
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_F_DOWN", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylScrapFixUD_F->GetNo()], g_data2c.cEtc.delayTime, L"200", 
													g_data2c.cPmIO[m_pCylScrapFixUD_F->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylScrapFixUD_F->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.off))
			}
			else
			{
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_F_UP", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylScrapFixUD_F->GetNo()], g_data2c.cEtc.delayTime, L"200", 
													g_data2c.cPmIO[m_pCylScrapFixUD_F->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapFixUD_F->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
			}
		}
	}

	if(bLog)
	{
		if(!g_logChk.bFunction[m_pCylScrapFixUD_R->GetNo()])
		{
			g_logChk.bFunction[m_pCylScrapFixUD_R->GetNo()] = TRUE;

			if(pmDOWN == bAct)
			{
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_R_DOWN", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylScrapFixUD_R->GetNo()], g_data2c.cEtc.delayTime, L"200", 
													g_data2c.cPmIO[m_pCylScrapFixUD_R->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylScrapFixUD_R->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.off))
			}
			else
			{
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_R_UP", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylScrapFixUD_R->GetNo()], g_data2c.cEtc.delayTime, L"200", 
													g_data2c.cPmIO[m_pCylScrapFixUD_R->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapFixUD_R->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
			}
		}
	}
}


//-------------------------------------------------------------------
BOOL COutPnp::IsCylScrapFixUD(BOOL bAct, BOOL bType, BOOL bLog)
{
	if(bAct != m_pCylScrapFixUD_F->GetPos(300))
		return (FALSE);
	if(bAct != m_pCylScrapFixUD_R->GetPos(300))
		return (FALSE);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");	
	if(PICKUP == bType)
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);

	_sprintf(cMaterialId, L"$");

	if(bLog)
	{
		if(g_logChk.bFunction[m_pCylScrapFixUD_F->GetNo()])
		{
			g_logChk.bFunction[m_pCylScrapFixUD_F->GetNo()] = FALSE;

			if(pmDOWN == bAct)
			{
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_F_DOWN", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylScrapFixUD_F->GetNo()], g_data2c.cEtc.delayTime, L"200", 
													g_data2c.cPmIO[m_pCylScrapFixUD_F->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapFixUD_F->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
			}
			else
			{
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_F_UP", g_data2c.cEtc.end,
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylScrapFixUD_F->GetNo()], g_data2c.cEtc.delayTime, L"200", 
													g_data2c.cPmIO[m_pCylScrapFixUD_F->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapFixUD_F->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
			}
		}
	}

	if(bLog)
	{
		if(g_logChk.bFunction[m_pCylScrapFixUD_R->GetNo()])
		{
			g_logChk.bFunction[m_pCylScrapFixUD_R->GetNo()] = FALSE;

			if(pmDOWN == bAct)
			{
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_R_DOWN", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylScrapFixUD_R->GetNo()], g_data2c.cEtc.delayTime, L"200", 
													g_data2c.cPmIO[m_pCylScrapFixUD_R->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapFixUD_R->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
			}
			else
			{
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_R_UP", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylScrapFixUD_R->GetNo()], g_data2c.cEtc.delayTime, L"200", 
													g_data2c.cPmIO[m_pCylScrapFixUD_R->GetNo()].In[pmUP][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylScrapFixUD_R->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.off))
			}
		}
	}


	return (TRUE);
}
//-------------------------------------------------------------------
BOOL COutPnp::AnalogVac(BOOL bAct)
{
	if(bAct == FALSE) // 백큠 OFF 일때
	{
		g_ao.m_dVolt[aoOutPnpVacVolt] = 0;// 백큠 0나오는지 확인
		return (TRUE);
	}
	else if(bAct == TRUE) // 백큠 ON일때
	{
		if(g_pNV->Pkg(OutPnpVacRcpUseSkip) == 0) // 기존 사용하는 Rcp이면,
		{
			if(g_pNV->DDm(OutPnpVacCommon) == 0)
				g_ao.m_dVolt[aoOutPnpVacVolt] = 4.3; // 설정안되어있으면 그냥 4.3
			else
				g_ao.m_dVolt[aoOutPnpVacVolt] = g_pNV->DDm(OutPnpVacCommon); // 4.3으로 넣어줌, 용량 최대로설정시,
		}
		else // 저용랑 or 신제품 셋업하는 Rcp
		{
			if(g_pNV->Pkg(OutPnpVacSetValue) == 0) // 만약 저용량 & 신제품셋업했을때, VAC가 0이면,
			{
				if(g_pNV->DDm(OutPnpVacCommon) == 0)
					g_ao.m_dVolt[aoOutPnpVacVolt] = 4.3; // 설정안되어있으면 그냥 4.3
				else
					g_ao.m_dVolt[aoOutPnpVacVolt] = g_pNV->DDm(OutPnpVacCommon); // 4.3으로 넣어줌, 용량 최대로설정시,
			}
			else
			{
				g_ao.m_dVolt[aoOutPnpVacVolt] = g_pNV->Pkg(OutPnpVacSetValue);
			}
		}
		return (TRUE);
	}
	return (FALSE);
}

//-------------------------------------------------------------------
void COutPnp::SetCylScrapOC(BOOL bAct)
{
	m_pCylScrapOC_F->Actuate(bAct);
	m_pCylScrapOC_R->Actuate(bAct);
}


//-------------------------------------------------------------------
BOOL COutPnp::IsCylScrapOC(BOOL bAct)
{
	if(bAct != m_pCylScrapOC_F->GetPos(300))
		return (FALSE);
	if(bAct != m_pCylScrapOC_R->GetPos(300))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL COutPnp::IsErr(void)
{
	if(!m_pMtY->m_state.isHome)
		return (TRUE);
	if(!m_pMtZ->m_state.isHome)
		return (TRUE);
	if(!m_pMtX->m_state.isHome)
		return (TRUE);

	if(0 < m_pCylScrapUD->GetErr())
		return (TRUE);
	if(0 < m_pCylScrapOC_F->GetErr())
		return (TRUE);
	if(0 < m_pCylScrapOC_R->GetErr())
		return (TRUE);
	if(0 < m_pCylScrapFixUD_F->GetErr())
		return (TRUE);
	if(0 < m_pCylScrapFixUD_R->GetErr())
		return (TRUE);
	if(0 < m_pVac->GetErr())
		return (TRUE);

	if(m_fsm.Between(C_PICKUP_START, C_PICKUP_END))
	{
		int nIdx = m_fsm.GetMsg();

		if(!g_pIndex[nIdx]->m_pMtX->m_state.isHome)
			return (TRUE);
		if(!g_pIndex[nIdx]->m_pMtT->m_state.isHome)
			return (TRUE);
		if(0 < g_pIndex[nIdx]->m_pCylMaskFB_L->GetErr())
			return (TRUE);
		if(0 < g_pIndex[nIdx]->m_pCylMaskFB_R->GetErr())
			return (TRUE);
		if(0 < g_pIndex[nIdx]->m_pCylMaskUD->GetErr())
			return (TRUE);
		if(0 < g_pIndex[nIdx]->m_pCylDustShutterOC->GetErr())
			return (TRUE);
	}

	if(m_fsm.Between(C_ADC_PICKUP_START, C_ADC_PICKUP_END) || 
	   m_fsm.Between(C_ADC_PUTDN_START, C_ADC_PUTDN_END))
	{
		if(0 < g_pIndex[INDEX_01]->m_pCylMaskFB_L->GetErr())
			return (TRUE);
		if(0 < g_pIndex[INDEX_01]->m_pCylMaskFB_R->GetErr())
			return (TRUE);
		if(0 < g_pIndex[INDEX_01]->m_pCylMaskUD->GetErr())
			return (TRUE);
	}

	return (FALSE);
}


//-------------------------------------------------------------------
int  COutPnp::GetExistErrScrap(void)
{	
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL isSenOn = g_dIn.AOn(iOutPnpScrapExist);

	if(ExistScrap() == isSenOn)
	{
		m_tmExistErrScrap.Reset();
	}
	else
	{
		if(m_tmExistErrScrap.TmOver(5000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}
	
	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
int  COutPnp::GetExistErrPcb(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL isSenOn = (pmON == m_pVac->GetPos(10)); // OFF->ON

	if(ExistPcb() == isSenOn)
	{
		m_tmExistErrPcb.Reset();
	}
	else
	{
		if(m_tmExistErrPcb.TmOver(2000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}
	
	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
int  COutPnp::GetExistErrKit(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL isSenOn = g_dIn.AOn(iOutPnpKitExist01) && g_dIn.AOn(iOutPnpKitExist02);

	if(ExistKit() == isSenOn)
	{
		m_tmExistErrKit.Reset();
	}
	else
	{
		if(m_tmExistErrKit.TmOver(5000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}
	
	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
void COutPnp::SetNextIndex(void)
{
	// Index 1, 3, 2, 4 순으로 동작되도록 수정
	if(INDEX_01 == m_nCurIndex)
	{
		if(g_pNV->UseSkip(usIndex03))
			m_nNextIndex = INDEX_03;
		else if(g_pNV->UseSkip(usIndex02))
			m_nNextIndex = INDEX_02;
		else if(g_pNV->UseSkip(usIndex04))
			m_nNextIndex = INDEX_04;
		else
			m_nNextIndex = INDEX_01;
	}
	else if(INDEX_02 == m_nCurIndex)
	{
		if(g_pNV->UseSkip(usIndex04))
			m_nNextIndex = INDEX_04;
		else if(g_pNV->UseSkip(usIndex01))
			m_nNextIndex = INDEX_01;
		else if(g_pNV->UseSkip(usIndex03))
			m_nNextIndex = INDEX_03;
		else
			m_nNextIndex = INDEX_02;
	}
	else if(INDEX_03 == m_nCurIndex)
	{
		if(g_pNV->UseSkip(usIndex02))
			m_nNextIndex = INDEX_02;
		else if(g_pNV->UseSkip(usIndex04))
			m_nNextIndex = INDEX_04;
		else if(g_pNV->UseSkip(usIndex01))
			m_nNextIndex = INDEX_01;
		else
			m_nNextIndex = INDEX_02;
	}
	else if(INDEX_04 == m_nCurIndex)
	{
		if(g_pNV->UseSkip(usIndex01))
			m_nNextIndex = INDEX_01;
		else if(g_pNV->UseSkip(usIndex03))
			m_nNextIndex = INDEX_03;
		else if(g_pNV->UseSkip(usIndex02))
			m_nNextIndex = INDEX_02;
		else
			m_nNextIndex = INDEX_04;
	}
}


//-------------------------------------------------------------------
void COutPnp::SetStageBusy(int nIdx)
{
	if(INDEX_01 == nIdx)
	{
		g_dOut.On(oSorterStageBusy01);
		g_dOut.Off(oSorterStageSafety01);
	}
	else if(INDEX_02 == nIdx)
	{
		g_dOut.On(oSorterStageBusy02);
		g_dOut.Off(oSorterStageSafety02);
	}
	else if(INDEX_03 == nIdx)
	{
		g_dOut.On(oSorterStageBusy03);
		g_dOut.Off(oSorterStageSafety03);
	}
	else if(INDEX_04 == nIdx)
	{
		g_dOut.On(oSorterStageBusy04);
		g_dOut.Off(oSorterStageSafety04);
	}
}
	
//-------------------------------------------------------------------
void COutPnp::CycleRunPickUp(void)
{
	if(!m_fsm.Between(C_PICKUP_START, C_PICKUP_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_OUTPNP_PICKUP_CYCLE_TM_OVER);
		return;
	}

	int nIdx = m_fsm.GetMsg();
	SetStageBusy(nIdx);

	if(!m_pMtY->IsRdy() || !m_pMtZ->IsRdy() || !m_pMtX->IsRdy())
		return;
	if(!g_pIndex[nIdx]->m_pMtX->IsRdy() || !g_pIndex[nIdx]->m_pMtT->IsRdy())
		return;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	if (g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);

	_char cLotId[_MAX_CHAR_SIZE_];
	mbstowcs(cLotId, g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + nIdx].lotID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INDEX01 + nIdx].lotID) + 1);

	_char cIndexId[_MAX_CHAR_SIZE_];
	_sprintf(cIndexId, L"INDEX%d", (nIdx + 1));

	_char cEventId[_MAX_CHAR_SIZE_];

	switch(m_fsm.Get())
	{
	case C_PICKUP_START:
		if(m_fsm.Once())
		{
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cOutPnp.deviceId, L"MOVE_PCB_PICKUP", g_data2c.cEtc.start, cMaterialId, cMaterialType, cIndexId, L"OUTPNP"))

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemOutPnpPcbPickupStart) = STATE_REQ;
		}
		else
		{
			// 초기동작
			if(!IsCylScrapFixUD(pmUP, PICKUP, TRUE))
			{
				SetCylScrapFixUD(pmUP, PICKUP, TRUE);
				break;
			}

			if(pmUP != m_pCylScrapUD->GetPos(300))
			{
				m_pCylScrapUD->Actuate(pmUP);

				if(!g_logChk.bFunction[m_pCylScrapUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylScrapUD->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_UP", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylScrapUD->GetNo()])
				{
					g_logChk.bFunction[m_pCylScrapUD->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_UP", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.off))
				}
			}
			
			if(!IsCylScrapOC(pmOPEN))
			{
				SetCylScrapOC(pmOPEN);

				if(!g_logChk.bFunction[m_pCylScrapOC_F->GetNo()])
				{
					g_logChk.bFunction[m_pCylScrapOC_F->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_F_OPEN", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_F->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.on))
				}
				if(!g_logChk.bFunction[m_pCylScrapOC_R->GetNo()])
				{
					g_logChk.bFunction[m_pCylScrapOC_R->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_R_OPEN", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_R->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.on))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylScrapOC_F->GetNo()])
				{
					g_logChk.bFunction[m_pCylScrapOC_F->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_F_OPEN", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_F->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.off))
				}
				if(g_logChk.bFunction[m_pCylScrapOC_R->GetNo()])
				{
					g_logChk.bFunction[m_pCylScrapOC_R->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_R_OPEN", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_R->GetNo()], g_data2c.cEtc.delayTime, L"300", 
														g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.off))
				}
			}
			
			if(!m_pMtZ->InPos(PZ_READY))
			{
				m_pMtZ->Move(PZ_READY);
				if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_READY", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cOutPnp.Z[PZ_READY][_POSIDX_], g_data2c.cOutPnp.Z[PZ_READY][_POS_], 
														g_data2c.cOutPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_READY][_SPD_], 
														g_data2c.cOutPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_READY][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_READY", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cOutPnp.Z[PZ_READY][_POSIDX_], g_data2c.cOutPnp.Z[PZ_READY][_POS_], 
														g_data2c.cOutPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_READY][_SPD_], 
														g_data2c.cOutPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_READY][_ACC_]))
				}
			}

			if(!m_pMtX->InPos(PX_READY))
			{
				m_pMtX->Move(PX_READY);
				if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_X_READY", g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
						g_data2c.cOutPnp.X[PX_READY][_POSIDX_], g_data2c.cOutPnp.X[PX_READY][_POS_], 
						g_data2c.cOutPnp.X[PX_READY][_SPDIDX_], g_data2c.cOutPnp.X[PX_READY][_SPD_], 
						g_data2c.cOutPnp.X[PX_READY][_ACCIDX_], g_data2c.cOutPnp.X[PX_READY][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_X_READY", g_data2c.cEtc.end, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
						g_data2c.cOutPnp.X[PX_READY][_POSIDX_], g_data2c.cOutPnp.X[PX_READY][_POS_], 
						g_data2c.cOutPnp.X[PX_READY][_SPDIDX_], g_data2c.cOutPnp.X[PX_READY][_SPD_], 
						g_data2c.cOutPnp.X[PX_READY][_ACCIDX_], g_data2c.cOutPnp.X[PX_READY][_ACC_]))
				}
			}

			if(!IsReadyMtOutPnpYPickUp(nIdx))
				MoveMtOutPnpYPickUp(nIdx);
			else
			{
				m_nPickUpRetryFirst = 0;
				m_nPickUpRetry = 0;
				m_fsm.Set(C_PICKUP_01);
			}
		}
		break;
	case C_PICKUP_01:
		{
			int nXAxisNo = g_pIndex[nIdx]->m_pMtX->m_config.axisNo;
			int nXIndexNo = CIndex::PX_OUT_PNP;
			int nTAxisNo = g_pIndex[nIdx]->m_pMtT->m_config.axisNo;
			int nTIndexNo = CIndex::PT_OUT_PNP;

			if(!g_pIndex[nIdx]->IsReadyMtIndexXOutPnp() || !g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_OUT_PNP, 1000))
			{
				g_pIndex[nIdx]->MoveMtIndexXOutPnp();
				g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_OUT_PNP);

				if(!g_logChk.bTransfer[nXAxisNo][nXIndexNo])
				{
					g_logChk.bTransfer[nXAxisNo][nXIndexNo] = TRUE;
					_sprintf(cEventId, L"MT_INDEX_X_%02d_OUT_PNP", (nIdx + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[nXAxisNo], 
														g_data2c.cIndex[nIdx].X[nXIndexNo][_POSIDX_], g_data2c.cIndex[nIdx].X[nXIndexNo][_POS_], 
														g_data2c.cIndex[nIdx].X[nXIndexNo][_SPDIDX_], g_data2c.cIndex[nIdx].X[nXIndexNo][_SPD_], 
														g_data2c.cIndex[nIdx].X[nXIndexNo][_ACCIDX_], g_data2c.cIndex[nIdx].X[nXIndexNo][_ACC_]))
				}
				if(!g_logChk.bTransfer[nTAxisNo][nTIndexNo])
				{
					g_logChk.bTransfer[nTAxisNo][nTIndexNo] = TRUE;
					_sprintf(cEventId, L"MT_INDEX_T_%02d_OUT_PNP", (nIdx + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[nTAxisNo], 
														g_data2c.cIndex[nIdx].T[nTIndexNo][_POSIDX_], g_data2c.cIndex[nIdx].T[nTIndexNo][_POS_], 
														g_data2c.cIndex[nIdx].T[nTIndexNo][_SPDIDX_], g_data2c.cIndex[nIdx].T[nTIndexNo][_SPD_], 
														g_data2c.cIndex[nIdx].T[nTIndexNo][_ACCIDX_], g_data2c.cIndex[nIdx].T[nTIndexNo][_ACC_]))
				}
			}
			else
			{
				if(g_logChk.bTransfer[nXAxisNo][nXIndexNo])
				{
					g_logChk.bTransfer[nXAxisNo][nXIndexNo] = FALSE;
					_sprintf(cEventId, L"MT_INDEX_X_%02d_OUT_PNP", (nIdx + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[nXAxisNo], 
														g_data2c.cIndex[nIdx].X[nXIndexNo][_POSIDX_], g_data2c.cIndex[nIdx].X[nXIndexNo][_POS_], 
														g_data2c.cIndex[nIdx].X[nXIndexNo][_SPDIDX_], g_data2c.cIndex[nIdx].X[nXIndexNo][_SPD_], 
														g_data2c.cIndex[nIdx].X[nXIndexNo][_ACCIDX_], g_data2c.cIndex[nIdx].X[nXIndexNo][_ACC_]))
				}
				if(g_logChk.bTransfer[nTAxisNo][nTIndexNo])
				{
					g_logChk.bTransfer[nTAxisNo][nTIndexNo] = FALSE;
					_sprintf(cEventId, L"MT_INDEX_T_%02d_OUT_PNP", (nIdx + 1));
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, cEventId, g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[nTAxisNo], 
														g_data2c.cIndex[nIdx].T[nTIndexNo][_POSIDX_], g_data2c.cIndex[nIdx].T[nTIndexNo][_POS_], 
														g_data2c.cIndex[nIdx].T[nTIndexNo][_SPDIDX_], g_data2c.cIndex[nIdx].T[nTIndexNo][_SPD_], 
														g_data2c.cIndex[nIdx].T[nTIndexNo][_ACCIDX_], g_data2c.cIndex[nIdx].T[nTIndexNo][_ACC_]))
				}

				if(pmCLOSE != g_pIndex[nIdx]->m_pCylDustShutterOC->GetPos(200))
					g_pIndex[nIdx]->m_pCylDustShutterOC->Actuate(pmCLOSE);
				else if(g_pIndex[nIdx]->CylIndexMaskFixAct(pmOPEN))
					m_fsm.Set(C_PICKUP_02);
			}
		}
		break;
	case C_PICKUP_02:
		if(m_fsm.Once())
		{
			MoveMtOutPnpZOverride(PZ_PICKUP_01 + nIdx);
		}
		else
		{
			if(m_fsm.Delay(7000))
			{
				m_pMtZ->Move(PZ_READY);
				m_fsm.Set(C_ERROR, ER_CYL_OUTPNP_SCRAP_DW_INDEX_1 + nIdx);
				break;
			}
			
			if(g_pIndex[nIdx]->ExistScrap())
			{
				//Down을 먼저 해서 H/W 걸림을 먼저 확인
				if(pmDOWN != m_pCylScrapUD->GetPos(100))
				{
					m_pCylScrapUD->Actuate(pmDOWN);

					if(!g_logChk.bFunction[m_pCylScrapUD->GetNo()])
					{
						g_logChk.bFunction[m_pCylScrapUD->GetNo()] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_DOWN", g_data2c.cEtc.start, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapUD->GetNo()], g_data2c.cEtc.delayTime, L"200", 
															g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.on, 
															g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.off))
					}
					break;
				}
				else
				{
					if(g_logChk.bFunction[m_pCylScrapUD->GetNo()])
					{
						g_logChk.bFunction[m_pCylScrapUD->GetNo()] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_DOWN", g_data2c.cEtc.end, 
															cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapUD->GetNo()], g_data2c.cEtc.delayTime, L"200", 
															g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmDOWN][pmOFF], g_data2c.cEtc.off, 
															g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmDOWN][pmON], g_data2c.cEtc.on))
					}
				}

				AnalogVac(TRUE);
				m_pVac->Actuate(pmON); // ON
				//m_pVacEject->Actuate(pmOFF);

				g_logChk.bFunction[m_pVac->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_ON", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVac->GetNo()], g_data2c.cEtc.delayTime, L"200", 
													g_data2c.cPmIO[m_pVac->GetNo()].In[pmON][pmON], L"'OFF'", 
													g_data2c.cPmIO[m_pVac->GetNo()].In[pmON][pmOFF], L"'ON'"))

				g_logChk.bFunction[m_pVacEject->GetNo()] = TRUE; 
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_EJECT_OFF", g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVacEject->GetNo()], L"'RUN'", L"'OFF'"))

				m_fsm.Set(C_PICKUP_03);
			}
			else
			{
				AnalogVac(TRUE);
				m_pVac->Actuate(pmON); // ON
				// Scrap이 없으면 Down 하지 않음
				// m_pVacEject->Actuate(pmOFF);
				m_fsm.Set(C_PICKUP_03);
			}
		}
		break;
	case C_PICKUP_03:
		if(m_fsm.Delay(10000))
		{
			//m_fsm.Set(C_PICKUP_RETRY_1); 
			//m_fsm.Set(C_ERROR, ER_OUTPNP_VAC_NOT_ON_INDEX_1 + nIdx);
			m_nPickUpErrNo = (ER_OUTPNP_VAC_NOT_ON_INDEX_1 + nIdx);
			m_fsm.Set(C_PICKUP_ERR);
			break;
		}

		if(g_opr.isDryRun)
		{
			if(m_fsm.Delay(1000))
			{
				m_fsm.Set(C_PICKUP_04);
			}
			break;
		}

		if(pmON == m_pVac->GetPos(750))// && pmOFF == m_pVacEject->GetPos(750))
		{
			if(g_logChk.bFunction[m_pVac->GetNo()])
			{
				g_logChk.bFunction[m_pVac->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_ON", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVac->GetNo()], g_data2c.cEtc.delayTime, L"200", 
													g_data2c.cPmIO[m_pVac->GetNo()].In[pmON][pmON], L"'ON'", 
													g_data2c.cPmIO[m_pVac->GetNo()].In[pmON][pmOFF], L"'OFF'"))
			}
			if(g_logChk.bFunction[m_pVacEject->GetNo()])
			{
				g_logChk.bFunction[m_pVacEject->GetNo()] = FALSE;
				//NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_EJECT_OFF", g_data2c.cEtc.end, 
				//									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVacEject->GetNo()], g_data2c.cEtc.delayTime, L"200", 
				//									g_data2c.cPmIO[m_pVacEject->GetNo()].In[pmOFF][pmOFF], L"'OFF'", 
				//									g_data2c.cPmIO[m_pVacEject->GetNo()].In[pmOFF][pmON], L"'ON'"))
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_EJECT_OFF", g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVacEject->GetNo()], L"'RUN'", L"'OFF'"))
			}
			m_fsm.Set(C_PICKUP_04);
		}
		else
		{
			if(m_fsm.Delay(4000))
			{
				if(m_fsm.Once())
				{
					int nZPos = PZ_PICKUP_01 + nIdx;
					double dOffset = g_pNV->Pkg(offsetOutPnpZPickUp1 + nIdx) * 1000.0;	
					double dPos = m_pMtZ->m_pTable->pos[nZPos] + dOffset + (0.1 * 1000.0); // 0.1 mm Retry
					m_pMtZ->PMove(nZPos, dPos);
				}
			}
		}
		break;
	case C_PICKUP_RETRY_1:
		if(0 == m_fsm.GetStep())
		{
			// Z Down 상태.
			m_pVac->Actuate(pmOFF); // OFF
			//m_pVacEject->Actuate(pmON); //

			if(pmOFF == m_pVac->GetPos(500))// && pmON == m_pVacEject->GetPos(500)) //
			{
				//m_pVacEject->Actuate(pmOFF); //
				m_fsm.RstDelay();
				m_fsm.SetStep(1);
			}
		}
		else if(1 == m_fsm.GetStep())
		{
			m_pVac->Actuate(pmOFF);// ON
			//m_pVacEject->Actuate(pmOFF);//
			if(pmON != m_pVac->GetPos(500))// || pmOFF != m_pVacEject->GetPos(500))//
				break;//

			m_nPickUpRetryFirst++;

			int OutPnpRetry = (int)g_pNV->DDm(OutPnpRetryCnt);
			if(OutPnpRetry < m_nPickUpRetryFirst)
			{
				// Scrap Clamp을 하지 않은 상태
				m_pCylScrapUD->Actuate(pmUP);
				m_pMtZ->Move(PZ_READY);
				m_fsm.Set(C_ERROR, ER_OUTPNP_VAC_NOT_ON_INDEX_1 + nIdx);
			}
			else
			{
				m_fsm.RstDelay();
				m_fsm.SetStep(2);
			}
		}
		else if(2 == m_fsm.GetStep())
		{
			m_pMtZ->Move(PZ_PICKUP_RETRY);
			m_fsm.RstDelay();
			m_fsm.SetStep(3);
		}
		else if(3 == m_fsm.GetStep())
		{
			if(!m_fsm.Delay(500))
				break;

			MoveMtOutPnpZPickUp(nIdx);
			m_fsm.RstDelay();
			m_fsm.SetStep(4);
		}
		else if(4 == m_fsm.GetStep())
		{
			if(m_fsm.Delay(5000))
			{
				m_fsm.RstDelay();
				m_fsm.SetStep(0);
				break;
			}

			m_pVac->Actuate(pmON); // OFF
			AnalogVac(TRUE);
			
			//m_pVacEject->Actuate(pmOFF);//
			if(pmON == m_pVac->GetPos(750))// && pmOFF == m_pVacEject->GetPos(750))//
			{
				m_fsm.Set(C_PICKUP_04);
			}
		}
		break;
	case C_PICKUP_04:
		if(!g_pIndex[nIdx]->ExistScrap())
		{
			m_fsm.Set(C_PICKUP_05);
			break;
		}

		if(m_fsm.Delay(7000))
		{
			m_nPickUpErrNo = (ER_CYL_OUTPNP_SCRAP_CLOSE_INDEX_1 + nIdx);
			m_fsm.Set(C_PICKUP_ERR);
			break;
		}

		if(!IsCylScrapOC(pmCLOSE))
		{
			SetCylScrapOC(pmCLOSE);

			if(!g_logChk.bFunction[m_pCylScrapOC_F->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_F->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_F_CLOSE", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_F->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.off))
			}
			if(!g_logChk.bFunction[m_pCylScrapOC_R->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_R->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_R_CLOSE", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_R->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.off))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylScrapOC_F->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_F->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_F_CLOSE", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_F->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.on))
			}
			if(g_logChk.bFunction[m_pCylScrapOC_R->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_R->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_R_CLOSE", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_R->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.on))
			}
		}

		m_fsm.Set(C_PICKUP_05);
		break;
	case C_PICKUP_05:
		if(m_fsm.Once())
		{
			m_pMtZ->Move(PZ_SCRAP_FIX_DW);
			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_SCRAP_FIX_DW])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_SCRAP_FIX_DW] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_SCRAP_FIX_DW", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_POSIDX_], g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_POS_], 
													g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_SPD_], 
													g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_ACC_]))
			}
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_SCRAP_FIX_DW])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_SCRAP_FIX_DW] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_SCRAP_FIX_DW", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_POSIDX_], g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_POS_], 
													g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_SPD_], 
													g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_SCRAP_FIX_DW][_ACC_]))
			}

			if(!g_pIndex[nIdx]->ExistScrap())
			{
				m_fsm.Set(C_PICKUP_06);
				break;
			}

			if(m_fsm.Delay(7000))
			{
				m_nPickUpErrNo = (ER_CYL_OUTPNP_SCRAP_FIX_DW_INDEX_1 + nIdx);
				m_fsm.Set(C_PICKUP_ERR);
				break;
			}

			// (추가부분) 선 다운 후 에러 체크 +스크랩 미리 업 
			// 담당자 전달 후 재 컨셉 협의
			SetCylScrapFixUD(pmDOWN, PICKUP, TRUE);
			if(pmUP != m_pCylScrapUD->GetPos(500)) 
				m_pCylScrapUD->Actuate(pmUP);

			if(!IsCylScrapFixUD(pmDOWN, PICKUP, TRUE))
				SetCylScrapFixUD(pmDOWN, PICKUP, TRUE);
			else
				m_fsm.Set(C_PICKUP_06);
		}
		break;
	case C_PICKUP_06:
		if(0 == m_fsm.GetStep())
		{
			if(!m_fsm.Delay(500 + g_pNV->NDm(outPnpScrapExisDelayTime)))    // scrap exist x1800 센서 튀는 현상으로 인해 딜레이 추가
				break;
			m_fsm.SetStep(1);
		}
		if(1 == m_fsm.GetStep())
		{
			if(!g_pIndex[nIdx]->ExistScrap())
			{
				if(pmOFF != m_pVac->GetPos(500) && !g_opr.isDryRun)
				{
					//m_fsm.Set(C_PICKUP_RETRY_2); 
					//m_fsm.Set(C_ERROR, ER_OUTPNP_VAC_NOT_ON_INDEX_1 + nIdx);
					m_nPickUpErrNo = (ER_OUTPNP_VAC_NOT_ON_INDEX_1 + nIdx);
					m_fsm.Set(C_PICKUP_ERR);
				}
				else 
					m_fsm.Set(C_PICKUP_END);
			}
			else
			{
				if(m_fsm.Delay(7000))
				{
					m_nPickUpErrNo = (ER_CYL_OUTPNP_SCRAP_UP_INDEX_1 + nIdx);
					m_fsm.Set(C_PICKUP_ERR);
					break;
				}

				// 임시 조치 2021.12.07 해제 필수 ***************
				if(!g_dIn.AOn(iOutPnpScrapExist) && !g_opr.isDryRun)
				{
					m_nPickUpErrNo = (ER_OUTPNP_SCRAP_NOT_EXIST_INDEX_1 + nIdx);
					m_fsm.Set(C_PICKUP_ERR);
					break;
				}

				if(pmON != m_pVac->GetPos(500))// && !g_opr.isDryRun)
				{
					//m_fsm.Set(C_PICKUP_RETRY_2); 
					//m_fsm.Set(C_ERROR, ER_OUTPNP_VAC_NOT_ON_INDEX_1 + nIdx);
					m_nPickUpErrNo = (ER_OUTPNP_VAC_NOT_ON_INDEX_1 + nIdx);
					m_fsm.Set(C_PICKUP_ERR);
					break;
				}

				if(pmUP != m_pCylScrapUD->GetPos(300))
				{
					m_pCylScrapUD->Actuate(pmUP);

					if(!g_logChk.bFunction[m_pCylScrapUD->GetNo()])
					{
						g_logChk.bFunction[m_pCylScrapUD->GetNo()] = TRUE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_UP", g_data2c.cEtc.start, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
							g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
							g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
					}
					break;
				}
				else
				{
					if(g_logChk.bFunction[m_pCylScrapUD->GetNo()])
					{
						g_logChk.bFunction[m_pCylScrapUD->GetNo()] = FALSE;
						NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_UP", g_data2c.cEtc.end, 
							cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
							g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.on, 
							g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.off))
					}			
				}

				m_fsm.Set(C_PICKUP_END);
			}
		}
		break;
	case C_PICKUP_ERR:
		{
			if(!g_pIndex[nIdx]->ExistScrap())
			{
				// 고객 요청 제외
				//	if(!IsReadyMtOutPnpZPickUp(nIdx))
				//		MoveMtOutPnpZPickUp(nIdx); // Error 시에는 Pos가 어딘지 모르므로 Override 적용하지 않음
				//	else if(pmOFF != m_pVac->GetPos(500))
				//		m_pVac->Actuate(pmOFF);
				//	else if(pmOFF != m_pVacEject->GetPos(500))
				//		m_pVacEject->Actuate(pmOFF);
				//	else 
				//	{
				m_pMtZ->Move(PZ_READY);
				m_fsm.Set(C_ERROR, m_nPickUpErrNo);
				//	}
			}
			else
			{
				// 고객 요청 제외
				//if(pmDOWN != m_pCylScrapUD->GetPos(500))
				//	m_pCylScrapUD->Actuate(pmDOWN);
				//else if(!IsReadyMtOutPnpZPickUp(nIdx)) 
				//	MoveMtOutPnpZPickUp(nIdx); // Error 시에는 Pos가 어딘지 모르므로 Override 적용하지 않음
				//else if(pmOFF != m_pVac->GetPos(500))
				//	m_pVac->Actuate(pmOFF);
				//else if(pmOFF != m_pVacEject->GetPos(500))
				//	m_pVacEject->Actuate(pmOFF);
				//else if(!IsCylScrapFixUD(pmUP))
				//	SetCylScrapFixUD(pmUP);
				//else if(!IsCylScrapOC(pmOPEN))
				//	SetCylScrapOC(pmOPEN);
				//else 
				//{
				m_pMtZ->Move(PZ_READY);
				//m_pCylScrapUD->Actuate(pmUP);
				m_fsm.Set(C_ERROR, m_nPickUpErrNo);
				//}
			}
		}
		break;
	case C_PICKUP_RETRY_2:
		{
			if(0 == m_fsm.GetStep())
			{
				if(!IsReadyMtOutPnpZPickUp(nIdx)) 
					MoveMtOutPnpZPickUp(nIdx); // Error 시에는 Pos가 어딘지 모르므로 Override 적용하지 않음
				else
				{
					m_fsm.RstDelay();
					m_fsm.SetStep(1);
				}
			}
			else if(1 == m_fsm.GetStep())
			{
				m_pVac->Actuate(pmOFF);// ON
				//m_pVacEject->Actuate(pmON);//
				if(pmOFF == m_pVac->GetPos(500))// && pmON == m_pVacEject->GetPos(500))//
				{
					//m_pVacEject->Actuate(pmOFF);
					m_fsm.RstDelay();
					m_fsm.SetStep(2);
				}
			}
			else if(2 == m_fsm.GetStep())
			{
				// 일정시간 Delay 후 Vac On 확인
				if(!m_fsm.Delay(500))
					break;

				m_pMtZ->Move(PZ_SCRAP_FIX_DW);
				m_fsm.RstDelay();
				m_fsm.SetStep(3);
			}
			else if(3 == m_fsm.GetStep())
			{
				MoveMtOutPnpZPickUp(nIdx); // Error 시에는 Pos가 어딘지 모르므로 Override 적용하지 않음
				m_fsm.RstDelay();
				m_fsm.SetStep(4);
			}
			else if(4 == m_fsm.GetStep())
			{
				if(m_fsm.Delay(5000))
				{
					m_nPickUpErrNo = (ER_OUTPNP_VAC_NOT_ON_INDEX_1 + nIdx);
					m_fsm.Set(C_PICKUP_ERR);
					break;
				}

				m_pVac->Actuate(pmON); // OFF
				AnalogVac(TRUE);

				if(pmON != m_pVac->GetPos(500))
					break;

				m_fsm.RstDelay();
				m_fsm.SetStep(5);
			}
			else if(5 == m_fsm.GetStep())
			{
				m_pMtZ->Move(PZ_SCRAP_FIX_DW);
				m_fsm.RstDelay();
				m_fsm.SetStep(6);
			}
			else if(6 == m_fsm.GetStep())
			{
				// Delay 가 없어서 추가함
				if(!m_fsm.Delay(1000))
					break;
				
				m_nPickUpRetry++;

				int OutPnpRetry = (int)g_pNV->DDm(OutPnpRetryCnt);
				if(pmON != m_pVac->GetPos(1000))
				{
					if(OutPnpRetry <= m_nPickUpRetry)
					{
						m_nPickUpErrNo = (ER_OUTPNP_VAC_NOT_ON_INDEX_1 + nIdx);
						m_fsm.Set(C_PICKUP_ERR);
					}
					else
					{
						m_fsm.RstDelay();
						m_fsm.SetStep(0);
					}
				}
				else 
					m_fsm.Set(C_PICKUP_06);
			}
		}
		break;
	case C_PICKUP_END:
		if(m_fsm.Once())
		{
			m_pMtZ->Move(PZ_READY);

			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_READY", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cOutPnp.Z[PZ_READY][_POSIDX_], g_data2c.cOutPnp.Z[PZ_READY][_POS_], 
													g_data2c.cOutPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cOutPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_READY][_ACC_]))
			}
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_READY", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cOutPnp.Z[PZ_READY][_POSIDX_], g_data2c.cOutPnp.Z[PZ_READY][_POS_], 
													g_data2c.cOutPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cOutPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_READY][_ACC_]))
			}

			// 임시 조치 2021.12.07 해제 필수 ***************
			if(g_pIndex[nIdx]->ExistScrap())
			{
				if(!g_dIn.AOn(iOutPnpScrapExist) && !g_opr.isDryRun)
				{
					m_nPickUpErrNo = ER_OUTPNP_SCRAP_EXIST;
					m_fsm.Set(C_PICKUP_ERR);
					break;
				}
			}

			// 체크 위치 변경 부분
			if(m_fsm.Delay(7000))
			{
				m_nPickUpErrNo = (ER_CYL_OUTPNP_SCRAP_FIX_DW_INDEX_1 + nIdx);
				m_fsm.Set(C_PICKUP_ERR);
				break;
			}
			
			//if((fabs(g_pNV->DDm(srcOutPnpAir)) < 10) && !g_opr.isDryRun)
			if(pmON != m_pVac->GetPos(500))// && !g_opr.isDryRun)
			{
				m_nPickUpErrNo = (ER_OUTPNP_VAC_NOT_ON_INDEX_1 + nIdx);
				m_fsm.Set(C_PICKUP_ERR);
				break;
			}

			if(g_pIndex[nIdx]->ExistScrap())
				ExistScrap() = TRUE;
			
			ExistPcb()   = TRUE;
			g_pIndex[nIdx]->ExistScrap() = FALSE;
			g_pIndex[nIdx]->ExistPcb()   = FALSE;
			g_pIndex[nIdx]->m_pMem->compOutPnp = TRUE;
			g_pNV->NDm(outPnpPcbIndex) = nIdx;

			g_pNV->DDm(ActOutPnpPadLifeCount)++; 

			if(g_pNV->UseSkip(usTcServer))
			{
				g_lotInfo.LotInfoCopy(LOT_INFO_INDEX01 + nIdx, LOT_INFO_OUTPNP);
				g_lotInfo.LotInfoClear(LOT_INFO_INDEX01 + nIdx);
			}

			// 설비가 Error 발생시 Qc Cycle Error 발생함으로 Cycle에서 Index Ready위치로 이동시킴
			g_pIndex[nIdx]->MoveMtIndexXInPnp();
			g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_IN_PNP);

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemOutPnpPcbPickupEnd) = STATE_REQ;

			if (g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode[0] == L'\0')
				_sprintf(cMaterialId, L"$");
			else
				mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);
			_sprintf(cLotId, L"%s", g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].lotID);
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cOutPnp.deviceId, L"MOVE_PCB_PICKUP", g_data2c.cEtc.end, cMaterialId, cMaterialType, cIndexId, L"OUTPNP"))
			m_fsm.Set(C_IDLE);
		}
		break;
	}
}

//-------------------------------------------------------------------
void COutPnp::CycleRunPcbPutDn(void)
{
	if(!m_fsm.Between(C_PUTDN_START, C_PUTDN_END))
		return;

	//if(m_fsm.TimeLimit(300000))
	//{
	//	m_fsm.Set(C_ERROR, ER_OUTPNP_VGRIP_PUTDN_CYCLE_TM_OVER);
	//	return;
	//}

	int nIdx = m_fsm.GetMsg();

	g_dOut.On(oSorterPickerReq);
	g_dOut.On(oSorterPickerBusy);

	_char cMaterialIdOutPnp[_MAX_CHAR_SIZE_]; 
	
	if(INDEX_01 == nIdx || INDEX_02 == nIdx)
	{
		g_dOut.Off(oSorterPickerSafety0102);
		g_dOut.On(oSorterPickerSafety0304);
	}
	else
	{
		g_dOut.On(oSorterPickerSafety0102);
		g_dOut.Off(oSorterPickerSafety0304);
	}

	if(!m_pMtY->IsRdy() || !m_pMtZ->IsRdy() || !m_pMtX->IsRdy())
		return;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");	
	if (g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);
	
	_char cLotId[_MAX_CHAR_SIZE_];
	mbstowcs(cLotId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].lotID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].lotID) + 1);

	_char cIndexId[_MAX_CHAR_SIZE_];
	_sprintf(cIndexId, L"SORTER_INDEX%d", (nIdx + 1));

	switch(m_fsm.Get())
	{
	case C_PUTDN_START:
		if(m_fsm.Once())
		{
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cOutPnp.deviceId, L"MOVE_PCB_PUTDN", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"OUTPNP", cIndexId))

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemOutPnpPcbTrfToSorterStart) = STATE_REQ;
			
			double tm = m_tmOutPnpCycle.Elapsed()/1000.0;
			g_pNV->DDm(cycleTmOutPnp) = tm;
			SeqLog(L"[OutPnp Cycle Time] INDEX[%d] Out-Pnp PCB Sorter Put Down -> Put Down [%3f] (sec)", nIdx, tm);
			m_tmOutPnpCycle.Reset();
			g_pNV->NDm(stateOutPnpInfo) = STATE_IDLE;
			g_pNV->NDm(stateLotInfoLog) = STATE_IDLE;
			
			m_nRetryPcbInfo = 0;
		}
		else
		{
			if(!m_pMtZ->InPos(PZ_READY))
			{
				m_pMtZ->Move(PZ_READY);

				if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_READY", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cOutPnp.Z[PZ_READY][_POSIDX_], g_data2c.cOutPnp.Z[PZ_READY][_POS_],
														g_data2c.cOutPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_READY][_SPD_], 
														g_data2c.cOutPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_READY][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
				{
					g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_READY", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
														g_data2c.cOutPnp.Z[PZ_READY][_POSIDX_], g_data2c.cOutPnp.Z[PZ_READY][_POS_],
														g_data2c.cOutPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_READY][_SPD_], 
														g_data2c.cOutPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_READY][_ACC_]))				}
			}
			
			if(!m_pMtX->InPos(PX_READY))
			{
				m_pMtX->Move(PX_READY);

				if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_X_READY", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
														g_data2c.cOutPnp.X[PX_READY][_POSIDX_], g_data2c.cOutPnp.X[PX_READY][_POS_], 
														g_data2c.cOutPnp.X[PX_READY][_SPDIDX_], g_data2c.cOutPnp.X[PX_READY][_SPD_], 
														g_data2c.cOutPnp.X[PX_READY][_ACCIDX_], g_data2c.cOutPnp.X[PX_READY][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY])
				{
					g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_X_READY", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
														g_data2c.cOutPnp.X[PX_READY][_POSIDX_], g_data2c.cOutPnp.X[PX_READY][_POS_], 
														g_data2c.cOutPnp.X[PX_READY][_SPDIDX_], g_data2c.cOutPnp.X[PX_READY][_SPD_], 
														g_data2c.cOutPnp.X[PX_READY][_ACCIDX_], g_data2c.cOutPnp.X[PX_READY][_ACC_]))
				}
			}

			if(!IsReadyMtOutPnpYPutDn(nIdx))
			{
				MoveMtOutPnpYPutDn(nIdx);
				break;
			}			
				
			if(g_pNV->UseSkip(usTcServer))
				m_fsm.Set(C_PUTDN_01);
			else
				m_fsm.Set(C_PUTDN_03);
		}
		break;
	case C_PUTDN_01:
		if(m_fsm.Once())
		{
			if(!g_pNV->UseSkip(usArts))
			{
				int nMaxUnitCnt = (int)g_pNV->Pkg(unitCnt);

				for(int i = 0; i < nMaxUnitCnt; i++)
				{
					g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbTestResult[i]='1';
				}
			}
			g_pNV->NDm(stateOutPnpInfo) = STATE_REQ;
						
			_sprintf(cMaterialIdOutPnp, L"\'%s\'", cMaterialId);
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"SORTER_LOT_INFO_SEND", g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'TCP'", L"'PCB_ID'", cMaterialIdOutPnp))
		}
		else
		{
			if(m_fsm.TimeLimit(7000))
			{
				g_dOut.On(oSorterError);
				m_fsm.Set(C_ERROR, ER_OUTPNP_SORTER_PCB_INFO_TM_OVER);
				break;
			}

			switch(g_pNV->NDm(stateOutPnpInfo))
			{
			case STATE_BUSY : 
				break;
			case STATE_COMP : 
				_sprintf(cMaterialIdOutPnp, L"\'%s\'", cMaterialId);
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"SORTER_LOT_INFO_SEND", g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'TCP'", L"'PCB_ID'", cMaterialIdOutPnp))
				m_fsm.Set(C_PUTDN_03);
				break;
			case STATE_ERR : 
				m_fsm.Set(C_PUTDN_02);
				break;
			}
		}
		break;
	case C_PUTDN_02:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateOutPnpInfo) = STATE_IDLE;
			m_nRetryPcbInfo++;
		}
		else 
		{
			if(m_nRetryPcbInfo < 3)
			{
				if(!m_fsm.Delay(3000))
					break;

				m_fsm.Set(C_PUTDN_01);
			}
			else 
			{
				g_dOut.On(oSorterError);
				m_fsm.Set(C_ERROR, ER_OUTPNP_SORTER_PCB_INFO_FAIL);
			}
		}
		break;
	case C_PUTDN_03:
		{
			if(g_dIn.AOn(iSorterError))
			{
				m_fsm.Set(C_PUTDN_ERR);
				break;
			}
			
			if(!IsReadyMtOutPnpXSorter(nIdx))
				MoveMtOutPnpXSorter(nIdx);
			else if(!IsReadyMtOutPnpZOverride(PZ_PUTDN_01 + nIdx))
				MoveMtOutPnpZOverride(PZ_PUTDN_01 + nIdx);
			else if(pmOFF != m_pVac->GetPos(500) )//|| pmON != m_pVacEject->GetPos(500))
			{
				m_pVac->Actuate(pmOFF); //OFF
				
				//m_pVacEject->Actuate(pmON);
				if(!g_logChk.bFunction[m_pVac->GetNo()])
				{
					g_logChk.bFunction[m_pVac->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_OFF", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVac->GetNo()], g_data2c.cEtc.delayTime, L"200", 
														g_data2c.cPmIO[m_pVac->GetNo()].In[pmOFF][pmOFF], g_data2c.cEtc.on, 
														g_data2c.cPmIO[m_pVac->GetNo()].In[pmOFF][pmON], g_data2c.cEtc.off))
				}
				if(!g_logChk.bFunction[m_pVacEject->GetNo()])
				{
					g_logChk.bFunction[m_pVacEject->GetNo()] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_EJECT_ON", g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVacEject->GetNo()], L"'RUN'", L"'ON'"))
				}
			}
			else
			{
				if(g_logChk.bFunction[m_pVac->GetNo()])
				{
					g_logChk.bFunction[m_pVac->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_OFF", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVac->GetNo()], g_data2c.cEtc.delayTime, L"200", 
														g_data2c.cPmIO[m_pVac->GetNo()].In[pmOFF][pmOFF], g_data2c.cEtc.off, 
														g_data2c.cPmIO[m_pVac->GetNo()].In[pmOFF][pmON], g_data2c.cEtc.on))
				}
				if(g_logChk.bFunction[m_pVacEject->GetNo()])
				{
					g_logChk.bFunction[m_pVacEject->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_EJECT_ON", g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVacEject->GetNo()], L"'RUN'", L"'ON'"))
				}


				g_logChk.bFunction[m_pVacEject->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_EJECT_OFF_INIT", g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVacEject->GetNo()], L"'RUN'", L"'OFF'"))

				ExistPcb() = FALSE;
				m_fsm.Set(C_PUTDN_04);
			}
		}
		break;
	case C_PUTDN_04:
		if(g_logChk.bFunction[m_pVacEject->GetNo()])
		{
			g_logChk.bFunction[m_pVacEject->GetNo()] = FALSE;
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"VAC_OUTPNP_EJECT_OFF_INIT", g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.vac, g_data2c.cEtc.actName, g_data2c.cPmName[m_pVacEject->GetNo()], L"'RUN'", L"'OFF'"))
		}
		if(!m_fsm.Delay(ULONG(g_pNV->DDm(OutPnPPlaceTime) * 1000)))
			break;

		if(!m_pMtZ->InPos(PZ_READY))
		{
			m_pMtZ->Move(PZ_READY);

			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_READY_INIT", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cOutPnp.Z[PZ_READY][_POSIDX_], g_data2c.cOutPnp.Z[PZ_READY][_POS_], 
													g_data2c.cOutPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cOutPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_READY][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_READY_INIT", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cOutPnp.Z[PZ_READY][_POSIDX_], g_data2c.cOutPnp.Z[PZ_READY][_POS_], 
													g_data2c.cOutPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cOutPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_READY][_ACC_]))
			}
		}

		if(!m_pMtX->InPos(PX_READY))
		{
			m_pMtX->Move(PX_READY);

			if(!g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY])
			{
				g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_X_READY", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
													g_data2c.cOutPnp.X[PX_READY][_POSIDX_], g_data2c.cOutPnp.X[PX_READY][_POS_], 
													g_data2c.cOutPnp.X[PX_READY][_SPDIDX_], g_data2c.cOutPnp.X[PX_READY][_SPD_], 
													g_data2c.cOutPnp.X[PX_READY][_ACCIDX_], g_data2c.cOutPnp.X[PX_READY][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY])
			{
				g_logChk.bTransfer[m_pMtX->m_config.axisNo][PX_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_X_READY", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtX->m_config.axisNo], 
													g_data2c.cOutPnp.X[PX_READY][_POSIDX_], g_data2c.cOutPnp.X[PX_READY][_POS_], 
													g_data2c.cOutPnp.X[PX_READY][_SPDIDX_], g_data2c.cOutPnp.X[PX_READY][_SPD_], 
													g_data2c.cOutPnp.X[PX_READY][_ACCIDX_], g_data2c.cOutPnp.X[PX_READY][_ACC_]))
			}
		}

		// Merge Lot id 비교 후에 마지막 Lot Clear
		if(g_pNV->UseSkip(usTcServer))
		{
			// Lot End는 Sorter에서 진행 하므로 Router에서는 정리만 한다.
			if(0 != _stricmp(g_pNV->m_pLotInfo->history[0].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].mergeLotID))
				m_fsm.Set(C_PUTDN_05); // 다르면 ..
			else
				m_fsm.Set(C_PUTDN_END); // 같으면 ..
		}
		else
			m_fsm.Set(C_PUTDN_END);
		break;

	case C_PUTDN_ERR:
		// Error 시에 Zup + X Ready후 Alarm.
		if(!m_pMtZ->InPos(PZ_READY))
			m_pMtZ->Move(PZ_READY);
		else if(!m_pMtX->InPos(PX_READY))
			m_pMtX->Move(PX_READY);
		else 
			m_fsm.Set(C_ERROR, ER_SORTER_ERROR);

		break;

	case C_PUTDN_05:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateLotInfoLog) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(5000))
			{
				g_pNV->NDm(stateLotInfoLog) = STATE_IDLE;
				g_lotInfo.LotFirstHistoryClear();
				g_lotInfo.LotHistorySort();
				m_fsm.Set(C_PUTDN_END);
				break;
			}

			switch (g_pNV->NDm(stateLotInfoLog))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				g_lotInfo.LotFirstHistoryClear();
				g_lotInfo.LotHistorySort();
				m_fsm.Set(C_PUTDN_END);
				break;
			case STATE_ERR:
				g_lotInfo.LotFirstHistoryClear();
				g_lotInfo.LotHistorySort();
				m_fsm.Set(C_PUTDN_END);
				break;
			}
		}
		break;

	case C_PUTDN_END:
		if(g_pNV->UseSkip(usTcServer))
		{
			g_lotInfo.LotInfoCopy(LOT_INFO_OUTPNP, LOT_INFO_OLD_OUTPNP);
			g_lotInfo.LotInfoClear(LOT_INFO_OUTPNP);
		}
		if(g_pNV->UseSkip(usSecsGem))
			g_pNV->NDm(gemOutPnpPcbTrfToSorterEnd) = STATE_REQ;
		
		if(g_pNV->UseSkip(usSecsGem))
			g_pNV->NDm(gemTrackOut) = STATE_REQ;
		
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cOutPnp.deviceId, L"MOVE_PCB_PUTDN", g_data2c.cEtc.end, L"$", L"$", L"OUTPNP", cIndexId))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void COutPnp::CycleRunScrapEject(void)
{
	if(!m_fsm.Between(C_SCRAP_EJECT_START, C_SCRAP_EJECT_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_OUTPNP_SCRAP_EJECT_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtY->IsRdy() || !m_pMtZ->IsRdy() || !m_pMtX->IsRdy())
		return;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");	
	if (g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode[0] == L'\0') 
		_sprintf(cMaterialId, L"$");
	else
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].pcbBarcode) + 1);

	_char cLotId[_MAX_CHAR_SIZE_];
	mbstowcs(cLotId, g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].lotID, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_OUTPNP].lotID) + 1);

	switch(m_fsm.Get())
	{
	case C_SCRAP_EJECT_START:
		if(m_fsm.Once())
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cOutPnp.deviceId, L"MOVE_SCRAP", g_data2c.cEtc.start, L"$", L"$", L"OUTPNP", L"SCRAP_BOX"))

		if(pmUP != m_pCylScrapUD->GetPos(200))
		{
			m_pCylScrapUD->Actuate(pmUP);

			if(!g_logChk.bFunction[m_pCylScrapUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapUD->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_UP", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.on))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylScrapUD->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapUD->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_UP", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapUD->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylScrapUD->GetNo()].In[pmUP][pmOFF], g_data2c.cEtc.off))			
			}
		}
				
		if(!IsCylScrapOC(pmCLOSE))
		{
			SetCylScrapOC(pmCLOSE);

			if(!g_logChk.bFunction[m_pCylScrapOC_F->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_F->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_CLOSE", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_F->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.off))
			}
			if(!g_logChk.bFunction[m_pCylScrapOC_R->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_R->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_CLOSE", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_R->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.off))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylScrapOC_F->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_F->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_CLOSE", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_F->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.on))
			}
			if(g_logChk.bFunction[m_pCylScrapOC_R->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_R->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_CLOSE", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_R->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmCLOSE][pmOFF], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmCLOSE][pmON], g_data2c.cEtc.on))
			}
		}
		
		if(!IsCylScrapFixUD(pmDOWN, PUTDOWN, TRUE))
		{
			SetCylScrapFixUD(pmDOWN, PUTDOWN, TRUE);
			break;
		}

		if(!m_pMtZ->InPos(PZ_READY))
		{
			m_pMtZ->Move(PZ_READY);
			break;
		}
		else
		{
		}
				
		if(!m_pMtX->InPos(PX_READY))
		{
			m_pMtX->Move(PX_READY);

			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_READY", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cOutPnp.Z[PZ_READY][_POSIDX_], g_data2c.cOutPnp.Z[PZ_READY][_POS_], 
													g_data2c.cOutPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cOutPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_READY][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Z_READY", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cOutPnp.Z[PZ_READY][_POSIDX_], g_data2c.cOutPnp.Z[PZ_READY][_POS_], 
													g_data2c.cOutPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cOutPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cOutPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cOutPnp.Z[PZ_READY][_ACC_]))
			}
		}
		
		if(!m_pMtY->InPos(PY_SCRAP_EJECT))
		{
			m_pMtX->Move(PY_SCRAP_EJECT);

			if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Y_SCRAP", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
													g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POSIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POS_], 
													g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPDIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPD_], 
													g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACCIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_SCRAP_EJECT] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"MT_OUT_PNP_Y_SCRAP", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
													g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POSIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_POS_], 
													g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPDIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_SPD_], 
													g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACCIDX_], g_data2c.cOutPnp.Y[PY_SCRAP_EJECT][_ACC_]))
			}
		}
		
		m_fsm.Set(C_SCRAP_EJECT_01);
		break;
	case C_SCRAP_EJECT_01:
		if(!IsCylScrapFixUD(pmUP, PUTDOWN, TRUE))
		{
			SetCylScrapFixUD(pmUP, PUTDOWN, TRUE);
			break;
		}

		if(!IsCylScrapOC(pmOPEN))
		{
			SetCylScrapOC(pmOPEN);

			if(!g_logChk.bFunction[m_pCylScrapOC_F->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_F->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_F_OPEN", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_F->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.on))
			}
			if(!g_logChk.bFunction[m_pCylScrapOC_R->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_R->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_R_OPEN", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_R->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.off, 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.on))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylScrapOC_F->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_F->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_F_OPEN", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_F->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylScrapOC_F->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.off))
			}
			if(g_logChk.bFunction[m_pCylScrapOC_R->GetNo()])
			{
				g_logChk.bFunction[m_pCylScrapOC_R->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cOutPnp.deviceId, L"CYL_OUTPNP_SCRAP_R_OPEN", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylScrapOC_R->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmOPEN][pmON], g_data2c.cEtc.on, 
													g_data2c.cPmIO[m_pCylScrapOC_R->GetNo()].In[pmOPEN][pmOFF], g_data2c.cEtc.off))
			}
		}

		ExistScrap() = FALSE;
		m_fsm.Set(C_SCRAP_EJECT_END);
		break;
	case C_SCRAP_EJECT_END:
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cOutPnp.deviceId, L"MOVE_SCRAP", g_data2c.cEtc.end, L"$", L"$", L"OUTPNP", L"SCRAP_BOX"))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void COutPnp::CycleRunAdcPickUp(void)
{
	if(!m_fsm.Between(C_ADC_PICKUP_START, C_ADC_PICKUP_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_OUTPNP_ADC_PICKUP_CYCLE_TM_OVER);
		return;
	}

	SetStageBusy(INDEX_01);

	if(!m_pMtY->IsRdy() || !m_pMtZ->IsRdy() || !m_pMtX->IsRdy())
		return;
	if(!g_pIndex[INDEX_01]->m_pMtX->IsRdy() || !g_pIndex[INDEX_01]->m_pMtT->IsRdy())
		return;

	switch(m_fsm.Get())
	{
	case C_ADC_PICKUP_START:
		// Out Pnp Kit Clamp
		if(!g_pIndex[INDEX_01]->CylIndexMaskFixAct(pmCLOSE))
			break;

		if(!IsCylScrapFixUD(pmUP))
			SetCylScrapFixUD(pmUP);
		else if(pmUP != m_pCylScrapUD->GetPos(300))
			m_pCylScrapUD->Actuate(pmUP);
		else if(!IsCylScrapOC(pmCLOSE))
			SetCylScrapOC(pmCLOSE);
		else if(!m_pMtZ->InPos(PZ_READY))
			m_pMtZ->Move(PZ_READY);
		else if(!m_pMtX->InPos(PX_READY))
			m_pMtX->Move(PX_READY);
		else if(!m_pMtY->InPos(PY_ADC_KIT_CLAMP))
			m_pMtY->Move(PY_ADC_KIT_CLAMP);
		else
			m_fsm.Set(C_ADC_PICKUP_01);
		break;
	case C_ADC_PICKUP_01:
		if(!g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_OUTPNP, 1000) || 
			!g_pIndex[INDEX_01]->m_pMtT->InPos(CIndex::PT_ADC_OUTPNP, 1000))
		{
			g_pIndex[INDEX_01]->m_pMtX->Move(CIndex::PX_ADC_OUTPNP);
			g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ADC_OUTPNP);
			break;
		}

		if(!g_pIndex[INDEX_01]->CylIndexMaskFixAct(pmCLOSE))
			break;
		
		if(pmOPEN != m_pSolKitClampOC->GetPos(300))
			m_pSolKitClampOC->Actuate(pmOPEN);
		else 
			m_fsm.Set(C_ADC_PICKUP_02);
		break;
	case C_ADC_PICKUP_02:
		if(m_fsm.Once())
		{
			m_pMtZ->Move(PZ_ADC_KIT_CLAMP);
		}
		else
		{
			if(!m_fsm.Delay(1000))
				break;
			if(!g_dIn.AOn(iOutPnpKitExist01) || !g_dIn.AOn(iOutPnpKitExist02))
			{
				g_err.Save(ER_ADC_KIT_EXIST_OUTPNP);
				m_fsm.Set(C_ADC_PICKUP_END);
				break;
			}
			if(pmCLOSE != m_pSolKitClampOC->GetPos(3000))
			{
				m_pSolKitClampOC->Actuate(pmCLOSE);
				break;
			}

			ExistKit() = TRUE;
			g_pIndex[INDEX_01]->ExistKitMovePicker() = FALSE;
			KitJobType() = g_pNV->NDm(adcIndexMovePickerJobType);
			g_pNV->NDm(adcIndexMovePickerJobType) = JOB_TYPE_IDLE;
			m_fsm.Set(C_ADC_PICKUP_END);
		}
		break;
	case C_ADC_PICKUP_END:
		if(m_fsm.Once())
			m_pMtZ->Move(PZ_READY);
		else
		{
			if(!g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_WAIT, 1000) || 
			   !g_pIndex[INDEX_01]->m_pMtT->InPos(CIndex::PT_ADC_WAIT, 1000))
			{
				g_pIndex[INDEX_01]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				break;
			}

			m_fsm.Set(C_IDLE);
		}
		break;
	}
}


//-------------------------------------------------------------------
void COutPnp::CycleRunAdcPutDn(void)
{
	if(!m_fsm.Between(C_ADC_PUTDN_START, C_ADC_PUTDN_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_OUTPNP_ADC_PUTDN_CYCLE_TM_OVER);
		return;
	}

	SetStageBusy(INDEX_01);

	if(!m_pMtY->IsRdy() || !m_pMtZ->IsRdy() || !m_pMtX->IsRdy())
		return;
	if(!g_pIndex[INDEX_01]->m_pMtX->IsRdy() || !g_pIndex[INDEX_01]->m_pMtT->IsRdy())
		return;

	switch(m_fsm.Get())
	{
	case C_ADC_PUTDN_START:
		if(!IsCylScrapFixUD(pmUP))
			SetCylScrapFixUD(pmUP);
		else if(pmUP != m_pCylScrapUD->GetPos(300))
			m_pCylScrapUD->Actuate(pmUP);
		else if(!IsCylScrapOC(pmCLOSE))
			SetCylScrapOC(pmCLOSE);
		else if(!m_pMtZ->InPos(PZ_READY))
			m_pMtZ->Move(PZ_READY);
		else if(!m_pMtX->InPos(PX_READY))
			m_pMtX->Move(PX_READY);
		else
		{
			if(!m_pMtY->InPos(PY_ADC_KIT_CLAMP))
				m_pMtY->Move(PY_ADC_KIT_CLAMP);
			else
				m_fsm.Set(C_ADC_PUTDN_01);
		}
		break;
	case C_ADC_PUTDN_01:
		if(!g_pIndex[INDEX_01]->CylIndexMaskFixAct(pmCLOSE))
			break;

		if(!g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_OUTPNP, 1000) || 
			!g_pIndex[INDEX_01]->m_pMtT->InPos(CIndex::PT_ADC_OUTPNP, 1000))
		{
			g_pIndex[INDEX_01]->m_pMtX->Move(CIndex::PX_ADC_OUTPNP);
			g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ADC_OUTPNP);
			break;
		}

		m_fsm.Set(C_ADC_PUTDN_02);
		break;
	case C_ADC_PUTDN_02:
		if(!m_pMtZ->InPos(PZ_ADC_KIT_CLAMP))
			m_pMtZ->Move(PZ_ADC_KIT_CLAMP);
		else if(pmOPEN != m_pSolKitClampOC->GetPos(3000))
			m_pSolKitClampOC->Actuate(pmOPEN);
		else
		{
			m_pMtZ->Move(PZ_READY);
			m_fsm.Set(C_ADC_PUTDN_03);
		}
		break;
	case C_ADC_PUTDN_03:
		ExistKit() = FALSE;
		g_pIndex[INDEX_01]->ExistKitMovePicker() = TRUE;
		g_pNV->NDm(adcIndexMovePickerJobType) = KitJobType();
		KitJobType() = JOB_TYPE_IDLE;
		m_fsm.Set(C_ADC_PUTDN_END);
		break;
	case C_ADC_PUTDN_END:
		if(!g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_WAIT, 1000) || 
			!g_pIndex[INDEX_01]->m_pMtT->InPos(CIndex::PT_ADC_WAIT, 1000))
		{
			g_pIndex[INDEX_01]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
			g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
			break;
		}

		m_fsm.Set(C_IDLE);
		break;
	}
}
