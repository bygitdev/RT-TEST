#include "..\DEF\Includes.h"


/////////////////////////////////////////////////////////////////////
CRail g_rail;
/////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
CRail::CRail()
{
	m_bRun			= FALSE;
	m_bCompRcv		= FALSE;
	m_bCompPcbInfo	= FALSE;
	m_bCompLotSend  = FALSE;
	m_bComp2D		= FALSE;
	m_bRemove		= FALSE;
	m_bLotMgzFirst  = FALSE;
	m_bManualLotIn	= FALSE;
	m_bRdyLdMz		= FALSE;
	m_bRdyInPnp		= FALSE;
	m_nGripCnt		= 0;
	m_nTcRetryCnt	= 0;
}


//-------------------------------------------------------------------
void CRail::AutoRun(void)
{
	m_bRdyLdMz	= FALSE;
	m_bRdyInPnp	= FALSE;

	BOOL bCycleOff  = !g_ldMz.m_fsm.Between(CLdMz::C_PUSHER_START, CLdMz::C_PUSHER_END); 
		 bCycleOff &= !g_inPnp.m_fsm.Between(CInPnp::C_PCB_PICKUP_START, CInPnp::C_PCB_PICKUP_END); 
		 bCycleOff &= !m_fsm.IsRun(); 

	if(!Exist() && bCycleOff) 
	{
		m_bCompRcv	    = FALSE;
		m_bCompPcbInfo  = FALSE;
		m_bCompLotSend  = FALSE;
		m_bCompLotSplit = FALSE;
		m_bComp2D	    = FALSE;
		m_bRemove	    = FALSE;

		g_pNV->NDm(flagLotMergeComp) = FALSE;
	}

	if(!m_bRun)
		return;

	m_bRun = FALSE;

	int nErrCode = g_err.GetNo();
	if(0 < nErrCode)
		return;

	if(m_fsm.IsRun())
		return;

	if(g_ldMz.m_fsm.Between(CLdMz::C_PUSHER_START, CLdMz::C_PUSHER_END)) 
		return;
	if(g_inPnp.m_fsm.Between(CInPnp::C_PCB_PICKUP_START, CInPnp::C_PCB_PICKUP_END)) 
		return;

	int existErrVal = GetExistErr(); 
	if(EXIST_UNCERTAIN == existErrVal)
		return;
	if(EXIST_ERR == existErrVal) 
	{
		g_err.Save(ER_RAIL_EXIST);
		return;
	}

	if(!m_pMtGrip->IsRdy()) 
		return;

	if(pmOPEN != m_pCylGripOC->GetPos(300)) 
	{
		m_pCylGripOC->Actuate(pmOPEN);
		return;
	}

	if(g_pNV->Pkg(PcbLengthOptionUse) == 0)
	{
		if(pmBWD != m_pCylGripFB->GetPos(300))
		{
			m_pCylGripFB->Actuate(pmBWD); 
			return;
		}
	}

	switch(GetState())
	{
	case S_IDLE:
		break;
	case S_WAIT: 
		if(g_inPnp.m_pMtY->InPos(CInPnp::PY_RAIL)) 
		{
			if(!g_inPnp.m_pMtZ->InPos(CInPnp::PZ_READY)) 
				break;
		}

		if(pmOPEN != m_pCylGripOC->GetPos(300)) 
			m_pCylGripOC->Actuate(pmOPEN);
		else if(!IsReadyMtRailXRevStart()) // 2025.03
			MoveMtRailXRevStart();
		else 
			m_bRdyLdMz = TRUE; 
		break;
	case S_RCV: 
		if(g_inPnp.m_pMtY->InPos(CInPnp::PY_RAIL)) 
		{
			if(!g_inPnp.m_pMtZ->InPos(CInPnp::PZ_READY)) 
				break;
		}

		if (!IsReadyMtRailXRevStart()) // 2025.03
			MoveMtRailXRevStart(); // 2025.03
		else
			m_fsm.Set(C_RCV_START);
		break;
	case S_PCB_INFO:
		if(!g_pNV->UseSkip(usTcServer) || g_opr.isDryRun)
		{
			m_bCompPcbInfo = TRUE;
			break;
		}
		m_fsm.Set(C_PCB_INFO_START);
		break;
	case S_LOT_SEND:
		{
			if(!g_pNV->UseSkip(usTcServer) || g_opr.isDryRun)
			{
				m_bCompLotSend = TRUE;
				break;
			}

// 			if(!g_pNV->UseSkip(usArts) && !m_bManualLotIn)
// 			{
// 				if(!g_pNV->NDm(stateManualLotIn)) 
// 				{
// 					g_pNV->NDm(stateManualLotIn) = 1;
// 					g_err.Save(ER_LOT_INFO_NOT_EXIST_RAIL);
// 				}
// 				else if(3==g_pNV->NDm(stateManualLotIn))
// 				{
// 					m_bManualLotIn = TRUE;
// 					g_pNV->NDm(stateManualLotIn) = 0;
// 				}
// 			}

			if(NULL == g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].partID[0])
			{
				g_err.Save(ER_LOT_INFO_PART_ID_NOT_EXIST);
				break;
			}
			if(NULL == g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID[0])
			{
				g_err.Save(ER_LOT_INFO_MERGE_LOT_ID_NOT_EXIST);
				break;
			}

			if(m_bLotMgzFirst)
			{
				if(!g_lotInfo.PartIDComp(LOT_INFO_RAIL, LOT_INFO_OLD_RAIL))
				{
// 					if(!g_pNV->UseSkip(usRfid) && !g_pNV->UseSkip(usArts))
// 					{
// 						m_fsm.Set(C_LOT_START_END);
// 					}
// 					else
// 					{
						// 이전 Part No가 없거나 Part No가 다를 때.
						m_fsm.Set(C_LOT_START_START);
//					}
				}
				else
				{

					BOOL bCanLotStart = false; 
					bCanLotStart = g_lotInfo.MergeLotIDComp(LOT_INFO_OLD_RAIL, LOT_INFO_RAIL);
					// Part No가 같을 때
// 					int nSetMergeCnt = 0;
// 
// 					if(g_pNV->UseSkip(usRfid))
// 						nSetMergeCnt = g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].tray1ProdQty; // Tray Lot End 기준 수량
// 					else
// 						nSetMergeCnt = (int)g_pNV->DDm(lotMergeQty);
// 
// 					int nCurMergeCnt = 9999;
// 
// 					for(int nNo = (LOT_INFO_MAX_CNT-1); 0 <= nNo; nNo--) // 뒤에서 부터 조회
// 					{
// 						if(NULL != g_pNV->m_pLotInfo->history[nNo].mergeLotID[0])
// 						{
// 							nCurMergeCnt = g_pNV->m_pLotInfo->history[nNo].lotMergeCurCnt;
// 							break;
// 						}
// 					}
// 
// 					// 현재 작업하려는 PCB의 Lot수량 + nCurMergeCnt 의 수량이 합산하여 1200을 초과하는 경우 
// 					int nextLotMergeCurCnt = nCurMergeCnt + g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty;
// 					BOOL bCanMerge = FALSE;
// 
// 					if((0 < g_pNV->Pkg(optionLotSplit)) && g_pNV->UseSkip(usRfid))
// 					{
// 						int nXOutCnt = g_lotInfo.GetXOutCnt(LOT_INFO_RAIL);
// 
// 						// Lot Split 사용시에는 현재 merge하려는 수량의 최소 1장이상이 tray1ProdQty 보다 작아야 한다.
// 						bCanMerge  = (nCurMergeCnt + (int)g_pNV->Pkg(unitCnt) - nXOutCnt) <= nSetMergeCnt; // Tray 기준 수량
// 						bCanMerge &= nCurMergeCnt < nSetMergeCnt;        // Lot 기준 수량
// 					}
// 					else if(g_pNV->UseSkip(usRfid))
// 					{
// 						bCanMerge  = nextLotMergeCurCnt <= nSetMergeCnt; // Tray 기준 수량
// 						bCanMerge &= nCurMergeCnt < nSetMergeCnt;        // Lot 기준 수량
// 					}
// 					else
// 					{
// 						bCanMerge  = nextLotMergeCurCnt <= g_pNV->DDm(lotMergeLimitQty); // 1200 ea
// 						bCanMerge &= nCurMergeCnt < nSetMergeCnt; // 800 ea
// 					}

					// last lot 수량 확인
					// 만일 Split Lot ID가 존재하면 Split Lot ID로 Merge 한다.
					if(bCanLotStart) // 수량 미달
					{
						if(!g_pNV->UseSkip(usRfid) && !g_pNV->UseSkip(usArts))
						{
							m_fsm.Set(C_LOT_START_END);
						}
						else
						{
							// 이전 Part No가 없거나 Part No가 다를 때.
							m_fsm.Set(C_LOT_START_START);
						}
					}
					else
					{
						m_fsm.Set(C_LOT_START_START);
					}
				}
			}
			else
			{
				// 첫번째 Strip이 아닐 때만 Lot Split를 진행한다.
				// Lot Split 진행시에는 수량이 Merge 되지 않은 상태임
				// 이미 수량 합산은 완료된 상태
				int nResult = -1;
				for(int nNo = 0; nNo < LOT_INFO_MAX_CNT; nNo++)
				{
					// History에 있어야 하고 이전 Lot 정보와도 맞아야 함
					// History는 남아 있을 수 있음
					BOOL bCompOk  = (0 == _stricmp(g_pNV->m_pLotInfo->history[nNo].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID));
						 bCompOk &= g_lotInfo.MergeLotIDComp(LOT_INFO_OLD_RAIL, LOT_INFO_RAIL);

					if(bCompOk)
					{
						nResult = nNo;
						break;
					}
				}

				// Merge Lot 일 경우 Max Count는 Lot_Qty 이므로 수량 비교 필요함
				if(-1 == nResult)
				{
					g_err.Save(ER_LOT_INFO_MERGE_LOT_ID_DIFFERENT);
					break;
				}
 				else
 				{
// 					if((0 < g_pNV->Pkg(optionLotSplit)) && g_pNV->UseSkip(usRfid))
// 					{
// 						if(g_pNV->NDm(flagSplitIDLotStart))
// 						{
// 							// Split Lot ID가 있어야 함
// 							if(NULL == g_pNV->m_pLotInfo->split.lotID[0])
// 							{
// 								g_err.Save(ER_LOT_SPLIT_ID_EMPTY);
// 								break;
// 							}
// 
// 							if(g_pNV->NDm(lotSplitCount) <= 0)
// 							{
// 								g_err.Save(ER_LOT_SPLIT_COUNT_EMPTY);
// 								break;
// 							}
// 
// 							// 2D Info에서 수량 합산을 하지 않은 상태
// 							//Lot Split 완료시에 새로운 LotID로 History 추가한다.
// 							// 새로운 공간에 Lot 정보 입력하여 수량 Cnt
// 							int nXOutCnt = g_lotInfo.GetXOutCnt(LOT_INFO_RAIL);
// 							
// 							for(int nNo = 0; nNo < LOT_INFO_MAX_CNT; nNo++)
// 							{
// 								if(NULL == g_pNV->m_pLotInfo->history[nNo].mergeLotID[0])
// 								{
// 									// Split ID를 Rail단의 정보에 입력
// 									// 이후 부터는 ndm을 살려 MMI에서 바꿔치기함
// 									strcpy(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID, g_pNV->m_pLotInfo->split.lotID);
// 									g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty = g_pNV->NDm(lotSplitCount);
// 
// 									strcpy(g_pNV->m_pLotInfo->history[nNo].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID);
// 									strcpy(g_pNV->m_pLotInfo->history[nNo].partID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].partID);
// 									g_pNV->m_pLotInfo->history[nNo].lotMergeCurCnt = (int)g_pNV->Pkg(unitCnt) - nXOutCnt; // 첫 수량 입력
// 									g_pNV->NDm(productCountDay) += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
// 					
// 									// MGZ 정보 추가 (초기 Data 입력)
// 									// RFID Mode Skip 일 때는 MGZ ID가 없으므로 Lot ID를 MGZ Id에 기입하도록 한다. 어쩔수 없음
// 									if(g_pNV->UseSkip(usRfid))
// 										strcpy(g_pNV->m_pLotInfo->history[nNo].mgzInfo[0].carrierID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].carrierID);
// 									else
// 										strcpy(g_pNV->m_pLotInfo->history[nNo].mgzInfo[0].carrierID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotID);
// 
// 									strcpy(g_pNV->m_pLotInfo->history[nNo].mgzInfo[0].lotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotID);
// 									g_pNV->m_pLotInfo->history[nNo].mgzInfo[0].pcbProductCnt = (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
// 									break;
// 								}
// 							}							
// 							
// 							g_pNV->NDm(flagSplitInfo) = TRUE; // mmi에 Split Lot ID와 수량을 Rail 정보에 기입 할 수 있도록 수정
// 							g_pNV->NDm(flagSplitIDLotStart) = FALSE;
// 							m_bCompLotSend = TRUE;
// 						}
// 						else
// 							m_bCompLotSend = TRUE;
// 					}
// 					else
 						m_bCompLotSend = TRUE;
 				}
			}
		}
		break;
	case S_LOT_SPLIT:
		{
			g_pNV->Pkg(optionLotSplit) = 0; // Lot Split 기능이 없음. 1Lot 1M/Z
			if((0 >= g_pNV->Pkg(optionLotSplit)))
			{
				g_pNV->NDm(flagLotMergeComp) = FALSE;
				m_bCompLotSplit = TRUE;
				break;
			}
			
			if((0 < g_pNV->Pkg(optionLotSplit)) && g_pNV->UseSkip(usRfid))
			{
				if(g_pNV->NDm(flagLotMergeComp))
				{
					int curLotQty = g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty;
					int curTrayQty = g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].tray1ProdQty;
		
					// 현재 PCB의 작업하려는 수량이 tray1ProdQty 를 초과하면 Lot Split 진행한다.
					// Lot Merge가 이루어진 이후에만 가능
					if(curTrayQty < curLotQty)
					{
						g_pNV->NDm(lotSplitCount) = curLotQty - curTrayQty;
						m_fsm.Set(C_LOT_SPLIT_START);
					}
					else
					{
						m_bCompLotSplit = TRUE;
						g_pNV->NDm(flagLotMergeComp) = FALSE;
					}
				}
				else
				{
					m_bCompLotSplit = TRUE;
					g_pNV->NDm(flagLotMergeComp) = FALSE;
				}
			}
			else
			{
				m_bCompLotSplit = TRUE;
				g_pNV->NDm(flagLotMergeComp) = FALSE;
			}
		}
		break;
	case S_PNP:
		if(!m_pMtGrip->InPos(PX_WAIT))
			m_pMtGrip->Move(PX_WAIT);
		else 
			m_bRdyInPnp = TRUE;
		break;
	}
}


