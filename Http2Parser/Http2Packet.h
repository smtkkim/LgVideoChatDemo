

#ifndef _HTTP2_PACKET_H_
#define _HTTP2_PACKET_H_

#include "SipPlatformDefine.h"
#include "Http2Frame.h"
#include "HttpPacket.h"
#include <string>


class CHttp2Packet
{
public:
	CHttp2Packet();
	~CHttp2Packet();

	bool AddPacket( const uint8_t * pszPacket, int iPacketLen );
	bool AddPacket( CHttpPacket * pclsPacket );
	bool GetFrame( CHttp2Frame * pclsFrame );

private:
	std::string m_strBuf;
};

#endif
