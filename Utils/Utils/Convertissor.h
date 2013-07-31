#pragma once

#include <string>

class CConvertissor
{
	static CConvertissor*	m_pInstance;
							CConvertissor(void);

public:
	static CConvertissor*	GetInstance();
	~CConvertissor(void);
	void					DoubleToHexString( double d, std::string& sString );
	double					HexStringToDouble( std::string s );
	void					HexStringToBinString( std::string s, std::string& sBin );
};
