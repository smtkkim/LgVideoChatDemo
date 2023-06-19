

#include "SipPlatformDefine.h"
#include "TcpThreadList.h"
#include "StringUtility.h"
#include "Log.h"
#include <time.h>
#include "TcpStackCallBack.h"
#include "MemoryDebug.h"

CTcpSessionInfo::CTcpSessionInfo() : m_iPort(0), m_hSocket(INVALID_SOCKET)
	, m_psttSsl(NULL)
	, m_iConnectTime(0), m_iRecvTime(0), m_iSendTime(0)
	, m_iThreadIndex(-1), m_iSessionIndex(-1)
	, m_pclsApp(NULL)
{

}

CTcpSessionInfo::~CTcpSessionInfo()
{
	Clear();
}


bool CTcpSessionInfo::Send( const char * pszPacket, int iPacketLen )
{
	bool bRes = false;

	if( CLog::GetLevel() & LOG_NETWORK )
	{
		Log( pszPacket, iPacketLen, true );
	}

	m_clsMutex.acquire();
	if( m_psttSsl )
	{
		if( SSLSend( m_psttSsl, pszPacket, iPacketLen ) == iPacketLen )
		{
			bRes = true;
		}
	}
	else if( m_hSocket != INVALID_SOCKET )
	{
		if( TcpSend( m_hSocket, pszPacket, iPacketLen ) == iPacketLen )
		{
			bRes = true;
		}
	}
	m_clsMutex.release();

	if( bRes && m_iSendTime )
	{
		time( &m_iSendTime );
	}

	return bRes;
}


void CTcpSessionInfo::Log( const char * pszPacket, int iPacketLen, bool bSend )
{
	const char * pszFunction = "TcpSend";

	if( bSend )
	{
		if( m_psttSsl ) pszFunction = "SSLSend";
	}
	else
	{
		if( m_psttSsl )
		{
			pszFunction = "SSLRecv";
		}
		else
		{
			pszFunction = "TcpRecv";
		}
	}

	if( iPacketLen >= 10 && IsPrintString( pszPacket, 10 ) )
	{
		CLog::Print( LOG_NETWORK, "%s(%s:%d) [%.*s] (%d)", pszFunction, m_strIp.c_str(), m_iPort, iPacketLen, pszPacket, iPacketLen );
	}
	else
	{
		char szBuf[250];
		int iBufLen = sizeof(szBuf) - 1;

		memset( szBuf, 0, sizeof(szBuf) );

		iBufLen /= 2;
		if( iBufLen > iPacketLen ) iBufLen = iPacketLen;

		for( int i = 0; i < iBufLen; ++i )
		{
			sprintf( szBuf + i * 2, "%02X", ( pszPacket[i] & 0xFF ) );
		}

		CLog::Print( LOG_NETWORK, "%s(%s:%d) [%s] (%d)", pszFunction, m_strIp.c_str(), m_iPort, szBuf, iPacketLen );
	}
}


void CTcpSessionInfo::Clear()
{
	m_clsMutex.acquire();
	if( m_psttSsl )
	{
		SSLClose( m_psttSsl );
		m_psttSsl = NULL;
	}

	m_hSocket = INVALID_SOCKET;
	m_clsMutex.release();

	m_strIp.clear();
	m_iPort = 0;

	m_iConnectTime = 0;
	m_iRecvTime = 0;

	if( m_pclsApp )
	{
		delete m_pclsApp;
		m_pclsApp = NULL;
	}
}

CTcpSessionList::CTcpSessionList( ) : m_psttPollFd(NULL), m_pclsSession(NULL), m_iPollFdMax(0), m_iPoolFdCount(0)
{
}

CTcpSessionList::~CTcpSessionList()
{
	if( m_psttPollFd )
	{
		free( m_psttPollFd );
		m_psttPollFd = NULL;
	}

	if( m_pclsSession )
	{
		delete [] m_pclsSession;
		m_pclsSession = NULL;
	}
}


