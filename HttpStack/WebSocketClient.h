

#ifndef _WEB_SOCKET_CLIENT_H_
#define _WEB_SOCKET_CLIENT_H_

#include "SipTcp.h"
#include "SipMutex.h"
#include "TlsFunction.h"


class IWebSocketClientCallBack
{
public:
	IWebSocketClientCallBack(){};
	virtual ~IWebSocketClientCallBack(){};

	virtual bool RecvWebSocket( const char * pszPacket, int iPacketLen ) = 0;
};


enum EWebSocketType
{
	E_WST_TEXT = 0,
	E_WST_BINARY,
	E_WST_PING,
	E_WST_PONG
};


class CWebSocketClient
{
public:
	CWebSocketClient();
	~CWebSocketClient();

	bool Connect( const char * pszUrl, IWebSocketClientCallBack * pclsCallBack );
	void Close();

	bool Send( std::string & strData );
	bool Send( EWebSocketType eType, const char * pszData, int iDataLen );
	bool SendTcp( const char * pszPacket, int iPacketLen );

	bool IsClosed();

	int		m_iRecvTimeout;
	bool	m_bStop;
	bool	m_bThreadRun;
	Socket m_hSocket;
	SSL * m_psttSsl;
	CSipMutex	m_clsMutex;
	IWebSocketClientCallBack * m_pclsCallBack;
	std::string m_strServerIp;
	int					m_iServerPort;
};

#endif
