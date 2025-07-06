#ifndef _SEQTIMER_H_
#define _SEQTIMER_H_

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <mmsystem.h>


namespace SEQ_TIMER
{
#define SEC_TO_USEC	(1000000)
#define TM_OVERFLOW (4294967296) // 2^32

	class CTimer
	{
	public:
		BOOL Reset()
		{
			m_dwStartTime = timeGetTime();	
			m_dwEndTime   = m_dwStartTime;
			return TRUE;
		}

		BOOL ResetEx(BOOL bOn)
		{
			if(bOn)
				Reset();

			return TRUE;
		}

		DWORD Elapsed()
		{
			m_dwEndTime = timeGetTime();
			if(m_dwStartTime < m_dwEndTime)
				m_dwElapsedTime = m_dwEndTime - m_dwStartTime;
			else
				m_dwElapsedTime = m_dwEndTime + (DWORD)(TM_OVERFLOW - m_dwStartTime);
			
			return m_dwElapsedTime;		//milli-seconds
		}

		BOOL TmOver(DWORD dwDelay)
		{
			DWORD dwElapsedTime = Elapsed(); 

			if(dwDelay < dwElapsedTime)
				return TRUE;
			else 
				return FALSE;
		}




	private:
		DWORD     m_dwStartTime;
		DWORD     m_dwEndTime;
		DWORD     m_dwElapsedTime;

	public:
		CTimer()
		{
			timeBeginPeriod(1);
			m_dwStartTime = timeGetTime();	
			m_dwEndTime   = m_dwStartTime;
		}
		virtual ~CTimer(){}
	};


	/////////////////////////////////////////////////////////////////////
	class CHQTimer
	{
	public:
		BOOL Reset()
		{
			QueryPerformanceCounter(&m_startTime);
			m_endTime.QuadPart = m_startTime.QuadPart;
			return TRUE;
		}

		BOOL ResetEx(BOOL bOn)
		{
			if(bOn)
				Reset();

			return TRUE;
		}

		DWORD Elapsed_us()
		{
			QueryPerformanceFrequency(&m_freq);
			QueryPerformanceCounter(&m_endTime);
			m_elapsedTime.QuadPart = (m_endTime.QuadPart - m_startTime.QuadPart) *SEC_TO_USEC / m_freq.QuadPart;

			return (DWORD)m_elapsedTime.QuadPart;		
		}

		DWORD Elapsed()
		{
			DWORD dwElapsedTime = Elapsed_us() / 1000;
			return (dwElapsedTime);		
		}

		BOOL TmOver_us(DWORD dwDelay_us)
		{
			DWORD dwElapsedTime = Elapsed_us();

			if(dwDelay_us < dwElapsedTime)
				return (TRUE);
			else 
				return (FALSE);
		}

		BOOL TmOver(DWORD dwDelay)
		{
			DWORD dwDelay_us = dwDelay * 1000;
			BOOL  bRet = TmOver_us(dwDelay_us);

			return (bRet);
		}

	private:
		LARGE_INTEGER    m_freq;			
		LARGE_INTEGER    m_startTime;    
		LARGE_INTEGER    m_endTime;      
		LARGE_INTEGER    m_elapsedTime;      

	public:
		CHQTimer(){}

		virtual ~CHQTimer(){}
	};//-----------------------------------------------------------------




	class CBlinkTimer  
	{
	private:
		CTimer m_timer;
		BOOL m_bBlink;

	public:
		CBlinkTimer()
		{
			m_bBlink = FALSE;;
		}

		virtual ~CBlinkTimer()
		{

		}

		BOOL Blink(BOOL bBlinkOn, int nOnTime, int nOffTime)
		{
			if(FALSE == bBlinkOn)
			{
				m_timer.Reset();
				m_bBlink = TRUE;
				return FALSE; 
			}

			if(TRUE == m_bBlink)
			{
				if(FALSE == m_timer.TmOver(nOnTime)) 
					return TRUE;

				m_timer.Reset();
				m_bBlink = FALSE;

			}
			else
			{
				if(FALSE == m_timer.TmOver(nOffTime)) 
					return FALSE;

				m_timer.Reset();
				m_bBlink = TRUE;
			}

			return m_bBlink; 
		}
	};//-----------------------------------------------------------------




	class COffDelayTimer
	{
	private:
		CTimer	m_timer;
		BOOL	m_bOn;

	public:
		COffDelayTimer()
		{
			m_bOn = FALSE;
			m_timer.Reset();
		}

		virtual ~COffDelayTimer(){}

		BOOL OffDelay(BOOL bOn, int nDelay)
		{
			if(m_bOn != bOn)
			{
				m_bOn = bOn;
				m_timer.Reset();
				return FALSE; 
			}

			if(FALSE == m_bOn)
			{
				if(m_timer.TmOver(nDelay))
					return (TRUE);
			}

			return (FALSE);
		}
	};//-----------------------------------------------------------------





	class COnDelayTimer
	{
	private:
		CTimer	m_timer;
		BOOL	m_bOn;

	public:
		COnDelayTimer()
		{
			m_bOn = FALSE;
			m_timer.Reset();
		}

		virtual ~COnDelayTimer(){}

		BOOL OnDelay(BOOL bOn, int nDelay)
		{
			if(m_bOn != bOn)
			{
				m_bOn = bOn;
				m_timer.Reset();
				return FALSE; 
			}

			if(TRUE == m_bOn)
			{
				if(m_timer.TmOver(nDelay))
					return (TRUE);
			}

			return (FALSE);
		}
	};//-----------------------------------------------------------------




	class COneShotDelay
	{
	private:
		CTimer	m_timer;
		BOOL	m_bOn;

	public:
		COneShotDelay()
		{
			m_bOn = FALSE;
			m_timer.Reset();
		}

		virtual ~COneShotDelay(){}

		BOOL OneShot(BOOL bOn, int nDelay)
		{
			if(FALSE == bOn)
			{
				m_bOn = FALSE;
				m_timer.Reset();
			}
			else
			{
				if(FALSE == m_bOn)
				{
					if(m_timer.TmOver(nDelay))
					{
						m_bOn = TRUE;
						return (TRUE);
					}
				}
			}

			return (FALSE);
		}
	};//-----------------------------------------------------------------




};

using namespace SEQ_TIMER;


#endif // _SEQTIMER_H_