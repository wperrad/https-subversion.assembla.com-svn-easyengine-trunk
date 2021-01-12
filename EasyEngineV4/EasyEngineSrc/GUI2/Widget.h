#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "../Utils2/Position.h"
#include "../Utils2/Dimension.h"
#include <string>

class CMesh;
class CListener;
class IShaderManager;

class CWidget
{

	CMesh*					_pRect;	
	CListener*				_pListener;
	CPosition				_NextCursorPos;
	bool					_bIsCursorInWidget;
	std::string				_strSkinName;
	CDimension				_Dimension;
	CPosition				_Position;
	int						m_nScreenResWidth;
	int						m_nScreenResHeight;

public:
							CWidget( int nWidth, int nHeight );
	virtual					~CWidget(void);	

	void					SetRect(CMesh* pRect);
	void					Display();
	
	CPosition				GetPosition()const;
	void					GetLogicalPosition( float& x, float& y, int nResWidth, int nResHeight ) const;
	CDimension				GetDimension();
	unsigned int			GetWidgetCount()const;

	void					SetPosition(float fPosX, float fPosY);
	void					SetPosition(CPosition p);
	void					SetY(float fY);
	void					SetListener(CListener* pListener);
	void					UpdateCallback(int nCursorXPos, int nCursorYPos, const unsigned int nButtonState);
	void					SetSkinName( const std::string& szSkinName );
	std::string				GetSkinName();
};




#endif // _WIDGET_H_