

#include "Http2FrameList.h"
#include "Log.h"
#include "MemoryDebug.h"

CHttp2FrameList::CHttp2FrameList()
{
}

CHttp2FrameList::~CHttp2FrameList()
{
	DeleteAll();
}


CHttp2Frame * CHttp2FrameList::CreateFrame()
{
	CHttp2Frame * pclsFrame = NULL;

	if( m_clsNotUseList.empty() == false )
	{
		pclsFrame = *(m_clsNotUseList.begin());
		m_clsNotUseList.pop_front();
	}
	else
	{
		pclsFrame = new CHttp2Frame();
		if( pclsFrame == NULL )
		{
			CLog::Print( LOG_ERROR, "%s new CHttp2Frame() error", __FUNCTION__ );
			return NULL;
		}
	}

	m_clsList.push_back( pclsFrame );

	return pclsFrame;
}


void CHttp2FrameList::Clear()
{
	HTTP2_FRAME_LIST::iterator itFL;

	for( itFL = m_clsList.begin(); itFL != m_clsList.end(); ++itFL )
	{
		(*itFL)->Clear();
		m_clsNotUseList.push_back( *itFL );
	}

	m_clsList.clear();
}


void CHttp2FrameList::DeleteAll()
{
	HTTP2_FRAME_LIST::iterator itFL;

	for( itFL = m_clsList.begin(); itFL != m_clsList.end(); ++itFL )
	{
		delete *itFL;
	}
	m_clsList.clear();

	for( itFL = m_clsNotUseList.begin(); itFL != m_clsNotUseList.end(); ++itFL )
	{
		delete *itFL;
	}
	m_clsNotUseList.clear();
}
