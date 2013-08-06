#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "IEntity.h"
#include "IRessource.h"

class IRenderer;
class IGeometryManager;
class CHuman;
class IFighter;

class CEntityManager : public IEntityManager
{
	map< IEntity*, int >::const_iterator	m_itCurrentParsedEntity;
	IRessourceManager&		m_oRessourceManager;
	IGeometryManager&		m_oGeometryManager;
	IRenderer&				m_oRenderer;
	map< int, IEntity* >	m_mIDEntities;	
	map< IEntity*, int >	m_mEntitiesID;
	map< string, IEntity* >	m_mNameEntities;
	map< IEntity*, string >	m_mEntitiesName;
	int						m_nLastEntityID;
	CHuman*					m_pPerso;
	IFileSystem&			m_oFileSystem;
	ICollisionManager&		m_oCollisionManager;
	void					CreateEntity( IEntity* pEntity, string sName = "noname" );
	map< IEntity*, int >	m_mCollideEntities;
	map< IFighter*, int >	m_mFighterEntities;
	map< IFighter*, int >::iterator	m_itCurrentFighter;

public:
	CEntityManager( const Desc& oDesc );
	IEntity*			CreateEntity( string sFileName, string sTypeName, IRenderer& oRenderer, bool bDuplicate = false );
	IEntity*			CreateEntity( string sName = "noname" );
	IEntity*			CreateRepere( IRenderer& oRenderer );
	IEntity*			CreateHuman( string sFileNamee, IFileSystem* pFileSystem );
	IEntity*			GetEntity( int nEntityID );
	IEntity*			GetEntity( string sEntityName );
	int					GetEntityID( IEntity* pEntity );
	int					GetEntityCount();
	IEntity*			CreateLightEntity( CVector Color, IRessource::TLight type, float fIntensity );
	void				SetLightIntensity( int nID, float fIntensity );
	void				DestroyEntity( IEntity* pEntity );
	void				DestroyAll();
	void				Clear();
	void				AddEntity( IEntity* pEntity, string sEntityName = "noname", int nID = -1 );
	void				SetZCollisionError( float e );
	void				SetPerso( IEntity* pPerso );
	IEntity*			GetPerso();
	IEntity*			CreateSphere( float fSize );
	IEntity*			CreateBox( IRenderer& oRenderer, const CVector& oDimension );
	IEntity*			CreateSphere( IRenderer& oRenderer, ISphere& oSphere );
	ISphere&			GetSphere( IEntity* pSphereEntity );
	IBox&				GetBox( IEntity* pBoxEntity );
	void				AddCollideEntity( IEntity* pEntity );
	void				RemoveCollideEntity( IEntity* pEntity );
	IEntity*			GetFirstCollideEntity();
	IEntity*			GetNextCollideEntity();
	int					GetCollideEntityID( IEntity* pEntity );
	IFighter*			GetFirstFighter();
	IFighter*			GetNextFighter();
};

extern "C" _declspec(dllexport) IEntityManager* CreateEntityManager( const IEntityManager::Desc& );

#endif // ENTITYMANAGER_H