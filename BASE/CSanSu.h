#ifndef _CSANSU_H_
#define _CSANSU_H_

#include <windows.h>

#define _USE_MATH_DEFINES 

#define RadsToDegrees( radian ) ((radian) * (180.0f / M_PI))
#define DegreesToRads( degrees ) ((degrees) * (M_PI/ 180.0f))


namespace SANSU
{
	// XY 구조체..
	typedef struct _PTXY
	{
		double dX;
		double dY;

		_PTXY operator+(const _PTXY& p2)
		{
			_PTXY tmp;
			tmp.dX = this->dX + p2.dX;
			tmp.dY = this->dY + p2.dY;
			return (tmp);
		}

		_PTXY operator-(const _PTXY& p2)
		{
			_PTXY tmp;
			tmp.dX = this->dX - p2.dX;
			tmp.dY = this->dY - p2.dY;
			return (tmp);
		}
		_PTXY operator*(double dScala)
		{
			_PTXY tmp;
			tmp.dX = this->dX * dScala;
			tmp.dY = this->dY * dScala;
			return (tmp);
		}

		_PTXY operator/(double dScala)
		{
			_PTXY tmp;
			tmp.dX = this->dX / dScala;
			tmp.dY = this->dY / dScala;

			return (tmp);
		}
	}POINT2D;


	// XYT구조체..
	typedef struct _PTXYT
	{
		double dX;
		double dY;
		double dT;

		_PTXYT operator+(const _PTXYT& p2)
		{
			_PTXYT tmp;
			tmp.dX = this->dX + p2.dX;
			tmp.dY = this->dY + p2.dY;
			tmp.dT = this->dT + p2.dT;
			return (tmp);
		}

		_PTXYT operator-(const _PTXYT& p2)
		{
			_PTXYT tmp;
			tmp.dX = this->dX - p2.dX;
			tmp.dY = this->dY - p2.dY;
			tmp.dT = this->dT - p2.dT;
			return (tmp);
		}

	}XYT;




	// Function declaration
	double Rad2Deg(double dRad);
	double Deg2Rad(double dDeg);
	double GetTheta(double dAdjacent, double dOppsite);
	double GetSlope(POINT2D pt1, POINT2D pt2);
	double GetDist(POINT2D pt1, POINT2D pt2);
	double GetDist(double dX, double dY);
	POINT2D   GetCen(POINT2D pt1, POINT2D pt2);
	POINT2D   Rotate(POINT2D ptXY, double dTheta);
	POINT2D   Rotate(POINT2D ptZero, POINT2D ptXY, double dTheta);
	POINT2D   Swap(POINT2D ptXY);
	BOOL   InRect(POINT2D ptXY, POINT2D ptCen, POINT2D ptSearch);
	

    // 두선의 교차점 찾기..
    POINT2D   GetIntersect(POINT2D pt1, double dSlope1, POINT2D pt2, double dSlope2);
    BOOL   GetIntersect(POINT2D& ptA1, POINT2D& ptA2, POINT2D& ptB1, POINT2D& ptB2, POINT2D* pResult);


    // 원의 중심 찾기..
    BOOL CircumCircle(POINT2D pt1, POINT2D pt2, POINT2D pt3, POINT2D* pPtCen, double* pRadius);
    BOOL CircleFit(POINT2D* pt, int N, POINT2D* pPtCen, double* pRadius);


	// double ceil(double x);  올림
	// double floor(double x); 내림

	BOOL Between(int nVal, int nMin, int nMax);
	BOOL AND(BOOL b1 = TRUE, BOOL b2 = TRUE, BOOL b3 = TRUE, BOOL b4 = TRUE, BOOL b5 = TRUE);
	BOOL OR(BOOL b1 = FALSE, BOOL b2 = FALSE, BOOL b3 = FALSE, BOOL b4 = FALSE, BOOL b5 = FALSE);
}

using namespace SANSU;




#endif //_CSANSU_H_

