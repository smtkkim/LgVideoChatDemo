

#include "Http2Define.h"
#include "Http2Settings.h"
#include "SipTcp.h"
#include <stdlib.h>
#include "MemoryDebug.h"

CHttp2Settings::CHttp2Settings() : m_pszPacket(NULL), m_iPacketLen(0), m_iPacketSize(0)
{
}

CHttp2Settings::~CHttp2Settings()
{
	Delete();
}


bool CHttp2Settings::Add( uint16_t iName, uint32_t iValue )
{
	if( m_pszPacket == NULL )
	{
		m_pszPacket = (uint8_t *)malloc( 6 * 10 );
		if( m_pszPacket == NULL ) return false;

		m_iPacketSize = 60;
	}

	if( m_iPacketLen + 6 > m_iPacketSize ) return false;

	iName = htons( iName );
	iValue = htonl( iValue );

	memcpy( m_pszPacket + m_iPacketLen, &iName, 2 );
	memcpy( m_pszPacket + m_iPacketLen + 2, &iValue, 4 );
	m_iPacketLen += 6;

	return true;
}


void CHttp2Settings::Clear()
{
	m_iPacketLen = 0;
}


void CHttp2Settings::Delete()
{
	if( m_pszPacket )
	{
		free( m_pszPacket );
		m_pszPacket = NULL;
	}

	m_iPacketSize = 0;
	m_iPacketLen = 0;
}


void CHttp2Settings::PrintLog( EnumLogLevel eLogLevel, uint8_t * pszPacket, int iPacketLen )
{
	uint16_t sId;
	uint32_t iValue;
	char szText[1024];
	int iLen = 0;

	for( int i = 0; i < iPacketLen; i += 6 )
	{
		memcpy( &sId, pszPacket + i, 2 );
		memcpy( &iValue, pszPacket + i + 2, 4 );

		sId = ntohs( sId );
		iValue = ntohl( iValue );

		switch( sId )
		{
		case HTTP2_SETTINGS_HEADER_TABLE_SIZE:
			iLen += snprintf( szText + iLen, sizeof(szText) - iLen, "HEADER_TABLE_SIZE(%u) ", iValue );
			break;
		case HTTP2_SETTINGS_ENABLE_PUSH:
			iLen += snprintf( szText + iLen, sizeof(szText) - iLen, "ENABLE_PUSH(%u) ", iValue );
			break;
		case HTTP2_SETTINGS_MAX_CONCURRENT_STREAMS:
			iLen += snprintf( szText + iLen, sizeof(szText) - iLen, "MAX_CONCURRENT_STREAMS(%u) ", iValue );
			break;
		case HTTP2_SETTINGS_INITIAL_WINDOW_SIZE:
			iLen += snprintf( szText + iLen, sizeof(szText) - iLen, "INITIAL_WINDOW_SIZE(%u) ", iValue );
			break;
		case HTTP2_SETTINGS_MAX_FRAME_SIZE:
			iLen += snprintf( szText + iLen, sizeof(szText) - iLen, "MAX_FRAME_SIZE(%u) ", iValue );
			break;
		case HTTP2_SETTINGS_MAX_HEADER_LIST_SIZE:
			iLen += snprintf( szText + iLen, sizeof(szText) - iLen, "MAX_HEADER_LIST_SIZE(%u) ", iValue );
			break;
		case HTTP2_SETTINGS_ACCEPT_CACHE_DIGEST:
			iLen += snprintf( szText + iLen, sizeof(szText) - iLen, "ACCEPT_CACHE_DIGEST(%u) ", iValue );
			break;
		case HTTP2_SETTINGS_ENABLE_CONNECT_PROTOCOL:
			iLen += snprintf( szText + iLen, sizeof(szText) - iLen, "ENABLE_CONNECT_PROTOCOL(%u) ", iValue );
			break;
		}
	}

	if( iLen > 0 )
	{
		CLog::Print( eLogLevel, "Settings %s", szText );
	}
}
