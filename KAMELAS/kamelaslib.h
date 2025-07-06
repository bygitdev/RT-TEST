#ifndef KAMELASLIBH
#define KAMELASLIBH

/**********************************************************************

Project
	K.A.M.E.L.A.S
	

Introduction
	issue tracker : http://cpp.koses.co.kr/projects/kamelas?jump=welcome
	git repository : userid@cpp.koses.co.kr:/srv/repos/git/Kamelas.git


Description
	프로세스간 통신 라이브러리 (IPC)


Copyrights
	hcchoi ( hcchoi@koses.co.kr )


Version History
	2009.?		first release
	2013.8.		ported for delphi environment

Comments
	이 라이브러리 (가칭 까멜라스)는 MMI, SEQ 와 같이 서로 다른 프로세스간의 통신(IPC)를
    지원하기 위해 만들어진 것으로, 서로 공유 이름과 공유 데이타 구조체를 외부 헤더 파일등을
	통해 공유하고, MMI 측은 SEQ에게 명령을 전달하고, 해당 명령이 정상적으로 전달되었는지 여부를
	타임아웃등을 통해 확인가능하고, SEQ 측은 매 싸이클 마다 MMI 로 부터 명령이 수신되었는지 여부를
	대기하여 명령이 있을 경우 이를 조속한 시간내에 처리한후 응답을 주도록 한다.
	내부적으로는 공유 메모리를 사용하고, 이를 통해 데이타를 주고, 받는 용도로 사용하고 있다.

how to use
	1. kamelaslib.h 파일을 include 한다
	2. KamelasBase* p = CreateKamelas(...); 을 이용하여 객체를 생성한다
	3. 클라이언트(MMI) 측은 p->send(...); p->recv(...); 로 통신을 진행한다.
	3. 서버(SEQ)측은 p->recv(...); p->send(...); 로 통신을 진행한다.
	4. DestroyKamelas(p) 을 이용하여 객체의 자원을 해제한다

**********************************************************************/
						
						
#include "kamelasbase.h"
#include "kamelasfactory.h"
#include "kamelasmanager.h"

#ifdef __BORLANDC__
	#pragma comment(lib, "kamelaslib.lib")
#else
	#ifdef _DEBUG
		#ifdef _UNICODE
			#pragma comment(lib, "kamelaslibDU.lib")
		#else
			#pragma comment(lib, "kamelaslibD.lib")
        #endif
    #else
		#ifdef _UNICODE
			#pragma comment(lib, "kamelaslibRU.lib")
		#else
            #pragma comment(lib, "kamelaslibR.lib")
        #endif
	#endif
#endif

using namespace kamelaslib;


#endif