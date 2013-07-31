#ifndef HIERARCHYMESH_CPP
#ifndef RESSOURCEMANAGER_CPP
#error
#endif
#endif

#ifndef HIERARCHY_MESH
#define HIERARCHY_MESH


#include "IRessource.h"

class CNode;
class IRenderer;

class CHierarchyMesh : public IRessource
{
	CNode*			m_pRoot;
	
	
public:
	struct Desc : public IRessource::Desc
	{
		CNode* pRootNode;
		Desc( IRenderer& oRenderer, IShader* pShader );
	};

					CHierarchyMesh( const Desc& oDesc );
	virtual			~CHierarchyMesh(void);
	void			Update();
	CNode*			GetRootNode();
	void			SetShader( IShader* pShader ){}
	IShader*		GetCurrentShader() const { return NULL; }
};


#endif //HIERARCHY_MESH