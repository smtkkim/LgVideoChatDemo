

#ifndef _FILE_UTILITY_H_
#define _FILE_UTILITY_H_

#include "SipPlatformDefine.h"
#include <string>

bool IsExistFile( const char * pszFileName );
int64_t GetFileSize( const char * pszFileName );
bool GetFileExt( const char * pszFileName, std::string & strExt );
bool GetFolderPathOfFilePath( const char * pszFilePath, std::string & strFolder );
bool GetFileNameOfFilePath( const char * pszFilePath, std::string & strFileName );
void DelFile( const char * pszFileName );

#endif
