

#ifndef _HTTP_PARAMETER_H_
#define _HTTP_PARAMETER_H_

#include <string>
#include <list>


class CHttpParameter
{
public:
	CHttpParameter( char cSep = '&' );
	~CHttpParameter();

	
	std::string	m_strName;

	
	std::string	m_strValue;

	
	char				m_cSep;

	int Parse( const char * pszText, int iTextLen );
	int ToString( char * pszText, int iTextSize );
	void Clear();
};


typedef std::list< CHttpParameter > HTTP_PARAMETER_LIST;

#endif
