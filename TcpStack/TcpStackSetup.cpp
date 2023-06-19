

#include "SipPlatformDefine.h"
#include "TcpStackSetup.h"
#include "Log.h"
#include "MemoryDebug.h"

CTcpStackSetup::CTcpStackSetup() : m_iListenPort(0)
	, m_iThreadInitCount(1), m_iThreadMaxCount(10), m_iMaxSocketPerThread(10)
	, m_iTcpRecvTimeout(3600), m_iTcpNoPacketTimeout(10), m_iTcpConnectTimeout(10)
	, m_bUseTls(false), m_bUseThreadPipe(true), m_bUseHttp2(false), m_bUseHttp2HeaderLog(false)
{
}

CTcpStackSetup::~CTcpStackSetup()
{
}


bool CTcpStackSetup::Parse( CXmlElement & clsXml )
{
	clsXml.SelectElementData( "ListenIp", m_strListenIp );
	clsXml.SelectElementData( "ListenPort", m_iListenPort );
	clsXml.SelectElementData( "ThreadInitCount", m_iThreadInitCount );
	clsXml.SelectElementData( "ThreadMaxCount", m_iThreadMaxCount );
	clsXml.SelectElementData( "MaxSocketPerThread", m_iMaxSocketPerThread );
	clsXml.SelectElementData( "TcpRecvTimeout", m_iTcpRecvTimeout );
	clsXml.SelectElementData( "TcpNoPacketTimeout", m_iTcpNoPacketTimeout );
	clsXml.SelectElementData( "TcpConnectTimeout", m_iTcpConnectTimeout );
	clsXml.SelectElementData( "UseTls", m_bUseTls );
	clsXml.SelectElementTrimData( "CertFile", m_strCertFile );
	clsXml.SelectElementTrimData( "CipherList", m_strCipherList );
	clsXml.SelectElementData( "UseThreadPipe", m_bUseThreadPipe );
	clsXml.SelectElementData( "UseHttp2", m_bUseHttp2 );
	clsXml.SelectElementData( "UseHttp2HeaderLog", m_bUseHttp2HeaderLog );

	if( m_iListenPort > 0 && m_bUseTls && m_strCertFile.empty() )
	{
		CLog::Print( LOG_ERROR, "%s CertFile is not defined", __FUNCTION__ );
		return false;
	}

	return true;
}
