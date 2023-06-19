

#ifndef _JSON_STRING_H_
#define _JSON_STRING_H_

#include "JsonType.h"


class CJsonString : public CJsonType
{
public:
	CJsonString();
	virtual ~CJsonString();

	virtual int Parse( const char * pszText, int iTextLen );
	virtual int ToString( std::string & strText, EJsonNewLine eNewLine = E_JNL_NULL, int iDepth = 0 );
	virtual int GetStringLen( );
	virtual CJsonType * Copy( );

	std::string m_strValue;
};

#endif
