#include "MMF.h"
#include <cassert>

//-------------------------------------------------------------------
// ex)	m_pStorage = new CDataStorage(_T("NVFILE"), _T("c:\\KOSES\\SEQ\\nv.dat"), NV_FILE_SIZE);
//
//		if(FALSE == m_pStorage->Open())
//		{
//			::MessageBox(NULL, _T("Data Storage"), _T("Error") , MB_OK);
//			return (FALSE);
//		}
//
//		m_pData = (NV_DATA*)m_pStorage->GetAddr();
//


CMMF::CMMF(LPCTSTR lpName, LPCTSTR lpFileName, int nSize, BOOL bCreateFile)
{
	_tcscpy(m_lpName, lpName);
	_tcscpy(m_lpFilename, lpFileName);
	m_nSize = nSize;

	m_pMapAddr = NULL;
	m_hFile    = NULL;
	m_hMap     = NULL;
	m_bCreateFile = bCreateFile;
}

CMMF::~CMMF()
{
	if(m_bCreateFile)
		UnmapViewOfFile(m_pMapAddr);			 // 이 시점에서 파일에 저장

	::CloseHandle(m_hMap);
	::CloseHandle(m_hFile);
	m_hFile = NULL;
	m_hMap  = NULL;
}


//-------------------------------------------------------------------
// 임의의 시점에 파일에 저장함.
void CMMF::Flush(void)
{
	if(m_bCreateFile)
		FlushViewOfFile(m_pMapAddr, m_nSize); 
}



BOOL CMMF::Open(void)
{
	if(m_bCreateFile) // Bin file 생성시
	{
		m_hFile = ::CreateFile(m_lpFilename, 
			                   GENERIC_READ | GENERIC_WRITE, 
			                   FILE_SHARE_READ | FILE_SHARE_WRITE,
			                   NULL, 
			                   OPEN_ALWAYS, 
			                   NULL,
			                   NULL);

		if(NULL == m_hFile || INVALID_HANDLE_VALUE == m_hFile)
			return (FALSE);
	}

	// 파일 맵핑
	m_hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_lpName);

	if(NULL == m_hMap)
	{
		if(m_bCreateFile)
		{
			m_hMap = ::CreateFileMapping(m_hFile, 
				                         NULL, 
				                         PAGE_READWRITE | SEC_COMMIT, 
				                         0, 
				                         m_nSize, 
				                         m_lpName);
		}
		else
		{
			m_hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, 
				                         NULL, 
				                         PAGE_READWRITE | SEC_COMMIT, 
				                         0, 
				                         m_nSize, 
				                         m_lpName);
		}
		

		if(NULL == m_hMap || INVALID_HANDLE_VALUE == m_hMap)
			return (FALSE);
	}
	
	
	// 메모리 맵핑 파일을 만듦. 첫 주소를 리턴
	m_pMapAddr = (PBYTE)::MapViewOfFile(m_hMap, 
		                                FILE_MAP_ALL_ACCESS, 
		                                0, 
		                                0, 
		                                m_nSize);

	if(NULL == m_pMapAddr)
		return (FALSE);

	return (TRUE);
}


PBYTE CMMF::GetAddr(void)
{
	return (m_pMapAddr);
}


