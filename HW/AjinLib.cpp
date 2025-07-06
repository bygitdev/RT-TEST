#include "AjinLib.h"

/////////////////////////////////////////////////////////////////////
AJIN::CAjinLib  g_ajinLib;
/////////////////////////////////////////////////////////////////////

namespace AJIN
{
    //---------------------------------------------------------------
	CAjinLib::CAjinLib()
	{
		m_lIrqNo = 0;
	}

    //---------------------------------------------------------------
	CAjinLib::~CAjinLib()
	{
	}


    //---------------------------------------------------------------
    BOOL CAjinLib::Open(bool bReset)
    {
        DWORD dwRet = AXT_RT_SUCCESS;

		if(g_bNoDevice)
			return (TRUE);

        if(AxlIsOpened())
            return (TRUE);

        if(bReset)
            dwRet = AxlOpen(m_lIrqNo);
        else
            dwRet = AxlOpenNoReset(m_lIrqNo);

        if(AXT_RT_SUCCESS != dwRet)
        {
            printf("\n Err[%d] : Open()", dwRet);
            return (FALSE);
        }
           
        return (TRUE);
    }


    //---------------------------------------------------------------
    BOOL CAjinLib::Close()
    {
		if(g_bNoDevice)
			return (TRUE);

        AxlClose();
        return (TRUE);
    }

    //---------------------------------------------------------------
	BOOL CAjinLib::LoadMotorPara()
	{
		if(g_bNoDevice)
			return (true);

		DWORD dwRet = AxmMotLoadParaAll("C:\\KOSES\\SEQ\\MotorPara.mot");
		if(AXT_RT_SUCCESS != dwRet)
		{
			printf("\n Err[%d] : LoadMotorPara()", dwRet);
			return (FALSE);
		}

		return (TRUE);
	}


    //---------------------------------------------------------------
    BOOL CAjinLib::SSCNetIII(long lBoardNo)
    {
		if(g_bNoDevice)
			return (TRUE);

        DWORD dwData[32];

        DWORD dwRet = AxlSetSendBoardCommand(lBoardNo, 0x74, dwData, 0);
        if(AXT_RT_SUCCESS != dwRet)
        {
            printf("\n Err[%d] : SSCNetIII()", dwRet);
            return (FALSE);
        }
        return (TRUE);
    }


    //---------------------------------------------------------------
    BOOL CAjinLib::IsAxisCntErr(long lMaxMtNo)
    {
		if(g_bNoDevice)
			return (TRUE);

		long AxisCount;
        
		AxmInfoGetAxisCount(&AxisCount);
        if(lMaxMtNo != AxisCount)
        {
            printf("\n AxisCount[%d] : IsAxisCntErr()", AxisCount);
            return (FALSE);
        }
        return (TRUE);
    }



}


