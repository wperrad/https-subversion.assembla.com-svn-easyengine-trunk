#include "EntityManager.h"
#include "Entity.h"
#include "Repere.h"
#include "BoxEntity.h"
#include "LightEntity.h"
#include "Exception.h"
#include "SphereEntity.h"
#include "IGeometry.h"
#include "NPCEntity.h"
#include "LineEntity.h"
#include "CylinderEntity.h"

CEntityManager::CEntityManager( const Desc& oDesc ):
IEntityManager( oDesc ),
m_oRessourceManager( oDesc.m_oRessourceManager ),
m_oRenderer( oDesc.m_oRenderer ),
m_nLastEntityID( -1 ),
m_pPerso( NULL ),
m_oFileSystem( oDesc.m_oFileSystem ),
m_oCollisionManager( oDesc.m_oCollisionManager ),
m_oGeometryManager( oDesc.m_oGeometryManager )
{
	m_itCurrentParsedEntity = m_mCollideEntities.end();
	m_itCurrentIAEntity = m_mIAEntities.end();
}

void CEntityManager::CreateEntity( IEntity* pEntity, string sName )
{
	if( m_nLastEntityID == -1 )
		m_nLastEntityID = 0;
	m_nLastEntityID++;
	m_mIDEntities[ m_nLastEntityID ] = pEntity;
	m_mEntitiesID[ pEntity ] = m_nLastEntityID;
	m_mNameEntities[ sName ] = pEntity;
	m_mEntitiesName[ pEntity ] = sName;
	IAEntity* pIAEntity = dynamic_cast< IAEntity* >( pEntity );
	if( pIAEntity )
		m_mIAEntities[ pIAEntity ] = 1;
	IFighterEntity* pFighterEntity = dynamic_cast< IFighterEntity* >( pEntity );
	if( pFighterEntity )
		m_mFighterEntities[ pFighterEntity ] = 1;
}

IEntity* CEntityManager::CreateEntity( std::string sFileName, string sTypeName, IRenderer& oRenderer, bool bDuplicate )
{
	CEntity* pEntity = NULL;
	if( sTypeName.size() == 0 )
		pEntity = new CEntity( sFileName, m_oRessourceManager, oRenderer, this, m_oGeometryManager, m_oCollisionManager, bDuplicate );
	else if( sTypeName == "Human" )
		pEntity = new CMobileEntity( sFileName, m_oRessourceManager, oRenderer, this, &m_oFileSystem, m_oCollisionManager, m_oGeometryManager );
	string sName;
	pEntity->GetName( sName );
	CreateEntity( pEntity, sName );
	return pEntity;
}

IEntity* CEntityManager::CreateEntity( string sName )
{
	CEntity* pEntity = new CEntity( m_oRessourceManager, m_oRenderer, this, m_oGeometryManager, m_oCollisionManager );
	CreateEntity( pEntity );
	m_mNameEntities[ sName ] = pEntity;
	m_mEntitiesName[ pEntity ] = sName;
	return pEntity;
}

IEntity* CEntityManager::GetEntity( int nEntityID )
{
	map< int, IEntity* >::iterator itEntity = m_mIDEntities.find( nEntityID );
	if( itEntity != m_mIDEntities.end() )
		return itEntity->second;
	return NULL;
}

IEntity* CEntityManager::GetEntity( string sEntityName )
{
	map< string, IEntity* >::iterator itEntity = m_mNameEntities.find( sEntityName );
	if( itEntity != m_mNameEntities.end() )
		return itEntity->second;
	return NULL;
}

IAEntity* CEntityManager::GetFirstIAEntity()
{
	m_itCurrentIAEntity = m_mIAEntities.begin();
	if( m_itCurrentIAEntity != m_mIAEntities.end() )
		return m_itCurrentIAEntity->first;
	return NULL;
}

IAEntity* CEntityManager::GetNextIAEntity()
{
	m_itCurrentIAEntity++;
	if( m_itCurrentIAEntity != m_mIAEntities.end() )
		return m_itCurrentIAEntity->first;
	return NULL;
}

IEntity* CEntityManager::GetFirstMobileEntity()
{
	m_itCurrentMobileEntity = m_mMobileEntity.begin();
	if( m_itCurrentMobileEntity != m_mMobileEntity.end() )
		return m_itCurrentMobileEntity->first;
	return NULL;
}

