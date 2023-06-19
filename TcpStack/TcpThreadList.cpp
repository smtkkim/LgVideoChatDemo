

#include "SipPlatformDefine.h"
#include "TcpStack.h"
#include "TcpThread.h"
#include "ServerUtility.h"
#include "TimeUtility.h"
#include "Log.h"
#include "MemoryDebug.h"

CTcpThreadInfo::CTcpThreadInfo() : m_iIndex(0), m_hSend(INVALID_SOCKET), m_hRecv(INVALID_SOCKET)
{

}

CTcpThreadInfo::~CTcpThreadInfo()
{
	Close();
}


void CTcpThreadInfo::Close()
{
	if( m_hSend != INVALID_SOCKET )
	{
		closesocket( m_hSend );
		m_hSend = INVALID_SOCKET;
	}

	if( m_hRecv != INVALID_SOCKET )
	{
		closesocket( m_hRecv );
		m_hRecv = INVALID_SOCKET;
	}
}

CTcpThreadList::CTcpThreadList() : m_iMaxSocketPerThread(0), m_iThreadIndex(0), m_pclsStack(NULL)
{
}

CTcpThreadList::~CTcpThreadList()
{
}


bool CTcpThreadList::Create( CTcpStack * pclsStack )
{
	if( pclsStack->m_clsSetup.m_iMaxSocketPerThread <= 0 )
	{
		CLog::Print( LOG_ERROR, "%s m_iMaxSocketPerThread(%d) is invalid", __FUNCTION__, pclsStack->m_clsSetup.m_iMaxSocketPerThread );
		return false;
	}

	// pipe 연산을 수행해야 하므로 1개 추가한다.
	m_iMaxSocketPerThread = pclsStack->m_clsSetup.m_iMaxSocketPerThread + 1;
	m_pclsStack = pclsStack;

	bool bError = false;

	m_clsMutex.acquire();
	for( int i = 0; i < pclsStack->m_clsSetup.m_iThreadInitCount; ++i )
	{
		if( AddThread() == false )
		{
			bError = true;
			break;
		}
	}
	m_clsMutex.release();

	if( bError )
	{
		Destroy();
		return false;
	}

	return true;
}


void CTcpThreadList::Destroy()
{
	THREAD_LIST::iterator	itTL;
	CTcpComm		clsTcpComm;

	SendCommandAll( (char *)&clsTcpComm, sizeof(clsTcpComm) );

	m_clsMutex.acquire();
	m_clsList.clear();
	m_clsMutex.release();
}


bool CTcpThreadList::SendCommand( const char * pszData, int iDataLen )
{
	bool	bRes = false, bFound = false;
	THREAD_LIST::iterator	itTL;
	int iMinCount = 2000000000;

	m_clsMutex.acquire();

	// 소켓을 최소 사용하는 쓰레드를 검색한다.
	for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
	{
		if( iMinCount > (*itTL)->m_clsSessionList.m_iPoolFdCount )
		{
			iMinCount = (*itTL)->m_clsSessionList.m_iPoolFdCount;
			if( iMinCount == 0 ) break;
		}
	}

	if( iMinCount < m_iMaxSocketPerThread )
	{
		for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
		{
			if( iMinCount == (*itTL)->m_clsSessionList.m_iPoolFdCount )
			{
				bRes = _SendCommand( (*itTL)->m_hSend, pszData, iDataLen );
				bFound = true;
				break;
			}
		}
	}

	if( bFound == false )
	{
		if( AddThread() )
		{
			bRes = _SendCommand( m_clsList[m_clsList.size()-1]->m_hSend, pszData, iDataLen );
		}
	}
	m_clsMutex.release();

	return bRes;
}


bool CTcpThreadList::SendCommand( const char * pszData, int iDataLen, int iThreadIndex )
{
	bool	bRes = false;
	THREAD_LIST::iterator	itTL;

	m_clsMutex.acquire();
	for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
	{
		if( (*itTL)->m_iIndex == iThreadIndex )
		{
			bRes = _SendCommand( (*itTL)->m_hSend, pszData, iDataLen );
			break;
		}
	}
	m_clsMutex.release();

	return bRes;
}