bool CTcpSessionList::Init( int iThreadIndex, int iPollFdMax )
{
	m_iPollFdMax = iPollFdMax;

	m_psttPollFd = ( struct pollfd * )malloc( sizeof(struct pollfd) * m_iPollFdMax );
	if( m_psttPollFd == NULL )
	{
		return false;
	}

	m_pclsSession = new CTcpSessionInfo[m_iPollFdMax];
	if( m_pclsSession == NULL )
	{
		free( m_psttPollFd );
		m_psttPollFd = NULL;
		return false;
	}

	for( int i = 0; i < m_iPollFdMax; ++i )
	{
		m_pclsSession[i].m_iThreadIndex = iThreadIndex;
		m_pclsSession[i].m_iSessionIndex = i;
		TcpSetPollIn( m_psttPollFd[i], INVALID_SOCKET );
	}

	return true;
}


bool CTcpSessionList::Insert( Socket hSocket )
{
	if( m_iPoolFdCount >= m_iPollFdMax ) return false;

	TcpSetPollIn( m_psttPollFd[m_iPoolFdCount], hSocket );
	++m_iPoolFdCount;

	return true;
}


int CTcpSessionList::Insert( CTcpComm & clsTcpComm )
{
	if( m_iPoolFdCount >= m_iPollFdMax )
	{
		for( int i = 1; i < m_iPoolFdCount; ++i )
		{
			if( m_psttPollFd[i].fd == INVALID_SOCKET )
			{
				Insert( i, clsTcpComm );
				return i;
			}
		}

		return -1;
	}

	Insert( m_iPoolFdCount, clsTcpComm );
	++m_iPoolFdCount;

	return m_iPoolFdCount - 1;
}


bool CTcpSessionList::Delete( int iIndex )
{
	if( iIndex >= m_iPoolFdCount || iIndex < 0 ) return false;

	CLog::Print( LOG_INFO, "CTcpSessionList::Delete(%d) (%s:%d)", iIndex, m_pclsSession[iIndex].m_strIp.c_str(), m_pclsSession[iIndex].m_iPort );

	m_pclsSession[iIndex].Clear();
	ClearFd( iIndex );

	if( iIndex == (m_iPoolFdCount - 1) )
	{
		for( int i = iIndex; i >= 0; --i )
		{
			if( m_psttPollFd[i].fd != INVALID_SOCKET ) break;
			m_iPoolFdCount = i;
		}
	}

	return true;
}


void CTcpSessionList::DeleteAll( )
{
	for( int i = 0; i < m_iPoolFdCount; ++i )
	{
		m_pclsSession[i].Clear();
		ClearFd( i );
	}

	m_iPoolFdCount = 0;
}


void CTcpSessionList::DeleteTimeout( int iTimeout, int iNoPacketTimeout
	, void (*BeforeDelete)( CTcpSessionList * pclsSessionList, int iIndex, void * pclsArg ), void * pclsArg )
{
	time_t	iTime, iNoPacketTime;

	time( &iTime );
	iNoPacketTime = iTime - iNoPacketTimeout;
	iTime -= iTimeout;

	for( int i = 1; i < m_iPoolFdCount; ++i )
	{
		if( m_pclsSession[i].m_hSocket == INVALID_SOCKET ) continue;

		if( m_pclsSession[i].m_iRecvTime == 0 )
		{
			if( m_pclsSession[i].m_iConnectTime < iNoPacketTime )
			{
				CLog::Print( LOG_DEBUG, "%s (%s,%d) no packet timeout", __FUNCTION__, m_pclsSession[i].m_strIp.c_str(), m_pclsSession[i].m_iPort );
				BeforeDelete( this, i, pclsArg );
				Delete( i );
			}
		}
		else if( m_pclsSession[i].m_iRecvTime < iTime )
		{
			CLog::Print( LOG_DEBUG, "%s (%s,%d) timeout", __FUNCTION__, m_pclsSession[i].m_strIp.c_str(), m_pclsSession[i].m_iPort );
			BeforeDelete( this, i, pclsArg );
			Delete( i );
		}
	}
}