IEntity* CEntityManager::GetNextMobileEntity()
{
	m_itCurrentMobileEntity++;
	if( m_itCurrentMobileEntity != m_mMobileEntity.end() )
		return m_itCurrentMobileEntity->first;
	return NULL;
}

IEntity* CEntityManager::CreateRepere( IRenderer& oRenderer )
{
	IEntity* pEntity = new CRepere( oRenderer );
	CreateEntity( pEntity );
	return pEntity;
}

IEntity* CEntityManager::CreateBox( IRenderer& oRenderer, const CVector& oDimension )
{
	IBox* pBox = m_oGeometryManager.CreateBox();
	pBox->Set( -oDimension / 2.f, oDimension );
	CBoxEntity* pBoxEntity = new CBoxEntity( oRenderer, *pBox );	
	CreateEntity( pBoxEntity );
	return pBoxEntity;
}

ISphere& CEntityManager::GetSphere( IEntity* pEntity )
{
	CSphereEntity* pSphereEntity = static_cast< CSphereEntity* >( pEntity );
	return pSphereEntity->GetSphere();
}

IBox& CEntityManager::GetBox( IEntity* pEntity )
{
	CBoxEntity* pBoxEntity = static_cast< CBoxEntity* >( pEntity );
	return pBoxEntity->GetBox();
}

IEntity* CEntityManager::CreateMobileEntity( string sFileName, IFileSystem* pFileSystem )
{
	IEntity* pEntity = new CMobileEntity( sFileName, m_oRessourceManager, m_oRenderer, this, pFileSystem, m_oCollisionManager, m_oGeometryManager );
	CreateEntity( pEntity );
	return pEntity;
}

IEntity* CEntityManager::CreateNPC( string sFileName, IFileSystem* pFileSystem )
{
	IEntity* pEntity = new CNPCEntity( sFileName, m_oRessourceManager, m_oRenderer, this, pFileSystem, m_oCollisionManager, m_oGeometryManager );
	CreateEntity( pEntity );
	return pEntity;
}

int	CEntityManager::GetEntityID( IEntity* pEntity )
{
	map< IEntity*, int >::iterator itEntity = m_mEntitiesID.find( pEntity );
	if( itEntity != m_mEntitiesID.end() )
		return itEntity->second;
	return -1;
}

int CEntityManager::GetEntityCount()
{
	return (int)m_mEntitiesID.size();
}

IEntity* CEntityManager::CreateLightEntity( CVector Color, IRessource::TLight type, float fIntensity )
{
	IRessource* pLight = m_oRessourceManager.CreateLight( Color, type, fIntensity, m_oRenderer );
	CLightEntity* pLightEntity = new CLightEntity( pLight, m_oRessourceManager, m_oRenderer, m_oGeometryManager, m_oCollisionManager );
	CreateEntity( pLightEntity );
	return pLightEntity;
}

void CEntityManager::SetLightIntensity( int nID, float fIntensity )
{
	CLightEntity* pLightEntity = dynamic_cast< CLightEntity* >( m_mIDEntities[ nID ] );
	if( !pLightEntity )
	{
		CBadTypeException e;
		throw e;
	}
	pLightEntity->SetIntensity( fIntensity );
}

void CEntityManager::DestroyEntity( IEntity* pEntity )
{
	map< IEntity*, int >::iterator itEntityID = m_mEntitiesID.find( pEntity );
	if( itEntityID != m_mEntitiesID.end() )
	{
		map< int, IEntity* >::iterator itIDEntity = m_mIDEntities.find( itEntityID->second );
		m_mEntitiesID.erase( itEntityID );
		m_mIDEntities.erase( itIDEntity );
		map< IEntity*, string >::iterator itEntityName = m_mEntitiesName.find( pEntity );
		if( itEntityName != m_mEntitiesName.end() )
		{
			map< string, IEntity* >::iterator itNameEntity = m_mNameEntities.find( itEntityName->second );
			if( itNameEntity != m_mNameEntities.end() )			
				m_mNameEntities.erase( itNameEntity );
			m_mEntitiesName.erase( itEntityName );
		}
		delete pEntity;
	}
}

