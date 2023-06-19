

#include "FileLog.h"
#include "TimeUtility.h"
#include "FileUtility.h"
#include <stdarg.h>

#ifndef WIN32
#include <unistd.h>
#endif

extern bool LogFileCompare( const std::string & strFirst, const std::string & strSecond );

CFileLog::CFileLog() : m_sttFd(NULL), m_iLevel(LOG_ERROR), m_iMaxLogSize(DEFAULT_LOG_FILE_SIZE), m_iLogSize(0)
	, m_iIndex(1), m_iMaxFolderSize(DEFAULT_LOG_FOLDER_SIZE), m_iFolderSize(0)
{
	memset( m_szDate, 0, sizeof(m_szDate) );
}

CFileLog::~CFileLog()
{
	Close();
}


bool CFileLog::Open( const char * pszDirName )
{
	if( pszDirName == NULL ) return false;

	int iLen = (int)strlen( pszDirName );
	bool bRes = false;

	if( iLen < 3 ) return false;

	m_clsMutex.acquire();

	if( m_strDirName.empty() )
	{
#ifdef WIN32
		if( pszDirName[iLen-1] == '\\' )
#else
		if( pszDirName[iLen-1] == '/' )
#endif
		{
			m_strDirName.append( pszDirName, iLen - 1 );
		}
		else
		{
			m_strDirName = pszDirName;
		}

		if( CDirectory::Create( m_strDirName.c_str() ) )
		{
			m_iFolderSize = CDirectory::GetSize( m_strDirName.c_str() );
			bRes = true;
		}
	}

	m_clsMutex.release();

	return bRes;
}


bool CFileLog::Close( )
{
	m_clsMutex.acquire();
	if( m_strDirName.empty() == false )
	{
		if( m_sttFd != NULL )
		{
			fclose( m_sttFd );
			m_sttFd = NULL;
		}

		m_strDirName.clear();
	}
	m_clsMutex.release();

	return true;
}


bool CFileLog::Print( EnumLogLevel iLevel, const char * fmt, ... )
{
	if( ( m_iLevel & iLevel ) == 0 ) return false;

	va_list		ap;
	char		szBuf[LOG_MAX_SIZE];
	char		szDate[9], szHeader[30];
	struct tm	sttTm;

	switch( iLevel )
	{
	case LOG_ERROR:
		snprintf( szHeader, sizeof(szHeader), "[ERROR] " );
		break;
	case LOG_INFO:
		snprintf( szHeader, sizeof(szHeader), "[INFO] " );
		break;
	case LOG_DEBUG:
		snprintf( szHeader, sizeof(szHeader), "[DEBUG] " );
		break;
	case LOG_NETWORK:
		snprintf( szHeader, sizeof(szHeader), "[NETWORK] " );
		break;
	case LOG_SYSTEM:
		snprintf( szHeader, sizeof(szHeader), "[SYSTEM] " );
		break;
	case LOG_HTTP2:
		snprintf( szHeader, sizeof(szHeader), "[HTTP2] " );
		break;
	case LOG_HTTP_HEADER:
		snprintf( szHeader, sizeof(szHeader), "[HTTP] " );
		break;
	default:
		memset( szHeader, 0, sizeof(szHeader) );
		break;
	}
	
	// 현재의 시간을 구한다.
	struct timeval	sttTime;

	gettimeofday( &sttTime, NULL );
#ifdef WIN32
	_localtime32_s( &sttTm, &sttTime.tv_sec );
#else
	localtime_r( &sttTime.tv_sec, &sttTm );	
#endif

	snprintf( szDate, sizeof(szDate), "%04d%02d%02d", sttTm.tm_year + 1900, sttTm.tm_mon + 1, sttTm.tm_mday );

	if( m_clsMutex.acquire() == false ) return false;

	if( m_strDirName.empty() == false && strcmp( m_szDate, szDate ) )
	{
		// 현재의 날짜와 클래스에 저장된 날짜가 다른 경우에는 로그 파일을 새로 제작하여서 Open 한다.
		char	szFileName[FULLPATH_FILENAME_MAX_SIZE];
		
		m_iIndex = 1;

OPEN_FILE:
		m_iLogSize = 0;

		snprintf( szFileName, sizeof(szFileName), "%s%c%04d%02d%02d_%d.txt", m_strDirName.c_str(), DIR_SEP, sttTm.tm_year + 1900, sttTm.tm_mon + 1, sttTm.tm_mday, m_iIndex );
		
		// 이미 Open 된 파일이 있는 경우에는 이를 닫는다.
		if( m_sttFd ) 
		{
			fclose( m_sttFd );
			m_sttFd = NULL;
		}

		DeleteOldFile();

		if( m_iMaxLogSize > 0 )
		{
			int64_t iFileSize = GetFileSize( szFileName );
			if( iFileSize >= m_iMaxLogSize )
			{
				++m_iIndex;
				goto OPEN_FILE;
			}

			m_iLogSize = (int)iFileSize;
		}
		
		m_sttFd = fopen( szFileName, "ab" );
		if( m_sttFd == NULL )
		{
			printf( "log file(%s) open error\n", szFileName );
		}

		snprintf( m_szDate, sizeof(m_szDate), "%s", szDate );
	}
	else if( m_iMaxLogSize > 0 && m_iLogSize >= m_iMaxLogSize )
	{
		++m_iIndex;
		goto OPEN_FILE;
	}

	m_clsMutex.release();

	va_start( ap, fmt );
	vsnprintf( szBuf, sizeof(szBuf)-1, fmt, ap );
	va_end( ap );

	if( m_clsMutex.acquire() == false ) return false;

	if( m_sttFd )
	{
		int iWrite;

#ifdef WIN32
		iWrite = fprintf( m_sttFd, "[%02d:%02d:%02d.%06u] %s[%X] %s\r\n"
			, sttTm.tm_hour, sttTm.tm_min, sttTm.tm_sec, sttTime.tv_usec, szHeader, GetCurrentThreadId(), szBuf );
#else
		iWrite = fprintf( m_sttFd, "[%02d:%02d:%02d.%06u] %s[%lX] %s\n"
			, sttTm.tm_hour, sttTm.tm_min, sttTm.tm_sec, (unsigned int)sttTime.tv_usec, szHeader, (unsigned long)pthread_self(), szBuf );
#endif
		fflush( m_sttFd );

		m_iLogSize += iWrite;
		m_iFolderSize += iWrite;
	}
	else
	{
#ifdef WIN32
		printf( "[%02d:%02d:%02d] %s%s\r\n", sttTm.tm_hour, sttTm.tm_min, sttTm.tm_sec, szHeader, szBuf );
#else
		printf( "[%02d:%02d:%02d] %s%s\n", sttTm.tm_hour, sttTm.tm_min, sttTm.tm_sec, szHeader, szBuf );
#endif
	}

	m_clsMutex.release();

	return true;
}