//-------------------------------------------------------------------
void CRail::CycleRun(void)
{
	if(!m_fsm.IsRun())
		return;

	if(m_fsm.IsStop())
		return;

	if(C_ERROR == m_fsm.Get())
	{
		if(0 < m_fsm.GetMsg())
			g_err.Save(m_fsm.GetMsg());

		g_pNV->NDm(statePmsTop)   = STATE_IDLE;
		g_pNV->NDm(statePmsBtm)   = STATE_IDLE;
		g_pNV->NDm(stateLotStart) = STATE_IDLE;
		g_pNV->NDm(stateLotMerge) = STATE_IDLE;
		g_pNV->NDm(statePcbInfo)  = STATE_IDLE;
		g_pNV->NDm(stateLotSplit) = STATE_IDLE;

		m_fsm.Set(C_IDLE);
		return;
	}
	else if(IsErr())
	{
		m_fsm.Set(C_ERROR);
		return;
	}

	CycleRcv();
	CyclePcbInfo();
	CycleLotMerge();
	CycleLotStart();
	CycleLotSplit();
}


//-------------------------------------------------------------------
void CRail::CycleRcv(void)
{
	if(!m_fsm.Between(C_RCV_START, C_RCV_END))
		return;

	if(m_fsm.TimeLimit(300000)) // 5분
	{
		m_fsm.Set(C_ERROR, ER_RAIL_RCV_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtGrip->IsRdy())
		return;

	BOOL bOptionUse = false;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	_sprintf(cMaterialType, L"PCB");	
	if(C_RCV_2D_START < m_fsm.Get())
		mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode) + 1);
	else
		_sprintf(cMaterialId, L"$");

	_char cMaterialIdRail[_MAX_CHAR_SIZE_]; 

	switch(m_fsm.Get())
	{
	case C_RCV_START:
		if(g_pNV->UseSkip(usSecsGem)) 
			g_pNV->NDm(gemInRailPcbReadStart) = STATE_REQ;
		
		m_nGripCnt = 0; 
		m_n2DRetryCnt = 0;
		m_bComp2D  = FALSE; 
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRail.deviceId, L"MOVE_PCB", L"START", cMaterialId, cMaterialType, L"RAIL_RCV_START", L"RAIL_RCV_END"))
		m_fsm.Set(C_RCV_01);
		break;
	case C_RCV_01: 
		if(pmOPEN != m_pCylGripOC->GetPos(300)) 
		{
			m_pCylGripOC->Actuate(pmOPEN);

			if(!g_logChk.bFunction[m_pCylGripOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylGripOC->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"CYL_RAIL_GRIPPER_OPEN_READY", g_data2c.cEtc.start, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylGripOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
								g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmOPEN][pmON], L"'OFF'",
								g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmOPEN][pmOFF], L"'ON'"))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylGripOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylGripOC->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"CYL_RAIL_GRIPPER_OPEN_READY", g_data2c.cEtc.end, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylGripOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
								g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmOPEN][pmON], L"'ON'",
								g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmOPEN][pmOFF], L"'OFF'"))
			}
		}

		if (!IsReadyMtRailXRevStart()) // 2025.03
		{
			MoveMtRailXRevStart(); // 2025.03
			if(!g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_RCV_START])
			{
				g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_RCV_START] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"MT_RAIL_GRIPPER_X_MOVE_RCV_START", g_data2c.cEtc.start, 
					cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtGrip->m_config.axisNo], 
					g_data2c.cRail.X[PX_RCV_START][_POSIDX_], g_data2c.cRail.X[PX_RCV_START][_POS_], 
					g_data2c.cRail.X[PX_RCV_START][_SPDIDX_], g_data2c.cRail.X[PX_RCV_START][_SPD_], 
					g_data2c.cRail.X[PX_RCV_START][_ACCIDX_], g_data2c.cRail.X[PX_RCV_START][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_RCV_START])
			{
				g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_RCV_START] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"MT_RAIL_GRIPPER_X_MOVE_RCV_START", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtGrip->m_config.axisNo], 
													g_data2c.cRail.X[PX_RCV_START][_POSIDX_], g_data2c.cRail.X[PX_RCV_START][_POS_], 
													g_data2c.cRail.X[PX_RCV_START][_SPDIDX_], g_data2c.cRail.X[PX_RCV_START][_SPD_], 
													g_data2c.cRail.X[PX_RCV_START][_ACCIDX_], g_data2c.cRail.X[PX_RCV_START][_ACC_]))
			}
		}
		m_fsm.Set(C_RCV_02);
		break;
	case C_RCV_02: 
		bOptionUse  = (int)g_pNV->Pkg(PcbLengthOptionUse); 

		if(m_fsm.Once())
		{
			//if (bOptionUse)
			//	m_pCylGripFB->Actuate(pmFWD);
		}
		else
		{ 
			if (bOptionUse)
			{
				m_pCylGripFB->Actuate(pmFWD);
				if(pmFWD != m_pCylGripFB->GetPos(500))
					break;
			}
			
			if(pmCLOSE != m_pCylGripOC->GetPos(500))
			{
				m_pCylGripOC->Actuate(pmCLOSE);
				if(!g_logChk.bFunction[m_pCylGripOC->GetNo()])
				{
					g_logChk.bFunction[m_pCylGripOC->GetNo()]= TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"CYL_RAIL_GRIPPER_CLOSE",  g_data2c.cEtc.start, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylGripOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
									g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmCLOSE][pmON], L"'ON'", 
									g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmCLOSE][pmOFF], L"'OFF'"))
				}
				break;
			}
			else
			{
				if(g_logChk.bFunction[m_pCylGripOC->GetNo()])
				{
					g_logChk.bFunction[m_pCylGripOC->GetNo()] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"CYL_RAIL_GRIPPER_CLOSE",  g_data2c.cEtc.end, 
									cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, g_data2c.cPmName[m_pCylGripOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
									g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmCLOSE][pmON], L"'ON'", 
									g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmCLOSE][pmOFF], L"'OFF'"))
				}
			}

			if(!g_dIn.AOn(iRailGripperExist) && !g_opr.isDryRun) 
			{
				m_bRemove = TRUE;
				m_fsm.Set(C_ERROR, ER_RAIL_GRIPPER_EXIST_NOT_ON);

				SeqLog(L"Err : 566");

				break;
			}

			m_pCylGripFB->Actuate(pmBWD);

			if(g_pNV->UseSkip(usArts) && g_pNV->UseSkip(usTcServer)) 
			{
				if(!m_bComp2D) 
				{
					m_fsm.Set(C_RCV_2D_START);
					m_n2dRetryCnt=0;
				}
				else
					m_fsm.Set(C_RCV_03);
			}
			else
				m_fsm.Set(C_RCV_03);			
		}
		
		break;
	case C_RCV_2D_START:
		if(m_fsm.Once())
		{
			g_pNV->NDm(statePmsTop) = STATE_IDLE;
		}
		else
		{
			//if(!m_pMtGrip->InPos(PX_2D)) 
			if (!IsReadyMtRailX2D()) // 2025.03
			{
				MoveMtRailX2D(); // 2025.03
				//m_pMtGrip->Move(PX_2D); 
				if(!g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_2D])
				{
					g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_2D] = TRUE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"MT_RAIL_GRIPPER_X_MOVE_2D", g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtGrip->m_config.axisNo], 
						g_data2c.cRail.X[PX_2D][_POSIDX_], g_data2c.cRail.X[PX_2D][_POS_], 
						g_data2c.cRail.X[PX_2D][_SPDIDX_], g_data2c.cRail.X[PX_2D][_SPD_], 
						g_data2c.cRail.X[PX_2D][_ACCIDX_], g_data2c.cRail.X[PX_2D][_ACC_]))
				}
				break;
			}
			else
			{
				if(g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_2D])
				{
					g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_2D] = FALSE;
					NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"MT_RAIL_GRIPPER_X_MOVE_2D", g_data2c.cEtc.end, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtGrip->m_config.axisNo], 
						g_data2c.cRail.X[PX_2D][_POSIDX_], g_data2c.cRail.X[PX_2D][_POS_], 
						g_data2c.cRail.X[PX_2D][_SPDIDX_], g_data2c.cRail.X[PX_2D][_SPD_], 
						g_data2c.cRail.X[PX_2D][_ACCIDX_], g_data2c.cRail.X[PX_2D][_ACC_]))
				}
			}

			if(!m_fsm.Delay(300))
				break;

			if(!g_dIn.AOn(iRailGripperExist) && !g_opr.isDryRun)
			{
				if(m_nGripCnt < GRIPPER_RETRY_MAX) // Retry 최대 3번까지 재검사
				{
					m_nGripCnt++;
					m_fsm.Set(C_RCV_01);
				}
				else
				{
					m_bRemove = TRUE;
					m_pCylGripOC->Actuate(pmOPEN);
					m_fsm.Set(C_ERROR, ER_RAIL_GRIPPER_EXIST_NOT_ON);

					SeqLog(L"Err : 640");

				}
				break;
			}
			else
			{
				//NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"PCB_2D_READ_1ST", g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.tcp, L"PCB_ID", L"$"))
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"PCB_2D_READ_1ST", g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'$'", L"'PCB_ID'", L"'$'")) 
				m_fsm.Set(C_RCV_2D_01);
			}
		}
		break;
	case C_RCV_2D_01:
		if (m_fsm.Once())
		{
			g_pNV->NDm(statePmsTop) = STATE_REQ; 
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_bRemove = TRUE;
				m_pCylGripOC->Actuate(pmOPEN);
				m_fsm.Set(C_ERROR, ER_RAIL_2D_READ_TM_OVER);
				break;
			}

			if(g_opr.isDryRun)
			{
				if(!m_fsm.Delay(2000))
					break;

				m_fsm.Set(C_RCV_2D_END);
				break;
			}

			switch(g_pNV->NDm(statePmsTop))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				// PCB Info 요청까지 끝난 후에 Comp 전송
				_sprintf(cMaterialIdRail, L"\'%s\'", cMaterialId);
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"PCB_2D_READ_1ST", g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'$'", L"'PCB_ID'", cMaterialIdRail))
					m_fsm.Set(C_RCV_2D_END);
				break;
			case STATE_ERR:
				m_fsm.Set(C_RCV_2D_01_RETRY);
				break;
			}
		}
		break;
	case C_RCV_2D_01_RETRY:
		if(m_fsm.Once())
		{
			g_pNV->NDm(statePmsTop) = STATE_IDLE;

			m_n2DRetryCnt++;

			if(3 < m_n2DRetryCnt)
			{
				m_n2DRetryCnt = 0;
				m_fsm.Set(C_RCV_2D_02);
			}
		}
		else
		{
			if(!m_fsm.Delay(2000))
				break;
			m_fsm.Set(C_RCV_2D_01);
		}
		break;
	case C_RCV_2D_02:
		if(m_fsm.Once())
		{
			g_pNV->NDm(statePmsBtm) = STATE_IDLE;
		}
		else
		{
			if(!m_fsm.Delay(300))
				break;

			NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"PCB_2D_READ_2ND", g_data2c.cEtc.start, cMaterialId, cMaterialType, g_data2c.cEtc.type, L"'$'", L"'PCB_ID'", L"'$'"))
			m_fsm.Set(C_RCV_2D_03);
		}
		break;
	case C_RCV_2D_03:
		{
			if(m_fsm.Once())
			{
				g_pNV->NDm(statePmsBtm) = STATE_REQ;
			}
			else
			{

			}
			if(m_fsm.TimeLimit(10000))
			{
				m_bRemove = TRUE;
				m_pCylGripOC->Actuate(pmOPEN);
				m_fsm.Set(C_ERROR, ER_RAIL_2D_READ_TM_OVER);
				break;
			}

			if(g_opr.isDryRun)
			{
				if(!m_fsm.Delay(2000))
					break;

				m_fsm.Set(C_RCV_2D_END);
				break;
			}

			switch(g_pNV->NDm(statePmsBtm))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				_sprintf(cMaterialIdRail, L"\'%s\'", cMaterialId);
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"PCB_2D_READ_2ND", g_data2c.cEtc.end, cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.tcp, L"'PCB_ID'", cMaterialId))
				m_fsm.Set(C_RCV_2D_END);
				break;
			case STATE_ERR:
				m_fsm.Set(C_RCV_2D_03_RETRY);
				break;
			}
		}
		break;
	case C_RCV_2D_03_RETRY:
		if(m_fsm.Once())
		{
			g_pNV->NDm(statePmsBtm) = STATE_IDLE;

			m_n2DRetryCnt++;

			if(3 < m_n2DRetryCnt)
			{
				m_bRemove = TRUE;
				m_pCylGripOC->Actuate(pmOPEN);
				m_fsm.Set(C_ERROR, ER_RAIL_2D_READ_FAIL);
			}
		}
		else
		{
			if(!m_fsm.Delay(2000))
				break;
			m_fsm.Set(C_RCV_2D_03);
		}
		break;
	case C_RCV_2D_END:
		m_bComp2D = TRUE; 
		g_pNV->NDm(statePmsTop) = STATE_IDLE; 
		g_pNV->NDm(statePmsBtm) = STATE_IDLE;
		m_fsm.Set(C_RCV_03);
		break;
	case C_RCV_03:
		if(m_fsm.Once())
		{
			MoveMtRailXRevEnd(); 
		}
		else
		{
			int	   nIndex  = PX_RCV_END; 
			double dOffset = (g_pNV->DDm(commonPcbUnitCenterToEdgeXLength) - g_pNV->Pkg(pcbUnitCenterToEdgeXLength)) * 1000.0;
				   dOffset -= g_pNV->Pkg(offsetRailXRcvEnd) * 1000.0;
			double dPos	    = m_pMtGrip->m_pTable->pos[nIndex] + dOffset;

			_char cPos[_MAX_CHAR_SIZE_];
			_sprintf(cPos, L"%03f", dPos);

			if(g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_RCV_END])
			{
				g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_RCV_END] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"MT_RAIL_GRIPPER_X_MOVE_RCV_END",  g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtGrip->m_config.axisNo], 
													g_data2c.cRail.X[PX_RCV_END][_POSIDX_], cPos, 
													g_data2c.cRail.X[PX_RCV_END][_SPDIDX_], g_data2c.cRail.X[PX_RCV_END][_SPD_], 
													g_data2c.cRail.X[PX_RCV_END][_ACCIDX_], g_data2c.cRail.X[PX_RCV_END][_ACC_]))
			}

			if(!g_opr.isDryRun)
			{
				BOOL bErr  = !g_dIn.AOn(iRailGripperExist);
					 bErr |= g_dIn.AOn(iRailExistStart);
					 bErr |= !g_dIn.AOn(iRailExistEnd);
					 bErr &= !g_opr.isDryRun;

				if(bErr)
				{
					//if(m_nGripCnt < 3) // Retry
					//{
					//	m_nGripCnt++;
					//	m_fsm.Set(C_RCV_01);
					//}
					//else
					//{
						m_bRemove = TRUE;
						m_pCylGripOC->Actuate(pmOPEN);
						
						if(!g_dIn.AOn(iRailGripperExist))
						{
							m_fsm.Set(C_ERROR, ER_RAIL_GRIPPER_EXIST_NOT_ON);

							SeqLog(L"Err : 844");
						}
						else if(g_dIn.AOn(iRailExistStart))
							m_fsm.Set(C_ERROR, ER_RAIL_EXIST_START_NOT_OFF);
						else // if(!g_dIn.AOn(iRailExistEnd))
							m_fsm.Set(C_ERROR, ER_RAIL_EXIST_END_NOT_ON);
					//}
					break;
				}
			}

			m_fsm.Set(C_RCV_04);
		}
		break;
	case C_RCV_04:
		if(pmOPEN != m_pCylGripOC->GetPos(100))
		{
			m_pCylGripOC->Actuate(pmOPEN);
			if(!g_logChk.bFunction[m_pCylGripOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylGripOC->GetNo()] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"CYL_RAIL_GRIPPER_OPEN", g_data2c.cEtc.start, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylGripOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmOPEN][pmON], L"'OFF'", 
													g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmOPEN][pmOFF], L"'ON'"))
			}
			break;
		}
		else
		{
			if(g_logChk.bFunction[m_pCylGripOC->GetNo()])
			{
				g_logChk.bFunction[m_pCylGripOC->GetNo()] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"CYL_RAIL_GRIPPER_OPEN", g_data2c.cEtc.end, 
													cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.cyl, g_data2c.cEtc.actName, 
													g_data2c.cPmName[m_pCylGripOC->GetNo()], g_data2c.cEtc.delayTime, L"300", 
													g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmOPEN][pmON], L"'ON'", 
													g_data2c.cPmIO[m_pCylGripOC->GetNo()].In[pmOPEN][pmOFF], L"'OFF'"))
			}
		}

		if(!IsReadyMtRailXAlign())
		{
			MoveMtRailXAlign();
			break;
		}

		m_fsm.Set(C_RCV_05);
		break;
	case C_RCV_05:
		if(!m_pMtGrip->InPos(PX_WAIT))
		{
			m_pMtGrip->Move(PX_WAIT);
			if(!g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_WAIT])
			{
				g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_WAIT] = TRUE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"MT_RAIL_GRIPPER_X_MOVE_WAIT", g_data2c.cEtc.start, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtGrip->m_config.axisNo], 
								g_data2c.cRail.X[PX_WAIT][_POSIDX_], g_data2c.cRail.X[PX_WAIT][_POS_], 
								g_data2c.cRail.X[PX_WAIT][_SPDIDX_], g_data2c.cRail.X[PX_WAIT][_SPD_], 
								g_data2c.cRail.X[PX_WAIT][_ACCIDX_], g_data2c.cRail.X[PX_WAIT][_ACC_]))
			}
			break;
		}
		else
		{
			if(g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_WAIT])
			{
				g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_WAIT] = FALSE;
				NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"MT_RAIL_GRIPPER_X_MOVE_WAIT", g_data2c.cEtc.end, 
								cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtGrip->m_config.axisNo], 
								g_data2c.cRail.X[PX_WAIT][_POSIDX_], g_data2c.cRail.X[PX_WAIT][_POS_], 
								g_data2c.cRail.X[PX_WAIT][_SPDIDX_], g_data2c.cRail.X[PX_WAIT][_SPD_], 
								g_data2c.cRail.X[PX_WAIT][_ACCIDX_], g_data2c.cRail.X[PX_WAIT][_ACC_]))
			}
		}

		m_fsm.Set(C_RCV_END);
		break;
	case C_RCV_END:
		m_bCompRcv = TRUE;
		
		if(g_pNV->UseSkip(usSecsGem))
			g_pNV->NDm(gemInRailPcbReadEnd) = STATE_REQ;
		
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRail.deviceId, L"MOVE_PCB", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"RAIL_RCV_START", L"RAIL_RCV_END"))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CRail::CyclePcbInfo(void)
{
	if(!m_fsm.Between(C_PCB_INFO_START, C_PCB_INFO_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_RAIL_PCB_INFO_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtGrip->IsRdy())
		return;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode) + 1);
	_sprintf(cMaterialType, L"PCB");	

	switch(m_fsm.Get())
	{
	case C_PCB_INFO_START:
		g_pNV->NDm(statePcbInfo) = STATE_IDLE;
		m_nTcRetryCnt = 0;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRail.deviceId, L"PCB_INFO", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"PCB_INFO_START", L"PCB_INFO_END"))
		m_fsm.Set(C_PCB_INFO_01);
		break;
	case C_PCB_INFO_01:
		if(m_fsm.Once())
		{
			if(g_pNV->UseSkip(usArts))
			{
				g_pNV->NDm(statePcbInfo) = STATE_REQ;
			}
			else
			{
				if(!g_pNV->UseSkip(usArts) && !m_bManualLotIn)
				{
					if(!g_pNV->NDm(stateManualLotIn)) 
					{
						g_pNV->NDm(stateManualLotIn) = 1;
						g_err.Save(ER_LOT_INFO_NOT_EXIST_RAIL);
					}
					else if(3==g_pNV->NDm(stateManualLotIn))
					{
						m_bManualLotIn = TRUE;
						g_pNV->NDm(stateManualLotIn) = 0;
						g_pNV->NDm(statePcbInfo) = STATE_COMP;
					}
					else if(5==g_pNV->NDm(stateManualLotIn))
					{
						g_err.Save(ER_RAIL_MANUAL_LOT_PART_DIFFERENT);
					}
				}
				else if(!g_pNV->UseSkip(usArts) && m_bManualLotIn)
				{
					g_pNV->NDm(statePcbInfo) = STATE_COMP;
				}
			}
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_fsm.Set(C_PCB_INFO_02);
				break;
			}

			switch(g_pNV->NDm(statePcbInfo))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				if(NULL == g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].partID[0])
				{
					m_fsm.Set(C_ERROR, ER_LOT_INFO_NOT_EXIST_RAIL);
					break;
				}
				if(NULL == g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID[0])
				{
					m_fsm.Set(C_ERROR, ER_LOT_INFO_NOT_EXIST_RAIL);
					break;
				}
				if(strlen(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID) < 3)
				{
					m_fsm.Set(C_ERROR, ER_LOT_INFO_NOT_EXIST_RAIL);
					break;
				}
				if(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty <= 0)
				{
					m_fsm.Set(C_ERROR, ER_LOT_INFO_NOT_EXIST_RAIL);
					break;
				}
				if(g_pNV->UseSkip(usRfid) && g_pNV->UseSkip(usArts))
				{
					if(NULL==g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbTestResult[0])
					{
						m_fsm.Set(C_ERROR, ER_LOT_INFO_NOT_EXIST_RAIL);
						break;
					}
				}
