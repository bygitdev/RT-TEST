#include ".\def\Includes.h"


//////////////////////////////////////////////////////////////////////////
CSEQ  g_seq;
//////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------
BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
    switch(dwCtrlType)
    {
    case CTRL_C_EVENT:
        break;
    case CTRL_CLOSE_EVENT:
        g_bClose = TRUE;	//ID_YES == ::MessageBox(NULL, "Do you really want to terminate sequence program ?", "Critical Decision", MB_YESNO);		
        break;
    case CTRL_BREAK_EVENT:
        break;
    case CTRL_LOGOFF_EVENT:
        break;
    case CTRL_SHUTDOWN_EVENT:
        break;
    default:
        break;
    }

    Sleep(1000);
    return (FALSE);
}


//------------------------------------------------------------------
void CSEQ::Main(void)
{
	printf("\n KOSES Router System");
	printf("\n Model : KRT-700");
	printf("\n Serial No : 78-22-0026");
	printf("\n Latest Update : %s", __DATE__);
	
	g_bClose = FALSE;
	m_mainState.Set(S_INIT);

 	while(!g_bClose)  
	{
		switch(m_mainState.Get())
		{
		case S_INIT:
			InitNV();
			printf("\n Init Nv Ok!!!");
			m_mainState.Set(S_INIT_AJIN);
			break;
		case S_INIT_AJIN:
			{
				BOOL isComp = InitAjin(TRUE);
				if(isComp)
				{
					printf("\n Init Ajin Complete!!!");
					g_update.Output();
					m_mainState.Set(S_INIT_CLASS);
					Sleep(1000);
				}
				else
				{
					Sleep(10000);
					printf("\n Retry Init Ajin-Board!!!");
				}
			}
			break;
		case S_INIT_CLASS:
			InitPnematic();
			printf("\n Init Pnematic!!!");
			InitClass();
			printf("\n Init Class!!!");
			m_mainState.Set(S_RUN);
			break;
		case S_RUN:
			g_update.Input();
			g_err.Run();
			g_mmi.Run();
			Seq();
			g_update.Output();
			g_update.Motor();
			break;
		case S_EMG_ON:
			printf("\n Emg On!!!");
			EmgOn();
			break;
		case S_EMG_OFF:
			printf("\n Emg Off!!!");
			EmgOff();
			break;
		}
		Sleep(1);
	}

	SafetyOutOff();
	Sleep(1000);
}


//------------------------------------------------------------------
BOOL CSEQ::InitNV(void)
{
	// PGM 중복 실행 체크
	HANDLE hEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, L"KOSES_SEQUENCE");
	if (NULL != hEvent)
	{
		::MessageBox(NULL, _T("Another sequence is running!!"), _T("Error") , MB_OK);
		return FALSE;
	}	
	else
	{
		CreateEvent(NULL, FALSE, FALSE, _T("KOSES_SEQUENCE"));
	}

	// Init Shared memory(kamelas)
	if(FALSE == g_mmi.Init())
	{
		::MessageBox(NULL, _T("Failed to Init Communication"), _T("Error") , MB_OK);
		return (FALSE);
	}

	// Init NV
	g_pNV = pNvSingleTon;
	if(FALSE == g_pNV->Open())
	{
		::MessageBox(NULL, _T("err : failed to init nv"), _T("Error"), MB_OK);
		return (FALSE);
	}

	for(int nMtNo = 0; nMtNo < MAX_MT_NO; nMtNo++)
	{
		g_mt[nMtNo].m_pTable = (PVATable*)&g_pNV->m_pMotTable[nMtNo];
	}

	// negrete
	if (NEGRETE)
	{
		if (FALSE == g_Piper.Init(1, L"SeqManager", L"SEQ_QUEMAP"))
			return (FALSE);

		if (FALSE == g_TpBase.Init(L"SEQ_QUEMAP"))
			return (FALSE);
	}

	return (TRUE);
}


