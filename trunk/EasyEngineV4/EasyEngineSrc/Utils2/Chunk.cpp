#include "chunk.h"

using namespace std;

CChunk::CChunk(void)
{
}

CChunk::~CChunk(void)
{
	
}

void CChunk::Add( const void* pData, const string& sName )
{	
	if ( m_MapDataArray.count( sName ) > 0 )
	{
		string sMessage = string( "CChunk::Add() : \"" ) + sName + "\" data already added";
		exception e( sMessage.c_str() );
	}
	m_MapDataArray.insert( map< string, void* >::value_type( sName, (void*)pData ) );
	m_vDesc.push_back( sName );
}

void* CChunk::Get( const string& sName ) const
{
	void* pRet = NULL;
	if (m_MapDataArray.count(sName) > 0)	
	{
		map< string,void* >::const_iterator itData = m_MapDataArray.find(sName);		
		pRet = itData->second;
	}
	return pRet;	
}


string CChunk::GetDesc(int nNumDesc)const
{
	return m_vDesc[nNumDesc];
}


void CChunk::Clear()
{
	m_MapDataArray.clear();
	m_vDesc.clear();
}


size_t CChunk::GetSize()const
{
	return m_MapDataArray.size();
}