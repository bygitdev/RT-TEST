#include "CSanSu.h"

#define _USE_MATH_DEFINES 
#include <math.h>
#include <Windows.h>
#include <stdio.h>


namespace SANSU
{

	//---------------------------------------------------------------------
	// Rad<->Deg
	double Rad2Deg(double dRad)
	{
		double dDeg = dRad * (180.0 / M_PI);
		return (dDeg);
	}

	double Deg2Rad(double dDeg)
	{
		double dRad = dDeg * (M_PI / 180.0);
		return (dRad);
	}




	//---------------------------------------------------------------------
	// Get Theta & Slope
    //                           |(oppsite)
    //                           |
    //        -------------------|
    //          adjacent
	double GetTheta(double dAdjacent, double dOppsite)
	{
		double dAdj, dOpp;
		double dTheta;

		dAdj = fabs(dAdjacent);
		dOpp = fabs(dOppsite);


		dTheta = atanf((float)(dOpp / dAdj));
		dTheta = Rad2Deg(dTheta);

		if(0 > dOppsite)
			dTheta = dTheta * (-1.0);

		return (dTheta);
	}

	double  GetSlope(POINT2D pt1, POINT2D pt2)
	{
		double dSlope = (pt2.dY - pt1.dY) / (pt2.dX - pt1.dX);

		return (dSlope);
	}



	//---------------------------------------------------------------------
	// �� S(a, b) �� �߽����� �� P(x, y) �� R ��ŭ ȸ�� �Ͽ����� P'(x', y') �� ��ǥ
	// x' = (x-a) * cosR - (y-b)sinR
	// y' = (x-a) * sinR + (y-b)cosR
	POINT2D Rotate(POINT2D ptXY, double dTheta)
	{
		POINT2D rtXY;
		double dRad = Deg2Rad(dTheta);

		rtXY.dX = (ptXY.dX)*cos(dRad) - (ptXY.dY)*sin(dRad);
		rtXY.dY = (ptXY.dX)*sin(dRad) + (ptXY.dY)*cos(dRad);

		return (rtXY);
	}

	POINT2D Rotate(POINT2D ptZero, POINT2D ptXY, double dTheta)
	{
		POINT2D rtXY;
		double dRad = Deg2Rad(dTheta);

		rtXY.dX = (ptXY.dX - ptZero.dX)*cos(dRad) - (ptXY.dY - ptZero.dY)*sin(dRad) + ptZero.dX;
		rtXY.dY = (ptXY.dX - ptZero.dX)*sin(dRad) + (ptXY.dY - ptZero.dY)*cos(dRad) + ptZero.dY;

		return (rtXY);
	}


	//---------------------------------------------------------------------
	// dX <-> dY
	POINT2D Swap(POINT2D ptXY)
	{
		POINT2D rtXY;

		rtXY.dX = ptXY.dY;
		rtXY.dY = ptXY.dX;

		return (rtXY);
	}





	//---------------------------------------------------------------------
	// pt1, pt2 ���������� �Ÿ�
	double  GetDist(POINT2D pt1, POINT2D pt2)
	{
		double dTmp = pow(pt2.dX - pt1.dX, 2) + pow(pt2.dY - pt1.dY, 2);
		       dTmp = sqrt(dTmp);
		return (dTmp);
	}

	double  GetDist(double dX, double dY)
	{
		double dTmp = (dX * dX) + (dY *dY);
		dTmp = sqrt(dTmp);
		return (dTmp);
	}

	//---------------------------------------------------------------------
	// pt1, pt2 ������ �߽�
	POINT2D GetCen(POINT2D pt1, POINT2D pt2)
	{
		POINT2D rtXY;

		rtXY.dX = (pt1.dX + pt2.dX) / 2.0;
		rtXY.dY = (pt1.dY + pt2.dY) / 2.0;

		return (rtXY);
	}



