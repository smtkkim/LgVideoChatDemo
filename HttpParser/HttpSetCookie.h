

#ifndef _HTTP_SET_COOKIE_H_
#define _HTTP_SET_COOKIE_H_

#include <string>

class CHttpSetCookie
{
public:
	CHttpSetCookie();
	~CHttpSetCookie();

	int Parse( const char * pszText, int iTextLen );
	void Clear();

	std::string m_strName;
	std::string m_strValue;
	std::string m_strDomain;
	std::string m_strPath;

	time_t	m_iExpireTime;
	bool		m_bSecure;
};

#endif
