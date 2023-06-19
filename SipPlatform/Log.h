

#ifndef _LOG_H_
#define _LOG_H_

#include "SipPlatformDefine.h"
#include <stdio.h>
#include "SipMutex.h"
#include "Directory.h"

#define LOG_MAX_SIZE								1024*8
#define FULLPATH_FILENAME_MAX_SIZE	1024
#define MIN_LOG_FILE_SIZE						1024*1024
#define MAX_LOG_FILE_SIZE						1024*1024*1024
#define DEFAULT_LOG_FILE_SIZE				1024*1024*10
#define DEFAULT_LOG_FOLDER_SIZE			3145728000UL


enum EnumLogLevel
{
	
	LOG_ERROR = 0x0001,
	
	LOG_INFO  = 0x0010,
	
	LOG_DEBUG = 0x0100,
	
	LOG_NETWORK = 0x0200,
	
	LOG_SYSTEM = 0x400,
	
	LOG_HTTP2 = 0x800,
	
	LOG_HTTP_HEADER = 0x1000
};


class ILogCallBack
{
public:
	virtual ~ILogCallBack(){};

	virtual void Print( EnumLogLevel eLevel, const char * fmt, ... ) = 0;
};


class CLog
{
private:
	static char				* m_pszDirName;		// 로그 파일을 저장할 디렉토리 이름
	static char				m_szDate[9];		// 현재 저장중인 로그 파일 이름
	static FILE				* m_sttFd;			// 로그 파일 지정자

	static CSipMutex	* m_pThreadMutex;	// Thread Mutex
	
	static int				m_iLevel;				// 로그 파일에 저장할 레벨
	static int				m_iMaxLogSize;	// 하나의 로그 파일에 저장할 수 있는 최대 크기
	static int				m_iLogSize;			// 현재까지 저장된 로그 크기
	static int				m_iIndex;				// 로그 파일 인덱스
	static int64_t		m_iMaxFolderSize;	// 로그 폴더의 최대 크기
	static int64_t		m_iFolderSize;		// 로그 폴더 크기

	static ILogCallBack * m_pclsCallBack;	// 로그 callback

public:
	static bool SetDirectory( const char * pszDirName );
	static void Release();

	static void SetCallBack( ILogCallBack * pclsCallBack );

	static int Print( EnumLogLevel iLevel, const char * fmt, ... );
	static void Print( void (* func)( FILE * fd ) );

	static int GetLevel( );
	static void SetLevel( int iLevel );
	static void SetNullLevel();

	static void SetDebugLevel( );
	static bool IsPrintLogLevel( EnumLogLevel iLevel );
	static void SetMaxLogSize( int iSize );
	static void SetMaxFolderSize( int64_t iSize );
	static int GetLogIndex();
	static void DeleteOldFile( );

	static void SortFileList( FILE_LIST & clsFileList );
};

#endif
