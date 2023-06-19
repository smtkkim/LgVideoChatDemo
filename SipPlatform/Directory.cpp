

#include <string.h>
#include "Directory.h"
#include "Log.h"

#ifndef WIN32
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#endif

#include "MemoryDebug.h"


bool CDirectory::Create( const char * szDirName, int iDirMode )
{
	int		i, iLen, iCount, n;
	char	* pszName;
	char	fisNotDirectory = 0;	// 디렉토리가 아닌가?
	char	fisError = 0;			// 디렉토리 생성시에 에러가 발생하였는가?

	iLen = (int)strlen( szDirName );
	pszName = new char[ iLen + 1 ];

	memset( pszName, 0, iLen + 1 );
	for( i = 0, iCount = 0; i < iLen; i++ )
	{
		if( szDirName[i] == DIR_SEP )
		{
			iCount++;

			// 디렉토리 이름이 "c:\test\temp\" 또는 "/test/temp/" 이므로 두번째 디렉토리
			// 구분자 부터 디렉토리를 생성하면 된다.
			if( iCount >= 2 )
			{
				n = CDirectory::IsDirectoryCheck( pszName );
				if( n == -1 )
				{
					// 디렉토리가 아닌 경우
					fisNotDirectory = 1;
					break;
				}
				else if( n == -2 )
				{
					// 디렉토리가 존재하지 않는 경우
#ifdef WIN32
					if( CreateDirectory( pszName, NULL ) == FALSE )
#else
					if( mkdir( pszName, iDirMode ) != 0 )
#endif
					{
						fisError = 1;
						break;
					}
				}
			}
		}

		pszName[i] = szDirName[i];
	}

	delete [] pszName;

	if( fisNotDirectory == 1 ) return false;
	if( fisError == 1 ) return false;

	// 디렉토리 이름이 "c:\test\temp" 또는 "/test/temp" 일 경우, 위의 loop 에서 temp 디렉토리를 
	// 생성하지 않으므로 이를 생성하기 위해서 아래의 코드가 필요하다.
	if( szDirName[iLen-1] != DIR_SEP )
	{
		n = CDirectory::IsDirectoryCheck( szDirName );
		if( n == -1 )
		{
			// 디렉토리가 아닌 경우
			return false;
		}
		else if( n == -2 )
		{
			// 디렉토리가 존재하지 않는 경우
#ifdef WIN32
			if( CreateDirectory( szDirName, NULL ) == FALSE )
#else
			if( mkdir( szDirName, iDirMode ) != 0 )
#endif
			{
				return false;
			}
		}
	}

	return true;
}


bool CDirectory::IsDirectory( const char * szDirName )
{
	if( CDirectory::IsDirectoryCheck( szDirName ) == 0 ) return true;

	return false;
}


int CDirectory::IsDirectoryCheck( const char * szDirName )
{
#ifdef WIN32
	// ACE 에서 S_ISDIR 을 지원하지 않기 때문에 윈도우의 경우를 위하여 WINAPI 를 사용하였음.
	DWORD iAttribute = GetFileAttributes( szDirName );

	if( iAttribute != -1 ) 
	{
		if( iAttribute & FILE_ATTRIBUTE_DIRECTORY )
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		// 파일이 존재하지 않는 경우.
		return -2;	
	}
#else
	struct stat		clsStat;

	if( stat( szDirName, &clsStat ) == 0 )
	{
		if( S_ISDIR( clsStat.st_mode ) ) 
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		// 파일이 존재하지 않는 경우.
		return -2;	
	}
#endif

	return 0;
}


void CDirectory::AppendName( std::string & strFileName, const char * pszAppend )
{
#ifdef WIN32
	strFileName.append( "\\" );
	strFileName.append( pszAppend );
#else
	strFileName.append( "/" );
	strFileName.append( pszAppend );
#endif
}


bool CDirectory::List( const char * pszDirName, FILE_LIST & clsFileList )
{
	clsFileList.clear();

#ifdef WIN32
	WIN32_FIND_DATA	sttFindData;
	HANDLE			hFind;
	BOOL				bNext = TRUE;
	std::string	strPath = pszDirName;

	strPath.append( "\\*.*" );

	hFind = FindFirstFile( strPath.c_str(), &sttFindData );
	if( hFind == INVALID_HANDLE_VALUE )
	{
		CLog::Print( LOG_ERROR, "FindFirstFile(%s) error(%d)", pszDirName, GetLastError() );
		return false;
	}

	for( ; bNext == TRUE; bNext = FindNextFile( hFind, &sttFindData ) )
	{
		if( !strcmp( sttFindData.cFileName, "." ) || !strcmp( sttFindData.cFileName, ".." ) ) continue;
		clsFileList.push_back( sttFindData.cFileName );
	}

	FindClose( hFind );
#else
	DIR						* psttDir;
	struct dirent	* psttDirent, sttDirent;
	int	n;

	psttDir = opendir( pszDirName );
	if( psttDir == NULL )
	{
		CLog::Print( LOG_ERROR, "opendir(%s) error(%d)", pszDirName, errno );
		return false;
	}

	for( n = readdir_r( psttDir, &sttDirent, &psttDirent ); psttDirent && n == 0; n = readdir_r( psttDir, &sttDirent, &psttDirent ) )
	{
		if( !strcmp( psttDirent->d_name, "." ) || !strcmp( psttDirent->d_name, ".." ) ) continue;

		clsFileList.push_back( psttDirent->d_name );
	}

	closedir( psttDir );
#endif

	return true;
}


