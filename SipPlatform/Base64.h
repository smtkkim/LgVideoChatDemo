

#ifndef _BASE64_H_
#define _BASE64_H_

#include <string>

int GetBase64EncodeLength( int iLength );
int GetBase64DecodeLength( int iLength );
int Base64Encode( const char * pszInput, int iInputLength, char * pszOutput, int iOutputLength );
int Base64Decode( const char * pszInput, int iInputLength, char * pszOutput, int iOutputLength );

bool Base64Encode( const char * pszInput, int iInputLength, std::string & strOutput );

#endif
