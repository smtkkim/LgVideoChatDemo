

#ifndef _TCP_SESSION_LIST_H_
#define _TCP_SESSION_LIST_H_

#include "SipTcp.h"
#include "SipMutex.h"
#include "TlsFunction.h"
#include <string>

class CTcpComm;
class ITcpStackCallBack;


class ITcpSessionApp
{
public:
	virtual ~ITcpSessionApp(){};
};


class CTcpSessionInfo
{
public:
	CTcpSessionInfo();
	~CTcpSessionInfo();

	bool Send( const char * pszPacket, int iPacketLen );
	void Log( const char * pszPacket, int iPacketLen, bool bSend );
	void Clear();

	std::string		m_strIp;
	int						m_iPort;

	Socket				m_hSocket;
	SSL						* m_psttSsl;

	CSipMutex			m_clsMutex;

	time_t				m_iConnectTime;
	time_t				m_iRecvTime;
	time_t				m_iSendTime;	// 클라이언트로 연결된 경우만 설정한다.

	int						m_iThreadIndex;
	int						m_iSessionIndex;

	ITcpSessionApp * m_pclsApp;
};


class CTcpSessionList
{
public:
	CTcpSessionList( );
	~CTcpSessionList( );

	bool Init( int iThreadIndex, int iPollFdMax );
	bool Insert( Socket hSocket );
	int Insert( CTcpComm & clsTcpComm );
	bool Delete( int iIndex );
	void DeleteAll( );
	void DeleteTimeout( int iTimeout, int iNoPacketTimeout
		, void (*BeforeDelete)( CTcpSessionList * pclsSessionList, int iIndex, void * pclsArg ), void * pclsArg );

	bool Send( int iIndex, const char * pszPacket, int iPacketLen );
	bool SendAll( const char * pszPacket, int iPacketLen, ITcpStackCallBack * pclsCallBack );
	bool SendAllExcept( const char * pszPacket, int iPacketLen, ITcpStackCallBack * pclsCallBack, int iThreadIndex, int iSessionIndex );

	struct pollfd * m_psttPollFd;
	CTcpSessionInfo * m_pclsSession;

	int	m_iPollFdMax;
	int m_iPoolFdCount;

private:
	void Insert( int iIndex, CTcpComm & clsTcpComm );
	void ClearFd( int iIndex );
};

#endif
