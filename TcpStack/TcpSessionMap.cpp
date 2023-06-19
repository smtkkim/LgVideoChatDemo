

#include "SipPlatformDefine.h"
#include "TcpSessionMap.h"
#include "TcpStackCallBack.h"
#include "MemoryDebug.h"

CTcpSessionMap::CTcpSessionMap() : m_iThreadIndex(0)
{
}

CTcpSessionMap::~CTcpSessionMap()
{
}


bool CTcpSessionMap::Insert( const char * pszIp, int iPort, CTcpSessionInfo * pclsSessionInfo )
{
	TCP_SESSION_MAP::iterator itTSM;
	std::string strKey;
	bool bRes = false;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itTSM = m_clsMap.find( strKey );
	if( itTSM == m_clsMap.end() )
	{
		pclsSessionInfo->m_iThreadIndex = GetThreadIndex();
		m_clsMap.insert( TCP_SESSION_MAP::value_type( strKey, pclsSessionInfo ) );
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CTcpSessionMap::Delete( const char * pszIp, int iPort )
{
	TCP_SESSION_MAP::iterator itTSM;
	std::string strKey;
	bool bRes = false;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itTSM = m_clsMap.find( strKey );
	if( itTSM != m_clsMap.end() )
	{
		// // thread 의 스택 변수의 포인터가 second 이기 때문에 delete 하면 안 된다.
		// delete itTSM->second;
		m_clsMap.erase( itTSM );

		int iMaxIndex = 0;

		for( itTSM = m_clsMap.begin(); itTSM != m_clsMap.end(); ++itTSM )
		{
			if( itTSM->second->m_iThreadIndex > iMaxIndex )
			{
				iMaxIndex = itTSM->second->m_iThreadIndex;
			}
		}

		m_iThreadIndex = iMaxIndex;
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


int CTcpSessionMap::GetCount( )
{
	int iCount;

	m_clsMutex.acquire();
	iCount = (int)m_clsMap.size();
	m_clsMutex.release();

	return iCount;
}


bool CTcpSessionMap::Send( const char * pszIp, int iPort, const char * pszPacket, int iPacketLen )
{
	TCP_SESSION_MAP::iterator itTSM;
	std::string strKey;
	bool bRes = false;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itTSM = m_clsMap.find( strKey );
	if( itTSM != m_clsMap.end() )
	{
		bRes = itTSM->second->Send( pszPacket, iPacketLen );
	}
	m_clsMutex.release();

	return bRes;
}


bool CTcpSessionMap::Send( int iThreadIndex, const char * pszPacket, int iPacketLen )
{
	TCP_SESSION_MAP::iterator itTSM;
	bool bRes = false;

	m_clsMutex.acquire();
	for( itTSM = m_clsMap.begin(); itTSM != m_clsMap.end(); ++itTSM )
	{
		if( itTSM->second->m_iThreadIndex == iThreadIndex )
		{
			bRes = itTSM->second->Send( pszPacket, iPacketLen );
			break;
		}
	}
	m_clsMutex.release();

	return bRes;
}


bool CTcpSessionMap::SendAll( const char * pszPacket, int iPacketLen, ITcpStackCallBack * pclsCallBack )
{
	TCP_SESSION_MAP::iterator itTSM;

	m_clsMutex.acquire();
	for( itTSM = m_clsMap.begin(); itTSM != m_clsMap.end(); ++itTSM )
	{
		if( pclsCallBack->IsSendAll( itTSM->second ) )
		{
			itTSM->second->Send( pszPacket, iPacketLen );
			pclsCallBack->AfterSendAllPerSession( itTSM->second, pszPacket, iPacketLen );
		}
	}
	m_clsMutex.release();

	return true;
}


bool CTcpSessionMap::SendAllExcept( const char * pszPacket, int iPacketLen, ITcpStackCallBack * pclsCallBack, int iThreadIndex, int iSessionIndex )
{
	TCP_SESSION_MAP::iterator itTSM;

	m_clsMutex.acquire();
	for( itTSM = m_clsMap.begin(); itTSM != m_clsMap.end(); ++itTSM )
	{
		if( pclsCallBack->IsSendAll( itTSM->second ) )
		{
			if( itTSM->second->m_iThreadIndex == iThreadIndex && itTSM->second->m_iSessionIndex == iSessionIndex ) continue;

			itTSM->second->Send( pszPacket, iPacketLen );
			pclsCallBack->AfterSendAllPerSession( itTSM->second, pszPacket, iPacketLen );
		}
	}
	m_clsMutex.release();

	return true;
}


void CTcpSessionMap::GetKey( const char * pszIp, int iPort, std::string & strKey )
{
	char szPort[6];

	snprintf( szPort, sizeof(szPort), "%d", iPort );

	strKey = pszIp;
	strKey.append( ":" );
	strKey.append( szPort );
}


int CTcpSessionMap::GetThreadIndex()
{
	++m_iThreadIndex;
	if( m_iThreadIndex > 2000000000 )
	{
		m_iThreadIndex = 1;
	}

	while( SelectThreadIndex( m_iThreadIndex ) )
	{
		++m_iThreadIndex;
		if( m_iThreadIndex > 2000000000 )
		{
			m_iThreadIndex = 1;
		}
	}

	return m_iThreadIndex;
}


bool CTcpSessionMap::SelectThreadIndex( int iThreadIndex )
{
	TCP_SESSION_MAP::iterator itTSM;

	for( itTSM = m_clsMap.begin(); itTSM != m_clsMap.end(); ++itTSM )
	{
		if( itTSM->second->m_iThreadIndex == iThreadIndex )
		{
			return true;
		}
	}

	return false;
}
