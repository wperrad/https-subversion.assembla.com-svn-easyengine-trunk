
class CTimeManager
{
	unsigned long		m_nCurrentTime;
	unsigned long		m_nTimeElapsedSinceLastUpdate;

	static CTimeManager*	s_pInstance;
	CTimeManager();

public:
	
	void				Update();
	unsigned long		GetCurrentTimeInMillisecond();
	int					GetTimeElapsedSinceLastUpdate();

	static CTimeManager*		Instance();
};