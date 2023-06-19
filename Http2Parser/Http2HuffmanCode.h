

#ifndef _HTTP_HUFFMAN_CODE_H_
#define _HTTP_HUFFMAN_CODE_H_

#include "SipPlatformDefine.h"

int HuffmanCodeEncode( const uint8_t * pszInput, uint32_t iInputLen, uint8_t * pszOutput, uint32_t iOutputSize );
int HuffmanCodeDecode( const uint8_t * pszInput, uint32_t iInputLen, uint8_t * pszOutput, uint32_t iOutputSize );

#endif
