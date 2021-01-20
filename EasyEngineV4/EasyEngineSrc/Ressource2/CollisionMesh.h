#pragma once

#include "IRessource.h"



class CCollisionMesh : public ICollisionMesh
{
public:
	struct Desc : public IRessource::Desc
	{
		ILoader::CCollisionModelInfos& m_oCollisionModelInfos;
		Desc(IRenderer& oRenderer, ILoader::CCollisionModelInfos& oCollisionModelInfos);
	};

	CCollisionMesh(CCollisionMesh::Desc& oDesc);
	void		Update();
	void		SetShader(IShader* pShader);
	bool		IsCollide(IBox* pBox);
	IShader*	GetShader() const;
	IGeometry*	GetGeometry(int index);
	int			GetGeometryCount() const;

private:
	ILoader::CCollisionModelInfos	m_oCollisionModelInfos;
};