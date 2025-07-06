#include "Tenkey.h"


namespace TENKEY
{
	//---------------------------------------------------------------
	CTenkey::CTenkey()
	{
	}

	CTenkey::~CTenkey()
	{
	}



	//---------------------------------------------------------------
	void CTenkey::SetIO(PUSHORT pInAddr, PUSHORT pOutAddr)
	{
		m_pKeyPad = (KEY_PAD*)pInAddr;
		m_pSegmentOutput = (SEGMENT*)pOutAddr;
	}



	//---------------------------------------------------------------
	int	 CTenkey::GetTenkeyNo(void)
	{
		int nTenkeyNo = NO_KEY;
		int nKeyPadNo = GetKeyPadNo();
		m_pSegmentOutput->mDigit = ~m_segment.mDigit;
		m_pSegmentOutput->lDigit = ~m_segment.lDigit;
		Display();

		switch(m_fsmKey.Get())
		{
		case C_INIT_TENKEY:
			m_segment.hDigit = m_segment.mDigit =m_segment.lDigit = 0;
			m_fsmKey.Set(C_KEY_PRESSED);
			break;

		case C_KEY_PRESSED:
			if(NO_KEY == nKeyPadNo || m_fsmBlink.IsRun())
			{
				m_fsmKey.RstDelay();
				break;
			}

			if(!m_fsmKey.Delay(50))
				break;

			if(SET_KEY == nKeyPadNo )
			{
				m_bRst = TRUE;
				nTenkeyNo = GetSegNo();
				m_fsmKey.Set(C_KEY_RELEASE);
				m_fsmBlink.Set(C_BLINK_START);
			}
			else if(CLR_KEY == nKeyPadNo)
			{
				m_bRst = TRUE;
				nTenkeyNo = GetSegNo() + 1000;
				m_fsmKey.Set(C_KEY_RELEASE);
				m_fsmBlink.Set(C_BLINK_START);
			}
			else 
			{
				//if(1 == m_segment.mDigit)
				//	m_segment.hDigit = 0;
				//else
				//	m_segment.hDigit = 1;

				m_segment.mDigit = m_segment.lDigit;
				m_segment.lDigit = nKeyPadNo;

				if(m_bRst)
					m_segment.hDigit = m_segment.mDigit = 0; 

				m_bRst = FALSE;
				m_fsmKey.Set(C_KEY_RELEASE);
			}
			break;

		case C_KEY_RELEASE:
			if(NO_KEY != nKeyPadNo)
				m_fsmKey.RstDelay();
			else if(m_fsmKey.Delay(100))
				m_fsmKey.Set(C_KEY_PRESSED);
			break;

		case C_KEY_DISABLE:
			//m_segment.hDigit = 1;
			m_segment.mDigit = m_segment.lDigit = 0xf;
			m_fsmKey.Set(C_IDLE);
			break;
		}

		return (nTenkeyNo);
	}




	//---------------------------------------------------------------
	void CTenkey::Enable(void)
	{
		m_fsmKey.Set(C_INIT_TENKEY);
	}


	//---------------------------------------------------------------
	void CTenkey::Disable(void)
	{
		m_fsmKey.Set(C_KEY_DISABLE);
	}


	//---------------------------------------------------------------
	void CTenkey::Display(void)
	{
		switch(m_fsmBlink.Get())
		{
		case C_BLINK_START:
			m_tmpSegment = m_segment;
			m_fsmBlink.Set(C_BLINK_OFF);
			break;

		case C_BLINK_OFF:
			//m_segment.hDigit = 1;
			m_segment.mDigit = m_segment.lDigit = 0xf;
			if(FALSE == m_fsmBlink.Delay(300))
				break;
			m_fsmBlink.Set(C_BLINK_ON);
			break;

		case C_BLINK_ON:
			m_segment = m_tmpSegment;
			m_fsmBlink.Set(C_IDLE);
			break;
		}
	}


	//---------------------------------------------------------------
	int	 CTenkey::GetKeyPadNo(void)
	{
		int nRow = ConvHexToDec(m_pKeyPad->row);
		int nCol = ConvHexToDec(m_pKeyPad->col);

		if(NO_KEY == nRow || NO_KEY == nCol)
			return (NO_KEY);

		int nKeypadMask[_ROW_CNT_][_COL_CNT_]=
		{
			{1,       2,       3}, 
			{4,       5,       6},
			{7,       8,       9},
			{CLR_KEY, 0, SET_KEY}
		};

		int nRet = nKeypadMask[nRow][nCol];

		return (nRet);
	}


	//---------------------------------------------------------------
	int  CTenkey::ConvHexToDec(WORD wVal)
	{
		int nRet = NO_KEY;

		if(0x0001 == wVal)
			nRet = 0;
		else if(0x0002 == wVal)
			nRet = 1;
		else if(0x0004 == wVal)
			nRet = 2;
		else if(0x0008 == wVal)
			nRet = 3;

		return (nRet);
	}


	//---------------------------------------------------------------
	int CTenkey::GetSegNo(void)
	{
		int segmentVal = (m_segment.lDigit) + (m_segment.mDigit * 10);
		//if(!m_segment.hDigit)
		//	segmentVal = segmentVal + 100;
		return segmentVal;
	}
}


