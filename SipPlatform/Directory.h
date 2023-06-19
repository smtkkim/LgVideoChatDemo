

#ifndef _DIRECTORY_H_
#define _DIRECTORY_H_

#include "SipPlatformDefine.h"
#include <string>
#include <list>

#ifdef WIN32
# include <windows.h>
# define DIR_SEP	'\\'
# define DIR_MODE		0
#else
# include <sys/stat.h>
# define DIR_SEP	'/'
# define DIR_MODE		S_IRWXU
#endif

typedef std::list< std::string > FILE_LIST;


class CDirectory
{
public:
	static bool Create( const char * szDirName, int iDirMode = DIR_MODE );
	static bool IsDirectory( const char * szDirName );
	static int IsDirectoryCheck( const char * szDirName );
	static void AppendName( std::string & strFileName, const char * pszAppend );
	static bool List( const char * pszDirName, FILE_LIST & clsFileList );
	static bool FileList( const char * pszDirName, FILE_LIST & clsFileList );
	static char * GetProgramDirectory( );
	static int64_t GetSize( const char * pszDirName );
	static void DeleteAllFile( const char * pszDirName );
	static void GetDirName( const char * pszFilePath, std::string & strDirName );
	static void GetFileName( const char * pszFilePath, std::string & strFileName );
	static bool Delete( const char * pszDirName );
};

#endif
