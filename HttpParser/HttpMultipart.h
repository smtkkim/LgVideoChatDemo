

#ifndef _HTTP_MULTIPART_H_
#define _HTTP_MULTIPART_H_

#include "StringUtility.h"
#include <map>

class CHttpMultipartData
{
public:
	std::string m_strContentType;
	std::string m_strFileName;
	std::string m_strValue;
};

typedef std::map< std::string, CHttpMultipartData * > HTTP_MULTIPART_DATA_MAP;

class CHttpMultipart
{
public:
	CHttpMultipart();
	~CHttpMultipart();

	void SetBoundary( const char * pszBoundary );
	void SetContentType( const char * pszContentType );
	void Clear();

	int Parse( const std::string & strText );
	int Parse( const char * pszText, int iTextLen );
	int ToString( std::string & strText );

private:
	int ParseData( const char * pszText, int iTextLen );

	std::string m_strBoundary;

public:
	HTTP_MULTIPART_DATA_MAP	m_clsMap;
};

#endif
