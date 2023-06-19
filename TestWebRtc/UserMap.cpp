

#include "SipPlatformDefine.h"
#include "UserMap.h"

CUserMap gclsUserMap;

CUserMap::CUserMap()
{
}

CUserMap::~CUserMap()
{
}


bool CUserMap::Insert( const char * pszUserId, const char * pszIp, int iPort )
{
	bool bRes = false;
	std::string strKey;
	USER_MAP::iterator itMap;
	USER_KEY_MAP::iterator itKeyMap;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itKeyMap = m_clsKeyMap.find( strKey );
	if( itKeyMap == m_clsKeyMap.end() )
	{
		itMap = m_clsMap.find( pszUserId );
		if( itMap == m_clsMap.end() )
		{
			CUserInfo clsInfo;

			clsInfo.m_strIp = pszIp;
			clsInfo.m_iPort = iPort;

			m_clsMap.insert( USER_MAP::value_type( pszUserId, clsInfo ) );
			m_clsKeyMap.insert( USER_KEY_MAP::value_type( strKey, pszUserId ) );

			bRes = true;
		}
	}
	m_clsMutex.release();

	return bRes;
}


bool CUserMap::Select( const char * pszUserId, CUserInfo & clsUserInfo )
{
	bool bRes = false;
	USER_MAP::iterator itMap;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( pszUserId );
	if( itMap != m_clsMap.end() )
	{
		clsUserInfo = itMap->second;

		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CUserMap::SelectUserId( const char * pszIp, int iPort, std::string & strUserId )
{
	bool bRes = false;
	std::string strKey;
	USER_KEY_MAP::iterator itKeyMap;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itKeyMap = m_clsKeyMap.find( strKey );
	if( itKeyMap != m_clsKeyMap.end() )
	{
		strUserId = itKeyMap->second;
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CUserMap::Delete( const char * pszIp, int iPort, std::string & strUserId )
{
	bool bRes = false;
	std::string strKey;
	USER_MAP::iterator itMap;
	USER_KEY_MAP::iterator itKeyMap;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itKeyMap = m_clsKeyMap.find( strKey );
	if( itKeyMap != m_clsKeyMap.end() )
	{
		strUserId = itKeyMap->second;

		itMap = m_clsMap.find( itKeyMap->second );
		if( itMap != m_clsMap.end() )
		{
			m_clsMap.erase( itMap );
		}

		m_clsKeyMap.erase( itKeyMap );
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


void CUserMap::GetKey( const char * pszIp, int iPort, std::string & strKey )
{
	char szPort[11];

	snprintf( szPort, sizeof(szPort), "%d", iPort );

	strKey = pszIp;
	strKey.append( "_" );
	strKey.append( szPort );
}


int CUserMap::GetSize()
{
	int size;

	m_clsMutex.acquire();

	size = (int)m_clsKeyMap.size();

	m_clsMutex.release();

	return size;
}


bool CUserMap::GetAllUserId( std::string& id )
{
	m_clsMutex.acquire();

	for (auto iter:m_clsKeyMap)
	{
		id += iter.second;	// contact list 를 보여줄 id
		id += "|";
	}
	id.pop_back();

	m_clsMutex.release();

	return 0;
}


