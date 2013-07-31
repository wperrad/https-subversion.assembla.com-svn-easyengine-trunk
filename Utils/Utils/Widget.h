#ifndef WIDGET_H
#define WIDGET_H

#include <windows.h>
#include <vector>

class CMenu2;

class CWidget
{
	HWND						m_hWnd;
	std::vector< CWidget* >		m_vChild;
	CWidget*					m_pParent;

protected:
	
	void						CreateWnd( DWORD dwExStyle, std::string sClassName, std::string sWindowName, 
											DWORD dwStyle, int nPosX, int nPosY, int nWidth, int nHeight, const CMenu2* pMenu );
public:
	struct Desc
	{
		int 			m_nWidth;
		int 			m_nHeight;
		int 			m_nPosX;
		int 			m_nPosY;
		CWidget*		m_pParent;
		Desc( int nWidth, int nHeight, CWidget* pParent );
	};

	struct CallbackArgs
	{
		CWidget* m_pParent;
		UINT m_msg;
		WPARAM m_wParam;
		LPARAM m_lParam;
		CallbackArgs();
		CallbackArgs( CWidget*, UINT, WPARAM, LPARAM );
	};

								CWidget( const Desc& desc );
	virtual						~CWidget();
	
	void						GetPosition( int& x, int& y )const;
	void						GetClientPosition( int& x, int& y )const;
	void						GetDimension( int& nWidth, int& nHeight )const;
	void						GetClientDimension( int& nWidth, int& nHeight )const;
	int							GetChildCount()const;
	CWidget*					GetChild( int iIndex )const;
	HDC							GetDeviceContext()const;

	void						SetPosition( int x, int y );
	void						SetCaption( std::string sCaption );
	
	void						AddChild( CWidget* pChild );
	virtual void				CallCallback( CallbackArgs& args ) = 0;
	void						ForcePaint();
	void						SendMessage2( CallbackArgs );
	HWND						GetHandle()const;
};

#endif // WIDGET_H