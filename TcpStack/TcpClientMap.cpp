

#include "SipPlatformDefine.h"
#include "TcpStack.h"
#include "TcpThread.h"
#include "TcpClientMap.h"
#include "Log.h"
#include "MemoryDebug.h"

CTcpClientIndex::CTcpClientIndex() : m_iThreadIndex(-1), m_iSessionIndex(-1)
{
}

CTcpClientInfo::CTcpClientInfo() : m_bTrying(true)
{
}

CTcpClientMap::CTcpClientMap() : m_pclsStack(NULL)
{
}

CTcpClientMap::~CTcpClientMap()
{
}


bool CTcpClientMap::Create( CTcpStack * pclsStack )
{
	m_pclsStack = pclsStack;

	return true;
}


bool CTcpClientMap::Send( const char * pszIp, int iPort, const char * pszPacket, int iPacketLen, bool bConnectIfNoSession )
{
	bool bRes = false;
	std::string strKey;
	TCP_CLIENT_MAP::iterator itMap;
	CTcpClientIndex clsIndex;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strKey );
	if( itMap != m_clsMap.end() )
	{
		if( itMap->second.m_bTrying )
		{
			// TCP connect 진행 중이면 전송된 패킷을 자료구조에 저장한다.
			std::string * pstrPacket = new std::string();
			if( pstrPacket )
			{
				pstrPacket->append( pszPacket, iPacketLen );
				itMap->second.m_clsSendPacketList.push_back( pstrPacket );
				bRes = true;
			}
		}
		else
		{
			// TCP connected 상태이면 해당 소켓으로 전송하기 위한 정보를 설정한다.
			clsIndex = itMap->second.m_clsIndex;
			bRes = true;
		}
	}
	else if( bConnectIfNoSession )
	{
		std::string * pstrPacket = new std::string();
		if( pstrPacket )
		{
			CTcpClientInfo clsInfo;

			pstrPacket->append( pszPacket, iPacketLen );
			m_clsMap.insert( TCP_CLIENT_MAP::value_type( strKey, clsInfo ) );
			itMap = m_clsMap.find( strKey );
			itMap->second.m_clsSendPacketList.push_back( pstrPacket );

			if( StartTcpClientThread( pszIp, iPort, m_pclsStack ) == false )
			{
				delete pstrPacket;
				m_clsMap.erase( itMap );
			}
			else
			{
				bRes = true;
			}
		}
	}
	m_clsMutex.release();

	if( clsIndex.m_iThreadIndex != -1 && clsIndex.m_iSessionIndex != -1 )
	{
		m_pclsStack->Send( clsIndex.m_iThreadIndex, clsIndex.m_iSessionIndex, pszPacket, iPacketLen );
	}

	return bRes;
}


bool CTcpClientMap::Delete( const char * pszIp, int iPort )
{
	SEND_PACKET_LIST clsSendPacketList;
	SEND_PACKET_LIST::iterator	itList;

	Delete( pszIp, iPort, clsSendPacketList );
	for( itList = clsSendPacketList.begin(); itList != clsSendPacketList.end(); ++itList )
	{
		//m_pclsStack->m_pclsCallBack->SendPacketError( pszIp, iPort, (*itList)->c_str(), (*itList)->length() );
		delete *itList;
	}

	return true;
}


bool CTcpClientMap::DeleteSendPacketList( const char * pszIp, int iPort, SEND_PACKET_LIST & clsList )
{
	bool bRes = false;
	std::string strKey;
	TCP_CLIENT_MAP::iterator itMap;

	clsList.clear();
	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strKey );
	if( itMap != m_clsMap.end() )
	{
		clsList = itMap->second.m_clsSendPacketList;
		itMap->second.m_clsSendPacketList.clear();
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CTcpClientMap::SetConnected( const char * pszIp, int iPort, int iThreadIndex, int iSessionIndex )
{
	bool bRes = false;
	std::string strKey;
	TCP_CLIENT_MAP::iterator itMap;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strKey );
	if( itMap != m_clsMap.end() )
	{
		itMap->second.m_clsIndex.m_iThreadIndex = iThreadIndex;
		itMap->second.m_clsIndex.m_iSessionIndex = iSessionIndex;
		itMap->second.m_bTrying = false;
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CTcpClientMap::Insert( const char * pszIp, int iPort, int iThreadIndex, int iSessionIndex )
{
	bool bRes = false;
	std::string strKey;
	CTcpClientInfo clsInfo;
	TCP_CLIENT_MAP::iterator itMap;

	GetKey( pszIp, iPort, strKey );

	clsInfo.m_bTrying = false;
	clsInfo.m_clsIndex.m_iThreadIndex = iThreadIndex;
	clsInfo.m_clsIndex.m_iSessionIndex = iSessionIndex;

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strKey );
	if( itMap == m_clsMap.end() )
	{
		m_clsMap.insert( TCP_CLIENT_MAP::value_type( strKey, clsInfo ) );
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}


bool CTcpClientMap::Delete( const char * pszIp, int iPort, SEND_PACKET_LIST & clsList )
{
	bool bRes = false;
	std::string strKey;
	TCP_CLIENT_MAP::iterator itMap;

	GetKey( pszIp, iPort, strKey );

	m_clsMutex.acquire();
	itMap = m_clsMap.find( strKey );
	if( itMap != m_clsMap.end() )
	{
		clsList = itMap->second.m_clsSendPacketList;
		bRes = true;
		m_clsMap.erase( itMap );
	}
	m_clsMutex.release();

	return bRes;
}


void CTcpClientMap::GetKey( const char * pszIp, int iPort, std::string & strKey )
{
	char szPort[6];

	snprintf( szPort, sizeof(szPort), "%d", iPort );

	strKey = pszIp;
	strKey.append( ":" );
	strKey.append( szPort );
}
