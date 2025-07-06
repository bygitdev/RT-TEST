#include "..\DEF\Includes.h"


/////////////////////////////////////////////////////////////////////
CAdc g_adc;
/////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
CAdc::CAdc()
{
	m_bRun			= FALSE;
	m_bRdyPickUp	= FALSE;
	m_bRdyPutDn		= FALSE;
}


//-------------------------------------------------------------------
void CAdc::AutoRun(void)
{
	m_bRdyPickUp	= FALSE;
	m_bRdyPutDn		= FALSE;

	if(!Exist() && !m_fsm.IsRun())
	{
		m_bRemove	 = FALSE;
		KitJobType() = JOB_TYPE_IDLE;
		KitInfo()	 = ADC_KIT_IDLE;
		AdcState()	 = ADC_RAIL_IDLE;
	}

	if(!m_bRun)
		return;

	m_bRun = FALSE;

	if(m_fsm.IsRun())
		return;

	if(g_inPnp.m_fsm.Between(CInPnp::C_ADC_RAIL_PICKUP_START, CInPnp::C_ADC_RAIL_PICKUP_END))
		return;
	if(g_inPnp.m_fsm.Between(CInPnp::C_ADC_RAIL_PUTDN_START, CInPnp::C_ADC_RAIL_PUTDN_END))
		return;

	if(g_inPnp.m_pMtY->ComparePos(CInPnp::PY_ADC_RAIL))
	{
		if(!g_inPnp.m_pMtZ->IsRdy(CInPnp::PZ_READY))
			return;
	}
	
	int existErrVal = GetExistErr();
	if(EXIST_UNCERTAIN == existErrVal)
		return;
	if(EXIST_ERR == existErrVal)
	{
		g_err.Save(ER_ADC_EXIST);
		return;
	}

	if(!g_opr.isDryRun)
	{
		if(g_pNV->NDm(mmiBtnAdcMode))
		{
			if(!g_dIn.AOn(iAdcKitMzExistTop))
			{
				g_err.Save(ER_ADC_MGZ_TOP_EXIST);
				return;
			}

			if(!g_dIn.AOn(iAdcKitMzExistBtm))
			{
				g_err.Save(ER_ADC_MGZ_BTM_EXIST);
				return;
			}
		}
	}

	if(m_bRemove)
	{
		g_err.Save(ER_ADC_RAIL_REMOVE_KIT);
		return;
	}

	BOOL bErr  = (AdcMzTopJobType() <= JOB_TYPE_IDLE);
		 bErr |= (AdcMzBtmJobType() <= JOB_TYPE_IDLE);
		 bErr |= (AdcMzTopJobType() == AdcMzBtmJobType());
		 bErr |= (g_pNV->Pkg(adcKitJobType) != AdcMzTopJobType() && g_pNV->Pkg(adcKitJobType) != AdcMzBtmJobType());

	if(bErr)
	{
		g_err.Save(ER_ADC_MGZ_JOB_TYPE_INCOLLECT); 
		return;
	}
	
	if(Exist())
	{
		if(!Between(AdcState(), ADC_RAIL_PICKUP, ADC_RAIL_PUSH))
		{
			g_err.Save(ER_ADC_RAIL_STATE_INFO_EMPTY);
			return;
		}
		if(!Between(KitInfo(), ADC_KIT_STAGE_01, ADC_KIT_PICKER))
		{
			g_err.Save(ER_ADC_RAIL_KIT_INFO_EMPTY);
			return;
		}
		if(KitJobType() < 1)
		{
			g_err.Save(ER_ADC_RAIL_KIT_JOB_TYPE_EMPTY);
			return;
		}
	}

	if(!m_pMtX->IsRdy() || !m_pMtZ->IsRdy())
		return;

	switch(GetState())
	{
	case S_IDLE:
		break;

	case S_READY:
		if(Exist())
		{
			// Adc Mode가 아니면 자재 제거
			g_err.Save(ER_ADC_RAIL_REMOVE);
		}
		else
		{
			if(!m_pMtX->InPos(PX_READY))
				m_pMtX->Move(PX_READY);
			else if(!m_pMtZ->InPos(PZ_READY))
				m_pMtZ->Move(PZ_READY);
		}
		break;
	case S_PUTDN_WAIT:
		if(!m_pMtX->InPos(PX_READY))
			m_pMtX->Move(PX_READY);
		else if(!m_pMtZ->InPos(PZ_PNP))
			m_pMtZ->Move(PZ_PNP);
		else
			m_bRdyPutDn = TRUE;
		break;
	case S_PICKUP_WAIT:
		if(!m_pMtX->InPos(PX_PNP_PICKUP))
			m_pMtX->Move(PX_PNP_PICKUP);
		else if(!m_pMtZ->InPos(PZ_PNP))
			m_pMtZ->Move(PZ_PNP);
		else
			m_bRdyPickUp = TRUE;
		break;
	case S_GRIP:
		{
			// Kit 간섭으로 Grip시에 Grip 하단에 Kit이 있으면 간섭 발생하여 문제 발생됨
			// Picker Grip은 하단에 아무것도 없으므로 Error 발생하지 않음
			int nGripNo = GetAdcMzGripNo();

			// 에러 개별로 적용 필요
			//if(Between(nGripNo, ADC_MZ_TOP_KIT_STAGE_01, ADC_MZ_TOP_KIT_MASK_04))
			//{
			//	int nNo = nGripNo - ADC_MZ_TOP_KIT_STAGE_01;
			//
			//	if(g_pNV->NDm(existAdcTopMzStage01 + (nNo + 1))) // 현재 Grip 하려는 Kit의 바로 아래것 Exist
			//	{
			//		g_err.Save(ER_ADC_MGZ_TOP_STAGE_01_GRIP_JAM + nNo);
			//		break;
			//	}
			//}
			//
			//if(Between(nGripNo, ADC_MZ_BTM_KIT_STAGE_01, ADC_MZ_BTM_KIT_MASK_04))
			//{
			//	int nNo = nGripNo - ADC_MZ_BTM_KIT_STAGE_01;
			//
			//	if(g_pNV->NDm(existAdcBtmMzStage01 + (nNo + 1))) // 현재 Grip 하려는 Kit의 바로 아래것 Exist
			//	{
			//		g_err.Save(ER_ADC_MGZ_BTM_STAGE_01_GRIP_JAM + nNo);
			//		break;
			//	}
			//}

			if(!m_pMtX->InPos(PX_READY))
				m_pMtX->Move(PX_READY);
			else if(IsAdcMzZPosMove(nGripNo, POS_TYPE_ALIGN))
				m_fsm.Set(C_GRIP_START, nGripNo);
		}
		break;
	case S_PUSH:
		{
			if(!m_pMtX->InPos(PX_READY))
			{
				m_pMtX->Move(PX_READY);
				break;
			}

			double dPos = 0.0;

			if(AdcMzTopJobType() == KitJobType())
			{
				if(Between(KitInfo(), ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
				{
					// Kit No가 아니라 빈곳에 넣을 수 있도록 한다.
					// Kit을 사람이 바꿀 수 있음
					for(int nNo = 0; nNo < 4; nNo++)
					{
						if(!g_pNV->NDm(existAdcTopMzStage01 + nNo))
						{
							dPos = GetAdcMzSlotGripUpPos(ADC_MZ_TOP_KIT_STAGE_01 + nNo);

							if(!m_pMtZ->InPos(PZ_TOP_STAGE_GRIP_UP_1ST, dPos, 50))
								m_pMtZ->PMove(PZ_TOP_STAGE_GRIP_UP_1ST, dPos);
							else 
								m_fsm.Set(C_PUSH_START, ADC_MZ_TOP_KIT_STAGE_01 + nNo);

							break;
						}
					}
				}
				else if(Between(KitInfo(), ADC_KIT_MASK_01, ADC_KIT_MASK_04))
				{
					for(int nNo = 0; nNo < 4; nNo++)
					{
						if(!g_pNV->NDm(existAdcTopMzMask01 + nNo))
						{
							dPos = GetAdcMzSlotGripUpPos(ADC_MZ_TOP_KIT_MASK_01 + nNo);

							if(!m_pMtZ->InPos(PZ_TOP_MASK_GRIP_UP_1ST, dPos, 50))
								m_pMtZ->PMove(PZ_TOP_MASK_GRIP_UP_1ST, dPos);
							else 
								m_fsm.Set(C_PUSH_START, ADC_MZ_TOP_KIT_MASK_01 + nNo);

							break;
						}
					}
				}
				else if(ADC_KIT_PICKER == KitInfo())
				{
					if(!g_pNV->NDm(existAdcTopMzPicker))
					{
						if(!m_pMtZ->InPos(PZ_TOP_PICKER_GRIP_UP))
							m_pMtZ->Move(PZ_TOP_PICKER_GRIP_UP);
						else 
							m_fsm.Set(C_PUSH_START, ADC_MZ_TOP_KIT_PICKER);
					}
				}
			}
			else if(AdcMzBtmJobType() == KitJobType())
			{
				if(Between(KitInfo(), ADC_KIT_STAGE_01, ADC_KIT_STAGE_04))
				{
					// Kit No가 아니라 빈곳에 넣을 수 있도록 한다.
					// Kit을 사람이 바꿀 수 있음
					for(int nNo = 0; nNo < 4; nNo++)
					{
						if(!g_pNV->NDm(existAdcBtmMzStage01 + nNo))
						{
							dPos = GetAdcMzSlotGripUpPos(ADC_MZ_BTM_KIT_STAGE_01 + nNo);
							if(!m_pMtZ->InPos(PZ_BTM_STAGE_GRIP_UP_1ST, dPos, 50))
								m_pMtZ->PMove(PZ_BTM_STAGE_GRIP_UP_1ST, dPos);
							else 
								m_fsm.Set(C_PUSH_START, ADC_MZ_BTM_KIT_STAGE_01 + nNo);

							break;
						}
					}
				}
				else if(Between(KitInfo(), ADC_KIT_MASK_01, ADC_KIT_MASK_04))
				{
					for(int nNo = 0; nNo < 4; nNo++)
					{
						if(!g_pNV->NDm(existAdcBtmMzMask01 + nNo))
						{
							dPos = GetAdcMzSlotGripUpPos(ADC_MZ_BTM_KIT_MASK_01 + nNo);
							if(!m_pMtZ->InPos(PZ_BTM_MASK_GRIP_UP_1ST, dPos, 50))
								m_pMtZ->PMove(PZ_BTM_MASK_GRIP_UP_1ST, dPos);
							else 
								m_fsm.Set(C_PUSH_START, ADC_MZ_BTM_KIT_MASK_01 + nNo);

							break;
						}
					}
				}
				else if(ADC_KIT_PICKER == KitInfo())
				{
					if(!g_pNV->NDm(existAdcBtmMzPicker))
					{
						if(!m_pMtZ->InPos(PZ_BTM_PICKER_GRIP_UP))
							m_pMtZ->Move(PZ_BTM_PICKER_GRIP_UP);
						else 
							m_fsm.Set(C_PUSH_START, ADC_MZ_BTM_KIT_PICKER);
					}
				}
			}
		}
		break;
	case S_END:
		{
			// 기타 항목 초기화 후 ADC Mode 종료 수정필요
			// 종료 조건 확인
			// 확인 후 센서 조건도 추가
			if(!m_pMtX->InPos(PX_READY))
				m_pMtX->Move(PX_READY);
			else if(!m_pMtZ->InPos(PZ_READY))
				m_pMtZ->Move(PZ_READY);
			else
			{
				BOOL bEnd = TRUE;
				for(int nNo = 0; nNo < 4; nNo++)
				{
					// Index 에 Mask는 없어야 하고 Picker에 모두 존재해야 함
					bEnd &= (!g_pIndex[nNo]->ExistKitMask() && g_pIndex[nNo]->ExistKitMaskPicker()) && (g_pNV->Pkg(adcKitJobType) == g_pNV->NDm(adcIndex01MaskJobType + nNo));
					bEnd &= g_pIndex[nNo]->ExistKitStage() && (g_pNV->Pkg(adcKitJobType) == g_pNV->NDm(adcIndex01StageJobType + nNo));
				}
				bEnd &= g_outPnp.ExistKit() && (g_pNV->Pkg(adcKitJobType) == g_pNV->NDm(adcOutPnpJobType));
				bEnd &= !g_pIndex[INDEX_01]->ExistKitMovePicker();
				bEnd &= !g_inPnp.ExistKit();
				bEnd &= !g_adc.Exist();
				bEnd &= !g_inPnp.m_fsm.Between(COutPnp::C_ADC_PICKUP_START, COutPnp::C_ADC_PICKUP_END);
				bEnd &= !g_inPnp.m_fsm.Between(COutPnp::C_ADC_PUTDN_START, COutPnp::C_ADC_PUTDN_END);
				bEnd &= !g_outPnp.m_fsm.Between(CInPnp::C_ADC_INDEX_PICKUP_START, CInPnp::C_ADC_INDEX_PICKUP_END);
				bEnd &= !g_outPnp.m_fsm.Between(CInPnp::C_ADC_INDEX_PUTDN_START, CInPnp::C_ADC_INDEX_PUTDN_END);
				bEnd &= g_inPnp.m_pMtY->IsRdy(CInPnp::PY_RAIL);
				bEnd &= g_pIndex[INDEX_01]->m_pMtX->InPos(CIndex::PX_ADC_WAIT);
				bEnd &= g_pIndex[INDEX_02]->m_pMtX->InPos(CIndex::PX_ADC_WAIT);
				bEnd &= g_pIndex[INDEX_03]->m_pMtX->InPos(CIndex::PX_ADC_WAIT);
				bEnd &= g_pIndex[INDEX_04]->m_pMtX->InPos(CIndex::PX_ADC_WAIT);

				if(bEnd)
				{
					g_opr.isStop = TRUE;
					g_pNV->NDm(mmiBtnAdcMode) = FALSE;
					g_pNV->NDm(adcKitOldJobType) = (int)g_pNV->Pkg(adcKitJobType); // 완료시에만 Update
					
					g_wr.Save(WR_ADC_FINISH);
				}
			}
		}
		break;
	}
}


//-------------------------------------------------------------------
void CAdc::CycleRun(void)
{
	if(!m_fsm.IsRun())
		return;

	if(m_fsm.IsStop())
		return;

	if(C_ERROR == m_fsm.Get())
	{
		if(0 < m_fsm.GetMsg())
			g_err.Save(m_fsm.GetMsg());

		g_pNV->NDm(stateAdc1D) = STATE_IDLE;
		m_fsm.Set(C_IDLE);
		return;
	}
	else if(IsErr())
	{
		m_fsm.Set(C_ERROR);
		return;
	}

	CycleAlign();
	CycleGrip();
	CyclePush();
}


//-------------------------------------------------------------------
void CAdc::CycleAlign(void)
{
	if(!m_fsm.Between(C_ALIGN_START, C_ALIGN_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_ADC_ALIGN_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtX->IsRdy() || !m_pMtZ->IsRdy())
		return;

	switch(m_fsm.Get())
	{
	case C_ALIGN_START:
		m_fsm.Set(C_IDLE);
		break;

	case C_ALIGN_END:
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CAdc::CycleGrip(void)
{
	if(!m_fsm.Between(C_GRIP_START, C_GRIP_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_ADC_GRIP_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtX->IsRdy() || !m_pMtZ->IsRdy())
		return;

	int nGripNo = m_fsm.GetMsg();

	switch(m_fsm.Get())
	{
	case C_GRIP_START:
		if(!m_pMtX->InPos(PX_READY))
			m_pMtX->Move(PX_READY);
		else if(IsAdcMzZPosMove(nGripNo, POS_TYPE_ALIGN))
			m_fsm.Set(C_GRIP_01);
		break;
	case C_GRIP_01:
		if(!m_pMtX->InPos(PX_ALIGN))
			m_pMtX->Move(PX_ALIGN);
		else
			m_fsm.Set(C_GRIP_02);
		//else if(g_dIn.AOn(iAdcKitJutExist)) // Sensor 감지되지 않음
		//	m_fsm.Set(C_GRIP_02);
		//else
		//{
		//	// Error Slot Exist
		//	m_fsm.Set(C_ERROR, ER_ADC_TOP_MZ_NOT_EXIST_STAGE_01 + nGripNo);
		//	m_pMtX->Move(PX_READY);
		//	break;
		//}
		break;
	case C_GRIP_02:
		if(IsAdcMzZPosMove(nGripNo, POS_TYPE_GRIP_DN))
			m_fsm.Set(C_GRIP_03);
		break;
	case C_GRIP_03:
		if(!m_pMtX->InPos(PX_GRIP))
			m_pMtX->Move(PX_GRIP);
		else if(IsAdcMzZPosMove(nGripNo, POS_TYPE_GRIP_UP))
			m_fsm.Set(C_GRIP_04);
		break;
	case C_GRIP_04:
		if(!g_pNV->UseSkip(usAdc1DBarcode))
		{
			if(nGripNo == ADC_MZ_TOP_KIT_STAGE_01 || nGripNo == ADC_MZ_BTM_KIT_STAGE_01)
				KitInfo() = ADC_KIT_STAGE_01;
			else if(nGripNo == ADC_MZ_TOP_KIT_STAGE_02 || nGripNo == ADC_MZ_BTM_KIT_STAGE_02)
				KitInfo() = ADC_KIT_STAGE_02;
			else if(nGripNo == ADC_MZ_TOP_KIT_STAGE_03 || nGripNo == ADC_MZ_BTM_KIT_STAGE_03)
				KitInfo() = ADC_KIT_STAGE_03;
			else if(nGripNo == ADC_MZ_TOP_KIT_STAGE_04 || nGripNo == ADC_MZ_BTM_KIT_STAGE_04)
				KitInfo() = ADC_KIT_STAGE_04;
			else if(nGripNo == ADC_MZ_TOP_KIT_MASK_01|| nGripNo == ADC_MZ_BTM_KIT_MASK_01)
				KitInfo() = ADC_KIT_MASK_01;
			else if(nGripNo == ADC_MZ_TOP_KIT_MASK_02|| nGripNo == ADC_MZ_BTM_KIT_MASK_02)
				KitInfo() = ADC_KIT_MASK_02;
			else if(nGripNo == ADC_MZ_TOP_KIT_MASK_03|| nGripNo == ADC_MZ_BTM_KIT_MASK_03)
				KitInfo() = ADC_KIT_MASK_03;
			else if(nGripNo == ADC_MZ_TOP_KIT_MASK_04|| nGripNo == ADC_MZ_BTM_KIT_MASK_04)
				KitInfo() = ADC_KIT_MASK_04;
			else if(nGripNo == ADC_MZ_TOP_KIT_PICKER || nGripNo == ADC_MZ_BTM_KIT_PICKER)
				KitInfo() = ADC_KIT_PICKER;

			if(nGripNo == ADC_MZ_TOP_KIT_STAGE_01)
				g_pNV->NDm(existAdcTopMzStage01) = FALSE;
			else if(nGripNo == ADC_MZ_TOP_KIT_STAGE_02)
				g_pNV->NDm(existAdcTopMzStage02) = FALSE;
			else if(nGripNo == ADC_MZ_TOP_KIT_STAGE_03)
				g_pNV->NDm(existAdcTopMzStage03) = FALSE;
			else if(nGripNo == ADC_MZ_TOP_KIT_STAGE_04)
				g_pNV->NDm(existAdcTopMzStage04) = FALSE;
			else if(nGripNo == ADC_MZ_TOP_KIT_MASK_01)
				g_pNV->NDm(existAdcTopMzMask01) = FALSE;
			else if(nGripNo == ADC_MZ_TOP_KIT_MASK_02)
				g_pNV->NDm(existAdcTopMzMask02) = FALSE;
			else if(nGripNo == ADC_MZ_TOP_KIT_MASK_03)
				g_pNV->NDm(existAdcTopMzMask03) = FALSE;
			else if(nGripNo == ADC_MZ_TOP_KIT_MASK_04)
				g_pNV->NDm(existAdcTopMzMask04) = FALSE;
			else if(nGripNo == ADC_MZ_TOP_KIT_PICKER)
				g_pNV->NDm(existAdcTopMzPicker) = FALSE;
			else if(nGripNo == ADC_MZ_BTM_KIT_STAGE_01)
				g_pNV->NDm(existAdcBtmMzStage01) = FALSE;
			else if(nGripNo == ADC_MZ_BTM_KIT_STAGE_02)
				g_pNV->NDm(existAdcBtmMzStage02) = FALSE;
			else if(nGripNo == ADC_MZ_BTM_KIT_STAGE_03)
				g_pNV->NDm(existAdcBtmMzStage03) = FALSE;
			else if(nGripNo == ADC_MZ_BTM_KIT_STAGE_04)
				g_pNV->NDm(existAdcBtmMzStage04) = FALSE;
			else if(nGripNo == ADC_MZ_BTM_KIT_MASK_01)
				g_pNV->NDm(existAdcBtmMzMask01) = FALSE;
			else if(nGripNo == ADC_MZ_BTM_KIT_MASK_02)
				g_pNV->NDm(existAdcBtmMzMask02) = FALSE;
			else if(nGripNo == ADC_MZ_BTM_KIT_MASK_03)
				g_pNV->NDm(existAdcBtmMzMask03) = FALSE;
			else if(nGripNo == ADC_MZ_BTM_KIT_MASK_04)
				g_pNV->NDm(existAdcBtmMzMask04) = FALSE;
			else if(nGripNo == ADC_MZ_BTM_KIT_PICKER)
				g_pNV->NDm(existAdcBtmMzPicker) = FALSE;

			if(Between(nGripNo, ADC_MZ_TOP_KIT_STAGE_01, ADC_MZ_TOP_KIT_PICKER))
				KitJobType() = AdcMzTopJobType();
			else
				KitJobType() = AdcMzBtmJobType();				

			if(!m_pMtX->InPos(PX_PNP_PICKUP))
				m_pMtX->Move(PX_PNP_PICKUP);
			else
			{
				if(!g_dIn.AOn(iAdcKitGripperExist) || !g_dIn.AOn(iAdcRailKitExist)) // 2022.03.10 에러 위치 변경
					m_fsm.Set(C_ERROR, ER_ADC_RAIL_FAIL);
				else
				{
					AdcState() = ADC_RAIL_PICKUP;
					Exist()	   = TRUE;
					//m_pMtX->Move(PX_PNP_PICKUP);
					m_fsm.Set(C_GRIP_END);
				}
			}


			//AdcState() = ADC_RAIL_PICKUP;
			//Exist()	   = TRUE;
			//m_pMtX->Move(PX_PNP_PICKUP);
			//m_fsm.Set(C_GRIP_END);
			break;
		}
		else
		{
			if(!m_pMtX->InPos(PX_1D))
				m_pMtX->Move(PX_1D);
			else
			{
				Exist() = TRUE;
				g_pNV->NDm(stateAdc1D) = STATE_IDLE;
				m_fsm.Set(C_GRIP_05);
			}
		}
		break;	
	case C_GRIP_05:
		if(m_fsm.Once())
		{
			g_pNV->NDm(adc1DKitInfo) = ADC_KIT_IDLE;
			g_pNV->NDm(adc1DJobType) = JOB_TYPE_IDLE;
			if(!g_opr.isDryRun)
				g_pNV->NDm(stateAdc1D) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				// 자재 제거를 위한 Pos
				m_pMtX->Move(PX_PNP_PICKUP);
				m_bRemove = TRUE;
				m_fsm.Set(C_ERROR, ER_ADC_MGZ_1D_READ_TM_OVER);
				break;
			}

			if(g_opr.isDryRun)
			{
				if(!m_fsm.Delay(2000))
					break;

				m_fsm.Set(C_GRIP_END);
				break;
			}

			switch(g_pNV->NDm(stateAdc1D))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				if(nGripNo == ADC_MZ_TOP_KIT_STAGE_01)
					g_pNV->NDm(existAdcTopMzStage01) = FALSE;
				else if(nGripNo == ADC_MZ_TOP_KIT_STAGE_02)
					g_pNV->NDm(existAdcTopMzStage02) = FALSE;
				else if(nGripNo == ADC_MZ_TOP_KIT_STAGE_03)
					g_pNV->NDm(existAdcTopMzStage03) = FALSE;
				else if(nGripNo == ADC_MZ_TOP_KIT_STAGE_04)
					g_pNV->NDm(existAdcTopMzStage04) = FALSE;
				else if(nGripNo == ADC_MZ_TOP_KIT_MASK_01)
					g_pNV->NDm(existAdcTopMzMask01) = FALSE;
				else if(nGripNo == ADC_MZ_TOP_KIT_MASK_02)
					g_pNV->NDm(existAdcTopMzMask02) = FALSE;
				else if(nGripNo == ADC_MZ_TOP_KIT_MASK_03)
					g_pNV->NDm(existAdcTopMzMask03) = FALSE;
				else if(nGripNo == ADC_MZ_TOP_KIT_MASK_04)
					g_pNV->NDm(existAdcTopMzMask04) = FALSE;
				else if(nGripNo == ADC_MZ_TOP_KIT_PICKER)
					g_pNV->NDm(existAdcTopMzPicker) = FALSE;
				else if(nGripNo == ADC_MZ_BTM_KIT_STAGE_01)
					g_pNV->NDm(existAdcBtmMzStage01) = FALSE;
				else if(nGripNo == ADC_MZ_BTM_KIT_STAGE_02)
					g_pNV->NDm(existAdcBtmMzStage02) = FALSE;
				else if(nGripNo == ADC_MZ_BTM_KIT_STAGE_03)
					g_pNV->NDm(existAdcBtmMzStage03) = FALSE;
				else if(nGripNo == ADC_MZ_BTM_KIT_STAGE_04)
					g_pNV->NDm(existAdcBtmMzStage04) = FALSE;
				else if(nGripNo == ADC_MZ_BTM_KIT_MASK_01)
					g_pNV->NDm(existAdcBtmMzMask01) = FALSE;
				else if(nGripNo == ADC_MZ_BTM_KIT_MASK_02)
					g_pNV->NDm(existAdcBtmMzMask02) = FALSE;
				else if(nGripNo == ADC_MZ_BTM_KIT_MASK_03)
					g_pNV->NDm(existAdcBtmMzMask03) = FALSE;
				else if(nGripNo == ADC_MZ_BTM_KIT_MASK_04)
					g_pNV->NDm(existAdcBtmMzMask04) = FALSE;
				else if(nGripNo == ADC_MZ_BTM_KIT_PICKER)
					g_pNV->NDm(existAdcBtmMzPicker) = FALSE;

				if(g_pNV->NDm(adc1DJobType) != g_pNV->Pkg(adcKitJobType))
				{
					// 자재 제거를 위한 Pos
					m_pMtX->Move(PX_PNP_PICKUP);
					m_bRemove = TRUE;
					g_pNV->NDm(stateAdc1D) = STATE_IDLE;
					m_fsm.Set(C_ERROR, ER_ADC_RAIL_KIT_JOB_TYPE_DIFFERENT);
				}
				else
				{
					KitJobType() = g_pNV->NDm(adc1DJobType);
					KitInfo()    = g_pNV->NDm(adc1DKitInfo);
					AdcState()   = ADC_RAIL_PICKUP;
					g_pNV->NDm(stateAdc1D) = STATE_IDLE;
					g_pNV->NDm(adc1DJobType) = JOB_TYPE_IDLE;
					g_pNV->NDm(adc1DKitInfo) = ADC_KIT_IDLE;
					m_fsm.Set(C_GRIP_END);
				}
				break;
			case STATE_ERR:
				// 자재 제거를 위한 Pos
				m_pMtX->Move(PX_PNP_PICKUP);
				m_bRemove = TRUE;
				g_pNV->NDm(stateAdc1D) = STATE_IDLE;
				m_fsm.Set(C_ERROR, ER_ADC_MGZ_1D_READ_FAIL);
				break;
			}
		}
		break;
	case C_GRIP_06: // 1D Error 처리
		// 구조상 안나올 듯.
		break;
	case C_GRIP_END:
		if(!m_pMtX->InPos(PX_PNP_PICKUP))
			m_pMtX->Move(PX_PNP_PICKUP);
		else
			m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CAdc::CyclePush(void)
{
	if(!m_fsm.Between(C_PUSH_START, C_PUSH_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_ADC_PUSH_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtX->IsRdy() || !m_pMtZ->IsRdy())
		return;

	int	nMzKitNo = m_fsm.GetMsg();

	switch(m_fsm.Get())
	{
	case C_PUSH_START:
		if(!m_pMtX->InPos(PX_READY))
			m_pMtX->Move(PX_READY);
		else if(IsAdcMzZPosMove(nMzKitNo, POS_TYPE_ALIGN))
			m_fsm.Set(C_PUSH_01);
		break;
	case C_PUSH_01:
		if(m_fsm.Once())
			m_pMtX->Move(PX_ALIGN);
		else
		{
			// 센서는 Check // 2022.03.10
			if(!m_pMtX->InPos(PX_READY))
				m_pMtX->Move(PX_READY);
			else
				if(g_dIn.AOn(iAdcRailKitExist))
					m_fsm.Set(C_ERROR, ER_ADC_RAIL_FAIL);
				else
					m_fsm.Set(C_PUSH_END);

			//m_pMtX->Move(PX_READY);
			//m_fsm.Set(C_PUSH_END);
		}
		break;
	case C_PUSH_END:
		Exist() = FALSE;
		// ndm 순서 바뀌면 안됨. 
		g_pNV->NDm(existAdcTopMzStage01 + nMzKitNo) = TRUE;
		m_fsm.Set(C_IDLE);
		break;
	}
}


BOOL CAdc::IsReadyPickUp(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyPickUp);
	}
	else
	{
		BOOL isRdy  = !m_fsm.IsRun();
			 isRdy &= m_pMtX->IsRdy(PX_PNP_PICKUP) && m_pMtX->InPos(PX_PNP_PICKUP);
			 isRdy &= m_pMtZ->IsRdy(PZ_PNP) && m_pMtZ->InPos(PZ_PNP);
			 isRdy &= EXIST_NORMAL == GetExistErr();
			 isRdy &= (g_dIn.AOn(iAdcKitGripperExist) && Exist()) || g_opr.isDryRun;
			 isRdy &= Between(KitInfo(), ADC_KIT_STAGE_01, ADC_KIT_PICKER);
			 isRdy &= (JOB_TYPE_IDLE < g_adc.KitJobType());
			 isRdy &= (ADC_RAIL_PICKUP == g_adc.AdcState());

		return (isRdy);
	}
}


BOOL CAdc::IsReadyPutDn(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyPutDn);
	}
	else
	{
		BOOL isRdy  = !m_fsm.IsRun();
			 isRdy &= m_pMtX->IsRdy(PX_READY) && m_pMtX->InPos(PX_READY);
			 isRdy &= m_pMtZ->IsRdy(PZ_PNP) && m_pMtZ->InPos(PZ_PNP);
			 isRdy &= EXIST_NORMAL == GetExistErr();
			 isRdy &= (!g_dIn.AOn(iAdcKitGripperExist) && !Exist()) || g_opr.isDryRun;
		
		return (isRdy);
	}
}


//-------------------------------------------------------------------
int  CAdc::GetAdcIndexReturnNo()
{
	int nResult = ADC_KIT_IDLE;

	// 삽입시에는 상단부터
	if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndex01MaskJobType)) && (g_pNV->NDm(existKitMask01) || g_pNV->NDm(existKitMaskPicker01)))
		nResult = ADC_KIT_MASK_01;
	else if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndex02MaskJobType)) && (g_pNV->NDm(existKitMask02) || g_pNV->NDm(existKitMaskPicker02)))
		nResult = ADC_KIT_MASK_02;
	else if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndex03MaskJobType)) && (g_pNV->NDm(existKitMask03) || g_pNV->NDm(existKitMaskPicker03)))
		nResult = ADC_KIT_MASK_03;
	else if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndex04MaskJobType)) && (g_pNV->NDm(existKitMask04) || g_pNV->NDm(existKitMaskPicker04)))
		nResult = ADC_KIT_MASK_04;
	else if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndex01StageJobType)) && g_pNV->NDm(existKitStage01))
		nResult = ADC_KIT_STAGE_01;
	else if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndex02StageJobType)) && g_pNV->NDm(existKitStage02))
		nResult = ADC_KIT_STAGE_02;
	else if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndex03StageJobType)) && g_pNV->NDm(existKitStage03))
		nResult = ADC_KIT_STAGE_03;
	else if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndex04StageJobType)) && g_pNV->NDm(existKitStage04))
		nResult = ADC_KIT_STAGE_04;
	else if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcOutPnpJobType)) && g_pNV->NDm(existKitOutPnp))
		nResult = ADC_KIT_PICKER; // Outpnp가 Picker를 잡고 있을 때
	else if((g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcIndexMovePickerJobType)) && g_pNV->NDm(existKitMovePicker))
		nResult = ADC_KIT_PICKER; // Index가 Picker을 가지고 있을 때

	return (nResult);
}


