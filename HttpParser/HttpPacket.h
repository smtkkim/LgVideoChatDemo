

#ifndef _HTTP_PACKET_H_
#define _HTTP_PACKET_H_

#include "HttpMessage.h"


enum EHttpPacketStatus
{
	H_HPS_HEADER = 0,
	H_HPS_BODY,
	H_HPS_BODY_END
};


class CHttpPacket
{
public:
	CHttpPacket();
	~CHttpPacket();

	bool AddPacket( const char * pszPacket, int iPacketLen );
	bool IsCompleted();
	void ClearMessage();

	CHttpMessage * GetHttpMessage();

	std::string * GetBuf();

private:
	CHttpMessage m_clsMessage;
	std::string m_strBuf;
	int m_iChunkedLen;
	EHttpPacketStatus m_eStatus;

	bool ParseChunkedHeader( );
	bool ParseChunkedBody( );
};

#endif
