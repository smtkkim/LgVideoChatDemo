

#include "SipPlatformDefine.h"
#include "MonitorString.h"
#include "TimeString.h"
#include "MemoryDebug.h"

CMonitorString::CMonitorString()
{
}

CMonitorString::~CMonitorString()
{
}


void CMonitorString::AddCol( const char * pszValue )
{
	m_strBuf.append( pszValue );
	m_strBuf.append( MR_COL_SEP );
}


void CMonitorString::AddCol( const std::string & strValue )
{
	m_strBuf.append( strValue );
	m_strBuf.append( MR_COL_SEP );
}


void CMonitorString::AddCol( const std::string & strIp, int iPort )
{
	m_strBuf.append( strIp );
	m_strBuf.append( ":" );
	AddCol( iPort );
}


void CMonitorString::AddCol( int iValue )
{
	char	szValue[21];

	snprintf( szValue, sizeof(szValue), "%d", iValue );
	AddCol( szValue );
}


void CMonitorString::AddCol( time_t iTime )
{
	char	szTime[21];

	GetDateTimeString( iTime, szTime, sizeof(szTime) );
	AddCol( szTime );
}


void CMonitorString::AddRow( const char * pszValue )
{
	m_strBuf.append( pszValue );
	m_strBuf.append( MR_ROW_SEP );
}


void CMonitorString::AddRow( const std::string & strValue )
{
	m_strBuf.append( strValue );
	m_strBuf.append( MR_ROW_SEP );
}


void CMonitorString::AddRow( const std::string & strIp, int iPort )
{
	m_strBuf.append( strIp );
	m_strBuf.append( ":" );
	AddRow( iPort );
}


void CMonitorString::AddRow( int iValue )
{
	char	szValue[21];

	snprintf( szValue, sizeof(szValue), "%d", iValue );
	AddRow( szValue );
}


void CMonitorString::AddRow( time_t iTime )
{
	char	szTime[21];

	GetDateTimeString( iTime, szTime, sizeof(szTime) );
	AddRow( szTime );
}


void CMonitorString::Clear( )
{
	m_strBuf.clear();
}


const char * CMonitorString::GetString()
{
	return m_strBuf.c_str();
}


int CMonitorString::GetLength()
{
	return (int)m_strBuf.length();
}