void CEntityManager::Clear()
{
	m_mIDEntities.clear();
	m_mEntitiesID.clear();
	m_mNameEntities.clear();
	m_mEntitiesName.clear();
	m_nLastEntityID = -1;
}

void CEntityManager::DestroyAll()
{
	map< int, IEntity* >::iterator itIDEntity = m_mIDEntities.begin();
	while( itIDEntity != m_mIDEntities.end() )
	{
		IEntity* pEntity = itIDEntity->second;
		DestroyEntity( pEntity );
		itIDEntity = m_mIDEntities.begin();
	}
}

void CEntityManager::AddEntity( IEntity* pEntity, string sEntityName, int nID )
{
	if( nID == -1 )
	{
		if( m_nLastEntityID == -1 )
			m_nLastEntityID = 0;
		m_nLastEntityID++;
		nID = m_nLastEntityID;
	}
	m_mIDEntities[ nID ] = pEntity;
	m_mEntitiesID[ pEntity ] = nID;
	m_mNameEntities[ sEntityName ] = pEntity;
	m_mEntitiesName[ pEntity ] = sEntityName;
}

void CEntityManager::SetZCollisionError( float e )
{
	CBody::SetZCollisionError( e );
}

void CEntityManager::SetPerso( IEntity* pPerso )
{
	if( m_pPerso )
		m_pPerso->SetCurrentPerso( false );
	m_pPerso = static_cast< CMobileEntity* >( pPerso );
	m_pPerso->SetCurrentPerso( true );
}

IEntity* CEntityManager::GetPerso()
{
	return m_pPerso;
}

IEntity* CEntityManager::CreateSphere( float fSize )
{
	IEntity* pSphere = CreateEntity( "sphere.bme", "", m_oRenderer );
	CreateEntity( pSphere, "Sphere" );
	return pSphere;
}

void CEntityManager::AddCollideEntity( IEntity* pEntity )
{
	int nID = (int)m_mCollideEntities.size();
	m_mCollideEntities[ pEntity ] = nID;
}

void CEntityManager::RemoveCollideEntity( IEntity* pEntity )
{
	map< IEntity*, int >::iterator itEntity = m_mCollideEntities.find( pEntity );
	if( itEntity != m_mCollideEntities.end() )
		m_mCollideEntities.erase( itEntity );
}

IEntity* CEntityManager::GetFirstCollideEntity()
{
	m_itCurrentParsedEntity = m_mCollideEntities.begin();
	if( m_itCurrentParsedEntity != m_mCollideEntities.end() )
		return m_itCurrentParsedEntity->first;
	return NULL;
}

IEntity* CEntityManager::GetNextCollideEntity()
{
	m_itCurrentParsedEntity++;
	if( m_itCurrentParsedEntity != m_mCollideEntities.end() )
		return m_itCurrentParsedEntity->first;
	return NULL;
}

int CEntityManager::GetCollideEntityID( IEntity* pEntity )
{
	map< IEntity*, int >::iterator itEntityID = m_mCollideEntities.find( pEntity );
	if( itEntityID != m_mCollideEntities.end() )
		return itEntityID->second;
	return -1;
}

IFighterEntity* CEntityManager::GetFirstFighterEntity()
{
	m_itCurrentFighterEntity = m_mFighterEntities.begin();
	if( m_itCurrentFighterEntity != m_mFighterEntities.end() )
		return m_itCurrentFighterEntity->first;
	return NULL;
}

IFighterEntity* CEntityManager::GetNextFighterEntity()
{
	m_itCurrentFighterEntity++;
	if( m_itCurrentFighterEntity != m_mFighterEntities.end() )
		return m_itCurrentFighterEntity->first;
	return NULL;
}

IEntity* CEntityManager::CreateLineEntity( const CVector& first, const CVector& last )
{
	return new CLineEntity( m_oRenderer, first, last );
}

IEntity* CEntityManager::CreateCylinder( float fRadius, float fHeight )
{
	return new CCylinderEntity( m_oGeometryManager, m_oRenderer, m_oRessourceManager, m_oCollisionManager, fRadius, fHeight );
}

extern "C" _declspec(dllexport) IEntityManager* CreateEntityManager( const IEntityManager::Desc& oDesc )
{
	return new CEntityManager( oDesc );
}