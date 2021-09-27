#include "Editor.h"
#include "Interface.h"
#include "EditorManager.h"
#include "ICameraManager.h"
#include "IEntity.h"
#include "IInputManager.h"
#include "ICollisionManager.h"
#include "IGeometry.h"
#include "IConsole.h"

CEditor::CEditor(EEInterface& oInterface) : 
m_oInterface(oInterface),
m_pEditorManager(nullptr),
m_oCameraManager(*static_cast<ICameraManager*>(oInterface.GetPlugin("CameraManager"))),
m_oEntityManager(*static_cast<IEntityManager*>(oInterface.GetPlugin("EntityManager"))),
m_oInputManager(*static_cast<IInputManager*>(oInterface.GetPlugin("InputManager"))),
m_oSceneManager(static_cast<ISceneManager&>(*oInterface.GetPlugin("SceneManager"))),
m_oRenderer(*static_cast<IRenderer*>(oInterface.GetPlugin("Renderer"))),
m_oCollisionManager(*static_cast<ICollisionManager*>(oInterface.GetPlugin("CollisionManager"))),
m_oGeometryManager(*static_cast<IGeometryManager*>(oInterface.GetPlugin("GeometryManager"))),
m_oConsole(*static_cast<IConsole*>(oInterface.GetPlugin("Console")))
{
	oInterface.HandlePluginCreation("EditorManager", HandleEditorManagerCreation, this);
}

void CEditor::HandleEditorManagerCreation(CPlugin* plugin, void* pData)
{
	if (pData) {
		CEditor* pEditor = static_cast<CEditor*>(pData);
		pEditor->m_pEditorManager = static_cast<IEditorManager*>(plugin);
	}
}