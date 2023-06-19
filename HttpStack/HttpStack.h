

#ifndef _HTTP_STACK_H_
#define _HTTP_STACK_H_

#include "TcpStack.h"
#include "HttpPacket.h"
#include "Http2Packet.h"
#include "Http2Conversion.h"
#include "WebSocketPacket.h"
#include "HttpStackCallBack.h"


enum EHttpSessionType
{
	E_HST_NULL = 0,
	E_HST_WEB_SOCKET,
	E_HST_HTTP_2
};



class CHttpMessageSendRecv
{
public:
	CHttpMessage m_clsSend;
	CHttpMessage m_clsRecv;
};

// key = HTTP/2 frame id
typedef std::map< int, CHttpMessageSendRecv > HTTP_MESSAGE_MAP;


class CHttpStackSession : public ITcpSessionApp
{
public:
	CHttpStackSession() : m_eType(E_HST_NULL){};
	virtual ~CHttpStackSession(){};

	CHttpMessageSendRecv * GetMessage( int iId )
	{
		HTTP_MESSAGE_MAP::iterator itMap = m_clsMessageMap.find( iId );
		if( itMap == m_clsMessageMap.end() )
		{
			CHttpMessageSendRecv clsData;

			m_clsMessageMap.insert( HTTP_MESSAGE_MAP::value_type( iId, clsData ) );
			itMap = m_clsMessageMap.find( iId );
			if( itMap == m_clsMessageMap.end() )
			{
				return NULL;
			}
		}

		return &itMap->second;
	}

	CHttpPacket				m_clsHttpPacket;
	CHttp2Packet			m_clsHttp2Packet;
	CHttp2Conversion	m_clsSendConversion;
	CHttp2Conversion	m_clsRecvConversion;
	CHttp2FrameList		m_clsFrameList;
	CWebSocketPacket	m_clsWsPacket;
	EHttpSessionType	m_eType;

	HTTP_MESSAGE_MAP	m_clsMessageMap;
};


class CHttpStack : public ITcpStackCallBack
{
public:
	CHttpStack();
	virtual ~CHttpStack();

	bool Start( CTcpStackSetup * pclsSetup, IHttpStackCallBack * pclsCallBack );
	bool Stop( );
	static void Release();

	// HttpStackWebSocket.hpp
	bool SendWebSocketPacket( const char * pszClientIp, int iClientPort, const char * pszData, int iDataLen );

	// ITcpStackCallBack
	virtual bool InComingConnected( CTcpSessionInfo * pclsSessionInfo );
	virtual void SessionClosed( CTcpSessionInfo * pclsSessionInfo );
	virtual bool RecvPacket( char * pszPacket, int iPacketLen, CTcpSessionInfo * pclsSessionInfo );

private:
	// HttpStackHttp2.hpp
	bool RecvPacketHttp2Pri( CTcpSessionInfo * pclsSessionInfo, CHttpStackSession * pclsApp );
	bool RecvPacketHttp2( char * pszPacket, int iPacketLen, CTcpSessionInfo * pclsSessionInfo, CHttpStackSession * pclsApp );

	// HttpStackWebSocket.hpp
	bool RecvPacketWebSocket( char * pszPacket, int iPacketLen, CTcpSessionInfo * pclsSessionInfo, CHttpStackSession * pclsApp );
	bool MakeWebSocketResponse( CHttpMessage * pclsRecv, CHttpMessage * pclsSend );

	CTcpStack m_clsTcpStack;
	IHttpStackCallBack * m_pclsCallBack;
	static const EVP_MD * m_psttMd;
};

#endif
