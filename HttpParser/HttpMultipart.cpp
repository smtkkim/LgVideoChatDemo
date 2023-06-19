

#include "HttpMultipart.h"
#include "HttpHeader.h"
#include "HttpParameterList.h"
#include "MemoryDebug.h"

CHttpMultipart::CHttpMultipart()
{
}

CHttpMultipart::~CHttpMultipart()
{
	Clear();
}


void CHttpMultipart::SetBoundary( const char * pszBoundary )
{
	m_strBoundary = "--";
	m_strBoundary.append( pszBoundary );
}


void CHttpMultipart::SetContentType( const char * pszContentType )
{
	CHttpParameterList clsParamList( ';', true );

	if( clsParamList.Parse( pszContentType ) != -1 )
	{
		std::string strBoundary;

		if( clsParamList.Select( "boundary", strBoundary ) )
		{
			SetBoundary( strBoundary.c_str() );
		}
	}
}


void CHttpMultipart::Clear()
{
	HTTP_MULTIPART_DATA_MAP::iterator itMap;

	for( itMap = m_clsMap.begin(); itMap != m_clsMap.end(); ++itMap )
	{
		delete itMap->second;
	}

	m_clsMap.clear();
}


int CHttpMultipart::Parse( const std::string & strText )
{
	return Parse( strText.c_str(), (int)strText.length() );
}


int CHttpMultipart::Parse( const char * pszText, int iTextLen )
{
	const char * pszBoundary = m_strBoundary.c_str();
	int iBoundaryLen = (int)m_strBoundary.length();
	int iStartPos = -1, iEndPos;

	Clear();

	for( int i = 0; i < iTextLen; )
	{
		if( pszText[i] != '-' || strncmp( pszText + i, pszBoundary, iBoundaryLen ) )
		{
			++i;
			continue;
		}

		if( iStartPos != -1 )
		{
			// \r\n 를 제거하기 위해서 -2 를 추가함
			iEndPos = i - 2;
			if( ParseData( pszText + iStartPos, iEndPos - iStartPos ) == -1 )
			{
				return -1;
			}
		}

		i += iBoundaryLen;

		if( ( i + 4 ) > iTextLen )
		{
			break;
		}

		if( pszText[i] == '-' && pszText[i+1] == '-' && pszText[i+2] == '\r' && pszText[i+3] == '\n' )
		{
			return i + 4;
		}
		
		if( pszText[i] != '\r' || pszText[i+1] != '\n' )
		{
			break;
		}

		iStartPos = i + 2;
	}

	return -1;
}


int CHttpMultipart::ToString( std::string & strText )
{
	HTTP_MULTIPART_DATA_MAP::iterator itMap;

	strText.clear();

	for( itMap = m_clsMap.begin(); itMap != m_clsMap.end(); ++itMap )
	{
		strText.append( m_strBoundary );
		strText.append( "\r\n" );
		strText.append( "Content-Disposition: form-data; name=\"" );
		strText.append( itMap->first );
		strText.append( "\"" );

		if( itMap->second->m_strFileName.empty() == false )
		{
			strText.append( "; filename=\"" );
			strText.append( itMap->second->m_strFileName );
			strText.append( "\"" );
		}

		strText.append( "\r\n" );

		if( itMap->second->m_strContentType.empty() == false )
		{
			strText.append( "Content-Type: " );
			strText.append( itMap->second->m_strContentType );
			strText.append( "\r\n" );
		}

		strText.append( "\r\n" );
		strText.append( itMap->second->m_strValue );
		strText.append( "\r\n" );
	}

	strText.append( m_strBoundary );
	strText.append( "--" );
	strText.append( "\r\n" );

	return (int)strText.length();
}


int CHttpMultipart::ParseData( const char * pszText, int iTextLen )
{
	CHttpMultipartData * pclsData = new CHttpMultipartData();
	if( pclsData == NULL ) return -1;

	std::string strName;
	CHttpHeader clsHeader;
	int iPos, iCurPos = 0;

	while( 1 )
	{
		iPos = clsHeader.Parse( pszText + iCurPos, iTextLen - iCurPos );
		if( iPos == -1 ) break;

		iCurPos += iPos;
		if( iPos == 2 ) break;

		if( !strcmp( clsHeader.m_strName.c_str(), "Content-Disposition" ) )
		{
			CHttpParameterList clsParamList( ';', true );

			if( clsParamList.Parse( clsHeader.m_strValue ) != -1 )
			{
				std::string strTemp;

				if( clsParamList.Select( "name", strTemp ) )
				{
					DeQuoteString( strTemp, strName );
				}

				if( clsParamList.Select( "filename", strTemp ) )
				{
					DeQuoteString( strTemp, pclsData->m_strFileName );
				}
			}
		}
		else if( !strcmp( clsHeader.m_strName.c_str(), "Content-Type" ) )
		{
			pclsData->m_strContentType = clsHeader.m_strValue;
		}
	}

	if( strName.empty() ) return -1;

	pclsData->m_strValue.append( pszText + iCurPos, iTextLen - iCurPos );

	m_clsMap.insert( HTTP_MULTIPART_DATA_MAP::value_type( strName, pclsData ) );

	return iCurPos;
}
