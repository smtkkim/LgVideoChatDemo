

#include "WebRtcServer.h"
#include "Directory.h"
#include "Log.h"
#include "MemoryDebug.h"

CHttpStack gclsStack;

int main( int argc, char * argv[] )
{
	if( argc < 2 )
	{
		printf( "[Usage] %s {Document root path} {RSA key/cert file path}\n", argv[0] );
		return 0;
	}

#ifdef WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	CWebRtcServer clsServer;
	CTcpStackSetup clsSetup;

#ifdef WIN32
	CLog::SetDirectory( "c:\\temp\\http" );
	CLog::SetLevel( LOG_INFO | LOG_DEBUG | LOG_NETWORK );
#endif

	const char * pszDocumentRoot = argv[1];
	const char * pszCertFile = NULL;

	if( argc >= 3 )
	{
		pszCertFile = argv[2];
	}

	// HTTP 수신 포트 번호를 설정한다.
	clsSetup.m_iListenPort = 8080;
	clsSetup.m_iMaxSocketPerThread = 1;
	clsSetup.m_iThreadMaxCount = 0;
	clsSetup.m_bUseThreadPipe = false;
	
	

	if( pszCertFile )
	{
		clsSetup.m_iListenPort = 443;
		clsSetup.m_bUseTls = true;
		clsSetup.m_strCertFile = pszCertFile;
	}

	// HTTP 서버에서 사용할 Document root 폴더를 설정한다.
	clsServer.m_strDocumentRoot = pszDocumentRoot;

	if( CDirectory::IsDirectory( clsServer.m_strDocumentRoot.c_str() ) == false )
	{
		printf( "[%s] is not directory\n", clsServer.m_strDocumentRoot.c_str() );
		return 0;
	}

	// HTTP 서버를 시작한다. HTTP 요청이 수신되면 이에 대한 이벤트를 CSimpleHttpServer 객체로 전달한다.
	if( gclsStack.Start( &clsSetup, &clsServer ) == false )
	{
		printf( "clsStack.Start error\n" );
		return 0;
	}

	while( clsServer.m_bStop == false )
	{
		sleep(1);
	}

	gclsStack.Stop();

	// 모든 쓰레드가 종료될 때까지 대기한다.
	sleep(2);

#ifdef WIN32
	CLog::Release();
#endif

	return 0;
}
