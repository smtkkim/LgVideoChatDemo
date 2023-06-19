

#include "SipPlatformDefine.h"
#include "JsonNull.h"
#include "MemoryDebug.h"

CJsonNull::CJsonNull()
{
	m_cType = JSON_TYPE_NULL;
}

CJsonNull::~CJsonNull()
{
}


int CJsonNull::Parse( const char * pszText, int iTextLen )
{
	if( !strncmp( pszText, "null", 4 ) )
	{
		return 4;
	}

	return -1;
}


int CJsonNull::ToString( std::string & strText, EJsonNewLine eNewLine, int iDepth )
{
	strText.append( "null" );

	return 4;
}


int CJsonNull::GetStringLen( )
{
	return 4;
}


CJsonType * CJsonNull::Copy( )
{
	CJsonNull * pclsNull = new CJsonNull();
	if( pclsNull == NULL ) return NULL;

	return pclsNull;
}
