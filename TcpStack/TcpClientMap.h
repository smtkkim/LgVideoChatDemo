

#ifndef _TCP_CLIENT_MAP_H_
#define _TCP_CLIENT_MAP_H_

#include "SipMutex.h"
#include <string>
#include <list>
#include <map>


class CTcpClientIndex
{
public:
	CTcpClientIndex();

	
	int		m_iThreadIndex;

	
	int		m_iSessionIndex;
};

typedef std::list< std::string * > SEND_PACKET_LIST;


class CTcpClientInfo
{
public:
	CTcpClientInfo();

	bool	m_bTrying;
	CTcpClientIndex	m_clsIndex;
	SEND_PACKET_LIST m_clsSendPacketList;
};

typedef std::map< std::string, CTcpClientInfo > TCP_CLIENT_MAP;

class CTcpStack;


class CTcpClientMap
{
public:
	CTcpClientMap();
	~CTcpClientMap();

	bool Create( CTcpStack * pclsStack );

	bool Send( const char * pszIp, int iPort, const char * pszPacket, int iPacketLen, bool bConnectIfNoSession );
	bool Delete( const char * pszIp, int iPort );
	bool DeleteSendPacketList( const char * pszIp, int iPort, SEND_PACKET_LIST & clsList );

	bool SetConnected( const char * pszIp, int iPort, int iThreadIndex, int iSessionIndex );
	bool Insert( const char * pszIp, int iPort, int iThreadIndex, int iSessionIndex );

private:
	bool Delete( const char * pszIp, int iPort, SEND_PACKET_LIST & clsList );
	void GetKey( const char * pszIp, int iPort, std::string & strKey );

	TCP_CLIENT_MAP	m_clsMap;
	CSipMutex				m_clsMutex;
	CTcpStack			* m_pclsStack;
};

#endif
