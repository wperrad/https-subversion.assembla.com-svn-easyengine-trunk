#ifndef WIDGET_H
#define WIDGET_H

#include "Container.h"
#include "IInputManager.h"
#include "../Utils2/Dimension.h"
#include "../Utils2/Position.h"

class CMesh;
class CListener;
class IShaderManager;
class IRessourceManager;
class IRessource;
class IShader;
class IMesh;

class CGUIWidget
{

	IMesh*					m_pMesh;
	CListener*				_pListener;
	bool					_bIsCursorInWidget;
	std::string				_strSkinName;
	static int				s_nScreenResWidth;
	static int				s_nScreenResHeight;
	static IShader*			s_pShader;

protected:
	CPosition				_NextCursorPos;
	CGUIWidget*				m_pParent;

public:

	CDimension				_Dimension;
	CPosition				_Position;

							CGUIWidget( int nWidth, int nHeight );
	virtual					~CGUIWidget(void);

	bool					operator==( const CGUIWidget& w );

	void					SetRect( IRessource* pMesh );
	virtual void			Display();
	
	CPosition				GetPosition()const;
	void					GetLogicalPosition( float& x, float& y, int nResWidth, int nResHeight ) const;
	void					GetLogicalDimension( float& x, float& y, int nResWidth, int nResHeight ) const;
	CDimension				GetDimension() const;
	unsigned int			GetWidgetCount()const;

	virtual void			SetPosition(float fPosX, float fPosY);
	void					SetPosition(CPosition p);
	void					SetY(float fY);
	void					SetListener(CListener* pListener);
	void					UpdateCallback(int nCursorXPos, int nCursorYPos, IInputManager::TMouseButtonState eButtonState);
	void					SetSkinName( const std::string& szSkinName );
	std::string				GetSkinName();
	virtual void			SetParent(CGUIWidget* parent);

	static void				Init( int nResX, int nResY, IShader* pShader );
};




#endif