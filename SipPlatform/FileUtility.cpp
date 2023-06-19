

#include "FileUtility.h"
#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include "MemoryDebug.h"


bool IsExistFile( const char * pszFileName )
{
	struct stat sttStat;

	if( stat( pszFileName, &sttStat ) == -1 )
	{
		return false;
	}

	return true;
}


int64_t GetFileSize( const char * pszFileName )
{
	struct stat sttStat;

	if( stat( pszFileName, &sttStat ) == -1 )
	{
		return 0;
	}

	return sttStat.st_size;
}


bool GetFileExt( const char * pszFileName, std::string & strExt )
{
	int iLen = (int)strlen( pszFileName );

	strExt.clear();

	for( int i = iLen - 1; i >= 0; --i )
	{
		if( pszFileName[i] == '.' )
		{
			strExt = pszFileName + i + 1;
			return true;
		}
	}

	return false;
}


bool GetFolderPathOfFilePath( const char * pszFilePath, std::string & strFolder )
{
	strFolder.clear();

	if( pszFilePath == NULL ) return false;

	int iLen = (int)strlen( pszFilePath );
	if( iLen < 2 ) return false;

#ifdef WIN32
	if( pszFilePath[iLen-1] == '\\' ) return false;
#else
	if( pszFilePath[iLen-1] == '/' ) return false;
#endif

	for( int i = iLen - 2; i >= 0; --i )
	{
#ifdef WIN32
		if( pszFilePath[i] == '\\' )
#else
		if( pszFilePath[i] == '/' )
#endif
		{

#ifndef WIN32
			if( i == 0 )
			{
				strFolder = "/";
				return true;
			}
#endif

			strFolder.append( pszFilePath, i );
			return true;
		}
	}

	return false;
}


bool GetFileNameOfFilePath( const char * pszFilePath, std::string & strFileName )
{
	strFileName.clear();

	if( pszFilePath == NULL ) return false;

	int iLen = (int)strlen( pszFilePath );
	if( iLen < 2 ) return false;

#ifdef WIN32
	if( pszFilePath[iLen-1] == '\\' ) return false;
#else
	if( pszFilePath[iLen-1] == '/' ) return false;
#endif

	for( int i = iLen - 2; i >= 0; --i )
	{
#ifdef WIN32
		if( pszFilePath[i] == '\\' )
#else
		if( pszFilePath[i] == '/' )
#endif
		{
			strFileName.append( pszFilePath + i + 1 );
			return true;
		}
	}

	return false;
}


void DelFile( const char * pszFileName )
{
#ifdef WIN32
	DeleteFile( pszFileName );
#else
	unlink( pszFileName );
#endif
}
