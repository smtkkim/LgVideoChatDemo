

#include "SipPlatformDefine.h"
#include "HttpParameterList.h"
#include "MemoryDebug.h"

CHttpParameterList::CHttpParameterList( char cSep, bool bRemoveFrontSpace ) : m_cSep( cSep ), m_bRemoveFrontSpace( bRemoveFrontSpace )
{
}

CHttpParameterList::~CHttpParameterList()
{
}


int CHttpParameterList::Parse( const char * pszText )
{
	int iLen = (int)strlen( pszText );

	return Parse( pszText, iLen );
}


int CHttpParameterList::Parse( const std::string & strText )
{
	return Parse( strText.c_str(), (int)strText.length() );
}


int CHttpParameterList::Parse( const char * pszText, int iTextLen )
{
	int iCurPos = 0, iPos;

	m_clsParamList.clear();

	while( iCurPos < iTextLen )
	{
		if( pszText[iCurPos] == m_cSep )
		{
			++iCurPos;
			continue;
		}

		if( m_bRemoveFrontSpace )
		{
			if( pszText[iCurPos] == ' ' )
			{
				++iCurPos;
				continue;
			}
		}

		iPos = ParseOne( pszText + iCurPos, iTextLen - iCurPos );
		if( iPos == -1 ) return -1;
		iCurPos += iPos;
	}

	return iCurPos;
}


int CHttpParameterList::ParseUrl( const char * pszText )
{
	int iLen = (int)strlen( pszText );

	return ParseUrl( pszText, iLen );
}


int CHttpParameterList::ParseUrl( const std::string & strText )
{
	return ParseUrl( strText.c_str(), (int)strText.length() );
}


int CHttpParameterList::ParseUrl( const char * pszText, int iTextLen )
{
	int iPos = -1;

	for( int i = 0; i < iTextLen; ++i )
	{
		if( pszText[i] == '?' )
		{
			iPos = i + 1;
			break;
		}
	}

	if( iPos == -1 ) return -1;

	return Parse( pszText + iPos, iTextLen - iPos );
}


int CHttpParameterList::ParseOne( const char * pszText, int iTextLen )
{
	CHttpParameter clsParam( m_cSep );

	int iPos = clsParam.Parse( pszText, iTextLen );
	if( iPos == -1 ) return -1;

	m_clsParamList.push_back( clsParam );

	return iPos;
}


int CHttpParameterList::ToString( char * pszText, int iTextSize )
{
	HTTP_PARAMETER_LIST::iterator	itList;
	int iLen = 0, iPos;

	for( itList = m_clsParamList.begin(); itList != m_clsParamList.end(); ++itList )
	{
		if( iLen >= iTextSize ) return -1;

		if( itList != m_clsParamList.begin() )
		{
			pszText[iLen++] = m_cSep;
		}

		iPos = itList->ToString( pszText + iLen, iTextSize - iLen );
		if( iPos == -1 ) return -1;
		iLen += iPos;
	}

	return iLen;
}


bool CHttpParameterList::Insert( const char * pszName, const char * pszValue )
{
	if( pszName == NULL ) return false;

	CHttpParameter clsParam;

	clsParam.m_strName = pszName;
	if( pszValue ) clsParam.m_strValue = pszValue;

	m_clsParamList.push_back( clsParam );

	return true;
}


bool CHttpParameterList::Update( const char * pszName, const char * pszValue )
{
	HTTP_PARAMETER_LIST::iterator	itList;

	for( itList = m_clsParamList.begin(); itList != m_clsParamList.end(); ++itList )
	{
		if( !strcasecmp( itList->m_strName.c_str(), pszName ) )
		{
			itList->m_strValue = pszValue;
			return true;
		}
	}

	return false;
}


bool CHttpParameterList::Select( const char * pszName, std::string & strValue )
{
	HTTP_PARAMETER_LIST::iterator	itList;

	for( itList = m_clsParamList.begin(); itList != m_clsParamList.end(); ++itList )
	{
		if( !strcasecmp( itList->m_strName.c_str(), pszName ) )
		{
			strValue = itList->m_strValue;
			return true;
		}
	}

	return false;
}


bool CHttpParameterList::Select( const char * pszName )
{
	HTTP_PARAMETER_LIST::iterator	itList;

	for( itList = m_clsParamList.begin(); itList != m_clsParamList.end(); ++itList )
	{
		if( !strcasecmp( itList->m_strName.c_str(), pszName ) )
		{
			return true;
		}
	}

	return false;
}


const char * CHttpParameterList::SelectValue( const char * pszName )
{
	HTTP_PARAMETER_LIST::iterator	itList;

	for( itList = m_clsParamList.begin(); itList != m_clsParamList.end(); ++itList )
	{
		if( !strcasecmp( itList->m_strName.c_str(), pszName ) )
		{
			return itList->m_strValue.c_str();
		}
	}

	return NULL;
}


void CHttpParameterList::ClearParam()
{
	m_clsParamList.clear();
}
