#ifndef BMELOADER_H
#define BMELOADER_H

#include "Loader.h"

// stl
#include <vector>
#include <string>

using namespace std;

class IGeometryManager;

struct CColor
{
	float r,g,b,a;
};

class CBMELoader : public ILoader
{
	IFileSystem&		m_oFileSystem;
	string				m_sExportPluginVersion;
	IGeometryManager&	m_oGeometryManager;

	void			LoadMesh( CBinaryFileStorage& fs, CMeshInfos& mi );
	void			LoadMaterial( CBinaryFileStorage& fs, CMaterialInfos& mi );
	void			LoadVertexWeight( CBinaryFileStorage& fs, CMeshInfos& mi );
	void			LoadGeometry( CBinaryFileStorage& fs, CMeshInfos& mi );

	void			LoadSkeleton( CBinaryFileStorage& fs, CAnimatableMeshData& oData );
	void			LoadKeyBoundingBoxes( CBinaryFileStorage& fs, CMeshInfos& mi );
	void			LoadBonesBoundingBoxes( CBinaryFileStorage& fs, map< int, IBox* >& mBonesBoundingBoxes );

	void			ExportAscii( string sFileName, const CAnimatableMeshData& oData );
	void			ExportMaterialAscii( const CChunk& oInChunk, FILE* pOutFile );
	void			ExportGeometryAscii( const CChunk& oInChunk, FILE* pOutFile );
	void			ExportSkeletonAscii( const CChunk& oInChunk, FILE* pOutFile );
	void			ExportVertexWeightAscii( const CChunk& oInChunk, FILE* pOutFile );
	void			ExportMaterialAscii( const CMeshInfos& oData, FILE* pOutFile );
	void			ExportGeometryAscii( const CMeshInfos& oData, FILE* pOutFile );
	void			ExportSkeletonAscii( const CAnimatableMeshData& oData, FILE* pOutFile );
	void			ExportSkeleton( const CAnimatableMeshData& oData, CAsciiFileStorage& store );
	void			ExportVertexWeightAscii( const CMeshInfos& oData, FILE* pOutFile );
	template<class T>
	void			ExportArrayAscii( string sTypeName, string sMessage, int nColCount, const vector< T >& vArray, FILE* pFile );

	template<class T> 
	void			SerializeAscii( const vector<T>& oVertex, string sTypeName, int nColCount, string& sOut  );


	void			ExportAnimatableMeshInfos( CBinaryFileStorage& store, const ILoader::CAnimatableMeshData& mi );
	void			ExportMeshInfos( const CMeshInfos& mi, CBinaryFileStorage& fs );
	void			ExportMaterialInfos( const CMaterialInfos& mi, CBinaryFileStorage& fs );
	void			ExportSkinningInfos( const ILoader::CMeshInfos& mi , CBinaryFileStorage& fs );
	void			ExportKeyBoundingBoxesInfos( const map< string,  map< int, IBox* > >& mKeyBoundingBoxes, CBinaryFileStorage& fs );
	void			ExportSkeleton( const CAnimatableMeshData& mi, CBinaryFileStorage& fs );
	void			ExportBonesBoundingBoxes( const map< int, IBox* >& m, CBinaryFileStorage& fs );

	void			ExportAnimatableMeshInfos( CAsciiFileStorage& store, const ILoader::CAnimatableMeshData& mi );
	void			ExportMeshInfos( const CMeshInfos& mi, CAsciiFileStorage& fs );
	void			ExportMaterialInfos( const CMaterialInfos& mi, CAsciiFileStorage& store );
	void			ExportSkinningInfos( const ILoader::CMeshInfos& mi , CAsciiFileStorage& fs );
	void			ExportBonesBoundingBoxes( const map< int, IBox* >& m, CAsciiFileStorage& fs );
	void			ExportKeyBoundingBoxesInfos( const map< string, map< int, IBox* > >& mKeyBoundingBoxes, CAsciiFileStorage& fs );

public:
	CBMELoader( IFileSystem& oFileSystem, IGeometryManager& oGeometryManager );
	void			Load( string sFileName, ILoader::IRessourceInfos& ri, IFileSystem& );
	void			Export( string sFileName, const ILoader::IRessourceInfos& ri );
};

#endif // BMELOADER_H