//------------------------------------------------------------------
BOOL CSEQ::InitPnematic(void)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SET IO
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	g_pm[CYL_MGZ_IN_STOPPER_UD_01].Set(CYL_MGZ_IN_STOPPER_UD_01, iCylMzInStopperUp01, iCylMzInStopperDw01, oCylMzInStopperUp01, oCylMzInStopperDw01);
	g_pm[CYL_MGZ_IN_STOPPER_UD_02].Set(CYL_MGZ_IN_STOPPER_UD_02, iCylMzInStopperUp02, iCylMzInStopperDw02, oCylMzInStopperUp02, oCylMzInStopperDw02);
	g_pm[CYL_MGZ_IN_STOPPER_UD_03].Set(CYL_MGZ_IN_STOPPER_UD_03, iCylMzInStopperUp03, iCylMzInStopperDw03, oCylMzInStopperUp03, oCylMzInStopperDw03);
	g_pm[CYL_MGZ_IN_LOAD_STOPPER_UD].Set(CYL_MGZ_IN_LOAD_STOPPER_UD, iCylMzLoadStopperUp, iCylMzLoadStopperDw, oCylMzLoadStopperUp, oCylMzLoadStopperDw);

	g_pm[CYL_MGZ_OUT_STOPPER_FB_01].Set(CYL_MGZ_OUT_STOPPER_FB_01, iCylMzOutStopperFwd01, iCylMzOutStopperBwd01, oCylMzOutStopperFwd01, pmDUMMY_IO);
	g_pm[CYL_MGZ_OUT_STOPPER_FB_02].Set(CYL_MGZ_OUT_STOPPER_FB_02, iCylMzOutStopperFwd02, iCylMzOutStopperBwd02, oCylMzOutStopperFwd02, pmDUMMY_IO);
	g_pm[CYL_MGZ_CLAMP_OC].Set(CYL_MGZ_CLAMP_OC, iCylMzClampOpen, iCylMzClampClose, oCylMzClampOp, oCylMzClampCl);
	g_pm[CYL_MGZ_CLAMP_ALIGN_FB].Set(CYL_MGZ_CLAMP_ALIGN_FB, iCylMzClampAlignFwd, iCylMzClampAlignBwd, oCylMzClampAlignFwd, pmDUMMY_IO);
	g_pm[CYL_RAIL_GRIPPER_OC].Set(CYL_RAIL_GRIPPER_OC, iCylRailGripperOp, iCylRailGripperCl, oCylRailGripperOp, oCylRailGripperCl);
	
	g_pm[SOL_INDEX_STAGE_KIT_OC_01].Set(SOL_INDEX_STAGE_KIT_OC_01, pmDUMMY_IO, pmDUMMY_IO, oSolIndexKitOpen01, pmDUMMY_IO);
	g_pm[SOL_INDEX_STAGE_KIT_OC_02].Set(SOL_INDEX_STAGE_KIT_OC_02, pmDUMMY_IO, pmDUMMY_IO, oSolIndexKitOpen02, pmDUMMY_IO);
	g_pm[SOL_INDEX_STAGE_KIT_OC_03].Set(SOL_INDEX_STAGE_KIT_OC_03, pmDUMMY_IO, pmDUMMY_IO, oSolIndexKitOpen03, pmDUMMY_IO);
	g_pm[SOL_INDEX_STAGE_KIT_OC_04].Set(SOL_INDEX_STAGE_KIT_OC_04, pmDUMMY_IO, pmDUMMY_IO, oSolIndexKitOpen04, pmDUMMY_IO);
	g_pm[CYL_INDEX_DUST_SHUTTER_OC_01].Set(CYL_INDEX_DUST_SHUTTER_OC_01, iCylIndexDustShutterOpen01, iCylIndexDustShutterClose01, pmDUMMY_IO, oCylIndexDustShutterClose01);
	g_pm[CYL_INDEX_DUST_SHUTTER_OC_02].Set(CYL_INDEX_DUST_SHUTTER_OC_02, iCylIndexDustShutterOpen02, iCylIndexDustShutterClose02, pmDUMMY_IO, oCylIndexDustShutterClose02);
	g_pm[CYL_INDEX_DUST_SHUTTER_OC_03].Set(CYL_INDEX_DUST_SHUTTER_OC_03, iCylIndexDustShutterOpen03, iCylIndexDustShutterClose03, pmDUMMY_IO, oCylIndexDustShutterClose03);
	g_pm[CYL_INDEX_DUST_SHUTTER_OC_04].Set(CYL_INDEX_DUST_SHUTTER_OC_04, iCylIndexDustShutterOpen04, iCylIndexDustShutterClose04, pmDUMMY_IO, oCylIndexDustShutterClose04);
	g_pm[SOL_INDEX_STAGE_AIR_BLOW_0102].Set(SOL_INDEX_STAGE_AIR_BLOW_0102, pmDUMMY_IO, pmDUMMY_IO, oSpare0908, pmDUMMY_IO);
	g_pm[SOL_INDEX_STAGE_AIR_BLOW_0304].Set(SOL_INDEX_STAGE_AIR_BLOW_0304, pmDUMMY_IO, pmDUMMY_IO, oSpare0909, pmDUMMY_IO);
	
	g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_01].Set(CYL_INDEX_MASK_KIT_FIX_L_FB_01, iCylIndexMaskKitFixLFwd01, iCylIndexMaskKitFixLBwd01, oCylIndexMaskKitFixFwd01, pmDUMMY_IO);
	g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_01].Set(CYL_INDEX_MASK_KIT_FIX_R_FB_01, iCylIndexMaskKitFixRFwd01, iCylIndexMaskKitFixRBwd01, oCylIndexMaskKitFixFwd01, pmDUMMY_IO);
	g_pm[CYL_INDEX_MASK_KIT_FIX_UD_01].Set(CYL_INDEX_MASK_KIT_FIX_UD_01, iCylIndexMaskKitFixUp01, iCylIndexMaskKitFixDw01, oCylIndexMaskKitFixUp01, pmDUMMY_IO);
	g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_02].Set(CYL_INDEX_MASK_KIT_FIX_L_FB_02, iCylIndexMaskKitFixLFwd02, iCylIndexMaskKitFixLBwd02, oCylIndexMaskKitFixFwd02, pmDUMMY_IO);
	g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_02].Set(CYL_INDEX_MASK_KIT_FIX_R_FB_02, iCylIndexMaskKitFixRFwd02, iCylIndexMaskKitFixRBwd02, oCylIndexMaskKitFixFwd02, pmDUMMY_IO);
	g_pm[CYL_INDEX_MASK_KIT_FIX_UD_02].Set(CYL_INDEX_MASK_KIT_FIX_UD_02, iCylIndexMaskKitFixUp02, iCylIndexMaskKitFixDw02, oCylIndexMaskKitFixUp02, pmDUMMY_IO);
	g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_03].Set(CYL_INDEX_MASK_KIT_FIX_L_FB_03, iCylIndexMaskKitFixLFwd03, iCylIndexMaskKitFixLBwd03, oCylIndexMaskKitFixFwd03, pmDUMMY_IO);
	g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_03].Set(CYL_INDEX_MASK_KIT_FIX_R_FB_03, iCylIndexMaskKitFixRFwd03, iCylIndexMaskKitFixRBwd03, oCylIndexMaskKitFixFwd03, pmDUMMY_IO);
	g_pm[CYL_INDEX_MASK_KIT_FIX_UD_03].Set(CYL_INDEX_MASK_KIT_FIX_UD_03, iCylIndexMaskKitFixUp03, iCylIndexMaskKitFixDw03, oCylIndexMaskKitFixUp03, pmDUMMY_IO);
	g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_04].Set(CYL_INDEX_MASK_KIT_FIX_L_FB_04, iCylIndexMaskKitFixLFwd04, iCylIndexMaskKitFixLBwd04, oCylIndexMaskKitFixFwd04, pmDUMMY_IO);
	
	g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_04].Set(CYL_INDEX_MASK_KIT_FIX_R_FB_04, iCylIndexMaskKitFixRFwd04, iCylIndexMaskKitFixRBwd04, oCylIndexMaskKitFixFwd04, pmDUMMY_IO);
	g_pm[CYL_INDEX_MASK_KIT_FIX_UD_04].Set(CYL_INDEX_MASK_KIT_FIX_UD_04, iCylIndexMaskKitFixUp04, iCylIndexMaskKitFixDw04, oCylIndexMaskKitFixUp04, pmDUMMY_IO);	
	g_pm[CYL_MASK_KIT_PICKER_UD_01].Set(CYL_MASK_KIT_PICKER_UD_01, iCylMaskPickerUp01, iCylMaskPickerDw01, oCylMaskPickerUp01, oCylMaskPickerDw01);	
	g_pm[CYL_MASK_KIT_PICKER_UD_02].Set(CYL_MASK_KIT_PICKER_UD_02, iCylMaskPickerUp02, iCylMaskPickerDw02, oCylMaskPickerUp02, oCylMaskPickerDw02);	
	g_pm[CYL_MASK_KIT_PICKER_UD_03].Set(CYL_MASK_KIT_PICKER_UD_03, iCylMaskPickerUp03, iCylMaskPickerDw03, oCylMaskPickerUp03, oCylMaskPickerDw03);	
	g_pm[CYL_MASK_KIT_PICKER_UD_04].Set(CYL_MASK_KIT_PICKER_UD_04, iCylMaskPickerUp04, iCylMaskPickerDw04, oCylMaskPickerUp04, oCylMaskPickerDw04);	
	g_pm[CYL_MASK_KIT_PICKER_OC_01].Set(CYL_MASK_KIT_PICKER_OC_01, iCylMaskPickerOp01, iCylMaskPickerCl01, oCylMaskPickerOp01, pmDUMMY_IO);	
	g_pm[CYL_MASK_KIT_PICKER_OC_02].Set(CYL_MASK_KIT_PICKER_OC_02, iCylMaskPickerOp02, iCylMaskPickerCl02, oCylMaskPickerOp02, pmDUMMY_IO);	
	g_pm[CYL_MASK_KIT_PICKER_OC_03].Set(CYL_MASK_KIT_PICKER_OC_03, iCylMaskPickerOp03, iCylMaskPickerCl03, oCylMaskPickerOp03, pmDUMMY_IO);	
	g_pm[CYL_MASK_KIT_PICKER_OC_04].Set(CYL_MASK_KIT_PICKER_OC_04, iCylMaskPickerOp04, iCylMaskPickerCl04, oCylMaskPickerOp04, pmDUMMY_IO);	

	g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].Set(CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01, iCylRouterBitClampUp01, iCylRouterBitClampDw01, oCylRouterBitClampUp01, oCylRouterBitClampDw01);
	g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].Set(CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02, iCylRouterBitClampUp02, iCylRouterBitClampDw02, oCylRouterBitClampUp02, oCylRouterBitClampDw02);
	g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_OC_01].Set(CYL_ROUTER_BIT_CHANGE_CLAMP_OC_01, iCylRouterBitClampOp01, iCylRouterBitClampCl01, oCylRouterBitClampOp01, pmDUMMY_IO);
	g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_OC_02].Set(CYL_ROUTER_BIT_CHANGE_CLAMP_OC_02, iCylRouterBitClampOp02, iCylRouterBitClampCl02, oCylRouterBitClampOp02, pmDUMMY_IO);
	g_pm[SOL_SPD_AIR_BLOW_0102].Set(SOL_SPD_AIR_BLOW_0102, pmDUMMY_IO, pmDUMMY_IO, oSolSpindleAirBlow0102, pmDUMMY_IO);
	g_pm[SOL_SPD_AIR_BLOW_0304].Set(SOL_SPD_AIR_BLOW_0304, pmDUMMY_IO, pmDUMMY_IO, oSolSpindleAirBlow0304, pmDUMMY_IO);
	g_pm[SOL_SPD_CHUCK_OC_01].Set(SOL_SPD_CHUCK_OC_01, pmDUMMY_IO, pmDUMMY_IO, oSolSpindleChuckOpen01, pmDUMMY_IO);
	g_pm[SOL_SPD_CHUCK_OC_02].Set(SOL_SPD_CHUCK_OC_02, pmDUMMY_IO, pmDUMMY_IO, oSolSpindleChuckOpen02, pmDUMMY_IO);
	g_pm[SOL_SPD_CHUCK_OC_03].Set(SOL_SPD_CHUCK_OC_03, pmDUMMY_IO, pmDUMMY_IO, oSolSpindleChuckOpen03, pmDUMMY_IO);
	g_pm[SOL_SPD_CHUCK_OC_04].Set(SOL_SPD_CHUCK_OC_04, pmDUMMY_IO, pmDUMMY_IO, oSolSpindleChuckOpen04, pmDUMMY_IO);

	g_pm[SOL_ROUTER_IONIZER_01].Set(SOL_ROUTER_IONIZER_01, pmDUMMY_IO, pmDUMMY_IO, oSolRouterIonizer01, pmDUMMY_IO);
	g_pm[SOL_ROUTER_IONIZER_02].Set(SOL_ROUTER_IONIZER_02, pmDUMMY_IO, pmDUMMY_IO, oSolRouterIonizer02, pmDUMMY_IO);
	g_pm[SOL_ROUTER_IONIZER_03].Set(SOL_ROUTER_IONIZER_03, pmDUMMY_IO, pmDUMMY_IO, oSolRouterIonizer03, pmDUMMY_IO);
	g_pm[SOL_ROUTER_IONIZER_04].Set(SOL_ROUTER_IONIZER_04, pmDUMMY_IO, pmDUMMY_IO, oSolRouterIonizer04, pmDUMMY_IO);
	g_pm[VAC_OUTPNP].Set(VAC_OUTPNP, iVacOutPnp, pmDUMMY_IO, pmDUMMY_IO, oVacOutPnp);
	//g_pm[VAC_OUTPNP].Set(VAC_OUTPNP, iVacOutPnp, pmDUMMY_IO, oVacOutPnp, pmDUMMY_IO); // 접점 바꾸고난뒤 삭제 220504
	//g_pm[VAC_OUTPNP].Set(VAC_OUTPNP, iVacOutPnp, pmDUMMY_IO, pmDUMMY_IO, oVacOutPnp); // 이전
	g_pm[VAC_OUTPNP_EJECT].Set(VAC_OUTPNP_EJECT, pmDUMMY_IO, pmDUMMY_IO, oVacOutPnpEject, pmDUMMY_IO);
	g_pm[CYL_OUTPNP_SCRAP_OC_F].Set(CYL_OUTPNP_SCRAP_OC_F, iCylOutPnpScrapOpF, iCylOutPnpScrapClF, oCylOutPnpScrapClampOp, oCylOutPnpScrapClampCl);
	g_pm[CYL_OUTPNP_SCRAP_OC_R].Set(CYL_OUTPNP_SCRAP_OC_R, iCylOutPnpScrapOpR, iCylOutPnpScrapClR, oCylOutPnpScrapClampOp, oCylOutPnpScrapClampCl);
	g_pm[CYL_OUTPNP_SCRAP_UD].Set(CYL_OUTPNP_SCRAP_UD, iCylOutPnpScrapUp, iCylOutPnpScrapDw, pmDUMMY_IO, oCylOutPnpScrapDw);	
	g_pm[CYL_OUTPNP_SCRAP_FIX_UD_F].Set(CYL_OUTPNP_SCRAP_FIX_UD_F, iCylOutPnpScrapFixUpF, iCylOutPnpScrapFixDwF, oCylOutPnpScrapFixUp, pmDUMMY_IO);
	g_pm[CYL_OUTPNP_SCRAP_FIX_UD_R].Set(CYL_OUTPNP_SCRAP_FIX_UD_R, iCylOutPnpScrapFixUpR, iCylOutPnpScrapFixDwR, oCylOutPnpScrapFixUp, pmDUMMY_IO);
	g_pm[SOL_OUTPNP_KIT_CLAMP_OC].Set(SOL_OUTPNP_KIT_CLAMP_OC, pmDUMMY_IO, pmDUMMY_IO, oSolOutPnpKitOpen, pmDUMMY_IO);

	g_pm[SPINDLE_F_01].Set(SPINDLE_F_01, iSpindleRunF_01, pmDUMMY_IO, oSpindleRunF_01, pmDUMMY_IO);
	g_pm[SPINDLE_R_01].Set(SPINDLE_R_01, iSpindleRunR_01, pmDUMMY_IO, oSpindleRunR_01, pmDUMMY_IO);

	g_pm[SPINDLE_F_02].Set(SPINDLE_F_02, iSpindleRunF_02, pmDUMMY_IO, oSpindleRunF_02, pmDUMMY_IO);
	g_pm[SPINDLE_R_02].Set(SPINDLE_R_02, iSpindleRunR_02, pmDUMMY_IO, oSpindleRunR_02, pmDUMMY_IO);

	g_pm[CYL_RAIL_GRIPPER_FB].Set(CYL_RAIL_GRIPPER_FB, iCylRailFwd, iCylRailBwd, oCylRailFwd, oCylRailBwd);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SET ERROR
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	g_pm[CYL_MGZ_IN_STOPPER_UD_01].SetErr(7000, ER_CYL_MGZ_IN_STOPPER_UD_01, pmUNCERTAIN);
	g_pm[CYL_MGZ_IN_STOPPER_UD_02].SetErr(7000, ER_CYL_MGZ_IN_STOPPER_UD_02, pmUNCERTAIN);
	g_pm[CYL_MGZ_IN_STOPPER_UD_03].SetErr(7000, ER_CYL_MGZ_IN_STOPPER_UD_03, pmUNCERTAIN);
	g_pm[CYL_MGZ_IN_LOAD_STOPPER_UD].SetErr(7000, ER_CYL_MGZ_LOAD_STOPPER_UD, pmUNCERTAIN);
	g_pm[CYL_MGZ_OUT_STOPPER_FB_01].SetErr(7000, ER_CYL_MGZ_OUT_STOPPER_FB_01, pmUNCERTAIN);
	g_pm[CYL_MGZ_OUT_STOPPER_FB_02].SetErr(7000, ER_CYL_MGZ_OUT_STOPPER_FB_02, pmUNCERTAIN);

	g_pm[CYL_MGZ_CLAMP_OC].SetErr(7000, ER_CYL_MGZ_CLAMP_OC, pmUNCERTAIN);
	g_pm[CYL_MGZ_CLAMP_ALIGN_FB].SetErr(7000, ER_CYL_MGZ_CLAMP_ALIGN_FB, pmUNCERTAIN);
	g_pm[CYL_RAIL_GRIPPER_OC].SetErr(7000, ER_CYL_RAIL_GRIPPER_OC, pmUNCERTAIN);
	g_pm[SOL_INDEX_STAGE_KIT_OC_01].SetErr(7000, ER_SOL_INDEX_STAGE_KIT_OC_01, pmUNCERTAIN);
	g_pm[SOL_INDEX_STAGE_KIT_OC_02].SetErr(7000, ER_SOL_INDEX_STAGE_KIT_OC_02, pmUNCERTAIN);
	g_pm[SOL_INDEX_STAGE_KIT_OC_03].SetErr(7000, ER_SOL_INDEX_STAGE_KIT_OC_03, pmUNCERTAIN);
	g_pm[SOL_INDEX_STAGE_KIT_OC_04].SetErr(7000, ER_SOL_INDEX_STAGE_KIT_OC_04, pmUNCERTAIN);
	g_pm[CYL_INDEX_DUST_SHUTTER_OC_01].SetErr(7000, ER_CYL_INDEX_DUST_SHUTTER_OC_01, pmUNCERTAIN);
	g_pm[CYL_INDEX_DUST_SHUTTER_OC_02].SetErr(7000, ER_CYL_INDEX_DUST_SHUTTER_OC_02, pmUNCERTAIN);
	g_pm[CYL_INDEX_DUST_SHUTTER_OC_03].SetErr(7000, ER_CYL_INDEX_DUST_SHUTTER_OC_03, pmUNCERTAIN);
	g_pm[CYL_INDEX_DUST_SHUTTER_OC_04].SetErr(7000, ER_CYL_INDEX_DUST_SHUTTER_OC_04, pmUNCERTAIN);
	g_pm[SOL_INDEX_STAGE_AIR_BLOW_0102].SetErr(7000, ER_SOL_INDEX_STAGE_AIR_BLOW_0102, pmUNCERTAIN);
	g_pm[SOL_INDEX_STAGE_AIR_BLOW_0304].SetErr(7000, ER_SOL_INDEX_STAGE_AIR_BLOW_0304, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_01].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_L_FB_01, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_01].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_R_FB_01, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_UD_01].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_UD_01, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_02].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_L_FB_02, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_02].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_R_FB_02, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_UD_02].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_UD_02, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_03].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_L_FB_03, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_03].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_R_FB_03, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_UD_03].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_UD_03, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_L_FB_04].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_L_FB_04, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_R_FB_04].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_R_FB_04, pmUNCERTAIN);
	g_pm[CYL_INDEX_MASK_KIT_FIX_UD_04].SetErr(7000, ER_CYL_INDEX_MASK_KIT_FIX_UD_04, pmUNCERTAIN);
	g_pm[CYL_MASK_KIT_PICKER_UD_01].SetErr(7000, ER_CYL_MASK_KIT_PICKER_UD_01, pmUNCERTAIN);
	g_pm[CYL_MASK_KIT_PICKER_UD_02].SetErr(7000, ER_CYL_MASK_KIT_PICKER_UD_02, pmUNCERTAIN);
	g_pm[CYL_MASK_KIT_PICKER_UD_03].SetErr(7000, ER_CYL_MASK_KIT_PICKER_UD_03, pmUNCERTAIN);
	g_pm[CYL_MASK_KIT_PICKER_UD_04].SetErr(7000, ER_CYL_MASK_KIT_PICKER_UD_04, pmUNCERTAIN);
	g_pm[CYL_MASK_KIT_PICKER_OC_01].SetErr(7000, ER_CYL_MASK_KIT_PICKER_OC_01, pmUNCERTAIN);
	g_pm[CYL_MASK_KIT_PICKER_OC_02].SetErr(7000, ER_CYL_MASK_KIT_PICKER_OC_02, pmUNCERTAIN);
	g_pm[CYL_MASK_KIT_PICKER_OC_03].SetErr(7000, ER_CYL_MASK_KIT_PICKER_OC_03, pmUNCERTAIN);
	g_pm[CYL_MASK_KIT_PICKER_OC_04].SetErr(7000, ER_CYL_MASK_KIT_PICKER_OC_04, pmUNCERTAIN);
	g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01].SetErr(7000, ER_CYL_ROUTER_BIT_CHANGE_CLAMP_UD_01, pmUNCERTAIN);
	g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02].SetErr(7000, ER_CYL_ROUTER_BIT_CHANGE_CLAMP_UD_02, pmUNCERTAIN);
	g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_OC_01].SetErr(7000, ER_CYL_ROUTER_BIT_CHANGE_CLAMP_OC_01, pmUNCERTAIN);
	g_pm[CYL_ROUTER_BIT_CHANGE_CLAMP_OC_02].SetErr(7000, ER_CYL_ROUTER_BIT_CHANGE_CLAMP_OC_02, pmUNCERTAIN);
	g_pm[SOL_SPD_AIR_BLOW_0102].SetErr(7000, ER_SOL_SPD_AIR_BLOW_0102, pmUNCERTAIN);
	g_pm[SOL_SPD_AIR_BLOW_0304].SetErr(7000, ER_SOL_SPD_AIR_BLOW_0304, pmUNCERTAIN);
	g_pm[SOL_SPD_CHUCK_OC_01].SetErr(7000, ER_SOL_SPD_CHUCK_OC_01, pmUNCERTAIN);
	g_pm[SOL_SPD_CHUCK_OC_02].SetErr(7000, ER_SOL_SPD_CHUCK_OC_02, pmUNCERTAIN);
	g_pm[SOL_SPD_CHUCK_OC_03].SetErr(7000, ER_SOL_SPD_CHUCK_OC_03, pmUNCERTAIN);
	g_pm[SOL_SPD_CHUCK_OC_04].SetErr(7000, ER_SOL_SPD_CHUCK_OC_04, pmUNCERTAIN);
	g_pm[SOL_ROUTER_IONIZER_01].SetErr(7000, ER_SOL_ROUTER_IONIZER_01, pmUNCERTAIN);
	g_pm[SOL_ROUTER_IONIZER_02].SetErr(7000, ER_SOL_ROUTER_IONIZER_02, pmUNCERTAIN);
	g_pm[SOL_ROUTER_IONIZER_03].SetErr(7000, ER_SOL_ROUTER_IONIZER_03, pmUNCERTAIN);
	g_pm[SOL_ROUTER_IONIZER_04].SetErr(7000, ER_SOL_ROUTER_IONIZER_04, pmUNCERTAIN);
	g_pm[VAC_OUTPNP].SetErr(20000, ER_VAC_OUTPNP, pmUNCERTAIN);
	g_pm[VAC_OUTPNP_EJECT].SetErr(7000, ER_VAC_OUTPNP_EJECT, pmUNCERTAIN);
	g_pm[CYL_OUTPNP_SCRAP_OC_F].SetErr(10000, ER_CYL_OUTPNP_SCRAP_OC_F, pmUNCERTAIN);
	g_pm[CYL_OUTPNP_SCRAP_OC_R].SetErr(10000, ER_CYL_OUTPNP_SCRAP_OC_R, pmUNCERTAIN);
	g_pm[CYL_OUTPNP_SCRAP_UD].SetErr(10000, ER_CYL_OUTPNP_SCRAP_UD, pmUNCERTAIN);
	g_pm[CYL_OUTPNP_SCRAP_FIX_UD_F].SetErr(10000, ER_CYL_OUTPNP_SCRAP_FIX_UD_F, pmUNCERTAIN);
	g_pm[CYL_OUTPNP_SCRAP_FIX_UD_R].SetErr(10000, ER_CYL_OUTPNP_SCRAP_FIX_UD_R, pmUNCERTAIN);
	g_pm[SOL_OUTPNP_KIT_CLAMP_OC].SetErr(7000, ER_SOL_OUTPNP_KIT_CLAMP_OC, pmUNCERTAIN);
	g_pm[SPINDLE_F_01].SetErr(7000, ER_SPINDLE_F_01, pmUNCERTAIN);
	g_pm[SPINDLE_R_01].SetErr(7000, ER_SPINDLE_R_01, pmUNCERTAIN);

	g_pm[SPINDLE_F_02].SetErr(7000, ER_SPINDLE_F_02, pmUNCERTAIN);
	g_pm[SPINDLE_R_02].SetErr(7000, ER_SPINDLE_R_02, pmUNCERTAIN);

	g_pm[CYL_RAIL_GRIPPER_FB].SetErr(7000, ER_CYL_RAIL_GRIPPER_FB, pmUNCERTAIN);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SET DEFAULT POS
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(!g_pNV->NDm(existOutPnpPcb))
	{
		//g_pm[VAC_OUTPNP_EJECT].Actuate(pmOFF);
		g_outPnp.AnalogVac(TRUE);
		g_pm[VAC_OUTPNP].Actuate(pmOFF);
	}

	return (TRUE);
}

