#ifndef _COMMWITHMMI_H_
#define _COMMWITHMMI_H_

#include "..\..\BASE\BaseAll.h"



#pragma pack(push, 1)

//-------------------------------------------------------------------
// Cmd Define
enum COMCMD
{
	CMD_NOTHING			= 0,
	CMD_RD_IO			= 1,
	CMD_WR_IO			= 2,
	CMD_RD_PKG			= 3,
	CMD_WR_PKG			= 4,
	CMD_RD_NDM			= 5,
	CMD_WR_NDM			= 6,
	CMD_RD_DDM			= 7,
	CMD_WR_DDM			= 8,
	CMD_RD_USESKIP		= 9,
	CMD_WR_USESKIP		= 10,

	CMD_RD_MT_STATUS	= 11,
	CMD_RD_MT_TABLE		= 12,
    CMD_WR_MT_TABLE		= 13,
	CMD_WR_MT_CMD		= 14,
	CMD_RD_ERR_WR		= 15,

	CMD_WR_INPNP_PRSVI		= 16, // prs offset x, y
	CMD_WR_ROUTER_F_PRSVI	= 17, // prs offset x, y
	CMD_WR_ROUTER_R_PRSVI	= 18, // prs offset x, y
	CMD_WR_QC_VI			= 19, // ??
	CMD_WR_TOP_VI			= 20, // 중심대비 오차값(x, y) + 범위 오차값(Width)
	CMD_WR_BTM_VI			= 21, // 중심대비 오차값(x, y) + 범위 오차값(Width)

	CMD_RD_INDEX01_PCB	= 30, // PCB Cutting 유무 ??
	CMD_WR_INDEX01_PCB	= 31,
	CMD_RD_INDEX02_PCB	= 32,
	CMD_WR_INDEX02_PCB	= 33,
	CMD_RD_INDEX03_PCB	= 34,
	CMD_WR_INDEX03_PCB	= 35,
	CMD_RD_INDEX04_PCB	= 36,
	CMD_WR_INDEX04_PCB	= 37,

	CMD_RD_GERBER_PARA	= 38,  
	CMD_WR_GERBER_PARA	= 39,
	CMD_RD_GERBER_DATA	= 40,  
	CMD_WR_GERBER_DATA	= 41,

	CMD_RD_LOT_INFO		= 42,  
	CMD_WR_LOT_INFO		= 43,

	CMD_RD_LOT_HISTORY	= 44,  
	CMD_WR_LOT_HISTORY	= 45,

	CMD_RD_LOT_SPLIT	= 46,  
	CMD_WR_LOT_SPLIT	= 47,

	CMD_WR_REGULATOR	= 90, // 추가

};


//-------------------------------------------------------------------
// Motor Cmd Define
enum MOTORCMD
{
	MTCMD_SERVO_ON	= 0,
	MTCMD_SERVO_OFF	= 1,
	MTCMD_INDEX_MOVE= 2,
	MTCMD_JOG_MOVE	= 3,
	MTCMD_JOG_STOP	= 4,
	MTCMD_R_MOVE	= 5,
	MTCMD_A_MOVE	= 6, // 절대위치 이동 
	MTCMD_ALL_HOME	= 7,
	MTCMD_HOME		= 8,
	MTCMD_RESET		= 9,
	MTCMD_STOP		= 10,
};


//-------------------------------------------------------------------
// 구조체..
typedef struct _CommMtControl
{
	int		nMtNo;
	int		nCmd;
	int		nCmdIndexNo;
	int		nDir;
	double	dPulse;
}CommMtControl;


typedef struct _CommMtTable
{
	int		nMtNo;
	double	dVel[50];
	double	dPos[50];
	double	dAcc[50];
}CommMtTable;


typedef struct _CommMtStatus
{
	int		nMtNo;
	BOOL	bServoOn;
	BOOL	bAlarm;
	BOOL	bDriving;
	BOOL	bPaused;
	BOOL	bHome;
	BOOL    bHoming;
	BOOL	bCw;
	BOOL	bCCw;
	BOOL	bOrg;
	int		nCurIndex;
	double	dRealCnt;
}CommMtStatus;


typedef struct _CommReadIO
{
	WORD inCH[50];
	WORD outCH[50];
}CommReadIO;


typedef struct _CommWriteIO
{
	int  nIONo;
	BOOL bOn;
}CommWriteIO;


typedef struct _CommAutoCal
{
	int	nSize; // 1,3,5,7,9....17
	POINT2D ptVal[300];
}CommAutoCal;


typedef struct _CommInt
{
	int nStart;
	int nEnd;
	int nVal[1000];
}CommInt;


typedef struct _CommDouble
{
	int nStart;
	int nEnd;
	double dVal[1000];
}CommDouble;


typedef struct _CommGerberData
{
	int nStart;
	int nEnd;
	GERBER data[500]; // max 100 point
}CommGerberData;


typedef struct _CommErrWar
{
	int err[10];
	int wr[10];
}CommErrWr;


typedef struct _CommLotInfo
{
	int part;
	LotInfo	data;
}CommLotInfo;


typedef struct _CommLotHistory
{
	int part;
	LotHistory	history;
}CommLotHistory;


typedef struct _CommLotSplitIfo
{
	LotSplitInfo split;
}CommLotSplitInfo;


typedef union tagBufferData
{
	CHAR bufferSize[BUFFER_SIZE]; //#define  BUFFER_SIZE      (1024 * 320) 

	CommMtControl	motControl;
	CommMtTable		motTable;
	CommMtStatus	motStatus;
	CommWriteIO		wrIO;
	CommReadIO		rdIO;

	CommDouble		pkg;
	CommDouble		ddm;
	CommInt			ndm;
	CommInt			useSkip;

	CommErrWr		errWr;

	POINT2D			prsInPnpResult;  // Trigger당 Vision Result
	POINT2D			prsRouterResultF; // Trigger당 Vision Result
	POINT2D			prsRouterResultR; // Trigger당 Vision Result

	CommDouble		gerberPara;
	CommGerberData	gerberData; // gerber Org, Index1,2,3,4
	CommLotInfo		lotInfo;
	CommLotHistory	lotHistory;
	CommLotSplitInfo lotSplitInfo;


}BUFFER_DATA;


typedef struct tagCommBuffer
{
	BUFFER_DATA buffer;
	int         command;
}COMM_BUFFER;


#pragma pack(pop)

class CMMI
{
public:
	CMMI() {}
	virtual ~CMMI() {}

	void Run(void);
	BOOL Init(void);

	void MtControl(CommMtControl motCommand);
	void MtIndexOffsetMove(CommMtControl motCommand);





private:
	CKamelasComm m_kamelas;

};

//////////////////////////////////////////////////////////////////////////
extern CMMI g_mmi;
//////////////////////////////////////////////////////////////////////////

#endif //_COMMWITHMMI_H_