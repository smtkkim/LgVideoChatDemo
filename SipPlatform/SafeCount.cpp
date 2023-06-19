

#include "SafeCount.h"

CSafeCount::CSafeCount() : m_iCount(0)
{
}

CSafeCount::~CSafeCount()
{
}


void CSafeCount::Increase()
{
	m_clsMutex.acquire();
	++m_iCount;
	m_clsMutex.release();
}


void CSafeCount::Decrease()
{
	m_clsMutex.acquire();
	--m_iCount;
	m_clsMutex.release();
}


void CSafeCount::SetCount( int iCount )
{
	m_clsMutex.acquire();
	m_iCount = iCount;
	m_clsMutex.release();
}


int CSafeCount::GetCount()
{
	int iCount;

	m_clsMutex.acquire();
	iCount = m_iCount;
	m_clsMutex.release();

	return iCount;
}
