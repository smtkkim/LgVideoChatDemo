

#ifndef _HTTP_DOMAIN_COOKIE_H_
#define _HTTP_DOMAIN_COOKIE_H_

#include "HttpSetCookie.h"
#include <list>
#include <map>

class CHttpDomainCookiePath
{
public:
	std::string m_strPath;
	std::string m_strName;
	std::string m_strValue;
	time_t			m_iExpireTime;
	bool				m_bSecure;
};

typedef std::list< CHttpDomainCookiePath > HTTP_DOMAIN_COOKIE_PATH_LIST;

class CHttpDomainCookieInfo
{
public:
	bool Insert( CHttpSetCookie * pclsSetCookie );
	bool Select( const char * pszPath, std::string & strCookie );

	HTTP_DOMAIN_COOKIE_PATH_LIST m_clsList;
};

// key = domain or ip
typedef std::map< std::string, CHttpDomainCookieInfo > HTTP_DOMAIN_COOKIE_MAP;

class CHttpDomainCookie
{
public:
	bool Insert( CHttpSetCookie * pclsSetCookie );
	bool Select( const char * pszDomain, const char * pszPath, std::string & strCookie );
	bool IsEmpty( );

private:
	HTTP_DOMAIN_COOKIE_MAP m_clsMap;
};

#endif
