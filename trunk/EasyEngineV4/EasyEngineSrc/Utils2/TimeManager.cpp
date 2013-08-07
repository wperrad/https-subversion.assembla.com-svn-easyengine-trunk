#include "TimeManager.h"
#include <windows.h>
#include <winbase.h>

CTimeManager* CTimeManager::s_pInstance = NULL;

CTimeManager::CTimeManager():
m_nCurrentTime( 0 ),
m_nTimeElapsedSinceLastUpdate( 0 )
{
}

void CTimeManager::Update()
{
	int nCurrentTime = GetTickCount();
	m_nTimeElapsedSinceLastUpdate = nCurrentTime - m_nCurrentTime;
	m_nCurrentTime = nCurrentTime;
}

unsigned long CTimeManager::GetCurrentTimeInMillisecond()
{
	return m_nCurrentTime;
}

int CTimeManager::GetTimeElapsedSinceLastUpdate()
{
	return (int)m_nTimeElapsedSinceLastUpdate;
}

CTimeManager* CTimeManager::Instance()
{
	if( !s_pInstance )
		s_pInstance = new CTimeManager;
	return s_pInstance;
}