//------------------------------------------------------------------
BOOL CSEQ::InitAjin(bool shouldInit)
{
	g_ajinLib.Close();
	Sleep(1000);
	g_ajinLib.Open(false);
	Sleep(2000);

	g_dIn.m_nMaxCh = DI_CH_CNT;
	g_dIn.m_nId[0] = 0;
	g_dIn.m_nId[1] = 1;
	g_dIn.m_nId[2] = 2;
	g_dIn.m_nId[3] = 3;
	g_dIn.m_nId[4] = 4;
	g_dIn.m_nId[5] = 5;
	g_dIn.m_nId[6] = 6;
	g_dIn.m_nId[7] = 7;
	g_dIn.m_nId[8] = 8;
	g_dIn.m_nId[9] = 9;
	g_dIn.m_nId[10] = 10;
	g_dIn.m_nId[11] = 11;
	g_dIn.m_nId[12] = 12;

	g_dOut.m_nMaxCh = DO_CH_CNT;
	g_dOut.m_nId[0] = 13;
	g_dOut.m_nId[1] = 14;
	g_dOut.m_nId[2] = 15;
	g_dOut.m_nId[3] = 16;
	g_dOut.m_nId[4] = 17;
	g_dOut.m_nId[5] = 18;
	g_dOut.m_nId[6] = 19;
	g_dOut.m_nId[7] = 20;
	g_dOut.m_nId[8] = 21;
	g_dOut.m_nId[9] = 22;

	if(!IsModuleCntErr(AXT_SIO_RDI32RTEX, DI_MODULE_CNT))
	{
		::MessageBox(NULL, _T("D/I Module Count Error!!!"), _T("Error"), MB_OK);
		return (FALSE);
	}

	if(!IsModuleCntErr(AXT_SIO_RDO32RTEX, DO_MODULE_CNT))
	{
		::MessageBox(NULL, _T("D/O Module Count Error!!!"), _T("Error"), MB_OK);
		return (FALSE);
	}
	
	BOOL bRet = g_aIn.Init(16);
	/*
	if(FALSE == bRet)
	{
		::MessageBox(NULL, _T("D/A Module Input Count Error!!!"), _T("Error") , MB_OK);
		return (FALSE);
	}
	*/
	//g_aInput.SetTriggerMode(); //Module No를 뭘 넣어줘야 하는지 ?
	//g_aInput.SetTriggerMode(0, NORMAL_MODE); //Module No를 뭘 넣어줘야 하는지 ?

	for(int nCh = 0; nCh < 16; nCh++)
	{
		g_aIn.SetRange(nCh, 0, 10);
	}

	// 4채널 지정
	bRet = g_ao.Init(8);

	if(FALSE == bRet)
	{
		::MessageBox(NULL, _T("D/A Module Output Count Error!!!"), _T("Error") , MB_OK);
		return (FALSE);
	}

	for(int nCh = 0; nCh < 4; nCh++)
	{
		g_ao.SetRange(nCh, -10, 10);
	}
	
	g_dOut.ReadAll();
	InterfaceAllOff(TRUE);
	Sleep(300);

	g_dOut.On(oPwrMtMain01, TRUE);
	g_dOut.On(oPwrMtMain02, TRUE);
	Sleep(500);

	g_dOut.On(oPwrMtIndexX01, TRUE);
	g_dOut.On(oPwrMtIndexX02, TRUE);
	g_dOut.On(oPwrMtRouterY01, TRUE);
	Sleep(300);

	g_dOut.On(oPwrMtRouterW0102, TRUE);
	g_dOut.On(oPwrMtIndexX03, TRUE);
	g_dOut.On(oPwrMtIndexX04, TRUE);
	Sleep(300);

	g_dOut.On(oPwrMtRouterY02, TRUE);
	g_dOut.On(oPwrMtLdY, TRUE);
	Sleep(300);

	g_dOut.On(oPwrMtLdZ, TRUE);
	g_dOut.On(oPwrMtPusherX_RailGripperX, TRUE);
	Sleep(300);

	g_dOut.On(oPwrMtInPnpY, TRUE);
	g_dOut.On(oPwrMtInPnpZ_AdcZ_AdcX, TRUE);
	Sleep(300);

	g_dOut.On(oPwrMtIndexT0102, TRUE);
	g_dOut.On(oPwrMtIndexT0304, TRUE);
	Sleep(300);

	g_dOut.On(oPwrMtRouterZ0102, TRUE);
	Sleep(300);

	g_dOut.On(oPwrMtRouterZ0304, TRUE);
	g_dOut.On(oPwrMtOutPnpY, TRUE);
	Sleep(300);

	g_dOut.On(oPwrMtOutPnpZ, TRUE);
	g_dOut.On(oPwrMtOutPnpX_InPnpClampY, TRUE);
	Sleep(300);


	g_dOut.On(oPwrMtMgzLiftZ, TRUE);
	Sleep(1000);

	BOOL isOk = g_ajinLib.SSCNetIII(SSCNET_BOARD2_NO);
	if(!isOk)
	{
		::MessageBox(NULL, _T("Failed to search sscnet board!!"), _T("Error"), MB_OK);
		return (FALSE);
	}

	g_ajinLib.Close();
	Sleep(1000);
	g_ajinLib.Open(false);

	if(!g_ajinLib.IsAxisCntErr(MAX_MT_NO))
	{
		::MessageBox(NULL, _T("Failed to axis count !!"), _T("Error"), MB_OK);
		return (FALSE);
	}

	if(!g_ajinLib.LoadMotorPara())
	{
		::MessageBox(NULL, _T("Failed to Open MotorPara.mot"), _T("Error"), MB_OK);
		return (FALSE);
	}

	int nMtNo = 0;
	DWORD dwStopMode, dwPot, dwNot;
	if(shouldInit)
	{
		for(nMtNo = 0; nMtNo < MAX_MT_NO; nMtNo++)
		{
			g_mt[nMtNo].m_config.axisNo = nMtNo;
			g_mt[nMtNo].m_config.homeIdx = 0;
			g_mt[nMtNo].m_state.isHome = FALSE;

			// set use/skip
			AxmSignalGetLimit(g_mt[nMtNo].m_config.axisNo, &dwStopMode, &dwPot, &dwNot);
			g_mt[nMtNo].m_config.enCw  = !(UNUSED == dwPot);
			g_mt[nMtNo].m_config.enCCw = !(UNUSED == dwNot);
		}
	}

	g_mt[MT_INDEX_X_01].SetElecGearRatio(1, 1);
	g_mt[MT_INDEX_X_02].SetElecGearRatio(1, 1);
	g_mt[MT_ROUTER_Y_01].SetElecGearRatio(1, 1);
	g_mt[MT_ROUTER_W_01].SetElecGearRatio(ENCODER_22BIT, 5000);
	g_mt[MT_INDEX_X_03].SetElecGearRatio(1, 1);
	g_mt[MT_INDEX_X_04].SetElecGearRatio(1, 1);
	g_mt[MT_ROUTER_Y_02].SetElecGearRatio(1, 1);
	g_mt[MT_ROUTER_W_02].SetElecGearRatio(ENCODER_22BIT, 5000);
	g_mt[MT_LD_Y].SetElecGearRatio(ENCODER_22BIT, 5000);
	g_mt[MT_LD_Z].SetElecGearRatio(ENCODER_22BIT, 5000);
	g_mt[MT_PUSHER_X].SetElecGearRatio(ENCODER_22BIT, 9600);
	g_mt[MT_RAIL_GRIPPER_X].SetElecGearRatio(ENCODER_22BIT, 20000);
	g_mt[MT_INPNP_Y].SetElecGearRatio(ENCODER_22BIT, 20000);
	g_mt[MT_INPNP_Z].SetElecGearRatio(ENCODER_22BIT, 10000);
	g_mt[MT_INPNP_CLAMP_Y].SetElecGearRatio(ENCODER_22BIT, 4000);
	g_mt[MT_INDEX_T_01].SetElecGearRatio(ENCODER_22BIT, 273);
	g_mt[MT_INDEX_T_02].SetElecGearRatio(ENCODER_22BIT, 273);
	g_mt[MT_INDEX_T_03].SetElecGearRatio(ENCODER_22BIT, 273);
	g_mt[MT_INDEX_T_04].SetElecGearRatio(ENCODER_22BIT, 273);
	g_mt[MT_SPINDLE_Z_01].SetElecGearRatio(ENCODER_22BIT, 5000);
	g_mt[MT_SPINDLE_Z_02].SetElecGearRatio(ENCODER_22BIT, 5000);
	g_mt[MT_SPINDLE_Z_03].SetElecGearRatio(ENCODER_22BIT, 5000);
	g_mt[MT_SPINDLE_Z_04].SetElecGearRatio(ENCODER_22BIT, 5000);
	g_mt[MT_OUTPNP_Y].SetElecGearRatio(1, 1); // Linear 교체 SetElecGearRatio(ENCODER_22BIT, 20000);
	g_mt[MT_OUTPNP_Z].SetElecGearRatio(ENCODER_22BIT, 10000);
	g_mt[MT_OUTPNP_X].SetElecGearRatio(ENCODER_22BIT, 19200);
	g_mt[MT_ADC_Z].SetElecGearRatio(ENCODER_22BIT, 10000);
	g_mt[MT_ADC_X].SetElecGearRatio(ENCODER_22BIT, 5000);
	g_mt[MT_MGZ_LOAD_Z].SetElecGearRatio(ENCODER_22BIT, 2000);

	Sleep(500);

	// Linear ServoOn 시에 자극검출 하므로 Z Home 시퀀스 추가 필요
	//for(nMtNo = MT_LD_Y; nMtNo < MAX_MT_NO; nMtNo++)
	//{
	//	g_mt[nMtNo].ServoOn();
	//	g_mt[nMtNo].SetCnt(0);
	//}


	g_mt[MT_INDEX_T_01].ServoOn();
	g_mt[MT_INDEX_T_01].SetCnt(0);
	g_mt[MT_INDEX_T_02].ServoOn();
	g_mt[MT_INDEX_T_02].SetCnt(0);
	g_mt[MT_INDEX_T_03].ServoOn();
	g_mt[MT_INDEX_T_03].SetCnt(0);
	g_mt[MT_INDEX_T_04].ServoOn();
	g_mt[MT_INDEX_T_04].SetCnt(0);
	g_mt[MT_ROUTER_W_01].ServoOn();
	g_mt[MT_ROUTER_W_01].SetCnt(0);
	g_mt[MT_ROUTER_W_02].ServoOn();
	g_mt[MT_ROUTER_W_02].SetCnt(0);
	g_mt[MT_SPINDLE_Z_01].ServoOn();
	g_mt[MT_SPINDLE_Z_01].SetCnt(0);
	g_mt[MT_SPINDLE_Z_02].ServoOn();
	g_mt[MT_SPINDLE_Z_02].SetCnt(0);
	g_mt[MT_SPINDLE_Z_03].ServoOn();
	g_mt[MT_SPINDLE_Z_03].SetCnt(0);
	g_mt[MT_SPINDLE_Z_04].ServoOn();
	g_mt[MT_SPINDLE_Z_04].SetCnt(0);
	g_mt[MT_MGZ_LOAD_Z].ServoOn();
	g_mt[MT_MGZ_LOAD_Z].SetCnt(0);
	g_mt[MT_PUSHER_X].ServoOn();
	g_mt[MT_PUSHER_X].SetCnt(0);
	g_mt[MT_RAIL_GRIPPER_X].ServoOn();
	g_mt[MT_RAIL_GRIPPER_X].SetCnt(0);
	g_mt[MT_LD_Y].ServoOn();
	g_mt[MT_LD_Y].SetCnt(0);
	g_mt[MT_LD_Z].ServoOn();
	g_mt[MT_LD_Z].SetCnt(0);
	g_mt[MT_OUTPNP_Z].ServoOn();
	g_mt[MT_OUTPNP_Z].SetCnt(0);
	g_mt[MT_OUTPNP_X].ServoOn();
	g_mt[MT_OUTPNP_X].SetCnt(0);
	g_mt[MT_INPNP_CLAMP_Y].ServoOn();
	g_mt[MT_INPNP_CLAMP_Y].SetCnt(0);
	g_mt[MT_INPNP_Y].ServoOn();
	g_mt[MT_INPNP_Y].SetCnt(0);
	g_mt[MT_INPNP_Z].ServoOn();
	g_mt[MT_INPNP_Z].SetCnt(0);
	g_mt[MT_ADC_Z].ServoOn();
	g_mt[MT_ADC_Z].SetCnt(0);
	g_mt[MT_ADC_X].ServoOn();
	g_mt[MT_ADC_X].SetCnt(0);

	Sleep(1000);

	if(!g_bNoDevice)
	{
		// Index 및 Router의 자극 검출 예외처리
		// Inpnp, Outpnp 도 Z Home 시퀀스 필요함.
		AxmMoveSignalSearch(MT_SPINDLE_Z_01, -5000, 10000, HomeSensor, SIGNAL_UP_EDGE, EMERGENCY_STOP);
		AxmMoveSignalSearch(MT_SPINDLE_Z_02, -5000, 10000, HomeSensor, SIGNAL_UP_EDGE, EMERGENCY_STOP);
		AxmMoveSignalSearch(MT_SPINDLE_Z_03, -5000, 10000, HomeSensor, SIGNAL_UP_EDGE, EMERGENCY_STOP);
		AxmMoveSignalSearch(MT_SPINDLE_Z_04, -5000, 10000, HomeSensor, SIGNAL_UP_EDGE, EMERGENCY_STOP);
		AxmMoveSignalSearch(MT_INPNP_Z, -5000, 10000, HomeSensor, SIGNAL_UP_EDGE, EMERGENCY_STOP);
		AxmMoveSignalSearch(MT_OUTPNP_Z, -5000, 10000, HomeSensor, SIGNAL_UP_EDGE, EMERGENCY_STOP);

		tmAxisZHome.Reset();
		BOOL bRun  = TRUE;
		while(bRun)
		{
			if(tmAxisZHome.TmOver(20000))
			{
				AxmMoveSStop(MT_SPINDLE_Z_01);
				AxmMoveSStop(MT_SPINDLE_Z_02);
				AxmMoveSStop(MT_SPINDLE_Z_03);
				AxmMoveSStop(MT_SPINDLE_Z_04);
				AxmMoveSStop(MT_INPNP_Z);
				AxmMoveSStop(MT_OUTPNP_Z);
				printf("\n System Init Axis Z Home Fail (Time Over) !!");
				return (FALSE);
			}

			DWORD readSignal1, readSignal2, readSignal3, readSignal4, readSignal5, readSignal6; 
			AxmHomeReadSignal(MT_SPINDLE_Z_01, &readSignal1);
			AxmHomeReadSignal(MT_SPINDLE_Z_02, &readSignal2);
			AxmHomeReadSignal(MT_SPINDLE_Z_03, &readSignal3);
			AxmHomeReadSignal(MT_SPINDLE_Z_04, &readSignal4);
			AxmHomeReadSignal(MT_INPNP_Z, &readSignal5);
			AxmHomeReadSignal(MT_OUTPNP_Z, &readSignal6);

			DWORD readDriving1, readDriving2, readDriving3, readDriving4, readDriving5, readDriving6; 
			AxmStatusReadInMotion(MT_SPINDLE_Z_01, &readDriving1);
			AxmStatusReadInMotion(MT_SPINDLE_Z_02, &readDriving2);
			AxmStatusReadInMotion(MT_SPINDLE_Z_03, &readDriving3);
			AxmStatusReadInMotion(MT_SPINDLE_Z_04, &readDriving4);
			AxmStatusReadInMotion(MT_INPNP_Z, &readDriving5);
			AxmStatusReadInMotion(MT_OUTPNP_Z, &readDriving6);

			DWORD homeOk  = !!readSignal1 && !!readSignal2;
				  homeOk &= !!readSignal3 && !!readSignal4;
				  homeOk &= !!readSignal4 && !!readSignal6;
				  homeOk &= !readDriving1 && !readDriving2;
				  homeOk &= !readDriving3 && !readDriving4;
				  homeOk &= !readDriving4 && !readDriving6;

			if(homeOk)
			{
				bRun = FALSE;
				printf("\n System Init Axis Z Home Ok !!");
			}
			Sleep(1);
		}
	}

	Sleep(500);
	// Linear Servo On
	g_mt[MT_INDEX_X_01].ServoOn();
	g_mt[MT_INDEX_X_01].SetCnt(0);
	g_mt[MT_INDEX_X_02].ServoOn();
	g_mt[MT_INDEX_X_02].SetCnt(0);
	g_mt[MT_INDEX_X_03].ServoOn();
	g_mt[MT_INDEX_X_03].SetCnt(0);
	g_mt[MT_INDEX_X_04].ServoOn();
	g_mt[MT_INDEX_X_04].SetCnt(0);
	g_mt[MT_ROUTER_Y_01].ServoOn();
	g_mt[MT_ROUTER_Y_01].SetCnt(0);
	g_mt[MT_ROUTER_Y_02].ServoOn();
	g_mt[MT_ROUTER_Y_02].SetCnt(0);
	g_mt[MT_OUTPNP_Y].ServoOn();
	g_mt[MT_OUTPNP_Y].SetCnt(0);

	InitEtc();

	printf("\n Init Etc!!!");


	return (TRUE);
}


