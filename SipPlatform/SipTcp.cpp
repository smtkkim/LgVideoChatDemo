

#include "SipPlatformDefine.h"
#include "SipTcp.h"
#include "MemoryDebug.h"

#ifdef WIN32
#pragma comment( lib, "ws2_32" )
#endif

#ifndef WIN32
#include <ctype.h>



static int ConnectTimeout( int iSockFd, const struct sockaddr *saptr, socklen_t saiLen, int iSecond )
{
	int					iFlags, n, iErrorNum = 0;
	socklen_t		iLen;

	iFlags = fcntl( iSockFd, F_GETFL, 0 );
	fcntl( iSockFd, F_SETFL, iFlags | O_NONBLOCK );

	if( ( n = connect( iSockFd, (struct sockaddr *)saptr, saiLen ) ) < 0 )
	{
		// QQQ : errno 가 17 이 계속 나오면서, 제대로 실행되지 않는 경우가 있었다.
		//     이를 패치하기 위해서 아래와 같이 EEXIST 를 무시하도록 수정하였다.
		// QQQ : errno 가 4 가 나와서 문제가 되는 경우가 있다. 그래서, 이를 패치함.
		if( errno != 0 && errno != EINPROGRESS && errno != EEXIST && errno != EINTR ) return -101;
	}

	if( n == 0 ) goto done;

	pollfd sttPoll[1];

	sttPoll[0].fd = iSockFd;
	sttPoll[0].events = POLLIN | POLLOUT;
	sttPoll[0].revents = 0;

	n = poll( sttPoll, 1, iSecond * 1000 );
	if( n < 0 )
	{
		return -102;
	}
	else if( n == 0 )
	{
		errno = ETIMEDOUT;
		return -102;
	}

	if( sttPoll[0].revents & ( POLLIN | POLLOUT ) )
	{
		iLen = sizeof(iErrorNum);
		if( getsockopt(iSockFd, SOL_SOCKET, SO_ERROR, &iErrorNum, &iLen) < 0 )
			return -103;
	}
	else
	{
		// iErrorNum log	
	}

done:
	fcntl( iSockFd, F_SETFL, iFlags );	// restore

	if( iErrorNum )
	{
		errno = iErrorNum;
		return -104;
	}

	return 0;
}
#endif


bool GetIpByName( const char * szHostName, char * szIp, int iLen )
{
	struct	hostent	* hptr;

	if( (hptr = gethostbyname(szHostName)) == NULL ) return false;
	
#ifdef WINXP
	snprintf( szIp, iLen, "%s", inet_ntoa( *(struct in_addr *)hptr->h_addr_list[0] ));
#else
	inet_ntop( AF_INET, (struct in_addr *)hptr->h_addr_list[0], szIp, iLen );
#endif

	return true;
}