//-------------------------------------------------------------------
int  CAdc::GetAdcMzGripNo()
{
	int nResult = ADC_MZ_KIT_IDLE;

	if(AdcMzTopJobType() == g_pNV->Pkg(adcKitJobType))
	{
		if(g_pNV->NDm(existAdcTopMzPicker))
			nResult = ADC_MZ_TOP_KIT_PICKER;
		else if(g_pNV->NDm(existAdcTopMzStage01))
			nResult = ADC_MZ_TOP_KIT_STAGE_01;
		else if(g_pNV->NDm(existAdcTopMzStage02))
			nResult = ADC_MZ_TOP_KIT_STAGE_02;
		else if(g_pNV->NDm(existAdcTopMzStage03))
			nResult = ADC_MZ_TOP_KIT_STAGE_03;
		else if(g_pNV->NDm(existAdcTopMzStage04))
			nResult = ADC_MZ_TOP_KIT_STAGE_04;
		else if(g_pNV->NDm(existAdcTopMzMask01))
			nResult = ADC_MZ_TOP_KIT_MASK_01;
		else if(g_pNV->NDm(existAdcTopMzMask02))
			nResult = ADC_MZ_TOP_KIT_MASK_02;
		else if(g_pNV->NDm(existAdcTopMzMask03))
			nResult = ADC_MZ_TOP_KIT_MASK_03;
		else if(g_pNV->NDm(existAdcTopMzMask04))
			nResult = ADC_MZ_TOP_KIT_MASK_04;
	}
	else if(AdcMzBtmJobType() == g_pNV->Pkg(adcKitJobType))
	{
		if(g_pNV->NDm(existAdcBtmMzPicker))
			nResult = ADC_MZ_BTM_KIT_PICKER;
		else if(g_pNV->NDm(existAdcBtmMzStage01))
			nResult = ADC_MZ_BTM_KIT_STAGE_01;
		else if(g_pNV->NDm(existAdcBtmMzStage02))
			nResult = ADC_MZ_BTM_KIT_STAGE_02;
		else if(g_pNV->NDm(existAdcBtmMzStage03))
			nResult = ADC_MZ_BTM_KIT_STAGE_03;
		else if(g_pNV->NDm(existAdcBtmMzStage04))
			nResult = ADC_MZ_BTM_KIT_STAGE_04;
		else if(g_pNV->NDm(existAdcBtmMzMask01))
			nResult = ADC_MZ_BTM_KIT_MASK_01;
		else if(g_pNV->NDm(existAdcBtmMzMask02))
			nResult = ADC_MZ_BTM_KIT_MASK_02;
		else if(g_pNV->NDm(existAdcBtmMzMask03))
			nResult = ADC_MZ_BTM_KIT_MASK_03;
		else if(g_pNV->NDm(existAdcBtmMzMask04))
			nResult = ADC_MZ_BTM_KIT_MASK_04;
	}

	return (nResult);
}


