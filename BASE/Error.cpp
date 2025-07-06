#include "Error.h"

void CError::Sort(void)
{
	for(int i = 0; i < MAX_ERROR_ARRAY; i++)
	{
		Swap(i, MAX_ERROR_ARRAY);
	}
}//------------------------------------------------------------------


void CError::Swap(int nStart, int nEnd)
{
	int nCnt = 0;
	int nTmp = 0;

	for(nCnt = nStart + 1; nCnt < nEnd; nCnt++)
	{
		if(m_err[nStart] == m_err[nCnt])       // ErrCode 동일 할 경우 
		{
			m_err[nCnt] = 0;
		}

		if(0 < m_err[nCnt])
		{
			if((m_err[nStart] > m_err[nCnt]) || (0 == m_err[nStart]))
			{
				nTmp = m_err[nStart];
				m_err[nStart] = m_err[nCnt];
				m_err[nCnt] = nTmp;
			}
		}
	}
}//------------------------------------------------------------------


void CError::Save(int nErrCode)
{
	if(TRUE == Find(nErrCode) || nErrCode <= 0)
		return;

	if((m_err[MAX_ERROR_ARRAY -1] == 0) || (m_err[MAX_ERROR_ARRAY -1] > nErrCode))
	{
		m_err[MAX_ERROR_ARRAY -1] = nErrCode;
		Sort();
	}
}//------------------------------------------------------------------


void CError::Del(int nErrCode)
{
	for(int i = 0; i < MAX_ERROR_ARRAY; i++)
	{
		if(m_err[i] == nErrCode)
		{
			m_err[i] = 0;
			Sort();
			break;
		}
	}
}//------------------------------------------------------------------


BOOL CError::Find(int nErrCode)
{
	for(int i = 0; i < MAX_ERROR_ARRAY; i++)
	{
		if(m_err[i] == nErrCode)
			return (TRUE);
	}

	return (FALSE);
}//------------------------------------------------------------------


void CError::Clear(void)
{
	for(int i = 0; i < MAX_ERROR_ARRAY; i++)
	{
		m_err[i] = 0;
	}
}//------------------------------------------------------------------

PINT CError::GetAddr(void)
{
	return (&m_err[0]);
}//------------------------------------------------------------------

int CError::GetNo(void)
{
	return (m_err[0]);
}//------------------------------------------------------------------

int CError::GetCnt(void)
{
	int nCnt = 0;

	for(int i = 0; i < MAX_ERROR_ARRAY; i++)
	{
		if(0 != m_err[i])
			nCnt++;
	}

	return (nCnt);
}//------------------------------------------------------------------