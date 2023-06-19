

#ifndef _TCP_STACK_SETUP_H_
#define _TCP_STACK_SETUP_H_

#include "XmlElement.h"


class CTcpStackSetup
{
public:
	CTcpStackSetup();
	~CTcpStackSetup();

	bool Parse( CXmlElement & clsXml );

	
	std::string m_strListenIp;

	
	int m_iListenPort;

	
	int m_iThreadInitCount;

	
	int m_iThreadMaxCount;

	
	int m_iMaxSocketPerThread;

	
	int m_iTcpRecvTimeout;

	
	int m_iTcpNoPacketTimeout;

	
	int m_iTcpConnectTimeout;

	
	bool m_bUseTls;

	
	std::string m_strCertFile;

	
	std::string m_strCipherList;

	
	bool m_bUseThreadPipe;

	
	bool m_bUseHttp2;

	
	bool m_bUseHttp2HeaderLog;
};

#endif
