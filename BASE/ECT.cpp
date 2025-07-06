#include "ECT.h"







/********************************************************************
- 2차원 보정

[양선형 보간(bilinear interpolation)]

A       E              B
<- a  -> <--- (1-a) --->
------------------------
b   |                      |
|	                   |
|       X              |
1-b |                      |  
|                      |
|                      |
|                      |
|                      |
------------------------
C        F               D

E = (1-a)A + aB = A + a(B-A)
F = (1-a)C + aD = C + a(D-A)
X = (1-b)E + bF = E + b(F-E)
********************************************************************/


CECT2D::CECT2D(LPCTSTR lpName, LPCTSTR lpFileName)
{
	int nSize = sizeof(ECT2D);
	m_pFile = new CMMF(lpName, lpFileName, nSize);

	m_pFile->Open();
	m_pECT = (ECT2D*)m_pFile->GetAddr();
}


CECT2D::~CECT2D()
{
	delete m_pFile;
}//------------------------------------------------------------------



POINT2D CECT2D::IdxNo(POINT2D ptCmd)
{
	int nCnt = 0;
	POINT2D ptIdx;
	ptIdx.dX = ptIdx.dY = 0;
	
	for(nCnt = (m_pECT->nMaxCntX - 2); 0 <= nCnt; nCnt--)
	{
		if((int)CmdPos(nCnt, 0).dX <= (int)ptCmd.dX)
		{
			ptIdx.dX = nCnt;
			break;
		}
	}

	for(nCnt = (m_pECT->nMaxCntY - 2); 0 <= nCnt; nCnt--)
	{
		if((int)CmdPos(0, nCnt).dY <= (int)ptCmd.dY)
		{
			ptIdx.dY = nCnt;
			break;
		}
	}

	return (ptIdx);
}//------------------------------------------------------------------



//반환값은 비젼으로 측정한 오차량(um)
POINT2D CECT2D::GetData(POINT2D ptCmd)
{
	POINT2D ptVal = ptCmd;
	POINT2D ptMin = CmdPos(0,0);
	POINT2D ptMax = CmdPos((m_pECT->nMaxCntX -1), (m_pECT->nMaxCntY - 1));

	if(ptMin.dX > ptVal.dX)
		ptVal.dX = ptMin.dX;
	else if(ptMax.dX < ptVal.dX)
		ptVal.dX = ptMax.dX;

	if(ptMin.dY > ptVal.dY)
		ptVal.dY = ptMin.dY;
	else if(ptMax.dY < ptVal.dY)
		ptVal.dY = ptMax.dY;

	int nXW = (int)IdxNo(ptVal).dX;
	int nXE = nXW + 1;
	int nYN	= (int)IdxNo(ptVal).dY;
	int nYS	= nYN + 1;

	double dWeightX = (ptVal.dX - CmdPos(nXW, nYN).dX) / (CmdPos(nXE, nYN).dX - CmdPos(nXW, nYN).dX);
	double dWeightY = (ptVal.dY - CmdPos(nXW, nYN).dY) / (CmdPos(nXW, nYS).dY - CmdPos(nXW, nYN).dY);

	POINT2D ptTop = m_pECT->ptVal[nXW][nYN] + ((m_pECT->ptVal[nXE][nYN] - m_pECT->ptVal[nXW][nYN]) * dWeightX);
	POINT2D ptBtm = m_pECT->ptVal[nXW][nYS] + ((m_pECT->ptVal[nXE][nYS] - m_pECT->ptVal[nXW][nYS]) * dWeightX);
	POINT2D ptErr = ptTop + ((ptBtm - ptTop) * dWeightY);

	return (ptErr);
}//------------------------------------------------------------------



POINT2D CECT2D::CmdPos(int nX, int nY)
{
	POINT2D ptRet;

	ptRet.dX = m_pECT->nInitPosX + (nX * m_pECT->nPitchX);
	ptRet.dY = m_pECT->nInitPosY + (nY * m_pECT->nPitchY);

	return (ptRet);
}//------------------------------------------------------------------



void CECT2D::SetInit(int nCntMaxX, int nCntMaxY, int nInitPosX, int nInitPosY, int nPitchX, int nPitchY)
{
	m_pECT->nMaxCntX   = nCntMaxX;
	m_pECT->nMaxCntY   = nCntMaxY;
	m_pECT->nInitPosX  = nInitPosX;
	m_pECT->nInitPosY  = nInitPosY;
	m_pECT->nPitchX    = nPitchX;
	m_pECT->nPitchY    = nPitchY;
}//------------------------------------------------------------------



void CECT2D::SetIdx(int nX, int nY, POINT2D ptVal)
{
	m_pECT->ptVal[nX][nY] = ptVal;
}//------------------------------------------------------------------



POINT2D CECT2D::GetIdx(int nX, int nY)
{
	return (m_pECT->ptVal[nX][nY]);
}//------------------------------------------------------------------



void CECT2D::Clear(void)
{
	int nSize = sizeof(m_pECT->ptVal);
	ZeroMemory(&m_pECT->ptVal[0][0], nSize);
}//------------------------------------------------------------------



void CECT2D::Flush(void)
{
	m_pFile->Flush();
}//------------------------------------------------------------------

