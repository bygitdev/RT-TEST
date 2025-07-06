#include "..\DEF\Includes.h"


//////////////////////////////////////////////////////////////////////////
CInPnp g_inPnp;
//////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
CInPnp::CInPnp() 
{
	m_bRun		   = FALSE;
	m_nPutDnIndex  = INDEX_01;
}


//-------------------------------------------------------------------
void CInPnp::AutoRun()
{
	if(!ExistKit() && !m_fsm.IsRun())
	{
		// Exist를 껏다가 켜면 Error 마지막 정보를 남겨 둬도 갢찮은지.
		KitJobType() = JOB_TYPE_IDLE;
		KitInfo()    = ADC_KIT_IDLE;
	}

	if(!ExistPcb() && !m_fsm.IsRun())
	{
		//LotInfoClear(LOT_INFO_INPNP);
	}

	if(!m_bRun)
		return;

	m_bRun = FALSE;

	if(m_fsm.IsRun())
		return;

	if(g_pNV->NDm(mmiBtnAdcMode))
	{
		int existErrVal = GetExistKitErr();
		if(EXIST_UNCERTAIN == existErrVal)
			return;
		if(EXIST_ERR == existErrVal)
		{
			g_err.Save(ER_IN_PNP_CLAMP_KIT_EXIST);
			return;
		}

	    if(ExistPcb())
		{
			g_err.Save(ER_ADC_IN_PNP_PCB_EXIST);
			return;
		}

		if(ExistKit())
		{
			if(KitJobType() <= JOB_TYPE_IDLE)
			{
				g_err.Save(ER_ADC_IN_PNP_JOB_TYPE_EMPTY);
				return;
			}

			if(!Between(KitInfo(), ADC_KIT_STAGE_01, ADC_KIT_PICKER))
			{
				g_err.Save(ER_ADC_IN_PNP_KIT_INFO_EMPTY);
				return;
			}
		}
	}
	else
	{
		int existErrVal = GetExistErr();
		if(EXIST_UNCERTAIN == existErrVal)
			return;
		if(EXIST_ERR == existErrVal)
		{
			g_err.Save(ER_IN_PNP_EXIST);
			return;
		}

		if(ExistKit())
		{
			g_err.Save(ER_ADC_IN_PNP_KIT_EXIST);
			return;
		}
	}

	if(!m_pMtW->IsRdy() || !m_pMtY->IsRdy() || !m_pMtZ->IsRdy())
		return;

	if(!m_pMtZ->InPos(PZ_READY))
	{
		m_pMtZ->Move(PZ_READY);
		return;
	}

	if(!g_pNV->UseSkip(usIndex01))
	{
		if(INDEX_01 == m_nPutDnIndex)
			m_nPutDnIndex = INDEX_03;
	}
	if(!g_pNV->UseSkip(usIndex03))
	{
		if(INDEX_03 == m_nPutDnIndex)
			m_nPutDnIndex = INDEX_02;
	}
	if(!g_pNV->UseSkip(usIndex02))
	{
		if(INDEX_02 == m_nPutDnIndex)
			m_nPutDnIndex = INDEX_04;
	}
	if(!g_pNV->UseSkip(usIndex04))
	{
		if(INDEX_04 == m_nPutDnIndex)
			m_nPutDnIndex = INDEX_01;
	}

	int nState = GetState();

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	if (S_INDEX_PUTDN == nState)
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode) + 1);
	else
		_sprintf(cMaterialId, L"$");

	switch(nState)
	{
	case S_IDLE:
		if(!m_pMtY->InPos(PY_RAIL))
		{
			m_pMtY->Move(PY_RAIL);

			if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Y_RAIL", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo],
													g_data2c.cInPnp.Y[PY_RAIL][_POSIDX_], g_data2c.cInPnp.Y[PY_RAIL][_POS_], 
													g_data2c.cInPnp.Y[PY_RAIL][_SPDIDX_], g_data2c.cInPnp.Y[PY_RAIL][_SPD_], 
													g_data2c.cInPnp.Y[PY_RAIL][_ACCIDX_], g_data2c.cInPnp.Y[PY_RAIL][_ACC_]))
			}			
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Y_RAIL", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo],
													g_data2c.cInPnp.Y[PY_RAIL][_POSIDX_], g_data2c.cInPnp.Y[PY_RAIL][_POS_], 
													g_data2c.cInPnp.Y[PY_RAIL][_SPDIDX_], g_data2c.cInPnp.Y[PY_RAIL][_SPD_], 
													g_data2c.cInPnp.Y[PY_RAIL][_ACCIDX_], g_data2c.cInPnp.Y[PY_RAIL][_ACC_]))
			}
		}
		break;
	case S_RAIL:
		if(!m_pMtY->InPos(PY_RAIL))
		{
			m_pMtY->Move(PY_RAIL);

			if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Y_RAIL", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
													g_data2c.cInPnp.Y[PY_RAIL][_POSIDX_], g_data2c.cInPnp.Y[PY_RAIL][_POS_], 
													g_data2c.cInPnp.Y[PY_RAIL][_SPDIDX_], g_data2c.cInPnp.Y[PY_RAIL][_SPD_], 
													g_data2c.cInPnp.Y[PY_RAIL][_ACCIDX_], g_data2c.cInPnp.Y[PY_RAIL][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL])
			{
				g_logChk.bTransfer[m_pMtY->m_config.axisNo][PY_RAIL] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Y_RAIL", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
													g_data2c.cInPnp.Y[PY_RAIL][_POSIDX_], g_data2c.cInPnp.Y[PY_RAIL][_POS_], 
													g_data2c.cInPnp.Y[PY_RAIL][_SPDIDX_], g_data2c.cInPnp.Y[PY_RAIL][_SPD_], 
													g_data2c.cInPnp.Y[PY_RAIL][_ACCIDX_], g_data2c.cInPnp.Y[PY_RAIL][_ACC_]))
			}
		}

		if(!m_pMtW->InPos(PW_RAIL_OPEN))
		{
			m_pMtW->Move(PW_RAIL_OPEN);

			if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN])
			{
				g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_CLAMP_RAIL_OPEN", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POSIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POS_], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPDIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPD_], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACCIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN])
			{
				g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_CLAMP_RAIL_OPEN", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POSIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POS_], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPDIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPD_], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACCIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACC_]))
			}
		}
		
		if(g_rail.IsReadyInPnp())
			m_fsm.Set(C_PCB_PICKUP_START);
		break;
	case S_INDEX_PUTDN:
		{
			m_nPutDnIndex = GetPcbPutDnIndex();

			if(INDEX_IDLE < m_nPutDnIndex)
			{
				if(!IsReadyMtInPnpYPcbPutDn(m_nPutDnIndex))
					MoveMtInPnpYPcbPutDn(m_nPutDnIndex);
				else if(g_pIndex[m_nPutDnIndex]->IsReadyInPnp())
					m_fsm.Set(C_PCB_PUTDN_START, m_nPutDnIndex);
			}
		}
		break;

	/////////////////////////////////////////////////////////////////////
	// ADC Status
	/////////////////////////////////////////////////////////////////////
	case S_ADC_RAIL_PICKUP:
		if(!m_pMtY->InPos(PY_ADC_RAIL))
			m_pMtY->Move(PY_ADC_RAIL);
		else if(g_adc.IsReadyPickUp())
			m_fsm.Set(C_ADC_RAIL_PICKUP_START, g_adc.KitInfo());
		break;
	case S_ADC_RAIL_PUTDN:
		if(!m_pMtY->InPos(PY_ADC_RAIL))
			m_pMtY->Move(PY_ADC_RAIL);
		else if(g_adc.IsReadyPutDn())
			m_fsm.Set(C_ADC_RAIL_PUTDN_START, KitInfo());
		break;
	case S_ADC_INDEX_PICKUP:
		{
			// Index의 Kit 종류 확인 후 Pickup
			int nReturnNo = g_adc.GetAdcIndexReturnNo();

			if(Between(nReturnNo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
			{
				int nIdx = nReturnNo - ADC_KIT_STAGE_01;
				int nPos = PY_ADC_STAGE_01 + nIdx;

				BOOL bReturn = FALSE;
				for(int nNo = 0; nNo < 4; nNo++)
				{
					bReturn |= !g_pIndex[nNo]->m_pMtX->IsRdy(CIndex::PX_ADC_WAIT);
					bReturn |= !g_pIndex[nNo]->m_pMtT->IsRdy(CIndex::PT_ADC_WAIT);
				}

				if(bReturn)
					break;

				if(INDEX_01 == nIdx)
				{
					BOOL bOutPnpBusy  = g_outPnp.m_fsm.Between(COutPnp::C_ADC_PICKUP_START, COutPnp::C_ADC_PICKUP_END);
						 bOutPnpBusy |= g_outPnp.m_fsm.Between(COutPnp::C_ADC_PUTDN_START, COutPnp::C_ADC_PUTDN_END);

					if(bOutPnpBusy)
					{
						// Index 2로 이동한 하여 대기한다. 
						m_pMtY->Move(PY_ADC_STAGE_02);
						break;
					}
				}

				if(!m_pMtY->InPos(nPos))
					m_pMtY->Move(nPos);
				else if(g_pIndex[nIdx]->IsReadyAdcInPnpPickUpStage())
					m_fsm.Set(C_ADC_INDEX_PICKUP_START, nReturnNo);
			}
			else if(Between(nReturnNo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
			{
				int nIdx = nReturnNo - ADC_KIT_MASK_01;
				int nPos = PY_ADC_MASK_01 + nIdx;

				BOOL bReturn = FALSE;
				for(int nNo = 0; nNo < 4; nNo++)
				{
					bReturn |= !g_pIndex[nNo]->m_pMtX->IsRdy(CIndex::PX_ADC_WAIT);
					bReturn |= !g_pIndex[nNo]->m_pMtT->IsRdy(CIndex::PT_ADC_WAIT);
				}

				if(bReturn)
					break;

				if(INDEX_01 == nIdx)
				{
					BOOL bOutPnpBusy  = g_outPnp.m_fsm.Between(COutPnp::C_ADC_PICKUP_START, COutPnp::C_ADC_PICKUP_END);
						 bOutPnpBusy |= g_outPnp.m_fsm.Between(COutPnp::C_ADC_PUTDN_START, COutPnp::C_ADC_PUTDN_END);

					if(bOutPnpBusy)
					{
						// Index 2로 이동한 하여 대기한다. 
						m_pMtY->Move(PY_ADC_STAGE_02);
						break;
					}
				}

				if(!m_pMtY->InPos(nPos))
					m_pMtY->Move(nPos);
				else if(g_pIndex[nIdx]->IsReadyAdcInPnpPickUpMask())
					m_fsm.Set(C_ADC_INDEX_PICKUP_START, nReturnNo);
			}
			else if(ADC_KIT_PICKER == nReturnNo)
			{
				BOOL bReturn = FALSE;
				for(int nNo = 0; nNo < 4; nNo++)
				{
					bReturn |= !g_pIndex[nNo]->m_pMtX->IsRdy(CIndex::PX_ADC_WAIT);
					bReturn |= !g_pIndex[nNo]->m_pMtT->IsRdy(CIndex::PT_ADC_WAIT);
				}

				if(bReturn)
					break;

				// OutPnp Cycle 중이거나 Index에 OutPnp Kit이 존재하면 
				// Station 제로포인트에 Index Mask F/B 동작이 간섭됨
				BOOL bOutPnpBusy  = g_outPnp.m_fsm.Between(COutPnp::C_ADC_PICKUP_START, COutPnp::C_ADC_PICKUP_END);
					 bOutPnpBusy |= g_outPnp.m_fsm.Between(COutPnp::C_ADC_PUTDN_START, COutPnp::C_ADC_PUTDN_END);
					 bOutPnpBusy |= g_pNV->NDm(existKitOutPnp);

				if(bOutPnpBusy)
				{
					// Index 2로 이동한 하여 대기한다. 
					m_pMtY->Move(PY_ADC_STAGE_02);
					break;
				}

				if(!m_pMtY->InPos(PY_ADC_MASK_01))
					m_pMtY->Move(PY_ADC_MASK_01);
				else if(g_pIndex[INDEX_01]->IsReadyAdcInPnpPickUpPicker())
					m_fsm.Set(C_ADC_INDEX_PICKUP_START, nReturnNo);
			}
		}
		break;
	case S_ADC_INDEX_PUTDN:
		if(Between(KitInfo(), ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
		{
			int nIdx = KitInfo() - ADC_KIT_STAGE_01;
			int nPos = PY_ADC_STAGE_01 + nIdx;

			BOOL bReturn = FALSE;
			for(int nNo = 0; nNo < 4; nNo++)
			{
				bReturn |= !g_pIndex[nNo]->m_pMtX->IsRdy(CIndex::PX_ADC_WAIT);
				bReturn |= !g_pIndex[nNo]->m_pMtT->IsRdy(CIndex::PT_ADC_WAIT);
				bReturn |= !g_pIndex[nNo]->IsCylMaskFixUD(pmUP, 300);
				bReturn |= !g_pIndex[nNo]->IsCylMaskFixFB(pmBWD, 300);
			}

			if(bReturn)
				break;

			if(INDEX_01 == nIdx)
			{
				// OutPnp Cycle 중이거나 Index에 OutPnp Kit이 존재하면 
				// Station 제로포인트에 Index Mask F/B 동작이 간섭됨
				BOOL bOutPnpBusy  = g_outPnp.m_fsm.Between(COutPnp::C_ADC_PICKUP_START, COutPnp::C_ADC_PICKUP_END);
					 bOutPnpBusy |= g_outPnp.m_fsm.Between(COutPnp::C_ADC_PUTDN_START, COutPnp::C_ADC_PUTDN_END);
					 bOutPnpBusy |= g_pNV->NDm(existKitMovePicker);

				if(bOutPnpBusy)
				{
					// Index 2로 이동한 하여 대기한다. 
					m_pMtY->Move(PY_ADC_STAGE_02);
					break;
				}
			}

			if(g_pIndex[nIdx]->IsReadyAdcInPnpPutDnStage())
			{
				if(!m_pMtY->InPos(nPos))
				{
					m_pMtY->Move(nPos);
					break;
				}

				m_fsm.Set(C_ADC_INDEX_PUTDN_START, KitInfo());
			}
		}
		else if(Between(KitInfo(), ADC_KIT_MASK_01, ADC_KIT_MASK_04))
		{
			int nIdx = KitInfo() - ADC_KIT_MASK_01;
			int nPos = PY_ADC_MASK_01 + nIdx;

			BOOL bReturn = FALSE;
			for(int nNo = 0; nNo < 4; nNo++)
			{
				bReturn |= !g_pIndex[nNo]->m_pMtX->IsRdy(CIndex::PX_ADC_WAIT);
				bReturn |= !g_pIndex[nNo]->m_pMtT->IsRdy(CIndex::PT_ADC_WAIT);
			}

			if(bReturn)
				break;

			if(INDEX_01 == nIdx)
			{
				// OutPnp Cycle 중이거나 Index에 OutPnp Kit이 존재하면 
				// Station 제로포인트에 Index Mask F/B 동작이 간섭됨
				BOOL bOutPnpBusy  = g_outPnp.m_fsm.Between(COutPnp::C_ADC_PICKUP_START, COutPnp::C_ADC_PICKUP_END);
					 bOutPnpBusy |= g_outPnp.m_fsm.Between(COutPnp::C_ADC_PUTDN_START, COutPnp::C_ADC_PUTDN_END);
					 bOutPnpBusy |= g_pNV->NDm(existKitMovePicker);

				if(bOutPnpBusy)
				{
					// Index 2로 이동한 하여 대기한다. 
					m_pMtY->Move(PY_ADC_STAGE_02);
					break;
				}
			}

			if(!m_pMtY->InPos(nPos))
				m_pMtY->Move(nPos);
			else if(g_pIndex[nIdx]->IsReadyAdcInPnpPutDnMask())
				m_fsm.Set(C_ADC_INDEX_PUTDN_START, KitInfo());
		}
		else if(ADC_KIT_PICKER == KitInfo())
		{
			BOOL bReturn = FALSE;
			for(int nNo = 0; nNo < 4; nNo++)
			{
				bReturn |= !g_pIndex[nNo]->m_pMtX->IsRdy(CIndex::PX_ADC_WAIT);
				bReturn |= !g_pIndex[nNo]->m_pMtT->IsRdy(CIndex::PT_ADC_WAIT);
			}

			if(bReturn)
				break;

			BOOL bOutPnpBusy  = g_outPnp.m_fsm.Between(COutPnp::C_ADC_PICKUP_START, COutPnp::C_ADC_PICKUP_END);
				 bOutPnpBusy |= g_outPnp.m_fsm.Between(COutPnp::C_ADC_PUTDN_START, COutPnp::C_ADC_PUTDN_END);

			if(bOutPnpBusy)
			{
				// Index 2로 이동한 하여 대기한다. 
				m_pMtY->Move(PY_ADC_STAGE_02);
				break;
			}

			if(!m_pMtY->InPos(PY_ADC_MASK_01))
				m_pMtY->Move(PY_ADC_MASK_01);
			else if(g_pIndex[INDEX_01]->IsReadyAdcInPnpPutDnPicker())
				m_fsm.Set(C_ADC_INDEX_PUTDN_START, KitInfo());
		}
		break;
	}
}
	

//-------------------------------------------------------------------
void CInPnp::CycleRun(void)
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

	CycleRunPcbPickUp();
	CycleRunPcbPutDn();
	CycleRunAdcRailPickUp();
	CycleRunAdcRailPutDn();
	CycleRunAdcIndexPickUp();
	CycleRunAdcIndexPutDn();
}


//-------------------------------------------------------------------
void CInPnp::Init(void)
{
	m_pMtW	= &g_mt[MT_INPNP_CLAMP_Y];
	m_pMtY	= &g_mt[MT_INPNP_Y];
	m_pMtZ	= &g_mt[MT_INPNP_Z];
}


//-------------------------------------------------------------------
int& CInPnp::ExistPcb(void)
{
	return (g_pNV->m_pData->ndm[existInPnp]);
}


//-------------------------------------------------------------------
int& CInPnp::ExistKit(void)
{
	return (g_pNV->m_pData->ndm[existInPnpClampKit]);
}


//-------------------------------------------------------------------
int& CInPnp::KitJobType(void)
{
	return (g_pNV->m_pData->ndm[adcInPnpKitJobType]);
}


//-------------------------------------------------------------------
int& CInPnp::KitInfo(void)
{
	return (g_pNV->m_pData->ndm[adcInPnpKitInfo]);
}


//-------------------------------------------------------------------
BOOL CInPnp::IsReadyMtInPnpYPcbPutDn(int nIdx)
{
	int    nIndex  = PY_PUTDN_01 + nIdx;
	double dOffset = g_pNV->Pkg(offsetInPnpYPutDn1 + nIdx) * 1000.0;	
	double dPos	   = m_pMtY->m_pTable->pos[nIndex] + dOffset;

	if(!m_pMtY->IsRdy())
		return (FALSE);

	if(!m_pMtY->InPos(nIndex, dPos, 50))
		return (FALSE);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode) + 1);

	_char cPos[_MAX_CHAR_SIZE_];
	_sprintf(cPos, L"%03f", dPos);

	_char cEventId[_MAX_CHAR_SIZE_];
	_sprintf(cEventId, L"MT_INPNP_Y_PUTDN_%02d", (nIdx + 1));

	if(g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex] = FALSE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, cEventId, g_data2c.cEtc.end, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
											g_data2c.cInPnp.Y[nIndex][_POSIDX_], cPos, 
											g_data2c.cInPnp.Y[nIndex][_SPDIDX_], g_data2c.cInPnp.Y[nIndex][_SPD_], 
											g_data2c.cInPnp.Y[nIndex][_ACCIDX_], g_data2c.cInPnp.Y[nIndex][_ACC_]))
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CInPnp::MoveMtInPnpYPcbPutDn(int nIdx)
{
	int	   nIndex  = PY_PUTDN_01 + nIdx;
	double dOffset = g_pNV->Pkg(offsetInPnpYPutDn1 + nIdx) * 1000.0;	
	double dPos	   = m_pMtY->m_pTable->pos[nIndex] + dOffset;

	m_pMtY->PMove(nIndex, dPos);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode) + 1);

	_char cPos[_MAX_CHAR_SIZE_];
	_sprintf(cPos, L"%03f", dPos);

	_char cEventId[_MAX_CHAR_SIZE_];
	_sprintf(cEventId, L"MT_INPNP_Y_PUTDN_%02d", (nIdx + 1));

	if(!g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtY->m_config.axisNo][nIndex] = TRUE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, cEventId, g_data2c.cEtc.start, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtY->m_config.axisNo], 
											g_data2c.cInPnp.Y[nIndex][_POSIDX_], cPos, 
											g_data2c.cInPnp.Y[nIndex][_SPDIDX_], g_data2c.cInPnp.Y[nIndex][_SPD_], 
											g_data2c.cInPnp.Y[nIndex][_ACCIDX_], g_data2c.cInPnp.Y[nIndex][_ACC_]))
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CInPnp::IsReadyMtInPnpZOverride(int nMtIdx)
{
	int    nIndex = nMtIdx;
	double dPos	  = m_pMtZ->m_pTable->pos[nIndex];
	double dOffset = 0.0;	

	if(Between(nMtIdx, PZ_PCB_PUTDN_01, PZ_PCB_PUTDN_04))
	{
		if(PZ_PCB_PUTDN_01 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetInPnpZPutDn1) * 1000.0;	
		else if(PZ_PCB_PUTDN_02 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetInPnpZPutDn2) * 1000.0;	
		else if(PZ_PCB_PUTDN_03 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetInPnpZPutDn3) * 1000.0;	
		else if(PZ_PCB_PUTDN_04 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetInPnpZPutDn4) * 1000.0;

		dPos += dOffset;
	}

	if(!m_pMtZ->IsRdy())
		return (FALSE);

	if(!m_pMtZ->InPos(nIndex, dPos, 50))
		return (FALSE);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");

	_char cPos[_MAX_CHAR_SIZE_];
	_sprintf(cPos, L"%03f", dPos);

	if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtZ->m_config.axisNo][nIndex] = FALSE;
		
		if(Between(nMtIdx, PZ_PCB_PUTDN_01, PZ_PCB_PUTDN_04))
		{
			_char cEventId[_MAX_CHAR_SIZE_];

			if(nMtIdx == PZ_PCB_PUTDN_01)
				_sprintf(cEventId, L"MT_INPNP_Z_PCB_PUTDN_01");
			else if(nMtIdx == PZ_PCB_PUTDN_02)
				_sprintf(cEventId, L"MT_INPNP_Z_PCB_PUTDN_02");
			else if(nMtIdx == PZ_PCB_PUTDN_03)
				_sprintf(cEventId, L"MT_INPNP_Z_PCB_PUTDN_03");
			else 
				_sprintf(cEventId, L"MT_INPNP_Z_PCB_PUTDN_04");

			mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode) + 1);
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, cEventId, g_data2c.cEtc.end, 				
												cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
												g_data2c.cInPnp.Z[nIndex][_POSIDX_], cPos, 
												g_data2c.cInPnp.Z[nIndex][_SPDIDX_], g_data2c.cInPnp.Z[nIndex][_SPD_], 
												g_data2c.cInPnp.Z[nIndex][_ACCIDX_], g_data2c.cInPnp.Z[nIndex][_ACC_]))
		}
		else
		{
			_sprintf(cMaterialId, L"$");
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Z_RAIL", g_data2c.cEtc.end, 				
												cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
												g_data2c.cInPnp.Z[nIndex][_POSIDX_], cPos, 
												g_data2c.cInPnp.Z[nIndex][_SPDIDX_], g_data2c.cInPnp.Z[nIndex][_SPD_], 
												g_data2c.cInPnp.Z[nIndex][_ACCIDX_], g_data2c.cInPnp.Z[nIndex][_ACC_]))
		}
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CInPnp::MoveMtInPnpZOverride(int nMtIdx)
{
	int	   nIndex = nMtIdx;
	double dPos	  = m_pMtZ->m_pTable->pos[nIndex];
	double dOffset = 0.0;	

	if(Between(nMtIdx, PZ_PCB_PUTDN_01, PZ_PCB_PUTDN_04))
	{
		if(PZ_PCB_PUTDN_01 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetInPnpZPutDn1) * 1000.0;	
		else if(PZ_PCB_PUTDN_02 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetInPnpZPutDn2) * 1000.0;	
		else if(PZ_PCB_PUTDN_03 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetInPnpZPutDn3) * 1000.0;	
		else if(PZ_PCB_PUTDN_04 == nMtIdx)
			dOffset = g_pNV->Pkg(offsetInPnpZPutDn4) * 1000.0;

		dPos += dOffset;
	}

	double endPos   = dPos;
	double slowPos  = dPos - m_pMtZ->m_pTable->pos[PZ_SLOW_DN_OFFSET];
	double startVel = m_pMtZ->m_pTable->vel[nIndex];
	double slowVel  = m_pMtZ->m_pTable->vel[PZ_SLOW_DN_OFFSET];

	m_pMtZ->Move2(nIndex, endPos, slowPos, slowVel, startVel);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");

	_char cPos[_MAX_CHAR_SIZE_];
	_sprintf(cPos, L"%03f", endPos);

	if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][nIndex])
	{
		g_logChk.bTransfer[m_pMtZ->m_config.axisNo][nIndex] = TRUE;

		if(Between(nMtIdx, PZ_PCB_PUTDN_01, PZ_PCB_PUTDN_04))
		{
			_char cEventId[_MAX_CHAR_SIZE_];

			if(nMtIdx == PZ_PCB_PUTDN_01)
				_sprintf(cEventId, L"MT_INPNP_Z_PCB_PUTDN_01");
			else if(nMtIdx == PZ_PCB_PUTDN_02)
				_sprintf(cEventId, L"MT_INPNP_Z_PCB_PUTDN_02");
			else if(nMtIdx == PZ_PCB_PUTDN_03)
				_sprintf(cEventId, L"MT_INPNP_Z_PCB_PUTDN_03");
			else 
				_sprintf(cEventId, L"MT_INPNP_Z_PCB_PUTDN_04");

			mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode) + 1);
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, cEventId, g_data2c.cEtc.start, 				
												cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
												g_data2c.cInPnp.Z[nIndex][_POSIDX_], cPos, 
												g_data2c.cInPnp.Z[nIndex][_SPDIDX_], g_data2c.cInPnp.Z[nIndex][_SPD_], 
												g_data2c.cInPnp.Z[nIndex][_ACCIDX_], g_data2c.cInPnp.Z[nIndex][_ACC_]))
		}
		else
		{
			_sprintf(cMaterialId, L"$");
			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Z_RAIL", g_data2c.cEtc.start, 				
												cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
												g_data2c.cInPnp.Z[nIndex][_POSIDX_], cPos, 
												g_data2c.cInPnp.Z[nIndex][_SPDIDX_], g_data2c.cInPnp.Z[nIndex][_SPD_], 
												g_data2c.cInPnp.Z[nIndex][_ACCIDX_], g_data2c.cInPnp.Z[nIndex][_ACC_]))
		}
	}

	return (TRUE);
}


