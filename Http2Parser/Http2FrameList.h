

#ifndef _HTTP2_FRAME_LIST_H_
#define _HTTP2_FRAME_LIST_H_

#include "Http2Frame.h"
#include <list>

typedef std::list< CHttp2Frame * > HTTP2_FRAME_LIST;


class CHttp2FrameList
{
public:
	CHttp2FrameList();
	~CHttp2FrameList();

	CHttp2Frame * CreateFrame();
	void Clear();
	void DeleteAll();

	HTTP2_FRAME_LIST m_clsList;

private:
	HTTP2_FRAME_LIST m_clsNotUseList;
};

#endif