// 				if(g_pNV->UseSkip(usRfid))
// 				{
// 					// 특이한 경우이나 Lot 수량이 Tray 수량보다 클 수 있을 듯.. 
// 					if((0 >= g_pNV->Pkg(optionLotSplit)))
// 					{
// 						// 작업해야할 수량이 Tray Max Count를 넘어서면 Error
// 						if(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty > g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].tray1ProdQty)
// 						{
// 							m_fsm.Set(C_ERROR, ER_LOT_QTY_OVER_TRAY1_PROD_QTY);
// 							break;
// 						}
// 					}
// 				}

				if(!m_bLotMgzFirst)
				{
					// 연속 자재 인데 Part No 가 틀리면 Err
					if(!g_lotInfo.PartIDComp(LOT_INFO_OLD_RAIL, LOT_INFO_RAIL))
					{
						m_fsm.Set(C_ERROR, ER_LOT_INFO_PART_ID_DIFFERENT);
						g_err.Save(ER_LOT_INFO_PART_ID_DIFFERENT);
						break;
					}
					if(!g_lotInfo.MergeLotIDComp(LOT_INFO_OLD_RAIL, LOT_INFO_RAIL))
					{
						m_fsm.Set(C_ERROR, ER_LOT_INFO_MERGE_LOT_ID_DIFFERENT);
						break;
					}

					// 연속자재 일 때 lotID 존재여부 확인
					int nResult = -1;
					for(int nNo = 0; nNo < LOT_INFO_MAX_CNT; nNo++)
					{
						if(0 == _stricmp(g_pNV->m_pLotInfo->history[nNo].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID))
						{
							nResult = nNo;
							break;
						}
					}

					if(-1 == nResult)
					{
						m_fsm.Set(C_ERROR, ER_LOT_HISTORY_MERGE_LOT_ID_NOT_EXIST);
						break;
					}
					else
					{
						// 마지막 carrierID에 생산 수량 합산
						int nResultMzNo = -1;
						for(int nMgzNo = (MGZ_INFO_MAX_CNT - 1); 0 <= nMgzNo; nMgzNo--)
						{
							if(NULL != g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].carrierID[0])
							{
								nResultMzNo = nMgzNo;
								break;
							}
						}

						if(-1 == nResultMzNo)
						{
							m_fsm.Set(C_ERROR, ER_LOT_HISTORY_LAST_CARRIER_ID_NOT_EXIST);
							break;
						}
						else
						{
							// Merge Lot 수량 확인하여 현 MGZ에 생산해야할 수량이 서버 PCB 정보와 불일치 하면 Error
							// 마지막 수량은 같은 Merge Lot ID 이므로 해당 수량과 같을 수 밖에 없음 usLotSplit 사용시 예외처리
							if((0 < g_pNV->Pkg(optionLotSplit)) && g_pNV->UseSkip(usRfid))
							{
								if(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty < g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt)
								{
									m_fsm.Set(C_ERROR, ER_LOT_HISTORY_LAST_LOT_QTY_OVER);
									break;
								}
							}
							else
							{
								if(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty <= g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt)
								{
									m_fsm.Set(C_ERROR, ER_LOT_HISTORY_LAST_LOT_QTY_OVER);
									break;
								}
							}

							int nXOutCnt = g_lotInfo.GetXOutCnt(LOT_INFO_RAIL);
							
							if((0 < g_pNV->Pkg(optionLotSplit)) && g_pNV->UseSkip(usRfid))
							{
								int nMaxCurCnt = g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt + (int)g_pNV->Pkg(unitCnt) - nXOutCnt;

								// 현재 PCB 수량 합산시 Lot 수량 초과하면 Lot Split 진행
								if(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].tray1ProdQty < nMaxCurCnt)
								{
									// Lot Split 진행
									g_pNV->NDm(flagSplitIDLotStart) = TRUE;
								}
								else
								{
									g_pNV->m_pLotInfo->history[nResult].mgzInfo[nResultMzNo].pcbProductCnt += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
									g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
									g_pNV->NDm(productCountDay) += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
								}
							}
							else
							{
								g_pNV->m_pLotInfo->history[nResult].mgzInfo[nResultMzNo].pcbProductCnt += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
								g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
								g_pNV->NDm(productCountDay) += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
							}
						}
					}
				}
				else // 첫번째 PCB 일 때
				{
					// Lot First 일 때 Merge Lot ID가 동일하면 Lot Start/Lot Merge 하지않고 Flow하도록 한다.
					BOOL bSameDevice  = g_lotInfo.PartIDComp(LOT_INFO_OLD_RAIL, LOT_INFO_RAIL);
						 bSameDevice &= g_lotInfo.MergeLotIDComp(LOT_INFO_OLD_RAIL, LOT_INFO_RAIL);
						
					if(bSameDevice)
					{
						int nResult = -1;
						for(int nNo = 0; nNo < LOT_INFO_MAX_CNT; nNo++)
						{
							BOOL bCompOk = (0 == _stricmp(g_pNV->m_pLotInfo->history[nNo].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID));

							if(bCompOk)
							{
								nResult = nNo;
								break;
							}
						}
						
						// Lot History 존재여부 확인
						if(-1 >= nResult)
						{
							m_fsm.Set(C_ERROR, ER_LOT_INFO_MERGE_LOT_ID_NOT_EXIST);
							break;
						}
						else
						{
							// Merge Lot 수량 확인하여 현 MGZ에 생산해야할 수량이 서버 PCB 정보와 불일치 하면 Error
							// 마지막 수량은 같은 Merge Lot ID 이므로 해당 수량과 같을 수 밖에 없음 usLotSplit 사용시 예외처리
							if((0 < g_pNV->Pkg(optionLotSplit)) && g_pNV->UseSkip(usRfid))
							{
								if(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty < g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt)
								{
									m_fsm.Set(C_ERROR, ER_LOT_HISTORY_LAST_LOT_QTY_OVER);
									break;
								}
							}
							else
							{
								if(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty <= g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt)
								{
									m_fsm.Set(C_ERROR, ER_LOT_HISTORY_LAST_LOT_QTY_OVER);
									break;
								}
							}
							int nXOutCnt = g_lotInfo.GetXOutCnt(LOT_INFO_RAIL);
							
							// Lot Split는 RFID, OHT 사용시에만 적용된다.
							if((0 < g_pNV->Pkg(optionLotSplit)) && g_pNV->UseSkip(usRfid))
							{
								// 연속자재이지만 총 수량을 먼저 비교한 후에 설정된 수량 초과시
								int nMaxCurCnt = g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt + (int)g_pNV->Pkg(unitCnt) - nXOutCnt;

								if(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].tray1ProdQty < nMaxCurCnt)
								{
									g_pNV->NDm(flagSplitIDLotStart) = TRUE;
								}
								else
								{
									// MGZ 정보 추가
									for(int nMgzNo = 0; nMgzNo < MGZ_INFO_MAX_CNT; nMgzNo++)
									{
										if(NULL != g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].carrierID[0])
											continue;

										// carrier ID가 없을 때 Data 입력
										if(g_pNV->UseSkip(usRfid))
											strcpy(g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].carrierID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].carrierID);
										else
											strcpy(g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].carrierID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID);
								
										strcpy(g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].lotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID);

										g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].pcbProductCnt = (int)g_pNV->Pkg(unitCnt) - nXOutCnt; // 첫 수량 기입
										g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
										g_pNV->NDm(productCountDay) += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
										break;
									}
								}
							}
							else
							{
								// MGZ 정보 추가
								for(int nMgzNo = 0; nMgzNo < MGZ_INFO_MAX_CNT; nMgzNo++)
								{
									if(NULL != g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].carrierID[0])
										continue;

									// carrier ID가 없을 때 Data 입력
									if(g_pNV->UseSkip(usRfid))
										strcpy(g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].carrierID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].carrierID);
									else
										strcpy(g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].carrierID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID);
								
									strcpy(g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].lotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotID);

									g_pNV->m_pLotInfo->history[nResult].mgzInfo[nMgzNo].pcbProductCnt = (int)g_pNV->Pkg(unitCnt) - nXOutCnt; // 첫 수량 기입
									g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
									g_pNV->NDm(productCountDay) += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
									break;
								}
							}
						}

						m_bLotMgzFirst = FALSE;
					}
					else // 다른 디바이스 일때
					{
						if(g_pNV->UseSkip(usRfid))
						{
							// 1Lot 1Mgz 기준으로 작업
							if(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty > (int)(g_pNV->Pkg(unitCnt)*g_pNV->Pkg(mzSlotZCnt)*g_pNV->Pkg(mzSlotYCnt)))
							{
								m_fsm.Set(C_ERROR, ER_LOT_QTY_OVER_1MGZ_MAX_QTY);
								break;
							}
						}

						// 만일 다른 Device가 들어 왔을 때 현 작업 수량이 불일치 하면 Alarm 띄워서 자재 제거요청
						int nResult = -1;
						for(int nNo = (LOT_INFO_MAX_CNT - 1); 0 <= nNo; nNo--)
						{
							if(NULL != g_pNV->m_pLotInfo->history[nNo].mergeLotID[0])
							{
								nResult = nNo;
								break;
							}
						}

						// 이전 작업 Lot이 존재할 때만 확인
						if(-1 < nResult)
						{
							BOOL bCheckLotQty = g_pNV->m_pLotInfo->history[nResult].lotMergeCurCnt == g_pNV->m_pLotInfo->data[LOT_INFO_OLD_RAIL].lotQty;

							if(!bCheckLotQty)
							{
								m_fsm.Set(C_ERROR, ER_LAST_LOT_QTY_DIFFERENT);
								break;
							}
						}
					}
				}
				m_fsm.Set(C_PCB_INFO_END);
				break;
			case STATE_ERR:
				m_fsm.Set(C_PCB_INFO_02);
				break;
			case STATE_PART_ERR:
				m_fsm.Set(C_ERROR, ER_RAIL_2D_READ_PART_DIFFERENT);
				break;		
			}
		}
		break;
	case C_PCB_INFO_02:
		if(m_fsm.Once())
		{
			m_nTcRetryCnt++;
		}
		else
		{
			if(3 <= m_nTcRetryCnt)
			{
				if(STATE_ERR == g_pNV->NDm(statePcbInfo))
					m_fsm.Set(C_ERROR, ER_PCB_INFO_FAIL);
				else 
					m_fsm.Set(C_ERROR, ER_PCB_INFO_TM_OVER);
			}
			else
			{
				if(m_fsm.Delay(10000))
				{
					m_fsm.Set(C_PCB_INFO_01);
				}
			}
		}
		break;
	case C_PCB_INFO_END:
		g_pNV->DDm(lotCurQty) = g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty; 
		m_bCompPcbInfo = TRUE;
		g_pNV->NDm(statePcbInfo) = STATE_IDLE;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRail.deviceId, L"PCB_INFO", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"PCB_INFO_START", L"PCB_INFO_END"))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CRail::CycleLotMerge(void)
{
	if(!m_fsm.Between(C_LOT_MERGE_START, C_LOT_MERGE_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_RAIL_LOT_MERGE_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtGrip->IsRdy())
		return;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode) + 1);
	_sprintf(cMaterialType, L"PCB");

	switch(m_fsm.Get())
	{
	case C_LOT_MERGE_START:
		g_pNV->NDm(stateLotMerge) = STATE_IDLE;
		m_nTcRetryCnt = 0;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRail.deviceId, L"LOT_MERGE", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"LOT_MERGE_START", L"LOT_MERGE_END"))
		m_fsm.Set(C_LOT_MERGE_01);
		break;
	case C_LOT_MERGE_01:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateLotMerge) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_fsm.Set(C_LOT_MERGE_02);
				break;
			}

			switch(g_pNV->NDm(stateLotMerge))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				m_fsm.Set(C_LOT_MERGE_END);
				break;
			case STATE_ERR:
				m_fsm.Set(C_LOT_MERGE_02);
				break;
			}
		}
		break;
	case C_LOT_MERGE_02:
		if(m_fsm.Once())
		{
			m_nTcRetryCnt++;
		}
		else
		{
			if(3 <= m_nTcRetryCnt)
			{				
				if(STATE_ERR == g_pNV->NDm(stateLotMerge))
				{
					// Merge Lot Fail이면 Lot Start로 진행
					//m_fsm.Set(C_ERROR, ER_LOT_MERGE_FAIL);
					g_pNV->NDm(stateLotMerge) = STATE_IDLE;
					m_fsm.Set(C_LOT_START_START);
				}
				else 
					m_fsm.Set(C_ERROR, ER_LOT_MERGE_TM_OVER);
			}
			else
			{
				if(m_fsm.Delay(10000))
				{
					m_fsm.Set(C_LOT_MERGE_01);
				}
			}
		}
		break;
	case C_LOT_MERGE_END:
		// 이전 작업 Lot와 수량 합산
		for(int nNo = (LOT_INFO_MAX_CNT-1); 0 <= nNo; nNo--) // 뒤에서 부터 조회
		{
			// 비교 할 정보가 없음 뒤에서부터 조회해서 수량 합산해야 함
			if(NULL != g_pNV->m_pLotInfo->history[nNo].mergeLotID[0])
			{
				int nXOutCnt = g_lotInfo.GetXOutCnt(LOT_INFO_RAIL);

				strcpy(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID, g_pNV->m_pLotInfo->history[nNo].mergeLotID);
				g_pNV->m_pLotInfo->history[nNo].lotMergeCurCnt += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
				g_pNV->NDm(productCountDay) += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;

				// MGZ 정보 추가
				for(int nMgzNo = 0; nMgzNo < MGZ_INFO_MAX_CNT; nMgzNo++)
				{
					if(NULL != g_pNV->m_pLotInfo->history[nNo].mgzInfo[nMgzNo].carrierID[0])
						continue;
					// carrier ID가 없을 때 Data 입력
					if(g_pNV->UseSkip(usRfid))
						strcpy(g_pNV->m_pLotInfo->history[nNo].mgzInfo[nMgzNo].carrierID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].carrierID);
					else
						strcpy(g_pNV->m_pLotInfo->history[nNo].mgzInfo[nMgzNo].carrierID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID);

					strcpy(g_pNV->m_pLotInfo->history[nNo].mgzInfo[nMgzNo].lotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotID);
					g_pNV->m_pLotInfo->history[nNo].mgzInfo[nMgzNo].pcbProductCnt = (int)g_pNV->Pkg(unitCnt) - nXOutCnt; // 첫 수량 기입
					break;
				}

				break;
			}
		}

		// Lot Merge가 되어도 Split Lot Clear
		g_lotInfo.LotSplitAllClear();
		g_pNV->NDm(lotSplitCount) = 0;
		g_pNV->NDm(flagSplitInfo) = FALSE;
		g_pNV->NDm(flagSplitIDLotStart) = FALSE;
		g_pNV->NDm(flagLotMergeComp) = TRUE; // Lot Merge 완료

		m_bCompLotSend = TRUE;
		m_bLotMgzFirst = FALSE;
		g_pNV->NDm(stateLotMerge) = STATE_IDLE;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRail.deviceId, L"LOT_MERGE", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"LOT_MERGE_START", L"LOT_MERGE_END"))
		m_fsm.Set(C_IDLE);
		break;
	}
}


