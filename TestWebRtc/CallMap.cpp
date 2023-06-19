

#include "CallMap.h"

CCallMap gclsCallMap;

CCallMap::CCallMap()
{
}

CCallMap::~CCallMap()
{
}


bool CCallMap::Insert( const char * pszCaller, const char * pszCallee )
{
	bool bRes = false;
	CALL_MAP::iterator itMap;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszCaller );
	if( itMap == m_clsMap.end() )
	{
		itMap = m_clsMap.find( pszCallee );
		if( itMap == m_clsMap.end() )
		{
			m_clsMap.insert( CALL_MAP::value_type( pszCaller, pszCallee ) );
			m_clsMap.insert( CALL_MAP::value_type( pszCallee, pszCaller ) );
			bRes = true;
		}
	}
	m_clsMutex.release();

	return bRes;
}


bool CCallMap::Select( const char * pszUserId, std::string & strOtherId )
{
	bool bRes = false;
	CALL_MAP::iterator itMap;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszUserId );
	if( itMap != m_clsMap.end() )
	{
		strOtherId = itMap->second;
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CCallMap::Delete( const char * pszUserId )
{
	bool bRes = false;
	CALL_MAP::iterator itMap;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszUserId );
	if( itMap != m_clsMap.end() )
	{
		std::string strOther = itMap->second;
		m_clsMap.erase( itMap );

		itMap = m_clsMap.find( strOther );
		if( itMap != m_clsMap.end() )
		{
			m_clsMap.erase( itMap );
		}
	}
	m_clsMutex.release();

	return bRes;
}
