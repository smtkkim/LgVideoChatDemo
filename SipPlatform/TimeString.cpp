

#include "SipPlatformDefine.h"
#include <stdio.h>
#include <stdlib.h>
#include "TimeString.h"
#include "MemoryDebug.h"


void LocalTime( time_t iTime, struct tm & sttTm )
{
#ifdef WIN32
	localtime_s( &sttTm, &iTime );
#else
	localtime_r( &iTime, &sttTm );	
#endif
}


void GetDateTimeString( time_t iTime, char * pszTime, int iTimeSize )
{
	struct tm	sttTm;

	LocalTime( iTime, sttTm );

	snprintf( pszTime, iTimeSize, "%04d%02d%02d%02d%02d%02d", sttTm.tm_year + 1900, sttTm.tm_mon + 1, sttTm.tm_mday
		, sttTm.tm_hour, sttTm.tm_min, sttTm.tm_sec );
}


void GetDateTimeString( char * pszTime, int iTimeSize )
{
	time_t		iTime;

	time( &iTime );

	GetDateTimeString( iTime, pszTime, iTimeSize );
}


void GetDateString( time_t iTime, char * pszDate, int iDateSize )
{
	struct tm	sttTm;

	LocalTime( iTime, sttTm );

	snprintf( pszDate, iDateSize, "%04d%02d%02d", sttTm.tm_year + 1900, sttTm.tm_mon + 1, sttTm.tm_mday );
}


void GetDateString( char * pszDate, int iDateSize )
{
	time_t		iTime;

	time( &iTime );

	GetDateString( iTime, pszDate, iDateSize );
}


void GetTimeString( time_t iTime, char * pszTime, int iTimeSize )
{
	struct tm	sttTm;

	LocalTime( iTime, sttTm );

	snprintf( pszTime, iTimeSize, "%02d%02d%02d", sttTm.tm_hour, sttTm.tm_min, sttTm.tm_sec );
}


void GetTimeString( char * pszTime, int iTimeSize )
{
	time_t		iTime;

	time( &iTime );

	GetTimeString( iTime, pszTime, iTimeSize );
}


time_t ParseCookieExpires( const char * pszTime )
{
	static const char * arrMonth[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL };

	struct tm	sttTm;
	char szBuf[11];
	char cType = 0;
	int iBufPos = 0;
	bool bClearBuf = false;

	memset( &sttTm, 0, sizeof(sttTm) );
	memset( szBuf, 0, sizeof(szBuf) );

	for( int i = 0; pszTime[i]; ++i )
	{
		if( pszTime[i] == ' ' )
		{
			switch( cType )
			{
			case 3:
				sttTm.tm_year = atoi( szBuf ) - 1900;
				break;
			case 6:
				sttTm.tm_sec = atoi( szBuf );
				break;
			}

			bClearBuf = true;
		}
		else if( pszTime[i] == '-' )
		{
			switch( cType )
			{
			case 1:
				sttTm.tm_mday = atoi( szBuf );
				break;
			case 2:
				for( int iIndex = 0; arrMonth[iIndex]; ++iIndex )
				{
					if( !strcasecmp( arrMonth[iIndex], szBuf ) )
					{
						sttTm.tm_mon = iIndex;
						break;
					}
				}
				break;
			}

			bClearBuf = true;
		}
		else if( pszTime[i] == ':' )
		{
			switch( cType )
			{
			case 4:
				sttTm.tm_hour = atoi( szBuf );
				break;
			case 5:
				sttTm.tm_min = atoi( szBuf );
				break;
			}

			bClearBuf = true;
		}
		else if( cType )
		{
			if( iBufPos >= ( (int)sizeof(szBuf) - 1 ) ) return 0;
			szBuf[iBufPos++] = pszTime[i];
		}

		if( bClearBuf )
		{
			memset( szBuf, 0, sizeof(szBuf) );
			iBufPos = 0;
			bClearBuf = false;
			++cType;
		}
	}

	time_t iTime = mktime( &sttTm );

	// Seoul = GMT + 9
	iTime += 9 * 3600;

	return iTime;
}
