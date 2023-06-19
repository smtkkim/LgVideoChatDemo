

#include "SipPlatformDefine.h"
#include "JsonType.h"
#include "MemoryDebug.h"

std::string CJsonType::m_strNewLine = "\n";

CJsonType::CJsonType() : m_cType(0)
{
}

CJsonType::~CJsonType()
{
}

const char * CJsonType::GetTypeString()
{
	switch( m_cType )
	{
	case JSON_TYPE_STRING: return "string";
	case JSON_TYPE_NUMBER: return "number";
	case JSON_TYPE_INT   : return "int";
	case JSON_TYPE_DOUBLE: return "double";
	case JSON_TYPE_OBJECT: return "object";
	case JSON_TYPE_ARRAY : return "array";
	case JSON_TYPE_BOOL  : return "bool";
	case JSON_TYPE_NULL  : return "null";
	}

	return "";
}
