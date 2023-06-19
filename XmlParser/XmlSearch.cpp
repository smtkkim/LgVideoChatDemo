

#include "XmlSearch.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define MAKE_NAME_LIST va_list ap; \
	char * pszTemp; \
	XML_NAME_LIST clsNameList; \
	va_start( ap, iNameCount ); \
	for( int i = 0; i < iNameCount ; ++i ) \
	{ \
		pszTemp = va_arg( ap, char * ); \
		if( pszTemp == NULL ) break; \
		clsNameList.push_back( pszTemp ); \
	} \
	va_end( ap );

CXmlSearch::CXmlSearch()
{
}

CXmlSearch::~CXmlSearch()
{
}


const char * CXmlSearch::SelectAttribute( const char * pszName, const char * pszAttrName, const int iIndex )
{
	CXmlElement * pclsElement = SelectElement( pszName, iIndex );
	if( pclsElement )
	{
		return pclsElement->SelectAttribute( pszAttrName );
	}

	return NULL;
}


bool CXmlSearch::SelectAttribute( const char * pszName, const char * pszAttrName, std::string & strValue, const int iIndex )
{
	CXmlElement * pclsElement = SelectElement( pszName, iIndex );
	if( pclsElement )
	{
		return pclsElement->SelectAttribute( pszAttrName, strValue );
	}

	return false;
}


bool CXmlSearch::SelectAttribute( const char * pszName, const char * pszAttrName, int & iValue, const int iIndex )
{
	CXmlElement * pclsElement = SelectElement( pszName, iIndex );
	if( pclsElement )
	{
		return pclsElement->SelectAttribute( pszAttrName, iValue );
	}

	return false;
}


bool CXmlSearch::SelectAttribute( const char * pszName, const char * pszAttrName, bool & bValue, const int iIndex )
{
	CXmlElement * pclsElement = SelectElement( pszName, iIndex );
	if( pclsElement )
	{
		return pclsElement->SelectAttribute( pszAttrName, bValue );
	}

	return false;
}


bool CXmlSearch::SelectElementData( const char * pszName, std::string & strData, const int iIndex )
{
	CXmlElement * pclsElement = SelectElement( pszName, iIndex );
	if( pclsElement )
	{
		strData = pclsElement->GetData();
		return true;
	}

	return false;
}


bool CXmlSearch::SelectElementData( const char * pszName, int & iData, const int iIndex )
{
	CXmlElement * pclsElement = SelectElement( pszName, iIndex );
	if( pclsElement )
	{
		iData = atoi( pclsElement->GetData() );

		return true;
	}

	return false;
}


bool CXmlSearch::SelectElementData( const char * pszName, bool & bData, const int iIndex )
{
	CXmlElement * pclsElement = SelectElement( pszName, iIndex );
	if( pclsElement )
	{
		bData = GetBoolean( pclsElement->GetData() );
		return true;
	}

	return false;
}


bool CXmlSearch::SelectElementData( const char * pszName, const char * pszChildName, std::string & strData, const int iIndex )
{
	CXmlElement * pclsElement = SelectElement( pszName, pszChildName, iIndex );
	if( pclsElement )
	{
		strData = pclsElement->GetData();
		return true;
	}

	return false;
}


bool CXmlSearch::SelectElementData( const char * pszName, const char * pszChildName, int & iData, const int iIndex )
{
	CXmlElement * pclsElement = SelectElement( pszName, pszChildName, iIndex );
	if( pclsElement )
	{
		iData = atoi( pclsElement->GetData() );
		return true;
	}

	return false;
}


bool CXmlSearch::SelectElementData( const char * pszName, const char * pszChildName, bool & bData, const int iIndex )
{
	CXmlElement * pclsElement = SelectElement( pszName, pszChildName, iIndex );
	if( pclsElement )
	{
		bData = GetBoolean( pclsElement->GetData() );
		return true;
	}

	return false;
}


bool CXmlSearch::SelectElementData( const int iIndex, std::string & strData, int iNameCount, ... )
{
	if( iIndex < 0 ) return false;
	if( iNameCount <= 0 ) return false;

	int iCount = 0;
	MAKE_NAME_LIST

	CXmlElement * pclsElement = SelectElement( &m_clsElementList, clsNameList, 0, iIndex, iCount );
	if( pclsElement )
	{
		strData = pclsElement->GetData();
		return true;
	}

	return false;
}


