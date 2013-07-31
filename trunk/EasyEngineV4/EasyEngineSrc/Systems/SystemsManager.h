#include "ISystems.h"


class CSystemsManager : public ISystemsManager
{
	IGeometryManager&	m_oGeometryManager;
public:
	IBone*	CreateBone() const;
	CSystemsManager( Desc& oDesc );
};

extern "C" _declspec(dllexport) ISystemsManager* CreateSystemsManager( ISystemsManager::Desc& oDesc );