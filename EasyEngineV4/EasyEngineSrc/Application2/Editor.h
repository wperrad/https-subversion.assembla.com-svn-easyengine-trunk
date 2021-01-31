#include "EEPlugin.h"

class IInputManager;
class ICameraManager;
class IEventDispatcher;
class IRenderer;
class IEntityManager;

class CEditor : public CPlugin
{
public:

	CEditor(
		IInputManager& oInputManager, 
		ICameraManager& oCameraManager, 
		IEventDispatcher& oEventDispatcher, 
		IRenderer& oRenderer, 
		IEntityManager& oEntityManager);

	void				SetEditionMode(bool bEditionMode);


private:
	static void			OnMouseEventCallback(CPlugin* plugin, IEventDispatcher::TMouseEvent, int x, int y);

	void				RayCast(int x, int y);
	bool				m_bEditionMode;
	IInputManager&		m_oInputManager;
	ICameraManager&		m_oCameraManager;
	IRenderer&			m_oRenderer;
	IEntityManager&		m_oEntityManager;
};