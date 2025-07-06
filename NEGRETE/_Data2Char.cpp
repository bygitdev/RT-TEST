#include "..\SEQ\Def\Includes.h"

_char* w2c(char* msg)
{
	_char pStr[1024] = { 0, };

#ifdef UNICODE
	int strSize = 0;
	strSize = MultiByteToWideChar(CP_ACP, 0, msg, -1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, msg, 1024, pStr, strSize);
	return pStr;
#else
	return msg;
#endif
}

void copyPmName(void)
{
	for (int nPmCnt = 0; nPmCnt < MAX_PM_NO; nPmCnt++)
	{
		_sprintf(g_data2c.cPmName[nPmCnt], L"'C%02d'", nPmCnt); 
		//if (SPINDLE_F >= nPmCnt)
		//	_sprintf(g_data2c.cPmName[nPmCnt], L"'SPD%02d'", nPmCnt - SPINDLE_F);
		//else if (VAC_OUTPNP >= nPmCnt)
		//	_sprintf(g_data2c.cPmName[nPmCnt], L"'V%02d'", nPmCnt - VAC_OUTPNP);
		//else if(SOL_SPD_CHUCK_OC_01 >= nPmCnt)
		//	_sprintf(g_data2c.cPmName[nPmCnt], L"'S%02d'", nPmCnt - SOL_SPD_CHUCK_OC_01);
		//else
		//{
		//	_sprintf(g_data2c.cPmName[nPmCnt], L"'C%02d'", nPmCnt);
		//}
	}

	_sprintf(g_data2c.cAcMtName[0], L"'A1'");
	_sprintf(g_data2c.cAcMtName[1], L"'A2'");
	_sprintf(g_data2c.cAcMtName[2], L"'A3'");
}

void copyPmIoData(void)
{
	for (int nPmCnt = 0; nPmCnt < MAX_PM_NO; nPmCnt++)
	{
		_sprintf(g_data2c.cPmIO[nPmCnt].In[pmOFF][pmOFF]	, L"'SEN_X%d'", g_pm[nPmCnt].GetOffIO(pmOFF, 0));
		_sprintf(g_data2c.cPmIO[nPmCnt].In[pmOFF][pmON]		, L"'SEN_X%d'", g_pm[nPmCnt].GetOnIO(pmOFF, 0));
		_sprintf(g_data2c.cPmIO[nPmCnt].In[pmON][pmOFF]		, L"'SEN_X%d'", g_pm[nPmCnt].GetOffIO(pmON, 0));
		_sprintf(g_data2c.cPmIO[nPmCnt].In[pmON][pmON]		, L"'SEN_X%d'", g_pm[nPmCnt].GetOnIO(pmON, 0));
		_sprintf(g_data2c.cPmIO[nPmCnt].out[pmOFF][pmOFF]	, L"'OUT_Y%d'", g_pm[nPmCnt].GetOffIO(pmOFF, 1));
		_sprintf(g_data2c.cPmIO[nPmCnt].out[pmOFF][pmON]	, L"'OUT_Y%d'", g_pm[nPmCnt].GetOnIO(pmOFF, 1));
		_sprintf(g_data2c.cPmIO[nPmCnt].out[pmON][pmOFF]	, L"'OUT_Y%d'", g_pm[nPmCnt].GetOffIO(pmON, 1));
		_sprintf(g_data2c.cPmIO[nPmCnt].out[pmON][pmON]		, L"'OUT_Y%d'", g_pm[nPmCnt].GetOnIO(pmON, 1));
	}


	_sprintf(g_data2c.cAcIO[0], L"'OUT_Y%d'", oAcMgzInRun);
	_sprintf(g_data2c.cAcIO[1], L"'OUT_Y%d'", oAcMgzOutRun);
	_sprintf(g_data2c.cAcIO[2], L"'OUT_Y%d'", oAcMgzLoadRun);
}

void copyMtName(void)
{
	for (int nMtCnt = 0; nMtCnt < MAX_MT_NO; nMtCnt++)
	{
		_sprintf(g_data2c.cMtName[nMtCnt], L"'M%02d'", nMtCnt + 1);
	}
}

