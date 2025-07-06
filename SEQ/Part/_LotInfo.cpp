#include "..\def\Includes.h"


/////////////////////////////////////////////////////////////////////
CLotInfo g_lotInfo;
/////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
void CLotInfo::AutoRun(void)
{
	CycleRun();

	if(m_fsm.IsRun())
		return;

	if(!g_pNV->UseSkip(usTcServer))
	{
		g_dOut.Off(oSorterRouterAllEmpty); 
		m_tmEmptyLotEnd.Reset();
		return;
	}

	// 설비 Auto 중 Part No가 다른 것이 들어 왔을 때 Change 기능
	bool bEmptyLotEnd = !IsExistInMc() && g_dIn.AOn(iSorterStageAllEmpty);
	bool bAdcLotEnd  = g_pNV->UseSkip(usRfidPartNoCompare) && g_pNV->NDm(needAutoRecipeChg);
		 bAdcLotEnd &= g_dIn.AOn(iSorterStageAllEmpty) && !g_dIn.AOn(iSorterLotEndComp);

	if(bEmptyLotEnd)
	{
		DWORD dwTm = (DWORD)(g_pNV->DDm(lotMergeEndTime) * 1000.0);
		
		if(!m_tmEmptyLotEnd.TmOver(dwTm))
			return;

		if(NULL == g_pNV->m_pLotInfo->history[0].mergeLotID[0])
		{
			g_dOut.On(oSorterRouterAllEmpty); 
			return;
		}
		else
		{
			// 현재 생산 수량이 무조건 맞아야 함 (Rail의 마지막 수량 확인)
			// 마지막 MGZ 생산 수량과 서버 생산 수량이 일치해야 함
			// 수량이 맞지 않으면 무한 대기 해야 함
			// Lot이 Merge 되는 순간 Total 수량이 바뀜
			BOOL bLotEnd  = (0 == _stricmp(g_pNV->m_pLotInfo->history[0].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].mergeLotID));
				 bLotEnd &= (g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].lotQty == g_pNV->m_pLotInfo->history[0].lotMergeCurCnt);

			if(bLotEnd)
				m_fsm.Set(C_TRACK_OUT_START);
			else
			{
				if(g_opr.isAuto)
					g_err.Save(ER_LOT_END_LAST_LOT_QTY_DIFFERENT);
			}
		}
	}
	else if(bAdcLotEnd)
	{
		if(NULL == g_pNV->m_pLotInfo->history[0].mergeLotID[0])
		{
			g_dOut.On(oSorterRouterAllEmpty); 
			return;
		}
		else
		{
			BOOL bLotEnd  = (0 == _stricmp(g_pNV->m_pLotInfo->history[0].mergeLotID, g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].mergeLotID));
				 bLotEnd &= (g_pNV->m_pLotInfo->data[LOT_INFO_OLD_OUTPNP].lotQty == g_pNV->m_pLotInfo->history[0].lotMergeCurCnt);

			if(bLotEnd)
				m_fsm.Set(C_TRACK_OUT_START);
			else
			{
				if(g_opr.isAuto)
					g_err.Save(ER_LOT_END_LAST_LOT_QTY_DIFFERENT);
			}
		}
	}
	else
	{
		g_dOut.Off(oSorterRouterAllEmpty);
		m_tmEmptyLotEnd.Reset();
	}

	return;
}


void CLotInfo::CycleRun(void)
{
	if(!m_fsm.IsRun())
		return;
	
	if(C_ERROR == m_fsm.Get())
	{
		if(0 < m_fsm.GetMsg())
			g_err.Save(m_fsm.GetMsg());
	
		m_fsm.Set(C_IDLE);
		return;
	}
	
	switch(m_fsm.Get())
	{
	case C_TRACK_OUT_START:
		if(m_fsm.Once())
		{
			g_pNV->NDm(stateLotInfoLog) = STATE_REQ;
		}
		else
		{
			if(m_fsm.TimeLimit(5000))
			{
				g_pNV->NDm(stateLotInfoLog) = STATE_IDLE;
				
				LotInfoAllClear(TRUE);

				SeqLog(L"Event : LotHistoryAllClear C_TRACK_OUT_START !!!");
				LotHistoryAllClear();
				m_fsm.Set(C_TRACK_OUT_END);
				break;
			}

			switch (g_pNV->NDm(stateLotInfoLog))
			{
			case STATE_BUSY:
				break;
			case STATE_COMP:
				LotInfoAllClear(TRUE);

				SeqLog(L"Event : LotHistoryAllClear STATE_COMP !!!");
				LotHistoryAllClear();
				m_fsm.Set(C_TRACK_OUT_END);
				break;
			case STATE_ERR:

				LotInfoAllClear(TRUE);

				SeqLog(L"Event : LotHistoryAllClear STATE_ERR !!!");
				LotHistoryAllClear();
				m_fsm.Set(C_TRACK_OUT_END);
				break;
			}
		}
		break;
	case C_TRACK_OUT_END:
		if(g_pNV->UseSkip(usSecsGem))
			g_pNV->NDm(gemTrackOut) = STATE_REQ;

		m_fsm.Set(C_IDLE);
		break;
	}
}

//-------------------------------------------------------------------
void CLotInfo::Init(void)
{
	//m_pnTrackOutActivated = &g_pNV->m_pData->ndm[isTrackActivated1];
}