//-------------------------------------------------------------------
double CAdc::GetAdcMzSlotAlignPos(int nMzKitNo)
{
	double basePos    = 0.0;
	double slotOffset = 0.0;
	double targetPos  = 0.0;
	int nNo = 0;
	
	if(Between(nMzKitNo, ADC_MZ_TOP_KIT_STAGE_01, ADC_MZ_TOP_KIT_STAGE_04))
	{
		nNo = nMzKitNo - ADC_MZ_TOP_KIT_STAGE_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_TOP_STAGE_ALIGN_1ST];
		slotOffset = g_pNV->DDm(adcMzStageSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(Between(nMzKitNo, ADC_MZ_TOP_KIT_MASK_01, ADC_MZ_TOP_KIT_MASK_04))
	{
		nNo = nMzKitNo - ADC_MZ_TOP_KIT_MASK_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_TOP_MASK_ALIGN_1ST];
		slotOffset = g_pNV->DDm(adcMzMaskSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_STAGE_01, ADC_MZ_BTM_KIT_STAGE_04))
	{
		nNo = nMzKitNo - ADC_MZ_BTM_KIT_STAGE_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_BTM_STAGE_ALIGN_1ST];
		slotOffset = g_pNV->DDm(adcMzStageSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_MASK_01, ADC_MZ_BTM_KIT_MASK_04))
	{
		nNo = nMzKitNo - ADC_MZ_BTM_KIT_MASK_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_BTM_MASK_ALIGN_1ST];
		slotOffset = g_pNV->DDm(adcMzMaskSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(ADC_MZ_TOP_KIT_PICKER == nMzKitNo)
	{
		targetPos = m_pMtZ->m_pTable->pos[PZ_TOP_PICKER_ALIGN];
	}
	else if(ADC_MZ_BTM_KIT_PICKER == nMzKitNo)
	{
		targetPos = m_pMtZ->m_pTable->pos[PZ_BTM_PICKER_ALIGN];
	}
			
	return (targetPos);
}


