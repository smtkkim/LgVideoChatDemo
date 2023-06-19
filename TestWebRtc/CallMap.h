

#ifndef _CALL_MAP_H_
#define _CALL_MAP_H_

#include <string>
#include <map>
#include "SipMutex.h"

// key = caller user id, value = callee user id
typedef std::map< std::string, std::string > CALL_MAP;


class CCallMap
{
public:
	CCallMap();
	~CCallMap();

	bool Insert( const char * pszCaller, const char * pszCallee );
	bool Select( const char * pszUserId, std::string & strOtherId );
	bool Delete( const char * pszUserId );

private:
	CALL_MAP	m_clsMap;
	CSipMutex	m_clsMutex;
};

extern CCallMap gclsCallMap;

#endif