void copyDeviceId(void)
{
	_sprintf(g_data2c.cMzInConv.deviceId, L"MGZ_IN_CONV");
	_sprintf(g_data2c.cMzOutConv.deviceId, L"MGZ_OUT_CONV");
	_sprintf(g_data2c.cMzLift.deviceId, L"MGZ_LOAD");
	_sprintf(g_data2c.cLdmz.deviceId, L"LOADER");
	_sprintf(g_data2c.cRail.deviceId, L"RAIL");
	_sprintf(g_data2c.cInPnp.deviceId, L"INPNP");
	_sprintf(g_data2c.cIndex[0].deviceId, L"INDEX01");
	_sprintf(g_data2c.cIndex[1].deviceId, L"INDEX02");
	_sprintf(g_data2c.cIndex[2].deviceId, L"INDEX03");
	_sprintf(g_data2c.cIndex[3].deviceId, L"INDEX04");
	_sprintf(g_data2c.cRouter[0].deviceId, L"ROUTER01");
	_sprintf(g_data2c.cRouter[1].deviceId, L"ROUTER02");
	_sprintf(g_data2c.cOutPnp.deviceId, L"OUTPNP");
	_sprintf(g_data2c.cAdc.deviceId, L"ADC");
}

void copyEtc(void)
{
	_sprintf(g_data2c.cEtc.start, L"START");
	_sprintf(g_data2c.cEtc.end, L"END");
	_sprintf(g_data2c.cEtc.type, L"'TYPE'");
	_sprintf(g_data2c.cEtc.cyl, L"'CYLINDER'");
	_sprintf(g_data2c.cEtc.acMotor, L"'AC_MOTOR'");
	_sprintf(g_data2c.cEtc.motor, L"'MOTOR'");
	_sprintf(g_data2c.cEtc.vision, L"'VISION'");
	_sprintf(g_data2c.cEtc.sensor, L"'SENSOR'");
	_sprintf(g_data2c.cEtc.spindle, L"'SPINDLE'");
	_sprintf(g_data2c.cEtc.sol, L"'SOLENOID'");
	_sprintf(g_data2c.cEtc.vac, L"'VACCUM'");
	_sprintf(g_data2c.cEtc.actName, L"'ACT_NAME'");
	_sprintf(g_data2c.cEtc.delayTime, L"'DELAYTIME'");
	_sprintf(g_data2c.cEtc.off, L"'OFF'");
	_sprintf(g_data2c.cEtc.on, L"'ON'");
}

void copyPosName(void)
{
	for (int nIndexCnt = 0; nIndexCnt < 20; nIndexCnt++)
	{
		_sprintf(g_data2c.cMzLift.Z[nIndexCnt][_POSIDX_], L"'POS_Z'");

		_sprintf(g_data2c.cLdmz.Y[nIndexCnt][_POSIDX_]		, L"'POS_Y'");
		_sprintf(g_data2c.cLdmz.Z[nIndexCnt][_POSIDX_]		, L"'POS_Z'");
		_sprintf(g_data2c.cLdmz.PusherX[nIndexCnt][_POSIDX_], L"'POS_X'");

		_sprintf(g_data2c.cRail.X[nIndexCnt][_POSIDX_], L"'POS_X'");

		_sprintf(g_data2c.cInPnp.Y[nIndexCnt][_POSIDX_]		, L"'POS_Y'");
		_sprintf(g_data2c.cInPnp.Z[nIndexCnt][_POSIDX_]		, L"'POS_Z'");
		_sprintf(g_data2c.cInPnp.ClampY[nIndexCnt][_POSIDX_], L"'POS_Y'");

		for (int nStgCnt = 0; nStgCnt < 4; nStgCnt++)
		{
			_sprintf(g_data2c.cIndex[nStgCnt].X[nIndexCnt][_POSIDX_], L"'POS_X'");
			_sprintf(g_data2c.cIndex[nStgCnt].T[nIndexCnt][_POSIDX_], L"'POS_T'");
		}

		for (int nRouterCnt = 0; nRouterCnt < 2; nRouterCnt++)
		{
			_sprintf(g_data2c.cRouter[nRouterCnt].Y[nIndexCnt][_POSIDX_]	, L"'POS_Y'");
			_sprintf(g_data2c.cRouter[nRouterCnt].Z1[nIndexCnt][_POSIDX_]	, L"'POS_Z'");
			_sprintf(g_data2c.cRouter[nRouterCnt].Z2[nIndexCnt][_POSIDX_]	, L"'POS_Z'");
			_sprintf(g_data2c.cRouter[nRouterCnt].W[nIndexCnt][_POSIDX_]	, L"'POS_W'");
		}

		_sprintf(g_data2c.cOutPnp.Y[nIndexCnt][_POSIDX_], L"'POS_Y'");
		_sprintf(g_data2c.cOutPnp.Z[nIndexCnt][_POSIDX_], L"'POS_Z'");
		_sprintf(g_data2c.cOutPnp.X[nIndexCnt][_POSIDX_], L"'POS_X'");
		_sprintf(g_data2c.cAdc.X[nIndexCnt][_POSIDX_]	, L"'POS_X'");
		_sprintf(g_data2c.cAdc.Z[nIndexCnt][_POSIDX_]	, L"'POS_Z'");
	}
}

