

#include "StringMap.h"
#include <string.h>
#include "MemoryDebug.h"

CStringMap::CStringMap()
{
}

CStringMap::~CStringMap()
{
}


bool CStringMap::Insert( const char * pszKey, const char * pszValue )
{
	STRING_MAP::iterator	itMap;
	bool	bRes = false;

	if( pszKey == NULL ) return false;
	if( pszValue == NULL ) pszValue = "";

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszKey );
	if( itMap == m_clsMap.end() )
	{
		m_clsMap.insert( STRING_MAP::value_type( pszKey, pszValue ) );
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CStringMap::Update( const char * pszKey, const char * pszValue )
{
	STRING_MAP::iterator	itMap;
	bool	bRes = false;

	if( pszKey == NULL ) return false;
	if( pszValue == NULL ) pszValue = "";

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszKey );
	if( itMap != m_clsMap.end() )
	{
		itMap->second = pszValue;
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CStringMap::Select( const char * pszKey )
{
	STRING_MAP::iterator	itMap;
	bool	bRes = false;

	if( pszKey == NULL ) return false;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszKey );
	if( itMap != m_clsMap.end() )
	{
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CStringMap::Select( const char * pszKey, std::string & strValue )
{
	STRING_MAP::iterator	itMap;
	bool	bRes = false;

	strValue.clear();

	if( pszKey == NULL ) return false;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszKey );
	if( itMap != m_clsMap.end() )
	{
		strValue = itMap->second;
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CStringMap::SelectWithValue( const char * pszValue, STRING_MAP & clsMap )
{
	STRING_MAP::iterator	itMap;
	bool	bRes = false;

	clsMap.clear();
	
	if( pszValue == NULL ) return false;

	m_clsMutex.acquire();
	for( itMap = m_clsMap.begin(); itMap != m_clsMap.end(); ++itMap )
	{
		if( !strcmp( pszValue, itMap->second.c_str() ) )
		{
			clsMap.insert( STRING_MAP::value_type( itMap->first, itMap->second ) );
			bRes = true;
		}
	}
	m_clsMutex.release();

	return bRes;
}


bool CStringMap::Delete( const char * pszKey )
{
	STRING_MAP::iterator	itMap;
	bool	bRes = false;

	if( pszKey == NULL ) return false;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszKey );
	if( itMap != m_clsMap.end() )
	{
		m_clsMap.erase( itMap );
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


int CStringMap::GetCount( )
{
	int iCount;

	m_clsMutex.acquire();
	iCount = (int)m_clsMap.size();
	m_clsMutex.release();

	return iCount;
}


void CStringMap::DeleteAll( )
{
	m_clsMutex.acquire();
	m_clsMap.clear();
	m_clsMutex.release();
}
