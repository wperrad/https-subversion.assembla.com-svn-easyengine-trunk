#pragma once

class IEntityManager;
class ISceneManager;
class IInputManager;
class ICameraManager;
class IConsole;
class ICollisionManager;
class IGeometryManager;
class EEInterface;
class CPlugin;
class IEditorManager;
class IRenderer;

class CEditor
{
public:
	CEditor(EEInterface& oInterface);

	

protected:
	static void HandleEditorManagerCreation(CPlugin* plugin, void* pData);

	IEditorManager*			m_pEditorManager;
	IEntityManager&			m_oEntityManager;
	ISceneManager&			m_oSceneManager;
	EEInterface&			m_oInterface;
	IInputManager&			m_oInputManager;
	IRenderer&				m_oRenderer;
	ICameraManager&			m_oCameraManager;
	ICollisionManager&		m_oCollisionManager;
	IGeometryManager&		m_oGeometryManager;
	IConsole&				m_oConsole;

};