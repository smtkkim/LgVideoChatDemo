

#ifndef _JSON_NULL_H_
#define _JSON_NULL_H_

#include "JsonType.h"

class CJsonNull : public CJsonType
{
public:
	CJsonNull();
	virtual ~CJsonNull();

	virtual int Parse( const char * pszText, int iTextLen );
	virtual int ToString( std::string & strText, EJsonNewLine eNewLine = E_JNL_NULL, int iDepth = 0 );
	virtual int GetStringLen( );
	virtual CJsonType * Copy( );
};

#endif
