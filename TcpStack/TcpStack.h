

#ifndef _TCP_STACK_H_
#define _TCP_STACK_H_

#include "TcpThreadList.h"
#include "TcpStackSetup.h"
#include "TcpClientMap.h"
#include "TcpSessionMap.h"
#include "TcpStackCallBack.h"
#include "TcpStackSetup.h"




class CTcpStack
{
public:
	CTcpStack();
	~CTcpStack();

	bool Start( CTcpStackSetup * pclsSetup, ITcpStackCallBack * pclsCallBack );
	bool Stop( );

	bool Send( const char * pszIp, int iPort, const char * pszPacket, int iPacketLen, bool bConnectIfNoSession );
	bool Send( int iThreadIndex, int iSessionIndex, const char * pszPacket, int iPacketLen );
	bool SendAll( const char * pszPacket, int iPacketLen );
	bool SendAllExcept( const char * pszPacket, int iPacketLen, int iThreadIndex, int iSessionIndex );

	CTcpStackSetup m_clsSetup;
	CTcpThreadList	m_clsThreadList;
	CTcpClientMap		m_clsClientMap;
	CTcpSessionMap	m_clsSessionMap;

	ITcpStackCallBack * m_pclsCallBack;

	Socket m_hTcpListenFd;
	bool m_bStop;
};

#endif
