

#ifndef _WEB_SOCKET_PACKET_H_
#define _WEB_SOCKET_PACKET_H_

#include "SipPlatformDefine.h"
#include <string>


class CWebSocketPacketHeader
{
public:
	uint8_t	m_iOpCode;
};


class CWebSocketPacket
{
public:
	CWebSocketPacket();
	~CWebSocketPacket();

	bool AddPacket( const char * pszPacket, int iPacketLen );
	bool GetData( CWebSocketPacketHeader & clsHeader, std::string & strData );

private:
	std::string m_strBuf;
};

#endif
