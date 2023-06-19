

#include "SipPlatformDefine.h"
#include "JsonObject.h"
#include "Log.h"
#include "MemoryDebug.h"

CJsonArray::CJsonArray()
{
	m_cType = JSON_TYPE_ARRAY;
}

CJsonArray::~CJsonArray()
{
	Clear();
}


int CJsonArray::Parse( const char * pszText, int iTextLen )
{
	int iPos = -1, iParseLen;
	uint8_t	cType = 0;
	std::string	strName;

	Clear();

	for( int i = 0; i < iTextLen; ++i )
	{
		if( pszText[i] == '[' )
		{
			iPos = i + 1;
			break;
		}
	}

	if( iPos == -1 ) 
	{
		CLog::Print( LOG_ERROR, "%s [ is not found", __FUNCTION__ );
		return -1;
	}

	for( int i = iPos; i < iTextLen; ++i )
	{
		if( cType == 0 )
		{
			CJsonType * pclsType = CJsonObject::GetJsonType( pszText, iTextLen, i );

			if( pclsType )
			{
				iParseLen = pclsType->Parse( pszText + i, iTextLen - i );
				if( iParseLen == -1 )
				{
					CLog::Print( LOG_ERROR, "%s json parse error", __FUNCTION__ );
					delete pclsType;
					return -1;
				}

				m_clsList.push_back( pclsType );
				cType = 1;
				i += iParseLen - 1;
			}
			else if( pszText[i] == ']' )
			{
				iPos = i + 1;
				break;
			}
		}
		else if( pszText[i] == ',' )
		{
			cType = 0;
		}
		else if( pszText[i] == ']' )
		{
			iPos = i + 1;
			break;
		}
	}

	return iPos;
}


int CJsonArray::ToString( std::string & strText, EJsonNewLine eNewLine, int iDepth )
{
	JSON_LIST::iterator itJL;
	std::string strBuf;

	if( m_clsList.empty() )
	{
		strBuf.append( "[ ]" );
	}
	else
	{
		strBuf.append( "[" );

		if( eNewLine != E_JNL_NULL ) strBuf.append( m_strNewLine );

		for( itJL = m_clsList.begin(); itJL != m_clsList.end(); ++itJL )
		{
			if( itJL == m_clsList.begin() )
			{
				if( eNewLine == E_JNL_NULL ) strBuf.append( " " );
			}
			else
			{
				if( eNewLine != E_JNL_NULL ) 
				{
					strBuf.append( "," );
					strBuf.append( m_strNewLine );
				}
				else
				{
					strBuf.append( ", " );
				}
			}

			if( eNewLine != E_JNL_NULL ) 
			{
				CJsonObject::AddTab( strBuf, iDepth + 1 );
			}

			CJsonObject::JsonToString( *itJL, strBuf, eNewLine == E_JNL_TOP ? E_JNL_NULL : eNewLine, iDepth + 1 );
		}

		if( eNewLine != E_JNL_NULL )
		{
			strBuf.append( m_strNewLine );
			CJsonObject::AddTab( strBuf, iDepth );
			strBuf.append( "]" );
		}
		else
		{
			strBuf.append( " ]" );
		}
	}

	strText.append( strBuf );

	return (int)strBuf.length();
}


int CJsonArray::GetStringLen( )
{
	JSON_LIST::iterator itJL;
	int iLen = 3;

	for( itJL = m_clsList.begin(); itJL != m_clsList.end(); ++itJL )
	{
		if( itJL == m_clsList.begin() )
		{
			iLen += 1;
		}
		else
		{
			iLen += 2;
		}

		iLen += (*itJL)->GetStringLen();
	}

	return iLen;
}


CJsonType * CJsonArray::Copy( )
{
	CJsonArray * pclsArray = new CJsonArray();
	if( pclsArray == NULL ) return NULL;

	JSON_LIST::iterator itJL;

	for( itJL = m_clsList.begin(); itJL != m_clsList.end(); ++itJL )
	{
		CJsonType * pclsType = (*itJL)->Copy();
		if( pclsType == NULL )
		{
			delete pclsArray;
			return NULL;
		}

		pclsArray->m_clsList.push_back( pclsType );
	}

	return pclsArray;
}


int CJsonArray::MakeString( std::string & strText, EJsonNewLine eNewLine )
{
	strText.clear();

	return ToString( strText, eNewLine );
}


void CJsonArray::Clear()
{
	JSON_LIST::iterator itJL;

	for( itJL = m_clsList.begin(); itJL != m_clsList.end(); ++itJL )
	{
		delete *itJL;
	}

	m_clsList.clear();
}


