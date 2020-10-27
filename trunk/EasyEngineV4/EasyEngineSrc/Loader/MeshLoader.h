#ifndef MESH_LOADER_H
#define MESH_LOADER_H


#include "loader.h"
#include "../utils2/EasyFile.h"
#include <string>
#include <vector>


class IMeshLoader :	public ILoader
{
protected:

	CEasyFile			m_CurrentFile;		
	void				LoadMeshInfos( CChunk& chunk, IFileSystem& oFileSystem );

	int					LoadNumElement( const std::string& sString );
	void				LoadMaterial( CChunk& chunk );	
	void				LoadVertexArray( int nVertexCount, std::vector< float >& vVertexArray);
	void				LoadNormalArray( int FaceCount, CChunk& oChunk );
	void				LoadUVVertexArray( int nTFaceCount, std::vector< float >& vUVVertexArray);
	void				LoadUVIndexArray( int nTFaceCount, std::vector< unsigned int >& vUVIndexArray);		
	
	// pure virtual	
	virtual void		LoadIndexArray( int nFaceCount, std::vector< unsigned int >& vIndexArray ) = 0;
	
	

public:
	virtual bool		Load( const std::string& sFileName, CChunk& chunk, IFileSystem& oFileSystem );
	void				Load( const std::string& sFileName, ILoader::IRessourceInfos& ri, IFileSystem& ){throw 1;}


										IMeshLoader();
	virtual								~IMeshLoader();
};


#endif //MESH_LOADER_H