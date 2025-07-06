#ifndef _ADC_H_
#define _ADC_H_

#include <Windows.h>
#include "..\..\HW\AjinLib.h"
#include "..\..\BASE\BaseAll.h"

class CAdc
{
public:
	enum PosX
	{
		PX_READY			= 0,
		PX_ALIGN			= 1, // Align + Push
		PX_GRIP				= 2, 
		PX_PNP_PICKUP		= 3, // Pickup & Putdown
		PX_1D				= 4, 
	};


	enum PosZ
	{
		PZ_READY					= 0, // ���ϴ� ��ġ
		PZ_PNP						= 1, // InPnp Pickup ��� 
		PZ_TOP_STAGE_ALIGN_1ST		= 2, // ������ Slot�� Pitch ó�� Align Pos
		PZ_TOP_STAGE_GRIP_DW_1ST	= 3,
		PZ_TOP_STAGE_GRIP_UP_1ST	= 4,
		PZ_TOP_MASK_ALIGN_1ST		= 5,
		PZ_TOP_MASK_GRIP_DW_1ST		= 6,
		PZ_TOP_MASK_GRIP_UP_1ST		= 7,
		PZ_TOP_PICKER_ALIGN			= 8,
		PZ_TOP_PICKER_GRIP_DW		= 9,
		PZ_TOP_PICKER_GRIP_UP		= 10,
		PZ_BTM_STAGE_ALIGN_1ST		= 11, // ������ Slot�� Pitch ó��
		PZ_BTM_STAGE_GRIP_DW_1ST	= 12,
		PZ_BTM_STAGE_GRIP_UP_1ST	= 13,
		PZ_BTM_MASK_ALIGN_1ST		= 14,
		PZ_BTM_MASK_GRIP_DW_1ST		= 15,
		PZ_BTM_MASK_GRIP_UP_1ST		= 16,
		PZ_BTM_PICKER_ALIGN			= 17,
		PZ_BTM_PICKER_GRIP_DW		= 18,
		PZ_BTM_PICKER_GRIP_UP		= 19,
	};


	enum Cmd
	{
		C_ALIGN_START	= 100, // MZ Off->On �ÿ� Slot Exist ���� Ȯ��
		C_ALIGN_END		,

		C_GRIP_START	= 200, // Grip �ÿ� Align �� �Ŀ� Grip �ϰ� 1D ���� ����
		C_GRIP_01		,
		C_GRIP_02		,
		C_GRIP_03		,
		C_GRIP_04		,
		C_GRIP_05		,
		C_GRIP_06		,
		C_GRIP_END		,

		C_PUSH_START	= 300, // Kit�� Slot�� ����
		C_PUSH_01		,
		C_PUSH_02		,
		C_PUSH_03		,
		C_PUSH_04		,
		C_PUSH_05		,
		C_PUSH_06		,
		C_PUSH_07		,
		C_PUSH_END		,
	};


	enum State
	{
		S_IDLE			= 0,
		S_READY			,
		S_PUTDN_WAIT	, // InPnp���� Kit�� ��ٸ�
		S_PICKUP_WAIT	, // InPnp���� Kit�� ��ٸ�
		S_GRIP			,
		S_PUSH			,
		S_END			,
	};


public:
	CAdc();
	virtual ~CAdc() {}

public:
	CFSM		m_fsm;
	BOOL		m_bRun;

	CMtAXL*		m_pMtX;
	CMtAXL*		m_pMtZ;

	void AutoRun(void);
	void CycleRun(void);
	void Init(void);

	int& Exist(void);    // Wait Zone�� Exist
	int& KitJobType(void); 
	int& KitInfo(void);  
	int& AdcState(void); // WaitPickup or Push

	int& AdcMzTopJobType(void);  
	int& AdcMzBtmJobType(void);  

	BOOL IsReadyPickUp(void);
	BOOL IsReadyPutDn(void);

	int  GetAdcIndexReturnNo();
	int  GetAdcMzGripNo();

	double GetAdcMzSlotAlignPos(int nMzKitNo);
	double GetAdcMzSlotGripDnPos(int nMzKitNo);
	double GetAdcMzSlotGripUpPos(int nMzKitNo);

	BOOL IsAdcMzZPosMove(int nMzKitNo, int nPosType); //enAdcMzKitNo , enAdcZPosType


private:
	CTimer		m_tmExistErr;

	BOOL		m_bRdyPutDn;
	BOOL		m_bRdyPickUp;
	BOOL		m_bRemove;
	
	int  GetState(void);
	BOOL IsErr(void);
	int  GetExistErr(void);

	void CycleAlign(void); // Align + Slot Exist Check
	void CycleGrip(void);
	void CyclePush(void);

};


/////////////////////////////////////////////////////////////////////
extern CAdc g_adc;
/////////////////////////////////////////////////////////////////////

#endif //_INRAIL_H_
