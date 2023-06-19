

#ifndef _HTTP_PARAMETER_LIST_H_
#define _HTTP_PARAMETER_LIST_H_

#include "HttpParameter.h"


class CHttpParameterList
{
public:
	CHttpParameterList( char cSep = '&', bool bRemoveFrontSpace = false );
	~CHttpParameterList();

	int Parse( const char * pszText );
	int Parse( const std::string & strText );
	int Parse( const char * pszText, int iTextLen );

	int ParseUrl( const char * pszText );
	int ParseUrl( const std::string & strText );
	int ParseUrl( const char * pszText, int iTextLen );
	
	int ParseOne( const char * pszText, int iTextLen );
	int ToString( char * pszText, int iTextSize );

	bool Insert( const char * pszName, const char * pszValue );
	bool Update( const char * pszName, const char * pszValue );
	bool Select( const char * pszName, std::string & strValue );
	bool Select( const char * pszName );
	const char * SelectValue( const char * pszName );

	void ClearParam();

	HTTP_PARAMETER_LIST m_clsParamList;

	
	char				m_cSep;

	
	bool				m_bRemoveFrontSpace;
};

#endif
