

#ifndef _SIMPLE_HTTP_SERVER_H_
#define _SIMPLE_HTTP_SERVER_H_

#include "HttpStack.h"
#include "StringUtility.h"
#include "UserDB.h"

struct upload_status {
  int lines_read;
  std::string str;
};

class CWebRtcServer : public IHttpStackCallBack
{
public:
	CWebRtcServer();
	virtual ~CWebRtcServer();

	virtual bool RecvHttpRequest( CHttpMessage * pclsRequest, CHttpMessage * pclsResponse );

	virtual void WebSocketConnected( const char * pszClientIp, int iClientPort, CHttpMessage * pclsRequest );
	virtual void WebSocketClosed( const char * pszClientIp, int iClientPort );
	virtual bool WebSocketData( const char * pszClientIp, int iClientPort, std::string & strData );
    static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp);

	std::string m_strDocumentRoot;
	bool m_bStop;
	CUserDB *m_clsUserDB;

private:
	bool Send( const char * pszClientIp, int iClientPort, const char * fmt, ... );
	bool SendCall( const char * pszClientIp, int iClientPort, std::string & strData, std::string & strUserId );
	int getTimeUtc(uint64_t* time_utc);
	bool NeedPasswdUpdate(time_t last_time, time_t current_time);
	bool validatePassword(const std::string& password);
	std::string generateKey(const int len);
	int generateTOTP(const std::string& secret);
	std::string base32_decode(const std::string& base32);
	int base32_decode_char(char c);
    int SendMail(std::string& userID, std::string& newPassword);
};

#endif
