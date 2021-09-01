#include "IEntity.h"
#include "Scene.h"

// stl
#include <map>

class CSceneManager : public ISceneManager
{
	EEInterface&					m_oInterface;
	IRessourceManager&				m_oRessourceManager;
	IRenderer&						m_oRenderer;
	ICameraManager&					m_oCameraManager;
	ICollisionManager&				m_oCollisionManager;
	IEntityManager&					m_oEntityManager;
	ILoaderManager&					m_oLoaderManager;

	std::map< string, CScene* >		m_mStringScene;
	std::map< CScene*, string >		m_mSceneString;

	CScene*							GetScene( IEntity* pScene );

public:

	CSceneManager(EEInterface& oInterface);
	virtual ~CSceneManager();

	IScene*	CreateScene(string sSceneName, string sRessourceFileName, string diffuseFileName) override;
	IScene*		GetScene( std::string sSceneName );
	string		GetName() override;
};

extern "C" _declspec(dllexport) ISceneManager* CreateSceneManager(EEInterface& oInterface);