Socket TcpConnect( const char * pszIp, int iPort, int iTimeout )
{
	char		szIp[INET6_ADDRSTRLEN];
	Socket	fd;

	memset( szIp, 0, sizeof(szIp) );
	if( isdigit(pszIp[0]) == 0 )
	{
		// if first character is not digit, suppose it is domain main.
		GetIpByName( pszIp, szIp, sizeof(szIp) );
	}
	else
	{
		snprintf( szIp, sizeof(szIp), "%s", pszIp );
	}

#ifndef WINXP
	if( strstr( szIp, ":" ) )
	{
		struct	sockaddr_in6	addr;		

		// connect server.
		if( ( fd = socket( AF_INET, SOCK_STREAM, 0 )) == INVALID_SOCKET )
		{
			return INVALID_SOCKET;
		}

		addr.sin6_family = AF_INET6;
		addr.sin6_port   = htons(iPort);

		inet_pton( AF_INET6, szIp, &addr.sin6_addr );

#ifndef WIN32
		if( iTimeout > 0 )
		{
			if( ConnectTimeout( fd, (struct sockaddr *)&addr, sizeof(addr), iTimeout ) != 0 )
			{
				closesocket( fd );
				return INVALID_SOCKET;
			}
		}
		else
#endif
		if( connect( fd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR )
		{
			closesocket( fd );
			return INVALID_SOCKET;
		}
	}
	else
#endif
	{
		struct	sockaddr_in	addr;		

		// connect server.
		if( ( fd = socket( AF_INET, SOCK_STREAM, 0 )) == INVALID_SOCKET )
		{
			return INVALID_SOCKET;
		}
	
		addr.sin_family = AF_INET;
		addr.sin_port   = htons(iPort);
	
#ifdef WINXP
		addr.sin_addr.s_addr = inet_addr( szIp );
#else
		inet_pton( AF_INET, szIp, &addr.sin_addr.s_addr );
#endif

#ifndef WIN32
		if( iTimeout > 0 )
		{
			if( ConnectTimeout( fd, (struct sockaddr *)&addr, sizeof(addr), iTimeout ) != 0 )
			{
				closesocket( fd );
				return INVALID_SOCKET;
			}
		}
		else
#endif
		if( connect( fd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR )
		{
			closesocket( fd );
			return INVALID_SOCKET;
		}
	}

	return fd;
}


int TcpSend( Socket fd, const char * szBuf, int iBufLen )
{
	int		n;	
	int		iSendLen = 0;
	
	while( 1 )
	{
		n = send( fd, szBuf + iSendLen, iBufLen - iSendLen, 0 );
		if( n == SOCKET_ERROR ) return SOCKET_ERROR ;
	
		iSendLen += n;
		if( iSendLen == iBufLen ) break;	
	}
	
	return iBufLen;
}


int TcpRecv( Socket fd, char * szBuf, int iBufLen, int iSecond )
{
	int				n;
	pollfd sttPoll[1];

	TcpSetPollIn( sttPoll[0], fd );

	n = poll( sttPoll, 1, 1000 * iSecond );
	if( n <= 0 )
	{
		return SOCKET_ERROR;
	}

	return recv( fd, szBuf, iBufLen, 0 );
}


int TcpRecvSize( Socket fd, char * szBuf, int iBufLen, int iSecond )
{
	int		 n, iRecvLen = 0;
	pollfd sttPoll[1];

	TcpSetPollIn( sttPoll[0], fd );

	while( iRecvLen < iBufLen )
	{
		n = poll( sttPoll, 1, 1000 * iSecond );
		if( n <= 0 )
		{
			return SOCKET_ERROR;
		}

		n = recv( fd, szBuf + iRecvLen, iBufLen - iRecvLen, 0 );
		if( n <= 0 ) return SOCKET_ERROR;

		iRecvLen += n;
	}

	return iRecvLen;
}


Socket TcpListen( int iPort, int iListenQ, const char * pszIp, bool bIpv6 )
{
	Socket	fd;
	const 	int		on = 1;

#ifndef WINXP
	if( bIpv6 )
	{
		struct	sockaddr_in6	addr;

		// create socket.
		if( ( fd = socket( AF_INET6, SOCK_STREAM, 0 )) == INVALID_SOCKET )
		{
			return INVALID_SOCKET;
		}
		
		memset( &addr, 0, sizeof(addr));
		
		addr.sin6_family = AF_INET6;
		addr.sin6_port   = htons(iPort);

		if( pszIp )
		{
			inet_pton( AF_INET6, pszIp, &addr.sin6_addr );
		}
		else
		{
			addr.sin6_addr = in6addr_any;	
		}

		if( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) == -1 )
		{
			
		}

		if( bind( fd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR )
		{
			closesocket( fd );
			return INVALID_SOCKET;
		}
	}
	else
#endif
	{
		struct	sockaddr_in	addr;
	
		// create socket.
		if( ( fd = socket( AF_INET, SOCK_STREAM, 0 )) == INVALID_SOCKET )
		{
			return INVALID_SOCKET;
		}
		
		memset( &addr, 0, sizeof(addr));
		
		addr.sin_family = AF_INET;
		addr.sin_port   = htons(iPort);
	
		if( pszIp )
		{
#ifdef WINXP
			addr.sin_addr.s_addr = inet_addr(pszIp);
#else
			inet_pton( AF_INET, pszIp, &addr.sin_addr.s_addr );
#endif
		}
		else
		{
			addr.sin_addr.s_addr = INADDR_ANY;	
		}
	
		if( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) == -1 )
		{
			
		}
	
		if( bind( fd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR )
		{
			closesocket( fd );
			return INVALID_SOCKET;
		}
	}

	if( listen( fd, iListenQ ) == SOCKET_ERROR )
	{
		closesocket( fd );
		return INVALID_SOCKET;
	}

	return fd;
}


Socket TcpAccept( Socket hListenFd, char * pszIp, int iIpSize, int * piPort, bool bIpv6 )
{
	socklen_t		iAddrLen;
	Socket			hConnFd;

#ifndef WINXP
	if( bIpv6 )
	{
		struct sockaddr_in6 sttAddr;
		iAddrLen = sizeof(sttAddr);
		hConnFd = accept( hListenFd, (struct sockaddr *)&sttAddr, &iAddrLen );
		if( hConnFd != INVALID_SOCKET )
		{
			if( piPort ) *piPort = ntohs( sttAddr.sin6_port );

			if( pszIp && iIpSize > 0 )
			{
				inet_ntop( AF_INET6, &sttAddr.sin6_addr, pszIp, iIpSize );
			}
		}
	}
	else
#endif
	{
		struct sockaddr_in sttAddr;
		iAddrLen = sizeof(sttAddr);
		hConnFd = accept( hListenFd, (struct sockaddr *)&sttAddr, &iAddrLen );
		if( hConnFd != INVALID_SOCKET )
		{
			if( piPort ) *piPort = ntohs( sttAddr.sin_port );
	
			if( pszIp && iIpSize > 0 )
			{
#ifdef WINXP
				snprintf( pszIp, iIpSize, "%s", inet_ntoa( sttAddr.sin_addr ) );
#else
				inet_ntop( AF_INET, &sttAddr.sin_addr, pszIp, iIpSize );
#endif
			}
		}
	}

	return hConnFd;
}


bool GetLocalIpPort( Socket hSocket, std::string & strIp, int & iPort )
{
	if( hSocket == INVALID_SOCKET ) return false;

	struct sockaddr_in sttAddr;
	int iAddrSize = sizeof(sttAddr);
	char szIp[INET6_ADDRSTRLEN];

	if( getsockname( hSocket, (struct sockaddr *)&sttAddr, (socklen_t*)&iAddrSize ) == SOCKET_ERROR ) return false;

#ifdef WINXP
	snprintf( szIp, sizeof(szIp), "%s", inet_ntoa( sttAddr.sin_addr ) );
#else
	inet_ntop( AF_INET, &sttAddr.sin_addr, szIp, sizeof(szIp) );
#endif

	strIp = szIp;
	iPort = ntohs( sttAddr.sin_port );

	return true;
}

#ifdef WIN32


static Socket TcpListenNotReuse( int iPort, int iListenQ, const char * pszIp )
{
	Socket	fd;

	struct	sockaddr_in	addr;

	// create socket.
	if( ( fd = socket( AF_INET, SOCK_STREAM, 0 )) == INVALID_SOCKET )
	{
		return INVALID_SOCKET;
	}
	
	memset( &addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(iPort);

	if( pszIp )
	{
#ifdef WINXP
		addr.sin_addr.s_addr = inet_addr(pszIp);
#else
		inet_pton( AF_INET, pszIp, &addr.sin_addr.s_addr );
#endif
	}
	else
	{
		addr.sin_addr.s_addr = INADDR_ANY;	
	}

	if( bind( fd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR )
	{
		closesocket( fd );
		return INVALID_SOCKET;
	}

	if( listen( fd, iListenQ ) == SOCKET_ERROR )
	{
		closesocket( fd );
		return INVALID_SOCKET;
	}

	return fd;
}

static bool gbIsStartPipeThread = false;
static int giListenPort = 0;


THREAD_API PipeThread( LPVOID lpParameter )
{
	int			iPort;
	char		szBuf[12];
	Socket	hListenFd, hConnFd;

	for( iPort = 1024; iPort < 65535; ++iPort )
	{
		hListenFd = TcpListenNotReuse( iPort, 255, "127.0.0.1" );
		if( hListenFd != INVALID_SOCKET ) break;
	}

	if( hListenFd == INVALID_SOCKET ) return 0;

	giListenPort = iPort;

	while( 1 )
	{
		hConnFd = accept( hListenFd, NULL, NULL );
		if( hConnFd == INVALID_SOCKET ) break;

		memcpy( szBuf, &hConnFd, sizeof(hConnFd) );
		send( hConnFd, szBuf, sizeof(hConnFd), 0 );
	}

	return 0;
}


int pipe( Socket filedes[2] )
{
	if( gbIsStartPipeThread == false )
	{
		DWORD		dwThreadId;
		HANDLE	hThread;

		hThread = CreateThread( NULL, 0, PipeThread, NULL, 0, &dwThreadId );
		if( hThread == NULL )
		{
			return -1;
		}

		gbIsStartPipeThread = true;

		// PipeThread 에서 소켓을 생성할 때까지 대기한다. (1초)
		for( int i = 0; i < 50; ++i )
		{
			if( giListenPort != 0 ) break;
			Sleep( 20 );
		}
	}

	int		n;
	char	szBuf[12];

	if( giListenPort == 0 ) return -1;

	filedes[1] = TcpConnect( "127.0.0.1", giListenPort );
	if( filedes[1] == INVALID_SOCKET ) return -1;

	n = recv( filedes[1], szBuf, sizeof(Socket), 0 );
	if( n != sizeof(Socket) )
	{
		closesocket( filedes[1] );
		return -1;
	}

	memcpy( &filedes[0], szBuf, sizeof(Socket) );

	return 0;
}

#endif
