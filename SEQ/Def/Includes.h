#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#ifdef __GLOBAL__
	#define EXTERN 
#else
	#define EXTERN extern
#endif


#define _USE_         1
#define _SKIP_        0 

#define DOOR_BYPASS	(FALSE)
#define TOP_BLOW	(FALSE)

#define	EXIST_UNCERTAIN		(-1)
#define EXIST_ERR			(1)
#define EXIST_NORMAL		(0)

#define SSCNET_BOARD1_NO	(0) 
#define SSCNET_BOARD2_NO	(1) 

// negrete
#define NEGRETE				TRUE
#define NEGRETE_WRITE(logw) {if(NEGRETE){logw;}}

#include <deque>
#include <windows.h>
#include <time.h>

#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"

// negrete
#include "..\..\NEGRETE\logdll.h"
#include "..\..\NEGRETE\need.h"
#include "..\..\NEGRETE\_Data2Char.h"

#include "IODef.h"
#include "PkgData.h"
#include "ErrNameDef.h"
#include "AxisDef.h"
#include "PneumaticDef.h"
#include "UserDataDef.h"

#include "..\SEQ.h"

#include "..\Default\CommWithMMI.h"
#include "..\Default\ReadWrite.h"
#include "..\Default\OpButton.h"
#include "..\Default\CheckError.h"

#include "..\Default\AllHome.h"
#include "..\Default\LampBuzzer.h"
#include "..\Default\SimpleFunc.h"
#include "..\Default\TenkeyProc.h"

#include "..\Part\_LdMzInConv.h"
#include "..\Part\_LdMzOutConv.h"
#include "..\Part\_MgzLoadZ.h"
#include "..\Part\_LdMz.h"
#include "..\Part\_Rail.h"
#include "..\Part\_InPnp.h"
#include "..\Part\_Index.h"
#include "..\Part\_Router.h"
#include "..\Part\_OutPnp.h"
#include "..\Part\_Adc.h"
#include "..\Part\_LotInfo.h"

EXTERN volatile bool	g_bClose;

EXTERN MCOPR		g_opr;			//장비의 제어를 담당하는 변수
EXTERN CMtAXL		g_mt[MAX_MT_NO];
EXTERN CPneumatic	g_pm[MAX_PM_NO]; 
EXTERN CNvMem*		g_pNV;
EXTERN CDIn			g_dIn;
EXTERN CDOut		g_dOut;
EXTERN ROUTER_DATA*	g_pRouterData;
EXTERN PATH_OFFSET*	g_pOffsetIndex[4];
EXTERN GERBER_PATH* g_pGerberPath;
EXTERN CIndex*		g_pIndex[4];
EXTERN CAXLAI		g_aIn;
EXTERN CAxlAO       g_ao;

// negrete
EXTERN	_char		g_cSwVer[50];
EXTERN	_char		g_cRecipeId[50];
EXTERN	BOOL		g_bEquipStop;
EXTERN	BOOL		g_bUserStop;
EXTERN	CTpBase		g_TpBase;
EXTERN	CPiper		g_Piper;
EXTERN	DATA2C		g_data2c;
EXTERN	LOG_CHK		g_logChk;


#endif//_INCLUDES_H_