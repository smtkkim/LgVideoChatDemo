/* 
 * Copyright (C) 2012 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */
#include <iostream>
#include <regex>
#include "WebRtcServer.h"
#include "HttpStatusCode.h"
#include "FileUtility.h"
#include "Directory.h"
#include "Log.h"
#include "UserMap.h"
#include "CallMap.h"
#include "MemoryDebug.h"

extern CHttpStack gclsStack;

CWebRtcServer::CWebRtcServer() : m_bStop(false)
{
	m_clsUserDB = new CUserDB();
}

CWebRtcServer::~CWebRtcServer()
{
	delete m_clsUserDB;
}

/**
 * @ingroup TestWebRtc
 * @brief HTTP 요청 수신 이벤트 callback
 * @param pclsRequest		HTTP 요청 메시지
 * @param pclsResponse	HTTP 응답 메시지 - 응용에서 저장한다.
 * @returns 응용에서 HTTP 응답 메시지를 정상적으로 생성하면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CWebRtcServer::RecvHttpRequest( CHttpMessage * pclsRequest, CHttpMessage * pclsResponse )
{
	std::string strPath = m_strDocumentRoot;
	std::string strExt;

	//CLog::Print( LOG_DEBUG, "req uri[%s]", pclsRequest->m_strReqUri.c_str() );

	// 보안상 .. 을 포함한 URL 을 무시한다.
	if( strstr( pclsRequest->m_strReqUri.c_str(), ".." ) )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

#ifdef _DEBUG
	// 메모리 누수 검사를 위해서 exit.html 을 수신하면 프로그램을 종료한다.
	if( !strcmp( pclsRequest->m_strReqUri.c_str(), "/exit.html" ) )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		m_bStop = true;
		return true;
	}
#endif

	if( !strcmp( pclsRequest->m_strReqUri.c_str(), "/" ) )
	{
		CDirectory::AppendName( strPath, "index.html" );
	}
	else
	{
#ifdef WIN32
		ReplaceString( pclsRequest->m_strReqUri, "/", "\\" );
#endif

		strPath.append( pclsRequest->m_strReqUri );
	}

	if( IsExistFile( strPath.c_str() ) == false )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

	// 파일별 Content-Type 을 설정한다.
	GetFileExt( strPath.c_str(), strExt );
	const char * pszExt = strExt.c_str();
	
	if( !strcmp( pszExt, "html" ) || !strcmp( pszExt, "htm" ) )
	{
		pclsResponse->m_strContentType = "text/html";
	}
	else if( !strcmp( pszExt, "css" ) )
	{
		pclsResponse->m_strContentType = "text/css";
	}
	else if( !strcmp( pszExt, "js" ) )
	{
		pclsResponse->m_strContentType = "text/javascript";
	}
	else if( !strcmp( pszExt, "png" ) || !strcmp( pszExt, "gif" ) )
	{
		pclsResponse->m_strContentType = "image/";
		pclsResponse->m_strContentType.append( pszExt );
	}
	else if( !strcmp( pszExt, "jpg" ) || !strcmp( pszExt, "jpeg" ) )
	{
		pclsResponse->m_strContentType = "image/jpeg";
	}
	else
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

	// 파일을 읽어서 HTTP body 에 저장한다.
	FILE * fd = fopen( strPath.c_str(), "rb" );
	if( fd == NULL )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

	int n;
	char szBuf[8192];

	while( ( n = (int)fread( szBuf, 1, sizeof(szBuf), fd ) ) > 0 )
	{
		pclsResponse->m_strBody.append( szBuf, n );
	}

	fclose( fd );

	pclsResponse->m_iStatusCode = HTTP_OK;

	return true;
}

/**
 * @ingroup TestWebRtc
 * @brief WebSocket 클라이언트 TCP 연결 시작 이벤트 callback
 * @param pszClientIp WebSocket 클라이언트 IP 주소
 * @param iClientPort WebSocket 클라이언트 포트 번호
 * @param pclsRequest	HTTP 요청 메시지
 */
void CWebRtcServer::WebSocketConnected( const char * pszClientIp, int iClientPort, CHttpMessage * pclsRequest )
{
	printf( "WebSocket[%s:%d] connected\n", pszClientIp, iClientPort );
}

