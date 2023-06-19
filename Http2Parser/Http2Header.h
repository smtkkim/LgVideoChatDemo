

#ifndef _HTTP2_HEADER_H_
#define _HTTP2_HEADER_H_

#include "SipPlatformDefine.h"




class CHttp2Header
{
public:
	CHttp2Header();
	~CHttp2Header();

	bool AddIndex( uint32_t iIndex );
	bool AddIndexValue( uint32_t iIndex, const char * pszValue, bool bIndexing = false );
	bool AddNameValue( const char * pszName, const char * pszValue, bool bIndexing = false );
	void Clear();
	void Delete();

	uint8_t	* m_pszPacket;
	int m_iPacketLen;

private:
	bool Create();
	int AddInt( uint8_t cPrefix, uint8_t cPrefixBit, uint32_t iIndex );

	int m_iPacketSize;
};

#endif
