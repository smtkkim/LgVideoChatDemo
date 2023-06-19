

#include "HttpStatusCode.h"
#include "HttpMessage.h"
#include <stdlib.h>
#include "MemoryDebug.h"

CHttpMessage::CHttpMessage() : m_iStatusCode(-1), m_iContentLength(-1), m_bChunked(false), m_iStreamIdentifier(0)
{
}

CHttpMessage::~CHttpMessage()
{
}


int CHttpMessage::Parse( const char * pszText, int iTextLen )
{
	int iCurPos;

	iCurPos = ParseHeader( pszText, iTextLen );
	if( iCurPos == -1 ) return -1;

	if( m_iContentLength > 0 )
	{
		if( m_iContentLength > ( iTextLen - iCurPos ) ) return -1;

		m_strBody.append( pszText + iCurPos, m_iContentLength );
		return iTextLen;
	}
	else if( m_iContentLength == -1 && iTextLen - iCurPos > 0 )
	{
		m_strBody.append( pszText + iCurPos );
		return iTextLen;
	}

	return iCurPos;
}


int CHttpMessage::ParseHeader( const char * pszText, int iTextLen )
{
	if( pszText == NULL || iTextLen <= 4 ) return -1;

	Clear();

	int iPos, iCurPos, iNameLen;
	const char * pszName, * pszValue;
	CHttpHeader	clsHeader;

	if( !strncmp( pszText, "HTTP/", 4 ) )
	{
		iCurPos = ParseStatusLine( pszText, iTextLen );
	}
	else
	{
		iCurPos = ParseRequestLine( pszText, iTextLen );
	}

	if( iCurPos == -1 ) return -1;

	while( iCurPos < iTextLen )
	{
		iPos = clsHeader.Parse( pszText + iCurPos, iTextLen - iCurPos );
		if( iPos == -1 ) return -1;
		iCurPos += iPos;

		iNameLen = (int)clsHeader.m_strName.length();
		if( iNameLen == 0 ) break;

		pszName = clsHeader.m_strName.c_str();
		pszValue = clsHeader.m_strValue.c_str();

		if( !strcasecmp( pszName, "Content-Type" ) )
		{
			m_strContentType = pszValue;
		}
		else if( !strcasecmp( pszName, "Content-Length" ) )
		{
			m_iContentLength = atoi( pszValue );
		}
		else
		{
			if( !strcasecmp( pszName, "transfer-encoding" ) )
			{
				if( strstr( pszValue, "chunked" ) )
				{
					m_bChunked = true;
				}
			}

			m_clsHeaderList.push_back( clsHeader );
		}
	}

	return iCurPos;
}


void CHttpMessage::Clear()
{
	m_strHttpMethod.clear();
	m_strReqUri.clear();
	m_strHttpVersion.clear();
	m_iStatusCode = -1;
	m_strReasonPhrase.clear();
	m_strContentType.clear();
	m_iContentLength = -1;
	m_clsHeaderList.clear();
	m_strBody.clear();
}


