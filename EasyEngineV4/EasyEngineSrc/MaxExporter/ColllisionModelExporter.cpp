#include "CollisionModelExporter.h"

// Interfaces
#include "IFileSystem.h"
#include "IGeometry.h"

// EE utils
#include "../Utils2/StringUtils.h"

// MAX
#include "3dsmaxport.h"
#include "simpobj.h"

#define COLLISIONMODELPLUGIN_CLASS_ID	Class_ID(0x85548e10, 0x4a264511)


void* CollisionModelExporterClassDesc::Create(BOOL loading)
{
	return new CollisionModelExporter;
}

Class_ID CollisionModelExporterClassDesc::ClassID()
{
	return COLLISIONMODELPLUGIN_CLASS_ID;
}



CollisionModelExporter::CollisionModelExporter()
{
	m_vExtension.push_back(L"col");
}

int	CollisionModelExporter::ExtCount()
{
	return (int)m_vExtension.size();
}

const TCHAR* CollisionModelExporter::Ext(int n)
{
	return m_vExtension[n].c_str();
}

int	CollisionModelExporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *pInterface, BOOL suppressPrompts, DWORD options)
{
	map< string, INode* > mBones;

	map< int, INode* > mBoneByID;
	INode* pRoot = pInterface->GetRootNode();
	map< string, int > mBoneIDByName;
	HWND hMaxDlg = pInterface->GetMAXHWnd();
	
	ILoader::CCollisionModelInfos model;
	GetPrimitives(pInterface, model.m_vPrimitives);
	if (!g_bInterruptExport)
	{		
		if (DumpCollisionModel(name, model))
			MessageBox(NULL, L"Export terminé", L"Export", MB_OK);
	}
	
	return TRUE;
}

bool CollisionModelExporter::DumpCollisionModel(const TCHAR* name, ILoader::CCollisionModelInfos& model)
{
	string sFileName;
	CStringUtils::ConvertWStringToString(name, sFileName);
	m_pLoaderManager->Export(sFileName, model);
	return true;
}

bool CollisionModelExporter::isCylinder(Object* pObject)
{
	bool ret = pObject->CanConvertToType(Class_ID(CYLINDER_CLASS_ID, 0)) == TRUE;
	return ret;
}


void CollisionModelExporter::GetPrimitives(Interface* pInterface, vector<IGeometry*>& primitives)
{
	INode* pRoot = pInterface->GetRootNode();
	for (int iNode = 0; iNode < pRoot->NumberOfChildren(); iNode++)
	{
		INode* pNode = pRoot->GetChildNode(iNode);
		wstring wTest = pNode->GetName();
		Object* pObject = pNode->EvalWorldState(0).obj;
		IGeometry* pGeometry = NULL;
		if (IsBone(pObject))
		{
			vector<IGeometry*> primitives;
			GetPrimitives(pInterface, primitives);
			continue;
		}
		if (pObject->CanConvertToType(Class_ID(CYLINDER_CLASS_ID, 0)) == TRUE)
		{
			pGeometry = m_pGeometryManager->CreateCylinder();
			StoreCylinderInfos(pNode, *(ICylinder*)pGeometry);
			if (g_bInterruptExport)
				break;
		}
		if (pObject->CanConvertToType(Class_ID(BOXOBJ_CLASS_ID, 0)) == TRUE)
		{
			pGeometry = m_pGeometryManager->CreateBox();
			StoreBoxInfos(pNode, *(IBox*)pGeometry);			
			if (g_bInterruptExport)
				break;
		}
		if(pGeometry)
			primitives.push_back(pGeometry);
	}
}

void CollisionModelExporter::StoreCylinderInfos(INode* pMesh, ICylinder& cylinder)
{
	Mesh& mesh = GetMeshFromNode(pMesh);
	Box3 box = mesh.getBoundingBox();
	CMatrix tm;
	MaxMatrixToEngineMatrix(pMesh->GetObjectTM(0), tm);

	cylinder.Set(tm, (box.Max().x - box.Min().x) / 2.f, box.Max().z - box.Min().z);
	
	/*
	cylinder.m_fHeight = box.Max().z - box.Min().z;
	cylinder.m_fRadius = (box.Max().x - box.Min().x) / 2.f;	
	ci.m_oXForm = tm;*/

	/*
	Object* pObject = pMesh->EvalWorldState(0).obj;
	GenCylinder* pCylinder = dynamic_cast< GenCylinder* > (pObject->ConvertToType(0, Class_ID(CYLINDER_CLASS_ID, 0)));
	}*/
}

void ConvertPoint3ToCVector(const Point3& p, CVector& v)
{
	v.m_x = p.x;
	v.m_y = p.y;
	v.m_z = p.z;
	v.m_w = 1;
}

void CollisionModelExporter::StoreBoxInfos(INode* pMesh, IBox& box)
{
	Mesh& mesh = GetMeshFromNode(pMesh);
	Box3 bbox = mesh.getBoundingBox();
	CMatrix tm;
	MaxMatrixToEngineMatrix(pMesh->GetObjectTM(0), tm);	
	CVector dim;
	ConvertPoint3ToCVector(bbox.Max() - bbox.Min(), dim);
	box.Set(CVector(bbox.Min().x, bbox.Min().y, bbox.Min().z), dim);
	box.SetWorldMatrix(tm);
}