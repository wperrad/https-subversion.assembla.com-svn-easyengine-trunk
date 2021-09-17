#pragma once

#include "ILoader.h"


class CBBoxLoader :	public ILoader
{
public:
	CBBoxLoader(IFileSystem& oFileSystem, IGeometryManager& oGeometryManager);
	virtual ~CBBoxLoader();

	void					Load(string sFileName, ILoader::IRessourceInfos& ri, IFileSystem&) override;
	void					Export(string sFileName, ILoader::IRessourceInfos& ri) override;

private:
	void					ExportKeyBoundingBoxesInfos(const map< string, map< int, IBox* > >& mKeyBoundingBoxes, CBinaryFileStorage& fs);
	void					LoadKeyBoundingBoxes(CBinaryFileStorage& fs, map< string, map< int, IBox* > >& mKeyBoundingBoxes);
	void					UpdateBBoxInfos(CAnimationBBoxInfos& oldBBox, const CAnimationBBoxInfos& newBBox);

	IGeometryManager&		m_oGeometryManager;
	IFileSystem&			m_oFileSystem;
};

