

#ifndef _JSON_TYPE_H_
#define _JSON_TYPE_H_

#include "SipPlatformDefine.h"
#include <string>

#define JSON_TYPE_STRING	0x01
#define JSON_TYPE_NUMBER	0x02
#define JSON_TYPE_INT			0x12
#define JSON_TYPE_DOUBLE	0x22
#define JSON_TYPE_OBJECT	0x03
#define JSON_TYPE_ARRAY		0x04
#define JSON_TYPE_BOOL		0x05
#define JSON_TYPE_NULL		0x00

enum EJsonNewLine
{
	E_JNL_NULL = 0,
	E_JNL_ALL,
	E_JNL_TOP
};


class CJsonType
{
public:
	CJsonType();
	virtual ~CJsonType();

	virtual int Parse( const char * pszText, int iTextLen ) = 0;
	virtual int ToString( std::string & strText, EJsonNewLine eNewLine = E_JNL_NULL, int iDepth = 0 ) = 0;
	virtual int GetStringLen( ) = 0;
	virtual CJsonType * Copy( ) = 0;

	const char * GetTypeString();

	uint8_t		m_cType;

	static std::string	m_strNewLine;
};

#endif