void copySpeedName(void)
{
	for (int nIndexCnt = 0; nIndexCnt < 20; nIndexCnt++)
	{
		_sprintf(g_data2c.cMzLift.Z[nIndexCnt][_SPDIDX_], L"'VEL_Z'");

		_sprintf(g_data2c.cLdmz.Y[nIndexCnt][_SPDIDX_], L"'VEL_Y'");
		_sprintf(g_data2c.cLdmz.Z[nIndexCnt][_SPDIDX_], L"'VEL_Z'");
		_sprintf(g_data2c.cLdmz.PusherX[nIndexCnt][_SPDIDX_], L"'VEL_X'");

		_sprintf(g_data2c.cRail.X[nIndexCnt][_SPDIDX_], L"'VEL_X'");

		_sprintf(g_data2c.cInPnp.Y[nIndexCnt][_SPDIDX_], L"'VEL_Y'");
		_sprintf(g_data2c.cInPnp.Z[nIndexCnt][_SPDIDX_], L"'VEL_Z'");
		_sprintf(g_data2c.cInPnp.ClampY[nIndexCnt][_SPDIDX_], L"'VEL_Y'");

		for (int nStgCnt = 0; nStgCnt < 4; nStgCnt++)
		{
			_sprintf(g_data2c.cIndex[nStgCnt].X[nIndexCnt][_SPDIDX_], L"'VEL_X'");
			_sprintf(g_data2c.cIndex[nStgCnt].T[nIndexCnt][_SPDIDX_], L"'VEL_T'");
		}

		for (int nRouterCnt = 0; nRouterCnt < 2; nRouterCnt++)
		{
			_sprintf(g_data2c.cRouter[nRouterCnt].Y[nIndexCnt][_SPDIDX_], L"'VEL_Y'");
			_sprintf(g_data2c.cRouter[nRouterCnt].Z1[nIndexCnt][_SPDIDX_], L"'VEL_Z'");
			_sprintf(g_data2c.cRouter[nRouterCnt].Z2[nIndexCnt][_SPDIDX_], L"'VEL_Z'");
			_sprintf(g_data2c.cRouter[nRouterCnt].W[nIndexCnt][_SPDIDX_], L"'VEL_W'");
		}

		_sprintf(g_data2c.cOutPnp.Y[nIndexCnt][_SPDIDX_], L"'VEL_Y'");
		_sprintf(g_data2c.cOutPnp.Z[nIndexCnt][_SPDIDX_], L"'VEL_Z'");
		_sprintf(g_data2c.cOutPnp.X[nIndexCnt][_SPDIDX_], L"'VEL_X'");
		_sprintf(g_data2c.cAdc.X[nIndexCnt][_SPDIDX_], L"'VEL_X'");
		_sprintf(g_data2c.cAdc.Z[nIndexCnt][_SPDIDX_], L"'VEL_Z'");
	}
}

