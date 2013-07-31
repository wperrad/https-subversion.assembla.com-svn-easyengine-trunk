#include "MaxExporter.h"

// stl
#include <vector>
#include <string>
#include <map>

// Engine
#include "../Utils2/chunk.h"
#include "math/matrix.h"
#include "ILoader.h"
#include "IGeometry.h"

using namespace std;



class CBinaryMeshMaxExporterClassDesc : public CMaxExporterClassDesc
{
public:
	void*		Create(BOOL loading = FALSE);
	Class_ID	ClassID();
};


class CBinaryMeshMaxExporter : public CMaxExporter
{
	vector< std::string >				m_vExtension;
	bool								m_bMultipleSmGroup;
	int									m_nCurrentSmGroup;
	bool								m_bFlipNormals;
	bool								m_bOpenglCoord;
	int									m_nMaterialCount;
	map< string, int >					m_mBoneIDByName;
	bool								m_bLog;
	bool								m_bExportSkinning;
	bool								m_bExportBoundingBox;
	bool								m_bExportBBoxAtKey;
	FILE*								m_pLogFile;
	map< int, CBox >					m_mBoneBox;

	static CBinaryMeshMaxExporter*		s_pCurrentInstance;


	void			GetMaterialTextureName( Mtl* pMaterial, std::string& sTextureName, int nMapIndex )const;
	void			GetNormals( Mesh& oMesh, std::vector< float >& vFaceNormal, std::vector< float >& vVertexNormal );
	void			GetFacesMtlArray( Mesh& oMesh, std::vector< unsigned short >& vMtlIDArray );
	void			GetSkeleton( INode* pRoot, std::map< std::string, INode* >& mBone );
	void			GetMeshesIntoHierarchy( Interface* pInterface, INode* pNode, vector< ILoader::CMeshInfos >& vMeshInfos );
	void			GetBonesIDByName( INode* pRoot, std::map< std::string, int >& mBoneIDByName ) const;
	void			GetBoneByID( const std::map< std::string, INode* >& mBoneByName, const std::map< std::string, int >& mBoneIDByName, std::map< int, INode* >& mBoneByID );
	void			GetWeightTable( IWeightTable& oWeightTable, const std::map< std::string, int >& mBoneID, string sObjectName );
	void			GetGeometry( Interface* pInterface, vector< ILoader::CMeshInfos >& vMeshInfos, INode* pRoot );
	void			GetBonesBoundingBoxes( const Mesh& oMesh, const IWeightTable& oWeightTable, const Matrix3& oModelTM, map< int, CBox >& mBoneBox );
	void			StoreMeshToMeshInfos( Interface* pInterface, INode* pMesh, ILoader::CMeshInfos& mi );
	void			StoreMaxMaterialToMaterialInfos( Mtl* pMaterial, ILoader::CMaterialInfos& mi );
	void			StoreSkinningToMeshInfos( const IWeightTable& wt, ILoader::CMeshInfos& mi );
	void			StoreMaxColorToVector( const Color c, vector< float >& v );
	void			StoreSkeletonToSkeletonMap( const map< int, INode* >& mNodeID, ILoader::CAnimatableMeshData& ami );

	bool			DumpModels( const std::string& sFilePath, ILoader::CAnimatableMeshData& ami );
	void			UpdateVersionFile( string sVersion );
	Point3			GetVertexNormal( Mesh& oMesh, int faceNo, RVertex* rv );
	void			GetBoundingBoxAtKey( Interface *pInterface, const vector< float >& vVertexArray, const vector< unsigned int >& vIndexArray, const IWeightTable& wt, map< int, CBox >& oBox );
	void			WriteLog( string sMessage );

	static INT_PTR CALLBACK ExportDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	

public:
	CBinaryMeshMaxExporter();
	~CBinaryMeshMaxExporter();
	int				ExtCount();
	const TCHAR*	Ext(int n);
	int				DoExport(const TCHAR *name,ExpInterface *ei, Interface *i, BOOL suppressPrompts=FALSE, DWORD options = 0 );
	
};

