

#include "SipPlatformDefine.h"
#include "SipTcp.h"
#include "TlsFunction.h"
#include "HttpClient.h"
#include "Log.h"
#include "FileUtility.h"
#include "MemoryDebug.h"

CHttpClient::CHttpClient() : m_iRecvTimeout(10), m_iStatusCode(0)
{
	InitNetwork();
}

CHttpClient::~CHttpClient()
{
}


bool CHttpClient::DoGet( const char * pszUrl, std::string & strOutputContentType, std::string & strOutputBody )
{
	strOutputContentType.clear();
	strOutputBody.clear();

	if( pszUrl == NULL )
	{
		CLog::Print( LOG_ERROR, "%s pszUrl is null", __FUNCTION__ );
		return false;
	}

	CHttpUri clsUri;
	int iUrlLen = (int)strlen( pszUrl );

	if( clsUri.Parse( pszUrl, iUrlLen ) == -1 )
	{
		CLog::Print( LOG_ERROR, "%s clsUri.Parse(%s) error", __FUNCTION__, pszUrl );
		return false;
	}

	CHttpMessage clsRequest;
	CHttpPacket	clsPacket;

	clsRequest.SetRequest( "GET", &clsUri );

	if( Execute( &clsUri, &clsRequest, &clsPacket ) )
	{
		CHttpMessage * pclsMessage = clsPacket.GetHttpMessage();

		strOutputContentType = pclsMessage->m_strContentType;
		strOutputBody = pclsMessage->m_strBody;
		return true;
	}

	return false;
}


bool CHttpClient::DoGet( const char * pszUrl, const char * pszInputContentType, const char * pszInputBody, std::string & strOutputContentType, std::string & strOutputBody )
{
	strOutputContentType.clear();
	strOutputBody.clear();

	if( pszUrl == NULL || pszInputContentType == NULL || pszInputBody == NULL )
	{
		CLog::Print( LOG_ERROR, "%s pszUrl or input content type or input body is null", __FUNCTION__ );
		return false;
	}

	CHttpUri clsUri;
	int iUrlLen = (int)strlen( pszUrl );
	int iContentLength = (int)strlen( pszInputBody );

	if( iContentLength <= 0 )
	{
		CLog::Print( LOG_ERROR, "%s pszInputBody's length(%d) error", __FUNCTION__, iContentLength );
		return false;
	}

	if( clsUri.Parse( pszUrl, iUrlLen ) == -1 )
	{
		CLog::Print( LOG_ERROR, "%s clsUri.Parse(%s) error", __FUNCTION__, pszUrl );
		return false;
	}

	CHttpMessage clsRequest;
	CHttpPacket clsPacket;

	clsRequest.SetRequest( "GET", &clsUri );
	clsRequest.m_strContentType = pszInputContentType;
	clsRequest.m_iContentLength = iContentLength;
	clsRequest.m_strBody = pszInputBody;

	if( Execute( &clsUri, &clsRequest, &clsPacket ) )
	{
		CHttpMessage * pclsMessage = clsPacket.GetHttpMessage();

		strOutputContentType = pclsMessage->m_strContentType;
		strOutputBody = pclsMessage->m_strBody;
		return true;
	}

	return false;
}


bool CHttpClient::DoGet( const char * pszUrl, HTTP_HEADER_LIST * pclsHeaderList, std::string & strOutputContentType, std::string & strOutputBody )
{
	strOutputContentType.clear();
	strOutputBody.clear();

	if( pszUrl == NULL )
	{
		CLog::Print( LOG_ERROR, "%s pszUrl is null", __FUNCTION__ );
		return false;
	}

	CHttpUri clsUri;
	int iUrlLen = (int)strlen( pszUrl );

	if( clsUri.Parse( pszUrl, iUrlLen ) == -1 )
	{
		CLog::Print( LOG_ERROR, "%s clsUri.Parse(%s) error", __FUNCTION__, pszUrl );
		return false;
	}

	CHttpMessage clsRequest;
	CHttpPacket	clsPacket;

	clsRequest.SetRequest( "GET", &clsUri );

	if( pclsHeaderList )
	{
		clsRequest.m_clsHeaderList.insert( clsRequest.m_clsHeaderList.end(), pclsHeaderList->begin(), pclsHeaderList->end() );
	}

	if( Execute( &clsUri, &clsRequest, &clsPacket ) )
	{
		CHttpMessage * pclsMessage = clsPacket.GetHttpMessage();

		strOutputContentType = pclsMessage->m_strContentType;
		strOutputBody = pclsMessage->m_strBody;
		return true;
	}

	return false;
}


