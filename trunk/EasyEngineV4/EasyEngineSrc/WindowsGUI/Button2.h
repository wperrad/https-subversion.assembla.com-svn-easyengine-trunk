#pragma once
#include "Control.h"

#include <string>


class CButton2 : public CControl
{
public:
	enum BUTTON_EVENT
	{
		BUTTON_EVENT_PRESS = 0
	};

	typedef void( *BUTTONCALLBACK )( CButton2*, BUTTON_EVENT, void* );

private:
	BUTTONCALLBACK	m_pfnCallback;
	void*	m_pAdditionalArgCallback;
public:

	struct Desc : public CControl::Desc
	{
		std::string m_sCaption;
		Desc( int nWidth, int nHeight, CWidget* pParent, std::string sCaption="" );
		std::string m_sButtonName;
	};

	CButton2( const CButton2::Desc& desc );
	~CButton2(void);
	void		CallCallback( CallbackArgs& args );
	void		SetCallback( BUTTONCALLBACK, void* );
};
