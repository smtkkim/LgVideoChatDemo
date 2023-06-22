

#include "SipPlatformDefine.h"
#include "TcpStack.h"
#include "TcpThread.h"
#include "ServerUtility.h"
#include "SipTcp.h"
#include "Log.h"
#include "MemoryDebug.h"


class CTcpClientArg
{
public:
	CTcpClientArg()
	{
	}

	~CTcpClientArg()
	{
	}

	
	std::string	m_strIp;

	
	int			m_iPort;

	
	CTcpStack * m_pclsStack;
};


THREAD_API TcpClientThread( LPVOID lpParameter )
{
	CTcpClientArg * pclsArg = (CTcpClientArg *)lpParameter;

	CLog::Print( LOG_INFO, "TcpClientThread started (%s:%d)", pclsArg->m_strIp.c_str(), pclsArg->m_iPort );

	Socket hConn = TcpConnect( pclsArg->m_strIp.c_str(), pclsArg->m_iPort, pclsArg->m_pclsStack->m_clsSetup.m_iTcpConnectTimeout );
	if( hConn == INVALID_SOCKET )
	{
		CLog::Print( LOG_ERROR, "%s TcpConnect(%s:%d) error(%d)", __FUNCTION__, pclsArg->m_strIp.c_str(), pclsArg->m_iPort, GetError() );

		pclsArg->m_pclsStack->m_clsClientMap.Delete( pclsArg->m_strIp.c_str(), pclsArg->m_iPort );
	}
	else
	{
		CTcpComm clsTcpComm;
		bool bAccept = true;

		if( pclsArg->m_pclsStack->m_clsSetup.m_bUseTls )
		{
			if( SSLConnect( hConn, &clsTcpComm.m_psttSsl ) == false )
			{
				CLog::Print( LOG_ERROR, "%s SSLConnect(%s:%d) error", __FUNCTION__, pclsArg->m_strIp.c_str(), pclsArg->m_iPort );
				closesocket( hConn );

				pclsArg->m_pclsStack->m_clsClientMap.Delete( pclsArg->m_strIp.c_str(), pclsArg->m_iPort );
				bAccept = false;
			}
		}

		if( bAccept )
		{
			if( pclsArg->m_pclsStack->m_clsSetup.m_bUseThreadPipe )
			{
				clsTcpComm.m_hSocket = hConn;
				snprintf( clsTcpComm.m_szIp, sizeof(clsTcpComm.m_szIp), "%s", pclsArg->m_strIp.c_str() );
				clsTcpComm.m_iPort = pclsArg->m_iPort;
				clsTcpComm.m_bClient = true;

				if( pclsArg->m_pclsStack->m_clsThreadList.SendCommand( (char *)&clsTcpComm, sizeof(clsTcpComm) ) == false )
				{
					CLog::Print( LOG_ERROR, "%s m_clsThreadList.SendCommand error", __FUNCTION__ );
					pclsArg->m_pclsStack->m_clsClientMap.Delete( pclsArg->m_strIp.c_str(), pclsArg->m_iPort );
					closesocket( hConn );
				}
			}
			else
			{
				CTcpNoPipeThreadArg * pclsNewArg = new CTcpNoPipeThreadArg();
				if( pclsNewArg == NULL )
				{
					CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
					closesocket( hConn );
				}
				else
				{
					pclsNewArg->m_hSocket = hConn;
					pclsNewArg->m_strIp = pclsArg->m_strIp;
					pclsNewArg->m_iPort = pclsArg->m_iPort;
					pclsNewArg->m_pclsStack = pclsArg->m_pclsStack;
					pclsNewArg->m_bClient = true;

					if( StartThread( "TcpNoPipeThread", TcpNoPipeThread, pclsNewArg ) == false )
					{
						CLog::Print( LOG_ERROR, "%s StartThread error", __FUNCTION__ );
						closesocket( hConn );
					}
				}

				pclsArg->m_pclsStack->m_clsClientMap.Delete( pclsArg->m_strIp.c_str(), pclsArg->m_iPort );
			}
		}
	}

	CLog::Print( LOG_INFO, "TcpClientThread terminated (%s:%d)", pclsArg->m_strIp.c_str(), pclsArg->m_iPort );

	delete pclsArg;

	//if( pclsArg->m_pclsStack->m_clsSetup.m_bUseTls )
	//{
		//ERR_remove_thread_state( NULL );
	//}

	return 0;
}


bool StartTcpClientThread( const char * pszIp, int iPort, CTcpStack * pclsStack )
{
	CTcpClientArg * pclsArg = new CTcpClientArg();
	if( pclsArg == NULL ) return false;

	pclsArg->m_strIp = pszIp;
	pclsArg->m_iPort = iPort;
	pclsArg->m_pclsStack = pclsStack;

	if( StartThread( "TcpClientThread", TcpClientThread, pclsArg ) == false )
	{
		delete pclsArg;
		return false;
	}

	return true;
}