//------------------------------------------------------------------
BOOL CSEQ::InitClass(void)
{
	g_pIndex[0] = (CIndex*)&g_index01;
	g_pIndex[1] = (CIndex*)&g_index02;
	g_pIndex[2] = (CIndex*)&g_index03;
	g_pIndex[3] = (CIndex*)&g_index04;

	g_opr.isStop		= TRUE;
	g_opr.isAuto		= FALSE;
	g_opr.isEmg			= FALSE;
	g_opr.isAllHome		= FALSE;
	g_opr.isDryRun		= FALSE;

	if(1 > g_pNV->NDm(groupNo))
		g_pNV->NDm(groupNo) = 1;
	if(1 > g_pNV->NDm(jobNo))
		g_pNV->NDm(jobNo) = 1;
	
    g_pRouterData	  = (ROUTER_DATA*)&g_pNV->gerberData(gerberPos);
    g_pOffsetIndex[0] = (PATH_OFFSET*)&g_pNV->gerberData(offsetIndex01);
    g_pOffsetIndex[1] = (PATH_OFFSET*)&g_pNV->gerberData(offsetIndex02);
    g_pOffsetIndex[2] = (PATH_OFFSET*)&g_pNV->gerberData(offsetIndex03);
    g_pOffsetIndex[3] = (PATH_OFFSET*)&g_pNV->gerberData(offsetIndex04);
    g_pGerberPath	  = (GERBER_PATH*)&g_pNV->gerberData(gerberPos);

	g_lampBuzzer.Init();
    g_tenkeyOpr.Init();
	
	g_adc.Init();
	g_MgzLoadZ.Init();
	g_ldMzInConv.Init();
	g_ldMzOutConv.Init();
	g_ldMz.Init();
	g_inPnp.Init();
	g_rail.Init();
	IndexInit();
	RouterInit();
	g_outPnp.Init();
	g_lotInfo.Init();

    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	timeBeginPeriod(1);
	HANDLE hProcess  = GetCurrentProcess();
	SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
	
	// negrete
	copy2Mtd(TRUE);
	_sprintf(g_cRecipeId, L"RCP_%04d", (int)((g_pNV->NDm(groupNo) * 1000) + g_pNV->NDm(jobNo)));

	return (TRUE);
}