void CTcpThreadList::SendCommandAll( const char * pszData, int iDataLen )
{
	THREAD_LIST::iterator	itTL;

	m_clsMutex.acquire();
	for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
	{
		_SendCommand( (*itTL)->m_hSend, pszData, iDataLen );
	}
	m_clsMutex.release();
}


int CTcpThreadList::RecvCommand( Socket hSocket, char * pszData, int iDataSize )
{
	int	n;

#ifdef WIN32
	n = recv( hSocket, pszData, iDataSize, 0 );
#else
	n = read( hSocket, pszData, iDataSize );
#endif
	
	return n;
}


bool CTcpThreadList::Send( int iThreadIndex, int iSessionIndex, const char * pszPacket, int iPacketLen )
{
	bool bRes = false;

	m_clsMutex.acquire();
	int iCount = (int)m_clsList.size();
	for( int i = 0; i < iCount; ++i )
	{
		if( m_clsList[i]->m_iIndex == iThreadIndex )
		{
			bRes = m_clsList[i]->m_clsSessionList.Send( iSessionIndex, pszPacket, iPacketLen );
			break;
		}
	}
	m_clsMutex.release();

	return bRes;
}


bool CTcpThreadList::SendAll( const char * pszPacket, int iPacketLen, ITcpStackCallBack * pclsCallBack )
{
	THREAD_LIST::iterator	itTL;

	m_clsMutex.acquire();
	for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
	{
		(*itTL)->m_clsSessionList.SendAll( pszPacket, iPacketLen, pclsCallBack );
	}
	m_clsMutex.release();

	return true;
}


bool CTcpThreadList::SendAllExcept( const char * pszPacket, int iPacketLen, ITcpStackCallBack * pclsCallBack, int iThreadIndex, int iSessionIndex )
{
	THREAD_LIST::iterator	itTL;

	m_clsMutex.acquire();
	for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
	{
		(*itTL)->m_clsSessionList.SendAllExcept( pszPacket, iPacketLen, pclsCallBack, iThreadIndex, iSessionIndex );
	}
	m_clsMutex.release();

	return true;
}

typedef std::list< int > THREAD_INDEX_LIST;


void CTcpThreadList::DeleteNoUseThread()
{
	THREAD_LIST::iterator	itTL;
	THREAD_INDEX_LIST clsDeleteList;
	THREAD_INDEX_LIST::iterator itTIL;
	int iUseCount = 0;

	m_clsMutex.acquire();
	for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
	{
		if( (*itTL)->m_clsSessionList.m_iPoolFdCount == 1 )
		{
			clsDeleteList.push_back( (*itTL)->m_iIndex );
		}
		else
		{
			++iUseCount;
		}
	}
	m_clsMutex.release();

	if( clsDeleteList.size() > 0 )
	{
		// 초기 실행 쓰레드 개수만큼은 유지되어야 한다.
		if( m_pclsStack->m_clsSetup.m_iThreadInitCount > iUseCount )
		{
			int iDeleteCount = m_pclsStack->m_clsSetup.m_iThreadInitCount - iUseCount;

			if( iDeleteCount >= (int)clsDeleteList.size() )
			{
				clsDeleteList.clear();
			}
			else
			{
				THREAD_INDEX_LIST::iterator itNext;

				for( itTIL = clsDeleteList.begin(); itTIL != clsDeleteList.end(); )
				{
					itNext = itTIL;
					++itNext;
					clsDeleteList.erase( itTIL );
					itTIL = itNext;

					--iDeleteCount;
					if( iDeleteCount == 0 ) break;
				}
			}
		}

		for( itTIL = clsDeleteList.begin(); itTIL != clsDeleteList.end(); ++itTIL )
		{
			DeleteThread( *itTIL );
		}
	}
}


