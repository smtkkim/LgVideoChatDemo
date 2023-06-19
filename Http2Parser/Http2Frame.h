

#ifndef _HTTP2_FRAME_H_
#define _HTTP2_FRAME_H_

#include "SipPlatformDefine.h"
#include "Log.h"


class CHttp2Frame
{
public:
	CHttp2Frame();
	~CHttp2Frame();

	bool Set( uint8_t cType, uint8_t cFlag, uint32_t iStreamIdentifier, uint8_t * pszBody, int iBodyLen );
	bool Set( uint8_t * pszPacket, int iPacketLen );
	bool SetFlags( uint8_t cFlag );
	void Clear();
	void Delete();

	uint8_t GetType();
	uint8_t GetFlags();
	uint32_t GetStreamIdentifier();
	uint8_t * GetBody();
	int GetBodyLen();

	void PrintLog( EnumLogLevel eLogLevel, const char * pszIp, int iPort, bool bSend );

	uint8_t	* m_pszPacket;
	int m_iPacketLen;

private:
	int m_iPacketSize;
};

#endif