bool CHttpClient::DoPost( const char * pszUrl, const char * pszInputContentType, const char * pszInputBody, std::string & strOutputContentType, std::string & strOutputBody )
{
	return DoPost( pszUrl, NULL, pszInputContentType, pszInputBody, 0, strOutputContentType, strOutputBody );
}


bool CHttpClient::DoPost( const char * pszUrl, HTTP_HEADER_LIST * pclsHeaderList, const char * pszInputContentType, const char * pszInputBody, int iInputBodyLen, std::string & strOutputContentType, std::string & strOutputBody )
{
	strOutputContentType.clear();
	strOutputBody.clear();

	if( pszUrl == NULL || pszInputContentType == NULL || pszInputBody == NULL )
	{
		CLog::Print( LOG_ERROR, "%s pszUrl or input content type or input body is null", __FUNCTION__ );
		return false;
	}

	CHttpUri clsUri;
	int iUrlLen = (int)strlen( pszUrl );
	int iContentLength = 0;

	if( iInputBodyLen > 0 )
	{
		iContentLength = iInputBodyLen;
	}
	else
	{
		iContentLength = (int)strlen( pszInputBody );
	}

	if( iContentLength <= 0 )
	{
		CLog::Print( LOG_ERROR, "%s pszInputBody's length(%d) error", __FUNCTION__, iContentLength );
		return false;
	}

	if( clsUri.Parse( pszUrl, iUrlLen ) == -1 )
	{
		CLog::Print( LOG_ERROR, "%s clsUri.Parse(%s) error", __FUNCTION__, pszUrl );
		return false;
	}

	CHttpMessage clsRequest;
	CHttpPacket clsPacket;

	clsRequest.SetRequest( "POST", &clsUri );
	clsRequest.m_strContentType = pszInputContentType;
	clsRequest.m_iContentLength = iContentLength;
	clsRequest.m_strBody.append( pszInputBody, iContentLength );

	if( pclsHeaderList )
	{
		clsRequest.m_clsHeaderList.insert( clsRequest.m_clsHeaderList.end(), pclsHeaderList->begin(), pclsHeaderList->end() );
	}

	if( Execute( &clsUri, &clsRequest, &clsPacket ) )
	{
		CHttpMessage * pclsMessage = clsPacket.GetHttpMessage();

		strOutputContentType = pclsMessage->m_strContentType;
		strOutputBody = pclsMessage->m_strBody;
		return true;
	}

	return false;
}


bool CHttpClient::DoSoap( const char * pszUrl, const char * pszSoapAction, const char * pszInputBody, std::string & strOutputBody )
{
	return DoSoap( pszUrl, pszSoapAction, "text/xml;charset=UTF-8", pszInputBody, strOutputBody );
}


bool CHttpClient::DoSoap( const char * pszUrl, const char * pszSoapAction, const char * pszInputContentType, const char * pszInputBody, std::string & strOutputBody )
{
	std::string strOutputContentType;

	strOutputBody.clear();

	if( pszSoapAction && strlen(pszSoapAction) > 0 )
	{
		HTTP_HEADER_LIST clsHeaderList;
		CHttpHeader clsHeader( "SOAPAction", pszSoapAction );

		clsHeaderList.push_back( clsHeader );

		return DoPost( pszUrl, &clsHeaderList, pszInputContentType, pszInputBody, 0, strOutputContentType, strOutputBody );
	}
	
	return DoPost( pszUrl, NULL, pszInputContentType, pszInputBody, 0, strOutputContentType, strOutputBody );
}


