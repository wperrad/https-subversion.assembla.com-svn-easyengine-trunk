#include "SystemsManager.h"
#include "Bone.h"

CSystemsManager::CSystemsManager( Desc& oDesc ) : 
ISystemsManager( oDesc ),
m_oGeometryManager( oDesc.m_oGeometryManager )
{
}

IBone*	CSystemsManager::CreateBone() const
{
	return new CBone( m_oGeometryManager );
}

string CSystemsManager::GetName()
{
	return "SystemManager";
}

extern "C" _declspec(dllexport) ISystemsManager* CreateSystemsManager( ISystemsManager::Desc& oDesc )
{
	return new CSystemsManager( oDesc );
}