//-------------------------------------------------------------------
double CAdc::GetAdcMzSlotGripDnPos(int nMzKitNo)
{
	double basePos    = 0.0;
	double slotOffset = 0.0;
	double targetPos  = 0.0;
	int nNo = 0;
	
	if(Between(nMzKitNo, ADC_MZ_TOP_KIT_STAGE_01, ADC_MZ_TOP_KIT_STAGE_04))
	{
		nNo = nMzKitNo - ADC_MZ_TOP_KIT_STAGE_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_TOP_STAGE_GRIP_DW_1ST];
		slotOffset = g_pNV->DDm(adcMzStageSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(Between(nMzKitNo, ADC_MZ_TOP_KIT_MASK_01, ADC_MZ_TOP_KIT_MASK_04))
	{
		nNo = nMzKitNo - ADC_MZ_TOP_KIT_MASK_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_TOP_MASK_GRIP_DW_1ST];
		slotOffset = g_pNV->DDm(adcMzMaskSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_STAGE_01, ADC_MZ_BTM_KIT_STAGE_04))
	{
		nNo = nMzKitNo - ADC_MZ_BTM_KIT_STAGE_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_BTM_STAGE_GRIP_DW_1ST];
		slotOffset = g_pNV->DDm(adcMzStageSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_MASK_01, ADC_MZ_BTM_KIT_MASK_04))
	{
		nNo = nMzKitNo - ADC_MZ_BTM_KIT_MASK_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_BTM_MASK_GRIP_DW_1ST];
		slotOffset = g_pNV->DDm(adcMzMaskSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(ADC_MZ_TOP_KIT_PICKER == nMzKitNo)
	{
		targetPos = m_pMtZ->m_pTable->pos[PZ_TOP_PICKER_GRIP_DW];
	}
	else if(ADC_MZ_BTM_KIT_PICKER == nMzKitNo)
	{
		targetPos = m_pMtZ->m_pTable->pos[PZ_BTM_PICKER_GRIP_DW];
	}
			
	return (targetPos);
}


