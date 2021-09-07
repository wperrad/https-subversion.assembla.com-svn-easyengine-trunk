#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "IEntity.h"
#include "IRessource.h"

class IRenderer;
class IGeometryManager;
class CMobileEntity;
class IAEntity;
class CEntity;
class CPlayer;
class CNode;

class CEntityManager : public IEntityManager
{
	EEInterface&							m_oInterface;
	map< CEntity*, int >::const_iterator	m_itCurrentParsedEntity;
	IRessourceManager&						m_oRessourceManager;
	IGeometryManager&						m_oGeometryManager;
	IPathFinder&							m_oPathFinder;
	IRenderer&								m_oRenderer;
	map< int, IEntity* >					m_mIDEntities;	
	map< IEntity*, int >					m_mEntitiesID;
	map< string, IEntity* >					m_mNameEntities;
	map< IEntity*, string >					m_mEntitiesName;
	int										m_nLastEntityID;
	IFileSystem&							m_oFileSystem;
	ICollisionManager&						m_oCollisionManager;
	ICameraManager&							m_oCameraManager;
	void									CreateEntity( IEntity* pEntity, string sName = "noname" );
	map< CEntity*, int >					m_mCollideEntities;
	map< IAEntity*, int >					m_mIAEntities;
	map< IAEntity*, int >::iterator			m_itCurrentIAEntity;
	map< IEntity*, int >					m_mMobileEntity;
	map< IEntity*, int >::iterator			m_itCurrentMobileEntity;
	map< IFighterEntity*, int >				m_mFighterEntities;
	map< IFighterEntity*, int >::iterator	m_itCurrentFighterEntity;
	IGUIManager*							m_pGUIManager;
	CPlayer*								m_pPlayer;

public:
	CEntityManager(EEInterface& oInterface);
	IEntity*			CreateEntity(string sFileName, string sTypeName, bool bDuplicate = false );
	IEntity*			CreateEmptyEntity( string sName = "noname" );
	IEntity*			CreateRepere( IRenderer& oRenderer );
	IEntity*			CreateNPC( string sFileName, IFileSystem* pFileSystem );
	IEntity*			CreateMapEntity(string sFileName, IFileSystem* pFileSystem);
	IEntity*			CreateTestEntity(string sFileName, IFileSystem* pFileSystem);
	IEntity*			CreateMobileEntity( string sFileNamee, IFileSystem* pFileSystem );
	IEntity*			CreatePlayer(string sFileName, IFileSystem* pFileSystem);
	IEntity*			CreatePlaneEntity(int slices, int size, string heightTexture, string diffuseTexture) override;
	void				SetPlayer(IPlayer* player);
	IPlayer*			GetPlayer();
	IEntity*			GetEntity( int nEntityID );
	IEntity*			GetEntity( string sEntityName );
	int					GetEntityID( IEntity* pEntity );
	int					GetEntityCount();
	IEntity*			CreateLightEntity( CVector Color, IRessource::TLight type, float fIntensity );
	float				GetLightIntensity(int nID) override;
	void				SetLightIntensity( int nID, float fIntensity ) override;
	void				DestroyEntity( IEntity* pEntity );
	void				DestroyAll();
	void				Clear();
	void				AddEntity( IEntity* pEntity, string sEntityName = "noname", int nID = -1 );
	void				SetZCollisionError( float e );
	IEntity*			CreateSphere( float fSize );
	IEntity*			CreateQuad(float lenght, float width);
	IEntity*			CreateBox(const CVector& oDimension ) override;
	ISphere&			GetSphere( IEntity* pSphereEntity );
	IBox&				GetBox( IEntity* pBoxEntity );
	void				AddCollideEntity( CEntity* pEntity );
	void				RemoveCollideEntity( CEntity* pEntity );
	CEntity*			GetFirstCollideEntity();
	CEntity*			GetNextCollideEntity();
	int					GetCollideEntityID( CEntity* pEntity );
	IAEntity*			GetFirstIAEntity();
	IAEntity*			GetNextIAEntity();
	IFighterEntity*		GetFirstFighterEntity();
	IFighterEntity*		GetNextFighterEntity();
	IEntity*			GetFirstMobileEntity();
	IEntity*			GetNextMobileEntity();
	IEntity*			CreateLineEntity( const CVector& first, const CVector& last );
	IEntity*			CreateCylinder( float fRadius, float fHeight );
	void				SetGUIManager(IGUIManager* pGUIManager);
	IGUIManager* 		GetGUIManager();
	void				Kill(int entityId);
	void				WearArmor(int entityId, string armorName) override;
	void				WearArmorToDummy(int entityId, string armorName) override;
	void				WearShoes(int entityId, string shoesName) override;

	template<class T>
	void				SerializeNodeInfos(INode* pNode, ostringstream& sLine, int nLevel = 0);
	void				SerializeMobileEntities(INode* pRoot,  string& sText);
	string				GetName() override;
	IBone*				CreateBone() const override;
};

extern "C" _declspec(dllexport) IEntityManager* CreateEntityManager(EEInterface& oInterface);

#endif // ENTITYMANAGER_H