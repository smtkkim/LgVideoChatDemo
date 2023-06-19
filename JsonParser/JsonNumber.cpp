

#include "SipPlatformDefine.h"
#include "JsonNumber.h"
#include "MemoryDebug.h"

CJsonNumber::CJsonNumber()
{
	m_cType = JSON_TYPE_NUMBER;
}

CJsonNumber::~CJsonNumber()
{
}


int CJsonNumber::Parse( const char * pszText, int iTextLen )
{
	m_strValue.clear();

	for( int i = 0; i < iTextLen; ++i )
	{
		if( i == 0 && pszText[i] == '-' )
		{
			// 첫번째 - 는 음수이므로 넘어간다.
		}
		else if( isspace( pszText[i] ) || pszText[i] == ',' || pszText[i] == '}' || pszText[i] == ']' )
		{
			m_strValue.append( pszText, i );
			return i;
		}
	}

	return -1;
}


int CJsonNumber::ToString( std::string & strText, EJsonNewLine eNewLine, int iDepth )
{
	strText.append( m_strValue );

	return (int)m_strValue.length();
}


int CJsonNumber::GetStringLen( )
{
	return (int)m_strValue.length();
}


CJsonType * CJsonNumber::Copy( )
{
	CJsonNumber * pclsNum = new CJsonNumber();
	if( pclsNum == NULL ) return NULL;

	pclsNum->m_strValue = m_strValue;

	return pclsNum;
}


bool CJsonNumber::IsDouble( )
{
	const char * pszValue = m_strValue.c_str();
	int iLen = (int)m_strValue.length();

	for( int i = 0; i < iLen; ++i )
	{
		if( pszValue[i] == 'e' || pszValue[i] == '.' ) return true;
	}

	return false;
}
