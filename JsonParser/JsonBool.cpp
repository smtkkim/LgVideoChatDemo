

#include "SipPlatformDefine.h"
#include "JsonBool.h"
#include "MemoryDebug.h"

CJsonBool::CJsonBool() : m_bValue(false)
{
	m_cType = JSON_TYPE_BOOL;
}

CJsonBool::~CJsonBool()
{
}


int CJsonBool::Parse( const char * pszText, int iTextLen )
{
	if( !strncmp( pszText, "true", 4 ) )
	{
		m_bValue = true;
		return 4;
	}
	else if( !strncmp( pszText, "false", 5 ) )
	{
		m_bValue = false;
		return 5;
	}

	return -1;
}


int CJsonBool::ToString( std::string & strText, EJsonNewLine eNewLine, int iDepth )
{
	if( m_bValue )
	{
		strText.append( "true" );
		return 4;
	}

	strText.append( "false" );
	return 5;
}


int CJsonBool::GetStringLen( )
{
	if( m_bValue )
	{
		return 4;
	}

	return 5;
}


CJsonType * CJsonBool::Copy( )
{
	CJsonBool * pclsBool = new CJsonBool();
	if( pclsBool == NULL ) return NULL;

	pclsBool->m_bValue = m_bValue;

	return pclsBool;
}