//-------------------------------------------------------------------
int CInPnp::GetState(void)
{
	int nState = S_IDLE;

	if(g_pNV->NDm(mmiBtnAdcMode))
	{
		if(ExistKit())
		{
			if(g_pNV->Pkg(adcKitJobType) == KitJobType())
				nState = S_ADC_INDEX_PUTDN;
			else
				nState = S_ADC_RAIL_PUTDN;
		}
		else
		{
			int nReturnNo = g_adc.GetAdcIndexReturnNo();
			int nGripNo   = g_adc.GetAdcMzGripNo();

			if(ADC_KIT_IDLE < nReturnNo)
				nState = S_ADC_INDEX_PICKUP;
			else if(ADC_MZ_KIT_IDLE < nGripNo)
				nState = S_ADC_RAIL_PICKUP;
			else if(g_adc.Exist() && (ADC_RAIL_PICKUP == g_adc.AdcState()))
				nState = S_ADC_RAIL_PICKUP;
		}
	}
	else
	{
		if(ExistPcb())
			nState = S_INDEX_PUTDN;
		else
			nState = S_RAIL;
	}

	return (nState);
}


//-------------------------------------------------------------------
BOOL CInPnp::IsErr(void)
{
	if(!m_pMtW->m_state.isHome)
		return (TRUE);
	if(!m_pMtY->m_state.isHome)
		return (TRUE);
	if(!m_pMtZ->m_state.isHome)
		return (TRUE);

	if(m_fsm.Between(C_ADC_INDEX_PICKUP_START, C_ADC_INDEX_PICKUP_END) || 
	   m_fsm.Between(C_ADC_INDEX_PUTDN_START, C_ADC_INDEX_PUTDN_END))
	{
		if(Between(m_fsm.GetMsg(), ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
		{
			int nIdx = m_fsm.GetMsg() - ADC_KIT_STAGE_01;
			if(!g_pIndex[nIdx]->m_pMtX->m_state.isHome || !g_pIndex[nIdx]->m_pMtX->m_state.isHome || !g_pIndex[nIdx]->m_pMtT->m_state.isHome)
				return (TRUE);
			if(0 < g_pIndex[nIdx]->m_pCylMaskFB_L->GetErr())
				return (TRUE);
			if(0 < g_pIndex[nIdx]->m_pCylMaskFB_R->GetErr())
				return (TRUE);
			if(0 < g_pIndex[nIdx]->m_pCylMaskUD->GetErr())
				return (TRUE);
		}
		else if(Between(m_fsm.GetMsg(), ADC_KIT_MASK_01, ADC_KIT_MASK_04))
		{
			int nIdx = m_fsm.GetMsg() - ADC_KIT_MASK_01;
			if(!g_pIndex[nIdx]->m_pMtX->m_state.isHome || !g_pIndex[nIdx]->m_pMtX->m_state.isHome || !g_pIndex[nIdx]->m_pMtT->m_state.isHome)
				return (TRUE);
			if(0 < g_pIndex[nIdx]->m_pCylMaskFB_L->GetErr())
				return (TRUE);
			if(0 < g_pIndex[nIdx]->m_pCylMaskFB_R->GetErr())
				return (TRUE);
			if(0 < g_pIndex[nIdx]->m_pCylMaskUD->GetErr())
				return (TRUE);
		}
		else if(ADC_KIT_PICKER == m_fsm.GetMsg())
		{
			if(!g_pIndex[INDEX_01]->m_pMtX->m_state.isHome || !g_pIndex[INDEX_01]->m_pMtX->m_state.isHome || !g_pIndex[INDEX_01]->m_pMtT->m_state.isHome)
				return (TRUE);
			if(0 < g_pIndex[INDEX_01]->m_pCylMaskFB_L->GetErr())
				return (TRUE);
			if(0 < g_pIndex[INDEX_01]->m_pCylMaskFB_R->GetErr())
				return (TRUE);
			if(0 < g_pIndex[INDEX_01]->m_pCylMaskUD->GetErr())
				return (TRUE);
		}
	}

	if(m_fsm.Between(C_ADC_RAIL_PICKUP_START, C_ADC_RAIL_PICKUP_END))
	{
		if(!g_adc.m_pMtX->m_state.isHome)
			return (TRUE);
	}

	return (FALSE);
}


//-------------------------------------------------------------------
int  CInPnp::GetExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL bSenOn = g_dIn.AOn(iInPnpExist01) || g_dIn.AOn(iInPnpExist02); 

	if(ExistPcb() == bSenOn)
	{
		m_tmExistErr.Reset();
	}
	else
	{
		if(m_tmExistErr.TmOver(5000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}
	
	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
int  CInPnp::GetExistKitErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	if(ExistKit() == g_dIn.AOn(iInPnpKitExist))
	{
		m_tmExistErr.Reset();
	}
	else
	{
		if(m_tmExistErr.TmOver(5000))
			return (EXIST_ERR);
		else
			return (EXIST_UNCERTAIN);
	}
	
	return (EXIST_NORMAL);
}


//-------------------------------------------------------------------
void CInPnp::CycleRunPcbPickUp(void)
{
	if(!m_fsm.Between(C_PCB_PICKUP_START, C_PCB_PICKUP_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_IN_PNP_PCB_PICKUP_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtW->IsRdy() || !m_pMtY->IsRdy() || !m_pMtZ->IsRdy())
		return;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialId, L"$");
	_sprintf(cMaterialType, L"PCB");	

	switch(m_fsm.Get())
	{
	case C_PCB_PICKUP_START:
		if(m_fsm.Once())
		{
			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemInPnpPcbPickupStart) = STATE_REQ;

			m_pMtZ->Move(PZ_READY);
			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Z_READY_INIT", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cInPnp.Z[PZ_READY][_POSIDX_], g_data2c.cInPnp.Z[PZ_READY][_POS_], 
													g_data2c.cInPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cInPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cInPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cInPnp.Z[PZ_READY][_ACC_]))
			}

			m_pMtW->Move(PW_RAIL_OPEN);
			if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN])
			{
				g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_CLAMP_Y_RAIL_OPEN", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POSIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POS_], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPDIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPD_], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACCIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACC_]))
			}
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Z_READY_INIT", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cInPnp.Z[PZ_READY][_POSIDX_], g_data2c.cInPnp.Z[PZ_READY][_POS_], 
													g_data2c.cInPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cInPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cInPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cInPnp.Z[PZ_READY][_ACC_]))
			}
			if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN])
			{
				g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_CLAMP_Y_RAIL_OPEN", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POSIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POS_], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPDIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPD_], 
													g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACCIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACC_]))
			}

			if(!IsReadyMtInPnpZOverride(PZ_PCB_RAIL))
			{
				MoveMtInPnpZOverride(PZ_PCB_RAIL);
				break;
			}

			if(!m_pMtW->InPos(PW_RAIL_PICKUP))
			{
				m_pMtW->Move(PW_RAIL_PICKUP);

				NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cInPnp.deviceId, L"PICKUP_PCB", g_data2c.cEtc.start, L"$", cMaterialType, L"RAIL", L"INPNP"))

				if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_PICKUP])
				{
					g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_PICKUP] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_CLAMP_Y_RAIL_PICKUP", g_data2c.cEtc.start, 
														L"$", cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
														g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_POSIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_POS_], 
														g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_SPDIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_SPD_], 
														g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_ACCIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_PICKUP])
				{
					g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_PICKUP] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_CLAMP_Y_RAIL_PICKUP", g_data2c.cEtc.end, 
														L"$", cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
														g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_POSIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_POS_], 
														g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_SPDIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_SPD_], 
														g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_ACCIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_PICKUP][_ACC_]))
				}
			}

			m_fsm.Set(C_PCB_PICKUP_END);
		}
		break;
	case C_PCB_PICKUP_END:
		if(!m_pMtZ->InPos(PZ_READY))
		{
			m_pMtZ->Move(PZ_READY);

			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Z_READY_PICKUP", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cInPnp.Z[PZ_READY][_POSIDX_], g_data2c.cInPnp.Z[PZ_READY][_POS_], 
													g_data2c.cInPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cInPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cInPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cInPnp.Z[PZ_READY][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Z_READY_PICKUP", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cInPnp.Z[PZ_READY][_POSIDX_], g_data2c.cInPnp.Z[PZ_READY][_POS_], 
													g_data2c.cInPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cInPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cInPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cInPnp.Z[PZ_READY][_ACC_]))
			}

			if(g_opr.isDryRun)
			{
				ExistPcb() = TRUE;
				g_rail.Exist() = FALSE;
				m_fsm.Set(C_IDLE);
				break;
			}

			g_lotInfo.LotInfoCopy(LOT_INFO_RAIL, LOT_INFO_INPNP);
			g_lotInfo.LotInfoCopy(LOT_INFO_RAIL, LOT_INFO_OLD_RAIL);
			g_lotInfo.LotInfoClear(LOT_INFO_RAIL);
			g_ldMz.m_bNewRailInfo = FALSE;
			ExistPcb() = TRUE;
			g_rail.Exist() = FALSE;

			if(!g_dIn.AOn(iInPnpExist01) && !g_dIn.AOn(iInPnpExist02))
				m_fsm.Set(C_ERROR, ER_IN_PNP_EXIST_NOT_ON);
			else if(g_dIn.AOn(iRailExistEnd))
				m_fsm.Set(C_ERROR, ER_RAIL_EXIST_END_NOT_OFF);
			else
			{
				if(g_pNV->UseSkip(usSecsGem))
					g_pNV->NDm(gemInPnpPcbPickupEnd) = STATE_REQ;

				mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode) + 1);
				NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cInPnp.deviceId, L"PICKUP_PCB", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"RAIL", L"INPNP"))

				m_fsm.Set(C_IDLE);
			}
		}	
		break;
	}
}


