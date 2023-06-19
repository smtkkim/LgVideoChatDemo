

#ifndef _HTTP_URI_H_
#define _HTTP_URI_H_

#include <string>


class CHttpUri
{
public:
	CHttpUri();
	~CHttpUri();

	int Parse( const char * pszText, int iTextLen );
	void Clear();

	
	std::string m_strProtocol;

	
	std::string m_strHost;

	
	int					m_iPort;

	
	std::string m_strPath;
};

#endif
