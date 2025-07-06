#ifndef _MTAXL_H_ 
#define _MTAXL_H_

#include <Windows.h>
#include <stdio.h>
#include "..\BASE\FSM.h"
#include ".\AXL\AXL.h"
#include ".\AXL\AXM.h"


//  SSCNET사용시 전자기어비 설정 
// 	미쯔비시 MRJ4B Type(SSCNETIII/H) 서보 드라이브의 경우 전자기어비가 존재 하지 않기 때문에
// 	PCI-R1604-SIIIH Master Board에서 전자기어비를 설정 하고 비 휘발성 메모리에 저장
// 	전자기어비에 큰값(Dafault -> 2^22 :10000 이고 Unit/Pulse가 1:1 일때 570,000 pps) 이상이 들어가게 되면
// 	계산식에서 오버플로우가 발생하여 반대방향으로 이동 할 수 있으며 이때 전자기어비의 분자/분모를 약분하여 입력 
// 	예를 들어 2^22의 분해능을 가지는 서보 모터의 한바퀴 돌 때 필요한 펄스 수를 40000이라 한다면
// 	분자(Num.)에는 2^22인 4194304 가 입력 되고 분모(Deno.)에는 40000을 넣어야 하지만
// 	분자와 분모를 서로 같은 수로 약분하여 16로 약분한 분자(Num.)에 262144, 분모(Deno.)에 2500을 넣으시면 됩니다.
// 	약분하는 수에 따라 /2일 때 1.15Mbps, /4일 때 2.3Mbps, /8일 때 4.6Mbps, /16일 때 9.2Mbps 까지 사용이 가능 

namespace AJIN
{
	#define ENCODER_22BIT	(4194304) // J4 Servo 
	#define ENCODER_21BIT	(2097152)
	#define ENCODER_20BIT	(1048576)
	#define ENCODER_19BIT	(524288)
	#define ENCODER_18BIT	(262144)  // Mitsubishi 소형
	#define ENCODER_17BIT	(131072)

	enum MTIndex
	{
		MIDX_HOME_FAST		= 47,
		MIDX_HOME_SLOW		= 48,
		MIDX_JOG			= 49,
		MIDX_MAX            = 50,
	};

	enum enCmdHome
	{
		C_START_HOME			= 100,
		C_END_HOME              = 199,
	};


	enum enCmdDrv
	{
		C_DRV_START			= 100,
		C_DRV_STOP			= 101,
		C_DRV_PAUSED		= 102,
		C_DRV_END			= 199,
	};


	enum CmdMtAlarmClearDef
	{
		C_CLEAR_ALARM_START		= 10,
		C_CLEAR_ALARM_END		= 11,
	};


	typedef struct _PVATable
	{
		double pos[50];
		double vel[50];
		double acc[50];
	}PVATable;

	typedef struct _MtState
	{
		BOOL	isErr;
		BOOL	isFwdDir;
		BOOL    isServoOn;
		BOOL    isAlarm;
		BOOL	isHome;
		BOOL	isHoming;
		BOOL	isPaused;
		BOOL    isDriving;
		BOOL    isCw;
		BOOL    isCCw;
		BOOL    isOrg;
		DWORD	ajinErrCode;

		double	cmdCnt;
		double	realCnt;
	}MtState;


	typedef struct _MtConfig
	{
		BOOL enCw;
		BOOL enCCw;
		BOOL enServoOn;
		BOOL enAlarm;

		int		axisNo;
		int		homeIdx;
		double  inposBand;
	}MtConfig;


	typedef struct _Profile
	{
		int		curIndex;
		int		cmdIndex;
		int		nextIndex;
		double	pos;
		double	vel;
		double	acc;
	}PROFILE;

	typedef struct _OverRide
	{
		double vel[5];
		double pos[5];
	}OVER_RIDE;

	typedef struct _Gantry
	{
		int slaveAxis;
		DWORD homeMode;
		double slaveOffset;
	}GANTRY;


	class CMtAXL 
	{
	public:
		CFSM m_fsmHome;
		CFSM m_fsmAlarm;
		CFSM m_fsmDrv;