//------------------------------------------------------------------
BOOL CSEQ::InitEtc(void)
{
	g_dOut.On(oPwrSpindleF, TRUE);
	g_dOut.On(oPwrSpindleR, TRUE);

	g_dOut.On(oPwrIonizer01, TRUE);
	g_dOut.On(oPwrIonizer02, TRUE);
	g_dOut.On(oPwrIonizer03, TRUE);
	g_dOut.On(oPwrIonizer04, TRUE);

	Sleep(300);

	g_dOut.On(oMainAir, TRUE);

	return (TRUE);
}


//------------------------------------------------------------------
void CSEQ::Seq(void)
{
	SimpleFunc();
	g_lampBuzzer.Run();
	g_opBtn.Run();	

	g_opr.isEmg = (g_dIn.BOn(iPowerOn) || g_dIn.BOn(iEmg01) || g_dIn.BOn(iEmg02) || g_dIn.BOn(iEmg03) || g_dIn.BOn(iEmg04));
	
	if(!g_bNoDevice)
	{
		if(TRUE == g_opr.isEmg)
		{
			m_mainState.Set(S_EMG_ON);
			return;
		}
	}
	
	g_allHome.Run();
	g_tenkeyOpr.Run();	

	//-------------------------------------------
	// CYCLE-RUN 
	g_adc.CycleRun();
	g_MgzLoadZ.CycleRun();
	g_ldMzInConv.CycleRun();
	g_ldMzOutConv.CycleRun();
	g_ldMz.CycleRun();
	g_rail.CycleRun();
	g_inPnp.CycleRun();
	g_index01.CycleRun();
	g_index02.CycleRun();
	g_index03.CycleRun();
	g_index04.CycleRun();
	g_routerF.CycleRun();
	g_routerR.CycleRun();
	g_outPnp.CycleRun();
	g_lotInfo.CycleRun();;
	
	g_opr.isCycleRun  = g_adc.m_fsm.IsRun();
	g_opr.isCycleRun |= g_MgzLoadZ.m_fsm.IsRun();
	g_opr.isCycleRun |= g_ldMzInConv.m_fsm.IsRun();
	g_opr.isCycleRun |= g_ldMzOutConv.m_fsm.IsRun();
	g_opr.isCycleRun |= g_ldMz.m_fsm.IsRun();
	g_opr.isCycleRun |= g_rail.m_fsm.IsRun();
	g_opr.isCycleRun |= g_inPnp.m_fsm.IsRun();
	g_opr.isCycleRun |= g_index01.m_fsm.IsRun();
	g_opr.isCycleRun |= g_index02.m_fsm.IsRun();
	g_opr.isCycleRun |= g_index03.m_fsm.IsRun();
	g_opr.isCycleRun |= g_index04.m_fsm.IsRun();
	g_opr.isCycleRun |= g_routerF.m_fsm.IsRun();
	g_opr.isCycleRun |= g_routerR.m_fsm.IsRun();
	g_opr.isCycleRun |= g_outPnp.m_fsm.IsRun();				   
	g_opr.isCycleRun |= g_lotInfo.m_fsm.IsRun();				

	if(g_opr.isPausedStop)
		return;

	if(!g_opr.isAllHome)
		return;
	
	//-------------------------------------------
	// AUTO-RUN 
	if(FALSE == g_opr.isStop)
	{
		if(FALSE == g_opr.isAuto) // STOP->RUN 
		{
			g_opr.isAuto = TRUE;
		}
		else
		{
			g_dOut.On(oSorterAutoRun);
		
			if(!g_opr.isDoorUnlock)
			{
				if(g_pNV->NDm(mmiBtnAdcMode))
				{
					// 다른 Part도 초기상태를 유지해야 하므로 
					// 확인 후 해당 Cycle에서 빠져 나갈 수 있도록 수정
					g_adc.m_bRun     = TRUE;
					g_inPnp.m_bRun   = TRUE;
					g_index01.m_bRun = TRUE;
					g_index02.m_bRun = TRUE;
					g_index03.m_bRun = TRUE;
					g_index04.m_bRun = TRUE;
					g_outPnp.m_bRun  = TRUE;
				}
				else
				{
					g_adc.m_bRun = TRUE;
					g_MgzLoadZ.m_bRun = TRUE;
					g_ldMzInConv.m_bRun = TRUE;
					g_ldMzOutConv.m_bRun = TRUE;
					g_ldMz.m_bRun = TRUE;
					g_inPnp.m_bRun = TRUE;
					g_rail.m_bRun = TRUE;
					g_index01.m_bRun = TRUE;
					g_index02.m_bRun = TRUE;
					g_index03.m_bRun = TRUE;
					g_index04.m_bRun = TRUE;
					g_routerF.m_bRun = TRUE;
					g_routerR.m_bRun = TRUE;
					g_outPnp.m_bRun = TRUE;
					g_lotInfo.m_bRun = TRUE;
				}
			}
		}
	}
	else
	{
		g_dOut.Off(oSorterAutoRun);

		if(TRUE == g_opr.isAuto) // RUN->STOP 
		{
			g_opr.isAuto = FALSE;
		}
	}

	g_adc.AutoRun();
	g_MgzLoadZ.AutoRun();
	g_ldMzInConv.AutoRun();
	g_ldMzOutConv.AutoRun();
	g_ldMz.AutoRun();
	g_rail.AutoRun();
	g_inPnp.AutoRun();
	g_index01.AutoRun();
	g_index03.AutoRun();
	g_index02.AutoRun();
	g_index04.AutoRun();
	g_routerF.AutoRun();
	g_routerR.AutoRun();
	g_outPnp.AutoRun();
	g_lotInfo.AutoRun();
}


