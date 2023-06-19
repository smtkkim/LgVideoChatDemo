

#ifndef _HTTP_CLIENT2_H_
#define _HTTP_CLIENT2_H_

#include "HttpPacket.h"
#include "HttpDomainCookie.h"
#include "SipTcp.h"
#include "TlsFunction.h"

class CHttpClient2
{
public:
	CHttpClient2();
	~CHttpClient2();

	bool DoGet( const char * pszUrl, std::string & strOutputContentType, std::string & strOutputBody );
	bool DoPost( const char * pszUrl, const char * pszInputContentType, const char * pszInputBody, int iInputBodyLen, std::string & strOutputContentType, std::string & strOutputBody );

	void Close();
	void SetRecvTimeout( int iRecvTimeout );
	void SetUserAgent( const char * pszUserAgent );
	int GetStatusCode();

private:
	bool Execute( CHttpUri * pclsUri, CHttpMessage * pclsRequest, CHttpPacket * pclsPacket );
	void AddCookie( CHttpUri * pclsUri, CHttpMessage * pclsMessage );
	void SetCookie( CHttpMessage * pclsMessage );

	int m_iRecvTimeout;
	int m_iStatusCode;

	std::string m_strHost;
	int	m_iPort;

	std::string m_strUserAgent;

	Socket m_hSocket;
	SSL * m_psttSsl;

	CHttpDomainCookie m_clsDomainCookie;
};

#endif