bool CTcpThreadList::DeleteThread( int iThreadIndex )
{
	THREAD_LIST::iterator	itTL;
	CTcpComm		clsTcpComm;
	bool bRes = false;

	SendCommand( (char *)&clsTcpComm, sizeof(clsTcpComm), iThreadIndex );

	m_clsMutex.acquire();
	for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
	{
		if( (*itTL)->m_iIndex == iThreadIndex )
		{
			// 쓰레드에서 자료구조를 사용하므로 쓰레드 종료할 때에 삭제한다.
			// delete *itTL;
			m_clsList.erase( itTL );
			bRes = true;
			break;
		}
	}

	if( bRes )
	{
		int iMaxIndex = 0;

		for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
		{
			if( (*itTL)->m_iIndex > iMaxIndex )
			{
				iMaxIndex = (*itTL)->m_iIndex;
			}
		}

		m_iThreadIndex = iMaxIndex;
	}
	m_clsMutex.release();

	return bRes;
}


void CTcpThreadList::GetString( CMonitorString & strBuf )
{
	THREAD_LIST::iterator	itTL;

	strBuf.Clear();

	m_clsMutex.acquire();
	for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
	{
		strBuf.AddRow( (*itTL)->m_clsSessionList.m_iPoolFdCount );
	}
	m_clsMutex.release();
}


bool CTcpThreadList::AddThread()
{
	if( m_pclsStack->m_clsSetup.m_iThreadMaxCount != 0 )
	{
		int iListCount = (int)m_clsList.size();

		if( iListCount >= m_pclsStack->m_clsSetup.m_iThreadMaxCount )
		{
			CLog::Print( LOG_ERROR, "%s thread count(%d) >= max thread count(%d)", __FUNCTION__, iListCount, m_pclsStack->m_clsSetup.m_iThreadMaxCount );
			return false;
		}
	}

	Socket	arrSocket[2];
	CTcpThreadInfo * pclsTcpThreadInfo = new CTcpThreadInfo();
	if( pclsTcpThreadInfo == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	pclsTcpThreadInfo->m_iIndex = GetThreadIndex();

	if( pclsTcpThreadInfo->m_clsSessionList.Init( pclsTcpThreadInfo->m_iIndex, m_iMaxSocketPerThread ) == false )
	{
		CLog::Print( LOG_ERROR, "%s m_clsSessionList.Init error", __FUNCTION__ );
		delete pclsTcpThreadInfo;
		return false;
	}

	if( pipe( arrSocket ) == -1 )
	{
		CLog::Print( LOG_ERROR, "%s pipe error", __FUNCTION__ );
		delete pclsTcpThreadInfo;
		return false;
	}

	pclsTcpThreadInfo->m_hRecv = arrSocket[0];
	pclsTcpThreadInfo->m_hSend = arrSocket[1];
	pclsTcpThreadInfo->m_clsSessionList.Insert( pclsTcpThreadInfo->m_hRecv );
	pclsTcpThreadInfo->m_pclsStack = m_pclsStack;

	m_clsList.push_back( pclsTcpThreadInfo );

	bool bRes = StartThread( "TcpPipeThread", TcpPipeThread, pclsTcpThreadInfo );
	if( bRes == false )
	{
		DeleteThread( pclsTcpThreadInfo->m_iIndex );
		delete pclsTcpThreadInfo;
	}

	return bRes;
}


bool CTcpThreadList::_SendCommand( Socket hSocket, const char * pszData, int iDataLen )
{
	int	n;

#ifdef WIN32
	n = send( hSocket, pszData, iDataLen, 0 );
#else
	n = write( hSocket, pszData, iDataLen );
#endif
	if( n != iDataLen )
	{
		return false;
	}

	return true;
}


int CTcpThreadList::GetCount()
{
	int iCount = 0;

	m_clsMutex.acquire();
	iCount = (int)m_clsList.size();
	m_clsMutex.release();

	return iCount;
}


int CTcpThreadList::GetThreadIndex()
{
	int iThreadIndex = 0;

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

	iThreadIndex = m_iThreadIndex;

	return iThreadIndex;
}


bool CTcpThreadList::SelectThreadIndex( int iThreadIndex )
{
	THREAD_LIST::iterator itTL;

	for( itTL = m_clsList.begin(); itTL != m_clsList.end(); ++itTL )
	{
		if( (*itTL)->m_iIndex == iThreadIndex )
		{
			return true;
		}
	}

	return false;
}
