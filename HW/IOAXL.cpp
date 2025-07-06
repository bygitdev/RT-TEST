#include "IOAXL.h"
#include "AjinLib.h"



namespace AJIN
{
	WORD g_wIOBitMask[16]=
	{
		0x0001, 0x0002, 0x0004, 0x0008,
		0x0010, 0x0020, 0x0040, 0x0080,
		0x0100, 0x0200, 0x0400, 0x0800,
		0x1000, 0x2000, 0x4000, 0x8000,
	};


	BOOL IsModuleCntErr(int nModuleType, int nCnt)
	{
		if(g_bNoDevice)
			return (true);

		long  lBoardNo   = 0;
		long  lModulePos = 0;
		DWORD dwModuleID = 0;

		int nModuleCnt = 0;
		for(long cnt = 0; cnt < 128; cnt++)
		{
			if(AXT_RT_SUCCESS == AxdInfoGetModule(cnt, &lBoardNo, &lModulePos, &dwModuleID))
			{
				if(nModuleType == dwModuleID)
				{
					nModuleCnt++;
				}
			}
		}

		if(nCnt == nModuleCnt)
			return (TRUE);
		else
			return (FALSE);
	}

	BOOL IsModuleCntErr(int nIOTotalCnt)
	{
		if(g_bNoDevice)
			return (true);

		long lModuleCnt = 0;
		AxdInfoGetModuleCount(&lModuleCnt);

		if(nIOTotalCnt == lModuleCnt)
			return (TRUE);
		else
			return (FALSE);
	}

	WORD CDIn::m_ch[]  = {0,};
	WORD CDOut::m_ch[] = {0,};

    //---------------------------------------------------------------
    // Digital Input class
    BOOL CDIn::AOn(int nNo, BOOL isRealTime)
    {
        int nCh = (nNo / 100);
        int nBit = ((nNo % 100) % 16);
    
		if(isRealTime)
		{
			int nOffset = (nCh * 16) + nBit;
			DWORD dwVal = 0;
			DWORD dwErr = AxdiReadInport(nOffset, &dwVal);

			if(dwVal)
				CDIn::m_ch[nCh] |= g_wIOBitMask[nBit];
			else
				CDIn::m_ch[nCh] &= ~g_wIOBitMask[nBit];

			return (!!dwVal);
		}
		else
		{
			BOOL bOn = !!(m_ch[nCh] & g_wIOBitMask[nBit]);
			return (bOn);
		}
    }

    BOOL CDIn::BOn(int nNo, BOOL isRealTime)
    {
        return (!AOn(nNo, isRealTime));
    }

    void CDIn::Set(int nNo, BOOL bOn)
    {
        int nCh = (nNo / 100);
        int nBit = ((nNo % 100) % 16);

        if(bOn)
            m_ch[nCh] |= g_wIOBitMask[nBit];
        else
            m_ch[nCh] &= ~g_wIOBitMask[nBit];
    }

    BOOL CDIn::ReadAll()
    {
        if(g_bNoDevice)
            return (TRUE);

        int nMaxModule = m_nMaxCh / 2;
        DWORD* pModuleVal = (DWORD*)&m_ch[0];

		DWORD d;
        for(int nCnt = 0; nCnt < nMaxModule; nCnt++)
        {
            d = AxdiReadInportDword(m_nId[nCnt], 0, &pModuleVal[nCnt]);
        }

        return (TRUE);
    }

	BOOL CDIn::Read(int nModuleNo, DWORD* pData)
	{
		if(g_bNoDevice)
			return (TRUE);

		DWORD dwRet = AxdiReadInportDword(nModuleNo, 0, pData);
		return (!dwRet);
	}



    //---------------------------------------------------------------
    // Digital Output class
    void CDOut::Set(int nNo, BOOL bOn, BOOL isRealTime)
    {
        int nCh = (nNo / 100);
        int nBit = ((nNo % 100) % 16);

        if(bOn)
            m_ch[nCh] |= g_wIOBitMask[nBit];
        else
            m_ch[nCh] &= ~g_wIOBitMask[nBit];

		if(isRealTime)
		{
			int nOffset = (nCh * 16) + nBit;
			DWORD dwErr = AxdoWriteOutport(nOffset, bOn);
		}
    }

    void CDOut::On(int nNo, BOOL isRealTime)
	{
		Set(nNo, TRUE, isRealTime);
	}
    void CDOut::Off(int nNo, BOOL isRealTime)
	{
		Set(nNo, FALSE, isRealTime);
	}

    BOOL CDOut::IsOn(int nNo, BOOL isRealTime)
    {
		int nCh = (nNo / 100);
		int nBit = ((nNo % 100) % 16);
		BOOL bOn = !!(m_ch[nCh] & g_wIOBitMask[nBit]);
		return (bOn);

		if(isRealTime)
		{
			int nOffset = (nCh * 16) + nBit;
			DWORD dwVal = 0;
			DWORD dwErr = AxdoReadOutport(nOffset, &dwVal);

			if(dwVal)
				m_ch[nCh] |= g_wIOBitMask[nBit];
			else
				m_ch[nCh] &= ~g_wIOBitMask[nBit];

			return (!!dwVal);
		}
    }

    BOOL CDOut::ReadAll()
    {
        if(g_bNoDevice)
            return (TRUE);

        int nMaxModule = m_nMaxCh / 2;
        DWORD* pModuleVal = (DWORD*)&m_ch[0];

		int errCode = 0;
        for(int nCnt = 0; nCnt < nMaxModule; nCnt++)
        {
            errCode = AxdoReadOutportDword(m_nId[nCnt], 0, &pModuleVal[nCnt]);
        }

        return (TRUE);
    }


