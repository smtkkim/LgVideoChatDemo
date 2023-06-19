

#include "Http2Packet.h"
#include "SipTcp.h"

CHttp2Packet::CHttp2Packet()
{
}

CHttp2Packet::~CHttp2Packet()
{
}


bool CHttp2Packet::AddPacket( const uint8_t * pszPacket, int iPacketLen )
{
	if( pszPacket && iPacketLen > 0 )
	{
		m_strBuf.append( (char *)pszPacket, iPacketLen );
	}

	return true;
}


bool CHttp2Packet::AddPacket( CHttpPacket * pclsPacket )
{
	std::string * pstrBuf = pclsPacket->GetBuf();

	if( pstrBuf->empty() == false )
	{
		m_strBuf.append( *pstrBuf );
		pstrBuf->clear();
	}

	return true;
}


bool CHttp2Packet::GetFrame( CHttp2Frame * pclsFrame )
{
	int iBufLen = (int)m_strBuf.length();

	if( iBufLen < 9 ) return false;

	int iFrameLen = 0;

	memcpy( (char *)&iFrameLen + 1, m_strBuf.c_str(), 3 );
	iFrameLen = ntohl( iFrameLen );

	if( ( iFrameLen + 9 ) > iBufLen ) return false;
	
	bool bRes = pclsFrame->Set( (uint8_t *)m_strBuf.c_str(), iFrameLen + 9 );
	m_strBuf.erase( 0, iFrameLen + 9 );

	return bRes;
}
