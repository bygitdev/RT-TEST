#ifndef _LOGDLL_H_
#define _LOGDLL_H_

 #ifdef __BORLANDC__
	#ifdef UNICODE
		#pragma comment(lib, "log_bc_uni.lib")
	#else
		#pragma comment(lib, "log_bc_mb.lib")
	#endif
 #else
	#ifdef UNICODE
		#pragma comment(lib, "log_vc_uni.lib")
	#else
		#pragma comment(lib, "log_vc_mb.lib")
	#endif
 #endif

#include "Piper_svr.h"
#include "TpBase.h"

#endif /// _LOGDLL_H_