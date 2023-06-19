

#ifndef _TCP_THREAD_H_
#define _TCP_THREAD_H_

#include "SipPlatformDefine.h"
#include "SipTcp.h"

class CTcpStack;

class CTcpNoPipeThreadArg
{
public:
	CTcpNoPipeThreadArg() : m_bClient(false)
	{}

	std::string	m_strIp;
	int			m_iPort;
	Socket	m_hSocket;
	CTcpStack * m_pclsStack;
	bool		m_bClient;
};

bool StartTcpClientThread( const char * pszIp, int iPort, CTcpStack * pclsStack );

THREAD_API TcpNoPipeThread( LPVOID lpParameter );
THREAD_API TcpPipeThread( LPVOID lpParameter );
THREAD_API TcpListenThread( LPVOID lpParameter );

bool IsTcpListenThreadRun();

#endif