//-------------------------------------------------------------------
void CRail::CycleLotStart(void)
{
	if(!m_fsm.Between(C_LOT_START_START, C_LOT_START_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_RAIL_LOT_START_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtGrip->IsRdy())
		return;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode) + 1);
	_sprintf(cMaterialType, L"PCB");

	switch(m_fsm.Get())
	{
	case C_LOT_START_START:
		g_pNV->NDm(stateLotStart) = STATE_IDLE;
		m_nTcRetryCnt = 0;
		NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRail.deviceId, L"LOT_START", g_data2c.cEtc.start, cMaterialId, cMaterialType, L"LOT_START_START", L"LOT_START_END"))
		m_fsm.Set(C_LOT_START_01);
		break;
	case C_LOT_START_01:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateLotStart) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_fsm.Set(C_LOT_START_02);
				break;
			}

			switch(g_pNV->NDm(stateLotStart))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				m_fsm.Set(C_LOT_START_END);
				break;
			case STATE_ERR:
				m_fsm.Set(C_LOT_START_02);
				break;
			}
		}
		break;
	case C_LOT_START_02:
		if(m_fsm.Once())
		{
			m_nTcRetryCnt++;
		}
		else
		{
			if(3 <= m_nTcRetryCnt)
			{
				if(STATE_ERR == g_pNV->NDm(stateLotStart))
					m_fsm.Set(C_ERROR, ER_LOT_START_FAIL);
				else 
					m_fsm.Set(C_ERROR, ER_LOT_START_TM_OVER);
			}
			else
			{
				if(m_fsm.Delay(10000))
				{
					m_fsm.Set(C_LOT_START_01);
				}
			}
		}
		break;
	case C_LOT_START_END:
		{
			// 새로운 공간에 Lot 정보 입력하여 수량 Cnt
			for(int nNo = 0; nNo < LOT_INFO_MAX_CNT; nNo++)
			{
				if(NULL == g_pNV->m_pLotInfo->history[nNo].mergeLotID[0])
				{
					int nXOutCnt = g_lotInfo.GetXOutCnt(LOT_INFO_RAIL);

					strcpy(g_pNV->m_pLotInfo->history[nNo].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID);
					strcpy(g_pNV->m_pLotInfo->history[nNo].partID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].partID);
					g_pNV->m_pLotInfo->history[nNo].lotMergeCurCnt = (int)g_pNV->Pkg(unitCnt) - nXOutCnt; // 첫 수량 입력 // x, a 뺌
					g_pNV->NDm(productCountDay) += (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
					
					// MGZ 정보 추가 (초기 Data 입력)
					// RFID Mode Skip 일 때는 MGZ ID가 없으므로 Lot ID를 MGZ Id에 기입하도록 한다. 어쩔수 없음
					if(g_pNV->UseSkip(usRfid))
						strcpy(g_pNV->m_pLotInfo->history[nNo].mgzInfo[0].carrierID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].carrierID);
					else
						strcpy(g_pNV->m_pLotInfo->history[nNo].mgzInfo[0].carrierID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].mergeLotID);

					strcpy(g_pNV->m_pLotInfo->history[nNo].mgzInfo[0].lotID, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotID);
					g_pNV->m_pLotInfo->history[nNo].mgzInfo[0].pcbProductCnt = (int)g_pNV->Pkg(unitCnt) - nXOutCnt;
					break;
				}
			}

			g_lotInfo.LotSplitAllClear();
			g_pNV->NDm(lotSplitCount) = 0;
			g_pNV->NDm(flagSplitInfo) = FALSE;
			g_pNV->NDm(flagSplitIDLotStart) = FALSE;

			g_pNV->NDm(stateLotStart) = STATE_IDLE;

			m_bCompLotSend = TRUE;
			m_bLotMgzFirst = FALSE;
			
			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemRecipeUpload) = STATE_REQ;
			
			NEGRETE_WRITE(g_TpBase.logTransfer(g_data2c.cRail.deviceId, L"LOT_START", g_data2c.cEtc.end, cMaterialId, cMaterialType, L"LOT_START_START", L"LOT_START_END"))
			m_fsm.Set(C_IDLE);
		}
		break;
	}
}


