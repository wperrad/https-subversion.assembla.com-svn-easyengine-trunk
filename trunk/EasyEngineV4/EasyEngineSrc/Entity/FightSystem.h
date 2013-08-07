#ifndef IFIGHTER_H
#define IFIGHTER_H

#include "Math/Vector.h"
#include "IRessource.h"


class IEntityManager;
class IAEntity;



class CFightSystem
{
	friend class CScene;
	map< IAEntity*, int >	m_mFighter;
	void	Update();
	IEntityManager&			m_oEntityManager;
	static CFightSystem*	s_pInstance;

	static void				OnHitAnimationCallback( IAnimation::TEvent e, void* pData );
	static void				OnHitReceivedCallback( IAnimation::TEvent e, void* pData );

public:
	CFightSystem( IEntityManager& oEntityManager );
	void	OnHit( IAEntity* pAgressor, string sHitBoneName );
	void	OnReceiveHit( IAEntity* pAssaulted, IAEntity* pAgressor );
	
};

#endif // IFIGHTER_H