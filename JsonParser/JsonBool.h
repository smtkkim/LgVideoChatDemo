

#ifndef _JSON_BOOL_H_
#define _JSON_BOOL_H_

#include "JsonType.h"


class CJsonBool : public CJsonType
{
public:
	CJsonBool();
	virtual ~CJsonBool();

	virtual int Parse( const char * pszText, int iTextLen );
	virtual int ToString( std::string & strText, EJsonNewLine eNewLine = E_JNL_NULL, int iDepth = 0 );
	virtual int GetStringLen( );
	virtual CJsonType * Copy( );

	bool m_bValue;
};

#endif