//-------------------------------------------------------------------
void CRail::CycleLotSplit(void)
{
	if(!m_fsm.Between(C_LOT_SPLIT_START, C_LOT_SPLIT_END))
		return;

	if(m_fsm.TimeLimit(300000))
	{
		m_fsm.Set(C_ERROR, ER_RAIL_LOT_SPLIT_CYCLE_TM_OVER);
		return;
	}

	if(!m_pMtGrip->IsRdy())
		return;

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode) + 1);
	_sprintf(cMaterialType, L"PCB");

	switch(m_fsm.Get())
	{
	case C_LOT_SPLIT_START:
		g_pNV->NDm(stateLotSplit) = STATE_IDLE;
		m_nTcRetryCnt = 0;
		NEGRETE_WRITE(g_TpBase.logTransfer(L"RAIL", L"LOT_SPLIT", L"START", cMaterialId, cMaterialType, L"LOT_SPLIT_START", L"LOT_SPLIT_END"))
		m_fsm.Set(C_LOT_SPLIT_01);
		break;
	case C_LOT_SPLIT_01:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateLotSplit) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(10000))
			{
				m_fsm.Set(C_LOT_SPLIT_02);
				break;
			}

			switch(g_pNV->NDm(stateLotSplit))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				m_fsm.Set(C_LOT_SPLIT_END);
				break;
			case STATE_ERR:
				m_fsm.Set(C_LOT_SPLIT_02);
				break;
			}
		}
		break;
	case C_LOT_SPLIT_02:
		if(m_fsm.Once())
		{
			m_nTcRetryCnt++;
		}
		else
		{
			if(3 <= m_nTcRetryCnt)
			{
				if(STATE_ERR == g_pNV->NDm(stateLotSplit))
					m_fsm.Set(C_ERROR, ER_LOT_SPLIT_FAIL);
				else 
					m_fsm.Set(C_ERROR, ER_LOT_SPLIT_TM_OVER);
			}
			else
			{
				if(m_fsm.Delay(10000))
				{
					m_fsm.Set(C_LOT_SPLIT_01);
				}
			}
		}
		break;
	case C_LOT_SPLIT_END:
		{
			// Split Lot ID 존재여부만 확인하여 알람처리하도록 한다.
			if(NULL == g_pNV->m_pLotInfo->split.lotID[0])
			{
				// Error
				m_fsm.Set(C_ERROR, ER_LOT_SPLIT_ID_EMPTY);
				break;
			}

			g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].lotQty = g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].tray1ProdQty;

			g_pNV->NDm(stateLotSplit) = STATE_IDLE;
			g_pNV->NDm(flagLotMergeComp) = FALSE;
			m_bCompLotSplit = TRUE;

			if(g_pNV->UseSkip(usSecsGem))
				g_pNV->NDm(gemRecipeUpload) = STATE_REQ;

			NEGRETE_WRITE(g_TpBase.logTransfer(L"RAIL", L"LOT_SPLIT", L"END", cMaterialId, cMaterialType, L"LOT_SPLIT_START", L"LOT_SPLIT_END"))

			m_fsm.Set(C_IDLE);
		}
		break;
	}
}


