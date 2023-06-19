

#ifndef _TCP_STACK_CALLBACK_H_
#define _TCP_STACK_CALLBACK_H_

#include "TcpSessionList.h"


class ITcpStackCallBack
{
public:
	virtual ~ITcpStackCallBack(){};

	
	virtual bool InComingConnected( CTcpSessionInfo * pclsSessionInfo ) = 0;

	
	virtual void SessionClosed( CTcpSessionInfo * pclsSessionInfo ) = 0;

	
	virtual bool RecvPacket( char * pszPacket, int iPacketLen, CTcpSessionInfo * pclsSessionInfo ) = 0;

	
	virtual bool IsSendAll( CTcpSessionInfo * pclsSessionInfo ){ return false; };

	
	virtual void AfterSendAllPerSession( CTcpSessionInfo * pclsSessionInfo, const char * pszPacket, int iPacketLen ){};
};

#endif
