

#ifndef _HTTP_STACK_CALLBACK_H_
#define _HTTP_STACK_CALLBACK_H_

#include "HttpMessage.h"


class IHttpStackCallBack
{
public:
	IHttpStackCallBack(){};
	virtual ~IHttpStackCallBack(){};

	
	virtual bool RecvHttpRequest( CHttpMessage * pclsRequest, CHttpMessage * pclsResponse ) = 0;

	
	virtual void WebSocketConnected( const char * pszClientIp, int iClientPort, CHttpMessage * pclsRequest ) = 0;

	
	virtual void WebSocketClosed( const char * pszClientIp, int iClientPort ) = 0;

	
	virtual bool WebSocketData( const char * pszClientIp, int iClientPort, std::string & strData ) = 0;
};

#endif
