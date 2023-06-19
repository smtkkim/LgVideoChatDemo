

#include "Base64.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char garrBase64Table[] =
	{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
	};
static char gcBase64Pad = '=';


int GetBase64EncodeLength( int iLength )
{
	return ((iLength + 3 - iLength % 3) * 4 / 3 + 1);
}


int GetBase64DecodeLength( int iLength )
{
	return( (iLength / 4 * 3 + 1) * sizeof(char) );
}


int Base64Encode( const char * pszInput, int iInputLength, char * pszOutput, int iOutputLength )
{
	const unsigned char *pszCurrent = (const unsigned char *)pszInput;
	unsigned char * pszResult = (unsigned char *)pszOutput;
	int i = 0;

	if( pszResult == NULL ) return -1;
	if( iOutputLength < GetBase64EncodeLength( iInputLength )) return -1;
	
	while( iInputLength > 2 )
	{ 
		pszResult[i++] = garrBase64Table[pszCurrent[0] >> 2];
		pszResult[i++] = garrBase64Table[((pszCurrent[0] & 0x03) << 4) + (pszCurrent[1] >> 4)];
		pszResult[i++] = garrBase64Table[((pszCurrent[1] & 0x0f) << 2) + (pszCurrent[2] >> 6)];
		pszResult[i++] = garrBase64Table[pszCurrent[2] & 0x3f];

		pszCurrent += 3;
		iInputLength -= 3; 
	}

	if( iInputLength != 0 )
	{
		pszResult[i++] = garrBase64Table[pszCurrent[0] >> 2];
		if( iInputLength > 1 ) 
		{
			pszResult[i++] = garrBase64Table[((pszCurrent[0] & 0x03) << 4) + (pszCurrent[1] >> 4)];
			pszResult[i++] = garrBase64Table[(pszCurrent[1] & 0x0f) << 2];
			pszResult[i++] = gcBase64Pad;
		}
		else 
		{
			pszResult[i++] = garrBase64Table[(pszCurrent[0] & 0x03) << 4];
			pszResult[i++] = gcBase64Pad;
			pszResult[i++] = gcBase64Pad;
		}
	}

	pszResult[i] = '\0';
	return i;
}


int Base64Decode( const char * pszInput, int iInputLength, char * pszOutput, int iOutputLength )
{
	const unsigned char *pszCurrent = (const unsigned char *)pszInput;
	unsigned char * pszResult = (unsigned char *)pszOutput;
	int ch, i = 0, j = 0, k;
	char *chp;

	if( pszResult == NULL ) return -1;
	if( iOutputLength < GetBase64DecodeLength(iInputLength) ) return -1;
	
	while( (ch = *pszCurrent++) != '\0' )
	{
		if( ch == gcBase64Pad ) break;
		chp = strchr(garrBase64Table, ch);
		if (chp == NULL) continue;
		ch = (int)(chp - garrBase64Table);

		switch( i % 4 )
		{
		case 0:
			pszResult[j] = ch << 2;
			break;
		case 1:
			pszResult[j++] |= ch >> 4;
			pszResult[j] = (ch & 0x0f) << 4;
			break;
		case 2:
			pszResult[j++] |= ch >>2;
			pszResult[j] = (ch & 0x03) << 6;
			break;
		case 3:
			pszResult[j++] |= ch;
			break;
		}
		i++;
	}

	k = j;
	if( ch == gcBase64Pad )
	{
		switch(i % 4) 
		{
		case 0:
		case 1:
			return -1;
		case 2:
			k++;
		case 3:
			pszResult[k++] = 0;
		}
	}

	pszResult[k] = '\0';
	return j;
}


bool Base64Encode( const char * pszInput, int iInputLength, std::string & strOutput )
{
	int iOutputLen = GetBase64EncodeLength( iInputLength );
	char * pszOutput = (char *)malloc( iOutputLen );
	if( pszOutput == NULL )
	{
		return false;
	}

	Base64Encode( pszInput, iInputLength, pszOutput, iOutputLen );
	strOutput = pszOutput;

	free( pszOutput );

	return true;
}
