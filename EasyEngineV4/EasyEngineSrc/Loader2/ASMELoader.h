#ifndef AMELOADER_H
#define AMELOADER_H

#include <map>
#include "Meshloader.h"


typedef std::map< unsigned int, float > Map_BoneID_Weight;
typedef std::vector< Map_BoneID_Weight > CSkin;


class CASMELoader : public IMeshLoader
{		
	void				LoadSkin(CChunk& chunk);
	void				LoadIndexArray(int nFaceCount, CChunk& chunk);		
	void				OptimizedSkinedMesh();	
	void				ExportHierarchyNode(CEasyFile& file);
	void				LoadHierarchy(CChunk& chunk);
	virtual void		LoadIndexArray(int nFaceCount, unsigned int* pIndexArray){}		
	

public:
						CASMELoader(void);
	virtual				~CASMELoader(void);	
	bool				Load( const std::string& sFileName, CChunk& chunk, IFileSystem& oFileSystem );	
	void				Export( const std::string& sFileName, const CChunk& chunk);		
	
};



#endif //AMELOADER_H