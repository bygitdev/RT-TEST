#ifndef _ERROR_H_
#define _ERROR_H_

#include <Windows.h>

#define MAX_ERROR_ARRAY  10

class CError
{
private:
	void Sort(void);
	void Swap(int nStart, int nEnd);

public:
	int  m_err[MAX_ERROR_ARRAY];
	void Save(int nErrCode);
	void Del(int nErrCode);
	BOOL Find(int nErrCode);
	void Clear(void);

	PINT GetAddr(void);
	int GetNo(void);
	int GetCnt(void);

public:
	CError(){}
	~CError(){}
};

#endif // _ERROR_H_