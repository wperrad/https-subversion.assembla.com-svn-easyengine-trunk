#include "Mesh.h"

class CAnimatableMesh : public IAnimatableMesh
{
	IBone*					m_pSkeleton;
	vector< CMesh* >		m_vMeshes;

public:

	CAnimatableMesh(  const IRessource::Desc& oDesc  );
	void 			Update();
	void			SetShader( IShader* pShader ) ;
	IShader*		GetCurrentShader() const;
	void			AddMesh( CMesh* pMesh );
	void			SetSkeleton( IBone* pBone );
	IMesh*			GetMesh( int nIndex );
	IBone*			GetSkeleton();
	unsigned int	GetMeshCount();
	

};