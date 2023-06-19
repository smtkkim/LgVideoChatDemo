

#ifndef _STRING_UTILITY_H_
#define _STRING_UTILITY_H_

#include "SipPlatformDefine.h"
#include <string>
#include <list>
#include <vector>

typedef std::list< std::string > STRING_LIST;
typedef std::vector< std::string > STRING_VECTOR;

void ReplaceString( std::string & strCallId, const char * pszBefore, const char * pszAfter );
bool SearchValue( std::string & strText, const char * pszKey, char cSep, std::string & strValue );
bool SearchValue( std::string & strText, const char * pszKey, char cSep, int & iValue );
void LeftTrimString( std::string & strText );
void RightTrimString( std::string & strText );
void TrimString( std::string & strText );
void SplitString( const char * pszText, STRING_LIST & clsList, char cSep );
void SplitString( const char * pszText, STRING_VECTOR & clsList, char cSep );

uint32_t GetUInt32( const char * pszText );
uint64_t GetUInt64( const char * pszText );

bool HexToString( const char * pszInput, std::string & strOutput );
bool HexToString( const char * pszInput, int iInputSize, std::string & strOutput );
void StringToHex( std::string & strInput, std::string & strOutput );
void StringToHex( const char * pszInput, int iInputSize, std::string & strOutput );

bool IsPrintString( const char * pszText, int iTextLen );
void DeQuoteString( std::string & strInput, std::string & strOutput );

void DecodeUri( std::string & strInput, std::string & strOutput );

#ifdef WIN32
bool Utf8ToAnsi( const char * pszUtf8, std::string & strOutput );
bool AnsiToUtf8( const char * pszAnsi, std::string & strOutput );
#endif

#endif
