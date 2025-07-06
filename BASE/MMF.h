#ifndef _NVFILE_H_
#define _NVFILE_H_

#include <windows.h>
#include <tchar.h>

class CMMF
{
private:
	HANDLE	m_hFile;
	HANDLE	m_hMap;
	TCHAR	m_lpName[MAX_PATH];
	TCHAR	m_lpFilename[MAX_PATH];
	int		m_nSize;
	PBYTE	m_pMapAddr;
	BOOL	m_bCreateFile;

public:
	BOOL  Open(void);
	PBYTE GetAddr(void);
	void  Flush(void);

 

public:
	explicit CMMF(LPCTSTR lpName, LPCTSTR lpFileName, int nSize, BOOL bCreateFile = TRUE);
	virtual ~CMMF();
};


#endif