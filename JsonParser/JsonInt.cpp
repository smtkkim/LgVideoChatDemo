

#include "SipPlatformDefine.h"
#include "JsonInt.h"
#include <stdlib.h>
#include "MemoryDebug.h"

CJsonInt::CJsonInt() : m_iValue(0)
{
	m_cType = JSON_TYPE_INT;
}

CJsonInt::~CJsonInt()
{
}


int CJsonInt::Parse( const char * pszText, int iTextLen )
{
	m_iValue = 0;

	for( int i = 0; i < iTextLen; ++i )
	{
		if( i == 0 && pszText[i] == '-' )
		{
			// 첫번째 - 는 음수이므로 넘어간다.
		}
		else if( isdigit( pszText[i] ) == 0 )
		{
			std::string strInt;

			strInt.append( pszText, i );
			m_iValue = atoll( strInt.c_str() );

			return i;
		}
	}

	return -1;
}


int CJsonInt::ToString( std::string & strText, EJsonNewLine eNewLine, int iDepth )
{
	char szText[22];

	int iLen = snprintf( szText, sizeof(szText), LONG_LONG_FORMAT, m_iValue );
	strText.append( szText );

	return iLen;
}


int CJsonInt::GetStringLen( )
{
	char szText[22];

	return snprintf( szText, sizeof(szText), LONG_LONG_FORMAT, m_iValue );
}


CJsonType * CJsonInt::Copy( )
{
	CJsonInt * pclsInt = new CJsonInt();
	if( pclsInt == NULL ) return NULL;

	pclsInt->m_iValue = m_iValue;

	return pclsInt;
}