int CFileLog::GetLevel( )
{
	return m_iLevel;
}


void CFileLog::SetLevel( int iLevel )
{
	m_iLevel = LOG_ERROR | LOG_SYSTEM;
	m_iLevel |= iLevel;
}


bool CFileLog::IsPrintLogLevel( EnumLogLevel iLevel )
{
	if( ( m_iLevel & iLevel ) == 0 ) return false;

	return true;
}


void CFileLog::SetMaxLogSize( int iSize )
{
	if( iSize < MIN_LOG_FILE_SIZE )
	{
		iSize = MIN_LOG_FILE_SIZE;
	}
	else if( iSize > MAX_LOG_FILE_SIZE )
	{
		iSize = MAX_LOG_FILE_SIZE;
	}

	m_iMaxLogSize = iSize;
}


void CFileLog::SetMaxFolderSize( int64_t iSize )
{
	// 최소 30일 로그 파일 저장 공간을 확보한다.
	if( m_iMaxLogSize == 0 )
	{
		if( iSize < ( MIN_LOG_FILE_SIZE * 30 ) )
		{
			iSize = MIN_LOG_FILE_SIZE * 30;
		}
	}
	else
	{
		if( iSize < m_iMaxLogSize * 30 )
		{
			iSize = m_iMaxLogSize * 30;
		}
	}

	m_iMaxFolderSize = iSize;
}


void CFileLog::DeleteOldFile( )
{
	if( m_iMaxFolderSize == 0 ) return;
	if( m_strDirName.empty() ) return;

	if( m_iFolderSize >= m_iMaxFolderSize )
	{
		int64_t iWantSize = m_iMaxFolderSize * 8 / 10;
		FILE_LIST clsFileList;
		FILE_LIST::iterator	itList;
		int64_t iFileSize;

		CDirectory::FileList( m_strDirName.c_str(), clsFileList );

		clsFileList.sort( LogFileCompare );

		for( itList = clsFileList.begin(); itList != clsFileList.end(); ++itList )
		{
			std::string strFileName = m_strDirName;

			CDirectory::AppendName( strFileName, itList->c_str() );

			iFileSize = GetFileSize( strFileName.c_str() );

	#ifdef WIN32
			DeleteFile( strFileName.c_str() );
	#else
			unlink( strFileName.c_str() );
	#endif

			m_iFolderSize -= iFileSize;
		
			if( m_iFolderSize < iWantSize ) break;
		}
	}
}
