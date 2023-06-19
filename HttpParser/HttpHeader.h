

#ifndef _HTTP_HEADER_H_
#define _HTTP_HEADER_H_

#include <string>
#include <list>


class CHttpHeader
{
public:
	CHttpHeader();
	CHttpHeader( const char * pszName, const char * pszValue );
	CHttpHeader( const char * pszName, std::string & strValue );
	~CHttpHeader();

	int Parse( const char * pszText, int iTextLen );
	int ToString( char * pszText, int iTextSize );

	void Set( const char * pszName, const char * pszValue );
	void Set( const char * pszName, std::string & strValue );
	void Clear();

	
	std::string	m_strName;

	
	std::string	m_strValue;
};

typedef std::list< CHttpHeader > HTTP_HEADER_LIST;

#endif
