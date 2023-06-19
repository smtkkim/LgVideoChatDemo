

#ifndef _JSON_OBJECT_H_
#define _JSON_OBJECT_H_

#include "JsonString.h"
#include "JsonNumber.h"
#include "JsonInt.h"
#include "JsonArray.h"
#include "JsonBool.h"
#include "JsonNull.h"
#include <map>

typedef std::map< std::string, CJsonType * > JSON_OBJECT_MAP;

class CJsonArray;




class CJsonObject : public CJsonType
{
public:
	CJsonObject();
	virtual ~CJsonObject();

	virtual int Parse( const char * pszText, int iTextLen );
	virtual int ToString( std::string & strText, EJsonNewLine eNewLine = E_JNL_NULL, int iDepth = 0 );
	virtual int GetStringLen( );
	virtual CJsonType * Copy( );

	int Parse( std::string & strText );
	int MakeString( std::string & strText, EJsonNewLine eNewLine = E_JNL_NULL );
	void Clear();

	bool SelectStringData( const char * pszName, std::string & strValue );

	bool SelectData( const char * pszName, std::string & strValue );
	bool SelectData( const char * pszName, int32_t & iValue );
	bool SelectData( const char * pszName, int64_t & iValue );
	bool SelectData( const char * pszName, bool & bValue );
	bool SelectData( const char * pszName, CJsonObject ** ppclsObject );
	bool SelectData( const char * pszName, CJsonArray ** ppclsArray );
	bool SelectData( const char * pszName, CJsonType ** ppclsType );

	bool InsertData( const char * pszName, const std::string & strValue );
	bool InsertData( const char * pszName, const char * pszValue );
	bool InsertData( const char * pszName, int32_t iValue );
	bool InsertData( const char * pszName, int64_t iValue );
	bool InsertData( const char * pszName, bool bValue );
	bool InsertData( const char * pszName, CJsonType * pclsType );
	bool InsertData( const char * pszName );

	bool InsertStringData( const char * pszName, int32_t iValue );
	bool InsertStringOrNullData( const char * pszName, std::string & strValue );
	bool InsertStringOrNullData( const char * pszName, const char * pszValue );

	bool UpdateData( const char * pszName, const std::string & strValue );
	bool UpdateData( const char * pszName, const char * pszValue );
	bool UpdateData( const char * pszName, int32_t iValue );
	bool UpdateData( const char * pszName, int64_t iValue );
	bool UpdateData( const char * pszName, bool bValue );
	bool UpdateData( const char * pszName, CJsonType * pclsType );
	bool UpdateData( const char * pszName );

	bool DeleteData( const char * pszName );

	bool Exist( const char * pszName );

	static CJsonType * GetJsonType( const char * pszText, int iTextLen, int iPos );
	static void JsonToString( CJsonType * pclsType, std::string & strText, EJsonNewLine eNewLine = E_JNL_NULL, int iDepth = 0 );
	static void AddTab( std::string & strText, int iDepth );

	JSON_OBJECT_MAP m_clsMap;
};

#endif