	//---------------------------------------------------------------------
	// ptXY�� ptCen�� �߽����� ptSearch(+/-)������ ��ġ�ϴ��� ����
	BOOL InRect(POINT2D ptXY, POINT2D ptCen, POINT2D ptSearch)
	{
		POINT2D pLTop, pRBtm;

		pLTop.dX = ptCen.dX - ptSearch.dX;
		pLTop.dY = ptCen.dY + ptSearch.dY;

		pRBtm.dX = ptCen.dX + ptSearch.dX;
		pRBtm.dY = ptCen.dY - ptSearch.dY;

		BOOL bRet = (pLTop.dX < ptXY.dX) && (ptXY.dX < pRBtm.dX) && (pLTop.dY > ptXY.dY) && (pRBtm.dY < ptXY.dY);
		return (bRet);
	}






    //---------------------------------------------------------------------
    // Line1 : pt1, dSlope1
    // Line2 : pt2, dSlope2
    // Line1�� Line2�� �������� ��ȯ
    POINT2D GetIntersect(POINT2D pt1, double dSlope1, POINT2D pt2, double dSlope2)
    {
        POINT2D rtXY;

        rtXY.dX = (dSlope1 * pt1.dX) - (dSlope2 * pt2.dX) + (pt2.dY - pt1.dY);
        rtXY.dX = rtXY.dX / (dSlope1 - dSlope2);

        rtXY.dY = (dSlope1*(rtXY.dX - pt1.dX)) + pt1.dY;

        return (rtXY);
    }

    //---------------------------------------------------------------------
    // Line1 : ptA1, ptA2
    // Line2 : ptB1, ptB2
    // Line1�� Line2�� �������� ��ȯ
    // dS�� dT�� ���� 0�� 1 ���̸� ����� ���, �� ���� �������� ����.
    // �׸��� dS�� dT�� ���ϴ� ���Ŀ��� �и� 0�� ��� �� ���� ����, ����(X)
    // �и�� ���� ��� 0�� ��� �� ���� ���ϼ���.
    BOOL GetIntersect(POINT2D& ptA1, POINT2D& ptA2, POINT2D& ptB1, POINT2D& ptB2, POINT2D* pResult)
    {
        double t;
        double s;
        double under = (ptB2.dY - ptB1.dY)*(ptA2.dX - ptA1.dX) - (ptB2.dX - ptB1.dX)*(ptA2.dY - ptA1.dY);
        if(under == 0) return false;

        double _t = (ptB2.dX - ptB1.dX)*(ptA1.dY - ptB1.dY) - (ptB2.dY - ptB1.dY)*(ptA1.dX - ptB1.dX);
        double _s = (ptA2.dX - ptA1.dX)*(ptA1.dY - ptB1.dY) - (ptA2.dY - ptA1.dY)*(ptA1.dX - ptB1.dX);

        t = _t / under;
        s = _s / under;

        if(t<0.0 || t>1.0 || s<0.0 || s>1.0)
            return (FALSE);
        if(_t == 0 && _s == 0)
            return (FALSE);


        pResult->dX = ptA1.dX + t * (double)(ptA2.dX - ptA1.dX);
        pResult->dY = ptA1.dY + t * (double)(ptA2.dY - ptA1.dY);

        return (TRUE);
    }





    //---------------------------------------------------------------------
    // ������ ������ ���� �߽��� ã��..
    BOOL CircumCircle(POINT2D pt1, POINT2D pt2, POINT2D pt3, POINT2D* pPtCen, double* pRadius)
    {
        double bax = pt2.dX - pt1.dX;
        double bay = pt2.dY - pt1.dY;
        double cax = pt3.dX - pt1.dX;
        double cay = pt3.dY - pt1.dY;

        double E = (bax * (pt1.dX + pt2.dX)) + (bay * (pt1.dY + pt2.dY));
        double F = (cax * (pt1.dX + pt3.dX)) + (cay * (pt1.dY + pt3.dY));
        double G = 2.0 * ((bax * (pt3.dY - pt2.dY)) - (bay * (pt3.dX - pt2.dX)));

        if(fabs(G) < 0.00000001)
            return (FALSE);

        pPtCen->dX = ((cay*E) - (bay*F)) / G;
        pPtCen->dY = ((bax*F) - (cax*E)) / G;

        POINT2D ptTmp;
        ptTmp.dX = pPtCen->dX - pt1.dX;
        ptTmp.dY = pPtCen->dY - pt1.dY;

        *pRadius = sqrt((ptTmp.dX*ptTmp.dX) + (ptTmp.dY*ptTmp.dY));

        return (TRUE);
    }



