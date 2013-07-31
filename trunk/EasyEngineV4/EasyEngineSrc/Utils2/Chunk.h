#ifndef CHUNK_H
#define CHUNK_H

#include <map>
#include <string>
#include <vector>


class CChunk
{
	std::map< std::string, void* >		m_MapDataArray;
	std::vector< std::string >			m_vDesc;

public:
										CChunk(void);
	virtual								~CChunk(void);

	void								Add( const void* pData, const std::string& sDesc );
	void*								Get( const std::string& sDesc ) const;/*
	inline template< class T >	T*		Get_Secure( const std::string& sDesc )
	{
		T* pData = dynamic_cast< T* > Get( sName );
		return pData;
	}*/
	std::string							GetDesc( int nNumDesc )const;
	void								Clear();
	size_t								GetSize()const;
};





#endif // CHUNK_H