void copyAccelName(void)
{
	for (int nIndexCnt = 0; nIndexCnt < 20; nIndexCnt++)
	{
		_sprintf(g_data2c.cMzLift.Z[nIndexCnt][_ACCIDX_], L"'ACC_Z'");

		_sprintf(g_data2c.cLdmz.Y[nIndexCnt][_ACCIDX_], L"'ACC_Y'");
		_sprintf(g_data2c.cLdmz.Z[nIndexCnt][_ACCIDX_], L"'ACC_Z'");
		_sprintf(g_data2c.cLdmz.PusherX[nIndexCnt][_ACCIDX_], L"'ACC_X'");

		_sprintf(g_data2c.cRail.X[nIndexCnt][_ACCIDX_], L"'ACC_X'");

		_sprintf(g_data2c.cInPnp.Y[nIndexCnt][_ACCIDX_], L"'ACC_Y'");
		_sprintf(g_data2c.cInPnp.Z[nIndexCnt][_ACCIDX_], L"'ACC_Z'");
		_sprintf(g_data2c.cInPnp.ClampY[nIndexCnt][_ACCIDX_], L"'ACC_Y'");

		for (int nStgCnt = 0; nStgCnt < 4; nStgCnt++)
		{
			_sprintf(g_data2c.cIndex[nStgCnt].X[nIndexCnt][_ACCIDX_], L"'ACC_X'");
			_sprintf(g_data2c.cIndex[nStgCnt].T[nIndexCnt][_ACCIDX_], L"'ACC_T'");
		}

		for (int nRouterCnt = 0; nRouterCnt < 2; nRouterCnt++)
		{
			_sprintf(g_data2c.cRouter[nRouterCnt].Y[nIndexCnt][_ACCIDX_], L"'ACC_Y'");
			_sprintf(g_data2c.cRouter[nRouterCnt].Z1[nIndexCnt][_ACCIDX_], L"'ACC_Z'");
			_sprintf(g_data2c.cRouter[nRouterCnt].Z2[nIndexCnt][_ACCIDX_], L"'ACC_Z'");
			_sprintf(g_data2c.cRouter[nRouterCnt].W[nIndexCnt][_ACCIDX_], L"'ACC_W'");
		}

		_sprintf(g_data2c.cOutPnp.Y[nIndexCnt][_ACCIDX_], L"'ACC_Y'");
		_sprintf(g_data2c.cOutPnp.Z[nIndexCnt][_ACCIDX_], L"'ACC_Z'");
		_sprintf(g_data2c.cOutPnp.X[nIndexCnt][_ACCIDX_], L"'ACC_X'");
		_sprintf(g_data2c.cAdc.X[nIndexCnt][_ACCIDX_], L"'ACC_X'");
		_sprintf(g_data2c.cAdc.Z[nIndexCnt][_ACCIDX_], L"'ACC_Z'");
	}
}