	BOOL CDOut::WriteAll()
	{
		if(g_bNoDevice)
			return (TRUE);

		int nMaxModule = m_nMaxCh / 2;
		DWORD* pModuleVal = (DWORD*)&m_ch[0];

		int errCode = 0;
		for(int nCnt = 0; nCnt < nMaxModule; nCnt++)
		{
			errCode = AxdoWriteOutportDword(m_nId[nCnt], 0, pModuleVal[nCnt]);
		}

		return (TRUE);
	}

	BOOL CDOut::Read(int nModuleNo, DWORD* pData)
	{
		if(g_bNoDevice)
			return (TRUE);

		DWORD dwRet = AxdoReadOutportDword(nModuleNo, 0, pData);
		return (!dwRet);
	}





    //---------------------------------------------------------------
    // 개별 io 접근용..
    BOOL CInPoint::AOn(void) 
    { 
		int nCh = (m_nNo / 100);
		int nBit = ((m_nNo % 100) % 16);
        
		if(g_bNoDevice)
        {
            bool bOn = !!(CDIn::m_ch[nCh] & g_wIOBitMask[nBit]);
            return (bOn);
        }
          
        int nOffset = (nCh * 16) + nBit;
        DWORD dwVal = 0;
        DWORD dwErr = AxdiReadInport(nOffset, &dwVal);

		if(dwVal)
			CDIn::m_ch[nCh] |= g_wIOBitMask[nBit];
		else
			CDIn::m_ch[nCh] &= ~g_wIOBitMask[nBit];

        return (!!dwVal);
    }
    BOOL CInPoint::BOn(void) { return (!AOn()); }
    void CInPoint::Set(BOOL bOn) 
    { 
        int nCh = (m_nNo / 100);
        int nBit = ((m_nNo % 100) % 16);

        if(bOn)
			CDIn::m_ch[nCh] |= g_wIOBitMask[nBit];
        else
			CDIn::m_ch[nCh] &= ~g_wIOBitMask[nBit];
    }


    void COutPoint::Set(BOOL bOn) 
    { 
        int nCh = (m_nNo / 100);
        int nBit = ((m_nNo % 100) % 16);

        if(bOn)
			CDOut::m_ch[nCh] |= g_wIOBitMask[nBit];
        else
			CDOut::m_ch[nCh] &= ~g_wIOBitMask[nBit];

        if(!g_bNoDevice)
        {
			int nOffset = (nCh * 16) + nBit;
            DWORD dwErr = AxdoWriteOutport(nOffset, bOn);
        }
    }
    void COutPoint::On(void) { Set(TRUE); }
    void COutPoint::Off(void) { Set(FALSE); }
    
    BOOL COutPoint::IsOn(void)
    {
		int nCh = (m_nNo / 100);
		int nBit = ((m_nNo % 100) % 16);

        if(g_bNoDevice)
        {   
            bool bOn = !!(CDOut::m_ch[nCh] & g_wIOBitMask[nBit]);
            return (bOn);
        }
        else
        {
			int nOffset = (nCh * 16) + nBit;
            DWORD dwVal = 0;
            DWORD dwErr = AxdoReadOutport(nOffset, &dwVal);
            return (!!dwVal);
        }
    }


	//////////////////////////////////////////////////////////////////////////
	// Analog Input

	//---------------------------------------------------------------
	BOOL CAXLAI::Init(int nChCnt)
	{
		long lCount = 0;
		DWORD dwRet = AxaiInfoGetChannelCount(&lCount);

		if(0 != dwRet)
			return (FALSE);
	
		m_nChCnt = lCount;

		if(m_nChCnt == nChCnt)
			return (TRUE);
		else
			return (FALSE);
	}


	//---------------------------------------------------------------
	void CAXLAI::SetRange(int nCh, double dMinVolt, double dMaxVolt)
	{
		DWORD dwRet = AxaiSetRange(nCh, dMinVolt, dMaxVolt);
	}


	//---------------------------------------------------------------
	void CAXLAI::SetTriggerMode(int lModuleNo, int uTriggerMode)
	{
		DWORD dwRet = AxaiSetTriggerMode(lModuleNo, uTriggerMode);
	}

		
	//---------------------------------------------------------------
	void CAXLAI::Read(void)
	{
		for(int nCnt = 0; nCnt < m_nChCnt; nCnt++)
		{
			DWORD dwRet = AxaiSwReadVoltage(nCnt, &m_dVolt[nCnt]);
		}
	}


	//---------------------------------------------------------------
	// Analog Output
	CAxlAO::CAxlAO()
	{
		m_nChCnt = 0;
		for(int n = 0; n < 128; n++)
		{
			m_dVolt[n] = 0;
		}
	}

	BOOL CAxlAO::Init(int nChCnt)
	{
		long lCount = 0;
		DWORD dwRet = AxaoInfoGetChannelCount(&lCount);

		if(0 != dwRet)
			return (FALSE);
	
		m_nChCnt = lCount;

		if(m_nChCnt == nChCnt)
			return (TRUE);
		else
			return (FALSE);
	}



	void CAxlAO::SetRange(int nCh, double dMvinVolt, double dMaxVolt)
	{
		DWORD dwRet = AxaoSetRange(nCh, dMvinVolt, dMaxVolt);
	}



	void CAxlAO::Write(void)
	{
		for(int nCnt = 0; nCnt < m_nChCnt; nCnt++)
		{
			DWORD dwRet = AxaoWriteVoltage(nCnt, m_dVolt[nCnt]);
		}
	}



	void CAxlAO::Read(void)
	{
		for(int nCnt = 0; nCnt < m_nChCnt; nCnt++)
		{
			m_dVolt[nCnt] = 0;
			DWORD dwRet = AxaoReadVoltage(nCnt, &m_dVolt[nCnt]);
		}
	}
}


