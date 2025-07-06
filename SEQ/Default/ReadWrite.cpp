#include "..\def\Includes.h"
//#include "ReadWrite.h"
//////////////////////////////////////////////////////////////////////////
CUpdate g_update;
//////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------
double CUpdate::DisplayAir(double dRunUnit, double dRunVolt, int aioNo)
{
	double Ret = (((g_aIn.m_dVolt[aioNo] * 1000) / 5514.619) - dRunUnit) * dRunVolt;

	// Ai 값 추출 파라미터
	int nAiChkNumb = (int)g_pNV->DDm(aiChkNum);

	if (g_pNV->DDm(aiChkTrg)==1)
	{
		g_pNV->DDm(aiChkInput_ai_data1) = g_aIn.m_dVolt[nAiChkNumb] * double(1000);
		g_pNV->DDm(aiChkInput_init_data) = (g_pNV->DDm(aiChkInput_display2) / (g_pNV->DDm(aiChkInput_display2) - g_pNV->DDm(aiChkInput_display1))) / 5514.619 * (g_pNV->DDm(aiChkInput_ai_data1) - ((g_pNV->DDm(aiChkInput_display1) / g_pNV->DDm(aiChkInput_display2)) * g_pNV->DDm(aiChkInput_ai_data2)));
		g_pNV->DDm(aiChkInput_ratio_data) = g_pNV->DDm(aiChkInput_display1) / ((g_pNV->DDm(aiChkInput_ai_data1) / 5514.619) - g_pNV->DDm(aiChkInput_init_data));
		g_pNV->DDm(aiChkTrg) = FALSE;
	}
	else if (g_pNV->DDm(aiChkTrg) == 2)
	{
		g_pNV->DDm(aiChkInput_ai_data2) = g_aIn.m_dVolt[nAiChkNumb] * double(1000);
		g_pNV->DDm(aiChkInput_init_data) = (g_pNV->DDm(aiChkInput_display2) / (g_pNV->DDm(aiChkInput_display2) - g_pNV->DDm(aiChkInput_display1))) / 5514.619 * (g_pNV->DDm(aiChkInput_ai_data1) - ((g_pNV->DDm(aiChkInput_display1) / g_pNV->DDm(aiChkInput_display2)) * g_pNV->DDm(aiChkInput_ai_data2)));
		g_pNV->DDm(aiChkInput_ratio_data) = g_pNV->DDm(aiChkInput_display1) / ((g_pNV->DDm(aiChkInput_ai_data1) / 5514.619) - g_pNV->DDm(aiChkInput_init_data));
		g_pNV->DDm(aiChkTrg) = FALSE;
	}
	else if (g_pNV->DDm(aiChkTrg) == 3)
	{
		g_ao.m_dVolt[aoOutPnpVacVolt] = g_pNV->Pkg(OutPnpVacSetValue);    // 2022-04-10 kys vac test
		g_pNV->DDm(aiChkTrg) = FALSE;
	}

	return (Ret);
}
//-------------------------------------------------------------------
void CUpdate::Input(void)
{
	g_dIn.ReadAll();

	g_aIn.Read();

	g_pNV->DDm(srcIonizer01)	= DisplayAir(g_pNV->DDm(IonizerRunUnit01), g_pNV->DDm(IonizerRunVolt01), aiIonizer01);
	g_pNV->DDm(srcIonizer02)	= DisplayAir(g_pNV->DDm(IonizerRunUnit02), g_pNV->DDm(IonizerRunVolt02), aiIonizer02);
	g_pNV->DDm(srcIonizer03)	= DisplayAir(g_pNV->DDm(IonizerRunUnit03), g_pNV->DDm(IonizerRunVolt03), aiIonizer03);
	g_pNV->DDm(srcIonizer04)	= DisplayAir(g_pNV->DDm(IonizerRunUnit04), g_pNV->DDm(IonizerRunVolt04), aiIonizer04);
	g_pNV->DDm(srcIndexFlow01)  = DisplayAir(g_pNV->DDm(indexFlowRunUnit01), g_pNV->DDm(indexFlowRunVolt01), aiIndexAirFlow01);
	g_pNV->DDm(srcIndexFlow02)  = DisplayAir(g_pNV->DDm(indexFlowRunUnit02), g_pNV->DDm(indexFlowRunVolt02), aiIndexAirFlow02);
	g_pNV->DDm(srcIndexFlow03)  = DisplayAir(g_pNV->DDm(indexFlowRunUnit03), g_pNV->DDm(indexFlowRunVolt03), aiIndexAirFlow03);
	g_pNV->DDm(srcIndexFlow04)  = DisplayAir(g_pNV->DDm(indexFlowRunUnit04), g_pNV->DDm(indexFlowRunVolt04), aiIndexAirFlow04);
	g_pNV->DDm(srcMainAir)		= DisplayAir(g_pNV->DDm(MainAirRunUnit), g_pNV->DDm(MainAirRunVolt), aiMainAir);
	g_pNV->DDm(srcOutPnpReg)	= DisplayAir(g_pNV->DDm(OutPnpRegAirRunUnit), g_pNV->DDm(OutPnpRegAirRunVolt), aiOutPnpRegAir);
	g_pNV->DDm(srcOutPnpFlow)   = DisplayAir(g_pNV->DDm(OutPnpAirFlowRunUnit), g_pNV->DDm(OutPnpAirFlowRunVolt), aiOutPnpVacAir);

	if(g_opr.isDryRun)
	{
		g_dIn.Set(iVacOutPnp, !g_dOut.IsOn(oVacOutPnp));
	}

	if(g_bNoDevice)
	{
		g_dIn.Set(iEmg01, TRUE);
		g_dIn.Set(iEmg02, TRUE);
		g_dIn.Set(iEmg03, TRUE);
		g_dIn.Set(iEmg04, TRUE);
	}
}


//-------------------------------------------------------------------
void CUpdate::Output(void)
{
	g_dOut.WriteAll();

	g_ao.Write();
}


//-------------------------------------------------------------------
void CUpdate::Motor(void)
{
	for(int nNo = 0; nNo < MAX_MT_NO; nNo++)
	{
		g_mt[nNo].Update();
	}
}
