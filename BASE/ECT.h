#ifndef _ECT_H_
#define _ECT_H_

#include <Windows.h>
#include <math.h>
#include <stdio.h>
#include "CSanSu.h"
#include "MMF.h"


#define MAX_ECT_TBL		50


typedef struct tag2DECT
{
	int		nMaxCntX;
	int		nMaxCntY;
	int		nInitPosX;
	int		nInitPosY;
	int		nPitchX;
	int		nPitchY;

	POINT2D ptVal[MAX_ECT_TBL][MAX_ECT_TBL];
}ECT2D, *PECT2D;

class CECT2D
{
private:
	CMMF*		m_pFile;
	ECT2D*		m_pECT;

	POINT2D	IdxNo(POINT2D ptCmd);
	POINT2D CmdPos(int nX, int nY);


public:
	CECT2D(LPCTSTR lpName, LPCTSTR lpFileName);
	virtual ~CECT2D();

	POINT2D GetData(POINT2D ptCmd);
	void SetInit(int nCntMaxX, int nCntMaxY, int nInitPosX, int nInitPosY, int nPitchX, int nPitchY);
	void SetIdx(int nX, int nY, POINT2D ptVal);
	POINT2D GetIdx(int nX, int nY);
	void Clear(void);
	void Flush(void);
};


#endif//_ECT_H_
