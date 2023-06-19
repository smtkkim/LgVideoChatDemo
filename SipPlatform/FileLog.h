

#ifndef _FILE_LOG_H_
#define _FILE_LOG_H_

#include "Log.h"


class CFileLog
{
public:
	CFileLog();
	~CFileLog();

public:
	bool Open( const char * pszDirName );
	bool Close( );

	bool Print( EnumLogLevel iLevel, const char * fmt, ... );

	int GetLevel( );
	void SetLevel( int iLevel );
	bool IsPrintLogLevel( EnumLogLevel iLevel );

	void SetMaxLogSize( int iSize );
	void SetMaxFolderSize( int64_t iSize );

private:
	void DeleteOldFile( );

	std::string	m_strDirName;		// 로그 파일을 저장할 디렉토리 이름
	char				m_szDate[9];		// 현재 저장중인 로그 파일 이름
	FILE				* m_sttFd;			// 로그 파일 지정자

	CSipMutex		m_clsMutex;			// Thread Mutex
	
	int				m_iLevel;					// 로그 파일에 저장할 레벨
	int				m_iMaxLogSize;		// 하나의 로그 파일에 저장할 수 있는 최대 크기
	int				m_iLogSize;				// 현재까지 저장된 로그 크기
	int				m_iIndex;					// 로그 파일 인덱스
	int64_t		m_iMaxFolderSize;	// 로그 폴더의 최대 크기
	int64_t		m_iFolderSize;		// 로그 폴더 크기
};

#endif
