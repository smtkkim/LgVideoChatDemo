

#include "HttpUri.h"
#include <stdlib.h>
#include <string.h>

CHttpUri::CHttpUri() : m_iPort(80)
{
}

CHttpUri::~CHttpUri()
{
}


int CHttpUri::Parse( const char * pszText, int iTextLen )
{
	char cType = 0;
	int iStartPos = 0;

	Clear();

	for( int i = 0; i < iTextLen; ++i )
	{
		if( pszText[i] == ':' )
		{
			if( cType == 0 )
			{
				if( (i+2) < iTextLen && pszText[i+1] == '/' && pszText[i+2] == '/' )
				{
					m_strProtocol.append( pszText, i );
					++cType;
					i += 2;
					iStartPos = i + 1;

					if( !strcmp( m_strProtocol.c_str(), "https" ) )
					{
						m_iPort = 443;
					}
				}
				else
				{
					return -1;
				}
			}
			else if( cType == 1 )
			{
				m_strHost.append( pszText + iStartPos, i - iStartPos );
				++cType;
				iStartPos = i + 1;
			}
		}
		else if( pszText[i] == '/' )
		{
			if( cType == 1 )
			{
				m_strHost.append( pszText + iStartPos, i - iStartPos );
				m_strPath.append( pszText + i );
				return i;
			}
			else if( cType == 2 )
			{
				std::string strTemp;

				strTemp.append( pszText + iStartPos, i - iStartPos );
				m_iPort = atoi( strTemp.c_str() );
				m_strPath.append( pszText + i );
				return i;
			}
		}
	}

	if( cType == 1 )
	{
		m_strHost.append( pszText + iStartPos );
		return iTextLen;
	}

	return -1;
}


void CHttpUri::Clear()
{
	m_strProtocol.clear();
	m_strHost.clear();
	m_iPort = 80;
	m_strPath.clear();
}