//-------------------------------------------------------------------
BOOL CLotInfo::IsExistInMc()
{
	BOOL isExist  = FALSE;
		 isExist |= g_pNV->NDm(existMzInStopper01);
		 isExist |= g_pNV->NDm(existMzInStopper02);
		 isExist |= g_pNV->NDm(existMzInStopper03);
		 isExist |= g_pNV->NDm(existMzLoadZExist);
		 isExist |= g_pNV->NDm(existMzLoadZArrival);
		 isExist |= g_pNV->NDm(existLdMz);
		 isExist |= g_pNV->NDm(existRail);
		 isExist |= g_pNV->NDm(existInPnp);
		 isExist |= g_pNV->NDm(existIndex01);
		 isExist |= g_pNV->NDm(existIndex02);
		 isExist |= g_pNV->NDm(existIndex03);
		 isExist |= g_pNV->NDm(existIndex04);
		 isExist |= g_pNV->NDm(existIndexScrap01);
		 isExist |= g_pNV->NDm(existIndexScrap02);
		 isExist |= g_pNV->NDm(existIndexScrap03);
		 isExist |= g_pNV->NDm(existIndexScrap04);
		 isExist |= g_pNV->NDm(existOutPnpScrap);
		 isExist |= g_pNV->NDm(existOutPnpPcb);

	return (isExist);
}


void CLotInfo::LotInfoCopy(int Org, int Target)
{
	int nSize = sizeof(LotInfo);
	LotInfo* pOrigin = &g_pNV->m_pLotInfo->data[Org];
	LotInfo* pTarget = &g_pNV->m_pLotInfo->data[Target];
	memcpy(pTarget, pOrigin, nSize);
}


void CLotInfo::LotInfoClear(int nNo)
{
	int nSize = sizeof(LotInfo);
	ZeroMemory(&g_pNV->m_pLotInfo->data[nNo], nSize);
}


void CLotInfo::LotInfoAllClear(bool ohtMode)
{
	int nSize = sizeof(LotInfo);

	if(ohtMode)
	{
		for(int nNo = 0; nNo < LOT_INFO_OLD_OUTPNP; nNo++)
		{
			ZeroMemory(&g_pNV->m_pLotInfo->data[nNo], nSize);
		}
	}
	else
	{
		for(int nNo = 0; nNo < LOT_INFO_MAX_CNT; nNo++)
		{
			ZeroMemory(&g_pNV->m_pLotInfo->data[nNo], nSize);
		}
	}
}


void CLotInfo::LotFirstHistoryClear()
{
	int nSize = sizeof(LotHistory);
	ZeroMemory(&g_pNV->m_pLotInfo->history[0], nSize);
}


BOOL CLotInfo::PartIDComp(int lotInfo1, int lotInfo2)
{
	// 같으면 TRUE, 다르면 FALSE
	BOOL bRet = (0 == _stricmp(g_pNV->m_pLotInfo->data[lotInfo1].partID, g_pNV->m_pLotInfo->data[lotInfo2].partID));

	return (bRet);
}


BOOL CLotInfo::MergeLotIDComp(int lotInfo1, int lotInfo2)
{
	// 같으면 TRUE, 다르면 FALSE
	BOOL bRet = (0 == _stricmp(g_pNV->m_pLotInfo->data[lotInfo1].mergeLotID, g_pNV->m_pLotInfo->data[lotInfo2].mergeLotID));

	return (bRet);
}


int CLotInfo::GetXOutCnt(int lotInfo)
{
	int nCnt = 0;
	int nMaxUnitCnt = (int)g_pNV->Pkg(unitCnt);

	for(int i = 0; i < nMaxUnitCnt; i++)
	{
		switch(g_pNV->m_pLotInfo->data[lotInfo].pcbTestResult[i])
		{
		case 'x':
		case 'X':
		case 'a':
		case 'A':
			nCnt++;
			break;
		}
	}
	return (nCnt);
}


void CLotInfo::LotHistoryAllClear()
{
	int nSize = sizeof(LotHistory);
	for(int nNo = 0; nNo < LOT_INFO_MAX_CNT; nNo++)
	{
		ZeroMemory(&g_pNV->m_pLotInfo->history[nNo], nSize);
	}
}


void CLotInfo::LotSplitAllClear()
{
	int nSize = sizeof(LotSplitInfo);

	ZeroMemory(&g_pNV->m_pLotInfo->split, nSize);
}


void CLotInfo::LotHistorySort()
{
	int nSize = sizeof(LotHistory);
	
	if(NULL != g_pNV->m_pLotInfo->history[0].mergeLotID[0])
		return;

	for(int nNo = 0; nNo < (LOT_INFO_MAX_CNT-1); nNo++) // 0 ~ 19 까지
	{
		if(NULL == g_pNV->m_pLotInfo->history[nNo].mergeLotID[0])
		{
			for(int nNextNo = nNo+1; nNextNo < LOT_INFO_MAX_CNT; nNextNo++) // 1 ~ 20 까지
			{
				if(NULL != g_pNV->m_pLotInfo->history[nNextNo].mergeLotID[0])
				{
					LotHistory* pTarget = &g_pNV->m_pLotInfo->history[nNo];
					LotHistory* pOrigin = &g_pNV->m_pLotInfo->history[nNextNo];
				
					memcpy(pTarget, pOrigin, nSize);
					ZeroMemory(&g_pNV->m_pLotInfo->history[nNextNo], nSize);
					break;
				}
			}
		}
	}
}


void CLotInfo::DelDeque(int nNo)
{
	int nEarse = -1;
	int nDequeSize = m_qLotHistory.size();
	for (int i=0; i < nDequeSize; i++)
	{			       
		if (m_qLotHistory.at(i).order == nNo)
		{            
			nEarse = i;
			break;
		}
	}
	if (nEarse > -1)
	{
		m_qLotHistory.erase(m_qLotHistory.begin() + nEarse);
	}
}
