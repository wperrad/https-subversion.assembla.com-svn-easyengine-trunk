#include "LinkedCamera.h"
#include "MobileEntity.h"
#include "ISystems.h"

CLinkedCamera::CLinkedCamera( float fFov ):
CCamera( fFov )
{
	m_sEntityName = "Linked camera";
}

CLinkedCamera::~CLinkedCamera()
{
}

void CLinkedCamera::Move( float fOffsetYaw, float fOffsetPitch, float fOffsetRoll, float fAvanceOffet, float fLeftOffset, float fUpOffset )
{
	throw 1;
}

void CLinkedCamera::SetEntityName(string sName)
{
	m_sEntityName = sName;
}

void CLinkedCamera::GetEntityName(string& sName)
{
	sName = m_sEntityName;
}

void CLinkedCamera::Zoom(int value)
{
	float y = value * 10;
	float z = value * 20;
	LocalTranslate(0, y, z);
}

void CLinkedCamera::Link(CNode* pNode)
{	
	CNode::Link(pNode);
	return;
	CMobileEntity* pPerso = dynamic_cast<CMobileEntity*>(pNode);
	if (pPerso) {
		 CNode* pTete = pPerso->GetSkeletonRoot()->GetChildBoneByName("Tete");
		 if (pTete) {
			 CNode::Link(pTete);
			 CVector pos;
			 pNode->GetWorldPosition(pos);
			 SetWorldPosition(pos);

			 /*
			 Pitch(-90.f);
			 Roll(180.f);
			 LocalTranslate(0.f, 5.f, 0.f);
			 LocalTranslate(0.f, 5.f, -10.f);*/
		 }
	}
}

void CLinkedCamera::Update() 
{ 
	CNode::Update(); 
}