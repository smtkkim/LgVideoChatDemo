

#include "SipPlatformDefine.h"
#include "FileLogSetup.h"
#include "Log.h"
#include "MemoryDebug.h"


bool SetupFileLog( CFileLog & clsFileLog, CXmlElement & clsElement )
{
	std::string strTemp;

	if( clsElement.SelectElementTrimData( "Folder", strTemp ) )
	{
		if( clsFileLog.Open( strTemp.c_str() ) == false )
		{
			CLog::Print( LOG_ERROR, "%s clsFileLog.Open(%s) error", __FUNCTION__, strTemp.c_str() );
			return false;
		}
	}

	int iLogLevel = 0;

	CXmlElement * pclsClient = clsElement.SelectElement( "Level" );
	if( pclsClient )
	{
		bool bTemp;

		pclsClient->SelectAttribute( "Debug", bTemp );
		if( bTemp ) iLogLevel |= LOG_DEBUG;

		pclsClient->SelectAttribute( "Info", bTemp );
		if( bTemp ) iLogLevel |= LOG_INFO;

		pclsClient->SelectAttribute( "Network", bTemp );
		if( bTemp ) iLogLevel |= LOG_NETWORK;

		pclsClient->SelectAttribute( "Http2", bTemp );
		if( bTemp ) iLogLevel |= LOG_HTTP2;
	}

	int			iLogMaxSize = 0;
	int64_t iLogMaxFolderSize = 0;

	clsElement.SelectElementData( "MaxSize", iLogMaxSize );
	clsElement.SelectElementData( "MaxFolderSize", iLogMaxFolderSize );

	clsFileLog.SetLevel( iLogLevel );
	clsFileLog.SetMaxLogSize( iLogMaxSize );
	clsFileLog.SetMaxFolderSize( iLogMaxFolderSize );

	return true;
}