/**
 * @ingroup TestWebRtc
 * @brief WebSocket 클라이언트 TCP 연결 종료 이벤트 callback
 * @param pszClientIp WebSocket 클라이언트 IP 주소
 * @param iClientPort WebSocket 클라이언트 포트 번호
 */
void CWebRtcServer::WebSocketClosed( const char * pszClientIp, int iClientPort )
{
	printf( "WebSocket[%s:%d] closed\n", pszClientIp, iClientPort );

	std::string strUserId;

	gclsUserMap.Delete( pszClientIp, iClientPort, strUserId );
	gclsCallMap.Delete( strUserId.c_str() );
}

/**
 * @ingroup TestWebRtc
 * @brief WebSocket 클라이언트 데이터 수신 이벤트 callback
 * @param pszClientIp WebSocket 클라이언트 IP 주소
 * @param iClientPort WebSocket 클라이언트 포트 번호
 * @param strData			WebSocket 클라이언트가 전송한 데이터
 * @returns WebSocket 클라이언트 연결을 유지하려면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CWebRtcServer::WebSocketData( const char * pszClientIp, int iClientPort, std::string & strData )
{
	printf( "WebSocket[%s:%d] recv[%s]\n", pszClientIp, iClientPort, strData.c_str() );

	STRING_VECTOR clsList;

	SplitString( strData.c_str(), clsList, '|' );

	int iCount = (int)clsList.size();

	if( iCount < 2 )
	{
		return false;
	}

	bool bReq = true;
	if( strcmp( clsList[0].c_str(), "req" ) ) bReq = false;

	const char * pszCommand = clsList[1].c_str();
	std::string strUserId;

	if (!strcmp(pszCommand, "check"))
	{
		if (iCount < 3)
		{
			printf("register request arg is not correct\n");
			return false;
		}

		std::string unique_id = clsList[2];

		if( unique_id.length() == 0)
		{
			printf("userid has not been entered");
			Send(pszClientIp, iClientPort, "res|check|410");
			return true;
		}

		int CountUserId = m_clsUserDB->CountUserId(unique_id);
		if ( CountUserId == 0 )   // check if id is alread registered
		{
			printf("same unique_id is already exist\n");
			Send(pszClientIp, iClientPort, "res|check|200");
		}
		else if ( CountUserId >= 1 )   // check if id is alread registered
		{
			printf("same unique_id is already exist\n");
			Send(pszClientIp, iClientPort, "res|check|400");
		}
	}
	else if (!strcmp(pszCommand, "register"))
	{
		if (iCount < 8)
		{
			printf("register request arg is not correct\n");
			return false;
		}

		std::string unique_id = clsList[2];
		std::string passwd = clsList[3];
		std::string username = clsList[4];
		std::string email = clsList[5];
		std::string phone = clsList[6];
		std::string address = clsList[7];

		if( unique_id.length() == 0
			|| passwd.length() == 0
			|| username.length() == 0
			|| email.length() == 0)
		{
			printf("can not INSERT the user info to mysql");
			Send(pszClientIp, iClientPort, "res|register|410");
			return true;
		}

		if (m_clsUserDB->CountUserId(unique_id) >= 1 )   // check if id is alread registered
		{
			printf("same unique_id is already exist\n");
			Send(pszClientIp, iClientPort, "res|register|400");
			return true;
		}

    	if (validatePassword(passwd) == false ) 
		{
        	printf("password format is not valid\n");
			Send(pszClientIp, iClientPort, "res|register|420");
			return true;
		}

		uint64_t utc_time;
		getTimeUtc(&utc_time);

		if (m_clsUserDB->RegisterUserId(unique_id, passwd, username, email, phone, address, utc_time) == 0)
		{
			printf("user is correctly registered");
			Send(pszClientIp, iClientPort, "res|register|200");
		}
		else
		{
			printf("can not INSERT the user info to mysql");
			Send(pszClientIp, iClientPort, "res|register|410");
			return true;
		}
	}
	else if( !strcmp( pszCommand, "login" ) )
	{
		if( iCount < 4 )
		{
			printf( "login request arg is not correct\n" );
			return false;
		}

		// passwd check
		std::string user_id = clsList[2];
		std::string db_user_passwd;

		if (m_clsUserDB->CountUserId(user_id) == 1)	// check if id is registered
		{
			uint64_t utc_time_now;
			getTimeUtc(&utc_time_now);

			if (m_clsUserDB->GetWrongPasswdLockTime(user_id) > utc_time_now)	// check if password lock time
			{
				// reject caused password lock time 
				Send(pszClientIp, iClientPort, "res|login|420");

				return true;
			}

			if (m_clsUserDB->GetUserPasswd(user_id, db_user_passwd) == 0)	// get the passwd
			{
				std::string entered_passwd = m_clsUserDB->sha256( m_clsUserDB->saltStr(clsList[3]) );

				if (!strcmp(entered_passwd.c_str(), db_user_passwd.c_str()))
				{
					// passwd OK
					printf("password is correct\n");
					m_clsUserDB->ClearWrongPasswdCnt(user_id);
					m_clsUserDB->UpdateWrongPasswdLockTime(user_id, 0);

                    // check password updated time
                    time_t last_updated_time = m_clsUserDB->GetPasswdUpdatedTime(user_id);
                    if (NeedPasswdUpdate((time_t)utc_time_now, last_updated_time))
                    {
                        printf("Password is outdated and  needs to be changed.\n");
                        Send(pszClientIp, iClientPort, "res|login|430");
                        return true;
                    }
				}
				else
				{
					printf("password is wrong\n");
					m_clsUserDB->IncreaseWrongPasswdCnt(user_id);

					// check if passwd was wrong 3 times
					if (m_clsUserDB->GetWrongPasswdCnt(user_id) >= 3)
					{
						// too many times passwd wrong and update lock time
						m_clsUserDB->UpdateWrongPasswdLockTime(user_id, utc_time_now + 60);
					}

					Send(pszClientIp, iClientPort, "res|login|400");



					return true;
				}
			}
			else
			{
				printf("can not get the passwd from mysql\n");
				Send(pszClientIp, iClientPort, "res|login|410");
				return true;
			}
		}
		else
		{
			printf("Unregisterd user\n");
			Send(pszClientIp, iClientPort, "res|login|300");
			return true;
		}


		if( gclsUserMap.Insert( clsList[2].c_str(), pszClientIp, iClientPort ) == false )
		{
			Send( pszClientIp, iClientPort, "res|login|500" );
		}
		else
		{
			Send( pszClientIp, iClientPort, "res|login|200" );
		}
	}
	else if( !strcmp( pszCommand, "invite" ) )
	{
		if( bReq )
		{
			if( iCount < 4 )
			{
				printf( "invite request arg is not correct\n" );
				return false;
			}

			const char * pszToId = clsList[2].c_str();
			const char * pszSdp = clsList[3].c_str();

			CUserInfo clsToUser;
			std::string strUserId;

			if( gclsUserMap.SelectUserId( pszClientIp, iClientPort, strUserId ) == false )
			{
				Send( pszClientIp, iClientPort, "res|invite|403" );
			}
			else if( gclsUserMap.Select( pszToId, clsToUser ) == false )
			{
				Send( pszClientIp, iClientPort, "res|invite|404" );
			}
			else if( gclsCallMap.Insert( strUserId.c_str(), pszToId ) == false )
			{
				Send( pszClientIp, iClientPort, "res|invite|500" );
			}
			else
			{
				if( Send( clsToUser.m_strIp.c_str(), clsToUser.m_iPort, "req|invite|%s|%s", strUserId.c_str(), pszSdp ) == false )
				{
					Send( pszClientIp, iClientPort, "res|invite|500" );
				}
				else
				{
					Send( pszClientIp, iClientPort, "res|invite|180" );
				}
			}
		}
		else
		{
			if( iCount < 3 )
			{
				printf( "invite response arg is not correct\n" );
				return false;
			}

			int iStatus = atoi( clsList[2].c_str() );

			SendCall( pszClientIp, iClientPort, strData, strUserId );

			if( iStatus > 200 )
			{
				gclsCallMap.Delete( strUserId.c_str() );
			}
		}
	}
	else if (!strcmp(pszCommand, "contact"))
	{
		// send all contact list
		std::string strUserAllId;
		std::string strContactList;
		int iUserCount = gclsUserMap.GetSize();
		int ret = gclsUserMap.GetAllUserId(strUserAllId);

		strContactList = "res|contact|" + std::to_string(iUserCount) + "|" + strUserAllId;

		Send(pszClientIp, iClientPort, strContactList.c_str());
	}
	else if (!strcmp(pszCommand, "userinfo"))
	{
		std::string strUserInfo;
		std::string username; 
		std::string email; 
		std::string phone; 
		std::string address;

		if (iCount < 3)
		{
			printf("userinfo request arg is not correct\n");
			return false;
		}

		m_clsUserDB->GetUserInfo(clsList[2], username, email, phone, address);

		strUserInfo = "res|userinfo|" + clsList[2] + "|" + username + "|" + email + "|" + phone + "|" + address;

		Send(pszClientIp, iClientPort, strUserInfo.c_str());
	}
	else if( !strcmp( pszCommand, "bye" ) )
	{
		SendCall( pszClientIp, iClientPort, strData, strUserId );

		gclsCallMap.Delete( strUserId.c_str() );
	}

	return true;
}

/**
 * @ingroup TestWebRtc
 * @brief WebSocket 클라이언트로 패킷을 전송한다.
 * @param pszClientIp WebSocket 클라이언트 IP 주소
 * @param iClientPort WebSocket 클라이언트 포트 번호
 * @param fmt					전송 문자열
 * @returns 성공하면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CWebRtcServer::Send( const char * pszClientIp, int iClientPort, const char * fmt, ... )
{
	va_list	ap;
	char		szBuf[8192];
	int			iBufLen;

	va_start( ap, fmt );
	iBufLen = vsnprintf( szBuf, sizeof(szBuf)-1, fmt, ap );
	va_end( ap );

	if( gclsStack.SendWebSocketPacket( pszClientIp, iClientPort, szBuf, iBufLen ) )
	{
		printf( "WebSocket[%s:%d] send[%s]\n", pszClientIp, iClientPort, szBuf );
		return true;
	}

	return false;
}

/**
 * @ingroup TestWebRtc
 * @brief 통화 INVITE 응답 및 BYE 요청/응답을 전송한다.
 * @param pszClientIp WebSocket 클라이언트 IP 주소
 * @param iClientPort WebSocket 클라이언트 포트 번호
 * @param strData			전송 패킷
 * @param strUserId		전송된 사용자 아이디
 * @returns 성공하면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CWebRtcServer::SendCall( const char * pszClientIp, int iClientPort, std::string & strData, std::string & strUserId )
{
	std::string strOtherId;
	CUserInfo clsOtherInfo;

	if( gclsUserMap.SelectUserId( pszClientIp, iClientPort, strUserId ) == false )
	{
		printf( "gclsUserMap.SelectUserId(%s:%d) error\n", pszClientIp, iClientPort );
		return false;
	}
	else if( gclsCallMap.Select( strUserId.c_str(), strOtherId ) == false )
	{
		printf( "gclsCallMap.Select(%s) error\n", strUserId.c_str() );
		return false;
	}
	else if( gclsUserMap.Select( strOtherId.c_str(), clsOtherInfo ) == false )
	{
		printf( "gclsUserMap.Select(%s) error\n", strOtherId.c_str() );
		return false;
	}

	return Send( clsOtherInfo.m_strIp.c_str(), clsOtherInfo.m_iPort, "%s", strData.c_str() );
}


//https://www.epochconverter.com/
int CWebRtcServer::getTimeUtc(uint64_t* time_utc)
{
	time_t _time;

	_time = time(&_time);
	*time_utc = (uint64_t)_time;

	return 0;
}

bool CWebRtcServer::NeedPasswdUpdate(time_t last_time, time_t current_time) {
    time_t diff = std::abs(last_time - current_time);

    // 30d * 24h * 60m * 60s
    return diff >= (30 * 24 * 60 * 60);
}

bool CWebRtcServer::validatePassword(const std::string& password) {
    // 최소 10자 이상의 길이를 가지는지 검사
    if (password.length() < 10) {
        return false;
    }

    // 문자, 특수문자, 숫자를 포함하는지 검사
    std::regex letterRegex("[a-zA-Z]");
    std::regex specialCharRegex("[!@#$%^&*-_=+]");
    std::regex digitRegex("[0-9]");

    if (!std::regex_search(password, letterRegex)) {
        return false;
    }

    if (!std::regex_search(password, specialCharRegex)) {
        return false;
    }

    if (!std::regex_search(password, digitRegex)) {
        return false;
    }

    return true;
}