bool CHttpClient::DoUpload( const char * pszUrl, const char * pszFilePath, const char * pszPostName, POST_NAME_VALUE_MAP & clsPostDataMap, std::string & strOutputContentType, std::string & strOutputBody )
{
	std::string strContentType, strBody, strBoundary, strFileName;
	char szBuf[8192];
	int iLen;

	GetFileNameOfFilePath( pszFilePath, strFileName );

	strBoundary = "D265359A-0089-402c-BD68-BD0E20D4D5B8";
	strContentType = "multipart/form-data; boundary=";
	strContentType.append( strBoundary );

	POST_NAME_VALUE_MAP::iterator itPM;

	for( itPM = clsPostDataMap.begin(); itPM != clsPostDataMap.end(); ++itPM )
	{
		strBody.append( "--" );
		strBody.append( strBoundary );
		strBody.append( "\r\n" );
		strBody.append( "Content-Disposition: form-data; name=\"" );
		strBody.append( itPM->first );
		strBody.append( "\"\r\n\r\n" );
		strBody.append( itPM->second );
		strBody.append( "\r\n" );
	}

	strBody.append( "--" );
	strBody.append( strBoundary );
	strBody.append( "\r\n" );

	strBody.append( "Content-Disposition: form-data; name=\"" );
	strBody.append( pszPostName );
	strBody.append( "\"; filename=\"" );
	strBody.append( strFileName );
	strBody.append( "\"\r\nContent-Type: application/octet-stream\r\n\r\n" );
	
	FILE * fd = fopen( pszFilePath, "rb" );
	if( fd == NULL )
	{
		CLog::Print( LOG_ERROR, "%s fopen(%s) error(%d)", __FUNCTION__, pszFilePath, GetError() );
		return false;
	}

	while( 1 )
	{
		iLen = (int)fread( szBuf, 1, sizeof(szBuf), fd );
		if( iLen <= 0 ) break;

		strBody.append( szBuf, iLen );
	}

	fclose( fd );

	strBody.append( "\r\n" );
	strBody.append( "--" );
	strBody.append( strBoundary );
	strBody.append( "--\r\n" );

	return DoPost( pszUrl, NULL, strContentType.c_str(), strBody.c_str(), (int)strBody.length(), strOutputContentType, strOutputBody );
}


void CHttpClient::SetUserAgent( const char * pszUserAgent )
{
	m_strUserAgent = pszUserAgent;
}


void CHttpClient::SetRecvTimeout( int iRecvTimeout )
{
	m_iRecvTimeout = iRecvTimeout;
}


int CHttpClient::GetStatusCode()
{
	return m_iStatusCode;
}


