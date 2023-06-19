

#ifndef _SIP_MUTEX_H_
#define _SIP_MUTEX_H_

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif


class CSipMutex
{
public:
	CSipMutex();
	~CSipMutex();
	
	bool acquire();
	bool release();

protected:
#ifdef WIN32
	CRITICAL_SECTION m_sttMutex;
#else
	pthread_mutex_t	 m_sttMutex;
#endif
};


class CSipMutexSignal : public CSipMutex
{
public:
	CSipMutexSignal();
	~CSipMutexSignal();
	
	bool wait();
	bool signal();
	bool broadcast();

private:
#ifdef WIN32
	CONDITION_VARIABLE		m_sttCond;
#else
	pthread_cond_t		m_sttCond;
#endif
};

#endif
