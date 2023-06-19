

#include "TcpStack.h"
#include "TcpThread.h"
#include "ServerUtility.h"
#include "TimeUtility.h"
#include "Log.h"
#include "MemoryDebug.h"

CTcpStack::CTcpStack() : m_pclsCallBack(NULL), m_hTcpListenFd(INVALID_SOCKET), m_bStop(false)
{
	InitNetwork();
}

CTcpStack::~CTcpStack()
{
}


bool CTcpStack::Start( CTcpStackSetup * pclsSetup, ITcpStackCallBack * pclsCallBack )
{
	if( pclsSetup == NULL || pclsCallBack == NULL ) return false;

	m_clsSetup = *pclsSetup;
	m_pclsCallBack = pclsCallBack;
	m_clsClientMap.Create( this );

	if( pclsSetup->m_iListenPort > 0 )
	{
		if( pclsSetup->m_bUseTls )
		{
			if( SSLServerStart( pclsSetup->m_strCertFile.c_str(), pclsSetup->m_strCipherList.c_str() ) == false )
			{
				CLog::Print( LOG_ERROR, "SSLServerStart error" );
				return false;
			}

			if( pclsSetup->m_bUseHttp2 )
			{
				SSLServerSetHttp2();
			}
		}

		if( pclsSetup->m_strListenIp.empty() )
		{
			m_hTcpListenFd = TcpListen( pclsSetup->m_iListenPort, 255 );
		}
		else
		{
			m_hTcpListenFd = TcpListen( pclsSetup->m_iListenPort, 255, pclsSetup->m_strListenIp.c_str() );
		}

		if( m_hTcpListenFd == INVALID_SOCKET )
		{
			CLog::Print( LOG_ERROR, "TcpListen(%d) error(%d)", pclsSetup->m_iListenPort, GetError() );
			return false;
		}

		if( StartThread( "TcpListenThread", TcpListenThread, this ) == false )
		{
			Stop();
			return false;
		}
	}
	else if( pclsSetup->m_bUseTls )
	{
		SSLClientStart();
	}

	if( m_clsSetup.m_bUseThreadPipe )
	{
		if( m_clsThreadList.Create( this ) == false )
		{
			Stop();
			return false;
		}
	}

	return true;
}


bool CTcpStack::Stop( )
{
	m_bStop = true;

	if( m_clsSetup.m_bUseThreadPipe )
	{
		m_clsThreadList.Destroy();
	}
	else
	{
		// TCP 쓰레드가 종료될 때까지 최대 2초 대기한다.
		for( int i = 0; i < 100; ++i )
		{
			if( m_clsSessionMap.GetCount() == 0 ) break;
			MiliSleep( 20 );
		}
	}

	// TCP listen 쓰레드가 종료될 때까지 최대 2초 대기한다.
	for( int i = 0; i < 100; ++i )
	{
		if( IsTcpListenThreadRun() == false )	break;
		MiliSleep( 20 );
	}

	if( m_hTcpListenFd != INVALID_SOCKET )
	{
		closesocket( m_hTcpListenFd );
		m_hTcpListenFd = INVALID_SOCKET;
	}

	if( m_clsSetup.m_bUseTls )
	{
		if( m_clsSetup.m_iListenPort > 0 )
		{
			SSLServerStop();
		}
		else
		{
			SSLClientStop();
		}
	}

	return true;
}


bool CTcpStack::Send( const char * pszIp, int iPort, const char * pszPacket, int iPacketLen, bool bConnectIfNoSession )
{
	if( m_clsSetup.m_bUseThreadPipe )
	{
		return m_clsClientMap.Send( pszIp, iPort, pszPacket, iPacketLen, bConnectIfNoSession );
	}

	if( m_clsSessionMap.Send( pszIp, iPort, pszPacket, iPacketLen ) == false )
	{
		// m_clsSessionMap 에 존재하지 않으므로 TCP connect 하고 TcpNoPipeThread 를 생성하는 과정을 실행한다.
		return m_clsClientMap.Send( pszIp, iPort, pszPacket, iPacketLen, bConnectIfNoSession );
	}

	return true;
}


bool CTcpStack::Send( int iThreadIndex, int iSessionIndex, const char * pszPacket, int iPacketLen )
{
	if( m_clsSetup.m_bUseThreadPipe )
	{
		return m_clsThreadList.Send( iThreadIndex, iSessionIndex, pszPacket, iPacketLen );
	}

	return m_clsSessionMap.Send( iThreadIndex, pszPacket, iPacketLen );
}


bool CTcpStack::SendAll( const char * pszPacket, int iPacketLen )
{
	if( m_clsSetup.m_bUseThreadPipe )
	{
		return m_clsThreadList.SendAll( pszPacket, iPacketLen, m_pclsCallBack );
	}

	return m_clsSessionMap.SendAll( pszPacket, iPacketLen, m_pclsCallBack );
}


bool CTcpStack::SendAllExcept( const char * pszPacket, int iPacketLen, int iThreadIndex, int iSessionIndex )
{
	if( m_clsSetup.m_bUseThreadPipe )
	{
		return m_clsThreadList.SendAllExcept( pszPacket, iPacketLen, m_pclsCallBack, iThreadIndex, iSessionIndex );
	}

	return m_clsSessionMap.SendAllExcept( pszPacket, iPacketLen, m_pclsCallBack, iThreadIndex, iSessionIndex );
}
