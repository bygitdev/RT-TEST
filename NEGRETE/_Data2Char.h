#ifndef _DATA2CHAR_H_
#define _DATA2CHAR_H_

#include <Windows.h>

#define _charSize	20
#define _MAX_AXIS_	29
#define _MAX_INDEX_	20
#define _MAX_PNEUM_	100
#define _MAX_AC_MT_	3
#define _MAX_CHAR_SIZE_	64

#define pmON	1
#define pmOFF	0

enum _mtd2charDef
{
	_POS_		= 0,
	_SPD_		= 1,
	_ACC_		= 2,
	_POSIDX_	= 3,	/// position name
	_SPDIDX_	= 4,	/// speed name
	_ACCIDX_	= 5,
	_MAX_		= 6,
};

typedef struct _globalChk
{
	BOOL bTransfer[_MAX_AXIS_][_MAX_INDEX_];
	BOOL bFunction[_MAX_PNEUM_];
}LOG_CHK;

typedef struct _groupEtc
{
	_char start[_charSize];
	_char end[_charSize];
	_char type[_charSize];
	_char cyl[_charSize];
	_char acMotor[_charSize];
	_char motor[_charSize];
	_char tcp[_charSize];
	_char vision[_charSize];
	_char sensor[_charSize];
	_char spindle[_charSize];
	_char sol[_charSize];
	_char vac[_charSize];
	_char actName[_charSize];
	_char delayTime[_charSize];
	_char off[_charSize];
	_char on[_charSize];
}cGroupEtc;

typedef struct _groupMzInConv
{
	_char deviceId[_charSize];
}cGroupMzInConv;

typedef struct _groupMzOutConv
{
	_char deviceId[_charSize];
}cGroupMzOutConv;

typedef struct _groupMzLd
{
	_char Z[_MAX_INDEX_][_MAX_][_charSize];
	_char deviceId[_charSize];
}cGroupMzLift;

typedef struct _groupLdmz
{
	_char Y[_MAX_INDEX_][_MAX_][_charSize];
	_char Z[_MAX_INDEX_][_MAX_][_charSize];
	_char PusherX[_MAX_INDEX_][_MAX_][_charSize];
	_char deviceId[_charSize];
}cGroupLdmz;

typedef struct _groupInRailGripper
{
	_char X[_MAX_INDEX_][_MAX_][_charSize];
	_char deviceId[_charSize];
}cGroupRail;

typedef struct _groupInpnp
{
	_char Y[_MAX_INDEX_][_MAX_][_charSize];
	_char Z[_MAX_INDEX_][_MAX_][_charSize];
	_char ClampY[_MAX_INDEX_][_MAX_][_charSize];
	_char deviceId[_charSize];
}cGroupInpnp;

typedef struct _groupIndex
{
	_char X[_MAX_INDEX_][_MAX_][_charSize];
	_char T[_MAX_INDEX_][_MAX_][_charSize];
	_char deviceId[_charSize];
}cGroupIndex;

typedef struct _groupRouter
{
	_char Y[_MAX_INDEX_][_MAX_][_charSize];
	_char Z1[_MAX_INDEX_][_MAX_][_charSize];
	_char Z2[_MAX_INDEX_][_MAX_][_charSize];
	_char W[_MAX_INDEX_][_MAX_][_charSize];
	_char deviceId[_charSize];
}cGroupRouter;

typedef struct _groupOutpnp
{
	_char Y[_MAX_INDEX_][_MAX_][_charSize];
	_char Z[_MAX_INDEX_][_MAX_][_charSize];
	_char X[_MAX_INDEX_][_MAX_][_charSize];
	_char deviceId[_charSize];
}cGroupOutpnp;

typedef struct _groupAdc
{
	_char X[_MAX_INDEX_][_MAX_][_charSize];
	_char Z[_MAX_INDEX_][_MAX_][_charSize];
	_char deviceId[_charSize];
}cGroupAdc;

typedef struct _pmIO
{			// act // io
	_char In[3][2][_charSize];
	_char out[3][2][_charSize];
}cPmIO;

typedef struct _data2char
{
	_char			cPmName[_MAX_PNEUM_][_charSize];
	cPmIO			cPmIO[_MAX_PNEUM_];

	_char			cAcMtName[_MAX_AC_MT_][_charSize]; // In, Out, MzIn
	_char			cAcIO[_MAX_AC_MT_][_charSize];

	_char			cMtName[_MAX_AXIS_][_charSize];

	cGroupEtc			cEtc;
	cGroupMzInConv		cMzInConv;
	cGroupMzOutConv		cMzOutConv;
	cGroupMzLift		cMzLift;
	cGroupLdmz			cLdmz;
	cGroupRail			cRail;
	cGroupInpnp			cInPnp;
	cGroupIndex			cIndex[4];
	cGroupRouter		cRouter[2];
	cGroupOutpnp		cOutPnp;
	cGroupAdc			cAdc;
}DATA2C;

_char* w2c(char* msg);

void copyPmName(void);
void copyPmIoData(void);

void copyMtName(void);
void copyDeviceId(void);
void copyEtc(void);

void copyPosName(void);
void copySpeedName(void);
void copyAccelName(void);

void copyPosData(void);
void copySpeedData(void);
void copyAccelData(void);

void copy2Mtd(BOOL bInit = FALSE);

#endif	/// _DATA2CHAR_H_