bool CJsonArray::SelectData( int iIndex, std::string & strValue )
{
	CJsonType * pclsType;

	if( SelectData( iIndex, &pclsType ) == false ) return false;
	if( pclsType->m_cType != JSON_TYPE_STRING )
	{
		if( pclsType->m_cType != JSON_TYPE_NULL )
		{
			CLog::Print( LOG_ERROR, "%s type(%d) is not string (%s)", __FUNCTION__, iIndex, pclsType->GetTypeString() );
		}

		return false;
	}

	strValue = ((CJsonString *)pclsType)->m_strValue;

	return true;
}


bool CJsonArray::SelectData( int iIndex, int64_t & iValue )
{
	CJsonType * pclsType;

	if( SelectData( iIndex, &pclsType ) == false ) return false;
	if( pclsType->m_cType != JSON_TYPE_INT )
	{
		if( pclsType->m_cType != JSON_TYPE_NULL )
		{
			CLog::Print( LOG_ERROR, "%s type(%d) is not int (%s)", __FUNCTION__, iIndex, pclsType->GetTypeString() );
		}

		return false;
	}

	iValue = ((CJsonInt *)pclsType)->m_iValue;

	return true;
}


bool CJsonArray::SelectData( int iIndex, bool & bValue )
{
	CJsonType * pclsType;

	if( SelectData( iIndex, &pclsType ) == false ) return false;
	if( pclsType->m_cType != JSON_TYPE_BOOL )
	{
		if( pclsType->m_cType != JSON_TYPE_NULL )
		{
			CLog::Print( LOG_ERROR, "%s type(%d) is not bool (%s)", __FUNCTION__, iIndex, pclsType->GetTypeString() );
		}

		return false;
	}

	bValue = ((CJsonBool *)pclsType)->m_bValue;

	return true;
}


bool CJsonArray::SelectData( int iIndex, CJsonObject ** ppclsObject )
{
	CJsonType * pclsType;

	if( SelectData( iIndex, &pclsType ) == false ) return false;
	if( pclsType->m_cType != JSON_TYPE_OBJECT )
	{
		if( pclsType->m_cType != JSON_TYPE_NULL )
		{
			CLog::Print( LOG_ERROR, "%s type(%d) is not object (%s)", __FUNCTION__, iIndex, pclsType->GetTypeString() );
		}

		return false;
	}

	*ppclsObject = (CJsonObject *)pclsType;

	return true;
}


bool CJsonArray::SelectData( int iIndex, CJsonArray ** ppclsArray )
{
	CJsonType * pclsType;

	if( SelectData( iIndex, &pclsType ) == false ) return false;
	if( pclsType->m_cType != JSON_TYPE_ARRAY )
	{
		if( pclsType->m_cType != JSON_TYPE_NULL )
		{
			CLog::Print( LOG_ERROR, "%s type(%d) is not array (%s)", __FUNCTION__, iIndex, pclsType->GetTypeString() );
		}

		return false;
	}

	*ppclsArray = (CJsonArray *)pclsType;

	return true;
}


bool CJsonArray::SelectData( int iIndex, CJsonType ** ppclsType )
{
	if( iIndex >= (int)m_clsList.size() ) 
	{
		CLog::Print( LOG_ERROR, "%s iIndex(%d) >= m_clsList.size(%d)", __FUNCTION__, iIndex, m_clsList.size() );
		return false;
	}

	JSON_LIST::iterator itJL;
	int iCount = 0;

	for( itJL = m_clsList.begin(); itJL != m_clsList.end(); ++itJL )
	{
		if( iIndex == iCount )
		{
			*ppclsType = *itJL;
			return true;
		}

		++iCount;
	}

	return false;
}


bool CJsonArray::InsertData( std::string & strValue )
{
	return InsertData( strValue.c_str() );
}