//-------------------------------------------------------------------
// EMG 처리..
void CSEQ::EmgOn(void)
{
	g_err.Run();
	g_mmi.Run();
	g_err.Save(ER_EMG);

	g_opr.isStop = TRUE;
	g_opr.isAuto = FALSE;
	g_opr.isAllHome = FALSE;

	if(m_mainState.Once())
	{
		InterfaceAllOff(TRUE);

		for(int nMtNo = 0; nMtNo < MAX_MT_NO; nMtNo++)
		{
			g_mt[nMtNo].ServoOff();
			g_mt[nMtNo].CancelHomeSearch();
		}
		g_dOut.Off(oPwrMtIndexX01, TRUE);
		g_dOut.Off(oPwrMtIndexX02, TRUE);
		g_dOut.Off(oPwrMtRouterY01, TRUE);
		g_dOut.Off(oPwrMtRouterW0102, TRUE);
		g_dOut.Off(oPwrMtIndexX03, TRUE);
		g_dOut.Off(oPwrMtIndexX04, TRUE);
		g_dOut.Off(oPwrMtRouterY02, TRUE);
		g_dOut.Off(oPwrMtLdY, TRUE);
		g_dOut.Off(oPwrMtLdZ, TRUE);
		g_dOut.Off(oPwrMtPusherX_RailGripperX, TRUE);
		g_dOut.Off(oPwrMtInPnpY, TRUE);
		g_dOut.Off(oPwrMtInPnpZ_AdcZ_AdcX, TRUE);
		g_dOut.Off(oPwrMtIndexT0102, TRUE);
		g_dOut.Off(oPwrMtIndexT0304, TRUE);
		g_dOut.Off(oPwrMtRouterZ0102, TRUE);
		g_dOut.Off(oPwrMtRouterZ0304, TRUE);
		g_dOut.Off(oPwrMtOutPnpY, TRUE);
		g_dOut.Off(oPwrMtOutPnpZ, TRUE);
		g_dOut.Off(oPwrMtOutPnpX_InPnpClampY, TRUE);

		g_dOut.Off(oPwrMtMgzLiftZ, TRUE);

		g_dOut.Off(oPwrMtMain01, TRUE);
		g_dOut.Off(oPwrMtMain02, TRUE);

		g_dOut.Off(oPwrSpindleF, TRUE);
		g_dOut.Off(oPwrSpindleR, TRUE);
		g_dOut.Off(oPwrIonizer01, TRUE);
		g_dOut.Off(oPwrIonizer02, TRUE);
		g_dOut.Off(oPwrIonizer03, TRUE);
		g_dOut.Off(oPwrIonizer04, TRUE);

		//-------------------------------------------
		// CYCLE-RUN IDLE
		g_adc.m_fsm.Set(C_IDLE);
		g_MgzLoadZ.m_fsm.Set(C_IDLE);
		g_ldMzInConv.m_fsm.Set(C_IDLE);
		g_ldMzOutConv.m_fsm.Set(C_IDLE);
		g_ldMz.m_fsm.Set(C_IDLE);
		g_rail.m_fsm.Set(C_IDLE);
		g_inPnp.m_fsm.Set(C_IDLE);
		g_index01.m_fsm.Set(C_IDLE);
		g_index03.m_fsm.Set(C_IDLE);
		g_index02.m_fsm.Set(C_IDLE);
		g_index04.m_fsm.Set(C_IDLE);
		g_routerF.m_fsm.Set(C_IDLE);
		g_routerR.m_fsm.Set(C_IDLE);
		g_outPnp.m_fsm.Set(C_IDLE);
		g_lotInfo.m_fsm.Set(C_IDLE);
		
		g_tenkeyOpr.m_fsm.Set(C_IDLE);
		g_tenkeyOpr.m_fsmJog.Set(C_IDLE);
		g_opBtn.m_fsm[COpBtn::swSTART].Set(C_IDLE);
		g_opBtn.m_fsm[COpBtn::swSTOP].Set(C_IDLE);
		g_opBtn.m_fsm[COpBtn::swRESET].Set(C_IDLE);
	}

	g_update.Input();
	g_update.Output();
	g_opr.isEmg  = (g_dIn.BOn(iPowerOn, TRUE) || g_dIn.BOn(iEmg01, TRUE) || g_dIn.BOn(iEmg02, TRUE) || g_dIn.BOn(iEmg03, TRUE) || g_dIn.BOn(iEmg04, TRUE));

	if(g_opr.isEmg)
		Sleep(500);
	else
	{
		Sleep(10000);
		m_mainState.Set(S_EMG_OFF);
	}
}


