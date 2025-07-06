#ifndef _IOAXL_H_
#define _IOAXL_H_

#include <windows.h>
#include <stdio.h>

#include ".\AXL\AXD.h"
#include ".\AXL\AXL.h"
#include ".\AXL\AXA.h"



namespace AJIN
{
	BOOL IsModuleCntErr(int nModuleType, int nCnt);
	BOOL IsModuleCntErr(int nIOTotalCnt);


    //---------------------------------------------------------------
    // Digital Input class
    class CDIn
    {
    public:
        CDIn() {}
        virtual ~CDIn() {}

        int			m_nMaxCh;
        int			m_nId[50];
        static WORD	m_ch[50];
	

        BOOL AOn(int nNo, BOOL isRealTime = FALSE);
        BOOL BOn(int nNo, BOOL isRealTime = FALSE);
        void Set(int nNo, BOOL bOn);
        BOOL ReadAll();
		BOOL Read(int nModuleNo, DWORD* pData);
    };

	

    //---------------------------------------------------------------
    // Digital Output class
    class CDOut
    {
    public:
        CDOut() {}
        virtual ~CDOut() {}

		int			m_nMaxCh;
        int			m_nId[50];
		static WORD	m_ch[50];

        void Set(int nNo, BOOL bOn, BOOL isRealTime = FALSE);
        void On(int nNo, BOOL isRealTime = FALSE);
        void Off(int nNo, BOOL isRealTime = FALSE);
        BOOL IsOn(int nNo, BOOL isRealTime = FALSE);
        BOOL ReadAll();
		BOOL WriteAll();
		BOOL Read(int nModuleNo, DWORD* pData);
    };



    //---------------------------------------------------------------
    // 개별 io 접근용..
    class CInPoint
    {
    public:
        int m_nNo;
        CInPoint() {}
        virtual ~CInPoint() {}

        BOOL AOn(void);
        BOOL BOn(void);
        void Set(BOOL bOn);
    };

    class COutPoint
    {
    public:
        int m_nNo;
        COutPoint() {}
        virtual ~COutPoint() {}

        void Set(BOOL bOn);
        void On(void);
        void Off(void);
        BOOL IsOn(void);
    };


	//-------------------------------------------------------------------
	// Analog Input class
	class CAXLAI
	{
	public:
		double	m_dVolt[128];
		int		m_nChCnt;

		BOOL Init(int nChCnt);
		void SetRange(int nCh, double dMinVolt, double dMaxVolt);
		void SetTriggerMode(int nModuleNo, int nTriggerMode);

		void Read(void);

		CAXLAI(){}
		virtual ~CAXLAI(){}

	};


	//-------------------------------------------------------------------
	// Analog Output class
	class CAxlAO
	{
	public:
		double	m_dVolt[128];
		int		m_nChCnt;

		BOOL Init(int nChCnt);
		void SetRange(int nCh, double dMvinVolt, double dMaxVolt);

		void Write(void);
		void Read(void);

		CAxlAO();
		virtual ~CAxlAO(){}

	};









}



#endif //_IOAXL_H_