//-------------------------------------------------------------------
double CAdc::GetAdcMzSlotGripUpPos(int nMzKitNo)
{
	double basePos    = 0.0;
	double slotOffset = 0.0;
	double targetPos  = 0.0;
	int nNo = 0;
	
	if(Between(nMzKitNo, ADC_MZ_TOP_KIT_STAGE_01, ADC_MZ_TOP_KIT_STAGE_04))
	{
		nNo = nMzKitNo - ADC_MZ_TOP_KIT_STAGE_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_TOP_STAGE_GRIP_UP_1ST];
		slotOffset = g_pNV->DDm(adcMzStageSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(Between(nMzKitNo, ADC_MZ_TOP_KIT_MASK_01, ADC_MZ_TOP_KIT_MASK_04))
	{
		nNo = nMzKitNo - ADC_MZ_TOP_KIT_MASK_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_TOP_MASK_GRIP_UP_1ST];
		slotOffset = g_pNV->DDm(adcMzMaskSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_STAGE_01, ADC_MZ_BTM_KIT_STAGE_04))
	{
		nNo = nMzKitNo - ADC_MZ_BTM_KIT_STAGE_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_BTM_STAGE_GRIP_UP_1ST];
		slotOffset = g_pNV->DDm(adcMzStageSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_MASK_01, ADC_MZ_BTM_KIT_MASK_04))
	{
		nNo = nMzKitNo - ADC_MZ_BTM_KIT_MASK_01;

		basePos    = m_pMtZ->m_pTable->pos[PZ_BTM_MASK_GRIP_UP_1ST];
		slotOffset = g_pNV->DDm(adcMzMaskSlotPitch) * (nNo) * 1000.0; 
		targetPos  = basePos + slotOffset;
	}
	else if(ADC_MZ_TOP_KIT_PICKER == nMzKitNo)
	{
		targetPos = m_pMtZ->m_pTable->pos[PZ_TOP_PICKER_GRIP_UP];
	}
	else if(ADC_MZ_BTM_KIT_PICKER == nMzKitNo)
	{
		targetPos = m_pMtZ->m_pTable->pos[PZ_BTM_PICKER_GRIP_UP];
	}
			
	return (targetPos);
}