bool CHttpClient::Execute( CHttpUri * pclsUri, CHttpMessage * pclsRequest, CHttpPacket * pclsPacket )
{
	char * pszBuf = NULL;
	int iBufLen, n;
	bool bRes = false;
	Socket hSocket = INVALID_SOCKET;
	SSL * psttSsl = NULL;
	CHttpMessage * pclsResponse = pclsPacket->GetHttpMessage();

	int iNewBufLen = 8192 + (int)pclsRequest->m_strBody.length();
	pszBuf = (char *)malloc( iNewBufLen );
	if( pszBuf == NULL )
	{
		CLog::Print( LOG_ERROR, "%s malloc error", __FUNCTION__ );
		return false;
	}

	memset( pszBuf, 0, iNewBufLen );

	pclsRequest->m_strHttpVersion = "HTTP/1.1";

	if( m_strUserAgent.empty() == false )
	{
		pclsRequest->AddHeader( "User-Agent", m_strUserAgent );
	}
	
	iBufLen = pclsRequest->ToString( pszBuf, iNewBufLen );
	if( iBufLen <= 0 )
	{
		CLog::Print( LOG_ERROR, "%s clsRequest.ToString() error", __FUNCTION__ );
		goto FUNC_END;
	}

	hSocket = TcpConnect( pclsUri->m_strHost.c_str(), pclsUri->m_iPort );
	if( hSocket == INVALID_SOCKET )
	{
		CLog::Print( LOG_ERROR, "%s TcpConnect(%s:%d) error", __FUNCTION__, pclsUri->m_strHost.c_str(), pclsUri->m_iPort );
		goto FUNC_END;
	}

	CLog::Print( LOG_NETWORK, "TcpConnect(%s:%d) success", pclsUri->m_strHost.c_str(), pclsUri->m_iPort );

	// https 프로토콜이면 TLS 로 연결한다.
	if( !strcmp( pclsUri->m_strProtocol.c_str(), "https" ) )
	{
		if( SSLConnect( hSocket, &psttSsl ) == false )
		{
			CLog::Print( LOG_ERROR, "%s SSLConnect error", __FUNCTION__ );
			goto FUNC_END;
		}

		CLog::Print( LOG_NETWORK, "SSLConnect(%s:%d) success", pclsUri->m_strHost.c_str(), pclsUri->m_iPort );
	}

	if( psttSsl )
	{
		if( SSLSend( psttSsl, pszBuf, iBufLen ) != iBufLen )
		{
			CLog::Print( LOG_ERROR, "%s SSLSend error", __FUNCTION__ );
			goto FUNC_END;
		}
	}
	else
	{
		if( TcpSend( hSocket, pszBuf, iBufLen ) != iBufLen )
		{
			CLog::Print( LOG_ERROR, "%s TcpSend error", __FUNCTION__ );
			goto FUNC_END;
		}
	}

	CLog::Print( LOG_NETWORK, "TcpSend(%s:%d) [%s]", pclsUri->m_strHost.c_str(), pclsUri->m_iPort, pszBuf );

	while( 1 )
	{
		memset( pszBuf, 0, iNewBufLen );

		if( psttSsl )
		{
			n = SSLRecv( psttSsl, pszBuf, iNewBufLen );
		}
		else
		{
			n = TcpRecv( hSocket, pszBuf, iNewBufLen, m_iRecvTimeout );
		}

		if( n <= 0 )
		{
			break;
		}

		CLog::Print( LOG_NETWORK, "TcpRecv(%s:%d) [%.*s]", pclsUri->m_strHost.c_str(), pclsUri->m_iPort, n, pszBuf );

		if( pclsPacket->AddPacket( pszBuf, n ) == false )
		{
			CLog::Print( LOG_ERROR, "%s clsPacket.AddPacket error", __FUNCTION__ );
			break;
		}

		if( pclsPacket->IsCompleted() ) break;
	}

	m_iStatusCode = pclsResponse->m_iStatusCode;

	if( pclsResponse->m_iStatusCode / 100 == 2 )
	{
		bRes = true;
	}

FUNC_END:
	if( psttSsl )
	{
		SSLClose( psttSsl );
		psttSsl = NULL;
	}

	if( hSocket != INVALID_SOCKET )
	{
		closesocket( hSocket );
		hSocket = INVALID_SOCKET;
	}

	if( pszBuf )
	{
		free( pszBuf );
		pszBuf = NULL;
	}

	// 3XX 응답에서 Location 헤더가 존재하면 해당 URL 로 다시 시도한다.
	if( pclsResponse->m_iStatusCode / 100 == 3 )
	{
		CHttpHeader * pclsHeader = pclsResponse->GetHeader( "Location" );
		if( pclsHeader && pclsHeader->m_strValue.empty() == false )
		{
			CHttpUri clsUri;

			if( clsUri.Parse( pclsHeader->m_strValue.c_str(), (int)pclsHeader->m_strValue.length() ) )
			{
				if( clsUri.m_strHost.empty() )
				{
					clsUri.m_strHost = pclsUri->m_strHost;
					clsUri.m_iPort = pclsUri->m_iPort;
					clsUri.m_strProtocol = pclsUri->m_strProtocol;
					clsUri.m_strPath = pclsHeader->m_strValue;
				}
				
				pclsRequest->SetRequest( pclsRequest->m_strHttpMethod.c_str(), &clsUri );

				return Execute( &clsUri, pclsRequest, pclsPacket );
			}
		}
	}

	return bRes;
}