int CHttpMessage::ToString( char * pszText, int iTextSize, bool bHeaderOnly )
{
	if( pszText == NULL || iTextSize <= 0 ) return -1;

	int iLen;

	if( m_strHttpVersion.empty() ) m_strHttpVersion = HTTP_VERSION;

	if( m_iStatusCode > 0 )
	{
		if( m_strReasonPhrase.empty() )
		{
			m_strReasonPhrase = GetReasonPhrase( m_iStatusCode );
		}

		iLen = snprintf( pszText, iTextSize, "%s %d %s\r\n", m_strHttpVersion.c_str(), m_iStatusCode, m_strReasonPhrase.c_str() );
	}
	else
	{
		if( m_strHttpMethod.empty() || m_strReqUri.empty() || m_strHttpVersion.empty() ) return -1;
		iLen = snprintf( pszText, iTextSize, "%s %s %s\r\n", m_strHttpMethod.c_str(), m_strReqUri.c_str(), m_strHttpVersion.c_str() );
	}

	if( m_strContentType.empty() == false )
	{
		iLen += snprintf( pszText + iLen, iTextSize - iLen, "Content-Type: %s\r\n", m_strContentType.c_str() );
	}

	m_iContentLength = (int)m_strBody.length();
	iLen += snprintf( pszText + iLen, iTextSize - iLen, "Content-Length: %d\r\n", m_iContentLength );

	for( HTTP_HEADER_LIST::iterator itList = m_clsHeaderList.begin(); itList != m_clsHeaderList.end(); ++itList )
	{
		iLen += snprintf( pszText + iLen, iTextSize - iLen, "%s: %s\r\n", itList->m_strName.c_str(), itList->m_strValue.c_str() );
	}

	iLen += snprintf( pszText + iLen, iTextSize - iLen, "\r\n" );

	if( m_iContentLength > 0 && bHeaderOnly == false )
	{
		if( m_iContentLength > ( iTextSize - iLen ) )
		{
			return -1;
		}

		memcpy( pszText + iLen, m_strBody.c_str(), m_strBody.length() );
		iLen += m_iContentLength;
	}

	return iLen;
}


int CHttpMessage::ToString( std::string & strText, bool bHeaderOnly )
{
	char szBuf[2048];

	strText.clear();

	if( m_strHttpVersion.empty() ) m_strHttpVersion = HTTP_VERSION;

	if( m_iStatusCode > 0 )
	{
		if( m_strReasonPhrase.empty() )
		{
			m_strReasonPhrase = GetReasonPhrase( m_iStatusCode );
		}

		snprintf( szBuf, sizeof(szBuf), "%s %d %s\r\n", m_strHttpVersion.c_str(), m_iStatusCode, m_strReasonPhrase.c_str() );
	}
	else
	{
		if( m_strHttpMethod.empty() || m_strReqUri.empty() || m_strHttpVersion.empty() ) return -1;
		snprintf( szBuf, sizeof(szBuf), "%s %s %s\r\n", m_strHttpMethod.c_str(), m_strReqUri.c_str(), m_strHttpVersion.c_str() );
	}

	strText.append( szBuf );

	if( m_strContentType.empty() == false )
	{
		snprintf( szBuf, sizeof(szBuf), "Content-Type: %s\r\n", m_strContentType.c_str() );
		strText.append( szBuf );
	}

	m_iContentLength = (int)m_strBody.length();
	snprintf( szBuf, sizeof(szBuf), "Content-Length: %d\r\n", m_iContentLength );
	strText.append( szBuf );

	for( HTTP_HEADER_LIST::iterator itList = m_clsHeaderList.begin(); itList != m_clsHeaderList.end(); ++itList )
	{
		strText.append( itList->m_strName );
		strText.append( ": " );
		strText.append( itList->m_strValue );
		strText.append( "\r\n" );
	}

	strText.append( "\r\n" );

	if( m_iContentLength > 0 && bHeaderOnly == false )
	{
		strText.append( m_strBody );
	}

	return (int)strText.length();
}

bool CHttpMessage::AddHeader( const char * pszName, std::string & strValue )
{
	if( pszName == NULL ) return false;

	CHttpHeader clsHeader( pszName, strValue );

	m_clsHeaderList.push_back( clsHeader );

	return true;
}


bool CHttpMessage::AddHeader( const char * pszName, const char * pszValue )
{
	if( pszName == NULL || pszValue == NULL ) return false;

	CHttpHeader clsHeader( pszName, pszValue );

	m_clsHeaderList.push_back( clsHeader );

	return true;
}


bool CHttpMessage::AddHeader( const char * pszName, int iValue )
{
	char szValue[11];

	snprintf( szValue, sizeof(szValue), "%d", iValue );

	return AddHeader( pszName, szValue );
}