bool CDirectory::FileList( const char * pszDirName, FILE_LIST & clsFileList )
{
	clsFileList.clear();

#ifdef WIN32
	WIN32_FIND_DATA	sttFindData;
	HANDLE			hFind;
	BOOL				bNext = TRUE;
	std::string	strPath = pszDirName;

	strPath.append( "\\*.*" );

	hFind = FindFirstFile( strPath.c_str(), &sttFindData );
	if( hFind == INVALID_HANDLE_VALUE )
	{
		CLog::Print( LOG_ERROR, "FindFirstFile(%s) error(%d)", pszDirName, GetLastError() );
		return false;
	}

	for( ; bNext == TRUE; bNext = FindNextFile( hFind, &sttFindData ) )
	{
		if( !strcmp( sttFindData.cFileName, "." ) || !strcmp( sttFindData.cFileName, ".." ) ) continue;
		if( sttFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) continue;

		clsFileList.push_back( sttFindData.cFileName );
	}

	FindClose( hFind );
#else
	DIR						* psttDir;
	struct dirent	* psttDirent, sttDirent;
	struct stat		sttStat;
	int		n;
	std::string		strFileName;

	psttDir = opendir( pszDirName );
	if( psttDir == NULL )
	{
		CLog::Print( LOG_ERROR, "opendir(%s) error(%d)", pszDirName, errno );
		return false;
	}

	for( n = readdir_r( psttDir, &sttDirent, &psttDirent ); psttDirent && n == 0; n = readdir_r( psttDir, &sttDirent, &psttDirent ) )
	{
		if( !strcmp( psttDirent->d_name, "." ) || !strcmp( psttDirent->d_name, ".." ) ) continue;
		strFileName = pszDirName;
		AppendName( strFileName, psttDirent->d_name );

		if( lstat( strFileName.c_str(), &sttStat ) < 0 ) continue;
		if( S_ISDIR( sttStat.st_mode ) ) continue;

		clsFileList.push_back( psttDirent->d_name );
	}

	closedir( psttDir );
#endif

	return true;
}


char * CDirectory::GetProgramDirectory( )
{
	static char	szDir[1024];

	if( strlen(szDir) == 0 )
	{
#ifdef WIN32
		int		i;
		HMODULE	hThis;

		hThis = GetModuleHandle( NULL );

		GetModuleFileName( hThis, szDir, sizeof(szDir));
		for( i = (int)strlen( szDir) - 1; i >= 0; i-- )
		{
			if( szDir[i] == '\\' ) 
			{
				szDir[i] = '\0';
				break;
			}
		}
#else
		if( readlink( "/proc/self/exe", szDir, sizeof(szDir) ) != -1 )
		{
			for( int i = (int)strlen( szDir) - 1; i >= 0; i-- )
			{
				if( szDir[i] == '/' ) 
				{
					szDir[i] = '\0';
					break;
				}
			}
		}
#endif
	}

	return szDir;
}

#define MAX_INT	4294967296L


int64_t CDirectory::GetSize( const char * pszDirName )
{
	uint64_t iTotalSize = 0;

#ifdef WIN32
	WIN32_FIND_DATA	sttFindData;
	HANDLE			hFind;
	BOOL				bNext = TRUE;
	std::string	strPath = pszDirName;

	strPath.append( "\\*.*" );

	hFind = FindFirstFile( strPath.c_str(), &sttFindData );
	if( hFind == INVALID_HANDLE_VALUE )
	{
		CLog::Print( LOG_ERROR, "FindFirstFile(%s) error(%d)", pszDirName, GetLastError() );
		return false;
	}

	for( ; bNext == TRUE; bNext = FindNextFile( hFind, &sttFindData ) )
	{
		if( !strcmp( sttFindData.cFileName, "." ) || !strcmp( sttFindData.cFileName, ".." ) ) continue;
		if( sttFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			std::string	strFolder = pszDirName;
			
			AppendName( strFolder, sttFindData.cFileName );
			iTotalSize += GetSize( strFolder.c_str() );
			continue;
		}
		
		if( sttFindData.nFileSizeHigh > 0 )
		{
			iTotalSize += (uint64_t)sttFindData.nFileSizeHigh * MAX_INT;
		}

		iTotalSize += sttFindData.nFileSizeLow;
	}

	FindClose( hFind );
#else
	DIR						* psttDir;
	struct dirent	* psttDirent, sttDirent;
	struct stat		sttStat;
	int		n;
	std::string		strFileName;

	psttDir = opendir( pszDirName );
	if( psttDir == NULL )
	{
		CLog::Print( LOG_ERROR, "opendir(%s) error(%d)", pszDirName, errno );
		return false;
	}

	for( n = readdir_r( psttDir, &sttDirent, &psttDirent ); psttDirent && n == 0; n = readdir_r( psttDir, &sttDirent, &psttDirent ) )
	{
		if( !strcmp( psttDirent->d_name, "." ) || !strcmp( psttDirent->d_name, ".." ) ) continue;
		strFileName = pszDirName;
		AppendName( strFileName, psttDirent->d_name );

		if( lstat( strFileName.c_str(), &sttStat ) < 0 ) continue;
		if( S_ISDIR( sttStat.st_mode ) )
		{
			std::string	strFolder = pszDirName;
			
			AppendName( strFolder, psttDirent->d_name );
			iTotalSize += GetSize( strFolder.c_str() );
			continue;
		}

		iTotalSize += sttStat.st_size;
	}

	closedir( psttDir );
#endif

	return iTotalSize;
}