//-------------------------------------------------------------------
BOOL CAdc::IsAdcMzZPosMove(int nMzKitNo, int nPosType)
{
	double dPosAlign = GetAdcMzSlotAlignPos(nMzKitNo);
	double dPosDn	 = GetAdcMzSlotGripDnPos(nMzKitNo);
	double dPosUp	 = GetAdcMzSlotGripUpPos(nMzKitNo);

	if(POS_TYPE_ALIGN == nPosType)
	{
		if(Between(nMzKitNo, ADC_MZ_TOP_KIT_STAGE_01, ADC_MZ_TOP_KIT_STAGE_04))
		{
			if(!m_pMtZ->InPos(PZ_TOP_STAGE_ALIGN_1ST, dPosAlign, 50))
				m_pMtZ->PMove(PZ_TOP_STAGE_ALIGN_1ST, dPosAlign);
			else
				return (TRUE);
		}
		else if(Between(nMzKitNo, ADC_MZ_TOP_KIT_MASK_01, ADC_MZ_TOP_KIT_MASK_04))
		{
			if(!m_pMtZ->InPos(PZ_TOP_MASK_ALIGN_1ST, dPosAlign, 50))
				m_pMtZ->PMove(PZ_TOP_MASK_ALIGN_1ST, dPosAlign);
			else
				return (TRUE);
		}
		else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_STAGE_01, ADC_MZ_BTM_KIT_STAGE_04))
		{
			if(!m_pMtZ->InPos(PZ_BTM_STAGE_ALIGN_1ST, dPosAlign, 50))
				m_pMtZ->PMove(PZ_BTM_STAGE_ALIGN_1ST, dPosAlign);
			else
				return (TRUE);
		}
		else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_MASK_01, ADC_MZ_BTM_KIT_MASK_04))
		{
			if(!m_pMtZ->InPos(PZ_BTM_MASK_ALIGN_1ST, dPosAlign, 50))
				m_pMtZ->PMove(PZ_BTM_MASK_ALIGN_1ST, dPosAlign);
			else
				return (TRUE);
		}
		else if(ADC_MZ_TOP_KIT_PICKER == nMzKitNo)
		{
			if(!m_pMtZ->InPos(PZ_TOP_PICKER_ALIGN))
				m_pMtZ->Move(PZ_TOP_PICKER_ALIGN);
			else 
				return (TRUE);
		}
		else if(ADC_MZ_BTM_KIT_PICKER == nMzKitNo)
		{
			if(!m_pMtZ->InPos(PZ_BTM_PICKER_ALIGN))
				m_pMtZ->Move(PZ_BTM_PICKER_ALIGN);
			else 
				return (TRUE);
		}
	}
	else if(POS_TYPE_GRIP_DN == nPosType)
	{
		if(Between(nMzKitNo, ADC_MZ_TOP_KIT_STAGE_01, ADC_MZ_TOP_KIT_STAGE_04))
		{
			if(!m_pMtZ->InPos(PZ_TOP_STAGE_GRIP_DW_1ST, dPosDn, 50))
				m_pMtZ->PMove(PZ_TOP_STAGE_GRIP_DW_1ST, dPosDn);
			else
				return (TRUE);
		}
		else if(Between(nMzKitNo, ADC_MZ_TOP_KIT_MASK_01, ADC_MZ_TOP_KIT_MASK_04))
		{
			if(!m_pMtZ->InPos(PZ_TOP_MASK_GRIP_DW_1ST, dPosDn, 50))
				m_pMtZ->PMove(PZ_TOP_MASK_GRIP_DW_1ST, dPosDn);
			else
				return (TRUE);
		}
		else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_STAGE_01, ADC_MZ_BTM_KIT_STAGE_04))
		{
			if(!m_pMtZ->InPos(PZ_BTM_STAGE_GRIP_DW_1ST, dPosDn, 50))
				m_pMtZ->PMove(PZ_BTM_STAGE_GRIP_DW_1ST, dPosDn);
			else
				return (TRUE);
		}
		else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_MASK_01, ADC_MZ_BTM_KIT_MASK_04))
		{
			if(!m_pMtZ->InPos(PZ_BTM_MASK_GRIP_DW_1ST, dPosDn, 50))
				m_pMtZ->PMove(PZ_BTM_MASK_GRIP_DW_1ST, dPosDn);
			else
				return (TRUE);
		}
		else if(ADC_MZ_TOP_KIT_PICKER == nMzKitNo)
		{
			if(!m_pMtZ->InPos(PZ_TOP_PICKER_GRIP_DW))
				m_pMtZ->Move(PZ_TOP_PICKER_GRIP_DW);
			else 
				return (TRUE);
		}
		else if(ADC_MZ_BTM_KIT_PICKER == nMzKitNo)
		{
			if(!m_pMtZ->InPos(PZ_BTM_PICKER_GRIP_DW))
				m_pMtZ->Move(PZ_BTM_PICKER_GRIP_DW);
			else 
				return (TRUE);
		}
	}
	else if(POS_TYPE_GRIP_UP == nPosType)
	{
		if(Between(nMzKitNo, ADC_MZ_TOP_KIT_STAGE_01, ADC_MZ_TOP_KIT_STAGE_04))
		{
			if(!m_pMtZ->InPos(PZ_TOP_STAGE_GRIP_UP_1ST, dPosUp, 50))
				m_pMtZ->PMove(PZ_TOP_STAGE_GRIP_UP_1ST, dPosUp);
			else
				return (TRUE);
		}
		else if(Between(nMzKitNo, ADC_MZ_TOP_KIT_MASK_01, ADC_MZ_TOP_KIT_MASK_04))
		{
			if(!m_pMtZ->InPos(PZ_TOP_MASK_GRIP_UP_1ST, dPosUp, 50))
				m_pMtZ->PMove(PZ_TOP_MASK_GRIP_UP_1ST, dPosUp);
			else
				return (TRUE);
		}
		else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_STAGE_01, ADC_MZ_BTM_KIT_STAGE_04))
		{
			if(!m_pMtZ->InPos(PZ_BTM_STAGE_GRIP_UP_1ST, dPosUp, 50))
				m_pMtZ->PMove(PZ_BTM_STAGE_GRIP_UP_1ST, dPosUp);
			else
				return (TRUE);
		}
		else if(Between(nMzKitNo, ADC_MZ_BTM_KIT_MASK_01, ADC_MZ_BTM_KIT_MASK_04))
		{
			if(!m_pMtZ->InPos(PZ_BTM_MASK_GRIP_UP_1ST, dPosUp, 50))
				m_pMtZ->PMove(PZ_BTM_MASK_GRIP_UP_1ST, dPosUp);
			else
				return (TRUE);
		}
		else if(ADC_MZ_TOP_KIT_PICKER == nMzKitNo)
		{
			if(!m_pMtZ->InPos(PZ_TOP_PICKER_GRIP_UP))
				m_pMtZ->Move(PZ_TOP_PICKER_GRIP_UP);
			else 
				return (TRUE);
		}
		else if(ADC_MZ_BTM_KIT_PICKER == nMzKitNo)
		{
			if(!m_pMtZ->InPos(PZ_BTM_PICKER_GRIP_UP))
				m_pMtZ->Move(PZ_BTM_PICKER_GRIP_UP);
			else 
				return (TRUE);
		}
	}
	return (FALSE);
}


