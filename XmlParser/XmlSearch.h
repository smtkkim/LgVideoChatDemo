

#ifndef _XML_SEARCH_H_
#define _XML_SEARCH_H_

#include "XmlElement.h"
#include <vector>

typedef std::vector< std::string > XML_NAME_LIST;


class CXmlSearch : public CXmlElement
{
public:
	CXmlSearch();
	~CXmlSearch();

	const char * SelectAttribute( const char * pszName, const char * pszAttrName, const int iIndex = 0 );
	bool SelectAttribute( const char * pszName, const char * pszAttrName, std::string & strValue, const int iIndex = 0 );
	bool SelectAttribute( const char * pszName, const char * pszAttrName, int & iValue, const int iIndex = 0 );
	bool SelectAttribute( const char * pszName, const char * pszAttrName, bool & bValue, const int iIndex = 0 );

	bool SelectElementData( const char * pszName, std::string & strData, const int iIndex = 0 );
	bool SelectElementData( const char * pszName, int & iData, const int iIndex = 0 );
	bool SelectElementData( const char * pszName, bool & bData, const int iIndex = 0 );

	bool SelectElementData( const char * pszName, const char * pszChildName, std::string & strData, const int iIndex = 0 );
	bool SelectElementData( const char * pszName, const char * pszChildName, int & iData, const int iIndex = 0 );
	bool SelectElementData( const char * pszName, const char * pszChildName, bool & bData, const int iIndex = 0 );

	bool SelectElementData( const int iIndex, std::string & strData, int iNameCount, ... );
	bool SelectElementData( const int iIndex, int & iData, int iNameCount, ... );
	bool SelectElementData( const int iIndex, bool & bData, int iNameCount, ... );

	CXmlElement * SelectElement( const char * pszName, const int iIndex = 0 );
	CXmlElement * SelectElement( const char * pszName, const char * pszChildName, const int iIndex = 0 );
	CXmlElement * SelectElement( const int iIndex, int iNameCount, ... );

private:
	CXmlElement * SelectElement( XML_ELEMENT_LIST * pclsList, const char * pszName, const int iIndex, int & iCount );
	CXmlElement * SelectElement( XML_ELEMENT_LIST * pclsList, const char * pszName, const char * pszChildName, const int iIndex, int & iCount );
	CXmlElement * SelectElement( XML_ELEMENT_LIST * pclsList, XML_NAME_LIST & clsNameList, int iNameIndex, const int iIndex, int & iCount );
};

#endif