BOOL CRail::IsReadyLdMz(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyLdMz);
	}
	else
	{
		BOOL isRdy  = !m_fsm.IsRun();
			 isRdy &= IsReadyMtRailXRevStart();// m_pMtGrip->IsRdy(PX_RCV_START) && m_pMtGrip->InPos(PX_RCV_START);
			 isRdy &= EXIST_NORMAL == GetExistErr();
			 isRdy &= (!g_dIn.AOn(iRailExistStart) && !g_dIn.AOn(iRailExistMid1) && !g_dIn.AOn(iRailExistMid2)) || g_opr.isDryRun;
			 isRdy &= (!g_dIn.AOn(iRailExistEnd) && !g_dIn.AOn(iRailGripperExist)) || g_opr.isDryRun;
			 isRdy &= !g_dIn.BOn(iMzClampPcbJam) || g_opr.isDryRun;
			 isRdy &= !Exist();

		return (isRdy);
	}
}


BOOL CRail::IsReadyInPnp(void)
{
	if(g_opr.isAuto)
	{
		return (m_bRdyInPnp);
	}
	else
	{
		BOOL isRdy  = !m_fsm.IsRun();
			 isRdy &= m_pMtGrip->IsRdy(PX_WAIT) && m_pMtGrip->InPos(PX_WAIT);
			 isRdy &= EXIST_NORMAL == GetExistErr();
			 isRdy &= g_dIn.AOn(iRailExistEnd) || g_opr.isDryRun;
			 isRdy &= Exist();
		
		return (isRdy);
	}
}


