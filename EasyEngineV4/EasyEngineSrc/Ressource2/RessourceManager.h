#ifndef RESSOURCEMANAGER_CPP
#ifndef MESH_CPP
#error
#endif //  MESH_CPP
#endif // RESSOURCEMANAGER_CPP



#ifndef RESSOURCEMANAGER_H
#define RESSOURCEMANAGER_H

#include <windows.h>
#include <map>
#include <string>
#include "../Utils2/Chunk.h"
#include "IRessource.h"
#include "IRessource.h"
#include "ILoader.h"

using namespace std;

class IMeshLoader;
class ITextureLoader;
class CTextureBase;
class CMesh;
class CMaterial;
class CLight;
class CHierarchyMesh;
class CLoaderManager;
class IRenderer;
class ILoaderManager;
class ITestMesh;
class ITestShaderManager;
class IDrawTool;
class ISystemsManager;
struct CMaterialInfos;


typedef unsigned int uint;

class CRessourceManager : public IRessourceManager
{
	typedef IRessource*( *TRessourceCreation)(  string sFileName, CRessourceManager*, IRenderer& );
	typedef ITestMesh*( *TTestRessourceCreation )( string sFileName, CRessourceManager*, ITestShaderManager&, IRenderer& );
	std::map< std::string, IRessource* >			m_mRessource;
	ILoaderManager&									m_oLoaderManager;
	ISystemsManager&								m_oSystemManager;

	vector< string >								m_vErrorMessage;
	IRessource*										GetRessourceByExtension( std::string sRessourceFileName, IRenderer& oRenderer );	
	IRessource*										CreateMaterial( ILoader::CMaterialInfos& mi, IRenderer& oRenderer, ITexture* pAlternative = NULL );
	IAnimatableMesh*								CreateMesh( ILoader::CAnimatableMeshData& oData, IRenderer& oRenderer, IRessource* pMaterial = NULL );
	IBone*											LoadSkeleton( ILoader::CAnimatableMeshData& oData );

	//static void										SetLocalTMByWorldTM( CNode* pNode );
	static IRessource*								CreateMesh( string sFileName, CRessourceManager* pRessourceManager, IRenderer& oRenderer );
	static IRessource*								CreateCollisionMesh(string sFileName, CRessourceManager* pRessouceManager, IRenderer& oRenderer);
	static IRessource*								CreateAnimation( string sFileName, CRessourceManager* pRessourceManager, IRenderer& oRenderer );
	//static IRessource*								CreateHierarchyMesh( string sFileName, CRessourceManager* pRessourceManager, IRenderer& oRenderer );
	static IRessource*								CreateTexture( string sFileName, CRessourceManager* pRessourceManager, IRenderer& oRenderer );
	static IRessource*								CreateLight( string sFileName, CRessourceManager* pRessourceManager, IRenderer& oRenderer );
	static void										CollectMaterials( const ILoader::CMaterialInfos& oMaterialInfos, IRenderer& oRenderer, IShader* pShader, IRessourceManager* pRessourceManager, std::map< int, CMaterial* >& vMaterials );
	static CMaterial*								CreateMaterial( const ILoader::CMaterialInfos*, IRenderer& oRenderer, IShader* pShader, IRessourceManager* pRessourceManager );

	static ITestMesh*								CreateTestMesh( const CChunk&, CRessourceManager* , ITestShaderManager&, IRenderer& );


	std::map< std::string, TRessourceCreation >		m_mRessourceCreation;
	std::map< std::string, TTestRessourceCreation >	m_mTestRessourceCreation;
	int												m_nLightCount;
	IRenderer*										m_pCurrentRenderer;
	bool											m_bCatchException;

	IDrawTool*										m_pDrawTool;

public:


						CRessourceManager( const Desc& desc );
	virtual 			~CRessourceManager();
	void				EnableCatchingException( bool bEnable );
	bool				IsCatchingExceptionEnabled();
	void				SetCurrentRenderer( IRenderer* pRenderer );
	IRessource*			GetRessource( const std::string& sRessourceFileName, IRenderer& oRenderer, bool bDuplicate = false );
	ITexture*			CreateTexture2D( IRenderer& oRenderer, IShader* pShader, int nUnitTexture, vector< unsigned char >& vData, int nWidth, int nHeight, IRenderer::TPixelFormat eFormat );
	ITestMesh*			GetTestRessource( const std::string& sRessourceFileName, ITestShaderManager&, IRenderer& oRenderer );
	int					GetLightCount();
	void				SetDrawTool( IDrawTool* pDrawTool ){ m_pDrawTool = pDrawTool; }
	IRessource*			CreateLight( CVector Color, IRessource::TLight type, float fIntensity, IRenderer& oRenderer );
	void				SetLightIntensity( IRessource* pLight, float fIntensity );
	float				GetLightIntensity( IRessource* pRessource );
	CVector				GetLightColor( IRessource* pRessource );
	IRessource::TLight	GetLightType( IRessource* pRessource );
	void				DisableLight( IRessource* pRessource );
	void				PopErrorMessage( string& sMessage );
	void				DestroyAllRessources();
};



extern "C" _declspec(dllexport) IRessourceManager* CreateRessourceManager( IRessourceManager::Desc& oDesc );

#endif  //RESSOURCEMANAGER_H