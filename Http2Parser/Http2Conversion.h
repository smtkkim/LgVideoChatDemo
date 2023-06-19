

#ifndef _HTTP2_CONVERSION_H_
#define _HTTP2_CONVERSION_H_

#include "HttpMessage.h"
#include "Http2Header.h"
#include "Http2FrameList.h"
#include "Http2HpackHeaderList.h"


class CHttp2Conversion
{
public:
	CHttp2Conversion();
	~CHttp2Conversion();

	bool MakeFrameList( CHttpMessage & clsMessage, CHttp2FrameList & clsFrameList );
	bool MakeMessage( CHttp2Frame & clsFrame, CHttpMessage & clsMessage );

private:
	bool AddIndex( uint32_t iIndex );
	bool AddIndexValue( uint32_t iIndex, const char * pszValue );
	bool AddNameValue( const char * pszName, const char * pszValue );
	bool HpackToMessage( CHttp2HpackHeader & clsHpack, CHttpMessage & clsMessage );
	void HpackToString( CHttp2HpackHeader & clsHpack, std::string & strOutput, const char * pszDefault, const char * pszType );
	void HpackToInt( CHttp2HpackHeader & clsHpack, int & iOutput, int iDefault, const char * pszType );
	void HpackAddHeader( CHttp2HpackHeader & clsHpack, CHttpMessage & clsMessage, const char * pszName );

	CHttpMessage		* m_pclsMessage;
	CHttp2FrameList * m_pclsFrameList;
	CHttp2Header					m_clsHeader;
	CHttp2HpackHeaderList	m_clsHpackList;
	int						m_iHeaderFrameCount;
};

#endif