//-------------------------------------------------------------------
void CInPnp::CycleRunPcbPutDn(void)
{
	if(!m_fsm.Between(C_PCB_PUTDN_START, C_PCB_PUTDN_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_IN_PNP_PCB_PUTDN_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtW->IsRdy() || !m_pMtY->IsRdy() || !m_pMtZ->IsRdy())
		return;

	int nIdx = m_fsm.GetMsg();

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_INPNP].pcbBarcode) + 1);
	_sprintf(cMaterialType, L"PCB");

	_char cIndexId[_MAX_CHAR_SIZE_];
	_sprintf(cIndexId, L"INDEX%d", (nIdx + 1));

	switch(m_fsm.Get())
	{
	case C_PCB_PUTDN_START:
		if(m_fsm.Once())
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cInPnp.deviceId, L"PUTDOWN_PCB", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"INPNP", cIndexId))

		if(!m_pMtZ->InPos(PZ_READY))
		{
			m_pMtZ->Move(PZ_READY);
			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Z_READY_PUTDN_INIT", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cInPnp.Z[PZ_READY][_POSIDX_], g_data2c.cInPnp.Z[PZ_READY][_POS_], 
													g_data2c.cInPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cInPnp.Z[PZ_READY][_SPD_],
													g_data2c.cInPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cInPnp.Z[PZ_READY][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Z_READY_PUTDN_INIT", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cInPnp.Z[PZ_READY][_POSIDX_], g_data2c.cInPnp.Z[PZ_READY][_POS_], 
													g_data2c.cInPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cInPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cInPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cInPnp.Z[PZ_READY][_ACC_]))
			}
		}

		if(!IsReadyMtInPnpYPcbPutDn(nIdx))
		{
			MoveMtInPnpYPcbPutDn(nIdx);
			break;
		}
		m_fsm.Set(C_PCB_PUTDN_01);
		break;
	case C_PCB_PUTDN_01:
		{
			if(!IsReadyMtInPnpZOverride(PZ_PCB_PUTDN_01 + nIdx))
			{
				MoveMtInPnpZOverride(PZ_PCB_PUTDN_01 + nIdx);
				break;
			}

			if(!m_pMtW->InPos(PW_RAIL_OPEN))
			{
				m_pMtW->Move(PW_RAIL_OPEN);
				if(!g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN])
				{
					g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_CLAMP_Y_RAIL_OPEN", g_data2c.cEtc.start, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
														g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POSIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POS_], 
														g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPDIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPD_], 
														g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACCIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN])
				{
					g_logChk.bTransfer[m_pMtW->m_config.axisNo][PW_RAIL_OPEN] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_CLAMP_Y_RAIL_OPEN", g_data2c.cEtc.end, 
														cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtW->m_config.axisNo], 
														g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POSIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_POS_], 
														g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPDIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_SPD_], 
														g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACCIDX_], g_data2c.cInPnp.ClampY[PW_RAIL_OPEN][_ACC_]))
				}
			}

			m_fsm.Set(C_PCB_PUTDN_END);
		}
		break;

	case C_PCB_PUTDN_END:
		if(!m_pMtZ->InPos(PZ_READY))
		{
			m_pMtZ->Move(PZ_READY);

			if(!g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Z_READY_PUTDN", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cInPnp.Z[PZ_READY][_POSIDX_], g_data2c.cInPnp.Z[PZ_READY][_POS_], 
													g_data2c.cInPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cInPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cInPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cInPnp.Z[PZ_READY][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY])
			{
				g_logChk.bTransfer[m_pMtZ->m_config.axisNo][PZ_READY] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cInPnp.deviceId, L"MT_INPNP_Z_READY_PUTDN", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtZ->m_config.axisNo], 
													g_data2c.cInPnp.Z[PZ_READY][_POSIDX_], g_data2c.cInPnp.Z[PZ_READY][_POS_], 
													g_data2c.cInPnp.Z[PZ_READY][_SPDIDX_], g_data2c.cInPnp.Z[PZ_READY][_SPD_], 
													g_data2c.cInPnp.Z[PZ_READY][_ACCIDX_], g_data2c.cInPnp.Z[PZ_READY][_ACC_]))
			}
		}

		ExistPcb() = FALSE;
				
		g_pIndex[nIdx]->ExistPcb()		= TRUE;
		g_pIndex[nIdx]->ExistScrap()	= TRUE;

		g_lotInfo.LotInfoCopy(LOT_INFO_INPNP, LOT_INFO_INDEX01 + nIdx);
		g_lotInfo.LotInfoClear(LOT_INFO_INPNP);

		if(INDEX_01 == nIdx)
			m_nPutDnIndex = INDEX_03;
		else if(INDEX_02 == nIdx)
			m_nPutDnIndex = INDEX_04;
		else if(INDEX_03 == nIdx)
			m_nPutDnIndex = INDEX_02;
		else if(INDEX_04 == nIdx)
			m_nPutDnIndex = INDEX_01;

		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cInPnp.deviceId, L"PUTDOWN_PCB", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"INPNP", cIndexId))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CInPnp::CycleRunAdcRailPickUp(void)
{
	if(!m_fsm.Between(C_ADC_RAIL_PICKUP_START, C_ADC_RAIL_PICKUP_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_IN_PNP_ADC_RAIL_PICKUP_CYCLE_TM_OVER);
		return;
	}

	int nKitInfo = m_fsm.GetMsg();

	if(!m_pMtW->IsRdy() || !m_pMtY->IsRdy() || !m_pMtZ->IsRdy())
		return;
	
	switch(m_fsm.Get())
	{
	case C_ADC_RAIL_PICKUP_START:
		if(!m_pMtZ->InPos(PZ_READY))
			m_pMtZ->Move(PZ_READY);
		else if(!m_pMtY->InPos(PY_ADC_RAIL))
			m_pMtY->Move(PY_ADC_RAIL);
		else if(!m_pMtW->InPos(PW_ADC_OPEN))
			m_pMtW->Move(PW_ADC_OPEN);
		else
			m_fsm.Set(C_ADC_RAIL_PICKUP_01);
		break;
	case C_ADC_RAIL_PICKUP_01:
		if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
		{
			if(!IsReadyMtInPnpZOverride(PZ_ADC_RAIL_STAGE))
				MoveMtInPnpZOverride(PZ_ADC_RAIL_STAGE);
			else if(!m_pMtW->InPos(PW_ADC_STAGE_PICKUP))
				m_pMtW->Move(PW_ADC_STAGE_PICKUP);
			else
				m_fsm.Set(C_ADC_RAIL_PICKUP_02);
		}
		else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
		{
			if(!IsReadyMtInPnpZOverride(PZ_ADC_RAIL_MASK))
				MoveMtInPnpZOverride(PZ_ADC_RAIL_MASK);
			else if(!m_pMtW->InPos(PW_ADC_MASK_PICKUP))
				m_pMtW->Move(PW_ADC_MASK_PICKUP);
			else
				m_fsm.Set(C_ADC_RAIL_PICKUP_02);
		}
		else if(ADC_KIT_PICKER == nKitInfo)
		{
			if(!IsReadyMtInPnpZOverride(PZ_ADC_RAIL_PICKER))
				MoveMtInPnpZOverride(PZ_ADC_RAIL_PICKER);
			else if(!m_pMtW->InPos(PW_ADC_PICKER_PICKUP))
				m_pMtW->Move(PW_ADC_PICKER_PICKUP);
			else
				m_fsm.Set(C_ADC_RAIL_PICKUP_02);
		}
		break;
	case C_ADC_RAIL_PICKUP_02:
		if(!m_pMtZ->InPos(PZ_READY))
			m_pMtZ->Move(PZ_READY);
		else if(!g_adc.m_pMtX->InPos(CAdc::PX_READY))
			g_adc.m_pMtX->Move(CAdc::PX_READY);
		else if(!g_dIn.AOn(iInPnpKitExist))
			m_fsm.Set(C_ERROR, ER_IN_PNP_KIT_EXIST_NOT_ON);
		else if(g_dIn.AOn(iAdcKitGripperExist))
			m_fsm.Set(C_ERROR, ER_ADC_GRIPPER_EXIST_NOT_OFF);
		else
			m_fsm.Set(C_ADC_RAIL_PICKUP_END);
		break;

	case C_ADC_RAIL_PICKUP_END:
		KitInfo()    = g_adc.KitInfo();
		KitJobType() = g_adc.KitJobType();
		ExistKit()   = TRUE;

		g_adc.KitInfo()    = ADC_KIT_IDLE;
		g_adc.KitJobType() = JOB_TYPE_IDLE;
		g_adc.Exist()      = FALSE;
		g_adc.AdcState()   = ADC_RAIL_IDLE;

		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CInPnp::CycleRunAdcRailPutDn(void)
{
	if(!m_fsm.Between(C_ADC_RAIL_PUTDN_START, C_ADC_RAIL_PUTDN_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_IN_PNP_ADC_RAIL_PUTDN_CYCLE_TM_OVER);
		return;
	}

	int nKitInfo = m_fsm.GetMsg();

	if(!m_pMtW->IsRdy() || !m_pMtY->IsRdy() || !m_pMtZ->IsRdy())
		return;

	switch(m_fsm.Get())
	{
	case C_ADC_RAIL_PUTDN_START:
		if(!m_pMtZ->InPos(PZ_READY))
			m_pMtZ->Move(PZ_READY);
		else if(!m_pMtY->InPos(PY_ADC_RAIL))
			m_pMtY->Move(PY_ADC_RAIL);
		else
		{
			if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
			{
				if(!m_pMtW->InPos(PW_ADC_STAGE_PICKUP))
					m_pMtW->Move(PW_ADC_STAGE_PICKUP);
				else
					m_fsm.Set(C_ADC_RAIL_PUTDN_01);
			}
			else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
			{
				if(!m_pMtW->InPos(PW_ADC_MASK_PICKUP))
					m_pMtW->Move(PW_ADC_MASK_PICKUP);
				else
					m_fsm.Set(C_ADC_RAIL_PUTDN_01);
			}
			else if(ADC_KIT_PICKER == nKitInfo)
			{
				if(!m_pMtW->InPos(PW_ADC_PICKER_PICKUP))
					m_pMtW->Move(PW_ADC_PICKER_PICKUP);
				else
					m_fsm.Set(C_ADC_RAIL_PUTDN_01);
			}
		}
		break;

	case C_ADC_RAIL_PUTDN_01:
		if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
		{
			if(!IsReadyMtInPnpZOverride(PZ_ADC_RAIL_STAGE))
				MoveMtInPnpZOverride(PZ_ADC_RAIL_STAGE);
			else
				m_fsm.Set(C_ADC_RAIL_PUTDN_02);
		}
		else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
		{
			if(!IsReadyMtInPnpZOverride(PZ_ADC_RAIL_MASK))
				MoveMtInPnpZOverride(PZ_ADC_RAIL_MASK);
			else
				m_fsm.Set(C_ADC_RAIL_PUTDN_02);
		}
		else if(ADC_KIT_PICKER == nKitInfo)
		{
			if(!IsReadyMtInPnpZOverride(PZ_ADC_RAIL_PICKER))
				MoveMtInPnpZOverride(PZ_ADC_RAIL_PICKER);
			else
				m_fsm.Set(C_ADC_RAIL_PUTDN_02);
		}
		break;

	case C_ADC_RAIL_PUTDN_02:
		if(!m_pMtW->InPos(PW_ADC_OPEN))
			m_pMtW->Move(PW_ADC_OPEN);
		else if(!m_pMtZ->InPos(PZ_READY))
			m_pMtZ->Move(PZ_READY);
		else if(g_dIn.AOn(iInPnpKitExist))
			m_fsm.Set(C_ERROR, ER_IN_PNP_KIT_EXIST_NOT_OFF);
		//else if(!g_dIn.AOn(iAdcKitGripperExist)) // Gripper에 내려 놓은 것이 아님
		//	m_fsm.Set(C_ERROR, ER_ADC_GRIPPER_EXIST_NOT_ON);
		else
			m_fsm.Set(C_ADC_RAIL_PUTDN_END);
		break;

	case C_ADC_RAIL_PUTDN_END:
		// 센서가 없으므로 Exist 만 On
		// ADC Part에서 Exist 예외처리 필요 임시
		g_adc.KitInfo()    = KitInfo();
		g_adc.KitJobType() = KitJobType();
		g_adc.Exist()	   = TRUE;
		g_adc.AdcState()   = ADC_RAIL_PUSH;

		KitInfo()	= ADC_KIT_IDLE;
		KitJobType()= JOB_TYPE_IDLE;
		ExistKit()	= FALSE;

		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CInPnp::CycleRunAdcIndexPickUp(void)
{
	if(!m_fsm.Between(C_ADC_INDEX_PICKUP_START, C_ADC_INDEX_PICKUP_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_IN_PNP_ADC_INDEX_PICKUP_CYCLE_TM_OVER);
		return;
	}

	int nKitInfo = m_fsm.GetMsg();

	if(!m_pMtW->IsRdy() || !m_pMtY->IsRdy() || !m_pMtZ->IsRdy())
		return;

	if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
	{
		int nIdx = nKitInfo - ADC_KIT_STAGE_01;
		if(!g_pIndex[nIdx]->m_pMtX->IsRdy() || !g_pIndex[nIdx]->m_pMtX->IsRdy())
			return;
	}
	else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
	{
		int nIdx = nKitInfo - ADC_KIT_MASK_01;
		if(!g_pIndex[nIdx]->m_pMtX->IsRdy() || !g_pIndex[nIdx]->m_pMtX->IsRdy())
			return;
	}
	else if(ADC_KIT_PICKER == nKitInfo)
	{
		if(!g_pIndex[INDEX_01]->m_pMtX->IsRdy() || !g_pIndex[INDEX_01]->m_pMtX->IsRdy())
			return;
	}

	switch(m_fsm.Get())
	{
	case C_ADC_INDEX_PICKUP_START:
		if(!m_pMtW->InPos(PW_ADC_OPEN))
			m_pMtW->Move(PW_ADC_OPEN);
		else if(!m_pMtZ->InPos(PZ_READY))
			m_pMtZ->Move(PZ_READY);
		else
		{
			if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
			{
				// Stage 동작시 Mask는 없어야 함.
				int nIdx = nKitInfo - ADC_KIT_STAGE_01;
				int nPos = PY_ADC_STAGE_01 + nIdx;

				if(pmCLOSE != g_pIndex[nIdx]->m_pSolStageKitOC->GetPos(500))
					g_pIndex[nIdx]->m_pSolStageKitOC->Actuate(pmCLOSE);
				else if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
					g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
					g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				else if(g_pIndex[nIdx]->CylIndexMaskFixAct(pmOPEN))
				{
					if(!m_pMtY->InPos(nPos))
						m_pMtY->Move(nPos);
					else
						m_fsm.Set(C_ADC_INDEX_PICKUP_01);
				}
			}
			else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
			{
				int nIdx = nKitInfo - ADC_KIT_MASK_01;
				int nPos = PY_ADC_MASK_01 + nIdx;

				if(!g_pIndex[nIdx]->CylIndexMaskFixAct(pmCLOSE))
					break;
				else if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
					g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
					g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				else
				{
					if(!m_pMtY->InPos(nPos))
						m_pMtY->Move(nPos);
					else
						m_fsm.Set(C_ADC_INDEX_PICKUP_01);
				}
			}
			else if(ADC_KIT_PICKER == nKitInfo)
			{
				if(!g_pIndex[INDEX_01]->CylIndexMaskFixAct(pmCLOSE))
					break;
				else if(!g_pIndex[INDEX_01]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
					g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				else if(!g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
					g_pIndex[INDEX_01]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				else
				{
					if(!m_pMtY->InPos(PY_ADC_MASK_01))
						m_pMtY->Move(PY_ADC_MASK_01);
					else
						m_fsm.Set(C_ADC_INDEX_PICKUP_01);
				}
			}
		}
		break;
	case C_ADC_INDEX_PICKUP_01:
		if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
		{
			int nIdx = nKitInfo - ADC_KIT_STAGE_01;
			int nPos = PZ_ADC_STAGE_01 + nIdx;

			if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_STAGE))
				g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_STAGE);
			else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_STAGE))
				g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_STAGE);
			else if(pmOPEN != g_pIndex[nIdx]->m_pSolStageKitOC->GetPos(500))
				g_pIndex[nIdx]->m_pSolStageKitOC->Actuate(pmOPEN);
			else if(!IsReadyMtInPnpZOverride(nPos))
				MoveMtInPnpZOverride(nPos);
			else if(!m_pMtW->InPos(PW_ADC_STAGE_PICKUP))
				m_pMtW->Move(PW_ADC_STAGE_PICKUP);
			else 
			{
				m_pMtZ->Move(PZ_READY);
				m_fsm.Set(C_ADC_INDEX_PICKUP_02);
			}
		}
		else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
		{
			int nIdx = nKitInfo - ADC_KIT_MASK_01;
			int nPos = PZ_ADC_MASK_01 + nIdx;

			if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_MASK))
				g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_MASK);
			else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_MASK))
				g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_MASK);
			else if(!g_pIndex[nIdx]->CylIndexMaskFixAct(pmOPEN))
					break;
			else if(!IsReadyMtInPnpZOverride(nPos))
				MoveMtInPnpZOverride(nPos);
			else if(!m_pMtW->InPos(PW_ADC_MASK_PICKUP))
				m_pMtW->Move(PW_ADC_MASK_PICKUP);
			else 
			{
				m_pMtZ->Move(PZ_READY);
				m_fsm.Set(C_ADC_INDEX_PICKUP_02);
			}
		}
		else if(ADC_KIT_PICKER == nKitInfo)
		{
			if(!g_pIndex[INDEX_01]->m_pMtT->InPos(CIndex::PT_ADC_OUT_PICKER))
				g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ADC_OUT_PICKER);
			else if(!g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_OUT_PICKER))
				g_pIndex[INDEX_01]->m_pMtX->Move(CIndex::PX_ADC_OUT_PICKER);
			else if(!g_pIndex[INDEX_01]->CylIndexMaskFixAct(pmCLOSE))
					break;
			else if(!IsReadyMtInPnpZOverride(PZ_ADC_PICKER_INDEX))
				MoveMtInPnpZOverride(PZ_ADC_PICKER_INDEX);
			else if(!m_pMtW->InPos(PW_ADC_PICKER_PICKUP))
				m_pMtW->Move(PW_ADC_PICKER_PICKUP);
			else 
			{
				m_pMtZ->Move(PZ_READY);
				m_fsm.Set(C_ADC_INDEX_PICKUP_02);
			}
		}
		break;
	case C_ADC_INDEX_PICKUP_02:
		if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
		{
			int nIdx = nKitInfo - ADC_KIT_STAGE_01;

			if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
			{
				g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				break;
			}
			else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
			{
				g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				break;
			}
		}
		else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
		{
			int nIdx = nKitInfo - ADC_KIT_MASK_01;
			int nPos = PY_ADC_MASK_01 + nIdx;

			if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
			{
				g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				break;
			}
			else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
			{
				g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				break;
			}
		}
		else if(ADC_KIT_PICKER == nKitInfo)
		{
			if(!g_pIndex[INDEX_01]->CylIndexMaskFixAct(pmOPEN))
				break;

			if(!g_pIndex[INDEX_01]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
			{
				g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				break;
			}
			else if(!g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
			{
				g_pIndex[INDEX_01]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				break;
			}
		}

		ExistKit() = TRUE;

		if(ADC_KIT_STAGE_01 == nKitInfo)
		{
			g_pNV->NDm(existKitStage01) = FALSE;
			KitJobType() = g_pNV->NDm(adcIndex01StageJobType);
			KitInfo() = ADC_KIT_STAGE_01;
			g_pNV->NDm(adcIndex01StageJobType) = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_STAGE_02 == nKitInfo)
		{
			g_pNV->NDm(existKitStage02) = FALSE;
			KitJobType() = g_pNV->NDm(adcIndex02StageJobType);
			KitInfo() = ADC_KIT_STAGE_02;
			g_pNV->NDm(adcIndex02StageJobType) = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_STAGE_03 == nKitInfo)
		{
			g_pNV->NDm(existKitStage03) = FALSE;
			KitJobType() = g_pNV->NDm(adcIndex03StageJobType);
			KitInfo() = ADC_KIT_STAGE_03;
			g_pNV->NDm(adcIndex03StageJobType) = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_STAGE_04 == nKitInfo)
		{
			g_pNV->NDm(existKitStage04)  = FALSE;
			KitJobType() = g_pNV->NDm(adcIndex04StageJobType);
			KitInfo() = ADC_KIT_STAGE_04;
			g_pNV->NDm(adcIndex04StageJobType) = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_MASK_01 == nKitInfo)
		{
			g_pNV->NDm(existKitMask01) = FALSE;
			KitJobType() = g_pNV->NDm(adcIndex01MaskJobType);
			KitInfo() = ADC_KIT_MASK_01;
			g_pNV->NDm(adcIndex01MaskJobType) = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_MASK_02 == nKitInfo)
		{
			g_pNV->NDm(existKitMask02) = FALSE;
			KitJobType() = g_pNV->NDm(adcIndex02MaskJobType);
			KitInfo() = ADC_KIT_MASK_02;
			g_pNV->NDm(adcIndex02MaskJobType) = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_MASK_03 == nKitInfo)
		{
			g_pNV->NDm(existKitMask03) = FALSE;
			KitJobType() = g_pNV->NDm(adcIndex03MaskJobType);
			KitInfo() = ADC_KIT_MASK_03;
			g_pNV->NDm(adcIndex03MaskJobType) = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_MASK_04 == nKitInfo)
		{
			g_pNV->NDm(existKitMask04) = FALSE;
			KitJobType() = g_pNV->NDm(adcIndex04MaskJobType);
			KitInfo() = ADC_KIT_MASK_04;
			g_pNV->NDm(adcIndex04MaskJobType) = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_PICKER == nKitInfo)
		{
			g_pNV->NDm(existKitMovePicker) = FALSE;
			KitJobType() = g_pNV->NDm(adcIndexMovePickerJobType);
			KitInfo() = ADC_KIT_PICKER;
			g_pNV->NDm(adcIndexMovePickerJobType) = JOB_TYPE_IDLE;
		}

		m_fsm.Set(C_ADC_INDEX_PICKUP_END);
		break;
	case C_ADC_INDEX_PICKUP_END:
		{
			BOOL bSenOff = FALSE;
			
			if(ADC_KIT_STAGE_01 == nKitInfo)
				bSenOff = !g_dIn.AOn(iIndexStageKitExistL01) && !g_dIn.AOn(iIndexStageKitExistR01);
			else if(ADC_KIT_STAGE_02 == nKitInfo)
				bSenOff = !g_dIn.AOn(iIndexStageKitExistL02) && !g_dIn.AOn(iIndexStageKitExistR02);
			else if(ADC_KIT_STAGE_03 == nKitInfo)
				bSenOff = !g_dIn.AOn(iIndexStageKitExistL03) && !g_dIn.AOn(iIndexStageKitExistR03);
			else if(ADC_KIT_STAGE_04 == nKitInfo)
				bSenOff = !g_dIn.AOn(iIndexStageKitExistL04) && !g_dIn.AOn(iIndexStageKitExistR04);
			else if(ADC_KIT_MASK_01 == nKitInfo)
				bSenOff = !g_dIn.BOn(iIndexMaskKitExist01);
			else if(ADC_KIT_MASK_02 == nKitInfo)
				bSenOff = !g_dIn.BOn(iIndexMaskKitExist02);
			else if(ADC_KIT_MASK_03 == nKitInfo)
				bSenOff = !g_dIn.BOn(iIndexMaskKitExist03);
			else if(ADC_KIT_MASK_04 == nKitInfo)
				bSenOff = !g_dIn.BOn(iIndexMaskKitExist04);
			else if(ADC_KIT_PICKER == nKitInfo)
				bSenOff = !g_dIn.AOn(iIndex01OutPnpKitExist01) && !g_dIn.AOn(iIndex01OutPnpKitExist02);
				//bSenOff = !g_dIn.AOn(iIndex01OutPnpKitExist01);

			if(g_dIn.AOn(iInPnpKitExist) && bSenOff)
				m_fsm.Set(C_ADC_INDEX_PICKUP_END);
			else
			{
				if(m_fsm.TimeLimit(5000))
				{
					if(!g_dIn.AOn(iInPnpKitExist))
						m_fsm.Set(C_ERROR, ER_IN_PNP_KIT_EXIST_NOT_ON);
					else if(!bSenOff)
					{
						// 추후에 센서 알람으로 수정
						if(ADC_KIT_STAGE_01 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_STAGE_01);
						else if(ADC_KIT_STAGE_02 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_STAGE_02);
						else if(ADC_KIT_STAGE_03 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_STAGE_03);
						else if(ADC_KIT_STAGE_04 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_STAGE_04);
						else if(ADC_KIT_MASK_01 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_MASK_01);
						else if(ADC_KIT_MASK_02 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_MASK_02);
						else if(ADC_KIT_MASK_03 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_MASK_03);
						else if(ADC_KIT_MASK_04 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_MASK_04);
						else if(ADC_KIT_PICKER == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_INDEX_01_PICKER_EXIST);
					}
				}
			}

			m_fsm.Set(C_IDLE);
		}
		break;
	}
}


