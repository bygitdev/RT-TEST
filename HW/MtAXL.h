#ifndef _MTAXL_H_ 
#define _MTAXL_H_

#include <Windows.h>
#include <stdio.h>
#include "..\BASE\FSM.h"
#include ".\AXL\AXL.h"
#include ".\AXL\AXM.h"


//  SSCNET���� ���ڱ��� ���� 
// 	������ MRJ4B Type(SSCNETIII/H) ���� ����̺��� ��� ���ڱ��� ���� ���� �ʱ� ������
// 	PCI-R1604-SIIIH Master Board���� ���ڱ��� ���� �ϰ� �� �ֹ߼� �޸𸮿� ����
// 	���ڱ��� ū��(Dafault -> 2^22 :10000 �̰� Unit/Pulse�� 1:1 �϶� 570,000 pps) �̻��� ���� �Ǹ�
// 	���Ŀ��� �����÷ο찡 �߻��Ͽ� �ݴ�������� �̵� �� �� ������ �̶� ���ڱ����� ����/�и� ����Ͽ� �Է� 
// 	���� ��� 2^22�� ���ش��� ������ ���� ������ �ѹ��� �� �� �ʿ��� �޽� ���� 40000�̶� �Ѵٸ�
// 	����(Num.)���� 2^22�� 4194304 �� �Է� �ǰ� �и�(Deno.)���� 40000�� �־�� ������
// 	���ڿ� �и� ���� ���� ���� ����Ͽ� 16�� ����� ����(Num.)�� 262144, �и�(Deno.)�� 2500�� �����ø� �˴ϴ�.
// 	����ϴ� ���� ���� /2�� �� 1.15Mbps, /4�� �� 2.3Mbps, /8�� �� 4.6Mbps, /16�� �� 9.2Mbps ���� ����� ���� 

namespace AJIN
{
	#define ENCODER_22BIT	(4194304) // J4 Servo 
	#define ENCODER_21BIT	(2097152)
	#define ENCODER_20BIT	(1048576)
	#define ENCODER_19BIT	(524288)
	#define ENCODER_18BIT	(262144)  // Mitsubishi ����
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
		// �ܺο��� �Ҵ� �ʿ�..
	
		

		//PVATable*	m_pTableSl; //������..kcm4000s+ ����..

		PVATable*	m_pTable;
		MtState		m_state;

		PROFILE		m_profile;
		MtConfig	m_config;
		


		OVER_RIDE	m_overRide;
		GANTRY      m_gantry;


	public:
		// ���� ���� �Լ�
		void SetCnt(double dCnt);
		void SetElecGearRatio(long lNumerator, long lDenominator);
		void SetUnitPerPulse(double dUnit, long lPulse);
		void ReadServoState(void);
		BOOL Update(void);

		double GetRealCnt(void);

		//���� ����Լ�
		void ServoOn(void);
		void ServoOff(void);
		void AlarmClear(void);
		void AlarmClearProc(void);

		//���� rd/wr �Լ�
		BOOL IsRdy(int nIndex = -1);
		BOOL InPos(int nIndex);
		BOOL InPos(int nIndex, double dInposBand);
		BOOL InPos(int nIndex, double dPos, double dInposBand);
		BOOL BetweenIdx(int nIndex1, int nIndex2, double dInposBand = -1);
		BOOL BetweenPos(double dPos1, double dPos2, double dInposBand = -1);
		BOOL BetweenPos(int nIndex, double dRange);
		BOOL ComparePos(int nIndex);


		// ��������(����)
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


	// ��������(���� : 2)
	BOOL MultiMove(CMtAXL* axis1, CMtAXL* axis2, int nIndex, double dVel = 0);
	BOOL MultiRMove(CMtAXL* axis1, CMtAXL* axis2, double dPulse, double dVel = 0);
	void MultiStop(CMtAXL* axis1, CMtAXL* axis2, BOOL bEStop);


}




#endif // _MTAXL_H_