#ifndef KAMELASLIBH
#define KAMELASLIBH

/**********************************************************************

Project
	K.A.M.E.L.A.S
	

Introduction
	issue tracker : http://cpp.koses.co.kr/projects/kamelas?jump=welcome
	git repository : userid@cpp.koses.co.kr:/srv/repos/git/Kamelas.git


Description
	���μ����� ��� ���̺귯�� (IPC)


Copyrights
	hcchoi ( hcchoi@koses.co.kr )


Version History
	2009.?		first release
	2013.8.		ported for delphi environment

Comments
	�� ���̺귯�� (��Ī ����)�� MMI, SEQ �� ���� ���� �ٸ� ���μ������� ���(IPC)��
    �����ϱ� ���� ������� ������, ���� ���� �̸��� ���� ����Ÿ ����ü�� �ܺ� ��� ���ϵ���
	���� �����ϰ�, MMI ���� SEQ���� ����� �����ϰ�, �ش� ����� ���������� ���޵Ǿ����� ���θ�
	Ÿ�Ӿƿ����� ���� Ȯ�ΰ����ϰ�, SEQ ���� �� ����Ŭ ���� MMI �� ���� ����� ���ŵǾ����� ���θ�
	����Ͽ� ����� ���� ��� �̸� ������ �ð����� ó������ ������ �ֵ��� �Ѵ�.
	���������δ� ���� �޸𸮸� ����ϰ�, �̸� ���� ����Ÿ�� �ְ�, �޴� �뵵�� ����ϰ� �ִ�.

how to use
	1. kamelaslib.h ������ include �Ѵ�
	2. KamelasBase* p = CreateKamelas(...); �� �̿��Ͽ� ��ü�� �����Ѵ�
	3. Ŭ���̾�Ʈ(MMI) ���� p->send(...); p->recv(...); �� ����� �����Ѵ�.
	3. ����(SEQ)���� p->recv(...); p->send(...); �� ����� �����Ѵ�.
	4. DestroyKamelas(p) �� �̿��Ͽ� ��ü�� �ڿ��� �����Ѵ�

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