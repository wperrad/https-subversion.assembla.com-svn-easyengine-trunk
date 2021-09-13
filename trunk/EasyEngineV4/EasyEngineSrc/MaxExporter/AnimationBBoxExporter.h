#pragma once
#include "MaxExporter.h"


class CAnimationBBoxExporterClassDesc : public CMaxExporterClassDesc
{
	void*		Create(BOOL loading = FALSE);
	Class_ID	ClassID();
};


class CAnimationBBoxExporter : public CMaxExporter
{

public:
	CAnimationBBoxExporter();
	virtual ~CAnimationBBoxExporter();

	int					ExtCount();
	const TCHAR*		Ext(int n);
	int					DoExport(const TCHAR *name, ExpInterface *ei, Interface *pInterface, BOOL suppressPrompts, DWORD options);

private:
	void				StoreMeshToMeshInfos(Interface* pInterface, INode* pMesh, ILoader::CMeshInfos& mi) override;

	vector<wstring>		m_vExtension;
};