bool CXmlSearch::SelectElementData( const int iIndex, int & iData, int iNameCount, ... )
{
	if( iIndex < 0 ) return false;
	if( iNameCount <= 0 ) return false;

	int iCount = 0;
	MAKE_NAME_LIST

	CXmlElement * pclsElement = SelectElement( &m_clsElementList, clsNameList, 0, iIndex, iCount );
	if( pclsElement )
	{
		iData = atoi( pclsElement->GetData() );
		return true;
	}

	return false;
}


bool CXmlSearch::SelectElementData( const int iIndex, bool & bData, int iNameCount, ... )
{
	if( iIndex < 0 ) return false;
	if( iNameCount <= 0 ) return false;

	int iCount = 0;
	MAKE_NAME_LIST

	CXmlElement * pclsElement = SelectElement( &m_clsElementList, clsNameList, 0, iIndex, iCount );
	if( pclsElement )
	{
		bData = GetBoolean( pclsElement->GetData() );
		return true;
	}

	return false;
}


CXmlElement * CXmlSearch::SelectElement( const char * pszName, const int iIndex )
{
	int iCount = 0;

	if( iIndex < 0 ) return NULL;

	return SelectElement( &m_clsElementList, pszName, iIndex, iCount );
}



CXmlElement * CXmlSearch::SelectElement( const char * pszName, const char * pszChildName, const int iIndex )
{
	int iCount = 0;

	if( iIndex < 0 ) return NULL;

	return SelectElement( &m_clsElementList, pszName, pszChildName, iIndex, iCount );
}


CXmlElement * CXmlSearch::SelectElement( const int iIndex, int iNameCount, ... )
{
	if( iIndex < 0 ) return NULL;
	if( iNameCount <= 0 ) return NULL;

	int iCount = 0;
	MAKE_NAME_LIST

	return SelectElement( &m_clsElementList, clsNameList, 0, iIndex, iCount );
}


CXmlElement * CXmlSearch::SelectElement( XML_ELEMENT_LIST * pclsList, const char * pszName, const int iIndex, int & iCount )
{
	XML_ELEMENT_LIST::iterator	itEL;
	CXmlElement * pclsElement = NULL;

	for( itEL = pclsList->begin(); itEL != pclsList->end(); ++itEL )
	{
		if( !strcmp( pszName, itEL->GetName() ) )
		{
			if( iCount == iIndex )
			{
				return &(*itEL);
			}

			++iCount;
		}
		else
		{
			pclsElement = SelectElement( itEL->GetElementList(), pszName, iIndex, iCount );
			if( pclsElement )
			{
				return pclsElement;
			}
		}
	}

	return NULL;
}


CXmlElement * CXmlSearch::SelectElement( XML_ELEMENT_LIST * pclsList, const char * pszName, const char * pszChildName, const int iIndex, int & iCount )
{
	XML_ELEMENT_LIST::iterator	itEL;
	CXmlElement * pclsElement = NULL;

	for( itEL = pclsList->begin(); itEL != pclsList->end(); ++itEL )
	{
		if( !strcmp( pszName, itEL->GetName() ) )
		{
			pclsElement = SelectElement( itEL->GetElementList(), pszChildName, iIndex, iCount );
			if( pclsElement )
			{
				return pclsElement;
			}
		}
		else
		{
			pclsElement = SelectElement( itEL->GetElementList(), pszName, pszChildName, iIndex, iCount );
			if( pclsElement )
			{
				return pclsElement;
			}
		}
	}

	return NULL;
}


CXmlElement * CXmlSearch::SelectElement( XML_ELEMENT_LIST * pclsList, XML_NAME_LIST & clsNameList, int iNameIndex, const int iIndex, int & iCount )
{
	XML_ELEMENT_LIST::iterator	itEL;
	CXmlElement * pclsElement = NULL;

	int iNameSize = (int)clsNameList.size();
	const char * pszName = clsNameList[iNameIndex].c_str();

	for( itEL = pclsList->begin(); itEL != pclsList->end(); ++itEL )
	{
		if( !strcmp( pszName, itEL->GetName() ) )
		{
			if( iNameSize == ( iNameIndex + 1 ) )
			{
				if( iCount == iIndex )
				{
					return &(*itEL);
				}

				++iCount;
			}
			else
			{
				pclsElement = SelectElement( itEL->GetElementList(), clsNameList, iNameIndex + 1, iIndex, iCount );
				if( pclsElement )
				{
					return pclsElement;
				}
			}
		}
		else
		{
			pclsElement = SelectElement( itEL->GetElementList(), clsNameList, iNameIndex, iIndex, iCount );
			if( pclsElement )
			{
				return pclsElement;
			}
		}
	}

	return NULL;
}