    BOOL SolLinearEQ3x3(double A[9], double bb[3], double x[3])
    {
        double invA[9];
        double det = (A[0] * (A[4] * A[8] - A[5] * A[7]) - A[1] * (A[3] * A[8] - A[5] * A[6]) + A[2] * (A[3] * A[7] - A[4] * A[6]));

        if(fabs(det) < 0.000000001)
        {
            x[0] = x[1] = x[2] = 0;
            return (FALSE);
        }

        det = 1.0 / det;

        invA[0] = (A[4] * A[8] - A[5] * A[7]) * det;
        invA[1] = (A[2] * A[7] - A[1] * A[8]) * det;
        invA[2] = (A[1] * A[5] - A[2] * A[4]) * det;
        invA[3] = (A[5] * A[6] - A[3] * A[8]) * det;
        invA[4] = (A[0] * A[8] - A[2] * A[6]) * det;
        invA[5] = (A[2] * A[3] - A[0] * A[5]) * det;
        invA[6] = (A[3] * A[7] - A[4] * A[6]) * det;
        invA[7] = (A[1] * A[6] - A[0] * A[7]) * det;
        invA[8] = (A[0] * A[4] - A[1] * A[3]) * det;

        x[0] = invA[0] * bb[0] + invA[1] * bb[1] + invA[2] * bb[2];
        x[1] = invA[3] * bb[0] + invA[4] * bb[1] + invA[5] * bb[2];
        x[2] = invA[6] * bb[0] + invA[7] * bb[1] + invA[8] * bb[2];

        return (TRUE);
    }

    //---------------------------------------------------------------
    // N���� ���� �̿��� ��Ŭ����
    BOOL CircleFit(POINT2D* pt, int N, POINT2D* pPtCen, double* pRadius)
    {
        double sx, sy;
        sx = sy = 0;
        double sx2, sy2, sxy;
        sx2 = sy2 = sxy = 0;
        double sx3, sy3, sx2y, sxy2;
        sx3 = sy3 = sx2y = sxy2 = 0;

        for(int k = 0; k < N; k++)
        {
            double x = pt[k].dX;
            double y = pt[k].dY;
            double xx = x*x;
            double yy = y*y;

            sx += x;
            sy += y;

            sx2 += xx;
            sy2 += yy;
            sxy += (x*y);

            sx3 += (x*xx);
            sy3 += (y*yy);
            sx2y += (xx*y);
            sxy2 += (yy*x);
        }

        double A[9], b[3], sol[3];

        A[0] = sx2; A[1] = sxy; A[2] = sx;
        A[3] = sxy; A[4] = sy2; A[5] = sy;
        A[6] = sx;  A[7] = sy;  A[8] = N;

        b[0] = -sx3 - sxy2;
        b[1] = -sx2y - sy3;
        b[2] = -sx2 - sy2;

        BOOL bOK = SolLinearEQ3x3(A, b, sol);

        if(FALSE == bOK)
            return (FALSE);

        double det = (sol[0] * sol[0]) + (sol[1] * sol[1]) - (4.0*sol[2]);

        if(det <= 0.0)
            return (FALSE);

        pPtCen->dX = -sol[0] / 2.0;
        pPtCen->dY = -sol[1] / 2.0;

        *pRadius = sqrt(det) / 2.0;

        return (TRUE);
    }





	//-------------------------------------------------------------------
	BOOL Between(int nVal, int nMin, int nMax)
	{
		if(nMin > nVal)
			return (FALSE);
		if(nMax < nVal)
			return (FALSE);

		return (TRUE);
	}
	

	BOOL AND(BOOL b1, BOOL b2, BOOL b3, BOOL b4, BOOL b5)
	{
		BOOL bRet = (b1 && b2 && b3 && b4 && b5);
		return (bRet);
	}

	BOOL OR(BOOL b1, BOOL b2, BOOL b3, BOOL b4, BOOL b5)
	{
		BOOL bRet = (b1 || b2 || b3 || b4 || b5);
		return (bRet);
	}
}












