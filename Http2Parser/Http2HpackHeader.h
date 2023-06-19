

#ifndef _HTTP2_HPACK_HEADER_H_
#define _HTTP2_HPACK_HEADER_H_

#include "SipPlatformDefine.h"
#include <string>


class CHttp2HpackHeader
{
public:
	CHttp2HpackHeader();
	~CHttp2HpackHeader();

	int Parse( const uint8_t * pszBody, int iBodyLen );
	int ParseString( const uint8_t * pszBody, int iBodyLen, std::string & strOutput );
	int ParseInt( const uint8_t * pszBody, int iBodyLen, int iPrefixBit, int & iOutput );
	void Clear();

	int m_iIndex;
	std::string m_strName;
	std::string m_strValue;
	bool m_bIncrementalIndexing;

private:
	uint8_t	* m_pszBuf;
	int				m_iBufSize;
};

#endif
