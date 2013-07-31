#ifndef PLUGINCREATION_H
#define PLUGINCREATION_H


template< class T, std::string sFuncName >
T* Create( const T::Desc& oDesc, const std::string& sDllPath )
{
	HMODULE hDll = LoadLibraryA( sDllPath.c_str() );
	if ( !hDll )
	{
		std::string sMessage = sDllPath + " introuvable";
		std::exception e( sMessage.c_str() );
		throw e;
	}
	T* ( *pCreate )( const T::Desc& ) = reinterpret_cast< T* ( * )( const T::Desc& ) > ( GetProcAddress( hDll, sFuncName.c_str() ) );
	return pCreate( oDesc );
}

#endif // PLUGINCREATION_H