		CFSM m_fsmNoDevice;
		CTimer m_tmAddCnt;
		int  m_nNoDeviceTimeLimit;

		int	m_nReadState;

	public:
		// 외부에서 할당 필요..
	
		

		//PVATable*	m_pTableSl; //보조용..kcm4000s+ 전용..

		PVATable*	m_pTable;
		MtState		m_state;

		PROFILE		m_profile;
		MtConfig	m_config;
		


		OVER_RIDE	m_overRide;
		GANTRY      m_gantry;


	public:
		// 상태 설정 함수
		void SetCnt(double dCnt);
		void SetElecGearRatio(long lNumerator, long lDenominator);
		void SetUnitPerPulse(double dUnit, long lPulse);
		void ReadServoState(void);
		BOOL Update(void);

		double GetRealCnt(void);

		//범용 출력함수
		void ServoOn(void);
		void ServoOff(void);
		void AlarmClear(void);
		void AlarmClearProc(void);

		//상태 rd/wr 함수
		BOOL IsRdy(int nIndex = -1);
		BOOL InPos(int nIndex);
		BOOL InPos(int nIndex, double dInposBand);
		BOOL InPos(int nIndex, double dPos, double dInposBand);
		BOOL BetweenIdx(int nIndex1, int nIndex2, double dInposBand = -1);
		BOOL BetweenPos(double dPos1, double dPos2, double dInposBand = -1);
		BOOL BetweenPos(int nIndex, double dRange);
		BOOL ComparePos(int nIndex);


		// 구동관련(단축)
		BOOL Move(int nIndex, double dVel = 0);
        BOOL Move(int nIndex, double dPos, double dVel, double dAcc);
		BOOL PMove(int nIndex, double dPos, double dVel = 0);
		BOOL Move2(int nIndex, double dPos, double dOverRidePos, double dOverRideVel, double dVel = -1);
		BOOL Move3(int nIndex, double dPos, int nOverRideCnt, int nOverRideMode);
		BOOL RMove(double dPulse, double dVel = 0);
	
	
		BOOL VelMove(int nIndex, int nDirection);
		void Stop(BOOL bEStop);

		// Home search
		void StartHomeSearch(void);
		void CancelHomeSearch(void);
		void HomeSearchProc(void);

		// Paused / Resume
		void Paused(void);
		void Resume(void);

		void OverRideVel(int vel);


		//ext function

		//ect
		BOOL SetEct(BOOL bEnable);

		//gantry
		int SetGantry();
		int ResetGantry();


	public:
		CMtAXL();
		virtual ~CMtAXL();
	};


	/********************************************************************
	- Trigger Class
	********************************************************************/
	class CMtQITrigger
	{
	private:
		long		m_lAxisNo;
		double		m_dbTriggerTime;
		DWORD		m_dwTriggerLevel;
		DWORD		m_dwTriggerSource;
		DWORD		m_dwInterruptUse;
		DWORD		m_dwLastError;

	public:
		DWORD SetTrigger(long axisNo, double trig_time, DWORD trig_level, DWORD source_select, DWORD interrupt_use);
		DWORD StartTrigAbsPeriod(DWORD out_method, double pos);
		DWORD StartTrigBlock(double start_pos, double end_pos, double period_pos);
		DWORD StartTrigOnlyAbs(long trig_no, double* pTrig_pos);
		DWORD ResetTrigger(void);
		DWORD StartTrigOneShot(void);

		DWORD VirtualMove(double dPos, double dVel = 500, double dAcc = 500);
		DWORD VirtualStop(void);
		DWORD SetCnt(double dCnt);

	public:
		CMtQITrigger(){};
		virtual ~CMtQITrigger(){};
	};


	// 구동관련(다축 : 2)
	BOOL MultiMove(CMtAXL* axis1, CMtAXL* axis2, int nIndex, double dVel = 0);
	BOOL MultiRMove(CMtAXL* axis1, CMtAXL* axis2, double dPulse, double dVel = 0);
	void MultiStop(CMtAXL* axis1, CMtAXL* axis2, BOOL bEStop);


}




#endif // _MTAXL_H_