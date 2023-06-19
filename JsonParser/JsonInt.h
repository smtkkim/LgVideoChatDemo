

#ifndef _JSON_INT_H_
#define _JSON_INT_H_

#include "SipPlatformDefine.h"
#include "JsonType.h"


class CJsonInt : public CJsonType
{
public:
	CJsonInt();
	virtual ~CJsonInt();

	virtual int Parse( const char * pszText, int iTextLen );
	virtual int ToString( std::string & strText, EJsonNewLine eNewLine = E_JNL_NULL, int iDepth = 0 );
	virtual int GetStringLen( );
	virtual CJsonType * Copy( );

	int64_t	m_iValue;
};

#endif
