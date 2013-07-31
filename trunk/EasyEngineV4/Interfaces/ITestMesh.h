#ifndef ITESTMESH_H
#define ITESTMESH_H

#include <vector>
#include <map>
#include "IRenderer.h"

class CNode;
class CMatrix;
class ITestShaderManager;

class ITestMesh
{
public:

	
	virtual void	SetHierarchyBone( CNode* pBone ) = 0;
};

#endif // ITESTMESH_H