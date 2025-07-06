#include "NvMem.h"
#include <cassert>

namespace NVMEM
{
	//---------------------------------------------------------------
	CNvMem::CNvMem()
	{
		timeBeginPeriod(1);
	}

	CNvMem::~CNvMem()
	{
		
	}

	//---------------------------------------------------------------
	CNvMem* CNvMem::Singleton(void)
	{
		static CNvMem NvMemory;
		return (&NvMemory);
	}


	//---------------------------------------------------------------
	BOOL CNvMem::Open(void)
	{
		// PKG, DM
		int nNVSize = sizeof(NvData);
		m_pMMFData = new CMMF(L"nvFile", L"C:\\KOSES\\SEQ\\Nv.dat", nNVSize);
	
		if(!m_pMMFData->Open())
			return (FALSE);
		m_pData = (NvData*)m_pMMFData->GetAddr();

		// Motor Table
		int nMotTableSize = (((3 * 8) * 50) * 100); // pos,vel,acc table(50) * axis(100)
		m_pMMFMotTable = new CMMF(L"motTable", L"C:\\KOSES\\SEQ\\MotNv.dat", nMotTableSize);

		if(!m_pMMFMotTable->Open())
			return (FALSE);
		m_pMotTable = (NvMotTable*)m_pMMFMotTable->GetAddr();

		// Gerver
		int nGerberSize = sizeof(NvGerber);
		m_pMMFGerber = new CMMF(L"nvGerber", L"C:\\KOSES\\SEQ\\GerberNv.dat", nGerberSize);

		if(!m_pMMFGerber->Open())
			return (FALSE);
		m_pGerber = (NvGerber*)m_pMMFGerber->GetAddr();

		// LotInfo
		int nLotInfoSize = sizeof(NvLotInfo);
		m_pMMFLotInfo = new CMMF(L"nvLotInfo", L"C:\\KOSES\\SEQ\\LotInfoNv.dat", nLotInfoSize);

		if(!m_pMMFLotInfo->Open())
			return (FALSE);
		m_pLotInfo = (NvLotInfo*)m_pMMFLotInfo->GetAddr();

		return (TRUE);
	}


	//---------------------------------------------------------------
	double& CNvMem::Pkg(int nNo)
	{
		return (m_pData->pkg[nNo]);
	}
	double& CNvMem::DDm(int nNo)
	{
		return (m_pData->ddm[nNo]);
	}
	INT32& CNvMem::NDm(int nNo)
	{
		return (m_pData->ndm[nNo]);
	}
	INT32& CNvMem::UseSkip(int nNo)
	{
		return (m_pData->useSkip[nNo]);
	}

	//---------------------------------------------------------------
	double& CNvMem::gerberPara(int nNo)
	{
		return (m_pGerber->para[nNo]);
	}
	double& CNvMem::gerberData(int nNo)
	{
		return (m_pGerber->data[nNo]);
	}

};