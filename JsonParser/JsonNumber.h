

#ifndef _JSON_NUMBER_H_
#define _JSON_NUMBER_H_

#include "JsonType.h"


class CJsonNumber : public CJsonType
{
public:
	CJsonNumber();
	virtual ~CJsonNumber();

	virtual int Parse( const char * pszText, int iTextLen );
	virtual int ToString( std::string & strText, EJsonNewLine eNewLine = E_JNL_NULL, int iDepth = 0 );
	virtual int GetStringLen( );
	virtual CJsonType * Copy( );

	bool IsDouble( );

	std::string m_strValue;
};

#endif