void CSEQ::EmgOff(void)
{
	BOOL isComp = InitAjin(FALSE);
	if(!isComp)
	{
		printf("\n Emg Off : Fail Reset Ajin!!");
		Sleep(10000);
		return;
	}

	printf("\n Emg Off : Reset Ajin Comp!!");
	m_mainState.Set(S_RUN);
}


//////////////////////////////////////////////////////////////////////////
void CSEQ::SafetyOutOff(void)
{
	InterfaceAllOff(TRUE);

	for(int nMtNo = 0; nMtNo < MAX_MT_NO; nMtNo++)
	{
		g_mt[nMtNo].ServoOff();
		g_mt[nMtNo].CancelHomeSearch();
	}

	g_dOut.Off(oPwrMtIndexX01, TRUE);
	g_dOut.Off(oPwrMtIndexX02, TRUE);
	g_dOut.Off(oPwrMtRouterY01, TRUE);
	g_dOut.Off(oPwrMtRouterW0102, TRUE);
	g_dOut.Off(oPwrMtIndexX03, TRUE);
	g_dOut.Off(oPwrMtIndexX04, TRUE);
	g_dOut.Off(oPwrMtRouterY02, TRUE);
	g_dOut.Off(oPwrMtLdY, TRUE);
	g_dOut.Off(oPwrMtLdZ, TRUE);
	g_dOut.Off(oPwrMtPusherX_RailGripperX, TRUE);
	g_dOut.Off(oPwrMtInPnpY, TRUE);
	g_dOut.Off(oPwrMtInPnpZ_AdcZ_AdcX, TRUE);
	g_dOut.Off(oPwrMtIndexT0102, TRUE);
	g_dOut.Off(oPwrMtIndexT0304, TRUE);
	g_dOut.Off(oPwrMtRouterZ0102, TRUE);
	g_dOut.Off(oPwrMtRouterZ0304, TRUE);
	g_dOut.Off(oPwrMtOutPnpY, TRUE);
	g_dOut.Off(oPwrMtOutPnpZ, TRUE);
	g_dOut.Off(oPwrMtOutPnpX_InPnpClampY, TRUE);

	g_dOut.Off(oPwrMtMgzLiftZ, TRUE);

	g_dOut.Off(oPwrMtMain01, TRUE);
	g_dOut.Off(oPwrMtMain02, TRUE);

	g_dOut.Off(oPwrSpindleF, TRUE);
	g_dOut.Off(oPwrSpindleR, TRUE);
	g_dOut.Off(oPwrIonizer01, TRUE);
	g_dOut.Off(oPwrIonizer02, TRUE);
	g_dOut.Off(oPwrIonizer03, TRUE);
	g_dOut.Off(oPwrIonizer04, TRUE);
}