bool CJsonArray::InsertData( const char * pszValue )
{
	CJsonString * pclsNew = new CJsonString();
	if( pclsNew == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	pclsNew->m_strValue = pszValue;
	m_clsList.push_back( pclsNew );

	return true;
}


bool CJsonArray::InsertData( int32_t iValue )
{
	return InsertData( (int64_t)iValue );
}


bool CJsonArray::InsertData( int64_t iValue )
{
	CJsonInt * pclsNew = new CJsonInt();
	if( pclsNew == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	pclsNew->m_iValue = iValue;
	m_clsList.push_back( pclsNew );

	return true;
}


bool CJsonArray::InsertData( bool bValue )
{
	CJsonBool * pclsNew = new CJsonBool();
	if( pclsNew == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	pclsNew->m_bValue = bValue;
	m_clsList.push_back( pclsNew );

	return true;
}


bool CJsonArray::InsertData( CJsonType * pclsType )
{
	CJsonType * pclsNew = pclsType->Copy();
	if( pclsNew == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	m_clsList.push_back( pclsNew );

	return true;
}


bool CJsonArray::InsertData( )
{
	CJsonNull * pclsNew = new CJsonNull();
	if( pclsNew == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	m_clsList.push_back( pclsNew );

	return true;
}


bool CJsonArray::InsertData( int iIndex, std::string & strValue )
{
	return InsertData( iIndex, strValue.c_str() );
}


bool CJsonArray::InsertData( int iIndex, const char * pszValue )
{
	CJsonString * pclsNew = new CJsonString();
	if( pclsNew == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	pclsNew->m_strValue = pszValue;

	if( InsertDataNoCopy( iIndex, pclsNew ) == false )
	{
		delete pclsNew;
		return false;
	}

	return true;
}


bool CJsonArray::InsertData( int iIndex, int32_t iValue )
{
	return InsertData( iIndex, (int64_t)iValue );
}


bool CJsonArray::InsertData( int iIndex, int64_t iValue )
{
	CJsonInt * pclsNew = new CJsonInt();
	if( pclsNew == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	pclsNew->m_iValue = iValue;

	if( InsertDataNoCopy( iIndex, pclsNew ) == false )
	{
		delete pclsNew;
		return false;
	}

	return true;
}


bool CJsonArray::InsertData( int iIndex, bool bValue )
{
	CJsonBool * pclsNew = new CJsonBool();
	if( pclsNew == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	pclsNew->m_bValue = bValue;

	if( InsertDataNoCopy( iIndex, pclsNew ) == false )
	{
		delete pclsNew;
		return false;
	}

	return true;
}


bool CJsonArray::InsertData( int iIndex, CJsonType * pclsType )
{
	CJsonType * pclsNew = pclsType->Copy();
	if( pclsNew == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	if( InsertDataNoCopy( iIndex, pclsNew ) == false )
	{
		delete pclsNew;
		return false;
	}

	return true;
}


bool CJsonArray::InsertDataNull( int iIndex )
{
	CJsonNull * pclsNew = new CJsonNull();
	if( pclsNew == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	if( InsertDataNoCopy( iIndex, pclsNew ) == false )
	{
		delete pclsNew;
		return false;
	}

	return true;
}


bool CJsonArray::InsertArray( CJsonArray * pclsArray )
{
	int iCount = pclsArray->GetCount();
	CJsonType * pclsData;
		
	for( int i = 0; i < iCount; ++i )
	{
		if( pclsArray->SelectData( i, &pclsData ) )
		{
			InsertData( pclsData );
		}
	}

	return true;
}


bool CJsonArray::DeleteData( int iIndex )
{
	if( iIndex >= (int)m_clsList.size() ) 
	{
		CLog::Print( LOG_ERROR, "%s iIndex(%d) >= m_clsList.size(%d)", __FUNCTION__, iIndex, m_clsList.size() );
		return false;
	}

	JSON_LIST::iterator itJL;
	int iCount = 0;

	for( itJL = m_clsList.begin(); itJL != m_clsList.end(); ++itJL )
	{
		if( iIndex == iCount )
		{
			delete *itJL;
			m_clsList.erase( itJL );
			return true;
		}

		++iCount;
	}

	return false;
}


int CJsonArray::GetCount()
{
	return (int)m_clsList.size();
}


bool CJsonArray::InsertDataNoCopy( int iIndex, CJsonType * pclsType )
{
	int iSize = (int)m_clsList.size();

	if( iIndex > iSize )
	{
		CLog::Print( LOG_ERROR, "%s iIndex(%d) > m_clsList.size(%d)", __FUNCTION__, iIndex, iSize );
		return false;
	}

	if( iIndex == iSize )
	{
		m_clsList.push_back( pclsType );
		return true;
	}

	JSON_LIST::iterator itJL;
	int iCount = 0;

	for( itJL = m_clsList.begin(); itJL != m_clsList.end(); ++itJL )
	{
		if( iIndex == iCount )
		{
			m_clsList.insert( itJL, pclsType );
			return true;
		}

		++iCount;
	}

	return false;
}