void copyPosData(void)
{
	for (int nIndex = 0; nIndex < _MAX_INDEX_; nIndex++)
	{
		_sprintf(g_data2c.cMzLift.Z[nIndex][_POS_]	, L"%03f", g_mt[MT_MGZ_LOAD_Z].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cLdmz.Y[nIndex][_POS_]		, L"%03f", g_mt[MT_LD_Y].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cLdmz.Z[nIndex][_POS_]		, L"%03f", g_mt[MT_LD_Z].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cLdmz.PusherX[nIndex][_POS_]	, L"%03f", g_mt[MT_PUSHER_X].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRail.X[nIndex][_POS_]	, L"%03f", g_mt[MT_RAIL_GRIPPER_X].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cInPnp.Y[nIndex][_POS_]		, L"%03f", g_mt[MT_INPNP_Y].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cInPnp.Z[nIndex][_POS_]		, L"%03f", g_mt[MT_INPNP_Z].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cInPnp.ClampY[nIndex][_POS_]	, L"%03f", g_mt[MT_INPNP_CLAMP_Y].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cIndex[0].X[nIndex][_POS_], L"%03f", g_mt[MT_INDEX_X_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[0].T[nIndex][_POS_], L"%03f", g_mt[MT_INDEX_T_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[1].X[nIndex][_POS_], L"%03f", g_mt[MT_INDEX_X_02].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[1].T[nIndex][_POS_], L"%03f", g_mt[MT_INDEX_T_02].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[2].X[nIndex][_POS_], L"%03f", g_mt[MT_INDEX_X_03].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[2].T[nIndex][_POS_], L"%03f", g_mt[MT_INDEX_T_03].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[3].X[nIndex][_POS_], L"%03f", g_mt[MT_INDEX_X_04].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[3].T[nIndex][_POS_], L"%03f", g_mt[MT_INDEX_T_04].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRouter[0].Y[nIndex][_POS_]	, L"%03f", g_mt[MT_ROUTER_Y_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].Y[nIndex][_POS_]	, L"%03f", g_mt[MT_ROUTER_Y_02].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRouter[0].Z1[nIndex][_POS_]	, L"%03f", g_mt[MT_SPINDLE_Z_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[0].Z2[nIndex][_POS_]	, L"%03f", g_mt[MT_SPINDLE_Z_02].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].Z1[nIndex][_POS_]	, L"%03f", g_mt[MT_SPINDLE_Z_03].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].Z2[nIndex][_POS_]	, L"%03f", g_mt[MT_SPINDLE_Z_04].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRouter[0].W[nIndex][_POS_]	, L"%03f", g_mt[MT_ROUTER_W_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].W[nIndex][_POS_]	, L"%03f", g_mt[MT_ROUTER_W_02].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cOutPnp.Y[nIndex][_POS_]		, L"%03f", g_mt[MT_OUTPNP_Y].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cOutPnp.Z[nIndex][_POS_]		, L"%03f", g_mt[MT_OUTPNP_Z].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cOutPnp.X[nIndex][_POS_]		, L"%03f", g_mt[MT_OUTPNP_X].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cAdc.X[nIndex][_POS_]			, L"%03f", g_mt[MT_ADC_X].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cAdc.Z[nIndex][_POS_]			, L"%03f", g_mt[MT_ADC_Z].m_pTable->pos[nIndex]);
	}
}

void copySpeedData(void)
{
	for (int nIndex = 0; nIndex < _MAX_INDEX_; nIndex++)
	{
		_sprintf(g_data2c.cMzLift.Z[nIndex][_SPD_]	, L"%03f", g_mt[MT_MGZ_LOAD_Z].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cLdmz.Y[nIndex][_SPD_]		, L"%03f", g_mt[MT_LD_Y].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cLdmz.Z[nIndex][_SPD_]		, L"%03f", g_mt[MT_LD_Z].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cLdmz.PusherX[nIndex][_SPD_]	, L"%03f", g_mt[MT_PUSHER_X].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRail.X[nIndex][_SPD_]	, L"%03f", g_mt[MT_RAIL_GRIPPER_X].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cInPnp.Y[nIndex][_SPD_]		, L"%03f", g_mt[MT_INPNP_Y].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cInPnp.Z[nIndex][_SPD_]		, L"%03f", g_mt[MT_INPNP_Z].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cInPnp.ClampY[nIndex][_SPD_]	, L"%03f", g_mt[MT_INPNP_CLAMP_Y].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cIndex[0].X[nIndex][_SPD_], L"%03f", g_mt[MT_INDEX_X_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[0].T[nIndex][_SPD_], L"%03f", g_mt[MT_INDEX_T_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[1].X[nIndex][_SPD_], L"%03f", g_mt[MT_INDEX_X_02].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[1].T[nIndex][_SPD_], L"%03f", g_mt[MT_INDEX_T_02].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[2].X[nIndex][_SPD_], L"%03f", g_mt[MT_INDEX_X_03].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[2].T[nIndex][_SPD_], L"%03f", g_mt[MT_INDEX_T_03].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[3].X[nIndex][_SPD_], L"%03f", g_mt[MT_INDEX_X_04].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[3].T[nIndex][_SPD_], L"%03f", g_mt[MT_INDEX_T_04].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRouter[0].Y[nIndex][_SPD_]	, L"%03f", g_mt[MT_ROUTER_Y_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].Y[nIndex][_SPD_]	, L"%03f", g_mt[MT_ROUTER_Y_02].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRouter[0].Z1[nIndex][_SPD_]	, L"%03f", g_mt[MT_SPINDLE_Z_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[0].Z2[nIndex][_SPD_]	, L"%03f", g_mt[MT_SPINDLE_Z_02].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].Z1[nIndex][_SPD_]	, L"%03f", g_mt[MT_SPINDLE_Z_03].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].Z2[nIndex][_SPD_]	, L"%03f", g_mt[MT_SPINDLE_Z_04].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRouter[0].W[nIndex][_SPD_]	, L"%03f", g_mt[MT_ROUTER_W_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].W[nIndex][_SPD_]	, L"%03f", g_mt[MT_ROUTER_W_02].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cOutPnp.Y[nIndex][_SPD_]		, L"%03f", g_mt[MT_OUTPNP_Y].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cOutPnp.Z[nIndex][_SPD_]		, L"%03f", g_mt[MT_OUTPNP_Z].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cOutPnp.X[nIndex][_SPD_]		, L"%03f", g_mt[MT_OUTPNP_X].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cAdc.X[nIndex][_SPD_]			, L"%03f", g_mt[MT_ADC_X].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cAdc.Z[nIndex][_SPD_]			, L"%03f", g_mt[MT_ADC_Z].m_pTable->pos[nIndex]);
	}
}

void copyAccelData(void)
{
	for (int nIndex = 0; nIndex < _MAX_INDEX_; nIndex++)
	{
		_sprintf(g_data2c.cMzLift.Z[nIndex][_ACC_]	, L"%03f", g_mt[MT_MGZ_LOAD_Z].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cLdmz.Y[nIndex][_ACC_]		, L"%03f", g_mt[MT_LD_Y].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cLdmz.Z[nIndex][_ACC_]		, L"%03f", g_mt[MT_LD_Z].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cLdmz.PusherX[nIndex][_ACC_]	, L"%03f", g_mt[MT_PUSHER_X].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRail.X[nIndex][_ACC_]	, L"%03f", g_mt[MT_RAIL_GRIPPER_X].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cInPnp.Y[nIndex][_ACC_]		, L"%03f", g_mt[MT_INPNP_Y].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cInPnp.Z[nIndex][_ACC_]		, L"%03f", g_mt[MT_INPNP_Z].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cInPnp.ClampY[nIndex][_ACC_]	, L"%03f", g_mt[MT_INPNP_CLAMP_Y].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cIndex[0].X[nIndex][_ACC_], L"%03f", g_mt[MT_INDEX_X_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[0].T[nIndex][_ACC_], L"%03f", g_mt[MT_INDEX_T_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[1].X[nIndex][_ACC_], L"%03f", g_mt[MT_INDEX_X_02].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[1].T[nIndex][_ACC_], L"%03f", g_mt[MT_INDEX_T_02].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[2].X[nIndex][_ACC_], L"%03f", g_mt[MT_INDEX_X_03].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[2].T[nIndex][_ACC_], L"%03f", g_mt[MT_INDEX_T_03].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[3].X[nIndex][_ACC_], L"%03f", g_mt[MT_INDEX_X_04].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cIndex[3].T[nIndex][_ACC_], L"%03f", g_mt[MT_INDEX_T_04].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRouter[0].Y[nIndex][_ACC_]	, L"%03f", g_mt[MT_ROUTER_Y_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].Y[nIndex][_ACC_]	, L"%03f", g_mt[MT_ROUTER_Y_02].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRouter[0].Z1[nIndex][_ACC_]	, L"%03f", g_mt[MT_SPINDLE_Z_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[0].Z2[nIndex][_ACC_]	, L"%03f", g_mt[MT_SPINDLE_Z_02].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].Z1[nIndex][_ACC_]	, L"%03f", g_mt[MT_SPINDLE_Z_03].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].Z2[nIndex][_ACC_]	, L"%03f", g_mt[MT_SPINDLE_Z_04].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cRouter[0].W[nIndex][_ACC_]	, L"%03f", g_mt[MT_ROUTER_W_01].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cRouter[1].W[nIndex][_ACC_]	, L"%03f", g_mt[MT_ROUTER_W_02].m_pTable->pos[nIndex]);

		_sprintf(g_data2c.cOutPnp.Y[nIndex][_ACC_]		, L"%03f", g_mt[MT_OUTPNP_Y].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cOutPnp.Z[nIndex][_ACC_]		, L"%03f", g_mt[MT_OUTPNP_Z].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cOutPnp.X[nIndex][_ACC_]		, L"%03f", g_mt[MT_OUTPNP_X].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cAdc.X[nIndex][_ACC_]			, L"%03f", g_mt[MT_ADC_X].m_pTable->pos[nIndex]);
		_sprintf(g_data2c.cAdc.Z[nIndex][_ACC_]			, L"%03f", g_mt[MT_ADC_Z].m_pTable->pos[nIndex]);
	}
}

void copy2Mtd(BOOL bInit)
{
	int nIndex, nMt;

	if(bInit)
	{
		copyPmName();
		copyPmIoData();
		copyMtName();
		copyDeviceId();
		copyEtc();

		copyPosName();
		copySpeedName();
		copyAccelName();

		for(nMt = 0; nMt < _MAX_AXIS_; nMt++)
		{
			for(nIndex = 0; nIndex < _MAX_INDEX_; nIndex++)
				g_logChk.bTransfer[nMt][nIndex] = FALSE;
		}

		for(nIndex = 0; nIndex < _MAX_PNEUM_; nIndex++)
			g_logChk.bFunction[nIndex] = FALSE;
	}
	
	copyPosData();
	copySpeedData();
	copyAccelData();
}