bool CTcpSessionList::Send( int iIndex, const char * pszPacket, int iPacketLen )
{
	return m_pclsSession[iIndex].Send( pszPacket, iPacketLen );
}


bool CTcpSessionList::SendAll( const char * pszPacket, int iPacketLen, ITcpStackCallBack * pclsCallBack )
{
	for( int i = 1; i < m_iPoolFdCount; ++i )
	{
		if( m_pclsSession[i].m_hSocket == INVALID_SOCKET ) continue;

		if( pclsCallBack->IsSendAll( &m_pclsSession[i] ) )
		{
			m_pclsSession[i].Send( pszPacket, iPacketLen );
			pclsCallBack->AfterSendAllPerSession( &m_pclsSession[i], pszPacket, iPacketLen );
		}
	}

	return true;
}


bool CTcpSessionList::SendAllExcept( const char * pszPacket, int iPacketLen, ITcpStackCallBack * pclsCallBack, int iThreadIndex, int iSessionIndex )
{
	for( int i = 1; i < m_iPoolFdCount; ++i )
	{
		if( m_pclsSession[i].m_hSocket == INVALID_SOCKET ) continue;

		if( pclsCallBack->IsSendAll( &m_pclsSession[i] ) )
		{
			if( m_pclsSession[i].m_iThreadIndex == iThreadIndex && m_pclsSession[i].m_iSessionIndex == iSessionIndex ) continue;

			m_pclsSession[i].Send( pszPacket, iPacketLen );
			pclsCallBack->AfterSendAllPerSession( &m_pclsSession[i], pszPacket, iPacketLen );
		}
	}

	return true;
}


void CTcpSessionList::Insert( int iIndex, CTcpComm & clsTcpComm )
{
	time_t	iTime;

	time( &iTime );

	m_pclsSession[iIndex].m_clsMutex.acquire();
	TcpSetPollIn( m_psttPollFd[iIndex], clsTcpComm.m_hSocket );
	m_pclsSession[iIndex].m_strIp = clsTcpComm.m_szIp;
	m_pclsSession[iIndex].m_iPort = clsTcpComm.m_iPort;
	m_pclsSession[iIndex].m_iConnectTime = iTime;
	m_pclsSession[iIndex].m_iRecvTime = iTime;

	if( clsTcpComm.m_bClient )
	{
		m_pclsSession[iIndex].m_iSendTime = iTime;
	}
	else
	{
		m_pclsSession[iIndex].m_iSendTime = 0;
	}

	m_pclsSession[iIndex].m_hSocket = clsTcpComm.m_hSocket;
	m_pclsSession[iIndex].m_psttSsl = clsTcpComm.m_psttSsl;
	m_pclsSession[iIndex].m_clsMutex.release();

	CLog::Print( LOG_INFO, "CTcpSessionList::Insert(%d) (%s:%d) client(%s)"
		, iIndex, m_pclsSession[iIndex].m_strIp.c_str(), m_pclsSession[iIndex].m_iPort, clsTcpComm.m_bClient ? "true" : "false" );
}


void CTcpSessionList::ClearFd( int iIndex )
{
	m_pclsSession[iIndex].m_clsMutex.acquire();
	if( m_psttPollFd[iIndex].fd != INVALID_SOCKET )
	{
		closesocket( m_psttPollFd[iIndex].fd );
		m_psttPollFd[iIndex].fd = INVALID_SOCKET;
	}
	m_psttPollFd[iIndex].events = 0;
	m_psttPollFd[iIndex].revents = 0;
	m_pclsSession[iIndex].m_clsMutex.release();
}
