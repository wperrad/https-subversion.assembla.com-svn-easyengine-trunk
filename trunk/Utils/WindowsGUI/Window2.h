#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#include <windows.h>
#include <string>
#include <map>
#include "Widget.h"
#include <commctrl.h>

class CMenu2;

typedef LRESULT (* WINDOW2CALLBACK)( CWidget*, UINT, WPARAM, LPARAM );

class CWindow2 : public CWidget
{
public:
	struct Desc : public CWidget::Desc
	{
		friend CWindow2;
	protected:
		std::string		m_sClassName;
	public:
		Desc( int nWidth, int nHeight, std::string sClassName, WINDOW2CALLBACK pfnCallback );
		WINDOW2CALLBACK	m_pfnWindowCallback;
		void(*m_pfnOnUpdate)( const CWindow2* );
		int				m_ColorBrush;
		DWORD			m_dwClassStyle;
		DWORD			m_dwExStyle;
		DWORD			m_dwStyle;
		bool			m_bCentered;
		CMenu2*			m_pMenu;
		bool			m_bFullscreen;
		int				m_nBits;
	};

private:
	
	bool									m_bFullscreen;
	WINDOW2CALLBACK							m_pfnWindowCallback;
	void									(*m_pfnOnUpdate)(const CWindow2*);
	std::string								m_sClassName;
	CMenu2*									m_pMenu;	
	static std::map< int, std::string >		s_mErrorString;
	static									std::map< HWND, CWindow2* >	s_mWindow;
	bool									TestDesc( const Desc& )const;
	int										m_nBits;

	static CWindow2*						s_pThis;
	static void								InitErrorString();
	static									LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

protected:
	static void								GetStringByErrorCode( int nErrorCode, std::string& sString );
	static CWindow2*						GetWindow( HWND hWnd );	
	bool									m_bDestroy;

public:

											CWindow2( const CWindow2::Desc& );
	virtual 								~CWindow2();
	int										GetBits();
	void									Show()const;
	void									ShowModal()const;
	void									ShowModalWithoutUpdate()const;
	void									Close()const;
	void									GetClientDimension( int& nWidth, int& nHeight )const;
	void									CallCallback( CallbackArgs& args );
	bool									IsFullscreenMode();
	void									Setfocus();
	void									SetForeground();
};

#endif // BASE_WINDOW_H