//-------------------------------------------------------------------
BOOL CRail::IsReadyMtRailXRevEnd() // 사용안함
{
	int    nIndex   = PX_RCV_END;
	double dOffset  = g_pNV->Pkg(offsetRailXRcvEnd) * 1000.0;	
		   dOffset += (g_pNV->DDm(commonPcbUnitCenterToEdgeXLength) - g_pNV->Pkg(pcbUnitCenterToEdgeXLength)) * 1000.0;	
	double dPos	    = m_pMtGrip->m_pTable->pos[nIndex] + dOffset;

	if(!m_pMtGrip->IsRdy())
		return (FALSE);

	if(!m_pMtGrip->InPos(nIndex, dPos, 50))
		return (FALSE);

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRail::MoveMtRailXRevEnd()
{
	int	   nIndex  = PX_RCV_END; 
	double dOffset = (g_pNV->DDm(commonPcbUnitCenterToEdgeXLength) - g_pNV->Pkg(pcbUnitCenterToEdgeXLength)) * 1000.0;
		   dOffset -= g_pNV->Pkg(offsetRailXRcvEnd) * 1000.0;
	double dPos	    = m_pMtGrip->m_pTable->pos[nIndex] + dOffset;

	m_pMtGrip->PMove(nIndex, dPos);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode) + 1);
	_sprintf(cMaterialType, L"PCB");	

	_char cPos[_MAX_CHAR_SIZE_];
	_sprintf(cPos, L"%03f", dPos);

	if(!g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_RCV_END])
	{
		g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_RCV_END] = TRUE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"MT_RAIL_GRIPPER_X_MOVE_RCV_END",  g_data2c.cEtc.start, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtGrip->m_config.axisNo], 
											g_data2c.cRail.X[PX_RCV_END][_POSIDX_], cPos, 
											g_data2c.cRail.X[PX_RCV_END][_SPDIDX_], g_data2c.cRail.X[PX_RCV_END][_SPD_], 
											g_data2c.cRail.X[PX_RCV_END][_ACCIDX_], g_data2c.cRail.X[PX_RCV_END][_ACC_]))
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRail::IsReadyMtRailXAlign()
{
	int    nIndex   = PX_ALIGN; 
	double dOffset  = (g_pNV->DDm(commonPcbUnitCenterToEdgeXLength) - g_pNV->Pkg(pcbUnitCenterToEdgeXLength)) * 1000.0;
		   dOffset  -= g_pNV->Pkg(offsetRailXAlign) * 1000.0; 	
	double dPos	    = m_pMtGrip->m_pTable->pos[nIndex] + dOffset;

	if(!m_pMtGrip->IsRdy()) 
		return (FALSE);

	if(!m_pMtGrip->InPos(nIndex, dPos, 50)) 
		return (FALSE);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode) + 1);
	_sprintf(cMaterialType, L"PCB");

	_char cPos[_MAX_CHAR_SIZE_];
	_sprintf(cPos, L"%03f", dPos);

	if(g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_ALIGN])
	{
		g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_ALIGN] = FALSE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"MT_RAIL_GRIPPER_X_MOVE_ALIGN",  g_data2c.cEtc.end, 
											cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtGrip->m_config.axisNo], 
											g_data2c.cRail.X[PX_RCV_END][_POSIDX_], cPos, 
											g_data2c.cRail.X[PX_RCV_END][_SPDIDX_], g_data2c.cRail.X[PX_RCV_END][_SPD_], 
											g_data2c.cRail.X[PX_RCV_END][_ACCIDX_], g_data2c.cRail.X[PX_RCV_END][_ACC_]))	
	}

	return (TRUE);
}