//-------------------------------------------------------------------
void CAdc::Init(void)
{
	m_pMtX = &g_mt[MT_ADC_X];
	m_pMtZ = &g_mt[MT_ADC_Z];
}


//-------------------------------------------------------------------
int  CAdc::GetState(void)
{
	int nState = S_IDLE;

	if(g_pNV->NDm(mmiBtnAdcMode))
	{
		if(Exist())
		{
			if(ADC_RAIL_PICKUP == AdcState())
				nState = S_PICKUP_WAIT;
			else if(ADC_RAIL_PUSH == AdcState())
				nState = S_PUSH;
		}
		else
		{
			int nReturnNo = GetAdcIndexReturnNo();
			int nGripNo   = GetAdcMzGripNo();

			BOOL bExistInPnp  = g_pNV->Pkg(adcKitJobType) != g_pNV->NDm(adcInPnpKitJobType);
				 bExistInPnp &= g_inPnp.ExistKit();

			if(ADC_KIT_IDLE < nReturnNo || bExistInPnp) // 받아야 할 Kit이 있을 때
				nState = S_PUTDN_WAIT;
			else if(ADC_MZ_KIT_IDLE < nGripNo)
				nState = S_GRIP;
			else
				nState = S_END;
		}
	}
	else
	{
		nState = S_READY;
	}
	
	return (nState);
}