//-------------------------------------------------------------------
void CInPnp::CycleRunAdcIndexPutDn(void)
{
	if(!m_fsm.Between(C_ADC_INDEX_PUTDN_START, C_ADC_INDEX_PUTDN_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_IN_PNP_ADC_INDEX_PUTDN_CYCLE_TM_OVER);
		return;
	}

	int nKitInfo = m_fsm.GetMsg();

	if(!m_pMtW->IsRdy() || !m_pMtY->IsRdy() || !m_pMtZ->IsRdy())
		return;

	if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
	{
		int nIdx = nKitInfo - ADC_KIT_STAGE_01;
		if(!g_pIndex[nIdx]->m_pMtX->IsRdy() || !g_pIndex[nIdx]->m_pMtX->IsRdy())
			return;
	}
	else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
	{
		int nIdx = nKitInfo - ADC_KIT_MASK_01;
		if(!g_pIndex[nIdx]->m_pMtX->IsRdy() || !g_pIndex[nIdx]->m_pMtX->IsRdy())
			return;
	}
	else if(ADC_KIT_PICKER == nKitInfo)
	{
		if(!g_pIndex[INDEX_01]->m_pMtX->IsRdy() || !g_pIndex[INDEX_01]->m_pMtX->IsRdy())
			return;
	}

	switch(m_fsm.Get())
	{
	case C_ADC_INDEX_PUTDN_START:
		if(!m_pMtZ->InPos(PZ_READY))
			m_pMtZ->Move(PZ_READY);
		else
		{
			if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
			{
				int nIdx = nKitInfo - ADC_KIT_STAGE_01;
				int nPos = PY_ADC_STAGE_01 + nIdx;

				if(pmOPEN != g_pIndex[nIdx]->m_pSolStageKitOC->GetPos(500))
					g_pIndex[nIdx]->m_pSolStageKitOC->Actuate(pmOPEN);
				else if(!m_pMtW->InPos(PW_ADC_STAGE_PICKUP))
					m_pMtW->Move(PW_ADC_STAGE_PICKUP);
				else if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
					g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
					g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				else if(g_pIndex[nIdx]->CylIndexMaskFixAct(pmOPEN))
				{
					if(!m_pMtY->InPos(nPos))
						m_pMtY->Move(nPos);
					else
						m_fsm.Set(C_ADC_INDEX_PUTDN_01);
				}
			}
			else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
			{
				int nIdx = nKitInfo - ADC_KIT_MASK_01;
				int nPos = PY_ADC_MASK_01 + nIdx;

				if(!g_pIndex[nIdx]->CylIndexMaskFixAct(pmOPEN))
					break;
				else if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
					g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
					g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				else if(!m_pMtY->InPos(nPos))
					m_pMtY->Move(nPos);
				else
					m_fsm.Set(C_ADC_INDEX_PUTDN_01);
			}
			else if(ADC_KIT_PICKER == nKitInfo)
			{
				if(!g_pIndex[INDEX_01]->CylIndexMaskFixAct(pmCLOSE))
					break;
				else if(!g_pIndex[INDEX_01]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
					g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				else if(!g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
					g_pIndex[INDEX_01]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				else
				{
					if(!m_pMtY->InPos(PY_ADC_MASK_01))
						m_pMtY->Move(PY_ADC_MASK_01);
					else
						m_fsm.Set(C_ADC_INDEX_PUTDN_01);
				}
			}
		}
		break;
	case C_ADC_INDEX_PUTDN_01:
		if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
		{
			int nIdx = nKitInfo - ADC_KIT_STAGE_01;
			int nPos = PZ_ADC_STAGE_01 + nIdx;

			if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_STAGE))
				g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_STAGE);
			else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_STAGE))
				g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_STAGE);
			else if(!IsReadyMtInPnpZOverride(nPos))
				MoveMtInPnpZOverride(nPos);
			else if(!m_pMtW->InPos(PW_ADC_OPEN))
				m_pMtW->Move(PW_ADC_OPEN);
			else if(pmCLOSE != g_pIndex[nIdx]->m_pSolStageKitOC->GetPos(500))
				g_pIndex[nIdx]->m_pSolStageKitOC->Actuate(pmCLOSE);
			else
			{
				m_pMtZ->Move(PZ_READY);
				m_fsm.Set(C_ADC_INDEX_PUTDN_02);
			}
		}
		else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
		{
			int nIdx = nKitInfo - ADC_KIT_MASK_01;
			int nPos = PZ_ADC_MASK_01 + nIdx;

			if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_MASK))
				g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_MASK);
			else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_MASK))
				g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_MASK);
			else if(!IsReadyMtInPnpZOverride(nPos))
				MoveMtInPnpZOverride(nPos);
			else if(!m_pMtW->InPos(PW_ADC_OPEN))
				m_pMtW->Move(PW_ADC_OPEN);
			else
			{
				m_pMtZ->Move(PZ_READY);
				m_fsm.Set(C_ADC_INDEX_PUTDN_02);
			}

		}
		else if(ADC_KIT_PICKER == nKitInfo)
		{
			if(!g_pIndex[INDEX_01]->m_pMtT->InPos(CIndex::PT_ADC_OUT_PICKER))
				g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ADC_OUT_PICKER);
			else if(!g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_OUT_PICKER))
				g_pIndex[INDEX_01]->m_pMtX->Move(CIndex::PX_ADC_OUT_PICKER);
			else if(!IsReadyMtInPnpZOverride(PZ_ADC_PICKER_INDEX))
				MoveMtInPnpZOverride(PZ_ADC_PICKER_INDEX);
			else if(!m_pMtW->InPos(PW_ADC_OPEN))
				m_pMtW->Move(PW_ADC_OPEN);
			//임시 미리 닫았음
			//else if(!g_pIndex[INDEX_01]->CylIndexMaskFixAct(pmCLOSE))
			//		break;
			else
			{
				m_pMtZ->Move(PZ_READY);
				m_fsm.Set(C_ADC_INDEX_PUTDN_02);
			}
		}
		break;
	case C_ADC_INDEX_PUTDN_02:
		{
			BOOL bSenOn = FALSE;
			
			if(ADC_KIT_STAGE_01 == nKitInfo)
				bSenOn = g_dIn.AOn(iIndexStageKitExistL01) && g_dIn.AOn(iIndexStageKitExistR01);
			else if(ADC_KIT_STAGE_02 == nKitInfo)
				bSenOn = g_dIn.AOn(iIndexStageKitExistL02) && g_dIn.AOn(iIndexStageKitExistR02);
			else if(ADC_KIT_STAGE_03 == nKitInfo)
				bSenOn = g_dIn.AOn(iIndexStageKitExistL03) && g_dIn.AOn(iIndexStageKitExistR03);
			else if(ADC_KIT_STAGE_04 == nKitInfo)
				bSenOn = g_dIn.AOn(iIndexStageKitExistL04) && g_dIn.AOn(iIndexStageKitExistR04);
			else if(ADC_KIT_MASK_01 == nKitInfo)
				bSenOn = g_dIn.AOn(iIndexMaskKitExist01);
			else if(ADC_KIT_MASK_02 == nKitInfo)
				bSenOn = g_dIn.AOn(iIndexMaskKitExist02);
			else if(ADC_KIT_MASK_03 == nKitInfo)
				bSenOn = g_dIn.AOn(iIndexMaskKitExist03);
			else if(ADC_KIT_MASK_04 == nKitInfo)
				bSenOn = g_dIn.AOn(iIndexMaskKitExist04);
			else if(ADC_KIT_PICKER == nKitInfo)
				bSenOn = g_dIn.AOn(iIndex01OutPnpKitExist01) && g_dIn.AOn(iIndex01OutPnpKitExist02);
				//bSenOn = g_dIn.AOn(iIndex01OutPnpKitExist01);
				
			if(!g_dIn.AOn(iInPnpKitExist) && bSenOn)
				m_fsm.Set(C_ADC_INDEX_PUTDN_END);
			else
			{
				if(m_fsm.TimeLimit(3000))
				{
					if(g_dIn.AOn(iInPnpKitExist))
						m_fsm.Set(C_ERROR, ER_IN_PNP_KIT_EXIST_NOT_OFF);
					else if(!bSenOn)
					//if(!bSenOn)
					{
						// 추후에 센서 알람으로 수정
						if(ADC_KIT_STAGE_01 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_STAGE_01);
						else if(ADC_KIT_STAGE_02 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_STAGE_02);
						else if(ADC_KIT_STAGE_03 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_STAGE_03);
						else if(ADC_KIT_STAGE_04 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_STAGE_04);
						else if(ADC_KIT_MASK_01 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_MASK_01);
						else if(ADC_KIT_MASK_02 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_MASK_02);
						else if(ADC_KIT_MASK_03 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_MASK_03);
						else if(ADC_KIT_MASK_04 == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_KIT_EXIST_MASK_04);
						else if(ADC_KIT_PICKER == nKitInfo)
							m_fsm.Set(C_ERROR, ER_ADC_INDEX_01_PICKER_EXIST);
						break;
					}
				}
			}
		}
		break;
	case C_ADC_INDEX_PUTDN_END:
		if(Between(nKitInfo, ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
		{
			int nIdx = nKitInfo - ADC_KIT_STAGE_01;

			if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
			{
				g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				break;
			}
			else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
			{
				g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				break;
			}
		}
		else if(Between(nKitInfo, ADC_KIT_MASK_01, ADC_KIT_MASK_04))
		{
			int nIdx = nKitInfo - ADC_KIT_MASK_01;
			int nPos = PY_ADC_MASK_01 + nIdx;

			// Z축 Up이후에Index Fix Close
			if(!g_pIndex[nIdx]->CylIndexMaskFixAct(pmCLOSE))
					break;

			if(!g_pIndex[nIdx]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
			{
				g_pIndex[nIdx]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				break;
			}
			else if(!g_pIndex[nIdx]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
			{
				g_pIndex[nIdx]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				break;
			}
		}
		else if(ADC_KIT_PICKER == nKitInfo)
		{
			if(!g_pIndex[INDEX_01]->m_pMtT->InPos(CIndex::PT_ADC_WAIT))
			{
				g_pIndex[INDEX_01]->m_pMtT->Move(CIndex::PT_ADC_WAIT);
				break;
			}
			else if(!g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_WAIT))
			{
				g_pIndex[INDEX_01]->m_pMtX->Move(CIndex::PX_ADC_WAIT);
				break;
			}
		}
		
		ExistKit() = FALSE;

		if(ADC_KIT_STAGE_01 == nKitInfo)
		{
			g_pNV->NDm(existKitStage01) = TRUE;
			g_pNV->NDm(adcIndex01StageJobType) = KitJobType();
			KitInfo() = ADC_KIT_IDLE;
			KitJobType() = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_STAGE_02 == nKitInfo)
		{
			g_pNV->NDm(existKitStage02) = TRUE;
			g_pNV->NDm(adcIndex02StageJobType) = KitJobType();
			KitInfo() = ADC_KIT_IDLE;
			KitJobType() = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_STAGE_03 == nKitInfo)
		{
			g_pNV->NDm(existKitStage03) = TRUE;
			g_pNV->NDm(adcIndex03StageJobType) = KitJobType();
			KitInfo() = ADC_KIT_IDLE;
			KitJobType() = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_STAGE_04 == nKitInfo)
		{
			g_pNV->NDm(existKitStage04) = TRUE;
			g_pNV->NDm(adcIndex04StageJobType) = KitJobType();
			KitInfo() = ADC_KIT_IDLE;
			KitJobType() = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_MASK_01 == nKitInfo)
		{
			g_pNV->NDm(existKitMask01) = TRUE;
			g_pNV->NDm(adcIndex01MaskJobType) = KitJobType();
			KitInfo() = ADC_KIT_IDLE;
			KitJobType() = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_MASK_02 == nKitInfo)
		{
			g_pNV->NDm(existKitMask02) = TRUE;
			g_pNV->NDm(adcIndex02MaskJobType) = KitJobType();
			KitInfo() = ADC_KIT_IDLE;
			KitJobType() = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_MASK_03 == nKitInfo)
		{
			g_pNV->NDm(existKitMask03) = TRUE;
			g_pNV->NDm(adcIndex03MaskJobType) = KitJobType();
			KitInfo() = ADC_KIT_IDLE;
			KitJobType() = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_MASK_04 == nKitInfo)
		{
			g_pNV->NDm(existKitMask04) = TRUE;
			g_pNV->NDm(adcIndex04MaskJobType) = KitJobType();
			KitInfo() = ADC_KIT_IDLE;
			KitJobType() = JOB_TYPE_IDLE;
		}
		else if(ADC_KIT_PICKER == nKitInfo)
		{
			g_pNV->NDm(existKitMovePicker) = TRUE;
			g_pNV->NDm(adcIndexMovePickerJobType) = KitJobType();
			KitInfo() = ADC_KIT_IDLE;
			KitJobType() = JOB_TYPE_IDLE;
		}

		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
int	CInPnp::GetPcbPutDnIndex()
{
	 // 1,3,2,4, 순으로 이동되도록 수정
	int nResult = m_nPutDnIndex;

	if(INDEX_01 == m_nPutDnIndex)
	{
		if(!g_index01.IsReadyInPnp())
		{
			if(g_index03.IsReadyInPnp())
				nResult = INDEX_03;
			else if(g_index02.IsReadyInPnp())
				nResult = INDEX_02;
			else if(g_index04.IsReadyInPnp())
				nResult = INDEX_04;
		}
	}
	else if(INDEX_02 == m_nPutDnIndex)
	{
		if(!g_index02.IsReadyInPnp())
		{
			if(g_index04.IsReadyInPnp())
				nResult = INDEX_04;
			else if(g_index01.IsReadyInPnp())
				nResult = INDEX_01;
			else if(g_index03.IsReadyInPnp())
				nResult = INDEX_03;
		}
	}
	else if(INDEX_03 == m_nPutDnIndex)
	{
		if(!g_index03.IsReadyInPnp())
		{
			if(g_index02.IsReadyInPnp())
				nResult = INDEX_02;
			else if(g_index04.IsReadyInPnp())
				nResult = INDEX_04;
			else if(g_index01.IsReadyInPnp())
				nResult = INDEX_01;
		}
	}
	else if(INDEX_04 == m_nPutDnIndex)
	{
		if(!g_index04.IsReadyInPnp())
		{
			if(g_index01.IsReadyInPnp())
				nResult = INDEX_01;
			else if(g_index03.IsReadyInPnp())
				nResult = INDEX_03;
			else if(g_index02.IsReadyInPnp())
				nResult = INDEX_02;
		}
	}

	return (nResult);
}
