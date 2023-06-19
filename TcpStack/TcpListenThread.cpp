

#include "SipPlatformDefine.h"
#include "TcpStack.h"
#include "TcpThread.h"
#include "ServerUtility.h"
#include "Log.h"
#include "MemoryDebug.h"

static bool gbTcpListenThreadRun = false;


THREAD_API TcpListenThread( LPVOID lpParameter )
{
	CTcpStack * pclsStack = (CTcpStack *)lpParameter;
	CTcpComm clsTcpComm;
	pollfd sttPoll[1];
	int		n, iSleepSecond = 0;

	CLog::Print( LOG_INFO, "TcpListenThread started(port=%d)", pclsStack->m_clsSetup.m_iListenPort );
	gbTcpListenThreadRun = true;

	TcpSetPollIn( sttPoll[0], pclsStack->m_hTcpListenFd );

	while( pclsStack->m_bStop == false )
	{
		n = poll( sttPoll, 1, 1000 );
		if( n <= 0 )
		{
			++iSleepSecond;

			if( iSleepSecond == 60 )
			{
				iSleepSecond = 0;

				if( pclsStack->m_clsSetup.m_bUseThreadPipe )
				{
					pclsStack->m_clsThreadList.DeleteNoUseThread();
				}
			}
			continue;
		}

		iSleepSecond = 0;

		clsTcpComm.m_hSocket = TcpAccept( pclsStack->m_hTcpListenFd, clsTcpComm.m_szIp, sizeof(clsTcpComm.m_szIp), &clsTcpComm.m_iPort );
		if( clsTcpComm.m_hSocket != INVALID_SOCKET )
		{
			if( pclsStack->m_clsSetup.m_bUseThreadPipe )
			{
				if( pclsStack->m_clsThreadList.SendCommand( (char *)&clsTcpComm, sizeof(clsTcpComm) ) == false )
				{
					CLog::Print( LOG_ERROR, "%s m_clsThreadList.SendCommand error", __FUNCTION__ );
					closesocket( clsTcpComm.m_hSocket );
				}
			}
			else
			{
				CTcpNoPipeThreadArg * pclsArg = new CTcpNoPipeThreadArg();
				if( pclsArg == NULL )
				{
					CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
					closesocket( clsTcpComm.m_hSocket );
				}
				else
				{
					pclsArg->m_strIp = clsTcpComm.m_szIp;
					pclsArg->m_iPort = clsTcpComm.m_iPort;
					pclsArg->m_hSocket = clsTcpComm.m_hSocket;
					pclsArg->m_pclsStack = pclsStack;

					if( StartThread( "TcpNoPipeThread", TcpNoPipeThread, pclsArg ) == false )
					{
						CLog::Print( LOG_ERROR, "%s StartThread error", __FUNCTION__ );
						closesocket( clsTcpComm.m_hSocket );
					}
				}
			}
		}
	}

	CLog::Print( LOG_INFO, "TcpListenThread terminated(port=%d)", pclsStack->m_clsSetup.m_iListenPort );
	gbTcpListenThreadRun = false;

	return 0;
}


bool IsTcpListenThreadRun()
{
	return gbTcpListenThreadRun;
}
