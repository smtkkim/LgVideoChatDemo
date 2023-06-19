

#ifndef _XML_ELEMENT_H_
#define _XML_ELEMENT_H_

#include "SipPlatformDefine.h"
#include <string>
#include <list>
#include <map>

class CXmlElement;


typedef std::map< std::string, std::string > XML_ATTRIBUTE_MAP;


typedef std::list< CXmlElement > XML_ELEMENT_LIST;




class CXmlElement
{
public:
	CXmlElement( const char * pszName = NULL );
	~CXmlElement();

	int Parse( const char * pszText, int iTextLen );
	int Parse( std::string & strText );

	int ToString( char * pszText, int iTextSize, bool bUseTab = false, int iDepth = 0 );
	void ToString( std::string & strText, bool bUseTab = false, int iDepth = 0 );
	void Clear( );

	bool ParseFile( const char * pszFileName );
	bool WriteFile( const char * pszFileName, bool bUseTab );

	const char * SelectAttribute( const char * pszName );
	const char * SelectAttributeTrim( const char * pszName );
	bool SelectAttribute( const char * pszName, std::string & strValue );
	bool SelectAttributeTrim( const char * pszName, std::string & strValue );
	bool SelectAttribute( const char * pszName, int & iValue );
	bool SelectAttribute( const char * pszName, int64_t & iValue );
	bool SelectAttribute( const char * pszName, bool & bValue );
	bool SelectAttribute( const char * pszName, double & dbValue );

	CXmlElement * SelectElement( const char * pszName, const int iIndex = 0 );
	CXmlElement * SelectElement( const int iIndex );
	bool SelectElementList( const char * pszName, XML_ELEMENT_LIST & clsList );

	const char * GetElementData( const char * pszName, const int iIndex = 0 );
	const char * GetElementDataTrim( const char * pszName, const int iIndex = 0 );
	bool SelectElementData( const char * pszName, std::string & strData, const int iIndex = 0 );
	bool SelectElementTrimData( const char * pszName, std::string & strData, const int iIndex = 0 );
	bool SelectElementData( const char * pszName, int & iData, const int iIndex = 0 );
	bool SelectElementData( const char * pszName, int64_t & iData, const int iIndex = 0 );
	bool SelectElementData( const char * pszName, bool & bData, const int iIndex = 0 );
	bool SelectElementData( const char * pszName, double & dbData, const int iIndex = 0 );

	const char * GetName();
	const char * GetData();

	bool IsDataEmpty();

	void SetName( const char * pszName );
	void SetData( const char * pszData );

	void InsertElementData( const char * pszName, CXmlElement * pclsElement );
	void InsertElementData( const char * pszName, const char * pszData );
	void InsertElementData( const char * pszName, std::string & strData );
	void InsertElementData( const char * pszName, int iData );
	void InsertElementData( const char * pszName, int64_t iData );
	void InsertElementData( const char * pszName, bool bData );
	void InsertElementData( const char * pszName, double dbData );
	void InsertElement( CXmlElement * pclsElement );

	bool UpdateElementData( const char * pszName, const char * pszData, const int iIndex = 0 );
	bool UpdateElementData( const char * pszName, std::string & strData, const int iIndex = 0 );
	bool UpdateElementData( const char * pszName, int iData, const int iIndex = 0 );
	bool UpdateElementData( const char * pszName, int64_t iData, const int iIndex = 0 );
	bool UpdateElementData( const char * pszName, bool bData, const int iIndex = 0 );
	bool UpdateElementData( const char * pszName, double dbData, const int iIndex = 0 );

	void InsertAttribute( const char * pszName, const char * pszValue );
	void InsertAttribute( const char * pszName, std::string & strValue );
	void InsertAttribute( const char * pszName, int iValue );
	void InsertAttribute( const char * pszName, int64_t iValue );
	void InsertAttribute( const char * pszName, bool bValue );
	void InsertAttribute( const char * pszName, double dbValue );

	XML_ELEMENT_LIST * GetElementList();

protected:
	std::string	m_strName;
	std::string m_strData;

	XML_ATTRIBUTE_MAP	m_clsAttributeMap;
	XML_ELEMENT_LIST	m_clsElementList;

	bool GetBoolean( const char * pszData );
};

#endif