//-------------------------------------------------------------------
BOOL CAdc::IsErr(void)
{
	if(!m_pMtX->m_state.isHome)
		return (TRUE);
	if(!m_pMtZ->m_state.isHome)
		return (TRUE);

	return (FALSE);
}


//-------------------------------------------------------------------
int& CAdc::Exist(void)
{
	return (g_pNV->m_pData->ndm[existAdcRail]);
}


//-------------------------------------------------------------------
int& CAdc::KitJobType(void)
{
	return (g_pNV->m_pData->ndm[adcRailKitJobType]);
}


//-------------------------------------------------------------------
int& CAdc::KitInfo(void)
{
	return (g_pNV->m_pData->ndm[adcRailKitInfo]);
}


//-------------------------------------------------------------------
int& CAdc::AdcState(void)
{
	return (g_pNV->m_pData->ndm[adcRailState]);
}


//-------------------------------------------------------------------
int& CAdc::AdcMzTopJobType(void)  
{
	return (g_pNV->m_pData->ndm[adcMzTopJobType]);
}


//-------------------------------------------------------------------
int& CAdc::AdcMzBtmJobType(void)
{
	return (g_pNV->m_pData->ndm[adcMzBtmJobType]);
}


//-------------------------------------------------------------------
int CAdc::GetExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	BOOL isSenOn = g_dIn.AOn(iAdcKitGripperExist) || g_dIn.AOn(iAdcRailKitExist);

	// Rail의 Kit 을 MGZ에 진입시킬시에는 Pusher가 뒤에 위치하므로 Kit가 감지되지 않는다.
	// 센서 추가하여 수정완료
	//if(ADC_RAIL_PUSH == AdcState())
	//	isSenOn = Exist();
	
	if(isSenOn == Exist())
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


