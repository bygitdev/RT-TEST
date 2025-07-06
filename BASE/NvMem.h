#ifndef _NVMEM_H_
#define _NVMEM_H_

#include <windows.h>
#include <tchar.h>
#include "MMF.h"


#define pNvSingleTon NVMEM::CNvMem::Singleton() //�߰� �������� �ʰ� �̰����� �����..
#define SCRAP_MAX_CNT		(20) // 16->20
#define LOT_INFO_MAX_CNT	(20)
#define MGZ_INFO_MAX_CNT	(100)

namespace NVMEM
{
#pragma pack(push, 2)

	// NV memory
	typedef struct _NvData
	{
		double		pkg[1000];
		double		ddm[1000];
		INT32		ndm[1000];
		INT32		useSkip[500];
	}NvData;

	// Shared memory
	typedef struct _NvMotTable
	{
		double pos[50];
		double vel[50];
		double acc[50];
	}NvMotTable;

	// Gerber memory
	typedef struct _NvGerber
	{
		double para[1000];
		double data[10000];
	}NvGerber;

	// Lot Info Memory
	typedef struct _LotInfo
	{
		//rfid
		char carrierID[64];	// ĳ���� ���� ID, RFID���鿡 ��ŷ�� ex) 15050032
		char eqpID[64];		// �����ȣ ex) AROUT-X04 
		int  mgzQty;		
		char lotID[64];		// LotID ex) X0SP050716
		char step[16];		// �ش�step ���� ex) M033
		char partID[64];	// ex) MZ7LN256HMJP-000H1-G11

		char portID[64];	 // ��Ʈ ID,  ex) AROUT-X04_I1 
		char mergeLotID[64]; // ex) X007B00508
		int  lotQty;		 // ex) 600
		char subLotID[64];	 //
		int  mergeLotQty;    // 

		char pcbBarcode[64]; // ���ڵ� ���� ex) 2000000000L2MZ7LN250150800000001
		char pcbModel[64];	 // ex) 2.5_850EVO_PRO_6A
		int  pcbArrayBaseQty;	//8
		char pcbTestResult[SCRAP_MAX_CNT]; // ex) 11111111 
		char pcbArtScrap[64];

		int  pcbXOutCnt;	//0
		int  scrapCnt;	    //8
		char scrapCode[SCRAP_MAX_CNT][64];	//SCRAP_INFO001=(SCRAP_CODE=3350 ARRAYSN=2000000000L1MZJPU512143300005772_3 SERIAL= EQPMODEL=ROUTER_SORTER)
		char scrapArraySN[SCRAP_MAX_CNT][64];
		char scrapSerial[SCRAP_MAX_CNT][64];

		int  lotStatus;  //Return Msg (invalid ����)
		                 // 0 : ���� Lot
		                 // 1 : Invalid Lot (�̹� ����� Lot ������ ���� �ջ� ���� ����)

		// OHT ���ÿ� �߰� -----------------------------
		char tray1Mark[64]; //Sorter���� ���ؾ� �� Tray OCR Code
		int tray1OhtQty;    // Oht���� �̼��ؾ� �� Tray Max ����
		int tray1ProdQty;   // Router���� Merge �� Unit Max ����
		char rejectLotID[64]; // RejectLotID X0SP050716
		char rejectCarrierID[64];  // ĳ���� ���� ID, Reject

		char reserved[1024];
	}LotInfo;


	typedef struct _LotMgzInfo
	{
		char carrierID[64];	// ĳ���� ���� ID, RFID���鿡 ��ŷ�� ex) 15050032
		char lotID[64];	
		int  pcbProductCnt;	// Mgz �� ���� ����
	}LotMgzInfo;


	// Lot Matching Memory
	typedef struct _LotHistory
	{
		// ��������
		int  exist; // Lot ���� ����
		int  order;	// Lot�� ����� ����
		int  lotMergeCurCnt; // �ش� Merge Lot Id�� ���� ����
		char mergeLotID[64]; // ex) X007B00508 // NULL�� �ƴϸ� Data�� �ִ� ������ ������
		char partID[64];	 // ex) MZ7LN256HMJP-000H1-G11

		LotMgzInfo mgzInfo[MGZ_INFO_MAX_CNT]; // merge Lot �� ���� ���� MGZ ���� ����
	}LotHistory;


	typedef struct _LotSplitInfo
	{
		char lotID[64];	
	}LotSplitInfo;


	// Lot Info Memory
	typedef struct _NvLotInfo
	{
		LotInfo data[LOT_INFO_MAX_CNT];
		LotHistory history[LOT_INFO_MAX_CNT]; // LotEnd�� ���� Lot ����
		LotSplitInfo split; // 1��
	}NvLotInfo;


#pragma pack(pop)

	//////////////////////////////////////////////////////////////////////////

	class CNvMem
	{
	private:
		CMMF*			m_pMMFData;
		CMMF*			m_pMMFShared;
		CMMF*			m_pMMFMotTable;
		CMMF*			m_pMMFGerber;
		CMMF*			m_pMMFLotInfo;

	public:
		NvData*			m_pData;
		NvMotTable*		m_pMotTable;
		NvGerber*		m_pGerber;
		NvLotInfo*		m_pLotInfo;

		double& Pkg(int nNo);
		double& DDm(int nNo);
		INT32&  NDm(int nNo);
		INT32&  UseSkip(int nNo);

		// Gerber
		double& gerberPara(int nNo);
		double& gerberData(int nNo);


	public:
		explicit CNvMem();
		virtual ~CNvMem();

		BOOL  Open(void);
		static CNvMem* Singleton(void);
	};
};

using namespace NVMEM;


#endif