//-------------------------------------------------------------------
BOOL CRail::MoveMtRailXAlign() 
{
	int	   nIndex  = PX_ALIGN;
	double dOffset = (g_pNV->DDm(commonPcbUnitCenterToEdgeXLength) - g_pNV->Pkg(pcbUnitCenterToEdgeXLength)) * 1000.0;
		   dOffset -= g_pNV->Pkg(offsetRailXAlign) * 1000.0; 
	double dPos	    = m_pMtGrip->m_pTable->pos[nIndex] + dOffset;
	
	m_pMtGrip->PMove(nIndex, dPos);

	_char cMaterialId[_MAX_CHAR_SIZE_], cMaterialType[_MAX_CHAR_SIZE_];
	mbstowcs(cMaterialId, g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode, strlen(g_pNV->m_pLotInfo->data[LOT_INFO_RAIL].pcbBarcode) + 1);
	_sprintf(cMaterialType, L"PCB");

	_char cPos[_MAX_CHAR_SIZE_];
	_sprintf(cPos, L"%03f", dPos);

	if(!g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_ALIGN])
	{
		g_logChk.bTransfer[m_pMtGrip->m_config.axisNo][PX_ALIGN] = TRUE;
		NEGRETE_WRITE(g_TpBase.logFunction(g_data2c.cRail.deviceId, L"MT_RAIL_GRIPPER_X_MOVE_ALIGN", g_data2c.cEtc.start, 
						cMaterialId, cMaterialType, g_data2c.cEtc.type, g_data2c.cEtc.motor, g_data2c.cEtc.actName, g_data2c.cMtName[m_pMtGrip->m_config.axisNo],
						g_data2c.cRail.X[PX_ALIGN][_POSIDX_], cPos,
						g_data2c.cRail.X[PX_ALIGN][_SPDIDX_], g_data2c.cRail.X[PX_ALIGN][_SPD_], 
						g_data2c.cRail.X[PX_ALIGN][_ACCIDX_], g_data2c.cRail.X[PX_ALIGN][_ACC_]))
	}

	return (TRUE);
}

//-------------------------------------------------------------------
BOOL CRail::IsReadyMtRailX2D() // 2025.03
{
	int    nIndex  = PX_2D;
	double dOffset = g_pNV->Pkg(offsetRailX2D) * 1000.0;

	double dPos = m_pMtGrip->m_pTable->pos[nIndex] - dOffset;

	if (!m_pMtGrip->IsRdy())
		return (FALSE);

	if (!m_pMtGrip->InPos(nIndex, dPos, 50))
		return (FALSE);

	return (TRUE);
}

//-------------------------------------------------------------------
BOOL CRail::MoveMtRailX2D()
{
	int    nIndex = PX_2D;
	double dOffset = g_pNV->Pkg(offsetRailX2D) * 1000.0;

	double dPos = m_pMtGrip->m_pTable->pos[nIndex] - dOffset;

	m_pMtGrip->PMove(nIndex, dPos);

	return (TRUE);
}

//-------------------------------------------------------------------
BOOL CRail::IsReadyMtRailXRevStart() // 2025.03
{
	int    nIndex  = PX_RCV_START;
	double dOffset = g_pNV->Pkg(offsetRailXRcvStart) * 1000.0;

	double dPos = m_pMtGrip->m_pTable->pos[nIndex] - dOffset;

	if (!m_pMtGrip->IsRdy())
		return (FALSE);

	if (!m_pMtGrip->InPos(nIndex, dPos, 50))
		return (FALSE);

	return (TRUE);
}

//-------------------------------------------------------------------
BOOL CRail::MoveMtRailXRevStart()
{
	int    nIndex = PX_RCV_START;
	double dOffset = g_pNV->Pkg(offsetRailXRcvStart) * 1000.0;

	double dPos = m_pMtGrip->m_pTable->pos[nIndex] - dOffset;

	m_pMtGrip->PMove(nIndex, dPos);

	return (TRUE);
}

//-------------------------------------------------------------------
void CRail::Init(void)
{
	m_pMtGrip = &g_mt[MT_RAIL_GRIPPER_X];
	m_pCylGripOC = &g_pm[CYL_RAIL_GRIPPER_OC];
	m_pCylGripFB = &g_pm[CYL_RAIL_GRIPPER_FB];
}


//-------------------------------------------------------------------
int  CRail::GetState(void)
{
	int nState = S_IDLE; 

	if(Exist()) 
	{
		if(!m_bCompRcv) 
			nState = S_RCV;
		else if(!m_bCompPcbInfo) 
			nState = S_PCB_INFO;
		else if(!m_bCompLotSend)
			nState = S_LOT_SEND;
		else if(!m_bCompLotSplit)
			nState = S_LOT_SPLIT;
		else
			nState = S_PNP; 
	}
	else
		nState = S_WAIT; 

	return (nState);
}


//-------------------------------------------------------------------
BOOL CRail::IsErr(void)
{
	if(!m_pMtGrip->m_state.isHome)
		return (TRUE);
	if(0 < m_pCylGripOC->GetErr())
		return (TRUE);
	if(0 < m_pCylGripFB->GetErr())
		return (TRUE);

	return (FALSE);
}


//-------------------------------------------------------------------
int& CRail::Exist(void)
{
	return (g_pNV->m_pData->ndm[existRail]);
}


//-------------------------------------------------------------------
int CRail::GetExistErr(void)
{
	if(g_opr.isDryRun)
		return (EXIST_NORMAL);

	//BOOL isSenOn = g_dIn.AOn(iRailExistEnd);
	//
	//if(!m_bCompRcv)
	//	isSenOn = g_dIn.AOn(iRailExistStart);

	BOOL isSenOn = g_dIn.AOn(iRailExistStart) || g_dIn.AOn(iRailExistMid1) || 
		           g_dIn.AOn(iRailExistMid2) || g_dIn.AOn(iRailExistEnd) || g_dIn.AOn(iRailGripperExist);

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


