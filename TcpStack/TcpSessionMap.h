

#ifndef _TCP_SESSION_MAP_H_
#define _TCP_SESSION_MAP_H_

#include "TcpSessionList.h"
#include <map>

typedef std::map< std::string, CTcpSessionInfo * > TCP_SESSION_MAP;


class CTcpSessionMap
{
public:
	CTcpSessionMap();
	~CTcpSessionMap();

	bool Insert( const char * pszIp, int iPort, CTcpSessionInfo * pclsSessionInfo );
	bool Delete( const char * pszIp, int iPort );
	int GetCount( );

	bool Send( const char * pszIp, int iPort, const char * pszPacket, int iPacketLen );
	bool Send( int iThreadIndex, const char * pszPacket, int iPacketLen );
	bool SendAll( const char * pszPacket, int iPacketLen, ITcpStackCallBack * pclsCallBack );
	bool SendAllExcept( const char * pszPacket, int iPacketLen, ITcpStackCallBack * pclsCallBack, int iThreadIndex, int iSessionIndex );

private:
	void GetKey( const char * pszIp, int iPort, std::string & strKey );
	int GetThreadIndex();
	bool SelectThreadIndex( int iThreadIndex );

	TCP_SESSION_MAP m_clsMap;
	CSipMutex m_clsMutex;
	int m_iThreadIndex;
};

#endif
