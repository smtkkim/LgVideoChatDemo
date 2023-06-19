

#include "Http2HpackHeaderList.h"

CHttp2HpackHeaderList::CHttp2HpackHeaderList() : m_iMaxCount(4096)
{
}

CHttp2HpackHeaderList::~CHttp2HpackHeaderList()
{
}


bool CHttp2HpackHeaderList::Insert( CHttp2HpackHeader * pclsHpack )
{
	CHttp2HpackHeader clsHpack;
	HTTP2_HPACK_HEADER_LIST::iterator itHL;

	m_clsList.push_front( clsHpack );
	itHL = m_clsList.begin();
	if( itHL == m_clsList.end() )
	{
		return false;
	}

	itHL->m_iIndex = pclsHpack->m_iIndex;
	itHL->m_strName = pclsHpack->m_strName;
	itHL->m_strValue = pclsHpack->m_strValue;

	for( int iCount = (int)m_clsList.size(); iCount > m_iMaxCount; --iCount )
	{
		m_clsList.pop_back();
	}

	return true;
}


bool CHttp2HpackHeaderList::Select( int iIndex, CHttp2HpackHeader ** ppclsHpack )
{
	HTTP2_HPACK_HEADER_LIST::iterator itHL;
	int iCount = 62;

	for( itHL = m_clsList.begin(); itHL != m_clsList.end(); ++itHL )
	{
		if( iCount == iIndex )
		{
			*ppclsHpack = &(*itHL);
			return true;
		}

		++iCount;
	}

	return false;
}


void CHttp2HpackHeaderList::SetMaxCount( int iMaxCount )
{
	m_iMaxCount = iMaxCount;
}
