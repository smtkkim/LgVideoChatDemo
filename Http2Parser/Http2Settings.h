

#ifndef _HTTP2_SETTINGS_H_
#define _HTTP2_SETTINGS_H_

#include "SipPlatformDefine.h"
#include "Log.h"


class CHttp2Settings
{
public:
	CHttp2Settings();
	~CHttp2Settings();

	bool Add( uint16_t iName, uint32_t iValue );
	void Clear();
	void Delete();

	static void PrintLog( EnumLogLevel eLogLevel, uint8_t * pszPacket, int iPacketLen );

	uint8_t	* m_pszPacket;
	int m_iPacketLen;

private:
	int m_iPacketSize;
};

#endif
