

#ifndef _HTTP_MESSAGE_H_
#define _HTTP_MESSAGE_H_

#include "SipPlatformDefine.h"

#define HTTP_VERSION	"HTTP/1.1"

#include "HttpHeader.h"
#include "HttpUri.h"




class CHttpMessage
{
public:
	CHttpMessage();
	~CHttpMessage();

	int Parse( const char * pszText, int iTextLen );
	int ParseHeader( const char * pszText, int iTextLen );
	int ToString( char * pszText, int iTextSize, bool bHeaderOnly = false );
	int ToString( std::string & strText, bool bHeaderOnly = false );
	void Clear();

	bool AddHeader( const char * pszName, std::string & strValue );
	bool AddHeader( const char * pszName, const char * pszValue );
	bool AddHeader( const char * pszName, int iValue );

	bool UpdateHeader( const char * pszName, const char * pszValue );
	bool ReplaceHeader( const char * pszName, const char * pszValue );
	
	CHttpHeader * GetHeader( const char * pszName );

	bool SetRequest( const char * pszMethod, CHttpUri * pclsUri, const char * pszUserAgent = NULL );
	bool IsRequest( );

	
	std::string		m_strHttpMethod;

	
	std::string		m_strReqUri;

	
	std::string		m_strHttpVersion;

	
	int						m_iStatusCode;

	
	std::string		m_strReasonPhrase;

	
	std::string		m_strContentType;

	
	int							m_iContentLength;

	
	bool						m_bChunked;

	
	HTTP_HEADER_LIST		m_clsHeaderList;

	
	std::string			m_strBody;

	
	uint32_t				m_iStreamIdentifier;

private:
	int ParseStatusLine( const char * pszText, int iTextLen );
	int ParseRequestLine( const char * pszText, int iTextLen );
};

#endif