bool CHttpMessage::UpdateHeader( const char * pszName, const char * pszValue )
{
	HTTP_HEADER_LIST::iterator itList;

	for( itList = m_clsHeaderList.begin(); itList != m_clsHeaderList.end(); ++itList )
	{
		if( !strcasecmp( pszName, itList->m_strName.c_str() ) )
		{
			itList->m_strValue = pszValue;
			return true;
		}
	}

	return false;
}


bool CHttpMessage::ReplaceHeader( const char * pszName, const char * pszValue )
{
	if( UpdateHeader( pszName, pszValue ) == false )
	{
		AddHeader( pszName, pszValue );
	}

	return true;
}


CHttpHeader * CHttpMessage::GetHeader( const char * pszName )
{
	HTTP_HEADER_LIST::iterator	itList;

	for( itList = m_clsHeaderList.begin(); itList != m_clsHeaderList.end(); ++itList )
	{
		if( !strcasecmp( pszName, itList->m_strName.c_str() ) )
		{
			return &(*itList);
		}
	}

	return NULL;
}


bool CHttpMessage::SetRequest( const char * pszMethod, CHttpUri * pclsUri, const char * pszUserAgent )
{
	m_strHttpMethod = pszMethod;

	if( pclsUri->m_strPath.empty() )
	{
		m_strReqUri = "/";
	}
	else
	{
		m_strReqUri = pclsUri->m_strPath;
	}

	m_strHttpVersion = HTTP_VERSION;

	std::string strHost = pclsUri->m_strHost;

	if( pclsUri->m_iPort != 80 && pclsUri->m_iPort != 443 )
	{
		char szPort[11];

		snprintf( szPort, sizeof(szPort), ":%d", pclsUri->m_iPort );
		strHost.append( szPort );
	}

	ReplaceHeader( "Host", strHost.c_str() );

	if( pszUserAgent && strlen(pszUserAgent) > 0 )
	{
		ReplaceHeader( "User-Agent", pszUserAgent );
	}

	return true;
}


bool CHttpMessage::IsRequest( )
{
	if( m_strHttpMethod.empty() == false ) return true;

	return false;
}


int CHttpMessage::ParseStatusLine( const char * pszText, int iTextLen )
{
	int		iPos, iStartPos = -1;
	char	cType = 0;

	for( iPos = 0; iPos < iTextLen; ++iPos )
	{
		if( cType != 2 )
		{
			if( pszText[iPos] == ' ' )
			{
				switch( cType )
				{
				case 0:
					m_strHttpVersion.append( pszText, iPos );
					break;
				case 1:
					{
						std::string strStatusCode;

						strStatusCode.append( pszText + iStartPos, iPos - iStartPos );
						m_iStatusCode = atoi( strStatusCode.c_str() );
						break;
					}
				}

				iStartPos = iPos + 1;
				++cType;
			}
		}
		else
		{
			if( pszText[iPos] == '\r' )
			{
				if( iPos + 1 >= iTextLen || pszText[iPos+1] != '\n' ) return -1;

				m_strReasonPhrase.append( pszText + iStartPos, iPos - iStartPos );
				return iPos + 2;
			}
		}
	}

	return -1;
}


int CHttpMessage::ParseRequestLine( const char * pszText, int iTextLen )
{
	int		iPos, iStartPos = -1;
	char	cType = 0;

	for( iPos = 0; iPos < iTextLen; ++iPos )
	{
		if( cType != 2 )
		{
			if( pszText[iPos] == ' ' )
			{
				switch( cType )
				{
				case 0:
					m_strHttpMethod.append( pszText, iPos );
					break;
				case 1:
					m_strReqUri.append( pszText + iStartPos, iPos - iStartPos );
					break;
				}

				iStartPos = iPos + 1;
				++cType;
			}
		}
		else
		{
			if( pszText[iPos] == '\r' )
			{
				if( iPos + 1 >= iTextLen || pszText[iPos+1] != '\n' ) return -1;

				m_strHttpVersion.append( pszText + iStartPos, iPos - iStartPos );
				return iPos + 2;
			}
		}
	}

	return -1;
}
