#include "Pneumatic.h"



namespace PNEUMATIC
{
	
	/********************************************************************
	Pneumatic Class
	********************************************************************/
	BOOL CPneumatic::Set(int nCylNo, int iOnOpUpFwd, int iOffClDnBwd, int oOnOpUpFwd, int oOffClDnBwd)
	{
		m_nNo = nCylNo;	/// negrete
        m_di1.m_nNo = iOnOpUpFwd;
        m_di2.m_nNo = iOffClDnBwd;
        m_do1.m_nNo = oOnOpUpFwd;
        m_do2.m_nNo = oOffClDnBwd;

		return (TRUE);
	}//--------------------------------------------------------------




	void CPneumatic::SetErr(int nLimitTime, int nErrCode, int nAct)
	{
		m_nTimeLimit   = nLimitTime;
		m_nErrorCode = nErrCode;
		m_nErrorAct  = nAct;
	}//--------------------------------------------------------------




	// delay ¥‹¿ß : ms
	int CPneumatic::GetPos(int nDelay)
	{
		BOOL bIn1, bIn2, bOut1, bOut2;
		bIn1 = bIn2 = bOut1 = bOut2 = FALSE;

		if((pmDUMMY_IO == m_do1.m_nNo) && (pmDUMMY_IO != m_do2.m_nNo))
		{
			bOut2 = m_do2.IsOn();
			bOut1 = !bOut2;
		}
		else if((pmDUMMY_IO != m_do1.m_nNo) && (pmDUMMY_IO == m_do2.m_nNo))
		{
			bOut1 = m_do1.IsOn();
			bOut2 = !bOut1;
		}
		else if((pmDUMMY_IO != m_do1.m_nNo) && (pmDUMMY_IO != m_do2.m_nNo))
		{
			bOut1 = m_do1.IsOn();
			bOut2 = m_do2.IsOn();
		}

		if((pmDUMMY_IO == m_di1.m_nNo) && (pmDUMMY_IO != m_di2.m_nNo))
		{
			bIn2 = m_di2.AOn(); //g_dIn.AOn(m_nIn2);
			bIn1 = !bIn2;
		}
		else if((pmDUMMY_IO != m_di1.m_nNo) && (pmDUMMY_IO == m_di2.m_nNo))
		{
			bIn1 = m_di1.AOn(); //g_dIn.AOn(m_nIn1);
			bIn2 = !bIn1;
		}
		else if((pmDUMMY_IO != m_di1.m_nNo) && (pmDUMMY_IO != m_di2.m_nNo))
		{
			bIn1 = m_di1.AOn();
			bIn2 = m_di2.AOn();
		}
		else
		{
			bIn1 = bOut1;
			bIn2 = bOut2;
		}

		BOOL bFwd  = (bIn1 && !bIn2 && bOut1 && !bOut2);
		     bFwd |= ((bOut1 == bOut2) && (bIn1 && !bIn2));

		BOOL bBwd  = (!bIn1 && bIn2 && !bOut1 && bOut2);
		     bBwd |= ((bOut1 == bOut2) && (!bIn1 && bIn2));

		if(bFwd)
		{	
			if(m_tmDelay.TmOver(nDelay)) 
				return (pmFWD);
		}
		else if(bBwd)
		{
			if(m_tmDelay.TmOver(nDelay)) 
				return (pmBWD);
		}
		else
		{
			m_tmDelay.Reset();
		}

		return (pmUNCERTAIN);
	}//--------------------------------------------------------------




	int CPneumatic::GetErr(void)
	{
		if(pmUNCERTAIN == GetPos())
		{
			if(m_tmErr.TmOver(m_nTimeLimit))  // Timer Over Check..
			{
				if(pmUNCERTAIN != m_nErrorAct)									
					Actuate(m_nErrorAct);

				return (m_nErrorCode);			
			}
		}
		else 
		{
			m_tmErr.Reset();
		}

		return (0);
	}//--------------------------------------------------------------




	int CPneumatic::GetNo(void)
	{
		return (m_nNo);
	}//--------------------------------------------------------------




	void CPneumatic::Actuate(int nCmdDirection)
	{
		if(pmFWD == nCmdDirection)			
			SetOut(TRUE, FALSE);
		else if(pmBWD == nCmdDirection)			
			SetOut(FALSE, TRUE);
		else if(pmFREE == nCmdDirection)
			SetOut(FALSE, FALSE);

		if(m_nCurDriection != nCmdDirection)
			m_tmDelay.Reset();

		m_nCurDriection = nCmdDirection;
	}//--------------------------------------------------------------




	void CPneumatic::SetOut(BOOL bOut1, BOOL bOut2)
	{
        if(pmDUMMY_IO != m_do1.m_nNo)
            m_do1.Set(bOut1);
        if(pmDUMMY_IO != m_do2.m_nNo)
            m_do2.Set(bOut2);
	}//---------------------------------------------------------------

	int CPneumatic::GetOnIO(int cmdAct, int nInOut)
	{
		int nRet = 0;
		if (pmOFF == cmdAct)
		{
			if (0 == nInOut)	/// in
				nRet = m_di2.m_nNo;
			else
				nRet = m_do2.m_nNo;
		}
		else
		{
			if (0 == nInOut)	/// in
				nRet = m_di1.m_nNo;
			else
				nRet = m_do1.m_nNo;
		}

		return nRet;
	}

	int CPneumatic::GetOffIO(int cmdAct, int nInOut)
	{
		int nRet = 0;
		if (pmOFF == cmdAct)
		{
			if (0 == nInOut)	/// in
				nRet = m_di1.m_nNo;
			else
				nRet = m_do1.m_nNo;
		}
		else
		{
			if (0 == nInOut)	/// in
				nRet = m_di2.m_nNo;
			else
				nRet = m_do2.m_nNo;
		}

		return nRet;
	}
}

