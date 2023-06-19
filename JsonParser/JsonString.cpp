

#include "SipPlatformDefine.h"
#include "JsonString.h"
#include "MemoryDebug.h"

CJsonString::CJsonString()
{
	m_cType = JSON_TYPE_STRING;
}

CJsonString::~CJsonString()
{
}


int CJsonString::Parse( const char * pszText, int iTextLen )
{
	char cSep = '\0';
	int iPos = 0;

	m_strValue.clear();

	for( int i = 0; i < iTextLen; ++i )
	{
		if( cSep == '\0' )
		{
			if( pszText[i] == '"' || pszText[i] == '\'' )
			{
				cSep = pszText[i];
				iPos = i + 1;
			}
		}
		else if( pszText[i] == cSep )
		{
			m_strValue.append( pszText + iPos, i - iPos );
			return i + 1;
		}
	}

	return -1;
}


int CJsonString::ToString( std::string & strText, EJsonNewLine eNewLine, int iDepth )
{
	strText.append( "\"" );
	strText.append( m_strValue );
	strText.append( "\"" );

	return (int)m_strValue.length() + 2;
}


int CJsonString::GetStringLen( )
{
	return (int)m_strValue.length() + 2;
}


CJsonType * CJsonString::Copy( )
{
	CJsonString * pclsString = new CJsonString();
	if( pclsString == NULL ) return NULL;

	pclsString->m_strValue = m_strValue;

	return pclsString;
}
