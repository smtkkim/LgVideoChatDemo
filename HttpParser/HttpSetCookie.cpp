

#include "SipPlatformDefine.h"
#include "HttpSetCookie.h"
#include "HttpParameterList.h"
#include "TimeString.h"
#include <stdlib.h>

CHttpSetCookie::CHttpSetCookie() : m_iExpireTime(0), m_bSecure(false)
{
}

CHttpSetCookie::~CHttpSetCookie()
{
}

int CHttpSetCookie::Parse( const char * pszText, int iTextLen )
{
	Clear();

	CHttpParameterList clsParamList( ';', true );

	int n = clsParamList.Parse( pszText, iTextLen );
	if( n == -1 ) return -1;

	HTTP_PARAMETER_LIST::iterator itPL;
	const char * pszName;

	for( itPL = clsParamList.m_clsParamList.begin(); itPL != clsParamList.m_clsParamList.end(); ++itPL )
	{
		if( itPL == clsParamList.m_clsParamList.begin() )
		{
			m_strName = itPL->m_strName;
			m_strValue = itPL->m_strValue;
		}
		else
		{
			pszName = itPL->m_strName.c_str();

			if( !strcasecmp( pszName, "expires" ) )
			{
				if( m_iExpireTime == 0 )
				{
					m_iExpireTime = ParseCookieExpires( itPL->m_strValue.c_str() );
				}
			}
			else if( !strcasecmp( pszName, "max-age" ) )
			{
				int iMaxAge = atoi( itPL->m_strValue.c_str() );
				if( iMaxAge <= 0 )
				{
					m_iExpireTime = 0;
				}
				else
				{
					time( &m_iExpireTime );
					m_iExpireTime += iMaxAge;
				}
			}
			else if( !strcasecmp( pszName, "path" ) )
			{
				m_strPath = itPL->m_strValue;
			}
			else if( !strcasecmp( pszName, "domain" ) )
			{
				m_strDomain = itPL->m_strValue;
			}
			else if( !strcasecmp( pszName, "secure" ) )
			{
				m_bSecure = true;
			}
		}
	}

	if( m_strDomain.empty() == false )
	{
		// 도메인이 .google.com 이면 맨 앞의 . 을 제거한다.
		if( m_strDomain[0] == '.' )
		{
			m_strDomain.erase( 0, 1 );
		}
	}

	return n;
}

void CHttpSetCookie::Clear()
{
	m_strName.clear();
	m_strValue.clear();
	m_iExpireTime = 0;
	m_strDomain.clear();
	m_strPath.clear();
	m_bSecure = false;
}
