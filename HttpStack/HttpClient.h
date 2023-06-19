

#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "HttpPacket.h"
#include <map>

typedef std::map< std::string, std::string > POST_NAME_VALUE_MAP;


class CHttpClient
{
public:
	CHttpClient();
	~CHttpClient();

	bool DoGet( const char * pszUrl, std::string & strOutputContentType, std::string & strOutputBody );
	bool DoGet( const char * pszUrl, const char * pszInputContentType, const char * pszInputBody, std::string & strOutputContentType, std::string & strOutputBody );
	bool DoGet( const char * pszUrl, HTTP_HEADER_LIST * pclsHeaderList, std::string & strOutputContentType, std::string & strOutputBody );

	bool DoPost( const char * pszUrl, const char * pszInputContentType, const char * pszInputBody, std::string & strOutputContentType, std::string & strOutputBody );
	bool DoPost( const char * pszUrl, HTTP_HEADER_LIST * pclsHeaderList, const char * pszInputContentType, const char * pszInputBody, int iInputBodyLen, std::string & strOutputContentType, std::string & strOutputBody );
	
	bool DoSoap( const char * pszUrl, const char * pszSoapAction, const char * pszInputBody, std::string & strOutputBody );
	bool DoSoap( const char * pszUrl, const char * pszSoapAction, const char * pszInputContentType, const char * pszInputBody, std::string & strOutputBody );
	
	bool DoUpload( const char * pszUrl, const char * pszFilePath, const char * pszPostName, POST_NAME_VALUE_MAP & clsPostDataMap, std::string & strOutputContentType, std::string & strOutputBody );

	void SetUserAgent( const char * pszUserAgent );
	void SetRecvTimeout( int iRecvTimeout );
	int GetStatusCode();

private:
	int m_iRecvTimeout;
	int m_iStatusCode;
	std::string m_strUserAgent;

	bool Execute( CHttpUri * pclsUri, CHttpMessage * pclsRequest, CHttpPacket * pclsPacket );
};

#endif
