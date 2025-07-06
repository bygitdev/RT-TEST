#ifndef _NEED_H_
#define _NEED_H_

#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include <tchar.h>
#include <cassert>

#pragma comment (lib, "ws2_32.lib")

//-----------------------------------------------------------
#ifdef DLL_EXPORTS
	#define  DLL_TYPE __declspec(dllexport)
#else
	#define DLL_TYPE __declspec(dllimport)
#endif
//-----------------------------------------------------------

#pragma comment(lib, "winmm.lib")

#ifdef UNICODE
	#define _char		wchar_t
	#define _strcpy		wcscpy
	#define _strcat		wcscat
	#define _strcmp		wcscmp
	#define _strlen		wcslen
	#define _sprintf	swprintf	/// stdio
	#define _fopen		_wfopen
#else
	#define _char		char
	#define _strcpy		strcpy 
	#define _strcat		strcat 
	#define _strcmp		strcmp
	#define _strlen		strlen 
	#define _sprintf	sprintf	/// stdio
	#define _fopen		fopen 
#endif

#define _MAX_PATH_	10000

_char* __stdcall charChange(char* message);
wchar_t* __stdcall SendWC(_char* message, int nBuf = 0);

#endif	/// _NEED_H_