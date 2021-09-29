#include "CharacterEditor.h"
#include "IEntity.h"
#include "Interface.h"
#include "IInputManager.h"
#include "ICamera.h"
#include "IConsole.h"
#include "WorldEditor.h"
#include "EditorManager.h"

CCharacterEditor::CCharacterEditor(EEInterface& oInterface, ICameraManager::TCameraType type) :
CEditor(oInterface, type),
CPlugin(nullptr, ""),
ICharacterEditor(oInterface),
m_bIsLeftMousePressed(false),
m_pWorldEditor(nullptr),
m_pCurrentCharacter(nullptr)
{
	m_pScene = m_oSceneManager.GetScene("Game");
	IEventDispatcher* pEventDispatcher = static_cast<IEventDispatcher*>(oInterface.GetPlugin("EventDispatcher"));
	pEventDispatcher->AbonneToMouseEvent(this, OnMouseEventCallback);
	oInterface.HandlePluginCreation("EditorManager", HandleEditorCreation, this);
}

void CCharacterEditor::HandleEditorCreation(CPlugin* pPlugin, void* pData)
{
	CEditorManager* pEditorManager = (CEditorManager*)pPlugin;
	CCharacterEditor* pCharacterEditor = (CCharacterEditor*)pData;
	if (pCharacterEditor && pEditorManager) {
		pCharacterEditor->m_pWorldEditor = dynamic_cast<CWorldEditor*>(pEditorManager->GetEditor(IEditor::Type::eWorld));
	}
}

void CCharacterEditor::SetEditionMode(bool bEditionMode)
{
	if (m_bEditionMode != bEditionMode) {
		CEditor::SetEditionMode(bEditionMode);
		if (m_bEditionMode) {
			m_pScene->Clear();
			CMatrix m;
			m_pEditorCamera->SetLocalMatrix(m);
			CVector pos(230, 150, -35);
			m_pEditorCamera->SetLocalPosition(pos);
			m_pEditorCamera->Yaw(106);
			m_pEditorCamera->Pitch(-12);
			m_pEditorCamera->Roll(-1);
			m_pEditorCamera->Update();
			IEntity* pLight = m_oEntityManager.CreateLightEntity(CVector(1, 1, 1), IRessource::TLight::OMNI, 0.1f);
			pLight->SetLocalPosition(pos);
			pLight->Link(m_pScene);
			m_oInputManager.ShowMouseCursor(true);
			m_pScene->Update();
		}
	}
}

void CCharacterEditor::Load(string sCharacterId)
{
	
}

void CCharacterEditor::Save()
{
	if (m_bEditionMode && m_pCurrentCharacter) {
		string sId;
		m_pCurrentCharacter->GetEntityName(sId);
		m_oEntityManager.SaveCharacter(sId);
	}
}

void CCharacterEditor::HandleEditorManagerCreation(IEditorManager* pEditor)
{

}

string CCharacterEditor::GetName()
{
	return "CharacterEditor";
}

bool CCharacterEditor::IsEnabled()
{
	return m_bEditionMode;
}

void CCharacterEditor::SpawnEntity(string sCharacterId)
{
	if(!m_bEditionMode)
		SetEditionMode(true);
	m_pCurrentCharacter = dynamic_cast<ICharacter*>(m_oEntityManager.GetEntity(sCharacterId));
	if (!m_pCurrentCharacter) {
		m_pCurrentCharacter = m_oEntityManager.BuildCharacterFromDatabase(sCharacterId, m_pScene);
		InitSpawnedCharacter();
		m_oCameraManager.SetActiveCamera(m_pEditorCamera);
	}
}

void CCharacterEditor::InitSpawnedCharacter()
{
	if (m_pCurrentCharacter) {
		m_pCurrentCharacter->SetWeight(0);
		m_pCurrentCharacter->SetWorldPosition(0, 0, 0);
		m_pCurrentCharacter->RunAction("stand", true);
	}
}

void CCharacterEditor::SetCurrentEditablePlayer(ICharacter* pPlayer)
{
	m_pCurrentCharacter = pPlayer;
	InitSpawnedCharacter();
}

void CCharacterEditor::SetCurrentEditableNPC(ICharacter* pNPCEntity)
{
	m_pCurrentCharacter = pNPCEntity;
	InitSpawnedCharacter();
}

void CCharacterEditor::AddHairs(string sHairsName)
{
	try
	{
		m_pCurrentCharacter->AddHairs(sHairsName);
	}
	catch (CEException& e) {
		m_oConsole.Println(e.what());
	}
}

void CCharacterEditor::WearShoes(string sShoesName)
{
	try
	{
		m_pCurrentCharacter->WearShoes(sShoesName);
	}
	catch (CEException& e) {
		m_oConsole.Println(e.what());
	}
}

void CCharacterEditor::SetTexture(string sTexture)
{
	m_pCurrentCharacter->SetDiffuseTexture(sTexture);
}

void CCharacterEditor::Edit(string id)
{
	SetEditionMode(true);
	SpawnEntity(id);
}

void CCharacterEditor::OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent e, int x, int y)
{
	CCharacterEditor* pEditor = dynamic_cast<CCharacterEditor*>(plugin);
	if (pEditor->m_bEditionMode) {
		if (e == IEventDispatcher::TMouseEvent::T_LBUTTONDOWN) {
			pEditor->m_bIsLeftMousePressed = true;
			pEditor->m_nMousePosX = x;
		}
		else if (e == IEventDispatcher::TMouseEvent::T_LBUTTONUP) {
			pEditor->m_bIsLeftMousePressed = false;
		}
		else if (e == IEventDispatcher::TMouseEvent::T_MOVE) {
			if (pEditor->m_bIsLeftMousePressed) {
				int delta = (x - pEditor->m_nMousePosX) / 5;
				pEditor->m_pCurrentCharacter->Yaw(delta);
				pEditor->m_nMousePosX = x;
			}
		}
	}
}