

#include "HttpStack.h"
#include "HttpStatusCode.h"
#include "Http2Define.h"
#include "Http2Settings.h"
#include "Log.h"
#include "Base64.h"
#include "MemoryDebug.h"

#include "HttpStackHttp2.hpp"
#include "HttpStackWebSocket.hpp"

const EVP_MD * CHttpStack::m_psttMd = NULL;

CHttpStack::CHttpStack() : m_pclsCallBack(NULL)
{
	if( m_psttMd == NULL )
	{
		//OpenSSL_add_all_digests();
		m_psttMd = EVP_get_digestbyname( "sha1" );
	}
}

CHttpStack::~CHttpStack()
{
}


bool CHttpStack::Start( CTcpStackSetup * pclsSetup, IHttpStackCallBack * pclsCallBack )
{
	if( pclsCallBack == NULL )
	{
		CLog::Print( LOG_ERROR, "%s IHttpStackCallBack is NULL", __FUNCTION__ );
		return false;
	}

	m_pclsCallBack = pclsCallBack;

	return m_clsTcpStack.Start( pclsSetup, this );
}


bool CHttpStack::Stop( )
{
	return m_clsTcpStack.Stop();
}


void CHttpStack::Release()
{
	//EVP_cleanup();
}


bool CHttpStack::InComingConnected( CTcpSessionInfo * pclsSessionInfo )
{
	return true;
}


void CHttpStack::SessionClosed( CTcpSessionInfo * pclsSessionInfo )
{
	if( pclsSessionInfo->m_pclsApp )
	{
		CHttpStackSession * pclsApp = (CHttpStackSession *)pclsSessionInfo->m_pclsApp;

		if( pclsApp->m_eType == E_HST_WEB_SOCKET )
		{
			m_pclsCallBack->WebSocketClosed( pclsSessionInfo->m_strIp.c_str(), pclsSessionInfo->m_iPort );
		}
	}
}


bool CHttpStack::RecvPacket( char * pszPacket, int iPacketLen, CTcpSessionInfo * pclsSessionInfo )
{
	if( pclsSessionInfo->m_pclsApp == NULL )
	{
		pclsSessionInfo->m_pclsApp = new CHttpStackSession();
		if( pclsSessionInfo->m_pclsApp == NULL )
		{
			CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
			return false;
		}
	}

	CHttpStackSession * pclsApp = (CHttpStackSession *)pclsSessionInfo->m_pclsApp;

	if( pclsApp->m_eType == E_HST_HTTP_2 )
	{
		return RecvPacketHttp2( pszPacket, iPacketLen, pclsSessionInfo, pclsApp );
	}
	
	if( pclsApp->m_eType == E_HST_WEB_SOCKET )
	{
		return RecvPacketWebSocket( pszPacket, iPacketLen, pclsSessionInfo, pclsApp );
	}
	// HTTP 프로토콜
	if( pclsApp->m_clsHttpPacket.AddPacket( pszPacket, iPacketLen ) == false )
	{
		CLog::Print( LOG_ERROR, "%s m_clsPacket.AddPacket error", __FUNCTION__ );
		return false;
	}

	if( pclsApp->m_clsHttpPacket.IsCompleted() )
	{
		CHttpMessage * pclsRecv = pclsApp->m_clsHttpPacket.GetHttpMessage();
		if( pclsRecv->IsRequest() )
		{
			CHttpMessage clsSend;
			CHttpHeader * pclsHeader;
			bool bClose = false;

			if( !strcmp( pclsRecv->m_strHttpMethod.c_str(), "PRI" ) && !strcmp( pclsRecv->m_strReqUri.c_str(), "*" ) && !strncmp( pclsRecv->m_strBody.c_str(), "SM", 2 ) )
			{
				// HTTP/2 프로토콜
				return RecvPacketHttp2Pri( pclsSessionInfo, pclsApp );
			}

			pclsHeader = pclsRecv->GetHeader( "Upgrade" );
			if( pclsHeader && !strcmp( pclsHeader->m_strValue.c_str(), "websocket" ) )
			{
				// WebSocket 을 위한 HTTP 요청이면 응용으로 callback 호출하지 않고 응답 메시지를 전송한다.
				if( MakeWebSocketResponse( pclsRecv, &clsSend ) == false )
				{
					return false;
				}

				pclsApp->m_eType = E_HST_WEB_SOCKET;
			}
			else
			{
				pclsHeader = pclsRecv->GetHeader( "Connection" );
				if( pclsHeader == NULL || !strcmp( pclsHeader->m_strValue.c_str(), "close" ) )
				{
					bClose = true;
				}

				if( m_pclsCallBack->RecvHttpRequest( pclsRecv, &clsSend ) == false )
				{
					CLog::Print( LOG_ERROR, "%s RecvHttpRequest error", __FUNCTION__ );
					return false;
				}
			}

			int iNewBufLen = 8192 + (int)clsSend.m_strBody.length();
			char * pszBuf = (char *)malloc( iNewBufLen );
			if( pszBuf == NULL )
			{
				CLog::Print( LOG_ERROR, "%s malloc error", __FUNCTION__ );
				return false;
			}
			
			int iBufLen = clsSend.ToString( pszBuf, iNewBufLen );
			if( iBufLen == -1 )
			{
				CLog::Print( LOG_ERROR, "%s ToString error", __FUNCTION__ );
				free( pszBuf );
				return false;
			}

			if( pclsSessionInfo->Send( pszBuf, iBufLen ) == false )
			{
				CLog::Print( LOG_ERROR, "%s Send error", __FUNCTION__ );
				free( pszBuf );
				return false;
			}

			free( pszBuf );

			if( bClose )
			{
				return false;
			}

			if( pclsApp->m_eType == E_HST_WEB_SOCKET )
			{
				m_pclsCallBack->WebSocketConnected( pclsSessionInfo->m_strIp.c_str(), pclsSessionInfo->m_iPort, pclsRecv );
			}
		}
		else
		{
			CLog::Print( LOG_ERROR, "%s http request error", __FUNCTION__ );
			return false;
		}
	}

	return true;
}