void CDirectory::DeleteAllFile( const char * pszDirName )
{
	FILE_LIST::iterator	itFile;
	FILE_LIST clsFileList;

	FileList( pszDirName, clsFileList );

	for( itFile = clsFileList.begin(); itFile != clsFileList.end(); ++itFile )
	{
		std::string strFileName = pszDirName;

		CDirectory::AppendName( strFileName, itFile->c_str() );

#ifdef WIN32
		DeleteFile( strFileName.c_str() );
#else
		unlink( strFileName.c_str() );
#endif		
	}
}


void CDirectory::GetDirName( const char * pszFilePath, std::string & strDirName )
{
	int iLen = (int)strlen( pszFilePath );

	strDirName.clear();

	for( int i = iLen - 1; i >= 0; --i )
	{
#ifdef WIN32
		if( pszFilePath[i] == '\\' )
#else
		if( pszFilePath[i] == '/' )
#endif
		{
			strDirName.append( pszFilePath, i );
			break;
		}
	}
}


void CDirectory::GetFileName( const char * pszFilePath, std::string & strFileName )
{
	int iLen = (int)strlen( pszFilePath );

	strFileName.clear();

	for( int i = iLen - 1; i >= 0; --i )
	{
#ifdef WIN32
		if( pszFilePath[i] == '\\' )
#else
		if( pszFilePath[i] == '/' )
#endif
		{
			strFileName = pszFilePath + i + 1;
			break;
		}
	}
}


bool CDirectory::Delete( const char * pszDirName )
{
#ifdef WIN32
	WIN32_FIND_DATA	sttFindData;
	HANDLE			hFind;
	BOOL				bNext = TRUE;
	std::string	strPath = pszDirName;
	std::string strFileName;

	strPath.append( "\\*.*" );

	hFind = FindFirstFile( strPath.c_str(), &sttFindData );
	if( hFind == INVALID_HANDLE_VALUE )
	{
		CLog::Print( LOG_ERROR, "FindFirstFile(%s) error(%d)", pszDirName, GetLastError() );
		return false;
	}

	for( ; bNext == TRUE; bNext = FindNextFile( hFind, &sttFindData ) )
	{
		if( !strcmp( sttFindData.cFileName, "." ) || !strcmp( sttFindData.cFileName, ".." ) ) continue;

		strFileName = pszDirName;
		CDirectory::AppendName( strFileName, sttFindData.cFileName );

		if( sttFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			CDirectory::Delete( strFileName.c_str() );
			RemoveDirectory( strFileName.c_str() );
		}
		else
		{
			DeleteFile( strFileName.c_str() );
		}
	}

	FindClose( hFind );

	RemoveDirectory( pszDirName );
#else
	DIR						* psttDir;
	struct dirent	* psttDirent, sttDirent;
	struct stat		sttStat;
	int		n;
	std::string		strFileName;

	psttDir = opendir( pszDirName );
	if( psttDir == NULL )
	{
		CLog::Print( LOG_ERROR, "opendir(%s) error(%d)", pszDirName, errno );
		return false;
	}

	for( n = readdir_r( psttDir, &sttDirent, &psttDirent ); psttDirent && n == 0; n = readdir_r( psttDir, &sttDirent, &psttDirent ) )
	{
		if( !strcmp( psttDirent->d_name, "." ) || !strcmp( psttDirent->d_name, ".." ) ) continue;

		strFileName = pszDirName;
		AppendName( strFileName, psttDirent->d_name );

		if( lstat( strFileName.c_str(), &sttStat ) < 0 ) continue;
		if( S_ISDIR( sttStat.st_mode ) )
		{
			CDirectory::Delete( strFileName.c_str() );
			rmdir( strFileName.c_str() );
		}
		else
		{
			unlink( strFileName.c_str() );
		}
	}

	closedir( psttDir );
	rmdir( pszDirName );
#endif

	return true;
}

