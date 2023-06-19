

#ifndef _HTTP2_HPACK_HEADER_LIST_H_
#define _HTTP2_HPACK_HEADER_LIST_H_

#include "Http2HpackHeader.h"
#include <list>
#include <map>

typedef std::list< CHttp2HpackHeader > HTTP2_HPACK_HEADER_LIST;


class CHttp2HpackHeaderList
{
public:
	CHttp2HpackHeaderList();
	~CHttp2HpackHeaderList();

	bool Insert( CHttp2HpackHeader * pclsHpack );
	bool Select( int iIndex, CHttp2HpackHeader ** ppclsHpack );

	void SetMaxCount( int iMaxCount );

private:
	HTTP2_HPACK_HEADER_LIST m_clsList;
	int m_iMaxCount;
};

#endif
